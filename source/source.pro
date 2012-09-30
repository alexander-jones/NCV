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
    picker.cpp

HEADERS  += \
    glwidget.h \
    camera.h \
    mainwindow.h \
    picker.h \
    glhelper.h

FORMS    += \
    mainwindow.ui

OTHER_FILES += \
    simple.vert \
    simple.frag \
    pick.frag \
    pick.vert

RESOURCES += \
    core-profile.qrc
