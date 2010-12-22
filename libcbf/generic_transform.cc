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
#include <cbf/debug_macros.h>
#include <cbf/xml_factory.h>

#ifndef CBF_HAVE_EIGEN2
  #include <cbf/svd.h>
#endif

namespace CBF {


void GenericEffectorTransform::update(const FloatVector &resource_value, const FloatMatrix &task_jacobian) {
	CBF_DEBUG("updating");

	//m_SensorTransform->task_jacobian(m_Jacobian);
	pseudo_inverse(task_jacobian, m_InverseTaskJacobian);
}

void DampedGenericEffectorTransform::update(const FloatVector &resource_value, const FloatMatrix &task_jacobian) {
	CBF_DEBUG("updating");

	//m_SensorTransform->task_jacobian(m_Jacobian);
	damped_pseudo_inverse(task_jacobian, m_InverseTaskJacobian, m_DampingConstant);
}


void DampedWeightedGenericEffectorTransform::update(
	const FloatVector &resource_value, 
	const FloatMatrix &task_jacobian
) {
	CBF_DEBUG("updating");

	//m_SensorTransform->task_jacobian(m_Jacobian);
	damped_pseudo_inverse(task_jacobian, m_InverseTaskJacobian);
}


#ifdef CBF_HAVE_XSD
	GenericEffectorTransform::GenericEffectorTransform(
		const CBFSchema::GenericEffectorTransform &xml_instance
	)
	{
		init(xml_instance.TaskDimension(), xml_instance.ResourceDimension());
	}

	PaddedEffectorTransform::PaddedEffectorTransform(
		const CBFSchema::PaddedEffectorTransform &xml_instance
	)
	{
		init(
			xml_instance.TaskDimension(), 
			xml_instance.ResourceDimension(), 
			*(XMLFactory<FloatVector>::instance()->create(xml_instance.Diagonal()))
		);
	}
	

	DampedGenericEffectorTransform::DampedGenericEffectorTransform(
		const CBFSchema::DampedGenericEffectorTransform &xml_instance
	) 
	{
		init(
			xml_instance.TaskDimension(), 
			xml_instance.ResourceDimension(),
			xml_instance.DampingConstant()
		);
	}

	DampedWeightedGenericEffectorTransform::DampedWeightedGenericEffectorTransform(const CBFSchema::DampedWeightedGenericEffectorTransform &xml_instance) 
	{
		init(
			xml_instance.TaskDimension(), 
			xml_instance.ResourceDimension(),
			xml_instance.DampingConstant()
		);
	}

	static XMLDerivedFactory<
		GenericEffectorTransform, 
		CBFSchema::GenericEffectorTransform
	> x1;

	static XMLDerivedFactory<
		DampedGenericEffectorTransform, 
		CBFSchema::DampedGenericEffectorTransform
	> x2;

	static XMLDerivedFactory<
		DampedWeightedGenericEffectorTransform, 
		CBFSchema::DampedWeightedGenericEffectorTransform
	> x3;

	static XMLDerivedFactory<
		PaddedEffectorTransform, 
		CBFSchema::PaddedEffectorTransform
	> x4;


#endif

} // namespace

