/* networking for gltron - the server code */

/* 
   tasks:
   movePlayers();
   doComputer();
*/

/* 
   currently, we have only two players and
   no computer
*/

/*
  data structures accessed:
  
  player->data
  dt (time since last frame)
  collision detection data
*/

typedef struct {
  int grid_size;
  int erase_crashed;
  float speed;
} ServerSettings;

static int dt;
static long lasttime;

static unsigned char* colmap = 0;
static int colwidth;

initServerData() {
  /*
    called upon starting the server
    allocate data structures

    settings: only erase_crashed and speed are important
    
  */

  colwidth = (grid_size + 7) / 8;
  if(colmap != 0) free(colmap);
  colmap = (unsigned char*) malloc(colwidth * grid_size);

}
  
resetServerData(ServerSettings *settings) {
  /*
    start with a clear game
  */
  
  lasttime = SystemGetElapsedTime();  
}
  
