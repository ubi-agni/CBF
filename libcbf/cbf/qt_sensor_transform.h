#ifndef CBF_QT_SENSOR_TRANSFORM_HH
#define CBF_QT_SENSOR_TRANSFORM_HH

#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QCheckBox>

#include <vector>

#include <cbf/config.h>
#include <cbf/sensor_transform.h>

namespace CBFSchema {
	class QtSensorTransform;
}

namespace CBF {

	/** @brief A SensorTransform wrapper that creates a window to display the current task space position */
	struct QtSensorTransform : public SensorTransform {
		QtSensorTransform(SensorTransformPtr operand) { init(operand); }

		void init(SensorTransformPtr operand) {
			m_Operand = operand;

			QGridLayout *layout = new QGridLayout;
			m_Widget.setLayout(layout);

			for (unsigned int i = 0; i < m_Operand->task_dim(); ++i) {
				QLabel *label = new QLabel;
				layout->addWidget(label, i, 1);
				m_Labels.push_back(label);

				QLabel *name = new QLabel(component_name(i).c_str());
				layout->addWidget(name, i, 0);
			}
			layout->setColumnStretch(1,10);

			m_Widget.show();
		}

		QtSensorTransform(const CBFSchema::QtSensorTransform &xml_instance);

		virtual void update(const FloatVector &resource_value) {
			m_Operand->update(resource_value);

			for (unsigned int i = 0; i < m_Labels.size(); ++i) {
				m_Labels[i]->setNum(m_Operand->result()[i]);
			}
		}

		virtual unsigned int task_dim() const { 
			return m_Operand->task_dim();
		}

		virtual unsigned int resource_dim() const { 
			return m_Operand->resource_dim();
		}

		virtual const FloatMatrix &task_jacobian() const {
			return m_Operand->task_jacobian();
		}

		virtual const FloatVector &result() const {
			return m_Operand->result();
		}
	
		SensorTransformPtr m_Operand;
		QWidget m_Widget;
		std::vector<QLabel*> m_Labels;
	};

} // namespace

#endif
