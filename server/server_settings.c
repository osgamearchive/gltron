#include "server_gltron.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 100
#define MAX_VAR_NAME_LEN 64

void initDefaultSettings() {
  game->pauseflag = 0;

  game->settings->erase_crashed = 1;
  game->settings->fast_finish = 1;
  game->settings->current_speed = 7.0;
  game->settings->game_speed = 2; /* fast */
  /* game->settings->grid_size MUST be divisible by 8 */
  game->settings->arena_size = 3;
  game->settings->grid_size = 600;

  game->settings->ai_player1 = 0;
  game->settings->ai_player2 = 1;
  game->settings->ai_player3 = 1;
  game->settings->ai_player4 = 1;

  game->settings->ai_level = 1;
  
  game->settings->grid_size = default_arena_sizes[game->settings->arena_size];

  /* choose speed */
  default_speeds[4] = game->settings->current_speed;
  game->settings->current_speed = default_speeds[ game->settings->game_speed ];
}

void initMainGameSettings() {
  game2 = &main_game2;
  game = &main_game;
  game->settings = (Settings*) malloc(sizeof(Settings));

  game2->network.status = 0;

  /* initialize defaults, then load modifications from file */
  initDefaultSettings();

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




void
get_args( int argc, char *argv[] ) 
{
  int j=0;
  int i;
  while(j++<argc-1) {
    if(argv[j][0] == '-') {
      i = 0;
      while(argv[j][++i] != 0) {
	switch(argv[j][i]) {
	case 'f':
	  if( (j+1) > (argc-1) )
	    {
	      printf("you must specify config file with -f\n");
	      exit(1);
	    } else {
	      strcpy(configpath, argv[++j]);
	      //printf("config file %s\n", configpath);
	    }
	  break;
	case 'h':
	default:
	  if( argc <= 2 )
	    {
	      printf("Usage: %s [-h] [-f <conffile>]\n\n", argv[0]);
	      printf("-f\tset config file path ( default %s )\n", configpath);
	      printf("-h\tthis help\n");
	      exit(1);
	    }
	}
      }
    } else {
      printf("Usage: %s [-h] [-f <conffile>]\n\n", argv[0]);
      printf("-f\tset config file path ( default %s )\n", configpath);
      printf("-h\tthis help\n");
      exit(1);
    }
  }
}



#define BUFSIZE 100

void
ReadConfigFile()
{
  FILE *f=NULL;
  char buf[BUFSIZE];
  char com[BUFSIZE], value[BUFSIZE];

  f = fopen(configpath, "r");
  if( f == NULL )
    {
      fprintf(stderr, "config file %s doesn't exists\n", configpath);
      exit(1);
    }

  while( ! feof(f) )
    {
      fgets(buf, BUFSIZE, f);
      if( buf[0] != '#' && buf[0] != '\n' )
	{
	  sscanf(buf, "%[a-zA-Z] %[a-zA-Z 0-9.]", com, value);
	  //printf("com %s, buf %s\n", com, value);
	  
	  //now we can set variables
	  if( ! strcmp(com, "passwd") )
	    {
	      if( strlen(value) > 8 || strlen(value) < 6 )
		{
		  fprintf(stderr, "passwd lenght must be >=6 and <=8\n");
		  exit(1);
		} else {
		  strcpy(settings.passwd, value);
		}
	    } else if(  ! strcmp(com, "tracker") )
	      {
		if( strlen(value) > 32  )
		  {
		    fprintf(stderr, "tracker's address length must be <= 32\n");
		    exit(1);
		  } else {
		    strcpy(settings.tracker, value);
		  }
	      } else if(  ! strcmp(com, "description") )
		{
		  if( strlen(value) > 32  )
		    {
		      fprintf(stderr, "description lenght is too big");
		      exit(1);
		    } else {
		      strcpy(settings.description, value);
		    }
		} else if(  ! strcmp(com, "port") )
		  {
		    settings.port = strtol(value, (char**) NULL, 10);
		  } else if(  ! strcmp(com, "tport") )
		    {
		      settings.tport = strtol(value, (char**) NULL, 10);
		    }
	}
    }
  fclose(f);
}

void
initServerDefaultSettings()
{
  strcpy(settings.tracker, "localhost");
  strcpy(settings.passwd, "passwd");
  strcpy(settings.description, "server gltron");

  settings.port  = 23460;
  settings.tport = 23470;
 }
