#include <cbf/functional.h>
#include <cbf/xml_factories.h>

namespace CBF {


#ifdef CBF_HAVE_XSD
	template<> template<> ApplyOperationSensorTransform<
		std::binder2nd<multiplies<FloatVector, double> >,
		std::binder2nd<multiplies<FloatMatrix, double> >
	>
	::ApplyOperationSensorTransform
		(const ::MultiplyOperationSensorTransform &xml_instance) :
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
		::NegateOperationSensorTransform, 
		SensorTransform, 
		::SensorTransform
	> x1;

	static XMLDerivedFactory<
		MultiplyOperationSensorTransform,
		::MultiplyOperationSensorTransform, 
		SensorTransform, 
		::SensorTransform
	> x2;
#endif

} // namespace


