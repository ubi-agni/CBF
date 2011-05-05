#ifndef CBF_FUNCTIONAL_HH
#define CBF_FUNCTIONAL_HH

#include <cbf/config.h>
#include <cbf/sensor_transform.h>
#include <cbf/debug_macros.h>

#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

#include <cbf/xml_object_factory.h>
#include <cbf/utilities.h>

#ifdef CBF_HAVE_XSD
	#include <cbf/schemas.hxx>
#endif

namespace CBF {

namespace ublas = boost::numeric::ublas;


/**
	@brief A generic SensorTransform that allows to be parametrized
	with two functors, one acting on the result of the 
	operand and one that acts on the jacobian matrix of the 
	operand.

	Note that the task and resource space dimensions are the same
	as those of the operand. See NegateOperationSensorTransform
	and MultiplyOperation examples..
*/
template<class VectorOperation, class MatrixOperation>
struct ApplySensorTransform : public SensorTransform {
	VectorOperation m_VectorOperation;
	MatrixOperation m_MatrixOperation;
	SensorTransformPtr m_Operand;

	ApplySensorTransform(
		SensorTransformPtr operand,
		VectorOperation vector_operation, 
		MatrixOperation matrix_operation
	) :
		m_Operand(operand),
		m_VectorOperation(vector_operation),
		m_MatrixOperation(matrix_operation)
	{ 
		init(m_Operand);
	}

	virtual void init(SensorTransformPtr operand) {
		CBF_DEBUG("task space dim: " << operand->task_dim() << "  resource dim: " << operand->resource_dim());
		m_Result = FloatVector(operand->task_dim()); 
		m_TaskJacobian = FloatMatrix(operand->task_dim(), operand->resource_dim());
	}

	#ifdef CBF_HAVE_XSD
		/**
			This constructor needs to be specialized per registered 
			type with corresponding XMLType. See e.g.

			NegateOperationSensorTransform
		*/
		template <class XMLType>
		ApplySensorTransform(const XMLType& xml_instance, ObjectNamespacePtr object_namespace) :
			SensorTransform(xml_instance, object_namespace)
		{ 
			m_VectorOperation = VectorOperation();
			m_MatrixOperation = MatrixOperation();
			m_Operand = 
				XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.Operand(), object_namespace); 

			init(m_Operand);
		}
	#endif

	virtual void update(const FloatVector &resource_value) {
		CBF_DEBUG("update");
		m_Operand->update(resource_value);
		m_Result = m_VectorOperation(m_Operand->result());
		m_TaskJacobian = m_MatrixOperation(m_Operand->task_jacobian());

		CBF_DEBUG("result " << m_Result);
		CBF_DEBUG("jac    " << m_TaskJacobian);
	}

	virtual unsigned int task_dim() const
		{ return m_Operand->task_dim(); }

	virtual unsigned int resource_dim() const
		{ return m_Operand->resource_dim(); }
};

template <class VectorOperation, class MatrixOperation>
ApplySensorTransform<VectorOperation, MatrixOperation> *
make_ApplySensorTransform(
		SensorTransformPtr operand,
		VectorOperation vector_operation, 
		MatrixOperation matrix_operation
) {
	return new ApplySensorTransform<
		VectorOperation,
		MatrixOperation
	> (
		operand,
		vector_operation,
		matrix_operation
	);
}

/**
	A SensorTransform that applies the operations to row wise blocks
	of its operand's output. This is useful e.g. if the task space 
	of a SensorTransform is made up of blocks of equal size. An example
	would be a KDLTreePositionSensorTransform with more than one
	controlled end-effector position
*/
template<class VectorOperation, class MatrixOperation>
struct BlockWiseApplySensorTransform : public SensorTransform {
	VectorOperation m_VectorOperation;
	MatrixOperation m_MatrixOperation;
	SensorTransformPtr m_Operand;
	unsigned int m_Blocksize;

	BlockWiseApplySensorTransform(
		SensorTransformPtr operand,
		VectorOperation vector_operation, 
		MatrixOperation matrix_operation,
		unsigned int blocksize
	) :
		m_Operand(operand),
		m_VectorOperation(vector_operation),
		m_MatrixOperation(matrix_operation),
		m_Blocksize(blocksize)
	{ 
		init_results(m_Operand);
	}

	virtual void init_results(SensorTransformPtr operand) {
		m_Result = FloatVector(operand->task_dim()); 
		m_TaskJacobian = FloatMatrix(operand->task_dim(), operand->resource_dim());
	}

	#ifdef CBF_HAVE_XSD
		template <class XMLType>
		BlockWiseApplySensorTransform(const XMLType& xml_instance, ObjectNamespacePtr object_namespace) :
			SensorTransform(xml_instance, object_namespace)
		{ 
			m_VectorOperation = VectorOperation();
			m_MatrixOperation = MatrixOperation();

			m_Operand = 
				XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.Operand(), object_namespace); 

			m_Blocksize = xml_instance.Blocksize();

			init_results(m_Operand);
		}
	#endif

	virtual void update(const FloatVector &resource_value) {
		m_Operand->update(resource_value);
		FloatVector tmp_result = m_Operand->result();
		FloatMatrix tmp_jacobian = m_Operand->task_jacobian();
		
		for (unsigned int i = 0, n = m_Operand->task_dim(); i < n; i += m_Blocksize) {
			CBF_DEBUG("i " << i);
			CBF_DEBUG("vector");
			ublas::vector_range<FloatVector> vr(
				m_Result, 
				ublas::range(i, i+m_Blocksize)
			);
			ublas::vector_range<FloatVector> vir(
				tmp_result, 
				ublas::range(i, i+m_Blocksize)
			);
			vr.assign(m_VectorOperation(vir));

			CBF_DEBUG("matrix");
			ublas::matrix_range<FloatMatrix> mr(
				m_TaskJacobian, 
				ublas::range(i, i+m_Blocksize), 
				ublas::range(0, tmp_jacobian.size2())
			);
			ublas::matrix_range<FloatMatrix> mir(
				tmp_jacobian, 
				ublas::range(i, i+m_Blocksize),
				ublas::range(0, tmp_jacobian.size2())
			);
			mr.assign(m_MatrixOperation(mir));
		}
	}

	virtual unsigned int task_dim() const
		{ return m_Operand->task_dim(); }

	virtual unsigned int resource_dim() const
		{ return m_Operand->resource_dim(); }
};

/**
	@brief A function to create a BlockWiseApplySensorTransform
*/
template <class VectorOperation, class MatrixOperation>
BlockWiseApplySensorTransform<VectorOperation, MatrixOperation> *
make_BlockWiseApplySensorTransform(
	SensorTransformPtr operand,
	VectorOperation vector_operation, 
	MatrixOperation matrix_operation,
	unsigned int blocksize
) {
	return new BlockWiseApplySensorTransform<
		VectorOperation,
		MatrixOperation
	> (
		operand,
		vector_operation,
		matrix_operation,
		blocksize
	);
}


/**
	A SensorTransform that applies the operations to row wise blocks
	of its operands' output. This is useful e.g. if the task space 
	of a SensorTransform is made up of blocks of equal size. An example
	would be a KDLTreePositionSensorTransform with more than one
	controlled end effector position

	In this case the Vector and MatrixOperation need to be 
	binary functions as they take one block from each operand
	a time..
*/
template<class VectorOperation, class MatrixOperation>
struct BlockWiseInnerProductSensorTransform : public SensorTransform {
	VectorOperation m_VectorOperation;
	MatrixOperation m_MatrixOperation;
	SensorTransformPtr m_Operand1;
	SensorTransformPtr m_Operand2;
	unsigned int m_Blocksize;
	unsigned int m_TaskDim;

	BlockWiseInnerProductSensorTransform(
		SensorTransformPtr operand1,
		SensorTransformPtr operand2,
		VectorOperation vector_operation, 
		MatrixOperation matrix_operation,
		unsigned int blocksize,
		unsigned int task_dim
	) :
		m_Operand1(operand1),
		m_Operand2(operand2),
		m_VectorOperation(vector_operation),
		m_MatrixOperation(matrix_operation),
		m_Blocksize(blocksize),
		m_TaskDim(task_dim)
	{ 
		init_results(m_Operand1);
	}

	virtual void init_results(SensorTransformPtr operand) {
		m_Result = FloatVector(operand->task_dim()); 
		m_TaskJacobian = FloatMatrix(operand->task_dim(), operand->resource_dim());
	}

	#ifdef CBF_HAVE_XSD
		template <class XMLType>
		BlockWiseInnerProductSensorTransform(const XMLType& xml_instance) :
			SensorTransform(xml_instance)
		{ 
			m_VectorOperation = VectorOperation();
			m_MatrixOperation = MatrixOperation();
			m_Operand1 = 
				XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.Operand()); 
			m_Operand2 = 
				XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.Operand2()); 
			m_Blocksize = xml_instance.Blocksize();
			init_results(m_Operand1);
		}
	#endif

	virtual void update(const FloatVector &resource_value) {
		m_Operand1->update(resource_value);
		m_Operand2->update(resource_value);
		FloatVector tmp_result1 = m_Operand1->result();
		FloatMatrix tmp_jacobian1 = m_Operand1->task_jacobian();
		FloatVector tmp_result2 = m_Operand2->result();
		FloatMatrix tmp_jacobian2 = m_Operand2->task_jacobian();
		
		for (unsigned int i = 0, n = m_Operand1->task_dim(); i < n; i += m_Blocksize) {
			CBF_DEBUG("i " << i);
			CBF_DEBUG("vector");
			ublas::vector_range<FloatVector> vr(
				m_Result, 
				ublas::range(i, i+m_Blocksize)
			);
			ublas::vector_range<FloatVector> vir1(
				tmp_result1, 
				ublas::range(i, i+m_Blocksize)
			);
			ublas::vector_range<FloatVector> vir2(
				tmp_result2, 
				ublas::range(i, i+m_Blocksize)
			);
			vr.assign(m_VectorOperation(vir1, vir2));

			CBF_DEBUG("matrix");
			ublas::matrix_range<FloatMatrix> mr(
				m_TaskJacobian, 
				ublas::range(i, i+m_Blocksize), 
				ublas::range(0, tmp_jacobian1.size2())
			);
			ublas::matrix_range<FloatMatrix> mir1(
				tmp_jacobian1, 
				ublas::range(i, i+m_Blocksize),
				ublas::range(0, tmp_jacobian1.size2())
			);
			ublas::matrix_range<FloatMatrix> mir2(
				tmp_jacobian2, 
				ublas::range(i, i+m_Blocksize),
				ublas::range(0, tmp_jacobian2.size2())
			);
			mr.assign(m_MatrixOperation(mir1, mir2));
		}
	}


	virtual unsigned int task_dim() const
		{ return m_TaskDim; }

	virtual unsigned int resource_dim() const
		{ return m_Operand1->resource_dim(); }
};

template <class VectorOperation, class MatrixOperation>
BlockWiseInnerProductSensorTransform<VectorOperation, MatrixOperation> *
make_BlockWiseInnerProductSensorTransform(
		SensorTransformPtr operand1,
		SensorTransformPtr operand2,
		VectorOperation vector_operation, 
		MatrixOperation matrix_operation,
		unsigned int blocksize,
		unsigned int task_dim
) {
	return new BlockWiseInnerProductSensorTransform<
		VectorOperation,
		MatrixOperation
	> (
		operand1,
		operand2,
		vector_operation,
		matrix_operation,
		blocksize,
		task_dim
	);
}



/**
	@brief Accumulate blocks of size block_size with a VectorOperation and a MatrixOperation
*/
template<class VectorOperation, class MatrixOperation>
struct BlockWiseAccumulateSensorTransform : public SensorTransform {
	VectorOperation m_VectorOperation;
	MatrixOperation m_MatrixOperation;
	SensorTransformPtr m_Operand;
	unsigned int m_Blocksize;
	FloatVector m_InitVector;
	FloatMatrix m_InitMatrix;

	BlockWiseAccumulateSensorTransform(
		SensorTransformPtr operand,
		VectorOperation vector_operation, 
		MatrixOperation matrix_operation,
		const FloatVector &init_vector,
		const FloatMatrix &init_matrix,
		unsigned int blocksize
	) :
		m_Operand(operand),
		m_VectorOperation(vector_operation),
		m_MatrixOperation(matrix_operation),
		m_Blocksize(blocksize),
		m_InitVector(init_vector),
		m_InitMatrix(init_matrix)
	{ 
		init_results(m_Operand);
	}

	virtual void init_results(SensorTransformPtr operand) {
		m_Result = FloatVector(m_Blocksize); 

		m_TaskJacobian = FloatMatrix(
			m_Blocksize, 
			operand->resource_dim()
		);
	}

	#ifdef CBF_HAVE_XSD
		BlockWiseAccumulateSensorTransform(
			const CBFSchema::BlockWiseAccumulateSensorTransform& xml_instance, ObjectNamespacePtr object_namespace
		) : 
			SensorTransform(xml_instance, object_namespace) 
		{ 
			m_VectorOperation = VectorOperation();
			m_MatrixOperation = MatrixOperation();

			m_Operand = 
				XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.Operand(), object_namespace); 

			m_Blocksize = xml_instance.Blocksize();
			m_InitVector = *XMLFactory<FloatVector>::instance()->create(xml_instance.InitVector(), object_namespace);
			m_InitMatrix = create_matrix(xml_instance.InitMatrix(), object_namespace);

			init_results(m_Operand);
		}

	#endif

	virtual void update(const FloatVector &resource_value) {
		m_Operand->update(resource_value);
		FloatVector tmp_result = m_Operand->result();
		FloatMatrix tmp_jacobian = m_Operand->task_jacobian();

		m_Result = m_InitVector;
		m_TaskJacobian = m_InitMatrix;
		
		for (unsigned int i = 0, n = m_Operand->task_dim(); i < n; i += m_Blocksize) {
			CBF_DEBUG("i " << i);
			CBF_DEBUG("vector");

			ublas::vector_range<FloatVector> vir(
				tmp_result, 
				ublas::range(i, i+m_Blocksize)
			);
			m_Result.assign(m_VectorOperation(m_Result, vir));

			CBF_DEBUG("matrix");

			ublas::matrix_range<FloatMatrix> mir(
				tmp_jacobian, 
				ublas::range(i, i+m_Blocksize),
				ublas::range(0, tmp_jacobian.size2())
			);
			m_TaskJacobian.assign(m_MatrixOperation(m_TaskJacobian, mir));
		}
	}


	virtual unsigned int task_dim() const
		{ return m_Blocksize; }

	virtual unsigned int resource_dim() const
		{ return m_Operand->resource_dim(); }
};


template <class VectorOperation, class MatrixOperation>
BlockWiseAccumulateSensorTransform<VectorOperation, MatrixOperation> *
make_BlockWiseAccumulateSensorTransform(
		SensorTransformPtr operand,
		VectorOperation vector_operation, 
		MatrixOperation matrix_operation,
		const FloatVector &init_vector,
		const FloatMatrix &init_matrix,
		unsigned int block_size
) {
	return new BlockWiseAccumulateSensorTransform<
		VectorOperation,
		MatrixOperation
	> (
		operand,
		vector_operation,
		matrix_operation,
		init_vector,
		init_matrix,
		block_size
	);
}


/**
	@brief A template that allows the construction of a SensorTransform from two functors
*/
template<class VectorOperation, class MatrixOperation>
struct GenericSensorTransform : public SensorTransform {
	GenericSensorTransform(
		const VectorOperation &vector_operation,
		const MatrixOperation &matrix_operation,
		unsigned int task_dim,
		unsigned int resource_dim
	) :
		m_VectorOperation(vector_operation),
		m_MatrixOperation(matrix_operation)
	{
		init(task_dim, resource_dim);
	}

	template<class XMLType>
	GenericSensorTransform() { }

	void init(unsigned int task_dim, unsigned int resource_dim) {
		m_Result = FloatVector(task_dim);
		m_TaskJacobian = FloatMatrix(task_dim, resource_dim);
	}

	virtual void update(const FloatVector &resource_value) {
		m_Result = m_VectorOperation(resource_value);
		m_TaskJacobian = m_MatrixOperation(resource_value);
	}	

	protected:
		VectorOperation m_VectorOperation;
		MatrixOperation m_MatrixOperation;
};


/**
	A simple functor that implements a multiplication operation
	where first and second result types may vary, but the result
	is of the first type..
*/
template<typename T, typename U> 
struct multiplies { 
	typedef T first_argument_type;
	typedef U second_argument_type;
	typedef T result_type;
	T operator()(T const& t, U const& u) const { return t * u; } 
};


template<class T, class U>
struct constant {
	constant(const U& value) : m_Value(value) { }
	U m_Value;
	U operator()(const T& in) {
		return m_Value;
	}
};

typedef ApplySensorTransform<
	std::negate<FloatVector>,
	std::negate<FloatMatrix>
> NegateOperationSensorTransform;

typedef ApplySensorTransform<
	std::binder2nd<multiplies<FloatVector, double> >,
	std::binder2nd<multiplies<FloatMatrix, double> >
> MultiplyOperationSensorTransform;

#if 0
typedef BlockWiseInnerProductSensorTransform<
	multiplies<FloatVector, double>,
	multiplies<FloatMatrix, double>
> BlockWiseWeightedSumSensorTransform;
#endif

typedef BlockWiseAccumulateSensorTransform<
	std::plus<FloatVector>,
	std::plus<FloatMatrix>
> BlockWiseSumSensorTransform;

typedef BlockWiseAccumulateSensorTransform<
	std::minus<FloatVector>,
	std::minus<FloatMatrix>
> BlockWiseDifferenceSensorTransform;

} // namespace

#endif

