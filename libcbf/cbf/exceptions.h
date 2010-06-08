#ifndef CBF_EXCEPTIONS_HH
#define CBF_EXCEPTIONS_HH

#include <stdexcept>
#include <sstream>

/**
	@brief A utility macro to make throwing meaningful exceptions easier..

	Note: This macro creates a local variable called cbf_error_stringstream. So do
	not use this macro if you have a local variable with that name.
*/
#define CBF_THROW_RUNTIME_ERROR(x) { \
	std::stringstream cbf_error_stringstream; \
	cbf_error_stringstream <<  """[" << __FILE__ << ":" << __LINE__ << " - " << __FUNCTION__ << "()]: " << x; \
	throw std::runtime_error(cbf_error_stringstream.str()); \
}

#endif
