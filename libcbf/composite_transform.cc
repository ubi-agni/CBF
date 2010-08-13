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
#include <cbf/plugin_impl_macros.h>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>


namespace CBF {

	void CompositeSensorTransform::set_transforms(std::vector<SensorTransformPtr> transforms) {
		// First set the member, so that task_dim() and resource_dim() return sensible value..
		m_SensorTransforms = transforms;

		m_TaskJacobian = ublas::zero_matrix<Float>(task_dim(), resource_dim());
		CBF_DEBUG("task_dim " << task_dim())
		m_Result = FloatVector(task_dim());
		CBF_DEBUG("m_Result " << m_Result)
	}

	void CompositeSensorTransform::set_resource(ResourcePtr resource) {
		SensorTransform::set_resource(resource);
		CBF_DEBUG("set_resource()")
		for (unsigned int i = 0; i < m_SensorTransforms.size(); ++i)
			m_SensorTransforms[i]->set_resource(resource);
	}

	unsigned int CompositeSensorTransform::resource_dim() const {
		//! NOTE: All sensor transforms should work on the same resource
		if (m_SensorTransforms.size() == 0) throw std::runtime_error("[CompositeSensorTransform]: No sensor transforms set");
	
		return m_SensorTransforms[0]->resource_dim();
	}
	
	unsigned int CompositeSensorTransform::task_dim() const {
		//! TODO: Cache result if inefficient as is..
		unsigned int sum = 0;
	
		for (unsigned int index = 0; index < m_SensorTransforms.size(); ++index) {
			sum += m_SensorTransforms[index]->task_dim();
		}

		CBF_DEBUG(sum)
		return sum;
	}
	
	void CompositeSensorTransform::update() {
	
		unsigned int current_task_pos = 0;
		for (unsigned int i = 0; i < m_SensorTransforms.size(); ++i) {
			//! Make all subordinate transforms update their state..
			m_SensorTransforms[i]->update();
	
			//! Assemble total jacobian..
			CBF_DEBUG("range: " << current_task_pos << " " << current_task_pos + m_SensorTransforms[i]->task_dim())
			ublas::matrix_range<FloatMatrix > mr(
				m_TaskJacobian,
				ublas::range(current_task_pos, current_task_pos + m_SensorTransforms[i]->task_dim()),
				ublas::range(0, resource_dim())
			);

	
			FloatMatrix tmp;
			tmp = m_SensorTransforms[i]->task_jacobian();
	
			mr.assign(tmp);
			CBF_DEBUG("m_Jacobian: " << m_TaskJacobian)
	
			ublas::vector<Float> tmp_result (m_SensorTransforms[i]->task_dim());
			tmp_result = m_SensorTransforms[i]->result();
			CBF_DEBUG("tmp_result " << tmp_result)
	
			CBF_DEBUG("current_task_pos " << current_task_pos << " task_dim " << m_SensorTransforms[i]->task_dim())
			for (unsigned int j = 0; j < m_SensorTransforms[i]->task_dim(); ++j) {
				m_Result(current_task_pos + j) = tmp_result(j);
			}
	
			current_task_pos += m_SensorTransforms[i]->task_dim();
		}
	
	}
	
	#ifdef CBF_HAVE_XSD
		CompositeSensorTransform::CompositeSensorTransform(const CompositeSensorTransformType &xml_instance) :
			SensorTransform(xml_instance)
		{
			CBF_DEBUG("yay!!!");
		
			std::vector<SensorTransformPtr> transforms;
		
			//! Instantiate the subordinate transforms
			SensorTransformChainType::SensorTransform_const_iterator it;
			for (
				it = xml_instance.SensorTransform().begin(); 
				it != xml_instance.SensorTransform().end();
				++it
			)
			{
#if 0
				SensorTransformPtr tr(PluginPool<SensorTransform>::get_instance()->create_from_xml(*it));
				transforms.push_back(tr);
				//tr->set_resource(ResourcePtr(new DummyResource(tr->get_resource_dim())));
#endif

				SensorTransformPtr tr(XMLBaseFactory<SensorTransform, SensorTransformType>::instance()->create(*it));
				transforms.push_back(tr);
			}
		
			set_transforms(transforms);
		}
		
	#endif

	CBF_PLUGIN_CLASS(CompositeSensorTransform, SensorTransform)

	
	static XMLDerivedFactory<CompositeSensorTransform, CompositeSensorTransformType, SensorTransform, SensorTransformType> x;
#if 0
template<> 
	XMLDerivedFactory<CompositeSensorTransform, CompositeSensorTransformType, SensorTransform, SensorTransformType>* 
		XMLBaseDerivedFactory<CompositeSensorTransform, CompositeSensorTransformType, SensorTransform, SensorTransformType>::m_Instance = 0;
#endif

};

