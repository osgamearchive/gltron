#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include "basic_types.h"

extern void resetRecognizer();
extern void doRecognizerMovement();
extern void drawRecognizer();
extern void drawRecognizerShadow();
extern void getRecognizerPositionVelocity(Point *p, Point *v);

#endif
