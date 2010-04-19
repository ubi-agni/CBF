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


	void SquarePotential::gradient (
		FloatVector &result,
		const std::vector<FloatVector > &references,
		const FloatVector &input
	) {
		//! First we find the closest reference vector
		Float min_dist = std::numeric_limits<Float>::max();
		unsigned int min_index = 0;

		//std::cout  << "[SquarePotential]: sizes: " << references[0].size() << " " << input.size() << std::endl;

		for (unsigned int i = 0; i < references.size(); ++i) {
			Float dist = distance(input, references[i]);
			if (dist < min_dist) {
				min_index = i;
				min_dist = dist;
			}
		}

		// CBF_DEBUG("min_index " << min_index)

		//! The gradient of a square function is just negative of
		//! input - reference..
		result = m_Coefficient * (references[min_index] - input);
		Float result_norm = norm(result);
		// CBF_DEBUG("result_norm " << result_norm)

		//! Normalize gradient step so it's not bigger than m_MaxGradientStep
		if (result_norm >= m_MaxGradientStep)
			result = (m_MaxGradientStep/result_norm) * result;
		// std::cout << "[SquaredPotential]: result: " << result << std::endl;

		m_Converged = check_convergence(distance(input, references[min_index]), norm(result));
		// CBF_DEBUG("m_Converged " << m_Converged)
	}



	#ifdef CBF_HAVE_XSD
	Potential::Potential(const PotentialType &xml_instance) {
		m_ConvergenceCriterion = 0;
		for (
			PotentialType::ConvergenceCriterion_const_iterator it = 
				xml_instance.ConvergenceCriterion().begin();
			it != xml_instance.ConvergenceCriterion().end();
			++it
		) {
			const DistanceThresholdType *d = dynamic_cast<const DistanceThresholdType *>(&(*it));
			if (d != 0) {
				m_ConvergenceCriterion |= DISTANCE_THRESHOLD;
				m_DistanceThreshold = d->Threshold();
			}
			const StepNormThresholdType *s = dynamic_cast<const StepNormThresholdType *>(&(*it));
			if (s != 0) {
				m_ConvergenceCriterion |= STEP_THRESHOLD;
				m_StepNormThreshold = s->Threshold();
			}
		}
	}

	SquarePotential::SquarePotential(const SquarePotentialType &xml_instance) :
		Potential(xml_instance) 
	{
		CBF_DEBUG("[SquarePotential(const SquaredPotentialType &xml_instance)]: yay!")
		CBF_DEBUG("Coefficient: " << xml_instance.Coefficient())
		m_Coefficient = xml_instance.Coefficient();

		m_Dim = xml_instance.Dimension();

		m_MaxGradientStep = xml_instance.MaxGradientStepNorm();

		// m_DistanceThreshold = xml_instance.DistanceThreshold();
	}

	CBF_PLUGIN_CLASS(SquarePotential, Potential)
	
	CompositePotential::CompositePotential(const CompositePotentialType &xml_instance) :
		Potential(xml_instance) 
	{
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

		//m_DistanceThreshold = xml_instance.DistanceThreshold();
	}
	#endif

	CBF_PLUGIN_CLASS(CompositePotential, Potential)
} // namespace
