#ifndef GRAPHICS_LIGHTS_H
#define GRAPHICS_LIGHTS_H

typedef enum eLightEnvironment {
	eCycles = 0,
	eRecognizer,
	eWorld
} eLightEnvironment;

void setupLights(eLightEnvironment eType);

#endif
