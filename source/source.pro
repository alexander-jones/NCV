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
    ncvglwidget.cpp

HEADERS  += \
    qglxoctree.h \
    qglxdynamicframebuffer.h \
    qglxcamera.h \
    qglxboundingbox.h \
    mainwindow.h \
    qglxcore.h \
    qglxsystem.h \
    qglxbuffer.h \
    ncvglwidget.h

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
    shaders/postProcess.vert

RESOURCES += \
    core-profile.qrc

#QMAKE_CXXFLAGS += -std=c++0x
