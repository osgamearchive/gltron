#include "gltron.h"

void drawGlow(Player *p, gDisplay *d, float dim) {
  float mat[4*4];
  
  glPushMatrix();
  glTranslatef(p->data->posx,
	       p->data->posy,
	       0);

  glShadeModel(GL_SMOOTH);

  glDepthMask(GL_FALSE);
  glEnable(GL_DEPTH_TEST);

  glBlendFunc(GL_ONE, GL_ONE);
  glEnable(GL_BLEND);

  glGetFloatv(GL_MODELVIEW_MATRIX, mat);
  mat[0] = mat[5] = mat[10] = 1.0;
  mat[1] = mat[2] = 0.0;
  mat[4] = mat[6] = 0.0;
  mat[8] = mat[9] = 0.0;
  glLoadMatrixf(mat);
  glBegin(GL_TRIANGLE_FAN);
  glColor3fv(p->pColorDiffuse);

  glVertex3f(0,TRAIL_HEIGHT/2, 0);
  glColor4f(0,0,0,0.0);
  glVertex3f(dim*cos(-0.2*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(-0.2*3.1415/5.0), 0);
  glVertex3f(dim*cos(1.0*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(1.0*3.1415/5.0), 0);
  glVertex3f(dim*cos(2.0*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(2.0*3.1415/5.0), 0);
  glVertex3f(dim*cos(3.0*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(3.0*3.1415/5.0), 0);
  glVertex3f(dim*cos(4.0*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(4.0*3.1415/5.0), 0);
  glVertex3f(dim*cos(5.2*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(5.2*3.1415/5.0), 0);
  glEnd();
  polycount += 5;


  glBegin(GL_TRIANGLES);
  glColor3fv(p->pColorDiffuse);
  glVertex3f(0,TRAIL_HEIGHT/2, 0);
  glColor4f(0,0,0,0.0);
  glVertex3f(0,-TRAIL_HEIGHT/4,0);
  glVertex3f(dim*cos(-0.2*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(-0.2*3.1415/5.0), 0);

  glColor3fv(p->pColorDiffuse);
  glVertex3f(0,TRAIL_HEIGHT/2, 0);
  glColor4f(0,0,0,0.0);
  glVertex3f(dim*cos(5.2*3.1415/5.0),
	     TRAIL_HEIGHT/2+dim*sin(5.2*3.1415/5.0), 0);
  glVertex3f(0,-TRAIL_HEIGHT/4,0);
  glEnd();
  polycount += 3;

  glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);

  glShadeModel( game->screen->shademodel );
  glPopMatrix();  
}

void drawImpact(Player *p) {

  glDisable(GL_LIGHTING);
  glPushMatrix();

  glRotatef(90, 90, 0, 1);

  /*
   * Drop impact slightly below the floor and move it a tiny bit
   * back from the impact point so we don't draw behind the
   * trail/wall that's hit.
   */
  glTranslatef(0.0, -0.5, -0.5);

  glColor3f(0.68, 0, 0);
  
  drawExplosion(&(p->data->impact_radius));
  glPopMatrix();
  glEnable(GL_LIGHTING);
}

