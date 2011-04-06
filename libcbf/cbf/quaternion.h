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

#ifndef CBF_QUATERNION_HH
#define CBF_QUATERNION_HH

#include <cbf/types.h>

#include <boost/numeric/ublas/matrix.hpp>

namespace CBF {



/**
	@brief A utility class representing a unit quaternion. 

	Provides some operations and conversion methods.
*/
struct Quaternion {
	Float 
		w,
		x,
		y,
		z;

	Quaternion(
		Float w = 0,
		Float x = 1,
		Float y = 0,
		Float z = 0
	) :
		w(w),
		x(x),
		y(y),
		z(z)
	{

	}

	/**
		This constructor expects a (minimum) 3x3 orthogonal matrix. Additional
		components are ignored..
	*/
	Quaternion(const FloatMatrix &m)
	{
		*this = m;
	}

	/**
		Create Quaternion from 4D vector. See corresponding assignment operator for details.
	*/
	
	Quaternion(const FloatVector &v)
	{
		*this = v;
	}

	Float dot(const Quaternion& q_rhs) const
	{
		return (
			w * q_rhs.w
			+ x * q_rhs.x
			+ y * q_rhs.y
			+ z * q_rhs.z
		);
	}

	inline Float length() const
	{
		return sqrt(w*w + x*x + y*y + z*z);
	}

	inline Quaternion& normalize()
	{
		Float len = length();

		if ( len > 0 )
			*this *= 1.0/len;

		return *this;
	}

	inline void assign(Float w_, Float x_, Float y_, Float z_)
	{
		w = w_; x = x_; y = y_; z = z_;
	}

	Quaternion operator*(const Quaternion& q_rhs)
	{
		return Quaternion(
			w * q_rhs.w - x * q_rhs.x - y * q_rhs.y - z * q_rhs.z,
			w * q_rhs.x + x * q_rhs.w + y * q_rhs.z - z * q_rhs.y,
			w * q_rhs.y + y * q_rhs.w + z * q_rhs.x - x * q_rhs.z,
			w * q_rhs.z + z * q_rhs.w + x * q_rhs.y - y * q_rhs.x
		).normalize();
	}

	// operator functions
	inline Quaternion operator+(const Quaternion& q_rhs) const
	{
		return Quaternion(w + q_rhs.w, x + q_rhs.x, y + q_rhs.y, z + q_rhs.z);
	}
	
	inline Quaternion operator-(const Quaternion& q_rhs) const
	{
		return Quaternion(w - q_rhs.w, x - q_rhs.x, y - q_rhs.y, z - q_rhs.z);
	}
	
	// Create the new object and return it, instead of changing sign for given object. Flexibility !?
	inline Quaternion operator-() const
	{
		return Quaternion(-w, -x, -y, -z);
	}
	
	inline Quaternion& operator *=(Float scale)
	{
		w *= scale; 
		x *= scale;    y *= scale;    z *= scale;
		return *this;
	}

	inline Quaternion operator *(const Float scale)
	{
		return Quaternion(w*scale, x*scale, y*scale, z*scale);
	}
	
	inline Quaternion& operator /=(Float scale)
	{
		w /= scale; 
		x /= scale;    y /= scale;    z /= scale;
		return *this;
	}

	inline Quaternion operator /(Float scale) const
	{
		return Quaternion(w/scale, x/scale, y/scale, z/scale);
	}
	
	inline Quaternion& operator+=(const Quaternion& q_rhs)
	{
		w += q_rhs.w; 
		x += q_rhs.x; y += q_rhs.y; z += q_rhs.z;
		return *this;
	}

	inline Quaternion& operator-=(const Quaternion& q_rhs)
	{
		w -= q_rhs.w; 
		x -= q_rhs.x; y -= q_rhs.y; z -= q_rhs.z;
		return *this;
	}

	/**
		Assignment operator from vector type (dim: 4). This does not do any conversion.
		It just copies the values over.
	*/
	Quaternion &operator=(const FloatVector &v) {
		w = v[0];
		x = v[1];
		y = v[2];
		z = v[3];
		return *this;
	}

	#define CBF_QUAT_AXIS_THRESH 0.00001
	Quaternion &from_axis_angle3(const FloatVector &v) {
		//FIXME: Float norm = ublas::norm_2(v);
		Float norm = v.norm();
		w = cos(0.5 * norm);
		Float sin_a = sin(0.5 * norm);

		if (norm > CBF_QUAT_AXIS_THRESH) {
			x = v[0] * sin_a / norm;
			y = v[1] * sin_a / norm;
			z = v[2] * sin_a / norm;
		} else {
			x = y = z = 0;
		}

		//normalize();
		return *this;
	}

	/** Precondition: ret must have size() == 3 */
	void to_axis_angle3(FloatVector &ret) {
		Float angle = acos(w) * 2.0;
		// CBF_DEBUG("angle: " << angle)
		ret.resize(3);
		ret[0] = x;
		ret[1] = y;
		ret[2] = z;
		//FIXME: Float norm = ublas::norm_2(ret);
		Float norm = ret.norm();
		// CBF_DEBUG("norm " << norm)
		if (norm > CBF_QUAT_AXIS_THRESH) {
			ret *= 1.0 / norm;
			ret *= angle;
		}
		else
			ret *= 0.0;

		// CBF_DEBUG("ret: " << ret)
	}

	/**
		Abuses the components to write the axis into x,y,z and the angle in w.
	*/
	Quaternion &axis_angle()
	{
#if 0
		normalize();
		Float cos_a = w;
		Float angle = acos( cos_a ) * 2.0;
		Float sin_a = sqrt( 1.0 - cos_a * cos_a );
		if (fabs(sin_a) < 0.0001) sin_a = 1.0;
		x = x / sin_a;
		y = y / sin_a;
		z = z / sin_a;
		w = angle;
#endif

#if 0
		normalize();
		if (w < -1) w = -1;
		if (w > 1) w = 1;
		Float angle = acos(w) * 2.0;
		w = angle;
		if (angle == 0) { 
			x = 0; y = 0; z = 1.0; 
		}
		else {
			Float norm = sqrt(x*x + y*y + z*z);
			x /= norm;
			y /= norm;
			z /= norm;
		}
#endif

#if 0
		else {
			x *= 1.0 / sin(angle/2.0);
			y *= 1.0 / sin(angle/2.0);
			z *= 1.0 / sin(angle/2.0);
		}
#endif

#if 0
		Float scale = sqrt(x * x + y * y + z * z);

		if (scale < 0.00000001) {
			x = 1; y = z = 0;
			w = 0;
		} else {
			x = x / scale;
			y = y / scale;
			z = z / scale;

			//if (w < -1) w = -1;
			//if (w > 1) w = 1;

			w = acos(w) * 2.0;
		}
#endif
		return *this;
	}

	/** Setup this quaternion from an unpacked axis angle representation */
	void from_axis_angle4(Quaternion &q)
	{
		Float sinAngle;
		Float half_angle = 0.5 * q.w;
		sinAngle = sin(half_angle);

		w = 0; x = q.x; y = q.y; z = q.z;
		normalize();

		x = (x * sinAngle);
		y = (y * sinAngle);
		z = (z * sinAngle);
		w = cos(half_angle);
	}

	/** Assignment operator from matrix type (3x3)*/
	Quaternion &operator=(const FloatMatrix &m) {
		//FloatMatrix m = ublas::trans(m_);
		Float trace = m(0,0) + m(1,1) + m(2,2) + 1.0;

		if( trace > 0.0001 )
		{
			// CBF_DEBUG("[Quaternion]: trace > 0")
			Float s = 0.5 / sqrt(trace);
			w = 0.25 / s;
			x = ( m(2,1) - m(1,2) ) * s;
			y = ( m(0,2) - m(2,0) ) * s;
			z = ( m(1,0) - m(0,1) ) * s;
		}
		else
		{
			// CBF_DEBUG("[Quaternion]: trace < 0")
			if ( m(0,0) > m(1,1) && m(0,0) > m(2,2) )
			{
				Float s = 2.0 * sqrt( 1.0 + m(0,0) - m(1,1) - m(2,2) );
				x = 0.25f * s;
				y = (m(0,1) + m(1,0) ) / s;
				z = (m(0,2) + m(2,0) ) / s;
				w = (m(2,1) - m(1,2) ) / s;

			}
			else if (m(1,1) > m(2,2))
			{
				Float s = 2.0 * sqrt( 1.0 + m(1,1) - m(0,0) - m(2,2) );
				x = (m(0,1) + m(1,0) ) / s;
				y = 0.25 * s;
				z = (m(1,2) + m(2,1) ) / s;
				w = (m(0,2) - m(2,0) ) / s;
			}
			else
			{
				Float s = 2.0 * sqrt( 1.0 + m(2,2) - m(0,0) - m(1,1) );
				x = (m(0,2) + m(2,0) ) / s;
				y = (m(1,2) + m(2,1) ) / s;
				z = 0.25 * s;
				w = (m(1,0) - m(0,1) ) / s;
			}
		}
		//std::cout << "[Quaternion]: w, x, y, z: " << w << " " << x << " " << y << " " << z << std::endl;
		//this->conjugate();
		return (*this);
	}

	Quaternion &conjugate() {
		x = -x; y = -y; z = -z;
		return *this;
	}

	/**
		Convert Quaternion to ublas::vector. This does not do any conversion. It just copies the values over.
	*/
	operator FloatVector () {
		FloatVector v(4);
		v[0] = w;
		v[1] = x;
		v[2] = y;
		v[3] = z;
		return v;
	}

	/**
		A conversion operator to create a 3x3 rotation matrix from this quaternion..
	*/
	operator FloatMatrix () {
		//FIXME: FloatMatrix m = ublas::zero_matrix<Float>(3,3);
		FloatMatrix m = FloatMatrix::Zero(3,3);

		m(0,0) = 1.0 - 2.0 * (y*y + z*z);
		m(1,0) = 2.0 * x * y + 2.0 * z * w;
		m(2,0) = 2.0 * x * z - 2.0 * y * w;
		m(0,1) = 2.0 * x * y - 2.0 * z * w;
		m(1,1) = 1.0 - 2.0 * (x*x + z*z);
		m(2,1) = 2.0 * y * z + 2.0 * x * w;
		m(0,2) = 2.0 * x * z + 2.0 * y * w;
		m(1,2) = 2.0 * y * z - 2.0 * x * w;
		m(2,2) = 1.0 - 2.0 * (x*x + y*y);

		return m;
	}
};

Quaternion qslerp(Quaternion q, Quaternion r, Float t);

/**
	An operator to be able to print a quaternion into an ostream
*/
std::ostream &operator<<(std::ostream &str, const Quaternion &q);

} // namespace

#endif
