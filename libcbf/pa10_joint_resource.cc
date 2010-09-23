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

#include <cbf/pa10_joint_resource.h>

namespace CBF {
	
	PA10JointResource::Helper PA10JointResource::m_Helper;
	
	const boost::numeric::ublas::vector<Float> &PA10JointResource::get() {
		return m_CurrentPosture;
	}
	
	void PA10JointResource::set(const boost::numeric::ublas::vector<Float> &in) {
		m_CurrentPosture = in;
	}
	
	void PA10JointResource::add(const boost::numeric::ublas::vector<Float> &in) {
		m_CurrentPosture += in;
	}
	
	
	#ifdef CBF_HAVE_XSD
		PA10JointResource::PA10JointResource(const CBFSchema::PA10JointResource &xml_instance)
		{
	
		}
		static XMLDerivedFactory<PA10JointResource, CBFSchema::PA10JointResource, Resource, CBFSchema::Resource> x;
	#endif
	
} // namespace

