#ifndef __WPROGRESS_H__
#define __WPROGRESS_H__


typedef struct
{  
  int       x;         //position of the progress bar
  int       y;
  int       width;     //size of the progress bar
  int       height;
  float     last;      //last prog
  float     prog;      //actual progression in percent
} Wprogressbar;

typedef struct
{
  int       x;         //position of the status progress
  int       y;
  int       nbchars;   //mac chars
  char     *status;    //actual status.  
} Wprogressstatus;


/** progress bar */
Wprogressbar     *new_wprogressbar(int x, int y, int width, int height);
void              draw_wprogressbar(Wprogressbar *wprogress);
void              update_wprogressbar(Wprogressbar *wprogress, float prog);

/** progress status */
Wprogressstatus  *new_wprogressstatus(int x, int y, int nbchars);
void              draw_wprogressstatus(Wprogressstatus  *wstatus);
void              update_wprogressstatus(Wprogressstatus *wstatus, char *status);
#endif
