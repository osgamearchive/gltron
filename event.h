
#ifndef EVENT_H
#define EVENT_H

enum {
  EVENT_TURN_LEFT = 1,
  EVENT_TURN_RIGHT = 2,
  EVENT_CRASH = 4,
  EVENT_STOP = 8
};

typedef struct GameEvent {
  int type; /* what */
  int player; /* who */
  int x; /* where */
  int y;
  int timestamp;
} GameEvent;

extern void createEvent(int player, int direction);
extern int processEvent(GameEvent *e);

#endif /* EVENT_H */
