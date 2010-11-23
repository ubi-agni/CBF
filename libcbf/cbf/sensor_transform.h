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
#include <cbf/object.h>

#include <boost/shared_ptr.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <vector>
#include <string>
#include <stdexcept>
#include <functional>
#include <map>
#include <string>

namespace CBFSchema { 
	class SensorTransform; 
	class ConstantSensorTransform;
	class BlockWiseMultiplySensorTransform;
}

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

		NOTE: Subclasses must make sure that the m_TaskJacobian is
		set to the right source during initialization (in all possible
		constructors) because the default implementations of
		task_dim() and resource_dim() use the task jacobian sizes
		to determine the respective dimensionalities..
	*/
	struct SensorTransform : public Object {
		SensorTransform()	:
			m_DefaultComponentName("A task space variable")
		{
	
		}

		SensorTransform(const CBFSchema::SensorTransform &xml_instance);

		/**
			@brief A virtual desctructor to allow the clean destruction 
			of subclass objects through a base class pointer..
		*/
		virtual ~SensorTransform() { }

		/**
			@brief The task space dimensionality 

			The default implementation returns the number of 
			rows in the task jacobian
		*/
		virtual unsigned int task_dim() const {
			return m_TaskJacobian.size1();
		}

		/**
			@brief The resource space dimensionality 

			The default implementation returns the number of 
			columns in the task jacobian
		*/
		virtual unsigned int resource_dim() const {
			return m_TaskJacobian.size2();
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
		virtual void update(const FloatVector &resource_value) = 0;
	
		/**
			@brief Return a reference to the result calculated in the 
			update() function.
		*/
		virtual const FloatVector &result() const { return m_Result; }

	
		/**
			@brief returns the current task jacobian

			A way to get to the current task jacobian. This is needed by the
			controller to construct the nullspace projector.
	
			May only be called after a call to update() to update the internal
			matrices.
		*/
		virtual const FloatMatrix &task_jacobian() const { return m_TaskJacobian; }
	

		/**
			@brief: Returns a reference to the string holding the human readable name
			for the n-th component of the task space..
		*/
		virtual const std::string& component_name(unsigned int n)  {
			if (n >= m_ComponentNames.size())
				return m_DefaultComponentName;

			return m_ComponentNames[n];
		}

		protected:
			/**
				This variable is used to cache the resourcevalue..
			*/
			FloatVector m_ResourceValue;

			/**
				Should be calculated by update() and returned by result()
			*/
			FloatVector m_Result;

			/**
				Should be calculated by update()
			*/
			FloatMatrix m_TaskJacobian;

			/**
				@brief Strings giving names to the components

				See component_names() for more info..
			*/
			std::vector<std::string> m_ComponentNames;

			std::string m_DefaultComponentName;
	};

	typedef boost::shared_ptr<SensorTransform> SensorTransformPtr;


	struct ConstantSensorTransform : public SensorTransform {
		ConstantSensorTransform(const FloatVector &value) {
			init(value);
		}

		ConstantSensorTransform(const CBFSchema::ConstantSensorTransform& xml_instance);

		void init(const FloatVector &value) {
			m_Result = value;
			m_TaskJacobian = ublas::zero_matrix<Float>(m_Result.size(), m_Result.size());
		}

		virtual void update(const FloatVector &resource_value) {

		}
	};


	/**
		@brief A SensorTransform to multiply different blocks with different constants
	*/
	struct BlockWiseMultiplySensorTransform : public SensorTransform {
		
		BlockWiseMultiplySensorTransform(
			SensorTransformPtr operand,
			const unsigned int blocksize, 
			const FloatVector &factors) {

		}

		BlockWiseMultiplySensorTransform(const CBFSchema::BlockWiseMultiplySensorTransform &xml_instance);

		void init(
			SensorTransformPtr operand,
			unsigned int blocksize, 
			const FloatVector &factors
		) {
			m_Operand = operand;
			m_Blocksize = blocksize;
			m_Factors = factors;

			m_Result = FloatVector(m_Operand->task_dim());
			m_TaskJacobian = FloatMatrix(m_Operand->task_dim(), m_Operand->resource_dim());
		}

		virtual void update(const FloatVector &resource_value) {
			m_Operand->update(resource_value);
			m_Result = m_Operand->result();
			m_TaskJacobian = m_Operand->task_jacobian();

			for (
				unsigned int current_row = 0, max_row = task_dim(); 
				current_row < max_row; 
				++current_row
			) {
				m_Result[current_row] *= m_Factors[current_row % m_Blocksize];
				for (unsigned int i = 0, imax = resource_dim(); i < imax; ++i) {
					m_TaskJacobian(current_row, i) *= m_Factors[current_row % m_Blocksize];
				}
			}
		}

		virtual unsigned int task_dim() const { 
			return m_Operand->task_dim();
		}

		virtual unsigned int resource_dim() const {
			return m_Operand->resource_dim();
		}
	
		protected:
			SensorTransformPtr m_Operand;
			unsigned int m_Blocksize;
			FloatVector m_Factors;
	};

} // namespace

#endif

