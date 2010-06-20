/*
    This file is part of CBF.

    CBF is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    CBF is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CBF.  If not, see <http://www.gnu.org/licenses/>.


    Copyright 2009, 2010 Florian Paul Schmidt
*/

#include <cbf/c_api.h>

#include <stdlib.h>
#include <stdio.h>

#define MAX_NUMBER_OF_STEPS 5000

int
main(int argc, char **argv) {
	cbf_init();

	//! Create controller from XML file...
	struct cbf_primitive_controller c;

	if (cbf_controller_create_from_file(&c, argv[1]) == 0)
	{
		printf("Error constructing controller. Exiting...\n");
		exit(EXIT_FAILURE);
	}

	//! Create placeholders for holding resource values and update steps...
	double *in  = calloc(cbf_controller_get_resource_dim(&c), sizeof(double));

	int i;

	double *out = calloc(cbf_controller_get_resource_dim(&c), sizeof(double));

	//! Execute controller some million times..
	for (i = 0; i < MAX_NUMBER_OF_STEPS; ++i)
	{
		cbf_controller_step(&c, in, out);

		int j;
		for (j = 0; j < cbf_controller_get_resource_dim(&c); ++j)
			in[j] += out[j];
	}

	cbf_controller_destroy(&c);
}

