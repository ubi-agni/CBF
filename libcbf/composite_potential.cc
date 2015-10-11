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

#include <cbf/config.h>
#include <cbf/composite_potential.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

	void CompositePotential::gradient (
		FloatVector &result, 
		const std::vector<FloatVector > &references, 
		const FloatVector &input
	) {
		result = FloatVector::Zero(input.size());

		unsigned int current_index = 0;
		for (unsigned int i = 0; i < m_Potentials.size(); ++i) {
			m_ref_buffers[i] = references[0].segment(current_index, m_Potentials[i]->dim());
			m_in_buffers[i] = input.segment(current_index, m_Potentials[i]->dim());

			std::vector<FloatVector > tmp_refs;
			tmp_refs.push_back(m_ref_buffers[i]);

			m_Potentials[i]->gradient(m_out_buffers[i], tmp_refs, m_in_buffers[i]);
			result.segment(current_index, m_out_buffers[i].size())
					= m_out_buffers[i];

			current_index += m_Potentials[i]->dim();
		}
		CBF_DEBUG("result " << result.transpose());
	}

#ifdef CBF_HAVE_XSD
	CompositePotential::CompositePotential(const CBFSchema::CompositePotential &xml_instance, ObjectNamespacePtr object_namespace) :
		Potential(xml_instance, object_namespace) 
	{
		CBF_DEBUG("[CompositePotential(const CompositePotentialType &xml_instance)]: yay!");
		//std::cout << "Coefficient: " << xml_instance.Coefficient() << std::endl;
		std::vector<PotentialPtr> tmp;
		for (
			CBFSchema::CompositePotential::Potential1_const_iterator it = xml_instance.Potential1().begin();
			it != xml_instance.Potential1().end();
			++it)
			{
				PotentialPtr pot = XMLObjectFactory::instance()->create<Potential>(*it, object_namespace);
				tmp.push_back(pot);
			}
		set_potentials(tmp);

		//m_DistanceThreshold = xml_instance.DistanceThreshold();
	}

	static XMLDerivedFactory<
		CompositePotential, 
		CBFSchema::CompositePotential
	> x1;
#endif

} // namespace
