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
#include <cbf/xml_object_factory.h>
#include <cbf/xml_factory.h>

namespace CBF {

	FloatVector &SquarePotential::select_reference(
		const std::vector<FloatVector > &references,
		const FloatVector &input)
	{
		assert(references.size() > 0);

		// Find the closest reference
		Float min_distance = distance(references[0], input);
		unsigned int min_index = 0;

		for (unsigned int i = 1; i < references.size(); ++i) {
			Float cur_distance = distance(references[i], input);
			if (cur_distance < min_distance) {
				min_index = i;
				min_distance = cur_distance;
			}
		}

		m_CurrentReference = references[min_distance];

		return m_CurrentReference;
	}

	void SquarePotential::gradient (
		FloatVector &result,
		const FloatVector &reference,
		const FloatVector &input)
	{
		result = (reference - input);
	}

	void SquarePotential::integration (
		FloatVector &nextpos,
		const FloatVector &currentpos,
		const FloatVector &taskvel,
		const Float timestep)
	{
		nextpos = currentpos + taskvel*timestep;
	}


#ifdef CBF_HAVE_XSD
	SquarePotential::SquarePotential(const CBFSchema::SquarePotential &xml_instance, ObjectNamespacePtr object_namespace) :
		Potential(xml_instance, object_namespace) 
	{
		CBF_DEBUG("[SquarePotential(const SquaredPotentialType &xml_instance)]: yay!");
		CBF_DEBUG("Coefficient: " << xml_instance.Coefficient());

    	// m_SensorDim = xml_instance.Dimension();
		// m_DistanceThreshold = xml_instance.DistanceThreshold();
	}

	static XMLDerivedFactory<SquarePotential, CBFSchema::SquarePotential> x;

	static XMLConstructorCreator<
		Potential, 
		SquarePotential,
		CBFSchema::SquarePotential
	> foobar;

	//static XMLConstructorCreator<Potential, CBFSchema::SquarePotential> x3;

#endif
} // namespace
