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

#ifndef CBF_IDENTITY_TRANSFORM_HH
#define CBF_IDENTITY_TRANSFORM_HH

#include <cbf/plugin_decl_macros.h>
#include <cbf/sensor_transform.h>
#include <cbf/effector_transform.h>

CBF_PLUGIN_PREAMBLE(IdentityEffectorTransform)
CBF_PLUGIN_PREAMBLE(IdentitySensorTransform)

namespace CBF {
	/**
		@brief Trivial transform that passes the input through unchanged
	*/
	struct IdentityEffectorTransform : public EffectorTransform {
		CBF_PLUGIN_DECL_METHODS(IdentityEffectorTransform)

		unsigned int m_Dim;
	
		IdentityEffectorTransform(unsigned int dim = 1) :
			m_Dim(dim)
		{
			m_InverseTaskJacobian = boost::numeric::ublas::identity_matrix<Float>(dim);
		}

		virtual void update() {
			
		}	
	
		virtual unsigned get_resource_dim() {
			return m_Dim;
		}
	
		virtual unsigned int get_task_dim() {
			return m_Dim;
		}
	};
	
	/**
		@brief Trivial transform that passes the input through unchanged
	*/
	struct IdentitySensorTransform : public SensorTransform {
		CBF_PLUGIN_DECL_METHODS(IdentitySensorTransform)
	
		unsigned int m_Dim;
	
		IdentitySensorTransform(unsigned int dim = 1) :
			m_Dim(dim)
		{
			// Setup the (constant) jacobian which is just the identity matrix.. [TODO: erm, check this]
			m_TaskJacobian = boost::numeric::ublas::identity_matrix<Float>(dim,dim);
		}

		virtual void update() {
			//! nothing to do as the jacobian is constant and computed during construction time
			m_Result = m_Resource->get();
		}
	
		virtual unsigned int resource_dim() {
			return m_Dim;
		}
	
		virtual unsigned int task_dim() {
			return m_Dim;
		}
	};


	typedef boost::shared_ptr<IdentityEffectorTransform> IdentityEffectorTransformPtr;
	
	typedef boost::shared_ptr<IdentitySensorTransform> IdentitySensorTransformPtr;
} // namespace

#endif
