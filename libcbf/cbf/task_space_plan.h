#ifndef CBF_TASK_SPACE_PLAN_HH
#define CBF_TASK_SPACE_PLAN_HH

#include <cbf/reference.h>

namespace CBF {

/**
	A TaskSpacePlan is a wrapper around a reference which 
	plans smooth trajectories and updates the task space
	reference at each cycle
*/
struct TaskSpacePlan : public Reference {
	/** 
		This is the wrapped reference..
	*/
	ReferencePtr m_Reference;

	/**
		Setting a new reference causes the TaskSpacePlan to
		rethink its planned trajectory to the target
	*/
	virtual void set_new_reference() = 0;

	/**
		This method should do one time shot heavy
		calculations that only need to be performed
		once per cycle..
	*/
	virtual void update() { }
};



} // namespace

#endif
