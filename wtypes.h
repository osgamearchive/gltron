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
