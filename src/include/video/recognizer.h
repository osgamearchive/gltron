#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include "Nebu_base.h"

extern void resetRecognizer();
extern void doRecognizerMovement();
extern void drawRecognizer();
extern void drawRecognizerShadow();
extern void getRecognizerPositionVelocity(Point *p, Point *v);

#endif
