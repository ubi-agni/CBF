/* -*- mode: c-non-suck; -*- */

#ifndef CBF_CONTROLLER_SEQUENCE_HH
#define CBF_CONTROLLER_SEQUENCE_HH

#include <cbf/plugin_decl_macros.h>
#include <cbf/controller.h>

#include <vector>
#include <boost/shared_ptr.hpp>

CBF_PLUGIN_PREAMBLE(ControllerSequence)

namespace CBF {
	
	/**
		@brief A controller that runs a sequence of controllers.
	
		This controller can be used to execute sequences of controllers. Each controller in the sequence is run until finished. When the last controller finished, the ControllerSequence finished, too.
	*/
	struct ControllerSequence : public Controller {
		CBF_PLUGIN_DECL_METHODS(ControllerSequence)

		std::vector<ControllerPtr> m_Controllers;
		std::vector<ControllerPtr>::iterator m_Iterator;
	
		ControllerSequence() {
	
		}
	
		virtual bool step();
	
		virtual bool finished();
	
		/**
			Resets this controller, so it can start from the beginning again. Note that this
			is also done when finished() is true.. So calling step() after this controller 
			finished just restarts it..
		*/
		virtual void reset() {
			m_Iterator = m_Controllers.begin();
		}
	};
	
	typedef boost::shared_ptr<ControllerSequence> ControllerSequencePtr;
	

} // namespace

#endif
