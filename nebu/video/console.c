#include "video/nebu_console.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define CONSOLE_DEPTH 100
#define CONSOLE_WIDTH 80

static char buffer[CONSOLE_DEPTH][CONSOLE_WIDTH];
static int position;
static int offset;

void consoleInit() {
  int i;

  for(i = 0; i < CONSOLE_DEPTH; i++)
    buffer[i][0] = '\0';

  position = 0;
  offset = 0;
  // fprintf(stderr, "console initialized\n");
}

void consoleAddLine(char *text) {
  int i = 0, x=0;

  while(i < CONSOLE_WIDTH - 1 && text[i] != 0) {
    buffer[position][i] = text[i];
    i++;
  }
  buffer[position][i] = '\0';
/*  fprintf(stderr, "added \"%s\" to console at buffer[%i] with "
 *	  " offset of %i\n",
 *	  buffer[position], position, offset);
 */

   position++;
   
   /* reposition the buffer to avoid overruns - tim */
   if(position >= 99){
      for(i=0;i<CONSOLE_DEPTH;i++){
	 strcpy(buffer[x], buffer[i]);
	 buffer[i][0] = '\0';
	 ++x;
      }
      position -= 4;
   }
}

void consoleDisplay(void(*func)(char *line, int call), int height) {
  int i;
  int j = 0;
  for(i = 0; i < height; i++) {
    if(*(buffer[ (position + i - height - offset +
		CONSOLE_DEPTH) % CONSOLE_DEPTH ]) != 0)
      func(buffer[ (position + i - height - offset +
		    CONSOLE_DEPTH) % CONSOLE_DEPTH ], j++);
  }
}
  
void consoleScrollForward(int range) {
  offset -= range;
  if(offset < 0) offset = 0;
}

void consoleScrollBackward(int range) {
  offset += range;
  if(offset > CONSOLE_DEPTH - 1)
    offset = CONSOLE_DEPTH - 1;
}

/*
  displayMessage

  post a message to the console and/or stdout/stderr.

  NOTE: Don't put newlines at the end of the format string,
        the function will handle adding them when appropriate.
 */
void displayMessage(outloc_e where, const char *fmt_str, ...) {

  char message[CONSOLE_WIDTH];
  va_list ap;
  
  va_start(ap, fmt_str);
  
  if (where & TO_CONSOLE) {
    vsprintf(message, fmt_str, ap);
    consoleAddLine(message); 
  }

  if (where & TO_STDOUT) {
    vfprintf(stdout, fmt_str, ap);
    fputc('\n', stdout);
  }

  if (where & TO_STDERR) {
    vfprintf(stderr, fmt_str, ap);
    fputc('\n', stderr);
  }
}

