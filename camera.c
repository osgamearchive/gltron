#include "gltron.h"

static float cam_park_pos[4][2] = {
  { 0.2, 0.2 },
  { 0.8, 0.2 },
  { 0.2, 0.8 },
  { 0.8, 0.8 }
};

static float park_height = 80.0;

void chaseCamMove() {
  int i;
  Camera *cam;
  Data *data;
  float dest[3];
  float tdest[3];
  float dcamx;
  float dcamy;
  float d, dt;

  dt = game2->time.dt;

  for(i = 0; i < game->players; i++) {
      
    cam = game->player[i].camera;
    data = game->player[i].data;
    
    if(data->speed == SPEED_GONE) {
      float tmp[3];
    
      dest[0] = cam_park_pos[i][0] * game->settings->grid_size;
      dest[1] = cam_park_pos[i][1] * game->settings->grid_size;
    
      dest[2] = park_height;
      tdest[0] = game->settings->grid_size / 2;
      tdest[1] = game->settings->grid_size / 2;
      tdest[2] = -10;
      vsub(dest, cam->cam, tmp);
      if(length(tmp) > 1) {
	normalize(tmp);
	vadd(cam->cam, tmp, cam->cam);
      } else
	vcopy(dest, cam->cam);
      vsub(tdest, cam->target, tmp);
      if(length(tmp) > 1) {
	normalize(tmp);
	vadd(cam->target, tmp, cam->target);
      } else 
	vcopy(tdest, cam->target);
    } else {
      
      switch(cam->camType) {
      case 0: /* Andi-cam */
	cam->cam[0] = data->posx + CAM_CIRCLE_DIST * COS(camAngle);
	cam->cam[1] = data->posy + CAM_CIRCLE_DIST * SIN(camAngle);
	cam->cam[2] = CAM_CIRCLE_Z;
	cam->target[0] = data->posx;
	cam->target[1] = data->posy;
	cam->target[2] = B_HEIGHT;
	break;
      case 1: /* Mike-cam */
	cam->target[0] = data->posx;
	cam->target[1] = data->posy;
	cam->target[2] = B_HEIGHT;
      
	dest[0] = cam->target[0] - CAM_FOLLOW_DIST * dirsX[ data->dir];
	dest[1] = cam->target[1] - CAM_FOLLOW_DIST * dirsY[ data->dir];

	d = sqrt((dest[0] - cam->cam[0]) * (dest[0] - cam->cam[0]) +
		 (dest[1] - cam->cam[1]) * (dest[1] - cam->cam[1]));
	if(d != 0) {
	  /*
	    dcamx = (float)dt * CAM_FOLLOW_SPEED * (dest[0] - cam->cam[0]) / d;
	    dcamy = (float)dt * CAM_FOLLOW_SPEED * (dest[1] - cam->cam[1]) / d;
	  */
	  dcamx = (float)dt * CAM_FOLLOW_SPEED_FACTOR *
	    game->settings->current_speed * (dest[0] - cam->cam[0]) / d;
	  dcamy = (float)dt * CAM_FOLLOW_SPEED_FACTOR *
	    game->settings->current_speed * (dest[1] - cam->cam[1]) / d;

	  if((dest[0] - cam->cam[0] > 0 && dest[0] - cam->cam[0] < dcamx) ||
	     (dest[0] - cam->cam[0] < 0 && dest[0] - cam->cam[0] > dcamx)) {
	    cam->cam[0] = dest[0];
	  } else cam->cam[0] += dcamx;

	  if((dest[1] - cam->cam[1] > 0 && dest[1] - cam->cam[1] < dcamy) ||
	     (dest[1] - cam->cam[1] < 0 && dest[1] - cam->cam[1] > dcamy)) {
	    cam->cam[1] = dest[1];
	  } else cam->cam[1] += dcamy;
	}
	break;
      case 2: /* 1st person */
#define H 3
	cam->target[0] = data->posx + dirsX[data->dir];
	cam->target[1] = data->posy + dirsY[data->dir];
	cam->target[2] = H;

	cam->cam[0] = data->posx;
	cam->cam[1] = data->posy;
	cam->cam[2] = H + 0.5;
#undef H
	break;
      }
    }
  }
}

void camMove() {
  camAngle += CAM_SPEED * game2->time.dt / 100;
  while(camAngle > 360) camAngle -= 360;
}

