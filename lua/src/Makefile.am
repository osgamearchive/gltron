SUBDIRS=lib

EXTRA_LTLIBRARIES=liblua.la

DEFS=
CPPFLAGS=
CFLAGS=-O2 -ansi -pedantic -Wall -I. -I../include
LDFALGS=
LIBS=-lm

liblua_la_SOURCES=lapi.c lcode.c ldebug.c ldo.c lfunc.c lgc.c llex.c lmem.c lobject.c lparser.c lstate.c lstring.c ltable.c ltests.c ltm.c lundump.c lvm.c lzio.c

all: liblua.la

clean:
	rm -f liblua.la
