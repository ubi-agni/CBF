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

/* -*- mode: c-non-suck; -*- */

#ifndef CBF_SPACENAVI_REFERENCE_HH
#define CBF_SPACENAVI_REFERENCE_HH

#include <stdexcept>

#include <sys/stat.h>
#include <fcntl.h>

#include <cbf/reference.h>
#include <cbf/types.h>

namespace CBFSchema { class SpaceNaviReference; }

namespace CBF {
	
	/** @brief: A reference that takes its information from a SpaceMouse */
	struct SpaceNaviReference : public Reference {
		SpaceNaviReference(const CBFSchema::SpaceNaviReference &xml_instance,
								 ObjectNamespacePtr object_namespace);
	
		SpaceNaviReference()
		{
			init();
		}

		virtual ~SpaceNaviReference();
	
		virtual void update();

		virtual unsigned int dim() {
			return 6u;
		}
	
		/**
			@brief Utility function for the constructor.
		*/
		void init();

		protected:
			void *m_Device;
	};
	
} // namespace

#endif
