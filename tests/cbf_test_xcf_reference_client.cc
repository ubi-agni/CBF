#include <cbf/types.h>
#include <xcf/RemoteServer.hpp>
#include <cbf/schemas.hxx>
#include <cbf/debug_macros.h>

#include <iostream>
#include <sstream>

using namespace CBF;

int main() {
	CBF_DEBUG("creating remote server object")
	XCF::RemoteServerPtr _remoteServer = XCF::RemoteServer::create("CBFReference");
 		
	while(true) {
		CBF_DEBUG("creating vector doc")
		BoostVectorType v("[5](1,1,1,1,1)");
		std::ostringstream s;
		Vector (s, v);

		CBF_DEBUG(s.str())

		std::string out;

		CBF_DEBUG("calling remote method")
		_remoteServer->callMethod("set_reference", s.str(), out);
	}
}

