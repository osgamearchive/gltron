void consoleInit();
void consoleAddLine(char *text);
void consoleDisplay(void(*func)(char *line, int call), int height);
void consoleScrollForward(int range);
void consoleScrollBackward(int range);
