SUBDIRS=lib

noinst_LIBRARIES=liblua.a

DEFS=
AM_CPPFLAGS=
AM_CFLAGS=-ansi -pedantic -Wall -I$(srcdir) -I$(top_srcdir)/lua/include
LDFLAGS=
LIBS=-lm

liblua_a_SOURCES=lapi.c lcode.c ldebug.c ldo.c lfunc.c lgc.c llex.c lmem.c \
	lobject.c lparser.c lstate.c lstring.c ltable.c ltests.c ltm.c lundump.c \
	lvm.c lzio.c
noinst_HEADERS=lapi.h lcode.h ldebug.h ldo.h lfunc.h lgc.h llex.h llimits.h lmem.h \
	lobject.h lopcodes.h lparser.h lstate.h lstring.h ltable.h ltm.h \
	lundump.h lvm.h lzio.h
