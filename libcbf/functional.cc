#include <cbf/functional.h>

namespace CBF {

typedef MapBlockWiseSensorTransformOperation<
	std::negate<FloatVector>, 
	std::negate<FloatMatrix> 
> NegateBlockWiseSensorTransformOperation;


typedef OperationSensorTransform<
	NegateBlockWiseSensorTransformOperation,
	NegateOperationSensorTransformType
> NegateOperationSensorTransform;

#ifdef CBF_HAVE_XSD
template <> OperationSensorTransform<
	NegateBlockWiseSensorTransformOperation,
	NegateOperationSensorTransformType
>::OperationSensorTransform(const NegateOperationSensorTransformType &xml_instance) {
	CBF_DEBUG("MWAHAHHAHAHAHHAHHH!!!")
}
#endif



static XMLDerivedFactory<
	NegateOperationSensorTransform, 
	NegateOperationSensorTransformType, 
	SensorTransform, 
	SensorTransformType
> x;

} // namespace


