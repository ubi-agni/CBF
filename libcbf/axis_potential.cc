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

#include <cbf/axis_potential.h>
#include <cbf/xml_factory.h>

namespace CBF {
	#ifdef CBF_HAVE_XSD

		AxisPotential::AxisPotential(const CBFSchema::AxisPotential &xml_instance, ObjectNamespacePtr object_namespace) :
			Potential(xml_instance, object_namespace) {
			CBF_DEBUG("[AxisAnglePotential(const AxisAnglePotentialType &xml_instance)]: yay!");
			CBF_DEBUG("Coefficient: " << xml_instance.Coefficient());
			m_Coefficient = xml_instance.Coefficient();
		}

		static XMLDerivedFactory<
			AxisPotential, 
			CBFSchema::AxisPotential
		> x1;

	#endif
} // namespace
