#include <cbf/functional.h>

namespace CBF {

typedef MapGenericBlockWiseSensorTransformOperation<
	std::negate<FloatVector>, std::negate<FloatMatrix> 
> neg;


typedef OperationSensorTransform<
	neg,
	NegateOperationSensorTransformType
> negneg;


static XMLDerivedFactory<
	negneg, NegateOperationSensorTransformType, SensorTransform, SensorTransformType
> x;

} // namespace


