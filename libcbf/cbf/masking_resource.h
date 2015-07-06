#ifndef CBF_MASKING_RESOURCE
#define CBF_MASKING_RESOURCE

#include <cbf/config.h>
#include <cbf/namespace.h>
#include <cbf/resource.h>

#include <vector>
#include <stdexcept>

namespace CBFSchema { class MaskingResource; }

namespace CBF {

/** 
	@brief A Resource that wraps another resource and masks some components out.. 

	This can be used to represent a subset of components of another resource. For this 
	purpose this resource contains an arbitrary other resource...
*/

struct MaskingResource : public Resource {
	MaskingResource(const CBFSchema::MaskingResource &xml_instance, ObjectNamespacePtr object_namespace);

  MaskingResource(ResourcePtr resource, std::vector<unsigned int> indexes);

  void read();

  void write(const FloatVector &vel, const Float timestep);

//  void update();

//  void add(const FloatVector &resource_step, const Float timestep);

//  void set(const FloatVector &pos);

//  const FloatVector &get_resource_vel();

//  const FloatVector &get();

	protected:
    ResourcePtr m_Resource;
    std::vector<unsigned int> m_Indexes;

    FloatVector m_IndexResourceValue;
    FloatVector m_IndexResourceValueVelocity;

    void set_resource_and_indexes(ResourcePtr masked_resource, std::vector<unsigned int> indexes);
};

} // namespace

#endif
