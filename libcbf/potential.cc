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

#include <cbf/potential.h>
#include <cbf/plugin_macros.h>

namespace CBF {

	bool Potential::check_convergence(Float dist, Float stepnorm) {
		CBF_DEBUG("distance_thresh " << (dist < m_DistanceThreshold) << " " << dist)
		bool converged = 
			((m_ConvergenceCriterion & DISTANCE_THRESHOLD) && (dist < m_DistanceThreshold)) ||
			((m_ConvergenceCriterion & STEP_THRESHOLD) && (stepnorm < m_StepNormThreshold));
		CBF_DEBUG("converged: " << converged)
		return converged;
	}


	void CompositePotential::gradient (
		FloatVector &result, 
		const std::vector<FloatVector > &references, 
		const FloatVector &input
	) {
		result = ublas::zero_vector<Float>(input.size());

		unsigned int current_index = 0;
		for (unsigned int i = 0; i < m_Potentials.size(); ++i) {
			CBF_DEBUG("[CompositePotential]: ----")
			std::copy(references[0].begin() + current_index, references[0].begin() + current_index + m_Potentials[i]->task_dim(), m_ref_buffers[i].begin());

			std::copy(input.begin() + current_index, input.begin() + current_index + m_Potentials[i]->task_dim(), m_in_buffers[i].begin());
			CBF_DEBUG("[CompositePotential]: in: " <<  m_in_buffers[i])

			std::vector<FloatVector > tmp_refs;
			tmp_refs.push_back(m_ref_buffers[i]);
			CBF_DEBUG("[CompositePotential]: tmp_refs: " <<  tmp_refs[0])
			CBF_DEBUG("[CompositePotential]: m_ref_buffers: " <<  m_ref_buffers[i])

			m_Potentials[i]->gradient(m_out_buffers[i], tmp_refs, m_in_buffers[i]);
			CBF_DEBUG("[CompositePotential]: out: " <<  m_out_buffers[i])
			std::copy(m_out_buffers[i].begin(), m_out_buffers[i].end(), result.begin() + current_index);

			current_index += m_Potentials[i]->task_dim();
		}
	}


	#ifdef CBF_HAVE_XSD
	SquarePotential::SquarePotential(const SquarePotentialType &xml_instance) {
		CBF_DEBUG("[SquarePotential(const SquaredPotentialType &xml_instance)]: yay!")
		CBF_DEBUG("Coefficient: " << xml_instance.Coefficient())
		if (xml_instance.Coefficient().present())
			m_Coefficient = *xml_instance.Coefficient();

		if (xml_instance.Dimension().present())
			m_Dim = *xml_instance.Dimension();

		if (xml_instance.MaxGradientStepNorm().present())
			m_MaxGradientStep = *xml_instance.MaxGradientStepNorm();

		if (xml_instance.DistanceThreshold().present())
			m_DistanceThreshold = *(xml_instance.DistanceThreshold());
	}

	CBF_PLUGIN_CLASS(SquarePotential, Potential)
	
	CompositePotential::CompositePotential(const CompositePotentialType &xml_instance) {
		CBF_DEBUG("[CompositePotential(const CompositePotentialType &xml_instance)]: yay!")
		//std::cout << "Coefficient: " << xml_instance.Coefficient() << std::endl;
		std::vector<PotentialPtr> tmp;
		for (
			CompositePotentialType::Potential_const_iterator it = xml_instance.Potential().begin();
			it != xml_instance.Potential().end();
			++it)
			{
				PotentialPtr pot = PluginPool<Potential>::get_instance()->create_from_xml(*it);
				tmp.push_back(pot);
			}
		set_potentials(tmp);

		if (xml_instance.DistanceThreshold().present())
			m_DistanceThreshold = *(xml_instance.DistanceThreshold());
	}
	#endif

	CBF_PLUGIN_CLASS(CompositePotential, Potential)
} // namespace
