#include <cbf/masking_resource.h>
#include <cbf/xml_object_factory.h>

namespace CBF {

#ifdef CBF_HAVE_XSD

MaskingResource::MaskingResource(const CBFSchema::MaskingResource &xml_instance, ObjectNamespacePtr object_namespace) {
	ResourcePtr resource 
		= XMLObjectFactory::instance()->create<Resource>
			(xml_instance.Resource1(), object_namespace);

	std::vector<unsigned int> indexes;

	for 
		(CBFSchema::MaskingResource::Index_const_iterator it = xml_instance.Index().begin();
		 it != xml_instance.Index().end(); 
		 ++it)
	{
			indexes.push_back(*it);
	}

	set_resource_and_indexes(resource, indexes);
}

MaskingResource::MaskingResource(ResourcePtr resource, std::vector<unsigned int> indexes) {
  set_resource_and_indexes(resource, indexes);
}

void MaskingResource::set_resource_and_indexes(ResourcePtr resource, std::vector<unsigned int> indexes)
{
  m_Resource = resource;
  m_Indexes  = indexes;

  for (unsigned int i = 0; i < m_Indexes.size(); ++i) {
    if (m_Indexes[i] > m_Resource->dim())
      throw std::runtime_error("Index out of bounds");
  }

  m_ResourceValue = FloatVector(m_Indexes.size());
  m_ResourceValueVelocity = FloatVector(m_Indexes.size());

  m_IndexResourceValue = FloatVector(m_Resource->dim());
  m_IndexResourceValueVelocity = FloatVector(m_Resource->dim());
}

void MaskingResource::read()
{
  m_Resource->read();

  for (unsigned int i = 0, len = m_Indexes.size(); i < len; ++i) {
    m_ResourceValue[i] = m_Resource->get_position()[m_Indexes[i]];
    m_ResourceValueVelocity[i] = m_Resource->get_velocity()[m_Indexes[i]];
  }
}

void MaskingResource::write(const FloatVector &vel, const Float timestep)
{
  m_IndexResourceValueVelocity.setZero();
  for (unsigned int i = 0, len = m_Indexes.size(); i < len; ++i) {
    m_IndexResourceValueVelocity[m_Indexes[i]] = vel[i];
  }

  m_Resource->write(m_IndexResourceValueVelocity, timestep);
}

static XMLDerivedFactory<MaskingResource, CBFSchema::MaskingResource> x;

#endif

} // namespace
