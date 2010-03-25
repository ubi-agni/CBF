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

#include <Python.h>
#include <boost/python.hpp>

#include <cbf/python_wrap.h>
#include <cbf/debug_macros.h>
#include <cbf/plugin_impl_macros.h>

#include <string>

namespace CBF {
	
	namespace bp = boost::python;
	
	/**
		This function tries to strip all characters up to the first occurence of
		"#---" [inclusive] and from the second occurence of "#---" [inclusive]
	*/
	void sanitize_string(std::string &str) {
		size_t first = str.find("#---\n");
	
		if (first != std::string::npos) {
			first = first +  std::string("#---\n").length();
			str = str.substr(first, str.length() - first);
			CBF_DEBUG("\n" << str)
		}
	
		size_t second = str.find("#---\n", first);
	
		if (second != std::string::npos) {
			str = str.substr(0, second);
			CBF_DEBUG("\n" << str)
		}
	}
	
	struct Helper {
		bp::object m_MainModule;
	
		Helper() {
			CBF_DEBUG("Py_Initialize")
			Py_InitializeEx(0);
	
			m_MainModule = bp::object((
				bp::handle<>(
					bp::borrowed(PyImport_AddModule("__main__"))
				)
			));
		}
	
		~Helper() {
			CBF_DEBUG("Py_Finalize not called. See boost python doc")
			//Py_Finalize();
		}
	};
	
	struct PythonInterpreter {
		static Helper m_Helper;
	};
	
	Helper PythonInterpreter::m_Helper = Helper();
	
	PythonPotential::PythonPotential(unsigned int dim) :
		m_Interpreter(PythonInterpreter()),
		m_Dim(dim)
	{
	}
	
	unsigned int PythonPotential::task_dim() const {
		return m_Dim;
	}
	
	void PythonPotential::gradient (
		ublas::vector<Float> &result, 
		const std::vector<ublas::vector<Float> > &references, 
		const ublas::vector<Float> &input
	) {
		try {
	
			//! Get access to main context...
			bp::object main_namespace = m_Interpreter.m_Helper.m_MainModule.attr("__dict__");
		
			//! Setup references...
			CBF_DEBUG("references[0]: " << references[0])
			bp::list refs;
			for (unsigned int i = 0; i < references.size(); ++i) {
				bp::list ref;
				for (unsigned int j = 0; j < references[i].size(); ++j)
					ref.append(references[i][j]);
				refs.append(ref);
			}
		
			main_namespace["references"] = refs;
		
			//! Setup input...
			bp::list in;
			for (unsigned int j = 0; j < input.size(); ++j)
				in.append(input[j]);
		
			main_namespace["input"] = in;
	
			//! Call script...
			//CBF_DEBUG(m_ExecScript)
			bp::handle<> ignored((PyRun_String(
				m_ExecScript.c_str(),
				Py_file_input,
				main_namespace.ptr(),
				main_namespace.ptr())
			));
	
			//! Extract output...
			CBF_DEBUG("Extracting result")
			bp::list res = bp::extract<bp::list>(main_namespace["result"]);
		
			result = CBF::ublas::vector<Float>(input.size());
		
			for (unsigned int j = 0; j < input.size(); ++j)
				result[j] = bp::extract<float>(res[j]);
		
			CBF_DEBUG("Result: " << result)
		}
		catch(...) {
			if (PyErr_Occurred())
			{
				PyErr_Print();
				PyErr_Clear();
			}
			throw;
		}
	}
	
	PythonSensorTransform::PythonSensorTransform(
		unsigned int task_dim,
		unsigned int resource_dim
	) :
		m_Interpreter(PythonInterpreter())
	{
	
	}
	
	void PythonSensorTransform::update() {
		//SensorTransform::update();
		try {
			//! Get access to main context...
			bp::object main_namespace = m_Interpreter.m_Helper.m_MainModule.attr("__dict__");
		
			//! Setup references...
			CBF_DEBUG("resource value: " << m_ResourceValue)
			bp::list res;
			for (unsigned int i = 0; i < m_ResourceValue.size(); ++i) {
				res.append(m_ResourceValue[i]);
			}
		
			main_namespace["resource"] = res;
		
			//! Call script...
			//CBF_DEBUG(m_ExecScript)
			bp::handle<> ignored((PyRun_String(
				m_ExecScript.c_str(),
				Py_file_input,
				main_namespace.ptr(),
				main_namespace.ptr())
			));
	
			//! Extract output...
			CBF_DEBUG("Extracting result")
			bp::list result = bp::extract<bp::list>(main_namespace["result"]);
		
			m_Result = ublas::vector<Float>(m_TaskDim);
		
			for (unsigned int j = 0; j < m_TaskDim; ++j)
				m_Result[j] = bp::extract<float>(result[j]);
		
			CBF_DEBUG("Extracting jacobian")
			m_Jacobian = FloatMatrix(m_TaskDim, m_ResourceDim);
	
			bp::list jacobian = bp::extract<bp::list>(main_namespace["jacobian"]);
			for (unsigned int i = 0; i < m_TaskDim; ++i) {
				bp::list jac_row = bp::extract<bp::list>(jacobian[i]);
	
				for (unsigned int j = 0; j < m_ResourceDim; ++j) {
					m_Jacobian(i,j) = bp::extract<float>(jac_row[j]);
				}
			}
	
			CBF_DEBUG("m_Result: " << m_Result)
			CBF_DEBUG("m_Jacobian: " << m_Jacobian)
		}
		catch(...) {
			if (PyErr_Occurred())
			{
				PyErr_Print();
				PyErr_Clear();
			}
	
			throw;
		}
	}
	
	#ifdef CBF_HAVE_XSD
		PythonPotential::PythonPotential(const PythonPotentialType &xml_instance) :
			m_Interpreter(PythonInterpreter()) 
		{
			m_Dim = xml_instance.Dimension();
		
			m_ExecScript = xml_instance.ExecScript();
		
			sanitize_string(m_ExecScript);
			CBF_DEBUG("ExecScript:\n" << m_ExecScript)
		
			if (xml_instance.InitScript().present())
				m_InitScript = *(xml_instance.InitScript());
		
			sanitize_string(m_InitScript);
			CBF_DEBUG("InitScript:\n" << m_InitScript)
		
			if (xml_instance.FiniScript().present())
				m_FiniScript = *(xml_instance.FiniScript());
		
			sanitize_string(m_FiniScript);
			CBF_DEBUG("FiniScript:\n" << m_FiniScript)
		}
		
		PythonSensorTransform::PythonSensorTransform(const PythonSensorTransformType &xml_instance) :
			m_Interpreter(PythonInterpreter()) 
		{
			m_TaskDim = xml_instance.TaskDimension();
			m_ResourceDim = xml_instance.ResourceDimension();
		
			m_ExecScript = xml_instance.ExecScript();
		
			sanitize_string(m_ExecScript);
			CBF_DEBUG("ExecScript:\n" << m_ExecScript)
		
			if (xml_instance.InitScript().present())
				m_InitScript = *(xml_instance.InitScript());
		
			sanitize_string(m_InitScript);
			CBF_DEBUG("InitScript:\n" << m_InitScript)
		
			if (xml_instance.FiniScript().present())
				m_FiniScript = *(xml_instance.FiniScript());
		
			sanitize_string(m_FiniScript);
			CBF_DEBUG("FiniScript:\n" << m_FiniScript)
		}
		
		CBF_PLUGIN_CLASS(PythonPotential, Potential)
		CBF_PLUGIN_CLASS(PythonSensorTransform, SensorTransform)
		
	#endif
} // namespace

