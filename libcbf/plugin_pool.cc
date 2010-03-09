#include <cbf/plugin_pool.h>

#include <cbf/plugin_decl_macros.h>

#include <cbf/sensor_transform.h>
#include <cbf/effector_transform.h>
#include <cbf/resource.h>
#include <cbf/potential.h>
#include <cbf/combination_strategy.h>
#include <cbf/controller.h>
#include <cbf/primitive_controller.h>
#include <cbf/reference.h>

#include <stdexcept>

#ifdef CBF_HAVE_XSD
	#include <schemas.hxx>
#endif

namespace CBF {

#ifdef CBF_HAVE_XSD

	/**
		A helper class used only by the read_from_xml_file function
	*/
	class ParsingErrorHandler : public xml_schema::error_handler
	{
		virtual bool handle (const std::basic_string<char>& id,
			unsigned long line,
			unsigned long column,
			severity,
			const std::basic_string<char>& message) {
				std::cout << "\"" << message <<"\"" << " in line: " << line << ", column: " << column << std::endl;
				return false;
			}
	};
	
	
	/**
		A utility function to get an xml document which can then be passed
		to one of the PluginPool::create methods
	*/
	template <class ComponentType, class XMLType>
	boost::shared_ptr<ComponentType> create_from_file(const std::string &filename) {
		ParsingErrorHandler err_handler;
		std::auto_ptr<XMLType> xml_instance
			(::Controller
				(filename,
				 err_handler,
				 xml_schema::flags::dont_validate));
		
		std::cout << "- Cooking controller..." << std::endl;
		//! Finally let's create the controller instance..
		boost::shared_ptr<ComponentType> ptr = PluginPool<ComponentType>::get_instance()->create_from_xml(*xml_instance);
		return ptr;
	}


	/**
		If XSD support is given, provide a definition of the corresponding create method. Note
		that for this to work [as this is an out of class definition] the plugin pools need
		to be explicitly instantiated below
	*/
	template<class SuperClass>
	boost::shared_ptr<SuperClass> PluginPool<SuperClass>::create_from_xml(const xsd::cxx::tree::_type &xml_document) {
		boost::shared_ptr<SuperClass> p;
		for (unsigned int i = 0; i < m_PluginHelpers.size(); ++i)
		{
			p = m_PluginHelpers[i]->create(xml_document);
			if (p.get() == 0) continue;
			else return p;
		}
		throw std::runtime_error("No responsible creator function found. Are you sure the type is correct?");
	}


	template<class SuperClass> template <class XMLType>
	boost::shared_ptr<SuperClass> PluginPool<SuperClass>::create_from_file(const std::string &file_name) {
		// XMLType t;
		return CBF::create_from_file<SuperClass, XMLType>(file_name);
	}
#endif



/**
	One specialized instance for every base type
*/
template class PluginPool<SensorTransform>;
template class PluginPool<EffectorTransform>;
template class PluginPool<Resource>;
template class PluginPool<Potential>;
template class PluginPool<Controller>;
//template class PluginPool<PrimitiveController>;
template class PluginPool<CombinationStrategy>;
template class PluginPool<Reference>;

template<> PluginPool<SensorTransform> *PluginPool<SensorTransform>::s_Instance = 0;
template<> PluginPool<EffectorTransform> *PluginPool<EffectorTransform>::s_Instance = 0;
template<> PluginPool<Resource> *PluginPool<Resource>::s_Instance = 0;
template<> PluginPool<Potential> *PluginPool<Potential>::s_Instance = 0;
template<> PluginPool<CBF::Controller>* PluginPool<CBF::Controller>::s_Instance = 0;
//template<> PluginPool<PrimitiveController> *PluginPool<PrimitiveController>::s_Instance = 0;
template<> PluginPool<CombinationStrategy> *PluginPool<CombinationStrategy>::s_Instance = 0;
template<> PluginPool<Reference> *PluginPool<Reference>::s_Instance = 0;

#ifdef CBF_HAVE_XSD
	/**
		@brief The static instance of the utility function to create a controller from a file
	*/
	template 
		boost::shared_ptr<CBF::Controller> 
		PluginPool<CBF::Controller>::create_from_file<ControllerType>(const std::string &filename);
#endif

} // namespace
