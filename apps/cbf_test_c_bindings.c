#include <cbf/c_bindings.h>

#include <stdlib.h>
#include <stdio.h>

#define MAX_NUMBER_OF_STEPS 5000

int
main(int argc, char **argv) {
	init_cbf();

	//! Create controller from XML file...
	struct primitive_controller c;
	if (create_controller_from_file(&c, argv[1]) == 0)
	{
		printf("Error constructing controller. Exiting...\n");
		exit(EXIT_FAILURE);
	}

	//! Create placeholders for holding resource values and update steps...
	double *in  = calloc(controller_get_resource_dim(&c), sizeof(double));

	int i;

#if 0
	for (i = 0; i < controller_get_resource_dim(&c); ++i) {
		in[i] = 0.1;
	}
#endif

	double *out = calloc(controller_get_resource_dim(&c), sizeof(double));

	//! Execute controller some million times..
	for (i = 0; i < MAX_NUMBER_OF_STEPS; ++i)
	{
		step_controller(&c, in, out);

		int j;
		for (j = 0; j < controller_get_resource_dim(&c); ++j)
			in[j] += out[j];
	}

	destroy_controller(&c);
}

