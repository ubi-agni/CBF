/*
    This file is part of CBF.

    CBF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    CBF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CBF.  If not, see <http://www.gnu.org/licenses/>.


    Copyright 2009, 2010 Florian Paul Schmidt
*/

#ifndef CBF_BASE_CONTROLLER_HH
#define CBF_BASE_CONTROLLER_HH

#include <cbf/plugin_decl_macros.h>
#include <cbf/types.h>

#include <boost/shared_ptr.hpp>

#include <memory>
#include <string>
#include <vector>

/**
	@brief The CBF namespace holds the user visible classes provided by the ControlBasisFramework lib.
*/
namespace CBF {
	/**
		@brief Superclass of all controller classes.
	
		Since there are not only simple closed loop 
		controllers (see PrimitiveController), but 
		also some sort	of controller "programs" (e.g. 
		policy learned by reinforcement learning), 
		need a base class providing a common controller 
		interface.
	*/
	struct Controller {
		/**
			A virtual destructor, so polymorphic cleanup 
			works nicely
		*/
		virtual ~Controller();
	
		/**
			Subclasses need to implement this. If this is 
			a controller program, the semantics still
			is to run a single step of "underlying" 
			controllers and then return immediately.. 
	
			The controller is expected to return true 
			when finished() would return true, too.
	
			Always run step() at least once before 
			calling finished() for the first time.
		*/
		virtual bool step() { return true; }
	
		/**
			Has this controller reached its goal? 
			E.g. convergence for primitive controllers..
	
			Always run step() at least once before 
			calling finished() for the first time.

			This base class implementation always
			returns true, as there's nothing to do.
		*/
		virtual bool finished() { return true; }
	};
	
	
	//! Convenience typedef
	typedef boost::shared_ptr<Controller> ControllerPtr;



	/**
		@brief An interface to be implemented by 
		all Controllers that can act as subordinate 
		controller.

		The main difference is that this controller
		type is not expected to take action. But
		rather it should return the result of
		its computations by storing it into
		the result vector.

		The do_step() method should also return 
		a reference to the result vector, too, 
		so calls can be chained..
	*/
	struct SubordinateController : public Controller {
		virtual FloatVector &subordinate_step(FloatVector &result) = 0;
	};
} // namespace



#endif
