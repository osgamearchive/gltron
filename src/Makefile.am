SUBDIRS = filesystem base configuration audio video input game

# bin_PROGRAMS = gltron lua_test
bin_PROGRAMS = gltron

lua_test_SOURCES = test.c
lua_test_LDADD = \
	$(top_srcdir)/lua/src/liblua.a \
	$(top_srcdir)/lua/src/lib/liblualib.a

gltron_SOURCES = gltron.c

gltron_LDADD = \
	game/libgame.a \
	input/libinput.a \
	audio/libaudio.a \
	video/libvideo.a \
	configuration/libconfiguration.a \
	base/libbase.a \
	filesystem/libfilesystem.a \
	$(top_srcdir)/nebu/input/libinput.a \
	$(top_srcdir)/nebu/audio/libaudio.a \
	$(top_srcdir)/nebu/video/libvideo.a \
	$(top_srcdir)/nebu/scripting/libscripting.a \
	$(top_srcdir)/nebu/filesystem/libfilesystem.a \
	$(top_srcdir)/nebu/base/libbase.a \
	$(top_srcdir)/lua/src/liblua.a \
	$(top_srcdir)/lua/src/lib/liblualib.a


# gltron_LDFLAGS = -static

AM_CFLAGS = -Iinclude -I$(top_srcdir)/lua/include
AM_CXXFLAGS = -Iinclude -I$(top_srcdir)/lua/include

lib_LIBRARIES = libGLtron.a

libGLtron_a_SOURCES =

libGLtron_a_LIBADD = \
	base/libbase.a \
	filesystem/libfilesystem.a \
	audio/libaudio.a \
	video/libvideo.a \
	input/libinput.a \
	game/libgame.a

## libNebu_a_LIBADD = \
## 	base/libbase.a \
## 	filesystem/libfilesystem.a \
## 	scripting/libscripting.a \
## 	audio/libaudio.a \
## 	video/libvideo.a \
## 	input/libinput.a
