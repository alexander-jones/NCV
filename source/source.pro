#-------------------------------------------------
#
# Project created by QtCreator 2012-09-27T02:09:43
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = source
TEMPLATE = app


SOURCES += main.cpp \
    glwidget.cpp \
    camera.cpp \
    mainwindow.cpp \
    sphere.cpp \
    multipletargetframebuffer.cpp

HEADERS  += \
    glwidget.h \
    camera.h \
    mainwindow.h \
    glhelper.h \
    sphere.h \
    multipletargetframebuffer.h

FORMS    += \
    mainwindow.ui

OTHER_FILES += \
    simple.vert \
    simple.frag

RESOURCES += \
    core-profile.qrc
