#include "video/video.h"

void setupLights(int type) {
	float white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	// float gray22[] = { 0.22f, 0.22f, 0.22f, 1.0f };
	float gray66[] = { 0.66f, 0.66f, 0.66f, 1.0f };
	float gray10[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	float black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	
	float posWorld0[] = { 1, .8f, 0, 0 };
	float posWorld1[] = { -1, -.8f, 0, 0 };

	float posCycles0[] = { 0, 0, 0, 1 };
	
	int i;
	
	// turn global ambient lighting off
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);

	switch(type) {
	case eWorld:
		glEnable(GL_LIGHTING);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		// glLoadIdentity();
		
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, posWorld0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, gray10);
		// glLightfv(GL_LIGHT0, GL_AMBIENT, black);
		glLightfv(GL_LIGHT0, GL_SPECULAR, white);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, white);

		glEnable(GL_LIGHT1);
		glLightfv(GL_LIGHT1, GL_POSITION, posWorld1);
		glLightfv(GL_LIGHT1, GL_AMBIENT, black);
		glLightfv(GL_LIGHT1, GL_SPECULAR, gray66);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, gray66);
		
		for(i = 2; i < 8; i++)
				glDisable(GL_LIGHT0 + i);
		
		glPopMatrix();

		break;
	case eCycles:
	case eRecognizer:
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		
		glLightfv(GL_LIGHT0, GL_POSITION, posCycles0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, gray10);
		glLightfv(GL_LIGHT0, GL_SPECULAR, white);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, white);

		for(i = 1; i < 8; i++)
				glDisable(GL_LIGHT0 + i);
		
		glPopMatrix();

		break;
	}
}		
