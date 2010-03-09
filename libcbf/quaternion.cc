#include <cbf/quaternion.h>
#include <cbf/debug_macros.h>

namespace CBF {

#define EPSILON         0.001
#define PID2            1.570796326794896619231322

std::ostream &operator<<(std::ostream &str, const Quaternion &q) {
	str << "(" << q.w << "," << q.x << "," << q.y << "," << q.z << ")";
	return str;
}

/** 
	Spherical linear interpolation. Taking two unit quaternions on the unit sphere, slerp 
	interpolates between the two quaternions. Implemented as Eq 3.52 in Realtime rendering book.

	\todo Review SLERP algorithm..
*/
Quaternion qslerp(Quaternion q, Quaternion r, Float t)
{
	Float cos_theta = q.dot(r);
	if (cos_theta < 0) r = r * -1;

	Float scaleQ, scaleR;
	// Ensure t is in the range [0,1]
	if ( t < 0 ) t = 0;
	else if ( t > 1 ) t = 1;

	// Check for unit quaternion - remove it later as it should not have been here..
	if ( q.length() != 1 ) q.normalize();
	if ( r.length() != 1 ) r.normalize();

	cos_theta = q.dot(r);
	Float theta = acos(cos_theta);
	Float invSin = 1.0 / sin(theta);

	// Check for inverting the rotation
	Quaternion val;

	// Travel along the shorter path. Ref : Adv Anim. by Watt & Watt
	if ( (1.0 + cos_theta) > EPSILON )
	{
		CBF_DEBUG("short path")
		// If angle is not small use SLERP.
		if ( (1.0 - cos_theta) > EPSILON )
		{
			scaleQ = sin( (1.0 -t)*theta ) * invSin ;
			scaleR = sin(t*theta) * invSin;
		}
		else    // For small angles use LERP
		{
			// std::cout << " are we using LERP " << std::endl;
			scaleQ = 1.0 - t;
			scaleR = t;
		}
		val = q * scaleQ  + r * scaleR ;
	}
	else // This is a long way
	{
		CBF_DEBUG("long path")
		// Clear the concept later...
		val.assign(r.z, -r.y, r.x, -r.w);
		scaleQ = sin( (1.0 - t)* PID2 );
		scaleR = sin( t * PID2 );
		val*=scaleR;
		q*=scaleQ;
		val +=val;
	}
	val.normalize();
	return val;
}


} // namespace
