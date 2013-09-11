# - Find sloccount
# Will define:
#
# SLOCCOUNT_EXECUTABLE - the sloccount binary
#
# Uses:
#
# SLOCCOUNT_ROOT - additional search hint for the binary
#
# Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation;
# either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#

INCLUDE(FindPackageHandleStandardArgs)

FIND_PROGRAM(SLOCCOUNT_EXECUTABLE sloccount HINTS ${SLOCCOUNT_ROOT})

FIND_PACKAGE_HANDLE_STANDARD_ARGS(sloccount DEFAULT_MSG SLOCCOUNT_EXECUTABLE)

# only visible in advanced view
MARK_AS_ADVANCED(SLOCCOUNT_EXECUTABLE)
