/** 
 *          File       :  wtypes.h
 *
 *          Contains   :  Definitions of types used by widgets
 *
 *          Version    :  1.0 ( 03/08/2001 )
 *
 *          Author     :  Nicolas Deniaud ( nicolas.deniaud@wanadoo.fr )
 *
 *          Part of gltron's project: http://www.gltron.org/
 */
#ifndef __WTYPES_H__
#define __WTYPES_H__



/** define a rect */
typedef struct Rect {
  short       top;
  short       left;
  short       bottom;
  short       right;
} Wrect;

/** define a cell */
typedef struct Point {
  short      v;
  short      h;
} Wpoint;

/** define a pointer */
typedef char *Wptr;
#endif
