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


    Copyright 2009, 2010 Viktor Richter
*/

/* -*- mode: c-non-suck; -*- */

#include <cbf/xcf_memory_sensor_transform.h>
#include <cbf/debug_macros.h>
#include <cbf/xml_factory.h>

namespace CBF {

	#ifdef CBF_HAVE_XSD
		XCFMemorySensorTransform::XCFMemorySensorTransform(
					const CBFSchema::XCFMemorySensorTransform &xml_instance) :
			SensorTransform(xml_instance)
		{
			SensorTransformPtr pt = (XMLObjectFactory::instance() 
						-> create<SensorTransform>(xml_instance.SensorTransform1()));
			m_SensorTransform = pt;
			m_MemoryPtr = memory::interface::MemoryInterface::getInstance(xml_instance.URI());
			m_ResultName = xml_instance.ResultName();
		}

		static XMLDerivedFactory<XCFMemorySensorTransform, CBFSchema::XCFMemorySensorTransform> x;
	#endif

	XCFMemorySensorTransform::XCFMemorySensorTransform(
					const std::string &uri, 
					const std::string &result_name, 
					SensorTransformPtr sensor_transform
					)
	{
		m_MemoryPtr = memory::interface::MemoryInterface::getInstance(uri);
		m_ResultName = result_name;
		m_SensorTransform = sensor_transform;
	}

	void XCFMemorySensorTransform::update(const FloatVector &resource_value){
		m_SensorTransform -> update(resource_value);
		send();
	}

	void XCFMemorySensorTransform::send(){

		//Getting the result vector from the SensorTransform, converting to string.
		CBF_DEBUG("creating vector string");
		std::stringstream vector_string;
		vector_string << m_SensorTransform -> result();
		
		//Getting the task-jacobian matrix from the SensorTransform, converting to string.
		CBF_DEBUG("creating matrix string");
		std::stringstream matrix_string;
		matrix_string << m_SensorTransform -> task_jacobian();

		if(m_ResultLocationPtr == NULL){
			//First insert the XML-document

			CBF_DEBUG("creating vector doc");
			CBFSchema::BoostVector vectorDoc(vector_string.str());

			CBF_DEBUG("creating matrix doc");
			CBFSchema::BoostMatrix matrixDoc(matrix_string.str());

			CBF_DEBUG("creating XCFMemorySensorTransformResult doc");
			CBFSchema::XCFMemorySensorTransformResult transformResult(m_ResultName, vectorDoc, matrixDoc);

			std::ostringstream s;
			CBFSchema::XCFMemorySensorTransformResult_ (s, transformResult);

			CBF_DEBUG("document: " << s.str());

			//Insert the result-XML at the server.
			CBF_DEBUG("insert result at memory-server");
		
			std::string documentText = m_MemoryPtr -> insert(s.str());

			xmltio::XPath rPath(ResultXPathString());
			xmltio::XPath tjPath(TeskJacobianXPathString());

			m_ResultLocationPtr = LocationPtr(new xmltio::Location(documentText,rPath));
			m_TaskJacobianLocationPtr = LocationPtr(new xmltio::Location(documentText,tjPath));
		} else {
			//Edit the document and replace the one on the memory

			*m_ResultLocationPtr = vector_string.str();
			*m_TaskJacobianLocationPtr = matrix_string.str();

			CBF_DEBUG("Document text:" << m_ResultLocationPtr -> getDocumentText());

			/*
			 * Because the xmltio::Location holds the whole document we can take the 
			 * one from the result-Location.
			*/
			m_MemoryPtr -> replace(m_ResultLocationPtr -> getDocumentText());
		

		}
}

} // namespace

