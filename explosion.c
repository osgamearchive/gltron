#include "gltron.h"
#include "explosion.h" 

/* shockwave behavior constants */
#define SHOCKWAVE_MIN_RADIUS 0.0
#define SHOCKWAVE_MAX_RADIUS 45.0
#define SHOCKWAVE_WIDTH 0.2
#define SHOCKWAVE_SPACING 6.0
#define SHOCKWAVE_SPEED 0.50 
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

void drawShockwaves(float *radius) {
  float rad = *radius;
  int waves;
  
  glColor3f(1,0,0);

  for(waves = 0; waves < NUM_SHOCKWAVES; waves++) {
    if (rad > SHOCKWAVE_MIN_RADIUS && rad < SHOCKWAVE_MAX_RADIUS) {
      drawWave(rad);
    }
    rad -= SHOCKWAVE_SPACING;
  }
  *radius += SHOCKWAVE_SPEED;
}

/* impact spire behavior constants */
#define SPIRE_SPEED 0.60
#define SPIRE_MAX_RADIUS 25
#define SPIRE_WIDTH 0.50
#define NUM_SPIRES 21 

void drawSpires(float *spire_radius) {
  int i;
  float sp_rad = *spire_radius;  
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

  if (sp_rad > SPIRE_MAX_RADIUS)
    return;

  glColor3f(1, 1, 1);
  glVertex3f(0, 0, 0);
 
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  

  glShadeModel(GL_SMOOTH);
     
  glBlendFunc(GL_ONE, GL_ONE);
  
  glBegin(GL_TRIANGLES);
  
  for (i=0; i < NUM_SPIRES; i++) {
    normcrossprod(vectors[i], zunit, right);
    normcrossprod(zunit, vectors[i], left);

    vmul(right, SPIRE_WIDTH);
    vmul(left, SPIRE_WIDTH);

    glColor4f(1,1,1,0.0);
    glVertex3fv(right);
    glVertex3f(sp_rad * vectors[i][0], sp_rad * vectors[i][1], 0.0);
    glVertex3fv(left);
  } 
  
  glEnd();
  
  *spire_radius += SPIRE_SPEED;
}

