#define CONSOLE_DEPTH 100
#define CONSOLE_WIDTH 80

#include <stdio.h>

static char buffer[CONSOLE_DEPTH][CONSOLE_WIDTH];
static int position;
static int offset;

void consoleInit() {
  int i;

  for(i = 0; i < CONSOLE_DEPTH; i++)
    buffer[i][0] = '\0';

  position = 0;
  offset = 0;
  fprintf(stderr, "console initialized\n");
}

void consoleAddLine(char *text) {
  int i = 0;
  while(i < CONSOLE_WIDTH - 1 && text[i] != 0) {
    buffer[position][i] = text[i];
    i++;
  }
  buffer[position][i] = '\0';
  fprintf(stderr, "added %s to console\n", buffer[position]);
  position++;
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

  

