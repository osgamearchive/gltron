#ifndef __SPLASH_H__
#define __SPLASH_H__

//Options for a Splash screen.
enum {
  PROGRESS_BOTTOM = 0,   //By default, progress information are on bottom
  PROGRESS_TOP    = 2    // progress information are on top of windoww
};

typedef struct
{
  int              width;      //size of the splash screen
  int              height;
  int              options;    //options
  SDL_Surface      *screen;    //Surface for the splash screen
  png_texture      *image;     //background of the splash screen
  Wprogressbar     *wprogress; //progress bar
  Wprogressstatus  *wstatus;   //status text
} Splash;


Splash   *new_splash(int width, int height, char *background,  int options);
void      draw_splash( Splash *splash );
void      update_splash(Splash *splash, float prog, char *status);
#endif
