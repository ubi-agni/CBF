#ifndef CBF_MASKING_RESOURCE
#define CBF_MASKING_RESOURCE

#include <cbf/config.h>
#include <cbf/resource.h>
#include <cbf/plugin_decl_macros.h>

CBF_PLUGIN_PREAMBLE(MaskingResource)

namespace CBF {

/** 
	@brief A Resource that wraps another resource and masks some components out.. 

	This can be used to represent a subset of components of another resource. For this 
	purpose this resource contains an arbitrary other resource...
*/

struct MaskingResource : public Resource {
	CBF_PLUGIN_DECL_METHODS(MaskingResource)

	MaskingResource(ResourcePtr masked_resource = ResourcePtr()) :
		m_Resource(masked_resource)
	{

	}

	virtual void set(const FloatVector &arg) { }

	virtual const FloatVector &get() { return m_Result; }

	virtual void add(const FloatVector &arg) { }

	virtual unsigned int dim() { return 0; }

	protected:
		ResourcePtr m_Resource;

		FloatVector m_Result;
};

} // namespace

#endif
