#include "gltron.h"

static void drawbg_splash( Splash *splash );
static void drawv_splash(  Splash *splash );

unsigned int splash_textures[1];

Splash*
new_splash( int width, int height,  char *background, int options )
{
  Splash      *splash;
  int         x, y, w;

  //Allocate the splash structure
  splash = ( Splash *) malloc(sizeof(Splash));
  if( splash == NULL )
    {
      fprintf(stderr, "could not allocate memory for splash\n");
      exit(1);
    }

  //init size
  splash->width   = width;
  splash->height  = height;

  //options
  splash->options = options;

  //load background texture 
  glGenTextures(1, splash_textures);
  glBindTexture(GL_TEXTURE_2D, splash_textures[0]);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  loadTexture(background, GL_RGBA);

  //Create progress things.
  //Finding position of progress bar
  w = (int)((splash->width-20)/100);
  w *=100;
  x = (game->screen->vp_w/2-w/2)+10;
  printf("width:%d, w:%d, x: %d\n", splash->width, w, x);
  if( splash->options & PROGRESS_TOP )
    {
      y = game->screen->vp_h/2+splash->height/2-10;
    } else {
      
      y = 10 + game->screen->vp_h/2-splash->height/2;
    }
  //Creating the progress bar
  splash->wprogress = new_wprogressbar(x, y, w, 3);

  //finding position of progress status
  if( splash->options & PROGRESS_TOP )
    {
      y = game->screen->vp_h/2+splash->height/2-30;
    } else {
      
      y = 30 + game->screen->vp_h/2-splash->height/2;
    }
  //Creating the progress status
  x = (game->screen->vp_w/2-splash->width/2)+10;
  splash->wstatus = new_wprogressstatus(x, y, (splash->width-10)*1.5/12);

  return splash;
}

void
draw_splash( Splash *splash )
{
  if( splash == NULL )
    return;
  
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  rasonly(game->screen);
  
  drawbg_splash( splash );
  draw_wprogressstatus(splash->wstatus);
  drawv_splash( splash );
  draw_wprogressbar(splash->wprogress);

  SystemSwapBuffers();  
}


static void drawbg_splash(Splash *splash ) {

  checkGLError("splash background start");

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, splash_textures[0]);

  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);

  glTexCoord2f(0.0, 0.0);
  glVertex2f(game->screen->vp_w/2-splash->width/2, game->screen->vp_h/2-splash->height/2);
  
  glTexCoord2f(1.0, 0.0);
  glVertex2f(game->screen->vp_w/2+splash->width/2, game->screen->vp_h/2-splash->height/2);

  glTexCoord2f(1.0, 1.0);
  glVertex2f(game->screen->vp_w/2+splash->width/2, game->screen->vp_h/2+splash->height/2);

  glTexCoord2f(0.0, 1.0);
  glVertex2f(game->screen->vp_w/2-splash->width/2, game->screen->vp_h/2+splash->height/2);
  
  glEnd();

  glDisable(GL_TEXTURE_2D);

}

void
update_splash(Splash *splash, float prog, char *status)
{  
  if( splash == NULL )
    return;
  if( prog < 0.0 && prog > 1.0 )
    return;
  update_wprogressbar(splash->wprogress, prog);
  update_wprogressstatus(splash->wstatus, status);
  draw_splash( splash );
}
static void
drawv_splash(  Splash *splash )
{
  char str[32];
  sprintf(str, "Version %s", VERSION);
  glColor3f(1.0, 0.0, 0.0);
  drawText(gameFtx, game->screen->vp_w/2,  90+game->screen->vp_h/2-splash->height/2, 8, str);
}

void
free_splash( Splash *splash )
{
  if( splash == NULL )
    return;

  //unload texture
  glDeleteTextures(1, splash_textures);

  free_wprogressbar(splash->wprogress);
  splash->wprogress=NULL;

  free_wprogressstatus(splash->wstatus);
  splash->wstatus = NULL;

  free(splash);
}
