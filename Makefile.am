SUBDIRS = lua nebu src

bin_PROGRAMS = gltron

gltron_SOURCES = src/gltron.c

gltron_LDADD = \
	src/game/libgame.a \
	src/input/libinput.a \
	src/audio/libaudio.a \
	src/video/libvideo.a \
	src/configuration/libconfiguration.a \
	src/base/libbase.a \
	src/filesystem/libfilesystem.a \
	nebu/input/libinput.a \
	nebu/audio/libaudio.a \
	nebu/video/libvideo.a \
	nebu/scripting/libscripting.a \
	nebu/filesystem/libfilesystem.a \
	nebu/base/libbase.a \
	lua/src/liblua.a \
	lua/src/lib/liblualib.a

# gltron_LDFLAGS = -static

AM_CFLAGS = \
	-Isrc/include \
	-Ilua/include \
	-Inebu/include

AM_CXXFLAGS = \
	-Isrc/include \
	-Ilua/include \
	-Inebu/include

tags:
	etags `find -name '*.c' -or -name '*.cpp' -or -name '*.h' -or -name '*.lua'`
