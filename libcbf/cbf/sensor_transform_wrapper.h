#ifndef CBF_SENSOR_TRANSFORM_WRAPPER_HH
#define CBF_SENSOR_TRANSFORM_WRAPPER_HH

#include <vector>

#include <cbf/sensor_transform.h>

namespace CBF {

	/** @brief 
			This SensorTransform can be used to select a number
			of rows of a jacobian
	*/
	struct RowSelector : public SensorTransform {
		/** The SensorTransform we are wrapping */
		SensorTransformPtr m_SensorTransform;

		/** The rows we want to keep */
		std::vector<int> m_Rows;

		std::vector<int> &rows() { return m_Rows; }

		void set_rows(const std::vector<int> &rows) { m_Rows = rows; }

		virtual void read() {
			m_SensorTransform->read();
		}
	};

} // namespace

#endif
