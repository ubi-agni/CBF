/* -*- mode: c-non-suck; -*- */

#include <cbf/c_bindings.h>

#include <cbf/config.h>
#include <schemas.hxx>
#include <cbf/controller.h>
#include <cbf/primitive_controller.h>
#include <cbf/dummy_resource.h>
#include <cbf/debug_macros.h>
#include <cbf/dummy_reference.h>
#include <algorithm>

void init_cbf() {

}

struct primitive_controller*
create_controller_from_file(
	struct primitive_controller *c, 
	const char *filename)
{
	CBF::PrimitiveControllerPtr *p = 0;
	try {
		//! Create an empty PrimitiveControllerPtr..
		p = new CBF::PrimitiveControllerPtr();

		CBF_DEBUG("[create_controller_from_file]: Creating ControllerType instance")
		//! Create ControllerType instance from xml file...
		std::auto_ptr<ControllerType> xml_instance (
			Controller(
				filename,
				xml_schema::flags::dont_validate
			)
		);
	
		CBF_DEBUG("[create_controller_from_file]: Creating controller...")

		//! Finally let's create the controller instance..
		CBF::ControllerPtr controller = 
			CBF::PluginPool<CBF::Controller>::get_instance()->create_from_xml(*xml_instance);

		CBF_DEBUG("[create_controller_from_file]: Checking if it's a PrimitiveController")
		//! Checking whether the created controller is a PrimitiveController.
		*p = boost::dynamic_pointer_cast<CBF::PrimitiveController>(controller);

		//! If not, delete our temporary and return 0
		if ((*p).get() == 0) {
			delete p;
			return 0;
		}

		//! Otherwise finally set the controller_ptr field in the passed struct.
		c->controller_ptr = (void *)p;
	}
	catch (const xml_schema::exception& e) 
	{
		CBF_DEBUG("[create_controller_from_file]: Some error happened: during parsing " << e)
		delete p;

		//! Rethrow the exception, so a debugger might catch it :)
		return 0;
	}
	catch(...) {
		CBF_DEBUG("[create_controller_from_file]: Some error happened")
		delete p;

		return 0;
	}
	return c;
}

struct primitive_controller*
create_controller_from_memory(struct primitive_controller *c, const char *mem)
{
	CBF_DEBUG("[create_controller_from_memory]: Not implemented yet")
	return c;
}

int
controller_get_resource_dim(struct primitive_controller *c) {
	CBF::PrimitiveControllerPtr *p = ((CBF::PrimitiveControllerPtr*)c->controller_ptr);
	try {
		//! Check whether the controller contains a dummy resource..
		CBF::DummyResourcePtr res = boost::dynamic_pointer_cast<CBF::DummyResource>((*p)->sensor_transform()->resource());
		if (res.get() == 0) {
			CBF_DEBUG("[step_controller]: No dummy resource found in controller")
			return 0;
		}

		//! Copy result over into out array...
		return res->dim();
	}
	catch (...)
	{
		CBF_DEBUG("[controller_get_resource_dim]: Something went wrong")
		return -1;
	}

}


int
controller_set_reference(struct primitive_controller* c, double *reference) {
	CBF::PrimitiveControllerPtr *p = ((CBF::PrimitiveControllerPtr*)c->controller_ptr);

	boost::shared_ptr<CBF::DummyReference> d = boost::dynamic_pointer_cast<CBF::DummyReference, CBF::Reference>((*p)->reference());

	if (d.get() == 0) {
		CBF_DEBUG("[controller_set_reference]: controlle_set_reference only works with a reference of type CBF::DummyReference")
		return -1;
	}

	std::copy(reference, reference + d->references()[0].size(), d->references()[0].begin());

	return 1;
}

int
controller_get_reference(struct primitive_controller* c, double *reference) {
	CBF::PrimitiveControllerPtr *p = ((CBF::PrimitiveControllerPtr*)c->controller_ptr);

	boost::shared_ptr<CBF::DummyReference> d = boost::dynamic_pointer_cast<CBF::DummyReference, CBF::Reference>((*p)->reference());

	if (d.get() == 0) {
		CBF_DEBUG("[controller_set_reference]: controlle_set_reference only works with a reference of type CBF::DummyReference")
		return -1;
	}

	std::copy(d->references()[0].begin(), d->references()[0].end(), reference);

	return 1;
}


int
step_controller(struct primitive_controller *c, double *in, double *out)
{
	CBF::PrimitiveControllerPtr *p = ((CBF::PrimitiveControllerPtr*)c->controller_ptr);
	try {
		//! Check whether the controller contains a dummy resource..
		CBF::DummyResourcePtr res = boost::dynamic_pointer_cast<CBF::DummyResource>((*p)->sensor_transform()->resource());
		if (res.get() == 0) {
			CBF_DEBUG("[step_controller]: No dummy resource found in controller")
			return -1;
		}

		//! Copy data over into the resource (assuming it's a dummy resource)..
		std::copy(in, in+res->dim(), res->m_Variables.begin());

		CBF_DEBUG(res->m_Variables)

		//! A place to store the result...
		boost::numeric::ublas::vector<CBF::Float> result;

		//! Actually perform the step...
		(*p)->subordinate_step(result);

		//! Copy result over into out array...
		std::copy(result.begin(), result.end(), out);
	}
	catch (...)
	{
		CBF_DEBUG("[step_controller]: Something went wrong")
		return -1;
	}

	return 1;
}

struct primitive_controller*
destroy_controller(struct primitive_controller *c)
{
	CBF::PrimitiveControllerPtr *p = ((CBF::PrimitiveControllerPtr*)c->controller_ptr);
	CBF::PrimitiveControllerPtr empty = CBF::PrimitiveControllerPtr();

	*p = empty;

	return c;
}

int 
controller_get_current_task_position(struct primitive_controller *c, double *out)
{
	CBF::PrimitiveControllerPtr *p = ((CBF::PrimitiveControllerPtr*)c->controller_ptr);

	std::copy((*p)->get_current_task_position().begin(),(*p)->get_current_task_position().end(), out);

	return 1;	
}

struct primitive_controller*
controller_get_subordinate_controller(struct primitive_controller *pc, struct primitive_controller *spc, int index) {
	CBF::PrimitiveControllerPtr *p = 
		((CBF::PrimitiveControllerPtr*)pc->controller_ptr);
	
	spc->controller_ptr = 
		(struct primitive_controller*)&((*p)->subordinate_controllers()[index]);

	return spc;
}


int 
controller_set_resource(struct primitive_controller *c, double *resource_in) {
	CBF::PrimitiveControllerPtr *p = 
		((CBF::PrimitiveControllerPtr*)c->controller_ptr);

	int resource_dim = (*p)->sensor_transform()->resource()->dim();

	CBF::FloatVector vec(resource_dim);

	std::copy(
		resource_in, 
		resource_in + resource_dim, 
		vec.begin()
	);

	(*p)->sensor_transform()->resource()->set(vec);

	return 1;
}


int 
controller_get_resource(struct primitive_controller *c, double *resource_out) {
	CBF::PrimitiveControllerPtr *p = 
		((CBF::PrimitiveControllerPtr*)c->controller_ptr);


	std::copy(
		(*p)->sensor_transform()->resource()->get().begin(),
		(*p)->sensor_transform()->resource()->get().end(),
		resource_out
	);

	return 1;
}

int
controller_is_finished(struct primitive_controller *pc) {
	CBF::PrimitiveControllerPtr *p = 
		((CBF::PrimitiveControllerPtr*)pc->controller_ptr);

	if ((*p)->finished()) return 1;
	else return 0;
}


