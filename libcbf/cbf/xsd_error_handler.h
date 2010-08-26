#ifndef CBF_XSD_ERROR_HANDLER_HH
#define CBF_XSD_ERROR_HANDLER_HH

#ifdef CBF_HAVE_XSD
	#include <cbf/schemas.hxx>
#endif

namespace CBF {

#ifdef CBF_HAVE_XSD

	/**
		This error handler can be used in conjunction with
		the generated serialization/deserialization methods
		to create C++ types from XML documents
	*/
	class XSDErrorHandler : public xml_schema::error_handler
	{
		virtual bool handle (const std::basic_string<char>& id,
			unsigned long line,
			unsigned long column,
			severity,
			const std::basic_string<char>& message) {
				std::cerr 
					<< "[ParsingErrorHandler]: \"" 
					<< message <<"\"" 
					<< " in line: " << line 
					<< ", column: " << column 
					<< std::endl;

				return false;
			}
	};
#endif

} // namespace

#endif
