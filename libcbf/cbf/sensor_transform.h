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

#ifndef CONTROL_BASIS_SENSOR_TRANSFORM_HH
#define CONTROL_BASIS_SENSOR_TRANSFORM_HH

#include <cbf/config.h>

#include <cbf/types.h>
#include <cbf/resource.h>

#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <vector>

class SensorTransformType;

namespace CBF {
	
	namespace ublas = boost::numeric::ublas;
	
	/**
		@brief A SensorTransform maps resource values into the task space. 

		Sensor transforms also provide the jacobian matrix for the 
		given resource values (remember that the jacobian might depend on 
		the current joint values for e.g. a robotic arm)..

		For information on how to inherit from this class correctly read 
		the documentation of the virtual	member functions below..

		Note that for the functionality that allows creation of controllers
		from XML files requires that all subclasses have a default 
		constructor (i.e. one without arguments)..
	*/
	struct SensorTransform {
		SensorTransform()	{
	
		}
	

		/**
			@brief A virtual desctructor to allow the clean destruction 
			of subclass objects through a base class pointer..
		*/
		virtual ~SensorTransform() {
	
		}
	
		/**
			@brief Update internal state and do expensive one shot calculations 

			This method must be called before result. Ideally right 
			after the resource has been changed. This method is meant 
			for SensorTransforms to be able to do one-shot expensive
			computations whose results will consequently be used by 
			different methods, e.g. the jacobian given that it depends 
			on the current resource value.
		*/
		virtual void update() = 0;
	
		/**
			@brief Return a reference to the result calculated in the 
			update() function.
		*/
		virtual FloatVector &result() { return m_Result; }

		/**
			@brief Needs to be implemented in subclass to allow 
			dimensionality checking when this is bound to a resource.
		*/
		virtual unsigned resource_dim() = 0;
	
		/**
			@brief Needs to be implemented in subclass to allow 
			dimensionality checking when bound to a resource.
		*/
		virtual unsigned int task_dim() = 0;
	
		ResourcePtr resource() {
			return m_Resource;
		}

		/**
			@brief Binf this sensor transform to a resource

			Subclasses might have to override this. See e.g. 
			CompositeSensorTransform for an example that does this.
		*/
		virtual void set_resource(ResourcePtr r) {
			if (r->dim() != resource_dim()) {
				std::stringstream stream;
				stream << "[SensorTransform]: Dimension mismatch!";
				stream 
					<< " Expected resource dim " 
					<< resource_dim() 
					<< " but got " 
					<< r->dim();
				throw std::runtime_error(stream.str().c_str());
			}
			m_Resource = r;
		}
	
		/**
			@brief returns the current task jacobian

			A way to get to the current task jacobian. This is needed by the
			controller to construct the nullspace projector.
	
			May only be called after a call to update() to update the internal
			matrices.
		*/
		virtual FloatMatrix &task_jacobian() { return m_TaskJacobian; }
	
		protected:
			/**
				A sensor transformation is bound to a resource..
			*/
			ResourcePtr m_Resource;

			/**
				This variable is used to cache the resourcevalue..
			*/
			FloatVector m_ResourceValue;

			/**
				Should be calculated by update()
			*/
			FloatVector m_Result;

			/**
				Should be calculated by update()
			*/
			FloatMatrix m_TaskJacobian;
	};
	
	typedef boost::shared_ptr<SensorTransform> SensorTransformPtr;
} // namespace

#endif

