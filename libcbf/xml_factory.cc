#include <cbf/xml_object_factory.h>
#include <cbf/xml_factory.h>

namespace CBF {
	#ifdef CBF_HAVE_XSD

		template <> XMLFactory<Object> 
			*XMLFactory<Object>::m_Instance = 0;

	#endif
} // namespace

