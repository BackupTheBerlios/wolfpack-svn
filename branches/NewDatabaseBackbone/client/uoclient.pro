
PROJECT = uoclient
TARGET = uoclient
TEMPLATE = app
OBJECTS_DIR = obj

CONFIG += qt thread exceptions release

# On Win32 Only MSVC++.NET is "really" supported
win32:GUID = {667DA75C-5D17-4F29-89E4-150CE6A077C4}
win32:MAKEFILE_GENERATOR = MSVC.NET
win32:TEMPLATE = vcapp
win32:QMAKE_LIBS_QT_THREAD = qt-mt333.lib

win32:CONFIG += windows
unix:CONFIG += x11 
unix:INCLUDEPATH += /usr/include/GL /usr/include/X11 /usr/include

# No idea what it does
# DEFINES += QT_CLEAN_NAMESPACE QT_COMPAT_WARNINGS

# not yet
#RC_FILE = res.rc
OBJECTS_DIR = obj
MOC_DIR = obj

win32:DEFINES -= UNICODE
win32:LIBS += libs/SDL.lib wsock32.lib opengl32.lib glu32.lib winmm.lib dxguid.lib

unix:LIBS += -lGL -lGLU

DEPENDPATH += src
INCLUDEPATH += include;libs/include

# MAIN INCLUDES
HEADERS += \
	include/binkw.h \
	include/config.h \
	include/enums.h \
	include/exceptions.h \
	include/log.h \
	include/mainwindow.h \
	include/mersennetwister.h \
	include/surface.h \
	include/texture.h \
	include/uoclient.h \
	include/vector.h 
	
HEADERS += \
	include/dialogs/login.h \
	include/dialogs/movecenter.h
	
HEADERS += \
	include/game/dynamicentity.h \
	include/game/entity.h \
	include/game/groundtile.h \
	include/game/mobile.h \
	include/game/statictile.h \
	include/game/world.h

# GUI INCLUDES
HEADERS += \
	include/gui/asciilabel.h \
	include/gui/bordergump.h \
	include/gui/container.h \
	include/gui/contextmenu.h \
	include/gui/control.h \
	include/gui/cursor.h \
	include/gui/gui.h \
	include/gui/gumpimage.h \
	include/gui/imagebutton.h \
	include/gui/itemimage.h \
	include/gui/label.h \
	include/gui/scrollbar.h \
	include/gui/textfield.h \
	include/gui/tiledgumpimage.h \
	include/gui/window.h \
	include/gui/worldview.h 

# MUL INCLUDES
HEADERS += \
	include/muls/animations.h \
	include/muls/art.h \
	include/muls/asciifonts.h \
	include/muls/gumpart.h \
	include/muls/hues.h \
	include/muls/maps.h \
	include/muls/textures.h \
	include/muls/tiledata.h \
	include/muls/unicodefonts.h \
	include/muls/verdata.h

HEADERS += \
	include/network/encryption.h \
	include/network/uopacket.h \
	include/network/uosocket.h

# MAIN src

SOURCES += \
	src/binkw.cpp \
	src/config.cpp \
	src/log.cpp \
	src/mainwindow.cpp \
	src/surface.cpp \
	src/texture.cpp \
	src/uoclient.cpp \
	src/utilities.cpp \
	src/vector.cpp

SOURCES += \
	src/dialogs/login.cpp \
	src/dialogs/movecenter.cpp 

SOURCES += \
	src/gui/asciilabel.cpp \
	src/gui/bordergump.cpp \
	src/gui/container.cpp \
	src/gui/contextmenu.cpp \
	src/gui/control.cpp \
	src/gui/cursor.cpp \
	src/gui/gui.cpp \
	src/gui/gumpimage.cpp \
	src/gui/imagebutton.cpp \
	src/gui/itemimage.cpp \
	src/gui/label.cpp \
	src/gui/scrollbar.cpp \
	src/gui/textfield.cpp \
	src/gui/tiledgumpimage.cpp \
	src/gui/window.cpp \
	src/gui/worldview.cpp 

SOURCES += \
	src/network/encryption.cpp \
	src/network/loginpackets.cpp \
	src/network/uopacket.cpp \
	src/network/uosocket.cpp

SOURCES += \
	src/game/dynamicentity.cpp \
	src/game/entity.cpp \
	src/game/groundtile.cpp \
	src/game/mobile.cpp \
	src/game/statictile.cpp \
	src/game/world.cpp

# MUL srcS
SOURCES += \
	src/muls/animations.cpp \
	src/muls/art.cpp \
	src/muls/asciifonts.cpp \
	src/muls/gumpart.cpp \
	src/muls/hues.cpp \
	src/muls/maps.cpp \
	src/muls/textures.cpp \
	src/muls/tiledata.cpp \
	src/muls/unicodefonts.cpp \
	src/muls/verdata.cpp

INTERFACES =

#TRANSLATIONS = \
#	languages/wolfpack_pt_br.ts \
#	languages/wolfpack_it.ts \
#	languages/wolfpack_nl.ts \
#	languages/wolfpack_es.ts \
#	languages/wolfpack_de.ts \
#	languages/wolfpack_fr.ts \
#	languages/wolfpack_ge.ts

#unix:SOURCES  += \
#	../src/unix/config_unix.cpp \
#	../src/unix/console_unix.cpp \
#	../src/unix/getopts_unix.cpp

win32:SOURCES += \
	src/config_win.cpp \
	src/startup_win.cpp \
	src/utilities_win.cpp
	
unix:SOURCES += \
	src/config_unix.cpp \
	src/startup_unix.cpp \
	src/utilities_unix.cpp

#
#DISTFILES += \
#	../release/AUTHORS.txt \
#	../release/LICENSE.GPL
#
