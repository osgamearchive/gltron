CFILES = server_computer.c server_engine.c server_event.c server_globals.c server_gltron.c server_record.c server_system.c server_settings.c server.c ../protocol.c server_tracker.c ../tracker_protocol.c
CFLAGS = -g3 -Wall $(shell sdl-config --cflags) -I..
LDFLAGS = $(shell sdl-config --libs) -lSDL_net
CC=gcc
server: $(CFILES)
	$(CC) -o server $(CFLAGS) $(CFILES) $(LDFLAGS)
#-pedantic 
