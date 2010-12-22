#ifndef CBF_NAMESPACE_HH
#define CBF_NAMESPACE_HH

#include <cbf/object.h>

#include <boost/shared_ptr.hpp>
#include <map>

namespace CBF {
	struct ObjectNamespace {
		typedef std::map<std::string, ObjectPtr> map;
	};

	
} // namespace

#endif
