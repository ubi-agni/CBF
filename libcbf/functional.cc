#include <cbf/functional.h>
#include <cbf/xml_factory.h>

namespace CBF {


#ifdef CBF_HAVE_XSD
	template<> template<> ApplyOperationSensorTransform<
		std::binder2nd<multiplies<FloatVector, double> >,
		std::binder2nd<multiplies<FloatMatrix, double> >
	>
	::ApplyOperationSensorTransform
		(const CBFSchema::MultiplyOperationSensorTransform &xml_instance) :
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
				SensorTransform, CBFSchema::SensorTransform
			>::instance()->create(xml_instance.Operand());
		}

	static XMLDerivedFactory<
		NegateOperationSensorTransform,
		CBFSchema::NegateOperationSensorTransform, 
		SensorTransform, 
		CBFSchema::SensorTransform
	> x1;

	static XMLDerivedFactory<
		MultiplyOperationSensorTransform,
		CBFSchema::MultiplyOperationSensorTransform, 
		SensorTransform, 
		CBFSchema::SensorTransform
	> x2;
#endif

} // namespace


