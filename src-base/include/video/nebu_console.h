#ifndef NEBU_CONSOLE_H
#define NEBU_CONSOLE_H

typedef enum {
  TO_CONSOLE = 0x01,
  TO_STDOUT  = 0x02,
  TO_STDERR  = 0x04
} outloc_e;

void consoleInit();
void consoleAddLine(char *text);
void consoleDisplay(void(*func)(char *line, int call), int height);
void consoleScrollForward(int range);
void consoleScrollBackward(int range);
void displayMessage(outloc_e where, const char *fmt_str, ...);

#endif /* CONSOLE_H */
