/* -*- c -*- */
#ifndef INCLUDED_LIB3DS_QUAT_H
#define INCLUDED_LIB3DS_QUAT_H
/*
 * The 3D Studio File Format Library
 * Copyright (C) 1996-2001 by J.E. Hoffmann <je-h@gmx.net>
 * All rights reserved.
 *
 * This program is  free  software;  you can redistribute it and/or modify it
 * under the terms of the  GNU Lesser General Public License  as published by 
 * the  Free Software Foundation;  either version 2.1 of the License,  or (at 
 * your option) any later version.
 *
 * This  program  is  distributed in  the  hope that it will  be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or  FITNESS FOR A  PARTICULAR PURPOSE.  See the  GNU Lesser General Public  
 * License for more details.
 *
 * You should  have received  a copy of the GNU Lesser General Public License
 * along with  this program;  if not, write to the  Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: quat.h 1651 2004-05-31 08:01:30Z andi75 $
 */

#ifndef INCLUDED_LIB3DS_TYPES_H
#include "lib3ds/types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern LIB3DSAPI void lib3ds_quat_zero(Lib3dsQuat c);
extern LIB3DSAPI void lib3ds_quat_identity(Lib3dsQuat c);
extern LIB3DSAPI void lib3ds_quat_copy(Lib3dsQuat dest, Lib3dsQuat src);
extern LIB3DSAPI void lib3ds_quat_axis_angle(Lib3dsQuat c, Lib3dsVector axis, Lib3dsFloat angle);
extern LIB3DSAPI void lib3ds_quat_neg(Lib3dsQuat c);
extern LIB3DSAPI void lib3ds_quat_abs(Lib3dsQuat c);
extern LIB3DSAPI void lib3ds_quat_cnj(Lib3dsQuat c);
extern LIB3DSAPI void lib3ds_quat_mul(Lib3dsQuat c, Lib3dsQuat a, Lib3dsQuat b);
extern LIB3DSAPI void lib3ds_quat_scalar(Lib3dsQuat c, Lib3dsFloat k);
extern LIB3DSAPI void lib3ds_quat_normalize(Lib3dsQuat c);
extern LIB3DSAPI void lib3ds_quat_inv(Lib3dsQuat c);
extern LIB3DSAPI Lib3dsFloat lib3ds_quat_dot(Lib3dsQuat a, Lib3dsQuat b);
extern LIB3DSAPI Lib3dsFloat lib3ds_quat_squared(Lib3dsQuat c);
extern LIB3DSAPI Lib3dsFloat lib3ds_quat_length(Lib3dsQuat c);
extern LIB3DSAPI void lib3ds_quat_ln(Lib3dsQuat c);
extern LIB3DSAPI void lib3ds_quat_ln_dif(Lib3dsQuat c, Lib3dsQuat a, Lib3dsQuat b);
extern LIB3DSAPI void lib3ds_quat_exp(Lib3dsQuat c);
extern LIB3DSAPI void lib3ds_quat_slerp(Lib3dsQuat c, Lib3dsQuat a, Lib3dsQuat b, Lib3dsFloat t);
extern LIB3DSAPI void lib3ds_quat_squad(Lib3dsQuat c, Lib3dsQuat a, Lib3dsQuat p, Lib3dsQuat q,
  Lib3dsQuat b, Lib3dsFloat t);
extern LIB3DSAPI void lib3ds_quat_tangent(Lib3dsQuat c, Lib3dsQuat p, Lib3dsQuat q, Lib3dsQuat n);
extern LIB3DSAPI void lib3ds_quat_dump(Lib3dsQuat q);

#ifdef __cplusplus
};
#endif
#endif

