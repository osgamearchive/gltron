#include "gltron.h"
#include "scripting.h"
#include <string.h>

void drawMenu(gDisplay *d) {
  /* draw Menu pCurrent */

  int i;
  int x, y, size, lineheight;
  int hsize, vsize;
  int maxw = 0;
  int nEntries;
  char pMenuName[200];
  int iActiveItem;

  rasonly(d);

#define MENU_TEXT_START_X 0.08
#define MENU_TEXT_START_Y 0.40

#define MENU_WIDTH 0.80
#define MENU_HEIGHT 0.40

#define MENU_TEXT_LINEHEIGHT 1.5

  x = (int) (d->vp_w * MENU_TEXT_START_X);
  y = (int) (d->vp_h * MENU_TEXT_START_Y);

  /* obtain menu name */
  scripting_Run("return Menu.current");
  scripting_CopyStringResult(pMenuName, 200);
  /* obtain some information about the active menu */
  scripting_RunFormat("return getn( Menu.%s.items )", pMenuName);
  scripting_GetIntegerResult(&nEntries);

  /* new stuff: calculate menu dimensions */
  for(i = 0; i < nEntries; i++) {
    int len = 0;
    int result;

    scripting_RunFormat("return strlen( Menu[Menu.%s.items[%d]].caption )", 
			pMenuName, i + 1);
    scripting_GetIntegerResult(&result);
    len += result;

    scripting_RunFormat("return GetMenuValueWidth( Menu.%s.items[%d] )",
			pMenuName, i + 1);
    scripting_GetIntegerResult(&result);
    len += result;

    if(len > maxw)
      maxw = len;
  }

  /* adjust size so menu fits into MENU_WIDTH/HEIGHT */

  hsize = (int) ((float)d->vp_w * MENU_WIDTH / (float)maxw );
  vsize = (int) ((float)d->vp_h * MENU_HEIGHT / 
		 ( (float)nEntries * MENU_TEXT_LINEHEIGHT));

  size = (hsize < vsize) ? hsize : vsize;

  lineheight = (int)( (float) size * MENU_TEXT_LINEHEIGHT);  

  /* printf("%d %d %d %d %d\n", x, y, size, maxw, pCurrent->nEntries); */
  /* draw the entries */

  scripting_Run("return Menu.active");
  scripting_GetIntegerResult(&iActiveItem);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  for(i = 0; i < nEntries; i++) {
    if(i == iActiveItem - 1) {
      float color[4];
      float active1[4];
      float active2[4];
      int j;
      float t;
      int time = SystemGetElapsedTime() & 4095; 
      t = sin( time * M_PI / 2048.0 ) / 2.0 + 0.5;

      scripting_GetFloatArray("menu_item_active1", active1, 4);
      scripting_GetFloatArray("menu_item_active2", active2, 4);

      for(j = 0; j < 4; j++) {
	color[j] = t * active1[j] + (1 - t) * active2[j];
      }
      glColor4fv(color);
      /* fprintf(stderr, "%.2f: %.2f %.2f %.2f\n", 
	 t, color[0], color[1], color[2]); */
    } else {
      float color[4];
      scripting_GetFloatArray("menu_item", color, 4);
      glColor4fv(color);
    }

      {
	char line[200];
	scripting_RunFormat("return "
			    "Menu[Menu.%s.items[%d]].caption .. ': ' .. "
			    "GetMenuValueString( Menu.%s.items[%d] )",
			    pMenuName, i + 1,
			    pMenuName, i + 1);
	scripting_CopyStringResult(line, sizeof(line));
	drawText(guiFtx, x, y, size, line);
      }

    /*
    if(i == pCurrent->iHighlight) 
      drawSoftwareHighlight(x, y, size, ((Menu*)*(pCurrent->pEntries + i))->display.szCaption);
    */
    y -= lineheight;
  }
  
  glDisable(GL_BLEND);
}





