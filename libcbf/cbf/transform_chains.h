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

#ifndef CBF_TRANSFORM_CHAINS_HH
#define CBF_TRANSFORM_CHAINS_HH

#include <vector>
#include <utility>
#include <boost/shared_ptr.hpp>

#include <cbf/config.h>

#include <cbf/sensor_transform.h>
#include <cbf/effector_transform.h>
#include <cbf/dummy_resource.h>
#include <cbf/utilities.h>

namespace CBFSchema { class SensorTransformChain; }

namespace CBF {
	
	/**
		@brief Due to the chain rule of multivariate calculus we can actually calculate a 
		pseudo inverses and nullspace projectors for chained sensor and effector transforms, too.
	
		By convention the sensor transforms are executed in ascending order. I.e.
		m_SensorTransforms[0] is taking the values from the resource, and is executed. The output
		of that one serves as input to m_SensorTransforms[1] and so on.
	*/
	struct SensorTransformChain : public SensorTransform {
		SensorTransformChain(const CBFSchema::SensorTransformChain &xml_instance);
	
		FloatMatrix m_Jacobian;
	
		virtual void update() {
			FloatMatrix jac = m_SensorTransforms[0]->task_jacobian(); 
	
			FloatMatrix tmp;
			for (unsigned int i = 1; i < m_SensorTransforms.size(); ++i) {
				tmp = m_SensorTransforms[i]->task_jacobian();
				jac = ublas::prod(tmp, jac);
			}
			m_Jacobian = jac;
		}
	
		virtual void exec(FloatVector &result) {
			if (m_SensorTransforms.empty()) throw std::runtime_error("[SensorTransformChain]: Empty!!");
	
			//! Copy the resource to the resource of the temporary variable
			FloatVector tmp = resource()->get();
			FloatVector tmp2;
	
			// CBF_DEBUG("[SensorTransformChain]: Resource: " << tmp)
	
			for (
				std::vector<SensorTransformPtr>::iterator it = m_SensorTransforms.begin();
				it != m_SensorTransforms.end();
				++it
			)
			{
				// CBF_DEBUG("[SensorTransformChain]: Executing subordinate transform");
				//! Make the result of previous transform available for current transform and...
				(*it)->resource()->set(tmp);
	
				//! ...execute the transform and...
				tmp2 = (*it)->result();
				// CBF_DEBUG("[SensorTransformChain]: Subordinate result: " << tmp2)
	
				tmp = tmp2;
			}
	
			result = tmp;
		}
	
		/**
			Returns the resource dim of the first sensor transform in the chain
		*/
		virtual unsigned resource_dim() const {
			if (m_SensorTransforms.size() == 0) throw std::runtime_error("[SensorTransformChain]: Empty!!");
	
			return (*m_SensorTransforms.begin())->resource_dim();
		}
	
		/**
			Returns the task dim of the last transform in the chain.
		*/
		virtual unsigned int task_dim() const {
			if (m_SensorTransforms.size() == 0) throw std::runtime_error("[SensorTransformChain]: Empty!!");
	
			return (*--(m_SensorTransforms.end()))->task_dim();
		}
	
	
		/**
			This method takes ownership of the SensorTransforms and replaces any resource
			with a dummy resource which is used to store the temporary results.
	
			You need to call set_resource() after this to make sure all intermediate 
			resources are adjusted with the correct dimensions.
		*/
		void set_transforms(std::vector<SensorTransformPtr> trs) {
			if (trs.empty()) throw std::runtime_error("[SensorTransformChain]: Empty vector of transforms");
	
			m_SensorTransforms = trs;
		}
	
	
		virtual void set_resource(ResourcePtr res) {
			// CBF_DEBUG("[SensorTransformChain]: set_resource... ")
			SensorTransform::set_resource(res);
	
			for (
				std::vector<SensorTransformPtr>::iterator it = m_SensorTransforms.begin();
				it != m_SensorTransforms.end();
				++it
			) {
				//if ((*it).get() == 0) throw std::runtime_error("[SensorTransform]: Invalid pointer");
				// CBF_DEBUG("[SensorTransformChain]: resource dim: " << (*it)->resource_dim());
				(*it)->set_resource(ResourcePtr(new DummyResource((*it)->resource_dim())));
			}
	
	}
	
		/**
			Do not alter the return value..
		*/
		std::vector<SensorTransformPtr> transforms() {
			return m_SensorTransforms;
		}
	
		virtual void get_task_jacobian(FloatMatrix &result) {
			result = m_Jacobian;
		}
	
		protected:
			/**
				This is a list of SensorTransforms. The output of each one is then used as input for the next
				one in the list. 
			*/
			std::vector<SensorTransformPtr> m_SensorTransforms;
	};
	
	typedef boost::shared_ptr<SensorTransformChain> SensorTransformChainPtr;
	
} // namespace

#endif

