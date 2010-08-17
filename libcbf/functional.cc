#include <cbf/functional.h>

namespace CBF {

typedef MapGenericBlockWiseSensorTransformOperation<
	std::negate<FloatVector>, std::negate<FloatMatrix> 
> NegateBlockWiseSensorTransformOperation;


typedef OperationSensorTransform<
	NegateBlockWiseSensorTransformOperation,
	NegateOperationSensorTransformType
> NegateOperationSensorTransform;


static XMLDerivedFactory<
	NegateOperationSensorTransform, 
	NegateOperationSensorTransformType, 
	SensorTransform, 
	SensorTransformType
> x;

} // namespace


