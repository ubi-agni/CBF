#include <cbf/functional.h>
#include <cbf/xml_factory.h>

namespace CBF {


#ifdef CBF_HAVE_XSD
	template<> template<> ApplySensorTransform<
		multiply<FloatVector, double>,
		multiply<FloatMatrix, double>
	>::ApplySensorTransform(
			const CBFSchema::MultiplyOperationSensorTransform &xml_instance, ObjectNamespacePtr object_namespace
	) :
		m_VectorOperation(multiply<FloatVector, double>(xml_instance.Factor())),
		m_MatrixOperation(multiply<FloatMatrix, double>(xml_instance.Factor()))
	{ 
		CBF_DEBUG("MultiplyOperationSensorTransform");
		m_Operand = XMLObjectFactory::instance()->create<SensorTransform>(xml_instance.Operand(), object_namespace);
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

	static XMLDerivedFactory<
		BlockWiseDifferenceSensorTransform,
		CBFSchema::BlockWiseDifferenceSensorTransform
	> x4;

#endif

} // namespace
