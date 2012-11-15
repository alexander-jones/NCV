#-------------------------------------------------
#
# Project created by QtCreator 2012-09-27T02:09:43
#
#-------------------------------------------------

QT  += core gui opengl
TARGET = source
TEMPLATE = app

#linux specific settings
unix:!macx{
    LIBS +=  -lGLEW  -lassimp
}

#windows specific settings
winx{
}

#mac specific settings
macx{
}

SOURCES += \
    qglxdynamicframebuffer.cpp \
    qglxcamera.cpp \
    qglxboundingbox.cpp \
    mainwindow.cpp \
    main.cpp \
    qglxsystem.cpp \
    qglxbuffer.cpp \
    ncv.cpp \
    slider.cpp \
    righttoolbar.cpp \
    lefttoolbar.cpp

HEADERS  += \
    qglxoctree.h \
    qglxdynamicframebuffer.h \
    qglxcamera.h \
    qglxboundingbox.h \
    mainwindow.h \
    qglxsystem.h \
    qglxbuffer.h \
    qglxcore.h \
    ncv.h \
    slider.h \
    righttoolbar.h \
    lefttoolbar.h

FORMS    += \
    mainwindow.ui

OTHER_FILES += \
    shaders/final.vert \
    shaders/final.frag \
    Doxyfile \
    assets/cube.nff \
    shaders/synapse.frag \
    shaders/synapse.vert \
    shaders/neuron.vert \
    shaders/neuron.frag \
    shaders/synapse.geom \
    shaders/selectionRect.frag \
    shaders/selectionRect.vert \
    shaders/selection.frag \
    shaders/selection.vert

RESOURCES += \
    core-profile.qrc

#QMAKE_CXXFLAGS += -std=c++0x
