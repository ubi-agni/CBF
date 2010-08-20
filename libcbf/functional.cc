#include <cbf/functional.h>

namespace CBF {


#ifdef CBF_HAVE_XSD

	template<> template<> ApplyOperationSensorTransform<
		std::binder2nd<multiplies<FloatVector, double> >,
		std::binder2nd<multiplies<FloatMatrix, double> >
	>
	::ApplyOperationSensorTransform
		(const MultiplyOperationSensorTransformType &xml_instance) :
		m_VectorOperation(
			std::bind2nd(
				multiplies<FloatVector, double>(),
				xml_instance.Factor()
			)
		),
		m_MatrixOperation(
			std::bind2nd(
				multiplies<FloatMatrix, double>(),
				xml_instance.Factor()
			)
		) { 
			m_Operand = XMLBaseFactory<
				SensorTransform, SensorTransformType
			>::instance()->create(xml_instance.Operand());
		}






	static XMLDerivedFactory<
		NegateOperationSensorTransform,
		NegateOperationSensorTransformType, 
		SensorTransform, 
		SensorTransformType
	> x1;

	static XMLDerivedFactory<
		MultiplyOperationSensorTransform,
		MultiplyOperationSensorTransformType, 
		SensorTransform, 
		SensorTransformType
	> x2;

#endif

} // namespace


