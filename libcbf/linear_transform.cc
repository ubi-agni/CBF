/* -*- mode: c-non-suck; -*- */

#include <cbf/linear_transform.h>
#include <cbf/plugin_macros.h>

namespace CBF {

	#ifdef CBF_HAVE_XSD
		LinearSensorTransform::LinearSensorTransform(const LinearSensorTransformType &xml_instance) 
		{
			CBF_DEBUG("[LinearSensorTransform(const LinearSensorTransformType &xml_instance)]: yay!!!");
			const MatrixType *m = &xml_instance.Matrix();
		
			const BoostMatrixType *m2 = 
				dynamic_cast<const BoostMatrixType*>(m);

			if (m2) {
				std::stringstream stream(std::string(m2->String()));
				stream >> m_Matrix;
				CBF_DEBUG(m_Matrix)
				if ((m_Matrix.size1() == 0) && (m_Matrix.size2() == 0)) 
					throw std::runtime_error
						("[LinearSensorTransform]: Matrix is empty");
			}
		}
	#endif
		
	CBF_PLUGIN_CLASS(LinearSensorTransform, SensorTransform)
} // namespace
