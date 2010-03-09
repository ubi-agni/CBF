#include <cbf/controller.h>
#include <cbf/plugin_pool.h>

//! This is the include generated from the schema controller.xsd by XSD:
#include <schemas.hxx>

#include <iostream>

//! The number of runs..
#define DEFAULT_RUNS 1
#define DEFAULT_MAX_STEPS 1000
unsigned int runs = DEFAULT_RUNS;
unsigned int max_steps = DEFAULT_MAX_STEPS;


class ParsingErrorHandler : public xml_schema::error_handler
{
	virtual bool handle (const std::basic_string<char>& id,
		unsigned long line,
		unsigned long column,
		severity,
		const std::basic_string<char>& message) {
			std::cout << "\"" << message <<"\"" << " in line: " << line << ", column: " << column << std::endl;
			return false;
		}
};

ParsingErrorHandler err_handler;

void print_usage() {
	std::cout << "Usage: test_recipes [options] <recipe1> <recipe2> ..." << std::endl;
	std::cout << "Legal options are: " << std::endl;
	std::cout << "-r <runs>     Number of runs of the controllers (default: " << DEFAULT_RUNS << ")" << std::endl;
	std::cout << "-m <steps>    Maximum number of steps to run controllers (default: " << DEFAULT_MAX_STEPS << ")" << std::endl;
}

/**
	We expect one or more strings each holding a recipe. Then these recipes
	are executed until each converges.
*/
int main(int argc, char *argv[]) {
	if (argc < 2) {
		print_usage();
		exit(EXIT_FAILURE);
	}

	//! Find first argument that does not belong to an option
	int first_recipe = 1;
	while(first_recipe < argc && argv[first_recipe][0] == '-')
	{
		if (argv[first_recipe + 1] == 0) {
			print_usage();
			exit(EXIT_FAILURE);
		}

		if (std::string(argv[first_recipe]) == "-r")
		{
			runs = atoi(argv[first_recipe + 1]);
		}
		else if (std::string(argv[first_recipe]) == "-m")
		{
			max_steps = atoi(argv[first_recipe + 1]);
		}
		else
		{
			print_usage();
			exit(EXIT_FAILURE);
		}
		
		//std::cout << "option: " << argv[first_recipe] << std::endl;
		first_recipe += 2;
	}

	//! Sanity check
	if (first_recipe >= argc) {
		print_usage();
		exit(EXIT_FAILURE);
	}

	// CBF::Controller::initialize_creator_functions();

	for (int arg = first_recipe; arg < argc; ++arg) {
		for (unsigned int i = 0; i < runs; ++i) {
			try
			{
				std::cout << "- Reading and parsing recipe..." << std::endl;
				std::auto_ptr<ControllerType> xml_instance (
					Controller(
						argv[arg],
						err_handler,
						xml_schema::flags::dont_validate
					)
				);
			
				std::cout << "- Cooking controller..." << std::endl;
				//! Finally let's create the controller instance..
				CBF::ControllerPtr ptr = CBF::PluginPool<CBF::Controller>::get_instance()->create_from_xml(*xml_instance);
			
				//std::cout << *xml_instance << std::endl;
			
				std::cout << "- Tasting controller \"" << argv[arg] << "\" [one dot is one step]:" << std::endl;
				//! Run controller until convergence..
				long long int steps = 0;
				do  { 
					#ifdef CBF_NDEBUG
						std::cout << ".";
					#else
						std::cout << "." << std::endl;
					#endif
					ptr->step(); ++steps;
				} while(!ptr->finished() && steps < max_steps);
				std::cout << std::endl << "- Steps: " << steps << std::endl;

			}
			catch (const xml_schema::exception& e) 
			{
				std::cerr << "Something went wrong instantiating the controller from file \"" << argv[arg] << "\": Reason: \n" << e << std::endl;
		
				//! Rethrow the exception, so a debugger might catch it :)
				throw;
			}
		}
	}
}

/**
	\example simple.xml 

	This example tests some KDL functionality (simple.xml)
*/

