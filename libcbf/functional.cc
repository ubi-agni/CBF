#include <cbf/functional.h>

namespace CBF {

typedef MapBlockWiseSensorTransformOperation<
	std::negate<FloatVector>, 
	std::negate<FloatMatrix> 
> NegateBlockWiseSensorTransformOperation;


typedef OperationSensorTransform<
	NegateBlockWiseSensorTransformOperation
> NegateOperationSensorTransform;

#ifdef CBF_HAVE_XSD
	template<> template<> OperationSensorTransform
		<NegateBlockWiseSensorTransformOperation>
	::OperationSensorTransform
		(const NegateOperationSensorTransformType &xml_instance) 
		{ CBF_DEBUG("MWAHAHHAHAHAHHAHHH!!!"); }
#endif


#ifdef CBF_HAVE_XSD
	static XMLDerivedFactory<
		NegateOperationSensorTransform, 
		NegateOperationSensorTransformType, 
		SensorTransform, 
		SensorTransformType
	> x;
#endif

} // namespace


