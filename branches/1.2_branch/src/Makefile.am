# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/src $(all_includes)

# these are the headers for your project
noinst_HEADERS = kiptablesgenerator.h kiptablesgenerator.h rulesdialog.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kiptablesgenerator.pot

KDE_ICON = kiptablesgenerator

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kiptablesgenerator

# the application source, library search path, and link libraries
kiptablesgenerator_SOURCES = main.cpp kiptablesgenerator.cpp rulesdialog.cpp
kiptablesgenerator_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kiptablesgenerator_LDADD = $(LIB_KIO) $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = kiptablesgenerator.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/kiptablesgenerator



