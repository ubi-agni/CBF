#include <cbf/types.h>
#include <xcf/RemoteServer.hpp>
#include <cbf/schemas.hxx>
#include <cbf/debug_macros.h>
#include <cbf/utilities.h>
#include <cbf/xml_factory.h>

#include <boost/numeric/ublas/io.hpp>

#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <memory>

using namespace CBF;

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cout << "Usage: cbf_test_xcf_reference_client name" << std::endl;
		exit (EXIT_FAILURE);
	}

	CBF_DEBUG("creating remote server object");
	XCF::RemoteServerPtr _remoteServer = XCF::RemoteServer::create(argv[1]);

	std::string dim_string;
	_remoteServer->callMethod("get_dimension", "", dim_string);
	CBF_DEBUG("dimension_xml: " << dim_string);

	std::istringstream vv_stream(dim_string);

	std::auto_ptr<CBFSchema::Vector> dim_v = CBFSchema::Vector_(vv_stream, xml_schema::flags::dont_validate);

	CBF::ObjectNamespacePtr object_namespace(new CBF::ObjectNamespace);

	CBF::FloatVector dim_vv = *CBF::XMLFactory<FloatVector>::instance()->create(*dim_v, object_namespace);
	CBF_DEBUG("dim_vv: " << dim_vv);
	
	unsigned int dim;

	dim = dim_vv[0];

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
		CBFSchema::BoostVector v(vector_string.str());

		std::ostringstream s;
		//s << v;
		CBFSchema::Vector_ (s, v);

		CBF_DEBUG("document: " << s.str());

		std::string out;

		CBF_DEBUG("calling remote method");
		_remoteServer->callMethod("set_reference", s.str(), out);
	}
}

