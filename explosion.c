#include "gltron.h"
#include "explosion.h" 

#define IMPACT_RADIUS_DELTA 0.025
#define IMPACT_MAX_RADIUS 25 

/* shockwave behavior constants */
#define SHOCKWAVE_MIN_RADIUS 0.0
#define SHOCKWAVE_MAX_RADIUS 45.0
#define SHOCKWAVE_WIDTH 0.2
#define SHOCKWAVE_SPACING 6.0
#define SHOCKWAVE_SPEED 1.2 /* speed relative to IMPACT_RADIUS_DELTA */
#define SHOCKWAVE_SEGMENTS 25
#define NUM_SHOCKWAVES 3 

/* draw a single semi-circular shockwave of a given radius */
static void drawWave(double radius) {
  int i, j;
  double angle;
  double delta_radius = SHOCKWAVE_WIDTH / SHOCKWAVE_SEGMENTS;
  double delta_angle = (180.0 / SHOCKWAVE_SEGMENTS) * (M_PI/180);
  double start_angle = 270.0 * (M_PI/180);
  
  for (i = 0; i < SHOCKWAVE_SEGMENTS; i++) {
    glBegin(GL_QUAD_STRIP);
    angle = start_angle;
    for (j = 0; j <= SHOCKWAVE_SEGMENTS; j++) {
      glVertex2d((radius + delta_radius) * sin(angle),
          (radius + delta_radius) * cos(angle));
      glVertex2d(radius * sin(angle), radius * cos(angle));
      angle += delta_angle;
    }
    glEnd();
    radius += delta_radius;
  }
}

static void drawShockwaves(float radius) {
  int waves;
  
  glColor3f(1,0,0);

  for(waves = 0; waves < NUM_SHOCKWAVES; waves++) {
    if (radius > SHOCKWAVE_MIN_RADIUS && radius < SHOCKWAVE_MAX_RADIUS) {
      drawWave(radius);
    }
    radius -= SHOCKWAVE_SPACING;
  }
}

/* impact spire behavior constants */
#define SPIRE_MAX_RADIUS 25
#define SPIRE_WIDTH 0.40
#define NUM_SPIRES 21 

static void drawSpires(float radius) {
  int i;
  float left[3], right[3];
  float zunit[3] = {0, 0, 1};
  
  float vectors[NUM_SPIRES][3] = {
    {  1.00,  0.20,  0.00  },
    {  0.80,  0.25,  0.00  },
    {  0.90,  0.50,  0.00  },
    {  0.70,  0.50,  0.00  },
    {  0.52,  0.45,  0.00  },
    {  0.65,  0.75,  0.00  },
    {  0.42,  0.68,  0.00  },
    {  0.40,  1.02,  0.00  },
    {  0.20,  0.90,  0.00  },
    {  0.08,  0.65,  0.00  },
    {  0.00,  1.00,  0.00  }, /* vertical spire */
    { -0.08,  0.65,  0.00  },
    { -0.20,  0.90,  0.00  },
    { -0.40,  1.02,  0.00  },
    { -0.42,  0.68,  0.00  },
    { -0.65,  0.75,  0.00  },
    { -0.52,  0.45,  0.00  },
    { -0.70,  0.50,  0.00  },
    { -0.90,  0.50,  0.00  },
    { -0.80,  0.30,  0.00  },
    { -1.00,  0.20,  0.00  }
  };

  glColor3f(1, 1, 1);
  glVertex3f(0, 0, 0);
 
  glBlendFunc(GL_ONE, GL_ONE);

  glBegin(GL_TRIANGLES);
  
  for (i=0; i < NUM_SPIRES; i++) {
    normcrossprod(vectors[i], zunit, right);
    normcrossprod(zunit, vectors[i], left);

    vmul(right, SPIRE_WIDTH);
    vmul(left, SPIRE_WIDTH);

    glColor4f(1,1,1,0.0);
    glVertex3fv(right);
    glVertex3f(radius * vectors[i][0], radius * vectors[i][1], 0.0);
    glVertex3fv(left);
  } 
  
  glEnd();
}

#define GLOW_START_OPACITY 1.2f
#define GLOW_INTENSITY 1.0f

static void drawImpactGlow(float glow_radius) {

  float opacity;
  
  opacity = GLOW_START_OPACITY - (glow_radius / IMPACT_MAX_RADIUS);
  
  glPushMatrix();
  glScalef(glow_radius, glow_radius, 1.0f);
  
  glBindTexture(GL_TEXTURE_2D, gScreen->textures[TEX_IMPACT]);
  glEnable(GL_TEXTURE_2D);

  glColor4f(GLOW_INTENSITY, GLOW_INTENSITY, GLOW_INTENSITY, opacity);
  glDepthMask(0);
  glBegin(GL_POLYGON);
  glTexCoord2f(0.0, 0.0); glVertex2f(-1.0, -1.0);
  glTexCoord2f(1.0, 0.0); glVertex2f(1.0, -1.0);
  glTexCoord2f(1.0, 1.0); glVertex2f(1.0, 1.0);
  glTexCoord2f(0.0, 1.0); glVertex2f(-1.0, 1.0);
  glEnd();
  glDepthMask(1);
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
}

void drawExplosion(float *radius) {

  float shockwave_radius = (*radius * SHOCKWAVE_SPEED);

  drawShockwaves(shockwave_radius);

  if (*radius < IMPACT_MAX_RADIUS) {
    drawImpactGlow(*radius);
    drawSpires(*radius);
  }
  
  *radius += game2->time.dt * IMPACT_RADIUS_DELTA;
}

