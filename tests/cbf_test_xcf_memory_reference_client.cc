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

int main(int argc, char **argv) {
	if (argc < 3) {
		std::cout << "Usage: cbf_test_xcf_memory_reference_client "
				"'memory_server-uri' 'ReferenceName'" << std::endl;
		exit (EXIT_FAILURE);
	}


	CBF_DEBUG("connecting to XCFMemory")
	mi::MemoryInterface::pointer memoryPtr(mi::MemoryInterface::getInstance(argv[1]));

	std::stringstream xPath;
//	xPath << "/p1:XCFMemoryReference/ReferenceName['" << argv[2] << "']";
	xPath << "/";

	memoryPtr -> addNamespacePrefix("xmlns:p1", "http://www.cit-ec.uni-bielefeld.de/CBF");
	mi::ResultsPtr results = memoryPtr -> query(xPath.str(), mi::MemoryInterface::LazyEvaluation);

	std::string document;	
	if(results -> hasNext()){
		results -> next(document);
	} else {
		CBF_DEBUG("XPath: " << xPath.str())
		CBF_THROW_RUNTIME_ERROR("Could not find a xcf_memory_reference with the specified name "
					"on the memory_server.");
	}

	xmltio::XPath referenceVectorPath("/p1:XCFMemoryReference/Vector/String");
	xmltio::Location referenceVectorLoc(document, referenceVectorPath);

	std::istringstream s(document);
	std::auto_ptr<CBFSchema::XCFMemoryReference> reference = 
				CBFSchema::XCFMemoryReference_(s, xml_schema::flags::dont_validate);

	CBF_DEBUG("dimension: " << create_vector(reference -> Vector()))
	
	unsigned int dim = (create_vector(reference -> Vector())).size();

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

		referenceVectorLoc = vector_string.str();

		CBF_DEBUG("document: " << referenceVectorLoc.getDocument())


		CBF_DEBUG("Replacing document")
		memoryPtr -> insert(referenceVectorLoc.getDocumentText());
	}
}

