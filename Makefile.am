# GNU automake Makefile.am for gltron

EXTRA_DIST = CREDITS README.html LICENSE CHANGELOG

SUBDIRS = art data sounds tools win32

bin_PROGRAMS = gltron

gltron_SOURCES = artpack.c png_texture.c pixels.c switchCallbacks.c gui.c \
	pause.c computer.c engine.c event.c game.c gltron.c gamegraphics.c \
	light.c software.c trail.c trails_buffered.c quad.c quadbuf.c \
	input.c settings.c texture.c load_texture.c fonttex.c \
	fonts.c menu.c findpath.c file.c directory.c model.c \
	modelgraphics.c screenshot.c skybox.c mtllib.c geom.c \
	system.c globals.c camera.c recognizer.c \
	console.c renderer_gl.c callbacks.h camera.h data.h \
	geom.h gltron.h light.h menu.h model.h png_texture.h \
	quad.h renderer_gl.h switchCallbacks.h system.h texture.h credits.c \
	wprogress.c splash.c  wprogress.h splash.h mouse.c mouse.h wstatictext.c \
	wstatictext.h

gltron_LDADD = @NETWORK_OBJS@ @SOUND_OBJS@

gltron_DEPENDENCIES = @NETWORK_OBJS@ @SOUND_OBJS@

EXTRA_gltron_SOURCES = sound.c sound3d.c sound.h sound3d.h \
	network.c system_net.c system_net.h

todo:
	perl todo2html.pl < TODO > TODO.html
