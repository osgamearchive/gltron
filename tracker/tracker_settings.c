#include "tracker_gltron.h"

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
	  if( j+1 > argc-1 )
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
	  sscanf(buf, "%[a-zA-Z] %[a-zA-Z 0-9]", com, value);
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
		    fprintf(stderr, "name length must be <= 32\n");
		    exit(1);
		  } else {
		    strcpy(settings.tracker, value);
		  }
	    } else if(  ! strcmp(com, "port") )
	      {
		settings.port = strtol(value, (char**) NULL, 10);
	      } else if(  ! strcmp(com, "timeout") )
	      {
		settings.timeout = strtol(value, (char**) NULL, 10);
	      }
	}
    }
  fclose(f);
}

void
initDefaultSettings()
{
  strcpy(settings.tracker, "tracker");
  strcpy(settings.passwd, "passwd");

  settings.port=23470;
  settings.timeout=60;

 }
