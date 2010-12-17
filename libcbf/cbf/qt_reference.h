#ifndef CBF_QT_REFERENCE_HH
#define CBF_QT_REFERENCE_HH

#include <cbf/config.h>
#include <cbf/reference.h>
#include <cbf/types.h>

#include <vector>

#include <boost/shared_ptr.hpp>

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QApplication>
#include <QtGui/QDoubleSpinBox>

namespace CBFSchema {
	class QtReference;
}

namespace CBF {

struct QtReference : public Reference {
	struct Control {
		Float min;
		Float max;
		unsigned int decimals;
		Float initial_value;
	};

	QtReference(const std::vector<Control> &controls) {
		init(controls);
	}

	QtReference(const CBFSchema::QtReference &xml_instance);

	void init(std::vector<Control> controls) {
		QVBoxLayout *layout = new QVBoxLayout();
		m_Widget.setLayout(layout);

		m_Values.resize(controls.size());

		for (unsigned int i = 0; i < controls.size(); ++i) {
			QDoubleSpinBox *s = new QDoubleSpinBox;

			s->setMinimum(controls[i].min);
			s->setMaximum(controls[i].max);
			s->setDecimals(controls[i].decimals);
			s->setValue(controls[i].initial_value);

			layout->addWidget(s);
			m_SpinBoxes.push_back(s);
		}

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
