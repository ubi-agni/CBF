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

#include <cbf/quaternion.h>
#include <cbf/debug_macros.h>

namespace CBF {

#define EPSILON         0.001

std::ostream &operator<<(std::ostream &str, const Quaternion &q) {
	str << "(" << q.w << "," << q.x << "," << q.y << "," << q.z << ")";
	return str;
}

/** 
	Spherical linear interpolation. Taking two unit quaternions on the unit sphere, slerp 
	interpolates between the two quaternions. Implemented as Eq 3.52 in Realtime rendering book.

	\todo Review SLERP algorithm..
*/
Quaternion qslerp(const Quaternion &q, const Quaternion &r, Float t)
{
	Float cos_theta = q.dot(r);
	Float scaleQ, scaleR = 1;

	// switch to other half sphere?
	if (cos_theta < 0) {
		scaleR = -1;
		cos_theta *= -1;
	}

	// Ensure t is in the range [0,1]
	if ( t < 0 ) t = 0;
	else if ( t > 1 ) t = 1;

	Float theta = acos(cos_theta);
	Float invSin = 1.0 / sin(theta);

	// If angle is not small use SLERP.
	if ( (1.0 - cos_theta) > EPSILON )
	{
		scaleQ = sin((1.0-t) * theta) * invSin;
		scaleR *= sin(t*theta) * invSin;
	}
	else // For small angles use LERP
	{
		scaleQ = 1.0 - t;
		scaleR *= t;
	}
	return q * scaleQ  + r * scaleR ;
}


} // namespace
