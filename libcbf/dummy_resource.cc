#include <cbf/dummy_resource.h>
#include <cbf/debug_macros.h>
#include <cbf/plugin_macros.h>
#include <cbf/utilities.h>

namespace CBF {
	#ifdef CBF_HAVE_XSD
	#ifdef CBF_HAVE_PLUGIN_SUPPORT
		DummyResource::DummyResource(const DummyResourceType &xml_instance)
		{
			m_Variables = ublas::zero_vector<Float>(xml_instance.Dimension());

#ifdef CBF_HAVE_GNUPLOT
			m_History = std::deque<FloatVector>(CBF_HIST_SIZE, FloatVector(m_Variables.size())),
			m_Times = FloatVector(CBF_HIST_SIZE, 0);


			for (unsigned int i = 0; i < CBF_HIST_SIZE; ++i) {
				m_Times[i] = i;
			}

			m_Count = 0;

#endif

			for (unsigned int i = 0; i < m_Variables.size(); ++i) {
				m_Variables[i] = 0.1 * 2 * M_PI * ((Float)rand()-(RAND_MAX/2.0))/(Float)RAND_MAX;
			}


		
			if (xml_instance.Vector().present())
				m_Variables = create_vector(xml_instance.Vector().get());
			CBF_DEBUG("current values: " << m_Variables)
		
		}
	#endif
	#endif
	
	CBF_PLUGIN_CLASS(DummyResource, Resource)
} //namespace

