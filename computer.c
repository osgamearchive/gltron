#include "gltron.h"

int freeway(Data *data, int dir) {
  int i;
  int wd = 20;

  for(i = 1; i < wd; i++)
    if(getCol((int)data->posx + dirsX[dir] * i, (int)data->posy + dirsY[dir] * i,
       colwidth, colmap)) break;
  return i;
}

void getDistPoint(Data *data, int d, int *x, int *y) {
  *x = (int)data->posx + dirsX[data->dir] * d;
  *y = (int)data->posy + dirsY[data->dir] * d;
}
  
void doComputer(Player *me, Data *him) {
  AI *ai;
  Data *data;
  int x, y;
  int i;
  int dtest[] = { 5, 10, 15 };
  int dn = 3;

  int fd = 15;

  int maxmoves = 100;
  int dir1, dir2;
  int s1, s2;
  int d1, d2;

  int tvalue = 0;

  if(me->ai == NULL) {
    printf("This player has no AI data!\n");
    return;
  }
  
  data = me->data;
  ai = me->ai;
  ai->moves++;

  if(ai->danger <= 0) {
    for(i = 0; i < dn; i++) {
      getDistPoint(me->data, dtest[i], &x, &y);
      if(getCol(x, y, colwidth, colmap)) ai->danger = dtest[i];
    }
  }

  if(ai->danger > 0) {
    dir1 = (data->dir + 1) % 4;
    dir2 = (data->dir + 3) % 4;
    s1 = freeway(data, dir1);
    s2 = freeway(data, dir2);

    if(s1 > ai->danger && s2 > ai->danger) { /* turn ok */
      if(s1 > fd && s1 - ai->tdiff > s2)
	tvalue = 1;
      else if(s2 > fd && s1 - ai->tdiff < s2)
	tvalue = 3;
      else tvalue = (s1 > s2) ? 1 : 3;
      turn(data, tvalue);
      ai->tdiff += (tvalue == 1) ? 1 : -1;
      ai->danger = 0;
    } else {
      ai->danger--;
    }
  } else if(ai->moves >= maxmoves) {
    dir1 = (data->dir + 1) % 4;
    dir2 = (data->dir + 3) % 4;
    d1 = abs((int)data->posx + dirsX[dir1] - (int)him->posx) +
      abs((int)data->posy + dirsY[dir1] - (int)him->posy);
    d2 = abs((int)data->posx + dirsX[dir2] - (int)him->posx) +
      abs((int)data->posy + dirsY[dir2] - (int)him->posy);
    tvalue = (d1 < d2) ? 1 : 3;
    if(freeway(data, (data->dir + tvalue) % 4) > fd) {
      turn(data, tvalue);
      ai->tdiff += (tvalue == 1) ? 1 : -1;
      ai->moves = 0;
    } else {
      ai->moves -= 20;
    }
  }
}
