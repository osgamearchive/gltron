#include "gltron.h"


int
getPlayer(int which)
{
  return slots[which].player;
}

int
find_freeplayer()
{
  int i,j;
  
  for(i=0; i<MAX_PLAYERS; ++i)
    {
      //look if player i is free
      for(j=0; j<MAX_PLAYERS; ++j)
	{
	  if( slots[j].player == i )
	    break;
	}
      if( j == MAX_PLAYERS )
	{
	  //player i is free we use it!
	  break;
	}
    }
  return i;
}

cnetEventList
createNetEventCell( GameEvent *e )
{
  cnetEventList cell= ( cnetEventList ) malloc(sizeof(tcnetEventList));
  cell->next = NULL;
  cell->event = *e;
  return cell;
}

void
initNetEventList(netEventList nsl )
{
  cnetEventList cell, next;

  //nsl must be allocate before...
  if( nsl == NULL )
    return;

  cell = nsl->head;

  if( cell == NULL )
    {
      return;
    }
  while( cell )
    { next = cell->next; free(cell); cell=next;printf("freeing cell...\n"); }
  nsl->head=NULL;
}


int
getWhich(int player)
{
  int i;
  for(i=0; i<MAX_PLAYERS;++i)
    {
      if( slots[i].player == player )
	{
	  //printf("getWich %d -> %d\n", player, i);
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
  printf("current pos is %d %d %d time is %d\n", game->player[e->player].data->iposx,
	 game->player[e->player].data->iposy,
	 game->player[e->player].data->dir,
	 game2->time.current);
  free(cell);
  return e;
}
