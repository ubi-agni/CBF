#ifndef CBF_MASKING_RESOURCE
#define CBF_MASKING_RESOURCE

#include <cbf/config.h>
#include <cbf/resource.h>
#include <cbf/plugin_decl_macros.h>

#include <vector>
#include <stdexcept>

CBF_PLUGIN_PREAMBLE(MaskingResource)

namespace CBF {

/** 
	@brief A Resource that wraps another resource and masks some components out.. 

	This can be used to represent a subset of components of another resource. For this 
	purpose this resource contains an arbitrary other resource...
*/

struct MaskingResource : public Resource {
	CBF_PLUGIN_DECL_METHODS(MaskingResource)

	MaskingResource(ResourcePtr masked_resource, std::vector<unsigned int> indexes)
	{
		set_resource_and_indexes(masked_resource, indexes);
	}

	virtual void set_resource_and_indexes(ResourcePtr masked_resource, std::vector<unsigned int> indexes) {
		for (unsigned int i = 0; i < m_Indexes.size(); ++i)
			if (m_Indexes[i] > m_Resource->dim()) 
				throw std::runtime_error("Index out of bounds");

		m_Result = FloatVector(m_Indexes.size());
	}
	
	virtual void update() {
		m_Resource->update();

		for (unsigned int i = 0, len = m_Indexes.size(); i < len; ++i)
			m_Result[i] = m_Resource->get()[m_Indexes[i]];
	}

	virtual void set(const FloatVector &arg) { }

	virtual const FloatVector &get() { 
		return m_Result; 
	}

	virtual void add(const FloatVector &arg) { 
		FloatVector tmp = ublas::zero_vector<Float>(m_Resource->get().size());

		for (unsigned int i = 0, len = m_Indexes.size(); i < len; ++i)
			tmp[m_Indexes[i]] = arg[i];

		m_Resource->add(tmp);
	}

	virtual unsigned int dim() { return 0; }

	protected:
		ResourcePtr m_Resource;
		FloatVector m_Result;
		std::vector<unsigned int> m_Indexes;
};

} // namespace

#endif
