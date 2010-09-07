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

#ifndef CBF_SVD_HH
#define CBF_SVD_HH

#include <cbf/types.h>

namespace CBF {

    /** found here: http://www.crbond.com/download/misc/svd.c*/
    /*  svd.c -- Singular value decomposition. Translated to 'C' from the
     *           original Algol code in "Handbook for Automatic Computation,
     *           vol. II, Linear Algebra", Springer-Verlag.
     *
     *  (C) 2000, C. Bond. All rights reserved.
     *
     *  This is almost an exact translation from the original, except that
     *  an iteration counter is added to prevent stalls. This corresponds
     *  to similar changes in other translations.
     *
     *  Returns an error code = 0, if no errors and 'k' if a failure to
     *  converge at the 'kth' singular value.
     * 
     *  Changed for CBF-matrix-compatibility
     */
int svd(int withu, int withv, Float eps, Float tol, const FloatMatrix &a,
               FloatMatrix &q, FloatMatrix &u , FloatMatrix &v);
} //namespace

#endif
