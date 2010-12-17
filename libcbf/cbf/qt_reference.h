#ifndef CBF_QT_REFERENCE_HH
#define CBF_QT_REFERENCE_HH

#include <cbf/config.h>
#include <cbf/reference.h>
#include <cbf/types.h>

#include <vector>
#include <string>

#include <boost/shared_ptr.hpp>

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QApplication>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>

namespace CBFSchema {
	class QtReference;
}

namespace CBF {

struct QtReference : public Reference {
	struct Control {
		Control() : control_name("Control") { }
		std::string control_name;
		Float min;
		Float max;
		unsigned int decimals;
		Float initial_value;
	};

	QtReference(const std::vector<Control> &controls, std::string window_title = "CBF:QtReference") {
		init(controls, window_title);
	}

	QtReference(const CBFSchema::QtReference &xml_instance);

	void init(std::vector<Control> controls, std::string window_title = "CBF:QtReference") {
		// QVBoxLayout *layout = new QVBoxLayout();
		QGridLayout *layout = new QGridLayout();
		m_Widget.setLayout(layout);

		m_Values.resize(controls.size());

		for (unsigned int i = 0; i < controls.size(); ++i) {
			QDoubleSpinBox *s = new QDoubleSpinBox;

			s->setMinimum(controls[i].min);
			s->setMaximum(controls[i].max);
			s->setDecimals(controls[i].decimals);
			s->setValue(controls[i].initial_value);
			s->setKeyboardTracking(false);

			layout->addWidget(s,i,1);

			QLabel *label = new QLabel(controls[i].control_name.c_str());
			layout->addWidget(label, i, 0);

			m_SpinBoxes.push_back(s);
		}
		layout->setColumnStretch(1, 10);

#if 0
		QDoubleSpinBox *valid = new QDoubleSpinBox;
		valid->setMinimum(-10000);
		valid->setMaximum(10000);
		valid->setDecimals(3);
		QObject::connect(m_SpinBoxes[0], SIGNAL(valueChanged(double)), valid, SLOT(setValue(double)));
		layout->addWidget(valid);
#endif

		m_Widget.setWindowTitle(window_title.c_str());

		m_Widget.show();

		m_References.resize(1);
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

	FloatVector m_Values;
	QWidget m_Widget;
	std::vector<QDoubleSpinBox*> m_SpinBoxes;
};

typedef boost::shared_ptr<QtReference> QtReferencePtr;

} // namespace

#endif
