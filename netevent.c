#include "gltron.h"

int
getPlayer(int which)
{
  return slots[which].player;
}

cnetEventList
createNetEventCell( GameEvent *e )
{
  cnetEventList cell= ( cnetEventList ) malloc(sizeof(tcnetEventList));
  cell->next = NULL;
  cell->event = *e;
  return cell;
}

int
getWhich(int player)
{
  int i;
  for(i=0; i<MAX_PLAYERS;++i)
    {
      if( slots[i].player == player )
	{
	  printf("getWich %d -> %d\n", player, i);
	  return i;
	}
    }
  return player;
}
netEventList
createNetEventList( void )
{
  netEventList nl = ( netEventList ) malloc(sizeof(tnetEventList));
  nl->head = NULL;
  return nl;
}

void
addNetEvent( GameEvent *e)
{
  cnetEventList cell;

  cell = neteventlist->head;
  if( cell == NULL )
    {
      neteventlist->head = createNetEventCell(e);
      return;
    }
  while( cell->next )
    { cell = cell->next; }
  cell->next = createNetEventCell(e);  
}

GameEvent *
getNetEvent()
{
  cnetEventList cell;
  GameEvent *e;

  if( neteventlist->head == NULL )
    return NULL;

  e=( GameEvent *) malloc(sizeof(GameEvent));

  cell = neteventlist->head;

  neteventlist->head = cell->next;
  *e= cell->event;
  free(cell);
  return e;
}
