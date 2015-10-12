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
#include <cbf/xml_object_factory.h>
#include <cbf/xml_factory.h>
#include <cbf/foreign_object.h>

namespace CBF {


void GenericEffectorTransform::update(const FloatVector &resource_value, const FloatMatrix &task_jacobian) {
	pseudo_inverse(task_jacobian, m_InverseTaskJacobian);
}

void DampedGenericEffectorTransform::update(const FloatVector &resource_value, const FloatMatrix &task_jacobian) {
	damped_pseudo_inverse(task_jacobian, m_InverseTaskJacobian, m_DampingConstant);
}

void ThresholdGenericEffectorTransform::update(const FloatVector &resource_value, const FloatMatrix &task_jacobian) {
	threshold_pseudo_inverse(task_jacobian, m_InverseTaskJacobian, m_Threshold);
}

#ifdef CBF_HAVE_XSD
	GenericEffectorTransform::GenericEffectorTransform(
		const CBFSchema::GenericEffectorTransform &xml_instance, ObjectNamespacePtr object_namespace
	) :
		EffectorTransform(xml_instance, object_namespace)
	{
		init(xml_instance.TaskDimension(), xml_instance.ResourceDimension());
	}

	DampedGenericEffectorTransform::DampedGenericEffectorTransform(
		const CBFSchema::DampedGenericEffectorTransform &xml_instance, ObjectNamespacePtr object_namespace
	) :
		EffectorTransform(xml_instance, object_namespace)
	{
		init(
			xml_instance.TaskDimension(),
			xml_instance.ResourceDimension(),
			xml_instance.DampingConstant()
		);
	}

	ThresholdGenericEffectorTransform::ThresholdGenericEffectorTransform(
		const CBFSchema::ThresholdGenericEffectorTransform &xml_instance, ObjectNamespacePtr object_namespace
	) :
		EffectorTransform(xml_instance, object_namespace)
	{
		init(
			xml_instance.TaskDimension(),
			xml_instance.ResourceDimension(),
			xml_instance.Threshold()
		);
	}

	PaddedEffectorTransform::PaddedEffectorTransform(
		const CBFSchema::PaddedEffectorTransform &xml_instance, ObjectNamespacePtr object_namespace
	) :
		EffectorTransform(xml_instance, object_namespace)
	{
		init(
			xml_instance.TaskDimension(), 
			xml_instance.ResourceDimension(), 
			*XMLObjectFactory::instance()->create<ForeignObject<FloatVector> >(xml_instance.Diagonal(), object_namespace)->m_Object
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
		ThresholdGenericEffectorTransform,
		CBFSchema::ThresholdGenericEffectorTransform
	> x3;

	static XMLDerivedFactory<
		PaddedEffectorTransform, 
		CBFSchema::PaddedEffectorTransform
	> x4;


#endif

} // namespace

