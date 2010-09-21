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

#include <cbf/composite_potential.h>
#include <cbf/config.h>
#include <cbf/plugin_impl_macros.h>
#include <cbf/xml_factories.h>

namespace CBF {

	void CompositePotential::gradient (
		FloatVector &result, 
		const std::vector<FloatVector > &references, 
		const FloatVector &input
	) {
		result = ublas::zero_vector<Float>(input.size());

		unsigned int current_index = 0;
		for (unsigned int i = 0; i < m_Potentials.size(); ++i) {
			CBF_DEBUG("[CompositePotential]: ----")
			std::copy(references[0].begin() + current_index, references[0].begin() + current_index + m_Potentials[i]->dim(), m_ref_buffers[i].begin());

			std::copy(input.begin() + current_index, input.begin() + current_index + m_Potentials[i]->dim(), m_in_buffers[i].begin());
			CBF_DEBUG("[CompositePotential]: in: " <<  m_in_buffers[i])

			std::vector<FloatVector > tmp_refs;
			tmp_refs.push_back(m_ref_buffers[i]);
			CBF_DEBUG("[CompositePotential]: tmp_refs: " <<  tmp_refs[0])
			CBF_DEBUG("[CompositePotential]: m_ref_buffers: " <<  m_ref_buffers[i])

			m_Potentials[i]->gradient(m_out_buffers[i], tmp_refs, m_in_buffers[i]);
			CBF_DEBUG("[CompositePotential]: out: " <<  m_out_buffers[i])
			std::copy(m_out_buffers[i].begin(), m_out_buffers[i].end(), result.begin() + current_index);

			current_index += m_Potentials[i]->dim();
		}
	}

#ifdef CBF_HAVE_XSD
	CompositePotential::CompositePotential(const ::CompositePotential &xml_instance) :
		Potential(xml_instance) 
	{
		CBF_DEBUG("[CompositePotential(const CompositePotentialType &xml_instance)]: yay!")
		//std::cout << "Coefficient: " << xml_instance.Coefficient() << std::endl;
		std::vector<PotentialPtr> tmp;
		for (
			::CompositePotential::Potential1_const_iterator it = xml_instance.Potential1().begin();
			it != xml_instance.Potential1().end();
			++it)
			{
				PotentialPtr pot = PluginPool<Potential>::get_instance()->create_from_xml(*it);
				tmp.push_back(pot);
			}
		set_potentials(tmp);

		//m_DistanceThreshold = xml_instance.DistanceThreshold();
	}
	CBF_PLUGIN_CLASS(CompositePotential, Potential)
#endif

} // namespace
