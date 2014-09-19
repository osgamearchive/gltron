#include "video/video.h"
#include "game/game.h"
#include "game/game_level.h"
#include "game/resource.h"
#include "base/nebu_resource.h"
#include "configuration/settings.h"

#include "base/nebu_math.h"
#include "video/nebu_renderer_gl.h"

static float alpha = 0;

static float phase = 26.13;
static float a = 2;
static float b = 3;

const static float rec_scale_factor = 0.25f;
const static float rec_area_edge_factor = 0.85;

static float x(void) { return sinf(a * alpha + M_PI / 2 + phase); }
static float y(void) { return sinf(b * alpha + phase); }

static float dx(void) { return a * cosf(a * alpha + M_PI / 2 + phase); }
static float dy(void) { return b * cosf(b * alpha + phase); }

float getRecognizerAngle(vec2 *velocity)
{
  float dxval = velocity->v[0];
  float dyval = velocity->v[1];
  
  float phi = acosf ( dxval / sqrtf( dxval * dxval + dyval * dyval ) );
  if (dyval < 0) {
    phi = 2 * PI - phi;
  }
  return (phi + PI / 2) * 180 / PI;
}

void getRecognizerPositionVelocity(vec2 *p, vec2 *v)
{
	float rec_width = box2_Width(& game2->level->boundingBox) * rec_area_edge_factor;
    float rec_height = box2_Height(& game2->level->boundingBox) * rec_area_edge_factor;
	box2_Center(p, & game2->level->boundingBox);
    // it's += because we add the x/y to the bounding box center
	p->v[0] += x() * rec_width / 2.0f;
	p->v[1] += y() * rec_height / 2.0f;
	v->v[0] = dx();
	v->v[1] = dy();
}

void drawRecognizerShadow(void) {
	float dirx;
	vec2 p, v;
	/* states */

	glEnable(GL_CULL_FACE);

	/* transformations */
	getRecognizerPositionVelocity(&p, &v);
	dirx = getRecognizerAngle(&v);

	glPushMatrix();
	glMultMatrixf(shadow_matrix);
	glTranslatef( p.v[0], p.v[1], RECOGNIZER_HEIGHT);
	glRotatef(dirx, 0, 0, 1); /* up */
	glScalef(rec_scale_factor, rec_scale_factor, rec_scale_factor);
	glEnable(GL_NORMALIZE);

	/* render */

	gltron_Mesh_Draw(((gltron_Mesh*)resource_Get(gTokenRecognizer, eRT_GLtronTriMesh)), TRI_MESH);

	/* restore */

	glDisable(GL_CULL_FACE);
	glPopMatrix();
}

void drawRecognizer(void) {
  float dirx;
  vec2 p, v;

  glPushMatrix();

  /* transformations */
  getRecognizerPositionVelocity(&p, &v);
  dirx = getRecognizerAngle(&v);

  glTranslatef( p.v[0], p.v[1], RECOGNIZER_HEIGHT);
  glRotatef(dirx, 0, 0, 1); /* up */
  
  glScalef(rec_scale_factor, rec_scale_factor, rec_scale_factor); 
  
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHT1);
  glLightfv(GL_LIGHT2, GL_SPECULAR, rec_spec_color); 
  glEnable(GL_LIGHT2);

  glDisable(GL_BLEND);

  glEnable(GL_CULL_FACE);
  
  if (gSettingsCache.light_cycles) {
	glEnable(GL_LIGHTING);
  }
  
  glEnable(GL_NORMALIZE);
  glColor4f(0.0, 0.0, 0.0, 1.0f);
  gltron_Mesh_Draw((gltron_Mesh*)resource_Get(gTokenRecognizer, eRT_GLtronTriMesh), TRI_MESH);

  glDisable(GL_LIGHT2);
  glEnable(GL_LIGHT1);
  glDisable(GL_LIGHTING);

  glColor4f(
	  rec_outline_color[0],
	  rec_outline_color[1],
	  rec_outline_color[2],
	  1);

#ifndef OPENGL_ES // OK, GL_POLYGON_OFFSET_LINE
    glPolygonOffset(1,8);
  glEnable(GL_POLYGON_OFFSET_LINE);
  drawSharpEdges((gltron_Mesh*)resource_Get(gTokenRecognizer, eRT_GLtronTriMesh));
  glDisable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(0, 0);
#else
    drawSharpEdges((gltron_Mesh*)resource_Get(gTokenRecognizer, eRT_GLtronTriMesh));
#endif
    
  glDisable(GL_CULL_FACE);
  
  glPopMatrix();
}  

void doRecognizerMovement(void) {
  alpha += game2->time.dt / 5000.0f;
}

void resetRecognizer(void) {
	alpha = 0;
}




