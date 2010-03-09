#ifndef CBF_DUMMY_RESOURCE_HH
#define CBF_DUMMY_RESOURCE_HH

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/resource.h>

#include <cbf/plugin_decl_macros.h>

#include <string>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <deque>

#include <boost/numeric/ublas/io.hpp>

CBF_PLUGIN_PREAMBLE(DummyResource)

#define CBF_HIST_SIZE 750

namespace CBF {
	namespace ublas = boost::numeric::ublas;

	/**
		@brief A resource that does nothing but hold the current value
	*/
	struct DummyResource : public Resource {
		CBF_PLUGIN_DECL_METHODS(DummyResource)

		FloatVector m_Variables;

#ifdef CBF_HAVE_GNUPLOT
		Gnuplot m_Gnuplot;
		std::deque<FloatVector> m_History;
		FloatVector m_Times;
#endif

		unsigned int m_Count;
	
		/**
			The variance argument can be used to initialize the resource
			with non zero values drawn from a distribution with range
			-variance to variance..

			TODO: implement variance. Right now a fixed range of [-1:1] is 
			used when variance != 0
		*/
		DummyResource(unsigned int variables = 1, Float variance = 0) :
#ifdef CBF_HAVE_GNUPLOT
			m_History(CBF_HIST_SIZE, ublas::vector<Float>(variables)),
			m_Times(CBF_HIST_SIZE, 0),
#endif
			m_Variables(variables)
		{
			if (variance != 0) {
				for (unsigned int i = 0; i < variables; ++i)
					m_Variables[i] = 2 * M_PI * ((Float)rand()-(RAND_MAX/2.0))/(Float)RAND_MAX;
			}
#ifdef CBF_HAVE_GNUPLOT
			for (unsigned int i = 0; i < CBF_HIST_SIZE; ++i) {
				m_Times[i] = i;
			}
#endif
			//CBF_DEBUG("current values" << m_Variables);
		}
	
	
		~DummyResource() {
			//CBF_DEBUG("Destructor")
		}
	
		virtual const FloatVector &get() {
			return m_Variables;
		}
	
		virtual void set(const FloatVector &arg) {
			m_Variables = arg;
#ifdef CBF_HAVE_GNUPLOT
			m_History.push_front(arg);
			if (m_History.size() > CBF_HIST_SIZE) m_History.pop_back();

			m_Count++;
			if ((m_Count = m_Count % CBF_HIST_SIZE) == 0) {
				//m_Gnuplot = Gnuplot();
				m_Gnuplot.remove_tmpfiles();
				m_Gnuplot.reset_all();
				for (unsigned int i = 0; i < m_Variables.size(); ++i) {
					FloatVector vec(CBF_HIST_SIZE, 0);
					for (unsigned int j = 0; j < CBF_HIST_SIZE; ++j) {
						vec[j] = m_History[(CBF_HIST_SIZE-1)-j][i];
					}
					m_Gnuplot.plot_xy(m_Times, vec, "y=x");
				}
				m_Gnuplot.showonscreen();
			}
#endif
			// CBF_DEBUG("current values" << m_Variables);
		}
	
		virtual void add(const FloatVector &arg) {
			set(m_Variables + arg);
			// CBF_DEBUG("current values" << m_Variables);
		}
	
		virtual unsigned int dim() {
			return m_Variables.size();
		}
	};
	
	typedef boost::shared_ptr<DummyResource> DummyResourcePtr;
} // namespace

#endif

