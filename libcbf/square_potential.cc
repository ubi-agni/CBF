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

#include <cbf/square_potential.h>
#include <cbf/xml_factory.h>

namespace CBF {

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
		if (result_norm >= m_MaxGradientStepNorm)
			result = (m_MaxGradientStepNorm/result_norm) * result;
		// std::cout << "[SquaredPotential]: result: " << result << std::endl;
	}



#ifdef CBF_HAVE_XSD
	SquarePotential::SquarePotential(const CBFSchema::SquarePotential &xml_instance) :
		Potential(xml_instance) 
	{
		CBF_DEBUG("[SquarePotential(const SquaredPotentialType &xml_instance)]: yay!")
		CBF_DEBUG("Coefficient: " << xml_instance.Coefficient())
		m_Coefficient = xml_instance.Coefficient();

		m_Dim = xml_instance.Dimension();

		// m_DistanceThreshold = xml_instance.DistanceThreshold();
	}

	static XMLDerivedFactory<SquarePotential, CBFSchema::SquarePotential, Potential, CBFSchema::Potential> x;
	static XMLCreator<
		SquarePotential, 
		CBFSchema::SquarePotential, 
		Constructor<
			SquarePotential, 
			CBFSchema::SquarePotential
		> > x2(Constructor<SquarePotential, CBFSchema::SquarePotential>());
	
#endif
} // namespace
