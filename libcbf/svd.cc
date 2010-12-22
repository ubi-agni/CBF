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


    Copyright 2009, 2010 Florian Paul Schmidt, Viktor Richter
*/

#include <cbf/svd.h>
#include <cbf/config.h>
#include <cbf/debug_macros.h>

namespace CBF {

int svd(
	int withu, int withv, Float eps, Float tol, const FloatMatrix &a,
	FloatMatrix &q,FloatMatrix &u ,FloatMatrix &v
) {
	int i,j,k,l(0),l1,iter,retval;
	Float c,f,g,h,s,x,y,z;

	int m = a.size1();
	int n = a.size2();

	FloatVector e(n);

	q *= 0.0;
	u *= 0.0;
	v *= 0.0;
	//q.clear();
	u.clear();
	v.clear();

	//e = (Float *)calloc(n,sizeof(Float));
	retval = 0;

	/* Copy 'a' to 'u' */		
	for (i=0;i<m;i++) {
		for (j=0;j<n;j++){
			u(i,j) = a(i,j);
		}
	}

	/* Householder's reduction to bidiagonal form. */
	g = x = 0.0;		
	for (i=0;i<n;i++) {
		e[i] = g;
		s = 0.0;
		l = i+1;
		for (j=i;j<m;j++)
			s += (u(j,i)*u(j,i));
		if (s < tol)
			g = 0.0;
		else {
			f = u(i,i);
			g = (f < 0) ? sqrt(s) : -sqrt(s);
			h = f * g - s;
			u(i,i) = f - g;
			for (j=l;j<n;j++) {
				s = 0.0;
				for (k=i;k<m;k++)
					s += (u(k,i) * u(k,j));
				f = s / h;
				for (k=i;k<m;k++)
					u(k,j) += (f * u(k,i));
			} /* end j */
		} /* end s */
		q(i,i) = g;
		s = 0.0;
		for (j=l;j<n;j++)
			s += (u(i,j) * u(i,j));
		if (s < tol)
			g = 0.0;
		else {
			f = u(i,i+1);
			g = (f < 0) ? sqrt(s) : -sqrt(s);
			h = f * g - s;
			u(i,i+1) = f - g;
			for (j=l;j<n;j++) 
				e[j] = u(i,j)/h;
			for (j=l;j<m;j++) {
				s = 0.0;
				for (k=l;k<n;k++) 
					s += (u(j,k) * u(i,k));
				for (k=l;k<n;k++)
					u(j,k) += (s * e[k]);
			} /* end j */
		} /* end s */
		y = fabs(q(i,i)) + fabs(e[i]);												 
		if (y > x)
			x = y;
	} /* end i */
	 /* accumulation of right-hand transformations */
	if (withv) {
		for (i=n-1;i>=0;i--) {
			if (g != 0.0) {
			h = u(i,i+1) * g;
			for (j=l;j<n;j++)
				v(j,i) = u(i,j)/h;
			for (j=l;j<n;j++) {
				s = 0.0;
				for (k=l;k<n;k++) 
					s += (u(i,k) * v(k,j));
				for (k=l;k<n;k++)
					v(k,j) += (s * v(k,i));

			} /* end j */
			} /* end g */
			for (j=l;j<n;j++)
				v(i,j) = v(j,i) = 0.0;
			v(i,i) = 1.0;
			g = e[i];
			l = i;
		} /* end i */

	} /* end withv, parens added for clarity */

	/* accumulation of left-hand transformations */
	if (withu) {
		for (i=n;i<m;i++) {
			for (j=n;j<m;j++) {
				u(i,j) = 0.0;
			}
			u(i,i) = 1.0;
		}
	}

	if (withu) {
		for (i=n-1;i>=0;i--) {
			l = i + 1;
			g = q(i,i);
			for (j=l;j<m;j++){	/* upper limit was 'n' */
				u(i,j) = 0.0;
			}
			if (g != 0.0) {
				h = u(i,i) * g;
				for (j=l;j<m;j++) { /* upper limit was 'n' */
					s = 0.0;
					for (k=l;k<m;k++){
						s += (u(k,i) * u(k,j));
					}
					f = s / h;
					for (k=i;k<m;k++){
						u(k,j) += (f * u(k,i));
					}
				} /* end j */
				for (j=i;j<m;j++){
					u(j,i) /= g;
				}
			} /* end g */
			else {
				for (j=i;j<m;j++){
					u(j,i) = 0.0;
				}
			}
			u(i,i) += 1.0;
		} /* end i*/
	} /* end withu, parens added for clarity */

	/* diagonalization of the bidiagonal form */
	eps *= x;
	for (k=n-1;k>=0;k--) {
		iter = 0;
		test_f_splitting:
		for (l=k;l>=0;l--) {
			if (fabs(e[l]) <= eps) goto test_f_convergence;
			if (fabs(q(l-1,l-1)) <= eps) goto cancellation;
		} /* end l */

		/* cancellation of e[l] if l > 0 */
	cancellation:
		c = 0.0;
		s = 1.0;
		l1 = l - 1;
		for (i=l;i<=k;i++) {
			f = s * e[i];
			e[i] *= c;
			if (fabs(f) <= eps) goto test_f_convergence;
			g = q(i,i);
			h = q(i,i) = sqrt(f*f + g*g);
			c = g / h;
			s = -f / h;
			if (withu) {
				for (j=0;j<m;j++) {
					y = u(j,l1);
					z = u(j,i);
					u(j,l1) = y * c + z * s;
					u(j,i) = -y * s + z * c;
				} /* end j */
			} /* end withu, parens added for clarity */
		} /* end i */
		test_f_convergence:
		z = q(k,k);
		if (l == k) goto convergence;
		/* shift from bottom 2x2 minor */
		iter++;
		if (iter > 30) {
			retval = k;
			break;
		}
		x = q(l,l);
		y = q(k-1,k-1);
		g = e[k-1];
		h = e[k];
		f = ((y-z)*(y+z) + (g-h)*(g+h)) / (2*h*y);
		g = sqrt(f*f + 1.0);
		f = ((x-z)*(x+z) + h*(y/((f<0)?(f-g):(f+g))-h))/x;
		/* next QR transformation */ 
		c = s = 1.0;
		for (i=l+1;i<=k;i++) {
			g = e[i];
			y = q(i,i);
			h = s * g;
			g *= c;
			e[i-1] = z = sqrt(f*f+h*h);
			c = f / z;
			s = h / z;
			f = x * c + g * s;
			g = -x * s + g * c;
			h = y * s;
			y *= c;	
			if (withv) {
				for (j=0;j<n;j++) {
					x = v(j,i-1);		
					z = v(j,i);	 
					v(j,i-1) = x * c + z * s;
					v(j,i) = -x * s + z * c;
				} /* end j */
			} /* end withv, parens added for clarity */
			q(i-1,i-1) = z = sqrt(f*f + h*h);
			c = f/z;
			s = h/z;
			f = c * g + s * y;
			x = -s * g + c * y;
			if (withu) {
				for (j=0;j<m;j++) {
					y = u(j,i-1);
					z = u(j,i);
					u(j,i-1) = y * c + z * s;
					u(j,i) = -y * s + z * c;
				} /* end j */
			} /* end withu, parens added for clarity */
		} /* end i */
		e[l] = 0.0;
		e[k] = f;
		q(k,k) = x;
		goto test_f_splitting;
		convergence:
		if (z < 0.0) {
			/* q[k] is made non-negative */
			q(k,k) = - z;
			if (withv) {
				for (j=0;j<n;j++)
					v(j,k) = -v(j,k);
			} /* end withv, parens added for clarity */
		} /* end z */
	} /* end k */

	CBF_DEBUG("q: " << q);
	CBF_DEBUG("u: " << u);
	CBF_DEBUG("v: " << v);

	return retval;
}	// C++ fallback for SVD with boost matrices
} //namespace
