EXTRA_LTLIBRARIES=liblualib.la

DEFS=
CPPFLAGS=
CFLAGS=-O2 -ansi -pedantic -Wall -I../ -I../../include
LDFALGS=
LIBS=-lm

liblualib_la_SOURCES=lauxlib.c lbaselib.c ldblib.c liolib.c lmathlib.c lstrlib.c

all: liblualib.la

clean:
	rm -f liblualib.la
