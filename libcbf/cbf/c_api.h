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

/* -*- mode: c-non-suck; -*- */

#ifndef CBF_C_BINDINGS_1212121_HH
#define CBF_C_BINDINGS_1212121_HH

/**
	This struct is intended to be an opaque container holding
	a handle to the real controller. At this time only PrimitiveControllers
	are supported.
*/
struct cbf_primitive_controller {
	/**
		This is a pointer to a boost::shared_ptr<PrimitiveController>. Do not mess 
		with this from within C.
	*/
	void *controller_ptr;
};

#ifdef __cplusplus
extern "C" {
#endif

void cbf_init();

/**
	This function creates a controller from a file representing an XML infoset. The parameter
	c needs to be pointing to a preallocated struct cbf_primitive_controller.

	Return 0 on failure. Else the memory pointed to by c is filled with appropriate data 
	and returned.
*/
struct cbf_primitive_controller*
cbf_controller_create_from_file(
	struct cbf_primitive_controller *c, 
	const char *filename
);

/**
	This function creates a file from a memory region holding a text representation
	of an XML infoset.

	Otherwise this function behaves as create_controller_from_file().

	TODO: Implement!!
*/
struct cbf_primitive_controller*
cbf_controller_create_from_memory(
	struct cbf_primitive_controller *c, 
	const char *mem
);


/**
	Fills a preallocated struct cbf_primitive_controller with a pointer that refers
	to the index-th subordinate controller.

	Arguments:
		pc - Pointer to a struct cbf_primitive controller of which a subordinate controller
		spc - Pointer to a struct cbf_primitive_controller that will hold a reference to the subordinate controller

	CAUTION:
	
	The pointer is only legal to use as long as the original controller is
	legal to use.

	Returns the pointer to the struct primitve_controller that was passed in as spc..
*/
struct cbf_primitive_controller*
cbf_controller_get_subordinate_controller(
	struct cbf_primitive_controller *pc, 
	struct cbf_primitive_controller *spc, 
	int index
);


/**
	Returns 1 if the controller is finished (converged). Returns 0 if the controller
	is not converged. 

	CAUTION:

	Precondition: The controller has to have been stepped at least once before thie test returns
	sensible values
*/
int
cbf_controller_is_finished(struct cbf_primitive_controller *pc);

/**
	Fills out with the current task space position represented as a double[]. Make
	sure out points to valid memory!

	NOTE: The controller has to be run (step() or do_step()) at least once before this function returns
	sensible values.

	Returns -1 on error..
*/
int 
cbf_controller_get_current_task_position(
	struct cbf_primitive_controller *c, 
	double *out
);

/**
	Returns the dimension of the resource of the controller..

	Returns -1 on error
*/
int
cbf_controller_get_resource_dim(
	struct cbf_primitive_controller *c
);

/**
	Sets the reference of the controller (the point in task space that the controller
	eventually should converge to.. The C++ interface allows to set several references
	and the controller is expected to converge to the closest one, but for simplicity
	this function only exposes a single reference point via this C interface.

	Returns -1 on error..
*/
int
cbf_controller_set_reference(
	struct cbf_primitive_controller* c, 
	double *reference
);

int 
cbf_controller_get_reference(
	struct cbf_primitive_controller* c, 
	double *reference
);


/**
	@brief Set current resource values (e.g. joint angles of a robotic arm) corresponding to the sensor transform

	Arguments:

	c - The struct cbf_primitive_controller thhat is referenced
	resource_out - an array of doubles holding values to be passed

	Returns -1 on failure
*/
int 
cbf_controller_set_resource(
	struct cbf_primitive_controller *c, 
	double *resource_in
);

/**
	@brief Set current resource values (e.g. joint angles of a robotic arm) corresponding to the sensor transform

	Arguments:

	c - The struct cbf_primitive_controller thhat is referenced
	resource_out - a preallocated array of doubles to be filled with the resource values

	Returns -1 on failure
*/
int 
cbf_controller_get_resource(
	struct cbf_primitive_controller *c, 
	double *resource_out
);


/**
	Run a single step of a controller. This function reads the current resource values from 
	the parameter in and writes the result into the memory pointed to by out.

	This function does not update the resource contained in the controller..

	Returns -1 on failure.
*/
int
cbf_controller_step(
	struct cbf_primitive_controller *c, 
	double *in, 
	double *out
);


/**
	This function steps the controller.. This function is self contained in the sense
	that the controller's resource is updated automatically. 

	TODO: Implement!!
*/
int
cbf_controller_step_internal(
	struct cbf_primitive_controller *c
);

/**
	Destroy the controller wrapped by the struct cbf_primitive_controller pointed to by c.

	This does not free() the struct itself. This is left to the user.
*/
struct cbf_primitive_controller*
cbf_controller_destroy(
	struct cbf_primitive_controller *c
);

#ifdef __cplusplus
}
#endif


#endif

