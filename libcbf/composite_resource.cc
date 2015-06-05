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

#include <cbf/composite_resource.h>
#include <cbf/xml_object_factory.h>

namespace CBF {


  CompositeResource::CompositeResource(std::vector<ResourcePtr> resources) {
    set_resources(resources);
  }

  void CompositeResource::set_resources(std::vector<ResourcePtr> resources) {
    m_Resources = resources;
    unsigned int dim = 0;

    for (unsigned int i = 0, len = m_Resources.size(); i < len; i++) {
      dim += m_Resources[i]->dim();
    }

    m_ResourceValue = FloatVector(dim);
    m_ResourceValueVelocity = FloatVector(dim);
  }

  const std::vector<ResourcePtr> &CompositeResource::resources() {
    return m_Resources;
  }

  void CompositeResource::update() {
    unsigned int current_start_index = 0;

    for (unsigned int i = 0, len = m_Resources.size(); i < len; i++) {
      m_Resources[i]->update();

      m_ResourceValue.segment(current_start_index, m_Resources[i]->get().size()) = m_Resources[i]->get();
      m_ResourceValueVelocity.segment(current_start_index, m_Resources[i]->get().size()) = m_Resources[i]->get_resource_vel();

      current_start_index += m_Resources[i]->dim();
    }
  }


  void CompositeResource::add(const FloatVector &resource_velocity, const Float timestep) {
    unsigned int current_start_index = 0;

    for (unsigned int i = 0, len = m_Resources.size(); i < len; i++){
      m_Resources[i]->add(resource_velocity.segment(current_start_index, m_Resources[i]->dim()), timestep);

      current_start_index += m_Resources[i]->dim();
    }
  }

  void CompositeResource::set(const FloatVector &pos) {
    unsigned int current_start_index = 0;

    for (unsigned int i = 0, len = m_Resources.size(); i < len; i++){
      m_Resources[i]->set(pos.segment(current_start_index, m_Resources[i]->dim()));

      current_start_index += m_Resources[i]->dim();
    }
  }

  const FloatVector &CompositeResource::get_resource_vel() {
    return m_ResourceValueVelocity;
  }

  const FloatVector &CompositeResource::get() {
    return m_ResourceValue;
  }

  void CompositeResource::set(const unsigned int resource_index, const FloatVector &pos)
  {
    assert(resource_index<m_Resources.size());
    m_Resources[resource_index]->set(pos);
  }

  void CompositeResource::set(const unsigned int resource_index, const FloatVector &pos, const FloatVector &vel)
  {
    assert(resource_index<m_Resources.size());
    m_Resources[resource_index]->update(pos, vel);
  }

  const FloatVector &CompositeResource::get(const unsigned int resource_index)
  {
    assert(resource_index<m_Resources.size());
    return m_Resources[resource_index]->get();
  }

  const FloatVector &CompositeResource::get_resource_vel(const unsigned int resource_index)
  {
    assert(resource_index<m_Resources.size());
    return m_Resources[resource_index]->get_resource_vel();
  }



#ifdef CBF_HAVE_XSD
	CompositeResource::CompositeResource(const CBFSchema::CompositeResource &xml_instance, ObjectNamespacePtr object_namespace) :
		Resource(xml_instance, object_namespace)
	 {
			std::vector<ResourcePtr> resources;
		
			//! Instantiate the subordinate resources
			CBFSchema::CompositeResource::Resource1_const_iterator it;
			for (
				it = xml_instance.Resource1().begin(); 
				it != xml_instance.Resource1().end();
				++it
			)
			{
				ResourcePtr tr(XMLObjectFactory::instance()->create<Resource>(*it, object_namespace));
				resources.push_back(tr);
				//tr->set_resource(ResourcePtr(new DummyResource(tr->get_resource_dim())));
			}
		
			set_resources(resources);
			CBF_DEBUG("dim: " << dim());
	}
	static XMLDerivedFactory<CompositeResource, CBFSchema::CompositeResource> x;

#endif

} // namespace



