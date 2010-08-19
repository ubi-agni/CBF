#ifndef CBF_FUNCTIONAL_HH
#define CBF_FUNCTIONAL_HH

#include <cbf/sensor_transform.h>
#include <cbf/debug_macros.h>

#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

namespace CBF {

namespace ublas = boost::numeric::ublas;

template<class VectorOperation, class MatrixOperation>
struct ApplyOperationSensorTransform : public SensorTransform {
	VectorOperation m_VectorOperation;
	MatrixOperation m_MatrixOperation;
	SensorTransformPtr m_Operand;

	ApplyOperationSensorTransform(
		SensorTransformPtr operand,
		VectorOperation vector_operation, 
		MatrixOperation matrix_operation
	) :
		m_Operand(operand),
		m_VectorOperation(vector_operation),
		m_MatrixOperation(matrix_operation)
	{ 
		init_results(m_Operand);
	}

	virtual void init_results(SensorTransformPtr operand) {
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
		ApplyOperationSensorTransform(const XMLType& xml_instance) { 
			m_VectorOperation = VectorOperation();
			m_MatrixOperation = MatrixOperation();
			m_Operand = 
				XMLBaseFactory<
					SensorTransform, 
					SensorTransformType
				>::instance()->create(xml_instance.Operand()); 

			init_results(m_Operand);
		}
	#endif

	virtual void update() {
		m_Operand->update();
		m_Result = m_VectorOperation(m_Operand->result());
		m_TaskJacobian = m_MatrixOperation(m_Operand->task_jacobian());
	}

	virtual void set_resource(ResourcePtr res) 
		{ m_Operand->set_resource(res); }

	virtual unsigned int task_dim() const
		{ return m_Operand->task_dim(); }

	virtual unsigned int resource_dim() const
		{ return m_Operand->resource_dim(); }
};

template <class VectorOperation, class MatrixOperation>
ApplyOperationSensorTransform<VectorOperation, MatrixOperation> *
make_ApplyOperationSensorTransform(
		SensorTransformPtr operand,
		VectorOperation vector_operation, 
		MatrixOperation matrix_operation
) {
	return new ApplyOperationSensorTransform<
		VectorOperation,
		MatrixOperation
	> (
		operand,
		vector_operation,
		matrix_operation
	);
}

template<class VectorOperation, class MatrixOperation>
struct ApplyOperationBlockWiseSensorTransform : public SensorTransform {
	VectorOperation m_VectorOperation;
	MatrixOperation m_MatrixOperation;
	SensorTransformPtr m_Operand;
	unsigned int m_Blocksize;

	ApplyOperationBlockWiseSensorTransform(
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
		ApplyOperationBlockWiseSensorTransform(const XMLType& xml_instance) { 
			m_VectorOperation = VectorOperation();
			m_MatrixOperation = MatrixOperation();
			m_Operand = 
				XMLBaseFactory<
					SensorTransform, 
					SensorTransformType
				>::instance()->create(xml_instance.Operand()); 
			m_Blocksize = xml_instance.Blocksize();
			init_results(m_Operand);
		}
	#endif

	virtual void update() {
		m_Operand->update();
		FloatVector tmp_result = m_Operand->result();
		FloatMatrix tmp_jacobian = m_Operand->task_jacobian();
		
		for (unsigned int i = 0, n = m_Operand->task_dim(); i < n; i += m_Blocksize) {
			CBF_DEBUG("iiiii " << i)
			CBF_DEBUG("vector")
			ublas::vector_range<FloatVector> vr(
				m_Result, 
				ublas::range(i, i+m_Blocksize)
			);
			ublas::vector_range<FloatVector> vir(
				tmp_result, 
				ublas::range(i, i+m_Blocksize)
			);
			vr.assign(m_VectorOperation(vir));

			CBF_DEBUG("matrix")
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

	virtual void set_resource(ResourcePtr res) 
		{ m_Operand->set_resource(res); }

	virtual unsigned int task_dim() const
		{ return m_Operand->task_dim(); }

	virtual unsigned int resource_dim() const
		{ return m_Operand->resource_dim(); }
};

template <class VectorOperation, class MatrixOperation>
ApplyOperationBlockWiseSensorTransform<VectorOperation, MatrixOperation> *
make_ApplyOperationBlockWiseSensorTransform(
		SensorTransformPtr operand,
		VectorOperation vector_operation, 
		MatrixOperation matrix_operation,
		unsigned int blocksize
) {
	return new ApplyOperationBlockWiseSensorTransform<
		VectorOperation,
		MatrixOperation
	> (
		operand,
		vector_operation,
		matrix_operation,
		blocksize
	);
}


} // namespace

#endif

