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



	#ifdef CBF_HAVE_XSD
	Potential::Potential(const PotentialType &xml_instance) {
		m_ConvergenceCriterion = 0;

		if (xml_instance.ConvergenceCriterion().begin() == xml_instance.ConvergenceCriterion().end())
			throw std::runtime_error("[Potential]: Missing ConvergenceCriterion");

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

		m_MaxGradientStepNorm = xml_instance.MaxGradientStepNorm();
	}
	#endif
} // namespace
