/* -*- mode: c-non-suck; -*- */

#ifndef CBF_DUMMY_REFERENCE_HH
#define CBF_DUMMY_REFERENCE_HH

#include <cbf/reference.h>
#include <cbf/types.h>
#include <cbf/plugin_decl_macros.h>

#include <boost/numeric/ublas/vector.hpp>

CBF_PLUGIN_PREAMBLE(DummyReference)

namespace CBF {

	struct DummyReference : public Reference {
		CBF_PLUGIN_DECL_METHODS(DummyReference)

		DummyReference(unsigned int num_references = 1, unsigned int dim = 1)
		{
			m_References = std::vector<FloatVector>(num_references, FloatVector(dim));
			for (unsigned int i = 0; i < num_references; ++i)
				for (unsigned int j = 0; j < dim; ++j)
					m_References[i][j] = 0;
		}

		virtual void update() { }
	
		virtual void set_references(std::vector<FloatVector> &refs) {
			m_References = refs;
		}

		/**
			Convenience function if there's only a single reference
		*/
		virtual void set_reference(FloatVector &ref) {
			m_References[0] = ref;
		}

		virtual std::vector<FloatVector> &references() {
			return m_References;
		}

		protected:
	};

	typedef boost::shared_ptr<DummyReference> DummyReferencePtr;
} // namespace

#endif
