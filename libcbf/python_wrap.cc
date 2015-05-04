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

#include <cbf/python_wrap.h>
#include <cbf/debug_macros.h>
#include <cbf/xml_object_factory.h>

#include <string>

#include <Python.h>
#include <boost/python.hpp>

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
			CBF_DEBUG("\n" << str);
		}
	
		size_t second = str.find("#---\n", first);
	
		if (second != std::string::npos) {
			str = str.substr(0, second);
			CBF_DEBUG("\n" << str);
		}
	}
	
	struct Helper {
		bp::object m_MainModule;
	
		Helper() {
			CBF_DEBUG("Py_Initialize");
			Py_InitializeEx(0);
	
			m_MainModule = bp::object((
				bp::handle<>(
					bp::borrowed(PyImport_AddModule("__main__"))
				)
			));
		}
	
		~Helper() {
			CBF_DEBUG("Py_Finalize not called. See boost python doc");
			//Py_Finalize();
		}
	};
	
	struct PythonInterpreter {
		static Helper m_Helper;
	};
	
	Helper PythonInterpreter::m_Helper = Helper();
	
  PythonPotential::PythonPotential(unsigned int sensor_dim, unsigned int task_dim) :
    m_Interpreter(PythonInterpreter()),
    m_SensorDim(sensor_dim),
    m_TaskDim(task_dim)
  {

  }
	
  unsigned int PythonPotential::sensor_dim() const {
    return m_SensorDim;
	}
	
  unsigned int PythonPotential::task_dim() const {
    return m_TaskDim;
  }

  FloatVector &PythonPotential::select_reference(
      const std::vector<FloatVector > &references,
      const FloatVector &input)
  {
    CBF_THROW_RUNTIME_ERROR("PythonPotential::select_reference() is not implemented yet.");
  }

  void PythonPotential::gradient (
      FloatVector &result,
      const FloatVector &reference,
      const FloatVector &input)
  {
    CBF_THROW_RUNTIME_ERROR("PythonPotential::gradient() is not implemented yet.");
    /*
		try {

			//! Get access to main context...
			bp::object main_namespace = m_Interpreter.m_Helper.m_MainModule.attr("__dict__");
		
			//! Setup references...
			CBF_DEBUG("references[0]: " << references[0]);
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
			//CBF_DEBUG(m_ExecScript);
			bp::handle<> ignored((PyRun_String(
				m_ExecScript.c_str(),
				Py_file_input,
				main_namespace.ptr(),
				main_namespace.ptr())
			));
	
			//! Extract output...
			CBF_DEBUG("Extracting result");
			bp::list res = bp::extract<bp::list>(main_namespace["result"]);
		
			result = CBF::FloatVector(input.size());
		
			for (unsigned int j = 0; j < input.size(); ++j)
				result[j] = bp::extract<float>(res[j]);
		
			CBF_DEBUG("Result: " << result);
		}
		catch(...) {
			if (PyErr_Occurred())
			{
				PyErr_Print();
				PyErr_Clear();
			}
			throw;
		}
    */
	}
	
  void PythonPotential::integration (
      FloatVector &nextpos,
      const FloatVector &currentpos,
      const FloatVector &currentgradient,
      const Float timestep)
  {

  }

	PythonSensorTransform::PythonSensorTransform(
		unsigned int task_dim,
		unsigned int resource_dim
		) : SensorTransform(),
			 m_Interpreter(PythonInterpreter())
	{
		m_TaskJacobian = FloatMatrix((int)task_dim, (int)resource_dim);
		m_Result = FloatVector(task_dim);

  }
	
	void PythonSensorTransform::update(const FloatVector &resource_value) {

    try {
			//! Get access to main context...
			bp::object main_namespace = m_Interpreter.m_Helper.m_MainModule.attr("__dict__");
		
			//! Setup references...
      CBF_DEBUG("resource value: " << resource_value);
			bp::list res;
      for (unsigned int i = 0; i < resource_value.size(); ++i) {
        res.append(resource_value[i]);
			}
		
			main_namespace["resource"] = res;
		
			//! Call script...
			//CBF_DEBUG(m_ExecScript);
			bp::handle<> ignored((PyRun_String(
				m_ExecScript.c_str(),
				Py_file_input,
				main_namespace.ptr(),
				main_namespace.ptr())
			));
	
			//! Extract output...
			CBF_DEBUG("Extracting result");
			bp::list result = bp::extract<bp::list>(main_namespace["result"]);
			assert(len(result) == task_dim());

			for (unsigned int j = 0; j < task_dim(); ++j)
				m_Result[j] = bp::extract<float>(result[j]);
		
			CBF_DEBUG("Extracting jacobian");
			bp::list jacobian = bp::extract<bp::list>(main_namespace["jacobian"]);
			assert(len(jacobian) == task_dim());
			for (unsigned int i = 0; i < task_dim(); ++i) {
				bp::list jac_row = bp::extract<bp::list>(jacobian[i]);
				assert(len(jac_row) == resource_dim());
	
				for (unsigned int j = 0; j < resource_dim(); ++j) {
					m_TaskJacobian(i,j) = bp::extract<float>(jac_row[j]);
				}
			}
	
			CBF_DEBUG("m_Result: " << m_Result);
			CBF_DEBUG("m_Jacobian: " << m_TaskJacobian);
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
	PythonPotential::PythonPotential(
		const CBFSchema::PythonPotential &xml_instance,
		ObjectNamespacePtr object_namespace) :
			m_Interpreter(PythonInterpreter()) 
		{
      //m_SensorDim = xml_instance.Dimension();
		
			m_ExecScript = xml_instance.ExecScript();
		
			sanitize_string(m_ExecScript);
			CBF_DEBUG("ExecScript:\n" << m_ExecScript);
		
			m_InitScript = xml_instance.InitScript();
		
			sanitize_string(m_InitScript);
			CBF_DEBUG("InitScript:\n" << m_InitScript);
		
			m_FiniScript = xml_instance.FiniScript();
		
			sanitize_string(m_FiniScript);
			CBF_DEBUG("FiniScript:\n" << m_FiniScript);
		}
		
		PythonSensorTransform::PythonSensorTransform(
			const CBFSchema::PythonSensorTransform &xml_instance,
			ObjectNamespacePtr object_namespace) :
			m_Interpreter(PythonInterpreter()) 
		{
			int task_dim = xml_instance.TaskDimension();
			int resource_dim = xml_instance.ResourceDimension();
			m_TaskJacobian = FloatMatrix(task_dim, resource_dim);
			m_Result = FloatVector(task_dim);

			m_ExecScript = xml_instance.ExecScript();
			sanitize_string(m_ExecScript);
			CBF_DEBUG("ExecScript:\n" << m_ExecScript);
		
			m_InitScript = xml_instance.InitScript();
			sanitize_string(m_InitScript);
			CBF_DEBUG("InitScript:\n" << m_InitScript);
		
			m_FiniScript = xml_instance.FiniScript();
			sanitize_string(m_FiniScript);
			CBF_DEBUG("FiniScript:\n" << m_FiniScript);
		}
		
		static XMLDerivedFactory<PythonPotential, CBFSchema::PythonPotential> x;
		static XMLDerivedFactory<PythonSensorTransform, CBFSchema::PythonSensorTransform> x2;

	#endif
} // namespace

