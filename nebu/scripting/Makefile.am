noinst_LIBRARIES=libscripting.a

libscripting_a_SOURCES= \
	scripting.c

libscripting_a_LIBADD= \
	$(top_builddir)/lua/src/liblua.a \
	$(top_builddir)/lua/src/lib/liblualib.a

AM_CFLAGS = \
	-I$(top_srcdir)/nebu/include/base \
	-I$(top_srcdir)/nebu/include/scripting \
	-I$(top_srcdir)/lua/include
