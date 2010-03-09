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

#include <cbf/dummy_resource.h>
#include <cbf/debug_macros.h>
#include <cbf/plugin_macros.h>
#include <cbf/utilities.h>

namespace CBF {
	#ifdef CBF_HAVE_XSD
	#ifdef CBF_HAVE_PLUGIN_SUPPORT
		DummyResource::DummyResource(const DummyResourceType &xml_instance)
		{
			m_Variables = ublas::zero_vector<Float>(xml_instance.Dimension());

#ifdef CBF_HAVE_GNUPLOT
			m_History = std::deque<FloatVector>(CBF_HIST_SIZE, FloatVector(m_Variables.size())),
			m_Times = FloatVector(CBF_HIST_SIZE, 0);


			for (unsigned int i = 0; i < CBF_HIST_SIZE; ++i) {
				m_Times[i] = i;
			}

			m_Count = 0;

#endif

			for (unsigned int i = 0; i < m_Variables.size(); ++i) {
				m_Variables[i] = 0.1 * 2 * M_PI * ((Float)rand()-(RAND_MAX/2.0))/(Float)RAND_MAX;
			}


		
			if (xml_instance.Vector().present())
				m_Variables = create_vector(xml_instance.Vector().get());
			CBF_DEBUG("current values: " << m_Variables)
		
		}
	#endif
	#endif
	
	CBF_PLUGIN_CLASS(DummyResource, Resource)
} //namespace

