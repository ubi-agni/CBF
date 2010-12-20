#include <cbf/qt_reference.h>
#include <cbf/xml_factory.h>
#include <cbf/schemas.hxx>

namespace CBF {

#ifdef CBF_HAVE_XSD 
	QtReference::QtReference(const CBFSchema::QtReference &xml_instance) {
		std::vector<Control> controls;
		for(
			CBFSchema::QtReference::Control_const_iterator it = xml_instance.Control().begin();
			it != xml_instance.Control().end();
			++it) {

			Control c;

			if ((*it).ControlName()) {
				c.control_name = *(*it).ControlName();
			}

			c.min = (*it).Minimum();
			c.max = (*it).Maximum();
			c.decimals = (*it).Decimals();
			c.initial_value = (*it).InitialValue();

			controls.push_back(c);
		}
		bool active = false;
		if (xml_instance.Active()) {
			active = *(xml_instance.Active());
		}

		if (xml_instance.WindowTitle()) {
			init(controls, active, *(xml_instance.WindowTitle()));
		} else {
			init(controls, active);
		}
	}

	static XMLDerivedFactory<QtReference, CBFSchema::QtReference> x1;
#endif

}// namespace


