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

#include <cbf/avoid_singularities.h>
#include <cbf/plugin_impl_macros.h>

#include <cbf/utilities.h>

#ifdef CBF_HAVE_PLUGIN_SUPPORT
#ifdef CBF_HAVE_XSD
	#include <schemas.hxx>
#endif
#endif

namespace CBF {

	FloatVector &AvoidSingularitiesController::subordinate_step(ublas::vector<Float> &result) {
		//! Make sure the sensor values are up to date..

		m_SensorTransform->update();
		m_TaskJacobian = m_SensorTransform->task_jacobian();
		CBF_DEBUG("resource: " << m_SensorTransform->resource()->get())

		unsigned int res_dim = m_SensorTransform->resource()->dim();

		// pseudo_inverse(m_TaskJacobian, m_InverseTaskJacobian);

		m_JacTimesJacTranspose = ublas::prod(
			m_TaskJacobian, ublas::trans(m_TaskJacobian));

		ublas::matrix<Float> ret(res_dim, res_dim);
		Float orig_det = pseudo_inverse(m_JacTimesJacTranspose, ret);

		//CBF_DEBUG(m_JacTimesJacTranspose)

		CBF_DEBUG("det: " << orig_det)

		ublas::vector<Float> res = ublas::zero_vector<Float>(res_dim);
		ublas::vector<Float> resource = m_SensorTransform->resource()->get();

		//! Estimate gradient step to maximize
		for (unsigned int i = 0; i < res_dim; ++i) {
			ublas::vector<Float> new_resource = resource;
			new_resource[i] += m_SamplingStep;
			m_SensorTransform->resource()->set(new_resource);
			//! wiggle a bit on the axis..

			m_SensorTransform->update();	
			m_TaskJacobian = m_SensorTransform->task_jacobian();

			//pseudo_inverse(
			//	m_SensorTransform->task_jacobian(), 
			//	m_InverseTaskJacobian);

			m_JacTimesJacTranspose = ublas::prod(
				m_TaskJacobian, ublas::trans(m_TaskJacobian));
	
			//ublas::matrix<Float> ret(res_dim, res_dim);
			Float new_det = pseudo_inverse(m_JacTimesJacTranspose, ret);

			CBF_DEBUG("inv: " << m_JacTimesJacTranspose)

			CBF_DEBUG("new_det: " << new_det)
			res[i] = (new_det - orig_det) / m_SamplingStep; 
			m_SensorTransform->resource()->set(resource);
		}

		//m_SensorTransform->resource()->set(resource);

		CBF_DEBUG("res: " << m_Coefficient * res)
		result = m_Coefficient * res;

		return result;
	}


	CBF_PLUGIN_CLASS(AvoidSingularitiesController, Controller)

	#ifdef CBF_HAVE_PLUGIN_SUPPORT
	#ifdef CBF_HAVE_XSD
		/**
			This constructor makes use of the PrimitiveController constructor to extract most of the
			relevant info..
		*/
		AvoidSingularitiesController::AvoidSingularitiesController(
			const AvoidSingularitiesControllerType& xml_instance
		) :
			PrimitiveController(xml_instance) 
		{
			m_SamplingStep = xml_instance.SamplingStep();

			CBF_DEBUG("AAAAAAARGH!!! " << m_SamplingStep)
		}
	#endif
	#endif


} // namespace

