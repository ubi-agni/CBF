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
 * multiple_controller.h
 *
 * Multiple controller includes different primitive controllers for different hardwares
 *
 *  Created on May 20, 2015
 *          by Seungsu Kim (skim@techfak.uni-bielefeld.de)
 */

#ifndef MULTIPLE_CONTROLLER_H
#define MULTIPLE_CONTROLLER_H

#include <cbf/config.h>
#include <cbf/types.h>
#include <cbf/utilities.h>
#include <cbf/primitive_controller.h>
#include <cbf/exceptions.h>
#include <cbf/namespace.h>
#include <cbf/composite_resource.h>

#include <boost/shared_ptr.hpp>

namespace CBFSchema { class MultipleController; }

namespace CBF {

  struct MultipleController : public Object {
      MultipleController(const CBFSchema::MultipleController &xml_instance, ObjectNamespacePtr object_namespace);

      MultipleController(std::vector<PrimitiveControllerPtr > controllers = std::vector<PrimitiveControllerPtr >());

      void push_back(PrimitiveControllerPtr controller) { m_PrimitiveControllers.push_back(controller); }

      PrimitiveControllerPtr at(unsigned int index) { return m_PrimitiveControllers.at(index); }

      unsigned int size() { return m_PrimitiveControllers.size(); }

      virtual void update();

      void get_resource(unsigned int resource_id);

      void get_controller_resource(unsigned int controller_id);

    protected :
      std::vector<PrimitiveControllerPtr > m_PrimitiveControllers;

      CompositeResourcePtr m_Resource;

  };

  typedef boost::shared_ptr<MultipleController > MultipleControllerPtr;

} // namespace


#endif // MULTIPLE_CONTROLLERS_H
