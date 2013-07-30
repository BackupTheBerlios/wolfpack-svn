
TEMPLATE = lib
TARGET = zlib
DESTDIR = lib

#CONFIG -= qt
CONFIG += staticlib warn_off release exceptions rtti

win32:DEFINES += _CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_DEPRECATE

INCLUDEPATH += .

# Input
HEADERS += crc32.h \
           deflate.h \
           gzguts.h \
           inffast.h \
           inffixed.h \
           inflate.h \
           inftrees.h \
           trees.h \
           zconf.h \
           zlib.h \
           zutil.h
SOURCES += adler32.c \
           compress.c \
           crc32.c \
           deflate.c \
           gzclose.c \
           gzlib.c \
           gzread.c \
           gzwrite.c \
           infback.c \
           inffast.c \
           inflate.c \
           inftrees.c \
           trees.c \
           uncompr.c \
           zutil.c
