# Makefile for gltron

SHELL = /bin/sh
CC = gcc.colorgcc
OPT = -O2

BASE_CFLAGS = -c -pedantic -Wall

ifdef USE_SOUND
ADD1 = -DSOUND
endif

ifdef FREEGLUT
ADD2 = -DFREEGLUT -I../FreeGlut/src
endif

CFLAGS = $(BASE_CFLAGS) $(ADD1) $(ADD2)

ifdef FREEGLUT
GL_LIBS = -L../FreeGlut/src -lGL -lGLU -lglut
else 
# GL_LIBS = -lGL -lGLU -lfreeglut
GL_LIBS = -lGL -lGLU -lSDL
# GL_LIBS = -lGL -lGLU -lglut
endif

ifdef FREEGLUT
XLIBS = -L/usr/X11/lib -L/usr/X11R6/lib -lX11
else
XLIBS = -L/usr/X11/lib -L/usr/X11R6/lib -lX11 -lXext -lXmu -lXt -lXi -lSM -lICE -lm -lpthread
endif

# SNDLIBS = `libmikmod-config --cflags` `libmikmod-config --libs`
SNDLIBS =  -lSDL_mixer

GLTRON_INSTALLDIR = /usr/bin
GLTRON_HOME = /usr/share/games/gltron

CFILES = \
	sgi_texture.c \
	switchCallbacks.c \
	gui.c \
	pause.c \
	computer.c \
	engine.c \
	gltron.c \
	graphics.c \
	gamegraphics.c \
	input.c \
	settings.c \
	texture.c \
	load_texture.c \
	fonttex.c \
	fonts.c \
	menu.c \
	file.c \
	model.c \
	modelgraphics.c \
	mtllib.c \
	geom.c \
	system.c \

# chooseModel.c \
# 	character.c \
# 	texfont.c \

SOUND_CFILES = \
	sound.c

OBJ = $(CFILES:.c=.o)
OBJ_SOUND = $(OBJ) $(SOUND_CFILES:.c=.o)

all: gltron

.c.o:
	$(CC) $(CFLAGS) $(OPT) $<

gltron: $(OBJ)
	$(CC) $(OPT) -o gltron $(OBJ) $(GL_LIBS) $(XLIBS)

gltron_sound: $(OBJ_SOUND)
	$(CC) $(OPT) -o gltron $(OBJ_SOUND) $(GL_LIBS) $(XLIBS) $(SNDLIBS)

sound:
	$(MAKE) gltron_sound USE_SOUND=1 

sound_freeglut:
	$(MAKE) gltron_sound USE_SOUND=1 FREEGLUT=1

freeglut:
	$(MAKE) gltron FREEGLUT=1

debug:
	$(MAKE) gltron OPT=-g

debug_sound:
	$(MAKE) gltron_sound OPT=-g USE_SOUND=1

DATA_FILES = menu.txt settings.txt \
	gltron.sgi gltron_floor.sgi gltron_wall.sgi gltron_crash.sgi \
	xenotron.ftx xenotron.0.sgi xenotron.1.sgi \
	t-u-low.obj tron.mtl

INSTALL_FILES = gltron $(DATA_FILES)

install: $(INSTALL_FILES)
	if [ ! -d $(GLTRON_INSTALLDIR) ] ; then \
	    mkdir -p $(GLTRON_INSTALLDIR) ; \
	fi
	if [ ! -d $(GLTRON_HOME) ] ; then \
	    mkdir -p $(GLTRON_HOME) ; \
	fi
	cp -f gltron $(GLTRON_INSTALLDIR)/gltron
	if [ -e gltron.it ] ; then \
	    cp -f gltron.it $(GLTRON_HOME)/gltron.it ; \
	fi
	cp -f $(DATA_FILES) $(GLTRON_HOME)

packages:
	cd .. ; \
	tar zcvf gltron.tar.gz gltron ; \
	mv -f gltron.tar.gz /usr/src/redhat/SOURCES
	rpm -ba gltron.rpm.spec
	cd /usr/src/redhat/RPMS/i386 ; \
	# alien --to-tgz -k gltron*.rpm ; \
	# alien --to-deb -k gltron*.rpm
	rpm -ba gltron_sound.rpm.spec
	cd /usr/src/redhat/RPMS/i386 ; \
	# alien --to-tgz -k gltron_*.rpm ; \
	# alien --to-deb -k gltron_*.rpm

clean: 
	rm -f *\.o gltron core

VERSION = 0.53
TAG = Release_0_53a

dist:
	cvs export -d gltron-$(VERSION) -r $(TAG) gltron ; \
	tar cfz gltron-$(VERSION).sound.tar.gz gltron-$(VERSION) ; \
	rm -f gltron-$(VERSION)/gltron.it ; \
	tar cfz gltron-$(VERSION).tar.gz gltron-$(VERSION); \
	rm -rf gltron-$(VERSION);

