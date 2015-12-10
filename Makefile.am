# Copyright 1999-2015 the Claws Mail team.
# This file is part of Claws Mail package, and distributed under the
# terms of the General Public License version 3 (or later).
# See COPYING file for license details.

plugindir = $(pkglibdir)/plugins

if BUILD_ARCHIVE_BUTTON_PLUGIN
plugin_LTLIBRARIES = archive_button.la
endif

archive_button_la_SOURCES = \
	archive_button.c archive_button.h \
	archive_button_prefs.c archive_button_prefs.h

archive_button_la_LDFLAGS = \
	-avoid-version -module

if CYGWIN
cygwin_export_lib = -L$(top_builddir)/src -lclaws-mail
else
cygwin_export_lib = 
endif

archive_button_la_LIBADD = $(cygwin_export_lib) \
	$(GTK_LIBS) 

archive_button_la_CPPFLAGS = \
	-I$(top_srcdir)/src \
	-I$(top_builddir)/src/common \
	-I$(top_srcdir)/src/common \
	-I$(top_srcdir)/src/gtk \
	$(GLIB_CFLAGS) \
	$(GTK_CFLAGS)
