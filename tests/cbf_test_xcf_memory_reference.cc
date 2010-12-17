#include <cbf/xcf_memory_reference.h>

#include <iostream>
#include <unistd.h>
#include <string>

using namespace CBF;

int main() {
	XCFMemoryReferencePtr r(new XCFMemoryReference("xcf:wb", "CBFMemoryReference", 5));

	while(true) {
		r->update();

		if (r->get().size() != 0)
			std::cout << "current reference: " << r->get()[0] << std::endl;

		usleep(500000);
	}

}

