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

#include <cbf/plugin_pool.h>
#include <cbf/controller.h>

#include <string>
#include <stdexcept>

#include <schemas.hxx>

int main(int argc, char *argv[]) {
	if (argc < 2) throw std::runtime_error
		("Too few arguments. Please be a bit more verbose!");

	for (int i = 1; i < argc; ++i) {
		std::string filename(argv[i]);

		CBF::PluginPool<CBF::Controller> *pp = 
			CBF::PluginPool<CBF::Controller>::get_instance();

		CBF::ControllerPtr c = 
			pp->create_from_file<ControllerType>(filename);

		while (c->step() == false) ;
	}

	return 0;
}
