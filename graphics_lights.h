#ifndef GRAPHICS_LIGHTS_H
#define GRAPHICS_LIGHTS_H

extern void setupLights(int type);

enum lighTypes {
	eCycles = 0,
	eRecognizer,
	eWorld
};

#endif
