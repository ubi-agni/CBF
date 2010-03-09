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

#ifndef CBF_AVOID_SING_HH
#define CBF_AVOID_SING_HH

#include <cbf/primitive_controller.h>

CBF_PLUGIN_PREAMBLE(AvoidSingularitiesController)

namespace CBF {

/**
	This controller is a hack. You only need to specify a sensor transform that
	provides a jacobian. This controller then takes the current resource values,
	wiggles them a bit, reevaluates the jacobian and then determines
	det(J# J) where J is the jacobian, and J# its pseudo inverse.. Note that
	J and J# depend on the current resource values q.
*/
struct AvoidSingularitiesController : public PrimitiveController {
	CBF_PLUGIN_DECL_METHODS(AvoidSingularitiesController)

	FloatMatrix m_JacTimesJacTranspose;

	Float m_SamplingStep;

	AvoidSingularitiesController() :
		m_SamplingStep(0.01) 
	{

	}

	virtual FloatVector &subordinate_step(ublas::vector<Float> &result);
};

} // namespace

#endif

