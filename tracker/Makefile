CFILES = tracker_gltron.c tracker.c tracker_settings.c tracker_globals.c ../tracker_protocol.c ../protocol.c ../server/server_system.c
CFLAGS = -g3 -Wall $(shell sdl-config --cflags) -I.. -I../server -DTRACKER
LDFLAGS = $(shell sdl-config --libs) -lSDL_net
CC=gcc
server: $(CFILES)
	$(CC) -o tracker $(CFLAGS) $(CFILES) $(LDFLAGS)
#-pedantic 
