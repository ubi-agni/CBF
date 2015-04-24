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
/*
 * quaternion_potential.h
 *
 * This code is implemented based on [1]
 *  [1] A. Ude et al. "Orientation in Cartesian space ...", ICRA, 2014
 *
 *  Created on April. 21, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#ifndef CBF_QUATERNION_POTENTIAL_H
#define CBF_QUATERNION_POTENTIAL_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/potential.h>
#include <cbf/namespace.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class QuaternionPotential; }

namespace CBF {
	
  struct QuaternionPotential : public Potential {
    QuaternionPotential(const CBFSchema::QuaternionPotential &xml_instance, ObjectNamespacePtr object_namespace);

    QuaternionPotential() :
      Potential()
    {

    }

    virtual void gradient (
      FloatVector &result,
      const std::vector<FloatVector > &references,
      const FloatVector &input
    );

    virtual void integration (
        FloatVector &nextpos,
        const FloatVector &currentpos,
        const FloatVector &taskvel,
        const Float timestep
    );

    virtual unsigned int sensor_dim() const { return 4u; }

    virtual unsigned int task_dim() const { return 3u; }

		virtual Float distance(const FloatVector &v1, const FloatVector &v2);

    virtual Float norm(const FloatVector &v) { return v.norm(); }
	};

  typedef boost::shared_ptr<QuaternionPotential> QuaternionPotentialPtr;

} // namespace

#endif //CBF_QUATERNION_POTENTIAL_H


