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

#ifndef CBF_WEIGHTED_SUM_TRANSFORMS_HH
#define CBF_WEIGHTED_SUM_TRANSFORMS_HH

#include <cbf/sensor_transform.h>
#include <cbf/plugin_decl_macros.h>
#include <cbf/types.h>

#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <stdexcept>
#include <cassert>

CBF_PLUGIN_PREAMBLE(WeightedSumSensorTransform)


namespace CBF {

namespace ublas = boost::numeric::ublas;

/** 
	@brief A sensor transform that allows combining several sensor 
	transforms working on the same resource in a weighted sum
*/
	
struct WeightedSumSensorTransform : public SensorTransform {
		CBF_PLUGIN_DECL_METHODS(WeightedSumSensorTransform)


		WeightedSumSensorTransform(
			std::vector<SensorTransformPtr> transforms 
				= std::vector<SensorTransformPtr>(), 
			FloatVector weights 
				= ublas::zero_vector<Float>()
		) {
			set_transforms(transforms);
			set_weights(weights);
		}

		void set_weights(const FloatVector &weights) {
			m_Weights = weights;
		}

		void set_transforms(std::vector<SensorTransformPtr> &transforms) {
			m_Transforms = transforms;

			//! Set the result and task jacobian matrixes to the right sizes...
			m_TaskJacobian = ublas::zero_matrix<Float>(transforms[0]->task_dim(), transforms[0]->resource_dim());
			m_Result = ublas::zero_vector<Float>(transforms[0]->task_dim());
		}

		void set_resource(ResourcePtr resource) {
			for (unsigned int i = 0; i < m_Transforms.size(); ++i) {
				m_Transforms[i]->set_resource(resource);
			}
			m_Resource = resource;
		}

		void update() {
			assert(m_Transforms.size() > 1);
			assert(m_Transforms.size() == m_Weights.size());

			for (unsigned int i = 0; i < m_Transforms.size(); ++i) {
				m_Transforms[i]->update();
			}

			m_TaskJacobian = ublas::zero_matrix<Float>(m_Transforms[0]->task_dim(), m_Transforms[0]->resource_dim());
			m_Result = ublas::zero_vector<Float>(m_Transforms[0]->task_dim());

			for (unsigned int i = 0; i < m_Transforms.size(); ++i) {
				m_TaskJacobian += m_Weights[i] * m_Transforms[i]->task_jacobian();
				m_Result += m_Weights[i] * m_Transforms[i]->result();
			}
		}

		// As both sensor transforms are required to have the same resource dimensionality, it does not matter
		// which one we return
		unsigned int resource_dim() const { return m_Transforms[0]->resource_dim(); }

		// As both sensor transforms are required to have the same task dimensionality, it does not matter
		// which one we return
		unsigned int task_dim() const { return m_Transforms[0]->task_dim(); }

		protected:
			std::vector<SensorTransformPtr> m_Transforms;
			FloatVector m_Weights;


};

typedef boost::shared_ptr<WeightedSumSensorTransform>
	WeightedSumSensorTransformPtr;

} // namespace

#endif
