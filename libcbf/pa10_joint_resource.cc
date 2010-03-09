#include <cbf/pa10_joint_resource.h>
#include <cbf/plugin_macros.h>

namespace CBF {
	
	PA10JointResource::Helper PA10JointResource::m_Helper;
	
	const boost::numeric::ublas::vector<Float> &PA10JointResource::get() {
		return m_CurrentPosture;
	}
	
	void PA10JointResource::set(const boost::numeric::ublas::vector<Float> &in) {
		m_CurrentPosture = in;
	}
	
	void PA10JointResource::add(const boost::numeric::ublas::vector<Float> &in) {
		m_CurrentPosture += in;
	}
	
	
	#ifdef CBF_HAVE_XSD
	PA10JointResource::PA10JointResource(const PA10JointResourceType &xml_instance)
	{
	
	}
	#endif
	
	CBF_PLUGIN_CLASS(PA10JointResource, Resource)
	
} // namespace

