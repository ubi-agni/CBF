#include <cbf/functional.h>

namespace CBF {

typedef ApplyOperationSensorTransform<
	std::negate<FloatVector>,
	std::negate<FloatMatrix>
> NegateOperationSensorTransform;

#ifdef CBF_HAVE_XSD
#if 0
	template<> template<> ApplyOperationSensorTransform<
		std::negate<FloatVector>,
		std::negate<FloatMatrix>
	>
	::ApplyOperationSensorTransform
		(const NegateOperationSensorTransformType &xml_instance) 
		{ CBF_DEBUG("MWAHAHHAHAHAHHAHHH!!!"); }
#endif

	static XMLDerivedFactory<
		NegateOperationSensorTransform,
		NegateOperationSensorTransformType, 
		SensorTransform, 
		SensorTransformType
	> x;
#endif

} // namespace


