#include "gltron.h"

static float cam_park_pos[4][2] = {
  { 0.2, 0.2 },
  { 0.8, 0.2 },
  { 0.2, 0.8 },
  { 0.8, 0.8 }
};

static float park_height = 80.0;

static void writeCamDefaults(Camera *cam, int type) {
  cam_defaults[cam->type->type][type] = cam->movement[type];
}

#define CLAMP_R_MIN 5
#define CLAMP_R_MAX 45
#define CLAMP_CHI_MIN M_PI / 8
#define CLAMP_CHI_MAX 3 * M_PI / 8

static void clampCam(Camera *cam) {
  if(cam->type->freedom[CAM_FREE_R]) {
    if(cam->movement[CAM_R] < CLAMP_R_MIN)
      cam->movement[CAM_R] = CLAMP_R_MIN;
    if(cam->movement[CAM_R] > CLAMP_R_MAX)
      cam->movement[CAM_R] = CLAMP_R_MAX;
  }

  if(cam->type->freedom[CAM_FREE_CHI]) {
    if(cam->movement[CAM_CHI] < CLAMP_CHI_MIN)
      cam->movement[CAM_CHI] = CLAMP_CHI_MIN;
    if(cam->movement[CAM_CHI] > CLAMP_CHI_MAX)
      cam->movement[CAM_CHI] = CLAMP_CHI_MAX;
  }
}

static void initCircleCamera(Camera *cam) {
  cam->movement[CAM_R] = cam_defaults[CAM_CIRCLE][CAM_R];
  cam->movement[CAM_CHI] = cam_defaults[CAM_CIRCLE][CAM_CHI];
  cam->movement[CAM_PHI] = cam_defaults[CAM_CIRCLE][CAM_PHI];

  cam->type->interpolated_cam = 0;
  cam->type->interpolated_target = 0;
  cam->type->coupled = 0;
  cam->type->freedom[CAM_FREE_R] = 1;
  cam->type->freedom[CAM_FREE_PHI] = 0;
  cam->type->freedom[CAM_FREE_CHI] = 1;
}


static void initFollowCamera(Camera *cam) {
  cam->movement[CAM_R] = cam_defaults[CAM_FOLLOW][CAM_R];
  cam->movement[CAM_CHI] = cam_defaults[CAM_FOLLOW][CAM_CHI];
  cam->movement[CAM_PHI] = cam_defaults[CAM_FOLLOW][CAM_PHI];

  cam->type->interpolated_cam = 1;
  cam->type->interpolated_target = 0;
  cam->type->coupled = 1;
  cam->type->freedom[CAM_FREE_R] = 1;
  cam->type->freedom[CAM_FREE_PHI] = 1;
  cam->type->freedom[CAM_FREE_CHI] = 1;
}


static void initCockpitCamera(Camera *cam) {
  cam->movement[CAM_R] = cam_defaults[CAM_COCKPIT][CAM_R];
  cam->movement[CAM_CHI] = cam_defaults[CAM_COCKPIT][CAM_CHI];
  cam->movement[CAM_PHI] = cam_defaults[CAM_COCKPIT][CAM_PHI];

  cam->type->interpolated_cam = 0;
  cam->type->interpolated_target = 1;
  cam->type->coupled = 1;
  cam->type->freedom[CAM_FREE_R] = 0;
  cam->type->freedom[CAM_FREE_PHI] = 0;
  cam->type->freedom[CAM_FREE_CHI] = 0;
}


static void initFreeCamera(Camera *cam) {
  cam->movement[CAM_R] = cam_defaults[CAM_FREE][CAM_R];
  cam->movement[CAM_CHI] = cam_defaults[CAM_FREE][CAM_CHI];
  cam->movement[CAM_PHI] = cam_defaults[CAM_FREE][CAM_PHI];

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

      dest[0] = cam_park_pos[i][0] * game2->rules.grid_size;
      dest[1] = cam_park_pos[i][1] * game2->rules.grid_size;
    
      dest[2] = park_height;
      tdest[0] = game2->rules.grid_size / 2;
      tdest[1] = game2->rules.grid_size / 2;
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
	static Uint32 last=0;
	Uint32 dt;
	if(game2->time.dt)
	  dt=game2->time.dt;
	else
	  dt=SDL_GetTicks()-last;
	if(last) {
	  last=SDL_GetTicks();
	  if(game2->input.mouse1 == 1)
	    cam->movement[CAM_R] += (cam->movement[CAM_R]-CLAMP_R_MIN+1) * dt / 300.0;
	  if(game2->input.mouse2 == 1)
	    cam->movement[CAM_R] -= (cam->movement[CAM_R]-CLAMP_R_MIN+1) * dt / 300.0;
	  writeCamDefaults(cam, CAM_R);
	}
	last=SDL_GetTicks();
      }
      if(cam->type->freedom[CAM_FREE_PHI]) {
	cam->movement[CAM_PHI] += - game2->input.mousex * MOUSE_CX;
	writeCamDefaults(cam, CAM_CHI);
      }
      if(cam->type->freedom[CAM_FREE_CHI]) {
	cam->movement[CAM_CHI] += game2->input.mousey * MOUSE_CY;
	writeCamDefaults(cam, CAM_PHI);
      }
      clampCam(cam);

      /* ok, now let's calculate the new camera destination coordinates */
      /* also, perform some camera dependant movement */
      switch(cam->type->type) {
      case CAM_TYPE_CIRCLING: /* Andi-cam */
	cam->movement[CAM_PHI] += CAM_SPEED * game2->time.dt;
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

      phi = cam->movement[CAM_PHI];
      chi = cam->movement[CAM_CHI];
      r = cam->movement[CAM_R];

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

void nextCameraType(int console_message) {
  int i;
  int current_cam_type = getSettingi("camType");
  int new_cam_type = (current_cam_type + 1) % CAM_COUNT;
  
  setSettingi("camType", new_cam_type);

  /* update the cached setting */
  game2->settingsCache.camType = new_cam_type;
  
  for (i = 0; i < game->players; i++) {
    if (game->player[i].ai->active == AI_HUMAN) {
      initCamera(game->player[i].camera, game->player[i].data, new_cam_type);
    }
  }

  if (console_message) {
    switch (new_cam_type) {
      case 0 :
        displayMessage(TO_CONSOLE, "Circling Camera");
        break;
      case 1 :
        displayMessage(TO_CONSOLE, "Behind Camera");
        break;
      case 2 :
        displayMessage(TO_CONSOLE, "Cockpit Camera");
        break;
      case 3 :
        displayMessage(TO_CONSOLE, "Mouse Camera");
        break;
    }
  }
}

