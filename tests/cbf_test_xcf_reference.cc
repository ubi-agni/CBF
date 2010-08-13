#include <cbf/xcf_vector_reference.h>

#include <iostream>

using namespace CBF;

int main() {
	XCFVectorReferencePtr r(new XCFVectorReference("CBFReference", 5));

	while(true) {
		r->update();

		if (r->get().size() != 0)
			std::cout << r->get()[0] << std::endl;
	}

}

