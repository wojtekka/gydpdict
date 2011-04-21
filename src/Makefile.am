AM_CFLAGS = -Wall

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = gydpdict

gydpdict_SOURCES = \
	main.c \
	callbacks.c callbacks.h \
	dictionary.c dictionary.h

gydpdict_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)

gydpdict_LDFLAGS = -rdynamic
