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

#include <cbf/sensor_transform.h>
#include <cbf/effector_transform.h>

namespace CBFSchema {
	class IdentityEffectorTransform;
	class IdentitySensorTransform;
}

namespace CBF {
	/**
		@brief Trivial transform that passes the input through unchanged
	*/
	struct IdentityEffectorTransform : public EffectorTransform {
		IdentityEffectorTransform(const CBFSchema::IdentityEffectorTransform &xml_instance);

		IdentityEffectorTransform(unsigned int dim = 1) :
			EffectorTransform(dim, dim)
		{
			m_InverseTaskJacobian = boost::numeric::ublas::identity_matrix<Float>(dim);
		}

		virtual void update(
			const FloatVector &resource_value, 
			const FloatMatrix &task_jacobian) 
		{ }	
	
		virtual void exec(const CBF::FloatVector& in, CBF::FloatVector& result)
		{
			result = in;
		}
	};

	typedef boost::shared_ptr<IdentityEffectorTransform> IdentityEffectorTransformPtr;

	
	/**
		@brief Trivial transform that passes the input through unchanged
	*/
	struct IdentitySensorTransform : public SensorTransform {
		IdentitySensorTransform(const CBFSchema::IdentitySensorTransform &xml_instance);
	
		unsigned int m_Dim;
	
		IdentitySensorTransform(unsigned int dim = 1) :
			SensorTransform(dim, dim)
		{
			init(dim);
			// Setup the (constant) jacobian which is just the identity matrix.. [TODO: erm, check this]
		}

		virtual void update(const FloatVector &resource_value) {
			//! nothing to do as the jacobian is constant and computed during construction time
			m_Result = resource_value;
		}
	
		virtual unsigned int resource_dim() const {
			return m_Dim;
		}
	
		virtual unsigned int task_dim() const {
			return m_Dim;
		}

		virtual void init(unsigned int dim) {
			m_Dim = dim;
			m_TaskJacobian = boost::numeric::ublas::identity_matrix<Float>(dim,dim);
		}
	};

	typedef boost::shared_ptr<IdentitySensorTransform> IdentitySensorTransformPtr;

} // namespace

#endif
