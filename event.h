
enum {
  EVENT_TURN_LEFT = 1,
  EVENT_TURN_RIGHT = 2,
  EVENT_CRASH = 4,
  EVENT_STOP = 8
};

extern void createEvent(int player, int direction);
extern int processEvent(GameEvent *e);
