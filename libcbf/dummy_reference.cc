/* -*- mode: c-non-suck; -*- */

#include <cbf/config.h>
#include <cbf/dummy_reference.h>
#include <cbf/plugin_macros.h>
#include <cbf/utilities.h>

namespace CBF {

	#ifdef CBF_HAVE_XSD
	DummyReference::DummyReference(const DummyReferenceType &xml_instance) {
		CBF_DEBUG("constructing")

		//! Set references..
		CBF_DEBUG("Setting Reference(s)...")
		for (
			DummyReferenceType::Vector_const_iterator it = 
				xml_instance.Vector().begin(); 
			it != xml_instance.Vector().end();
			++it
		)
		{
			FloatVector tmp;
			CBF_DEBUG("Extracting reference vector coefficients...")
			try {
				tmp = create_vector(*it);
			} catch(...) {
				CBF_DEBUG("extracting cefficients failed");
				throw;
			}
	
			CBF_DEBUG("Reference: " << tmp)

			m_References.push_back(tmp);
			//PrimitiveControllerPtr controller(new PrimitiveController(*it));
			//m_SubordinateControllers.push_back(controller);
		}
	}
	#endif

	CBF_PLUGIN_CLASS(DummyReference, Reference)
} // namespace




