#include <cbf/functional.h>
#include <cbf/xml_factory.h>

namespace CBF {


#ifdef CBF_HAVE_XSD
	template<> template<> ApplySensorTransform<
		std::binder2nd<multiplies<FloatVector, double> >,
		std::binder2nd<multiplies<FloatMatrix, double> >
	>::ApplySensorTransform(
			const CBFSchema::MultiplyOperationSensorTransform &xml_instance
	) :
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
		) 
	{ 
		CBF_DEBUG("MultiplyOperationSensorTransform")
		m_Operand = XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.Operand());
	}

	static XMLDerivedFactory<
		NegateOperationSensorTransform,
		CBFSchema::NegateOperationSensorTransform
	> x1;

	static XMLDerivedFactory<
		MultiplyOperationSensorTransform,
		CBFSchema::MultiplyOperationSensorTransform
	> x2;

	static XMLDerivedFactory<
		BlockWiseSumSensorTransform,
		CBFSchema::BlockWiseSumSensorTransform
	> x3;

#endif

} // namespace


