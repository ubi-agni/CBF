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

#ifndef CBF_COMP_RESOURCE_HH
#define CBF_COMP_RESOURCE_HH

#include <cbf/resource.h>

#include <vector>

namespace CBF {

/** @brief: A class used to combine several resources into a single one */
struct CompositeResource : public Resource
{

	protected:
		std::vector<ResourcePtr> m_Resources;
};

} // namespace

#endif

