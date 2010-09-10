#include <cbf/types.h>
#include <xcf/RemoteServer.hpp>
#include <cbf/schemas.hxx>
#include <cbf/debug_macros.h>

#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>

using namespace CBF;

int main(int argc, char **argv) {
	if (argc < 3) {
		std::cout << "Usage: cbf_test_xcf_reference_client name dimension" << std::endl;
		exit (EXIT_FAILURE);
	}

	CBF_DEBUG("creating remote server object")
	XCF::RemoteServerPtr _remoteServer = XCF::RemoteServer::create(argv[1]);

	std::stringstream str(argv[2]);
	unsigned int dim;
	str >> dim;

	while(true) {
		std::cout << "Enter " << dim << " values" << std::endl;

		std::string temp;
		std::cin >> temp;

		std::cout << "Entered: " << temp << std::endl;
		
		std::stringstream vector_string;
		std::stringstream user_input(temp);

		vector_string << "[" << dim << "](";
		for (unsigned int i = 0; i < dim; ++i) {
			float tmp;
			user_input >> tmp;
			vector_string << tmp;
			if (i != dim - 1) vector_string << ",";
		}
		vector_string << ")";

		CBF_DEBUG("creating vector doc")
		BoostVectorType v(vector_string.str());

		std::ostringstream s;
		Vector (s, v);

		CBF_DEBUG(s.str())

		std::string out;

		CBF_DEBUG("calling remote method")
		_remoteServer->callMethod("set_reference", s.str(), out);
	}
}

