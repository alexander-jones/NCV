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
    camera.cpp

HEADERS  += \
    glwidget.h \
    camera.h

FORMS    +=

OTHER_FILES += \
    simple.vert \
    simple.frag

RESOURCES += \
    core-profile.qrc
