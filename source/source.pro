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
    qglxcamera.cpp \
    qglxboundingbox.cpp \
    mainwindow.cpp \
    main.cpp \
    qglxsystem.cpp \
    qglxbuffer.cpp \
    ncv.cpp \
    righttoolbar.cpp \
    ncvwidget.cpp \
    managementSidebar.cpp \
    sidebar.cpp \
    camerasidebar.cpp \
    spinbox3d.cpp \
    lightingsidebar.cpp \
    collapsiblewidget.cpp \
    qglxtexture1d.cpp \
    qglxtexture3d.cpp \
    qglxtexture2d.cpp \
    qglxtexture.cpp \
    qglxframebufferobject.cpp \
    colorrangewidget.cpp \
    attributewidget.cpp \
    colorbitwidget.cpp

HEADERS  += \
    qglxoctree.h \
    qglxcamera.h \
    qglxboundingbox.h \
    mainwindow.h \
    qglxsystem.h \
    qglxbuffer.h \
    qglxcore.h \
    ncv.h \
    righttoolbar.h \
    ncvwidget.h \
    qglxtexture.h \
    managementSidebar.h \
    sidebar.h \
    camerasidebar.h \
    spinbox3d.h \
    lightingsidebar.h \
    collapsiblewidget.h \
    qglxtexture1d.h \
    qglxtexture3d.h \
    qglxtexture2d.h \
    qglxframebufferobject.h \
    colorrangewidget.h \
    attributewidget.h \
    colorbitwidget.h

FORMS    += \
    mainwindow.ui

OTHER_FILES += \
    Doxyfile \
    assets/cube.nff \
    shaders/synapse.frag \
    shaders/synapse.vert \
    shaders/neuron.vert \
    shaders/neuron.frag \
    shaders/selectionRect.frag \
    shaders/selection.frag \
    shaders/postProcess.vert \
    shaders/synapse.geom \
    shaders/lighting.frag \
    shaders/neuronFloat.vert \
    shaders/neuronBit.vert \
    shaders/bit.frag \
    shaders/float.frag \
    shaders/connectionBit.geom \
    shaders/connectionBit.vert \
    shaders/connectionFloat.vert \
    shaders/connectionFloat.geom \
    shaders/simpleBlend.frag \
    assets/sphere.nff \
    assets/startTriangle.png \
    assets/endTriangle.png

RESOURCES += \
    core-profile.qrc

#QMAKE_CXXFLAGS += -std=c++0x
