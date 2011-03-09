#ifndef CBF_QT_REFERENCE_HH
#define CBF_QT_REFERENCE_HH

#include <cbf/config.h>
#include <cbf/reference.h>
#include <cbf/types.h>
#include <cbf/namespace.h>

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QApplication>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>

namespace CBFSchema {
	class QtReference;
}

namespace CBF {

/** @brief A Reference that can be set from a GUI window */
struct QtReference : public Reference {
	struct Control {
		Control() : control_name("Control") { }
		std::string control_name;
		Float min;
		Float max;
		unsigned int decimals;
		Float initial_value;
	};

	QtReference(const std::vector<Control> &controls, bool active = false, std::string window_title = "CBF:QtReference") {
		init(controls, active, window_title);
	}

	QtReference(const CBFSchema::QtReference &xml_instance, ObjectNamespacePtr object_namespace);

	void init(std::vector<Control> controls, bool active, std::string window_title = "CBF:QtReference") {
		QGridLayout *layout = new QGridLayout();
		m_Widget.setLayout(layout);

		m_Values.resize(controls.size());

		layout->addWidget(new QLabel("Active"), 0,0);
		m_ActiveCheckBox = new QCheckBox();
		m_ActiveCheckBox->setChecked(active);
		layout->addWidget(m_ActiveCheckBox, 0, 1);

		for (unsigned int i = 0; i < controls.size(); ++i) {
			QDoubleSpinBox *s = new QDoubleSpinBox;

			s->setMinimum(controls[i].min);
			s->setMaximum(controls[i].max);
			s->setDecimals(controls[i].decimals);
			s->setValue(controls[i].initial_value);
			s->setKeyboardTracking(false);

			layout->addWidget(s,i+1,1);

			QLabel *label = new QLabel(controls[i].control_name.c_str());
			layout->addWidget(label, i+1, 0);

			m_SpinBoxes.push_back(s);
		}
		layout->setColumnStretch(1, 10);

		m_Widget.setWindowTitle(window_title.c_str());

		m_Widget.show();

		m_References.resize(1);
		m_Values.resize(controls.size());
	}

	virtual void update() {
		for (unsigned int i = 0; i < m_Values.size(); ++i) {
			m_Values[i] = m_SpinBoxes[i]->value();
		}
		m_References[0] = m_Values;
	}

	virtual unsigned int dim() {
		return m_Values.size();
	}

	virtual std::vector<FloatVector> &get() {
		if (m_ActiveCheckBox->isChecked()) {
			return m_References;
		}
		return m_EmptyReferenceVector;		
	}

	QCheckBox *m_ActiveCheckBox;
	FloatVector m_Values;
	QWidget m_Widget;

	std::vector<FloatVector> m_EmptyReferenceVector;

	std::vector<QDoubleSpinBox*> m_SpinBoxes;
};

typedef boost::shared_ptr<QtReference> QtReferencePtr;

} // namespace

#endif
