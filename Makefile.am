# GNU automake Makefile.am for gltron

EXTRA_DIST = CREDITS README.html README.lua COPYING ChangeLog INSTALL FAQ TODO gltron.6

SUBDIRS = lua data art sounds scripts music

bin_PROGRAMS = gltron

CFLAGS=@CFLAGS@ -I$(srcdir)/lua/src -I$(srcdir)/lua/include
CPPFLAGS=@CPPFLAGS@ -I$(srcdir)/lua/src -I$(srcdir)/lua/include

gltron_SOURCES = artpack.c png_texture.c pixels.c switchCallbacks.c gui.c \
	pause.c computer.c engine.c event.c game.c gltron.c gamegraphics.c \
	light.c trail.c trails_buffered.c quad.c quadbuf.c \
	input.c settings.c texture.c load_texture.c fonttex.c \
	fonts.c menu.c findpath.c file.c directory.c file_io.c \
	screenshot.c geom.c model.c material.c explosion.c explosion.h \
	system.c globals.c camera.c recognizer.c \
	console.c renderer_gl.c callbacks.h camera.h data.h \
	geom.h file.h gltron.h light.h menu.h model.h png_texture.h \
	quad.h renderer_gl.h switchCallbacks.h system.h texture.h credits.c \
	floor.c floor.h skybox.c skybox.h engine.h event.h \
	visuals_2d.c \
	scripting.c scripting.h scripting_interface.c console.h settings.h \
	util.c util.h vector.c matrix.c vector.h matrix.h \
	basic_types.h game_data.h client_data.h recognizer.h file_io.h \
	system_keynames.c system_keynames.h \
	timedemo.c timedemo.h random.c random.h \
	graphics_lights.c graphics_lights.h \
	graphics_world.c graphics_world.h \
	graphics_hud.c graphics_hud.h \
	graphics_fx.c graphics_fx.h \
	graphics_utility.c graphics_utility.h \
	trail_geometry.c trail_render.c trail_geometry.h \
	# extgl.h extgl.c 


gltron_LDADD = @NETWORK_OBJS@ @SOUND_OBJS@ lua/src/liblua.a lua/src/lib/liblualib.a -lm

gltron_DEPENDENCIES = @NETWORK_OBJS@ @SOUND_OBJS@ lua/src/liblua.a lua/src/lib/liblualib.a

man_MANS = gltron.6

EXTRA_gltron_SOURCES = sound.c sound_glue.cpp sound_glue.h \
	Source.cpp Source3D.cpp SourceMusic.cpp SourceSample.cpp \
	SoundSystem.cpp SourceEngine.h SourceEngine.cpp \
	SourceCopy.cpp Sound.h SoundSystem.h SourceMusic.h SourceCopy.h Source3D.h \
	Source.h Vector3.h SourceSample.h

todo:
	perl todo2html.pl < TODO > TODO.html
