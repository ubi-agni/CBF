#ifndef CBF_FUNCTIONAL_HH
#define CBF_FUNCTIONAL_HH

#include <cbf/sensor_transform.h>
#include <cbf/debug_macros.h>

#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

namespace CBF {
	namespace ublas = boost::numeric::ublas;
	/** 
		A base type for defining Operations on SensorTransforms
	*/
	struct UnarySensorTransformOperation {
		UnarySensorTransformOperation() { }

		virtual FloatVector operator()(const FloatVector &result) = 0;
		virtual FloatMatrix operator()(const FloatMatrix &result) = 0;
	};

	/**
		A derived type that can be used to produce new UnarySensorTransformOperations
		from template functors..

		The only requirement for the two functors is that they provide
		FloatVector operator()(const FloatVector &input)

		and 

		FloatMatrix operator()(const FloatMatrix &input)
	*/
	template <class VectorOperation, class MatrixOperation>
	struct GenericSensorTransformOperation : public UnarySensorTransformOperation {
		virtual FloatVector operator()(const FloatVector &input) 
			{ return VectorOperation()(input); }

		virtual FloatMatrix operator()(const FloatMatrix &input)
			{ return MatrixOperation()(input); }
	};

	

	/**
		If the task space of the sensor transform is built from same sized blocks
		this this class allows to apply a function to the individual blocks
	*/
	template <class VectorOperation, class MatrixOperation>
	struct MapBlockWiseSensorTransformOperation  {
		unsigned int m_Blocksize;

		unsigned int m_TaskDim;
		unsigned int m_ResourceDim;

		MapBlockWiseSensorTransformOperation(
			unsigned int blocksize = 0,
			unsigned int task_dim = 1,
			unsigned int resource_dim = 1
		) : 
			m_Blocksize(blocksize),
			m_TaskDim(task_dim),
			m_ResourceDim(resource_dim) { }

		virtual FloatVector operator()(const FloatVector &input) 
		{
			CBF_DEBUG("Apply Operation on vector")
			FloatVector tmp(input.size());
			for (unsigned int i = 0, rows = input.size(); i < rows; i += m_Blocksize) {
				CBF_DEBUG(i << " " << m_Blocksize)
				ublas::vector_range<FloatVector> vr(tmp, ublas::range(i, i+m_Blocksize));
				ublas::vector_range<const FloatVector> vir(input, ublas::range(i, i+m_Blocksize));
				vr.assign(VectorOperation()(vir));
			}
			CBF_DEBUG("res in: " << input)
			CBF_DEBUG("res out: " << tmp)
			return tmp;
		}

		virtual FloatMatrix operator()(const FloatMatrix &input)
		{
			CBF_DEBUG("Apply Operation on matrix")
			FloatMatrix tmp(input.size1(), input.size2());
			for (unsigned int i = 0, rows = input.size1();  i < rows; i += m_Blocksize) {
				CBF_DEBUG(i << " " << m_Blocksize)
				ublas::matrix_range<FloatMatrix> vr(tmp, ublas::range(i, i+m_Blocksize), ublas::range(0, tmp.size2()));
				ublas::matrix_range<const FloatMatrix> vir(input, ublas::range(i, i+m_Blocksize),  ublas::range(0, tmp.size2()));
				vr.assign(MatrixOperation()(vir));
			}
			CBF_DEBUG("jac in: " << input)
			CBF_DEBUG("jac out: " << tmp)
			return tmp;
		}

		unsigned int task_dim() { return 1; }
	};

	/**
		If the task space of the sensor transform is built from same sized blocks
		this this class allows to apply a function to the individual blocks and 
		accumulate the result. The resulting taskspace has the dimension of
		blocksize..

		The InitialVector and InitialMatrix are the accumulation basis (e.g. the
		zero vector and zero matrix for a summing operation)

		VectorOperation and MatrixOperation need to be binary functions that
		take two vectors (matrices) and return a single vector (matrix)

		The Vector and MatrixOperation take the accumulator as first argument
		and the to be accumulated element as second argument..
	*/
	template <class VectorOperation, class MatrixOperation>
	struct AccumulateBlockWiseSensorTransformOperation  {
		unsigned int m_Blocksize;
		FloatVector m_InitialVector;
		FloatMatrix m_InitialMatrix;

		AccumulateBlockWiseSensorTransformOperation(
			unsigned int blocksize = 0,
			FloatVector initial_vector = FloatVector(),
			FloatMatrix initial_matrix = FloatMatrix()
		) : 
			m_Blocksize(blocksize),
			m_InitialVector(initial_vector),
			m_InitialMatrix(initial_matrix) 
		{ }

		virtual FloatVector operator()(const FloatVector &input) 
		{
			CBF_DEBUG("Apply Operation on vector")
			FloatVector tmp = m_InitialVector;
			for (unsigned int i = 0, rows = input.size(); i < rows; i += m_Blocksize) {
				CBF_DEBUG(i << " " << m_Blocksize)
				ublas::vector_range<const FloatVector> vir(input, ublas::range(i, i+m_Blocksize));
				tmp = VectorOperation(tmp, vir);
			}
			CBF_DEBUG("res in: " << input)
			CBF_DEBUG("res out: " << tmp)
			return tmp;
		}

		virtual FloatMatrix operator()(const FloatMatrix &input)
		{
			CBF_DEBUG("Apply Operation on matrix")
			FloatMatrix tmp = m_InitialMatrix;
			for (unsigned int i = 0, rows = input.size1();  i < rows; i += m_Blocksize) {
				CBF_DEBUG(i << " " << m_Blocksize)
				ublas::matrix_range<const FloatMatrix> vir(input, ublas::range(i, i+m_Blocksize),  ublas::range(0, tmp.size2()));
				tmp = MatrixOperation(tmp, vir);
			}
			CBF_DEBUG("jac in: " << input)
			CBF_DEBUG("jac out: " << tmp)
			return tmp;
		}
	};


	/**
		This class provides a way to create a SensorTransform that 
		performs the Operation each cycle on the operand
	*/
	#ifdef CBF_HAVE_XSD
		template <class Operation, class XMLType>
	#else
		template <class Operation>
	#endif
	struct OperationSensorTransform : public SensorTransform {
		SensorTransformPtr m_Operand;
		Operation m_Operation;

		OperationSensorTransform(Operation &operation, SensorTransformPtr operand = SensorTransformPtr()) 
			: m_Operand(operand), m_Operation(operation) { }

	#ifdef CBF_HAVE_XSD
		OperationSensorTransform(const XMLType &xml_instance) : SensorTransform(xml_instance) { 
 			SensorTransformPtr tr(XMLBaseFactory<SensorTransform, SensorTransformType>::instance()->create(xml_instance.Operand()));
			m_Operand = tr;
			m_Operation = Operation(xml_instance.Blocksize());
		}
	#endif

		virtual void set_resource(ResourcePtr res) {
			m_Operand->set_resource(res);
		}

		virtual void update() {
			m_Operand->update();
			CBF_DEBUG("Applying operation on vector")
			m_Result = m_Operation(m_Operand->result());

			CBF_DEBUG("Applying operation on matrix")
			m_TaskJacobian = m_Operation(m_Operand->task_jacobian());
		}

		virtual unsigned int task_dim() const { return m_Operand->task_dim(); }
		virtual unsigned int resource_dim() const { return m_Operand->resource_dim(); }
	};


} // namespace

#endif

