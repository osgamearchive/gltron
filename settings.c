#include "gltron.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 100
#define MAX_VAR_NAME_LEN 64

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

  sf[0].value = &(game->settings->speed);
}

int* getVi(char* name) {
  int i;
  for(i = 0; i < si_count; i++) {
    if(strstr(name, si[i].name) == name) 
      return si[i].value;
  }
  return 0;
}

void initMainGameSettings(char *filename) {
  char *fname;
  char *home;
  char buf[100];
  char expbuf[100];
  int i;
  FILE* f;

  game = &main_game;
  game->settings = (Settings*) malloc(sizeof(Settings));
  initSettingData(filename);

  /* initialize defaults, then load modifications from file */

  game->pauseflag = 0;

  game->settings->show_help = 0;
  game->settings->show_fps = 1;
  game->settings->show_wall = 1;
  game->settings->show_glow = 1;
  game->settings->show_2d = 0;
  game->settings->show_alpha = 1;
  game->settings->alpha_trails = 0;
  game->settings->show_floor_texture = 1;
  game->settings->show_crash_texture = 1;
  game->settings->show_model = 1;
  game->settings->turn_cycle = 1;
  game->settings->line_spacing = 20;
  game->settings->erase_crashed = 0;
  game->settings->fast_finish = 1;
  game->settings->fov = 105;
  game->settings->speed = 7.0;
  game->settings->game_speed = 1; /* normal */
  /* game->settings->grid_size MUST be divisible by 8 */
  game->settings->arena_size = 1;
  game->settings->grid_size = 240;
  game->settings->width = 640;
  game->settings->height = 480;
  game->settings->show_ai_status = 1;
  game->settings->camType = 0;
  game->settings->mouse_warp = 0;
  game->settings->windowMode = 0;
  game->settings->model_backwards = 0;
  game->settings->use_mipmaps = 1;

  game->settings->display_type = 0;
  game->settings->playMusic = 1;
  game->settings->playEffects = 1;

  game->settings->ai_player1 = 0;
  game->settings->ai_player2 = 1;
  game->settings->ai_player3 = 1;
  game->settings->ai_player4 = 1;
  
  /* not included in .gltronrc */

  game->settings->screenSaver = 0;

  game->settings->content[0] = 0;
  game->settings->content[1] = 1;
  game->settings->content[2] = 2;
  game->settings->content[3] = 3;

  /* go for .gltronrc (or whatever is defined in RC_NAME) */

  home = getenv("HOME"); /* find homedir */
  if(home == 0) {
    fname = malloc(strlen(CURRENT_DIR) + strlen(RC_NAME) + 2);
    sprintf(fname, "%s%c%s", CURRENT_DIR, SEPERATOR, RC_NAME);
  } else {
    fname = malloc(strlen(home) + strlen(RC_NAME) + 2);
    sprintf(fname, "%s%c%s", home, SEPERATOR, RC_NAME);
  }
  f = fopen(fname, "r");
  if(f == 0)
    printf("no %s found - using defaults\n", fname);
  else {
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
      }
    }
    free(fname);
    fclose(f);
  }

  game->settings->grid_size = default_arena_sizes[game->settings->arena_size];

  /* sanity check: speed, grid_size */
  if(game->settings->speed <= 0) {
    fprintf(stderr, "[gltron] sanity check failed: speed = %.2ff\n",
	    game->settings->speed);
    game->settings->speed = 6.0;
    fprintf(stderr, "[gltron] reset speed: speed = %.2f\n",
	    game->settings->speed);
  }
  if(game->settings->grid_size % 8) {
    fprintf(stderr, "[gltron] sanity check failed: grid_size %% 8 != 0: "
	    "grid_size = %d\n", game->settings->grid_size);
    game->settings->grid_size = 240;
    fprintf(stderr, "[gltron] reset grid_size: grid_size = %d\n",
	    game->settings->grid_size);
  }

  /* choose speed */
  default_speeds[4] = game->settings->speed;
  game->settings->current_speed = default_speeds[ game->settings->game_speed ];
  printf("[gltron] speed set to %.2f (level %d)\n",
	 game->settings->current_speed, game->settings->game_speed);

}

void saveSettings() {
  char *fname;
  char *home;
  int i;
  FILE* f;

  home = getenv("HOME"); /* find homedir */
  if(home == 0) {
    fname = malloc(strlen(CURRENT_DIR) + strlen(RC_NAME) + 2);
    sprintf(fname, "%s%c%s", CURRENT_DIR, SEPERATOR, RC_NAME);
  } else {
    fname = malloc(strlen(home) + strlen(RC_NAME) + 2);
    sprintf(fname, "%s%c%s", home, SEPERATOR, RC_NAME);
  }
  f = fopen(fname, "w");
  if(f == 0) {
    printf("can't open %s ", fname);
    perror("for writing");
    return; /* can't write rc */
  }
  for(i = 0; i < si_count; i++)
    fprintf(f, "iset %s %d\n", si[i].name, *(si[i].value));
  for(i = 0; i < sf_count; i++)
    fprintf(f, "fset %s %.2f\n", sf[i].name, *(sf[i].value));
  printf("written settings to %s\n", fname);
  free(fname);
  fclose(f);
}
