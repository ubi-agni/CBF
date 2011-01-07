#include <cbf/types.h>
#include <cbf/schemas.hxx>
#include <cbf/debug_macros.h>
#include <cbf/utilities.h>
#include <cbf/exceptions.h>

#include <boost/numeric/ublas/io.hpp>

#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <memory>

#include <Memory/Interface.hpp>
#include <xmltio/Location.hpp>
#include <xmltio/XPath.hpp>

using namespace CBF;
namespace mi = memory::interface;

/**
	@brief This app can be used to call the 'add' event on an XCFMemoryResource.
	Usage: cbf_test_xcf_memory_reference_client 'memory_server-uri' 'ResourceName'
*/
int main(int argc, char **argv) {
	if (argc < 3) {
		std::cout << "Usage: cbf_test_xcf_memory_reference_client "
				"'memory_server-uri' 'ResourceName'" << std::endl;
		exit (EXIT_FAILURE);
	}


	CBF_DEBUG("connecting to XCFMemory");
	mi::MemoryInterface::pointer memoryPtr(mi::MemoryInterface::getInstance(argv[1]));

	std::stringstream xPath;
//	xPath << "/*[local-name()='p1' and namespace-uri()='http://www.cit-ec.uni-bielefeld.de/CBF']";
//	xPath << "/p1:XCFMemoryResourceState/ResourceName['" << argv[2] << "']";
	xPath << "/";

	memoryPtr -> addNamespacePrefix("xmlns:p1", "http://www.cit-ec.uni-bielefeld.de/CBF");

	mi::ResultsPtr results = memoryPtr -> query(xPath.str(), 
			memory::interface::Event::INSERT | memory::interface::Event::REPLACE);

	std::string document;
	bool noDocumentFound = true;
	std::auto_ptr<CBFSchema::XCFMemoryResourceState> resourceState;
	while(noDocumentFound && (results -> hasNext())){ 
		results -> next(document);
		try{
			CBF_DEBUG("Parsing as XCFMemoryResurceState: " << document);
			std::istringstream s(document);
			resourceState = CBFSchema::XCFMemoryResourceState_(s, xml_schema::flags::dont_validate);
			noDocumentFound = false;
		} catch (...){
			CBF_DEBUG("Could not parse document. Trying next.");
		}
	}

	unsigned int dim;
	if (noDocumentFound){
		std::cout << "Please enter the resource-dimension" << std::endl;
		std::cin >> dim;
	} else {
		CBF_DEBUG("dimension: " << create_vector(resourceState -> Vector()));
		dim = (create_vector(resourceState -> Vector())).size();
	}

	while(true) {
		std::cout << "Enter " << dim << " values" << std::endl;

		std::stringstream vector_string;

		vector_string << "[" << dim << "](";

		for (unsigned int i = 0; i < dim; ++i) {
			float tmp;
			std::cin >> tmp;
			vector_string << tmp;
			if (i != dim - 1) vector_string << ",";
		}
		vector_string << ")";

		CBF_DEBUG("creating vector doc");
		CBFSchema::BoostVector vectorDoc(vector_string.str());

		CBF_DEBUG("creating XCFMemoryResourceAdd doc");
		CBFSchema::XCFMemoryResourceAdd resourceAdd(argv[2], vectorDoc);

		std::ostringstream s;
		CBFSchema::XCFMemoryResourceAdd_ (s, resourceAdd);

		CBF_DEBUG("document: " << s.str());

		//Insert the result-XML at the server.
		CBF_DEBUG("insert state at memory-server");
		
		memoryPtr -> insert(s.str());
	}
}

