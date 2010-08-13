#include <cbf/functional.h>
namespace CBF {

typedef MapGenericBlockWiseSensorTransformOperation<
	std::negate<FloatVector>, std::negate<FloatMatrix> 
> neg;


typedef OperationSensorTransform<
	neg
> negneg;


// static XMLDerivedFactory<negneg, CompositeSensorTransformType, SensorTransform, SensorTransformType> x;

} // namespace


