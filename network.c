#include "gltron.h"

#define HEADER_SIZE 6

void clientConnectToServer() {
  
}

void sendNetEvent(GameEvent *e) {
  unsigned char buf[HEADER_SIZE];
  buf[0] = NET_EVENT;
  buf[1] = '\0';
  SystemNet_Write32(sizeof(GameEvent), buf + 2);
  /* TODO: decide if to send to server or multiple clients */
  SystemTCPsend(buf, sizeof(buf));
  SystemTCPsend((char*) e, sizeof(GameEvent));
}

void updateNet() {
  unsigned char buf[HEADER_SIZE];
  NetData *data;
  list *p;
  list *l;

  while(SystemCheckSockets()) {
    data = (NetData*) malloc(sizeof(NetData));
    for(p = game2->network->data; p->next != NULL; p = p->next);
    l = (list*) malloc(sizeof(list));
    p->next = l;
    l->next = NULL;
    l->data = data;

    SystemTCPrecv(buf, sizeof(buf));
    data->type[0] = buf[0];
    data->type[1] = buf[1];
    data->length = SystemNet_Read32(buf + 2);
    if(data->length != 0) {
      data->data = (char*) malloc(data->length);
      SystemTCPrecv(data->data, data->length);
    }
  }
  return;
}

GameEvent* getNetEvent() {
  static GameEvent *current = NULL;
  GameEvent *result;
  list *p;
  list *l;
  NetData *data;

  if(game2->network == NULL) {
    fprintf(stderr, "networking not even initialized\n");
    return NULL;
  }

  if(current == NULL) {
    for(p = game2->network->data; p->next != NULL; p  = p->next) {
      data = (NetData*) p->data;
      if(data->type[0] == NET_EVENT) {
	current = (GameEvent*) data->data;
	/* remove event from list now */
	p->data = p->next->data;
	l = p->next;
	p->next = p->next->next;
	free(l);
      }
    }
  }

  if(current != NULL) { /* werde wohl kaum events aus der Zukunft bekommen */
    if(current->timestamp < game2->time.current) {
      result = current;
      current = NULL;
      return result;
    }      
  }
  return NULL;
}
