# GNU automake Makefile.am for gltron

EXTRA_DIST = CREDITS README.html LICENSE CHANGELOG

# SUBDIRS = art data 
SUBDIRS=lua

bin_PROGRAMS = gltron

gltron_SOURCES = artpack.c png_texture.c pixels.c switchCallbacks.c gui.c \
	pause.c computer.c engine.c event.c game.c gltron.c gamegraphics.c \
	light.c software.c trail.c trails_buffered.c quad.c quadbuf.c \
	input.c settings.c texture.c load_texture.c fonttex.c \
	fonts.c menu.c findpath.c file.c directory.c \
	screenshot.c geom.c model.cpp material.cpp \
	system.c globals.c record.c camera.c recognizer.c \
	console.c renderer_gl.c callbacks.h camera.h data.h \
	geom.h file.h gltron.h light.h menu.h model.h png_texture.h \
	quad.h renderer_gl.h switchCallbacks.h system.h texture.h credits.c \
        SkyBox.cpp SkyBox.h skybox_glue.cpp skybox_glue.h engine.h event.h

gltron_LDADD = @NETWORK_OBJS@ @SOUND_OBJS@ lua/src/liblua.la lua/src/lib/liblualib.la

gltron_DEPENDENCIES = @NETWORK_OBJS@ @SOUND_OBJS@ lua/src/liblua.la lua/src/lib/liblualib.la

EXTRA_gltron_SOURCES = sound.c sound_glue.cpp sound.h sound_glue.h \
	Source.cpp Source3D.cpp SourceMusic.cpp SourceSample.cpp SoundSystem.cpp \
	SourceCopy.cpp 

LIBTOOL_DEPS = @LIBTOOL_DEPS@

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

todo:
	perl todo2html.pl < TODO > TODO.html
