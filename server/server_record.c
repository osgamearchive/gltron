#include "server_gltron.h"

int startPlaying(char *filename) {
  /*
  char *path;
  
  path = getFullPath(filename);
  if(path != NULL) {
    if((game2->play = fopen(path, "rb")) == NULL) {
      fprintf(stderr, 
	      "[playing] can't open file %s for reading\n", filename);
    }
    fprintf(stderr, "starting demo playback\n");
    free(path);
  } else {
    fprintf(stderr, "[playing] no demo file %s available\n", filename);
  }
  */

  if((game2->play = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "[playing] no demo file %s available\n", filename);
    return 1;
  }
  return 0;
}

void stopRecording() {
  printf("start...\n");
}

void writeEvent(GameEvent* e) {
  SendEvents(e);
}

int startRecording(char *filename) {
  /**
  fprintf(stderr, "starting demo recording\n");
  if((game2->record = fopen(filename, "wb")) == NULL) {
      fprintf(stderr, "[recording] can't open file %s for writing\n", 
	      filename);    
      return 1;
  }
  */
  return 0;
}

void stopPlaying() {
  fclose(game2->play);
}

GameEvent* readEvent() {
  GameEvent *e;
  e = (GameEvent*) malloc(sizeof(GameEvent));
  if(fscanf(game2->play, "%d %d %d %d %d ", &(e->type), &(e->player),
	    &(e->x), &(e->y), &(e->timestamp)) != 5) {
    free(e);
    return NULL;
  }
  return e;
}

void getEvents() {
  static GameEvent *latest = NULL;

  printf("getevents...\n");

  if(latest == NULL)
    latest = readEvent();
  if(latest != NULL) {
    while(latest->timestamp < game2->time.current) {
      /* TODO: check if it's safe to ignore the return value here */
      processEvent(latest);
      latest = readEvent();
      if(latest == NULL) return;
    }
  }
}
  
int writeDemoInfo() {
  int i;
  Data *data;

  if(game2->record == NULL) {
    fprintf(stderr, "[recording] illegal file handle\n");
    return 1;
  }
  fprintf(game2->record, "Rules: Speed: %f, EraseCrashed: %d\n",
	  game2->rules.speed, game2->rules.eraseCrashed);
  fprintf(game2->record, "Players: %d\n", game2->players);
  for(i = 0; i < game2->players; i++) {
    data = game->player[i].data;
    fprintf(game2->record, "x: %d y: %d direction: %d\n",
	    data->iposx,
	    data->iposy,
	    data->dir);

  }
  return 0;
}

int readDemoInfo() {
  int i;

  if(game2->play == NULL) {
    fprintf(stderr, "[playing] illegal file handle\n");
    return 1;
  }
  if(fscanf(game2->play, "Rules: Speed: %f, EraseCrashed: %d ",
	    &(game2->rules.speed), &(game2->rules.eraseCrashed)) != 2 ||
     fscanf(game2->play, "Players: %d ", &(game2->players)) != 1)
    return 1;
  game2->startPositions = (int*) malloc(3 * game2->players * sizeof(int));
  for(i = 0; i < game2->players; i++) {
    if(fscanf(game2->play, "x: %d y: %d direction: %d ",
	      (game2->startPositions + 3 * i + 0),
	      (game2->startPositions + 3 * i + 1),
	      (game2->startPositions + 3 * i + 2)) != 3)
      return 1;
  }
  return 0;
}
									   
    
