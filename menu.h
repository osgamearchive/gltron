#ifndef MENUS
#define MENUS

enum {
  MENU_ACTION = 1,
  MENU_LEFT = 2,
  MENU_RIGHT = 4
};

typedef struct {
  /* fonttex *font; */
  float fgColor[4]; /* entries */
  float hlColor[4]; /* the highlighted one */

  char szCaption[64];
} mDisplay;

typedef struct Menu {
  int nEntries;
  int iHighlight;
  mDisplay display;
  char szName[64];
  char szCapFormat[64];
  struct Menu** pEntries;
  struct Menu* parent;
  void* param; /* reserved to bind parameters at runtime */
} Menu;

typedef struct {
  void* data;
  void* next;
} node;

#endif





