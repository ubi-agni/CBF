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

#include <cbf/combination_strategy.h>
#include <cbf/debug_macros.h>
#include <cbf/xml_factory.h>

namespace CBF {

	#ifdef CBF_HAVE_XSD
		AddingStrategy::AddingStrategy(const CBFSchema::AddingStrategy &xml_instance) {
			CBF_DEBUG("yay!!")
		}

		template<> XMLBaseFactory<CombinationStrategy, CBFSchema::CombinationStrategy>* 
			XMLBaseFactory<CombinationStrategy, CBFSchema::CombinationStrategy>::m_Instance = 0;

		static XMLDerivedFactory<AddingStrategy, CBFSchema::AddingStrategy, CombinationStrategy, CBFSchema::CombinationStrategy> x;
	#endif

} // namespace

