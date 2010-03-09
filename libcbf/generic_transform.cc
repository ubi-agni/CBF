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

#include <cbf/generic_transform.h>
#include <cbf/plugin_macros.h>

namespace CBF {

void GenericEffectorTransform::update() {
	CBF_DEBUG("updating")

	//m_SensorTransform->task_jacobian(m_Jacobian);
	pseudo_inverse(m_SensorTransform->task_jacobian(), m_InverseTaskJacobian);

	CBF_DEBUG("m_InvJacobian: " << m_InverseTaskJacobian)
}


void DampedWeightedGenericEffectorTransform::update() {
	CBF_DEBUG("updating")

	//m_SensorTransform->task_jacobian(m_Jacobian);
	pseudo_inverse(m_SensorTransform->task_jacobian(), m_InverseTaskJacobian);

	CBF_DEBUG("m_InvJacobian: " << m_InverseTaskJacobian)
}


#ifdef CBF_HAVE_XSD
	GenericEffectorTransform::GenericEffectorTransform(const GenericEffectorTransformType &xml_instance)
	{
	
	}
	
	DampedWeightedGenericEffectorTransform::DampedWeightedGenericEffectorTransform(const DampedWeightedGenericEffectorTransformType &xml_instance)
	{
	
	}
#endif

CBF_PLUGIN_CLASS(GenericEffectorTransform, EffectorTransform)
CBF_PLUGIN_CLASS(DampedWeightedGenericEffectorTransform, EffectorTransform)


} // namespace

