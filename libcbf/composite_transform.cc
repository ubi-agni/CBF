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

#include <cbf/composite_transform.h>
#include <cbf/debug_macros.h>
#include <cbf/xml_factory.h>

#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>


namespace CBF {

	void CompositeSensorTransform::set_transforms(std::vector<SensorTransformPtr> transforms) {
		// First set the member, so that task_dim() and resource_dim() return sensible value..
		m_SensorTransforms = transforms;

		unsigned int total_resource_dim = m_SensorTransforms[0]->resource_dim();

		unsigned int total_task_dim = 0;
		for (unsigned int i = 0; i < m_SensorTransforms.size(); ++i)
			total_task_dim += m_SensorTransforms[i]->task_dim();


		m_TaskJacobian = ublas::zero_matrix<Float>(total_task_dim, total_resource_dim);
		CBF_DEBUG("task_dim " << task_dim());
		m_Result = FloatVector(task_dim());
		CBF_DEBUG("m_Result " << m_Result);
	}

#if 0
	void CompositeSensorTransform::set_resource(ResourcePtr resource) {
		SensorTransform::set_resource(resource);
		CBF_DEBUG("set_resource()");
		for (unsigned int i = 0; i < m_SensorTransforms.size(); ++i)
			m_SensorTransforms[i]->set_resource(resource);
	}
#endif

	void CompositeSensorTransform::update(const FloatVector &resource_value) {
		unsigned int current_task_pos = 0;
		for (unsigned int i = 0; i < m_SensorTransforms.size(); ++i) {
			//! Make all subordinate transforms update their state..
			m_SensorTransforms[i]->update(resource_value);
	
			//! Assemble total jacobian..
			CBF_DEBUG("range: " << current_task_pos << " " << current_task_pos + m_SensorTransforms[i]->task_jacobian().size1());
			ublas::matrix_range<FloatMatrix > mr(
				m_TaskJacobian,
				ublas::range(current_task_pos, current_task_pos + m_SensorTransforms[i]->task_jacobian().size1()),
				ublas::range(0, resource_dim())
			);

	
			FloatMatrix tmp;
			tmp = m_SensorTransforms[i]->task_jacobian();
	
			CBF_DEBUG("tmp " << tmp);

			mr.assign(tmp);
			CBF_DEBUG("m_Jacobian: " << m_TaskJacobian);
	
			ublas::vector<Float> tmp_result (m_SensorTransforms[i]->task_jacobian().size1());
			tmp_result = m_SensorTransforms[i]->result();
			CBF_DEBUG("tmp_result " << tmp_result);
	
			CBF_DEBUG("current_task_pos " << current_task_pos << " task_dim " << m_SensorTransforms[i]->task_jacobian().size1());
			for (unsigned int j = 0; j < m_SensorTransforms[i]->task_jacobian().size1(); ++j) {
				m_Result(current_task_pos + j) = tmp_result(j);
			}
	
			current_task_pos += m_SensorTransforms[i]->task_jacobian().size1();
		}
	
	}
	
	#ifdef CBF_HAVE_XSD
		CompositeSensorTransform::CompositeSensorTransform(const CBFSchema::CompositeSensorTransform &xml_instance) :
			SensorTransform(xml_instance)
		{
			CBF_DEBUG("yay!!!");
		
			std::vector<SensorTransformPtr> transforms;
		
			//! Instantiate the subordinate transforms
			CBFSchema::CompositeSensorTransform::SensorTransform1_const_iterator it;
			for (
				it = xml_instance.SensorTransform1().begin(); 
				it != xml_instance.SensorTransform1().end();
				++it
			)
			{
				SensorTransformPtr tr(XMLObjectFactory::instance()->create<SensorTransform>(*it));
				transforms.push_back(tr);
			}
		
			set_transforms(transforms);
		}
		
		static XMLDerivedFactory<CompositeSensorTransform, CBFSchema::CompositeSensorTransform> x;
	#endif
};

