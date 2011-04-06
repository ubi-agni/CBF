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

#ifndef CBF_PA10_RESOURCE_HH
#define CBF_PA10_RESOURCE_HH

#include <cbf/resource.h>

#include "pa10/pa.h"

#include <boost/numeric/ublas/vector.hpp>
#include <string>

#include <iostream>
#include <map>
#include <stdexcept>

namespace CBFSchema { class PA10JointResource; } 

namespace CBF {
	
	
	struct PA10JointResource : public Resource {
		PA10JointResource(const CBFSchema::PA10JointResource &xml_instance);
	
		PA10JointResource(ARM arm = ARM0) :
			m_Arm(arm)
		{
			pa_opn_arm(m_Arm);
		}
	
		~PA10JointResource() {
			pa_cls_arm(m_Arm);
		}
	
		/** The arm that is to be controlled */
		ARM m_Arm;
	
		FloatVector m_CurrentPosture;
	
		virtual const FloatVector &get();
	
		virtual void set(const FloatVector &);
	
		virtual void add(const FloatVector &);
	
		virtual unsigned int dim() { return 7; }
	
		/**
			The PA10 library requires the functions pa_ini_sys and pa_ter_sys to be 
			called respectively before and after using any other pa_ functions. Thus
			we have a static member instance see m_Helper) of this class and let
			the C++ runtime do the construction and destruction (and thus calling of
			pa_ini_sys and pa_ter_sys) for us :)
		*/
		struct Helper {
			Helper() {
				//std::cout << "pa_ini_sys" << std::endl;
				// int ret = pa_ini_sys();
	
				//if (ret != 0)
				//	throw std::runtime_error("[PA10JointResource]: Could not initialize PA10 library");
			}
	
			~Helper() {
				//std::cout << "pa_ter_sys" << std::endl;
				//int ret = pa_ter_sys();
				//if (ret != 0)
				//	throw std::runtime_error("[PA10JointResource]: Could not terminate PA10 library");
			}
		};
	
		/**
			See the PA10JointResource::Helper doc..
		*/
		static Helper m_Helper;
	};
	
	typedef boost::shared_ptr<PA10JointResource> PA10JointResourcePtr;
} // namespace

#endif
