#include <cbf/xml_factory.h>

namespace CBF {

	#ifdef CBF_HAVE_XSD

		XMLObjectFactory* 
			XMLObjectFactory::m_Instance = 0;

	#endif

} // namespace

