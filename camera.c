#include "gltron.h"

static float cam_park_pos[4][2] = {
  { 0.2, 0.2 },
  { 0.8, 0.2 },
  { 0.2, 0.8 },
  { 0.8, 0.8 }
};

static float park_height = 80.0;

void initCircleCamera(Camera *cam) {
  cam->movement->r = cam_defaults[CAM_CIRCLE][CAM_R];
  cam->movement->chi = cam_defaults[CAM_CIRCLE][CAM_CHI];
  cam->movement->phi = cam_defaults[CAM_CIRCLE][CAM_PHI];

  cam->type->interpolated_cam = 0;
  cam->type->interpolated_target = 0;
  cam->type->coupled = 0;
  cam->type->freedom[CAM_FREE_R] = 1;
  cam->type->freedom[CAM_FREE_PHI] = 0;
  cam->type->freedom[CAM_FREE_CHI] = 1;
}


void initFollowCamera(Camera *cam) {
  cam->movement->r = cam_defaults[CAM_FOLLOW][CAM_R];
  cam->movement->chi = cam_defaults[CAM_FOLLOW][CAM_CHI];
  cam->movement->phi = cam_defaults[CAM_FOLLOW][CAM_PHI];
  cam->movement->r = CAM_FOLLOW_DIST;
  cam->movement->chi = M_PI / 4;
  cam->movement->phi = M_PI / 72;

  cam->type->interpolated_cam = 1;
  cam->type->interpolated_target = 0;
  cam->type->coupled = 1;
  cam->type->freedom[CAM_FREE_R] = 1;
  cam->type->freedom[CAM_FREE_PHI] = 1;
  cam->type->freedom[CAM_FREE_CHI] = 1;
}


void initCockpitCamera(Camera *cam) {
  cam->movement->r = cam_defaults[CAM_COCKPIT][CAM_R];
  cam->movement->chi = cam_defaults[CAM_COCKPIT][CAM_CHI];
  cam->movement->phi = cam_defaults[CAM_COCKPIT][CAM_PHI];
  cam->movement->r = CAM_COCKPIT_Z;
  cam->movement->chi = 0;
  cam->movement->phi = 0;

  cam->type->interpolated_cam = 0;
  cam->type->interpolated_target = 1;
  cam->type->coupled = 1;
  cam->type->freedom[CAM_FREE_R] = 0;
  cam->type->freedom[CAM_FREE_PHI] = 0;
  cam->type->freedom[CAM_FREE_CHI] = 0;
}


void initFreeCamera(Camera *cam) {
  cam->movement->r = cam_defaults[CAM_FREE][CAM_R];
  cam->movement->chi = cam_defaults[CAM_FREE][CAM_CHI];
  cam->movement->phi = cam_defaults[CAM_FREE][CAM_PHI];

  cam->type->interpolated_cam = 0;
  cam->type->interpolated_target = 0;
  cam->type->coupled = 0;
  cam->type->freedom[CAM_FREE_R] = 1;
  cam->type->freedom[CAM_FREE_PHI] = 1;
  cam->type->freedom[CAM_FREE_CHI] = 1;
}

void initCamera(Camera *cam, Data *data, int type) {
  cam->type->type = type;

  switch(cam->type->type) {
  case CAM_TYPE_CIRCLING: initCircleCamera(cam); break;
  case CAM_TYPE_FOLLOW: initFollowCamera(cam); break;
  case CAM_TYPE_COCKPIT: initCockpitCamera(cam); break;
  case CAM_TYPE_MOUSE: initFreeCamera(cam); break;
  }
  cam->target[0] = data->posx;
  cam->target[1] = data->posy;
  cam->target[2] = 0;

  cam->cam[0] = data->posx + CAM_CIRCLE_DIST;
  cam->cam[1] = data->posy;
  cam->cam[2] = CAM_CIRCLE_Z;
}

void interpolate(float *dest, float *source) {
  /* horrible
  int i;
  for(i = 0; i < 3; i++)
    dest[i] = source[i] + 0.1 * (dest[i] - source[i]);
  */

  /* also horrible
  float d, dcamx, dcamy;
  int dt;

  dt = game2->time.dt;

  d = sqrt((dest[0] - source[0]) * (dest[0] - source[0]) +
	   (dest[1] - source[1]) * (dest[1] - source[1]));
  if(d != 0) {
    dcamx = (float)dt * CAM_FOLLOW_SPEED_FACTOR *
      game->settings->current_speed * (dest[0] - source[0]) / d;
    dcamy = (float)dt * CAM_FOLLOW_SPEED_FACTOR *
      game->settings->current_speed * (dest[1] - source[1]) / d;

    if((dest[0] - source[0] > 0 && dest[0] - source[0] < dcamx) ||
       (dest[0] - source[0] < 0 && dest[0] - source[0] > dcamx)) {
      source[0] = dest[0];
    } else source[0] += dcamx;

    if((dest[1] - source[1] > 0 && dest[1] - source[1] < dcamy) ||
       (dest[1] - source[1] < 0 && dest[1] - source[1] > dcamy)) {
      source[1] = dest[1];
    } else source[1] += dcamy;
  }
  source[2] = dest[2];
  */
}
  
void doCameraMovement() {
  int i;
  Camera *cam;
  Data *data;
  Player *p;

  for(i = 0; i < game->players; i++) {
      
    cam = game->player[i].camera;
    data = game->player[i].data;
    p = game->player + i;

    if(data->speed == SPEED_GONE) {
      /* observer camera */
      float dest[3];
      float tmp[3];
      float tdest[3];

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
      float dest[3];
      float tdest[3];
      float phi, chi, r;
      /* first, process all movement commands */
      /* that means, check for mouse input mainly */
      if(cam->type->freedom[CAM_FREE_R]) {
	if(game2->input.mouse1 == 1)
	  cam->movement->r += CAM_DR * game2->time.dt / 1000.0;
	if(game2->input.mouse2 == 1)
	  cam->movement->r -= CAM_DR * game2->time.dt / 1000.0;
      }
      if(cam->type->freedom[CAM_FREE_PHI]) {
	cam->movement->phi += - game2->input.mousex * MOUSE_CX;
      }
      if(cam->type->freedom[CAM_FREE_CHI]) {
	cam->movement->chi += game2->input.mousey * MOUSE_CY;
      }
      /* ok, now let's calculate the new camera destination coordinates */
      /* also, perform some camera dependant movement */
      switch(cam->type->type) {
      case CAM_TYPE_CIRCLING: /* Andi-cam */
	cam->movement->phi += CAM_SPEED * game2->time.dt;
	tdest[0] = data->posx;
	tdest[1] = data->posy;
	tdest[2] = B_HEIGHT;
	break;
      case CAM_TYPE_FOLLOW: /* Mike-cam */
	tdest[0] = data->posx;
	tdest[1] = data->posy;
	tdest[2] = B_HEIGHT;
	break;
      case CAM_TYPE_COCKPIT: /* 1st person */
	tdest[0] = data->posx + dirsX[data->dir];
	tdest[1] = data->posy + dirsY[data->dir];
	tdest[2] = CAM_COCKPIT_Z;
	/*
	cam->cam[0] = data->posx;
	cam->cam[1] = data->posy;
	cam->cam[2] = CAM_COCKPIT_Z + 0.5;
	*/
	break;
      case CAM_TYPE_MOUSE: /* mouse camera */
	tdest[0] = data->posx;
	tdest[1] = data->posy;
	tdest[2] = B_HEIGHT;
	break;
      }

      phi = cam->movement->phi;
      chi = cam->movement->chi;
      r = cam->movement->r;

      if(cam->type->coupled) {
	int time;
	time = game2->time.current - p->data->turn_time;
	if(time < TURN_LENGTH) {
	  int dir, ldir;
	  dir = p->data->dir;
	  ldir = p->data->last_dir;
	  if(dir == 1 && ldir == 2)
	    dir = 4;
	  if(dir == 2 && ldir == 1)
	    ldir = 4;
	  phi += ((TURN_LENGTH - time) * camAngles[ldir] + 
		  time * camAngles[dir]) / TURN_LENGTH;
	}
	else
	  phi += camAngles[p->data->dir];
      }
	
      /* move camera and perform necessary interpolations */
      dest[0] = data->posx + r * cos(phi) * sin(chi);
      dest[1] = data->posy + r * sin(phi) * sin(chi);
      dest[2] = r * cos(chi);

      memcpy(cam->cam, dest, sizeof(cam->cam));
      memcpy(cam->target, tdest, sizeof(cam->target));
    }
  }

  /* mouse events consumed */
  game2->input.mousex = 0;
  game2->input.mousey = 0;

}
