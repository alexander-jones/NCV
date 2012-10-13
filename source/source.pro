#-------------------------------------------------
#
# Project created by QtCreator 2012-09-27T02:09:43
#
#-------------------------------------------------

QT  += core gui opengl
#LIBS +=  -lGLEW  -lassimp
TARGET = source
TEMPLATE = app


SOURCES += main.cpp \
    glwidget.cpp \
    camera.cpp \
    mainwindow.cpp \
    multipletargetframebuffer.cpp

HEADERS  += \
    glwidget.h \
    camera.h \
    mainwindow.h \
    glhelper.h \
    multipletargetframebuffer.h

FORMS    += \
    mainwindow.ui

OTHER_FILES += \
    multi_target.vert \
    multi_target.frag \
    cube.nff \
    final.vert

RESOURCES += \
    core-profile.qrc
