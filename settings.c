#include "gltron.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 100
#define MAX_VAR_NAME_LEN 64

/* this just screams to be replaced by scripting code */

void settings_artpack(char *buf, FILE *f) {
  if(f != NULL) {
    fprintf(f, "vset artpack %s\n", artpack_list[artpack_index]);
  } else {
    char pack[256];
    char **artpack;
    int i = 0;
    fprintf(stderr, "reading artpack setting\n");
    sscanf(buf, "vset artpack %s ", pack);
    for(artpack = artpack_list; *artpack != NULL; artpack++) {
      if(strstr(pack, *artpack) == pack) {
	artpack_index = i;
	return;
      }
      i++;
    }
    fprintf(stderr, "artpack '%s' not found - setting default\n", pack);
    artpack_index = 0;
  }
}

void settings_cam_settings(char *buf, FILE *f) {
  int i;
  if(f != NULL) {
    fprintf(f, "vset cam_settings ");
    for(i = 0; i < 4; i++)
      fprintf(f, "%s %.3f %.3f %.3f ", cam_names[i], 
	      cam_defaults[i][0], cam_defaults[i][1], cam_defaults[i][2]);
    fprintf(f, "\n");
  } else {
    char pattern[256];
    fprintf(stderr, "reading camera setting\n");
    for(i = 0; i < 4; i++) {
      char *camera;
      camera = strstr(buf, cam_names[i]); 
      if(camera  != NULL) {
	sprintf(pattern, "%s %%f %%f %%f ", cam_names[i]);
	sscanf(camera, pattern, cam_defaults[i], 
	       cam_defaults[i] + 1, cam_defaults[i] + 2);
      } else {
	fprintf(stderr, "can't find camera %s in %s", cam_names[i], buf);
      }
    }
  }
}
	    
void settings_key_actions(char *buf, FILE *f) {
  int i, tmp[8];
  if(f != NULL) {
    fprintf(f, "vset keys ");
    for(i = 0; i < 8; i++)
      fprintf(f, "%d ", key_actions[i].key);
    fprintf(f, "\n");
  } else {
    sscanf(buf, "vset keys %d %d %d %d %d %d %d %d ", 
	   tmp + 0, tmp + 1, tmp + 2, tmp + 3, tmp + 4,
	   tmp + 5, tmp + 6, tmp + 7);
    for(i = 0; i < 8; i++)
      key_actions[i].key = tmp[i];
  }
}

void settings_cycle_colors_diffuse(char *buf, FILE *f) {
  int i, t;
  float tmp[4];
  if(f != NULL) {
    for(i = 0; i < MAX_PLAYERS; i++)
      fprintf(f, "vset cycle_colors_diffuse %d %.3f %.3f %.3f %.3f\n", i,
	      colors_model_diffuse[i][0],
	      colors_model_diffuse[i][1],
	      colors_model_diffuse[i][2],
	      colors_model_diffuse[i][3]);
  } else {
    sscanf(buf, "vset cycle_colors_diffuse %d %f %f %f %f ", &t,
	   tmp + 0, tmp + 1, tmp + 2, tmp + 3);
    if(t < MAX_PLAYERS)
      memcpy(colors_model_diffuse[t], tmp, sizeof(tmp));
    else
      fprintf(stderr, "error reading colors: unknown player %d\n", t);
  }
}

void settings_cycle_colors_specular(char *buf, FILE *f) {
  int i, t;
  float tmp[4];
  if(f != NULL) {
    for(i = 0; i < MAX_PLAYERS; i++)
      fprintf(f, "vset cycle_colors_specular %d %.3f %.3f %.3f %.3f\n", i,
	      colors_model_specular[i][0],
	      colors_model_specular[i][1],
	      colors_model_specular[i][2],
	      colors_model_specular[i][3]);
  } else {
    sscanf(buf, "vset cycle_colors_specular %d %f %f %f %f ", &t,
	   tmp + 0, tmp + 1, tmp + 2, tmp + 3);
    if(t < MAX_PLAYERS)
      memcpy(colors_model_specular[t], tmp, sizeof(tmp));
    else
      fprintf(stderr, "error reading colors: unknown player %d\n", t);
  }
}

void settings_trail_colors(char *buf, FILE *f) {
  int i, t;
  float tmp[4];
  if(f != NULL) {
    for(i = 0; i < MAX_PLAYERS; i++)
      fprintf(f, "vset trail_colors %d %.3f %.3f %.3f %.3f\n", i,
	      colors_alpha[i][0],
	      colors_alpha[i][1],
	      colors_alpha[i][2],
	      colors_alpha[i][3]);
  } else {
    sscanf(buf, "vset trail_colors %d %f %f %f %f ", &t,
	   tmp + 0, tmp + 1, tmp + 2, tmp + 3);
    if(t < MAX_PLAYERS)
      memcpy(colors_alpha[t], tmp, sizeof(tmp));
    else
      fprintf(stderr, "error reading colors: unknown player %d\n", t);
  }
}

void initSettingData(char *filename) {
  FILE *f;
  int n, i, count, j;
  char buf[BUFSIZE];
  char c;

  f = fopen(filename, "r");
  fgets(buf, BUFSIZE, f);
  sscanf(buf, "%d ", &n);
  for(i = 0; i < n; i++) {
    fgets(buf, BUFSIZE, f);
    sscanf(buf, "%c%d ", &c, &count);
    switch(c) {
    case 'i': /* it's int */
      si = malloc(sizeof(struct settings_int) * count);
      si_count = count;
      for(j = 0; j < count; j++) {
	fgets(buf, BUFSIZE, f);
	buf[MAX_VAR_NAME_LEN - 1] = 0;
	sscanf(buf, "%s ", (si + j)->name);
      }
      break;
    case 'f': /* float */
      sf = malloc(sizeof(struct settings_float) * count);
      sf_count = count;
      for(j = 0; j < count; j++) {
	fgets(buf, BUFSIZE, f);
	buf[MAX_VAR_NAME_LEN - 1] = 0;
	sscanf(buf, "%s ", (sf + j)->name);
      }
      break;
    case 'v': /* void */
      sv = malloc(sizeof(struct settings_v) * count);
      sv_count = count;
      for(j = 0; j < count; j++) {
	fgets(buf, BUFSIZE, f);
	buf[MAX_VAR_NAME_LEN - 1] = 0;
	sscanf(buf, "%s ", (sv + j)->name);
      }
      break;
    default:
      printf("unrecognized type '%c' in settings.txt\n", c);
      exit(1);
    }
  }
  fclose(f);

  si[0].value = &(game->settings->show_help);
  si[1].value = &(game->settings->show_fps);
  si[2].value = &(game->settings->show_wall);
  si[3].value = &(game->settings->show_glow);
  si[4].value = &(game->settings->show_2d);
  si[5].value = &(game->settings->show_alpha);
  si[6].value = &(game->settings->show_floor_texture);
  si[7].value = &(game->settings->line_spacing);
  si[8].value = &(game->settings->erase_crashed);
  si[9].value = &(game->settings->fast_finish);
  si[10].value = &(game->settings->fov);
  si[11].value = &(game->settings->width);
  si[12].value = &(game->settings->height);
  si[13].value = &(game->settings->show_ai_status);
  si[14].value = &(game->settings->camType);
  si[15].value = &(game->settings->display_type);
  si[16].value = &(game->settings->playMusic);
  si[17].value = &(game->settings->show_model);
  si[18].value = &(game->settings->ai_player1);
  si[19].value = &(game->settings->ai_player2);
  si[20].value = &(game->settings->ai_player3);
  si[21].value = &(game->settings->ai_player4);
  si[22].value = &(game->settings->show_crash_texture);
  si[23].value = &(game->settings->turn_cycle);
  si[24].value = &(game->settings->mouse_warp);
  si[25].value = &(game->settings->windowMode);
  si[26].value = &(game->settings->model_backwards);
  si[27].value = &(game->settings->grid_size);
  si[28].value = &(game->settings->playEffects);
  si[29].value = &(game->settings->use_mipmaps);
  si[30].value = &(game->settings->game_speed);
  si[31].value = &(game->settings->arena_size);
  si[32].value = &(game->settings->alpha_trails);
  si[33].value = &(game->settings->lod);
  si[34].value = &(game->settings->ai_level);
  si[35].value = &(game->settings->mipmap_filter);
  si[36].value = &(game->settings->stretch_textures);
  si[37].value = &(game->settings->show_skybox);
  si[38].value = &(game->settings->bitdepth_32);
  si[39].value = &(game->settings->show_decals);
  si[40].value = &(game->settings->show_scores);
  si[41].value = &(game->settings->shadow_lod);
  si[42].value = &(game->settings->show_gl_logo);
  si[43].value = &(game->settings->show_recognizer);

  sf[0].value = &(game->settings->current_speed);
  sf[1].value = &(game->settings->musicVolume);
  sf[2].value = &(game->settings->fxVolume);

  sv[0].value = settings_key_actions;
  sv[1].value = settings_cycle_colors_diffuse;
  sv[2].value = settings_cycle_colors_specular;
  sv[3].value = settings_trail_colors;
  sv[4].value = settings_cam_settings;
  sv[5].value = settings_artpack;
}

int* getVi(char* name) {
  int i;
  for(i = 0; i < si_count; i++) {
    if(strstr(name, si[i].name) == name) 
      return si[i].value;
  }
  return NULL;
}

/*
void* getVv(char *name) {
  int i;
  for(i = 0; i < sv_count; i++) {
    if(strstr(name, sv[i].name) == name)
      return sv[i].value;
  }
  return NULL;
}
*/

float* getVf(char* name) {
  int i;
  for(i = 0; i < sf_count; i++) {
    if(strstr(name, sf[i].name) == name) 
      return sf[i].value;
  }
  return NULL;
}

void loadIniFile(char *fname) {
  char buf[100];
  char expbuf[100];
  int i;
  FILE* f;

  f = fopen(fname, "r");
  if(f == NULL) {
    fprintf(stderr, "can't load ini file '%s'\n", fname);
    return;
  }

  while(fgets(buf, sizeof(buf), f)) {
    /* process rc-file */

    if(strstr(buf, "iset") == buf) {
      /* linear search through settings */
      /* first: integer */
      for(i = 0; i < si_count; i++) {
	sprintf(expbuf, "iset %s ", si[i].name);
	if(strstr(buf, expbuf) == buf) {
	  sscanf(buf + strlen(expbuf), "%d ", si[i].value);
	  /* printf("assignment: %s\t%d\n", si[i].name, *(si[i].value)); */
	  break;
	}
      }
    } else if(strstr(buf, "fset") == buf) {
      for(i = 0; i < sf_count; i++) {
	sprintf(expbuf, "fset %s ", sf[i].name);
	if(strstr(buf, expbuf) == buf) {
	  sscanf(buf + strlen(expbuf), "%f ", sf[i].value);
	  /* printf("assignment: %s\t%.2f\n", sf[i].name, *(sf[i].value)); */
	  break;
	}
      }
    } else if(strstr(buf, "vset") == buf) {
      for(i = 0; i < sv_count; i++) {
	sprintf(expbuf, "vset %s ", sv[i].name);
	if(strstr(buf, expbuf) == buf)
	  sv[i].value(buf, NULL);
      }
    }
  }
  fclose(f);
}

void initDefaultSettings() {
  game->pauseflag = 0;

  game->settings->show_help = 0;
  game->settings->show_fps = 0;
  game->settings->show_wall = 1;
  game->settings->show_glow = 1;
  game->settings->show_2d = 0;
  game->settings->show_alpha = 1;
  game->settings->light_cycles = 1;
  game->settings->alpha_trails = 0;
  game->settings->show_skybox = 0;
  game->settings->show_floor_texture = 1;
  game->settings->show_crash_texture = 1;
  game->settings->show_decals = 1;
  game->settings->show_model = 1;
  game->settings->show_recognizer = 0;
  game->settings->lod = 0;
  game->settings->shadow_lod = 2;
  game->settings->turn_cycle = 1;
  game->settings->line_spacing = 20;
  game->settings->erase_crashed = 1;
  game->settings->fast_finish = 1;
  game->settings->fov = 105;
  game->settings->znear = 0.5;
  game->settings->current_speed = 7.0;
  game->settings->game_speed = 2; /* fast */
  /* game->settings->grid_size MUST be divisible by 8 */
  game->settings->arena_size = 3;
  game->settings->grid_size = 600;
  game->settings->width = 640;
  game->settings->height = 480;
  game->settings->show_ai_status = 0;
  game->settings->show_scores = 1;
  game->settings->show_gl_logo = 1;
  game->settings->camType = 1;
  game->settings->mouse_warp = 0;
  game->settings->windowMode = 0;
  game->settings->model_backwards = 0;
  game->settings->use_mipmaps = 1;
  game->settings->mipmap_filter = TRILINEAR;
  game->settings->stretch_textures = 0;
  game->settings->softwareRendering = 0;
  game->settings->bitdepth_32 = 0;

  game->settings->display_type = 3;
  game->settings->playMusic = 1;
  game->settings->playEffects = 1;

  game->settings->musicVolume = 0.3;
  game->settings->fxVolume = 0.7;

  game->settings->ai_player1 = 0;
  game->settings->ai_player2 = 1;
  game->settings->ai_player3 = 1;
  game->settings->ai_player4 = 1;

  game->settings->ai_level = 2;
  
  /* not included in .gltronrc */

  game->settings->screenSaver = 0;

  game->settings->content[0] = 0;
  game->settings->content[1] = 1;
  game->settings->content[2] = 2;
  game->settings->content[3] = 3;

  game->settings->grid_size = default_arena_sizes[game->settings->arena_size];

  /* choose speed */
  default_speeds[4] = game->settings->current_speed;
  game->settings->current_speed = default_speeds[ game->settings->game_speed ];

}

void initMainGameSettings(char *filename) {
  char *fname;
  char *home;

  game2 = &main_game2;
  game = &main_game;
  game->settings = (Settings*) malloc(sizeof(Settings));
  initSettingData(filename);

  /* initialize some struct members */

  game2->input.mouse1 = 0;
  game2->input.mouse2 = 0;
  game2->input.mousex = 0;
  game2->input.mousey = 0;
  game2->network.status = 0;

  /* initialize defaults, then load modifications from file */
  initDefaultSettings();

  /* go for .gltronrc (or whatever is defined in RC_NAME) */

  home = getenv("HOME"); /* find homedir */
  if(home == NULL) {
    fname = malloc(strlen(CURRENT_DIR) + strlen(RC_NAME) + 2);
    sprintf(fname, "%s%c%s", CURRENT_DIR, SEPERATOR, RC_NAME);
  } else {
    fname = malloc(strlen(home) + strlen(RC_NAME) + 2);
    sprintf(fname, "%s%c%s", home, SEPERATOR, RC_NAME);
  }

  loadIniFile(fname);
  free(fname);

  /* sanity check: speed, grid_size */
  if(game->settings->current_speed <= 0) {
    fprintf(stderr, "[gltron] sanity check failed: speed = %.2ff\n",
	    game->settings->current_speed);
    game->settings->current_speed = 6.0;
    fprintf(stderr, "[gltron] reset speed: speed = %.2f\n",
	    game->settings->current_speed);
  }
  if(game->settings->grid_size % 8) {
    fprintf(stderr, "[gltron] sanity check failed: grid_size %% 8 != 0: "
	    "grid_size = %d\n", game->settings->grid_size);
    game->settings->grid_size = 240;
    fprintf(stderr, "[gltron] reset grid_size: grid_size = %d\n",
	    game->settings->grid_size);
  }

}

void saveSettings() {
  char *fname;
  char *home;
  int i;
  FILE* f;

  home = getenv("HOME"); /* find homedir */
  if(home == NULL) {
    fname = malloc(strlen(CURRENT_DIR) + strlen(RC_NAME) + 2);
    sprintf(fname, "%s%c%s", CURRENT_DIR, SEPERATOR, RC_NAME);
  } else {
    fname = malloc(strlen(home) + strlen(RC_NAME) + 2);
    sprintf(fname, "%s%c%s", home, SEPERATOR, RC_NAME);
  }
  f = fopen(fname, "w");
  if(f == NULL) {
    printf("can't open %s ", fname);
    perror("for writing");
    return; /* can't write rc */
  }
  for(i = 0; i < si_count; i++)
    fprintf(f, "iset %s %d\n", si[i].name, *(si[i].value));
  for(i = 0; i < sf_count; i++)
    fprintf(f, "fset %s %.2f\n", sf[i].name, *(sf[i].value));
  for(i = 0; i < sv_count; i++) {
    (sv[i].value)(NULL, f);
  }
  printf("written settings to %s\n", fname);
  free(fname);
  fclose(f);
}
