extern Game2 game2;

void processEvent(GameEvent e*) {
  Data *data;
  switch(e.type) {
  case EVENT_TURN:
    data = game->players[e->player].data;
    data->iposx = e->x; /* warp player (back) to turn location */
    data->iposy = e->y;
    doTurn(data, e->timestamp);
    break;
  case EVENT_CRASH:
    data = game.players[e->player].data;
    data->iposx = e->x;
    data->iposy = e->y;
    crashPlayer(e->player);
    break;
  }
}
     
void idleDemo(void) {
  
#ifdef SOUND
  soundIdle();
#endif

  /* take care of timers */
  game2->time->lastFrame = game2->time->current;
  game2->time->current = SystemGetElapsedTime() - offset;
  game2->time->dt = game2->time->current - game2->time->lastFrame;

  /* process events */

  while(game2->current != NULL) {
    if(game2->current->timestamp < game2->time->current) {
      processEvent(&(game2->history->event));
    } else { break; }
    game2->current = game2->current->next;
  }
}





