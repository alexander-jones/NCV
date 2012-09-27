#-------------------------------------------------
#
# Project created by QtCreator 2012-09-27T02:01:57
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = brain-viz
TEMPLATE = app


SOURCES += main.cpp \
    glwidget.cpp \
    camera.cpp

HEADERS  += \
    glwidget.h \
    camera.h

FORMS    +=

RESOURCES += \
    core-profile.qrc

OTHER_FILES += \
    simple.vert \
    simple.frag
