#include "gltron.h"
#define MAX_CHARS 1024
static char buff[MAX_CHARS]; //TODO Defines MAX_CHARS
static int  nbreads;

void
initkeyreading()
{
  nbreads=0;
  printf("Entering keyboardreadingCallbacks...\n");
}

void
keyboardreadingreturn()
{
  char command[MAX_CHARS], params[MAX_CHARS];

  //We have our buffer.
  sscanf(buff, "/%s %s", command, params);
  if( strlen(command) > 0 )
    {
      fprintf(stderr, "command: %s\nparams: %s\n", command, params);
    } else {
      fprintf(stderr, "send chat: %s\n", buff);
    }
}

void
cleankeyreading()
{
  printf("Leaving keyboardreadingCallbacks...\n");
  //call a function at return...
  keyboardreadingreturn();
}

void
keyboardreadingpressed(int k)
{
  fprintf(stderr, "%c", k);
}

void keyreading(int k, int x, int y)
{
  if( nbreads > MAX_CHARS )
    {
      //leave, no space left...
      restoreCallbacks();
    } else {
      switch( k )
	{
	case 13://return
	  //finished, we leave.
	  buff[nbreads++]='\0';
	  keyboardreadingreturn();
	  restoreCallbacks();
	  break;
	case 27://escape
	  //???
	  break;
	case 127://backspace
	  nbreads--;
	  break;
	default:
	  buff[nbreads++]=k;
	  //call a function after key is pressed ( example: trigger. )
	  keyboardreadingpressed(k);
	  break;
	}
    }
}

void displaykeyboardreading() {
  SystemSwapBuffers();
}

void idlekeyboardreading() { }

callbacks keyboardreadingCallbacks = {
  displaykeyboardreading, idlekeyboardreading, keyreading, initkeyreading,
  cleankeyreading, NULL, NULL, NULL
};
