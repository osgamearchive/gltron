#include "video/video.h"
#include "game/game.h"

#define TEX_SPLIT (1.0 - BOW_DIST2) / (1 - BOW_DIST1)
#undef TEX_SPLIT

static float normal1[] = { 1.0, 0.0, 0.0 };
static float normal2[] = { 0.0, 1.0, 0.0 };

/* 
   getDists returns the minimum distance from (the wall) *line to the
   specified (eye) point
   the z component is ignored
 */
float getDist(segment2 *s, float* eye) {
  float n[2];
  float tmp[2];
  n[0] = s->vStart.v[0] + s->vDirection.v[1];
  n[1] = s->vStart.v[1] - s->vDirection.v[0];
  tmp[0] = eye[0] - s->vStart.v[0];
  tmp[1] = eye[1] - s->vStart.v[1];
  if(n[0] == n[1] == 0) return length(tmp);
  return abs(scalarprod2(n, tmp) / length(n));
}

/*
  getSegmentEnd[XY]() returns the end point of the
  last trail segment line (before the lightcycles bow starts)
*/

float dists[] = { BOW_DIST2, BOW_DIST3, BOW_DIST1, 0 };

float getSegmentEndX(Data *data, int dist) {
  float tlength, blength;
	segment2 *s = data->trails + data->trailOffset;
	
  if(dirsX[data->dir] == 0) 
		return s->vStart.v[0] + s->vDirection.v[0];

  tlength = segment2_Length(s);
  blength = (tlength < 2 * BOW_LENGTH) ? tlength / 2 : BOW_LENGTH;
  return 
		s->vStart.v[0] + s->vDirection.v[0] -
		dists[dist] * blength * dirsX[ data->dir ];
}

float getSegmentEndY(Data *data, int dist) {
  float tlength, blength;
	segment2 *s = data->trails + data->trailOffset;

  if(dirsY[data->dir] == 0)
		return s->vStart.v[1] + s->vDirection.v[1];

  tlength = segment2_Length(s);
  blength = (tlength < 2 * BOW_LENGTH) ? tlength / 2 : BOW_LENGTH;
  return 
		s->vStart.v[1] + s->vDirection.v[1] -
		dists[dist] * blength * dirsY[ data->dir ];
}

/* getSegmentEndUV() calculates the texture coordinates for the last segment */
float getSegmentEndUV(segment2 *s, Data *data) {
  float tlength, blength;
  tlength = segment2_Length(s);
  blength = (tlength < 2 * BOW_LENGTH) ? tlength / 2 : BOW_LENGTH;
  return (tlength - 2 * blength) / DECAL_WIDTH;
}

/* getSegmentUV gets UV coordinates for an ordinary segment */
float getSegmentUV(segment2 *s) {
  return segment2_Length(s) / DECAL_WIDTH;
}

/* 
   drawTrailLines() draws a white line on top of each trail segment
   the alpha value is reduced with increasing distance to the player
*/

void drawTrailLines(Player *p, PlayerVisual *pV) {
  segment2 *s;
  float height;

  float *normal;
  float dist;
  float alpha;
  Data *data;
  Camera *cam;

  float trail_top[] = { 1.0, 1.0, 1.0, 1.0 };

  data = p->data;
  cam = p->camera;

  height = data->trail_height;
  if(height <= 0) return;

  /*
  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  */

  if (gSettingsCache.antialias_lines) {
    glEnable(GL_LINE_SMOOTH); /* enable line antialiasing */
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_LIGHTING);

  glBegin(GL_LINES);

  s = data->trails;
  while(s != data->trails + data->trailOffset) { 
		/* the current line is not drawn */
    /* compute distance from line to eye point */
    dist = getDist(s, cam->cam);
		alpha = (game2->rules.grid_size - dist / 2) / game2->rules.grid_size;
    // trail_top[3] = alpha;
    glColor4fv(trail_top);
    
    if(s->vDirection.v[1] == 0) normal = normal1;
    else normal = normal2;
    glNormal3fv(normal);
    glVertex3f(s->vStart.v[0],
							 s->vStart.v[1],
							 height);
    glVertex3f(s->vStart.v[0] + s->vDirection.v[0],
							 s->vStart.v[1] + s->vDirection.v[1],
							 height);
    s++;
    polycount++;
  }
  glEnd();

  /* compute distance from line to eye point */
  dist = getDist(s, cam->cam);
  alpha = (game2->rules.grid_size - dist / 2) / game2->rules.grid_size;
	// trail_top[3] = alpha;
  glColor4fv(trail_top);

  glBegin(GL_LINES);

	glVertex3f(s->vStart.v[0],
						 s->vStart.v[1],
						 height);
  glVertex3f( getSegmentEndX(data, 0),
	      getSegmentEndY(data, 0),
	      height );

  glEnd();

  /* glEnable(GL_LIGHTING); */
  glDisable(GL_BLEND);
  glDisable(GL_LINE_SMOOTH); /* disable line antialiasing */
  
  /*
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  */
}

/* 
   drawTrailShadow() draws a alpha-blended shadow on the floor for each
   trail segment.
   The light source source is (in homogenous coordinates)
   at (-1,-1,1,0) (I hope that's correct)
*/

void drawTrailShadow(Player* p, PlayerVisual *pV) {
  /* states */

  if(gSettingsCache.use_stencil) {
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_GREATER, 1, 1);
    glEnable(GL_BLEND);
    glColor4fv(shadow_color);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  } else {
    glColor3f(0, 0, 0);
    glDisable(GL_BLEND);
  }

  /* transformation */

  glPushMatrix();
  glMultMatrixf(shadow_matrix);

  /* geometry */
	{
		int vOffset = 0;
		int iOffset = 0;

		TrailMesh mesh;
		mesh.pVertices = (vec3*) malloc(1000 * sizeof(vec3));
		mesh.pNormals = (vec3*) malloc(1000 * sizeof(vec3));
		mesh.pColors = (unsigned char*) malloc(1000 * 4 * sizeof(float));
		mesh.pTexCoords = (vec2*) malloc(1000 * sizeof(vec2));
		mesh.pIndices = (unsigned short*) malloc(1000 * 2);
		mesh.iUsed = 0;
		
		trailGeometry(p, pV, &mesh, &vOffset, &iOffset);
		bowGeometry(p, pV, &mesh, &vOffset, &iOffset);
		trailStatesShadowed();
		trailRender(&mesh);
		// no states restore, because we're drawing shadowed geometry

		free(mesh.pVertices);
		free(mesh.pNormals);
		free(mesh.pColors);
		free(mesh.pTexCoords);
		free(mesh.pIndices);
	}

  /* restore */

  if(gSettingsCache.use_stencil)
    glDisable(GL_STENCIL_TEST);

  glDisable(GL_BLEND);

  glPopMatrix();
}

