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

#ifndef XCF_MEMORY_SENSOR_TRANSFORM_HH
#define XCF_MEMORY_SENSOR_TRANSFORM_HH

#include <cbf/sensor_transform.h>

#include <Memory/Interface.hpp>

namespace CBF {

	/**
		@brief This is a wrapper to provide XCFMemory communication for the SensorTransform
		it behaves like the passed SensorTransform except that after making the usual 
		update() calculations, it publishes the resuliting matrices on the XCFMemory
		server.

		@param uri The URI of the XCFMemory server on which the data should be published.
		@param resultName The name of the XCFMemorySensorTransformResult.
		@param m_SensorTransform The SensorTransformPtr of the used SensorTransform.
	*/
	struct XCFMemorySensorTransform : public SensorTransform {

		XCFMemorySensorTransform(
					const std::string &uri, 
					const std::string &resultName, 
					SensorTransformPtr m_SensorTransform
					);

		/**
			@brief A virtual desctructor to allow the clean destruction 
			of subclass objects through a base class pointer..
		*/
		virtual ~XCFMemorySensorTransform() { }
	
		/**
			@brief Returns the task space dimensionality.
		*/
		virtual unsigned int task_dim() const {
			return m_SensorTransform -> task_dim();
		}

		/**
			@brief Returns the resource space dimensionality.
		*/
		virtual unsigned int resource_dim() const {
			return m_SensorTransform -> resource_dim();
		}
	
		/**
			@brief Calls update() on the used SensorTransform and publishes the
			resulting matrices on the XCFMemory server.
		*/
		virtual void update(const FloatVector &resource_value);

		/**
			@brief Returns a reference to the result calculated in the 
			update() function.
		*/
		virtual const FloatVector &result() const { return m_SensorTransform -> result();}

	
		/**
			@brief Returns the current task jacobian.
		*/
		virtual const FloatMatrix &task_jacobian() const { return m_SensorTransform -> task_jacobian(); }
	

		/**
			@brief: Returns a reference to the string holding the human readable name
			for the n-th component of the task space..
		*/
		virtual const std::string& component_name(unsigned int n)  {
			return m_SensorTransform -> component_name(n);
		}

		/**
			@brief: Sends the result and the task jacobian to the XCFMemory server.
		*/
		virtual void send();

		protected:

		/**
			@brief. The name of the XCFMemorySensorTransformResult.
		*/
		std::string resultName;

		/**
			@brief: The SensorTransform, which is to wrap.
		*/
		SensorTransformPtr m_SensorTransform;
		
		/**
			@brief: The pointer to the XCFMemory server.
		*/
		memory::interface::MemoryInterface::pointer memoryPtr;
			
	};

} // namespace

#endif

