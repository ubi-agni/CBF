#include <cbf/plugin_pool.h>
#include <cbf/controller.h>

#include <string>
#include <stdexcept>

#include <schemas.hxx>

int main(int argc, char *argv[]) {
	if (argc < 2) throw std::runtime_error
		("Too few arguments. Please be a bit more verbose!");

	for (int i = 1; i < argc; ++i) {
		std::string filename(argv[i]);

		CBF::PluginPool<CBF::Controller> *pp = 
			CBF::PluginPool<CBF::Controller>::get_instance();

		CBF::ControllerPtr c = 
			pp->create_from_file<ControllerType>(filename);

		c->step();
	}

	return 0;
}
