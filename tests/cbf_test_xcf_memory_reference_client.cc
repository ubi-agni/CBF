#include <cbf/types.h>
#include <cbf/debug_macros.h>
#include <cbf/utilities.h>
#include <cbf/exceptions.h>

#include <cbf/schemas.hxx>

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
	@brief This app can be used to send XCFMemoryReferenceVectors to an active_memory.
	Usage: cbf_test_xcf_memory_reference_client 'memory_server-uri' 'ResourceName'
*/
int main(int argc, char **argv) {
	std::string mname;
	std::string rname;
	if (argc < 3) {
		mname = "xcf:wb";
		rname = "CBFMemoryReference";
	} else {
		mname = argv[1];
		rname = argv[2];
	}


	CBF_DEBUG("connecting to XCFMemory...");
	mi::MemoryInterface::pointer memoryPtr(mi::MemoryInterface::getInstance(mname));

	std::stringstream xPath;
	xPath << "//*[local-name() = 'XCFMemoryReferenceInfo' and ";
	xPath << "namespace-uri() = 'http://www.cit-ec.uni-bielefeld.de/CBF']";
	xPath << "/ReferenceName[text() = '" << rname << "']";

	mi::ResultsPtr results = memoryPtr -> query(xPath.str());

	std::string document;
	std::vector<int> dimensions;
	std::auto_ptr<CBFSchema::XCFMemoryReferenceInfo> reference;
	while(results -> hasNext()){
		results -> next(document);
		try{
			CBF_DEBUG("Parsing as XCFmemoryReferenceInfo: " << document);
			std::istringstream s(document);
			reference = CBFSchema::XCFMemoryReferenceInfo_(s, xml_schema::flags::dont_validate);
			dimensions.push_back(reference -> Dimension());
		} catch (xml_schema::exception& e){
			CBF_DEBUG("Could not parse document: " << e.what());
		} catch (...){
			CBF_DEBUG("Could not parse document. Trying next.");
		}
	}

	if (dimensions.size() == 0){
		CBF_DEBUG("XPath: " << xPath.str());
		CBF_THROW_RUNTIME_ERROR("Could not find an XCFMemoryReferenceInfo with the specified name on the memory.");
	}

	if (dimensions.size() > 1){
		int firstDimension = dimensions.at(0);
		std::vector<int>::const_iterator it;
		for (it = dimensions.begin(); it != dimensions.end(); ++it){
			if (*it != firstDimension){
				CBF_DEBUG("XPath: " << xPath.str());
				CBF_THROW_RUNTIME_ERROR("Found multiple XCFMemoryReferences with the specified name "
									"and different dimensions on this memory");
			}
		}
	}
	ObjectNamespacePtr object_namespace(new ObjectNamespace);

	CBF_DEBUG("dimension: " << dimensions.at(0));
	
	unsigned int dim = dimensions.at(0);

	while(true) {
		std::cout << "Enter " << dim << " values" << std::endl;

		std::stringstream vector_string;

		CBF::FloatVector tmp(dim);
		CBF::Float value;
		for (unsigned int i = 0; i < dim; ++i) {
			std::cin >> value;
			tmp(i) = value;
		}

		// creating the XCFMemoryReferenceVector document
		std::stringstream vecstr;
		vecstr << tmp;
		CBFSchema::XCFMemoryReferenceVector v(rname, CBFSchema::EigenVector(vecstr.str()));

		std::ostringstream s;
		CBFSchema::XCFMemoryReferenceVector_ (s, v);

		CBF_DEBUG("document: " << s.str());


		CBF_DEBUG("Sending document");
		memoryPtr -> insert(s.str());
	}
}

