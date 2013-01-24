#-------------------------------------------------
#
# Project created by QtCreator 2012-09-27T02:09:43
#
#-------------------------------------------------

QT  += core gui opengl
TARGET = source
TEMPLATE = app
CONFIG+= wwwidgets

#linux specific settings
unix:!macx{
    LIBS +=  -lGLEW  -lassimp -lGLU
}

#windows specific settings
win32{
    LIBS += -lglew32 -lassimp -lwwwidgets4d
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
    ncvwidget.cpp \
    managementSidebar.cpp \
    camerasidebar.cpp \
    spinbox3d.cpp \
    lightingsidebar.cpp \
    qglxtexture1d.cpp \
    qglxtexture3d.cpp \
    qglxtexture2d.cpp \
    qglxtexture.cpp \
    qglxframebufferobject.cpp \
    colorrangewidget.cpp \
    colorbitwidget.cpp \
    attributewidget.cpp \
    combowidget.cpp \
    qcustomplot.cpp \
    orientationbutton.cpp \
    distributewidget.cpp \
    randomdatasource.cpp

HEADERS  += \
    qglxcore.h \
    qglxoctree.h \
    qglxcamera.h \
    qglxboundingbox.h \
    mainwindow.h \
    qglxsystem.h \
    qglxbuffer.h \
    ncv.h \
    ncvwidget.h \
    qglxtexture.h \
    spinbox3d.h \
    qglxtexture1d.h \
    qglxtexture3d.h \
    qglxtexture2d.h \
    qglxframebufferobject.h \
    colorrangewidget.h \
    attributewidget.h \
    colorbitwidget.h \
    groupingtree.h \
    combowidget.h \
    managementSidebar.h \
    lightingsidebar.h \
    camerasidebar.h \
    qcustomplot.h \
    orientationbutton.h \
    distributewidget.h \
    randomdatasource.h

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
    shaders/connectionFloat.geom \
    shaders/simpleBlend.frag \
    assets/sphere.nff \
    assets/startTriangle.png \
    assets/endTriangle.png \
    shaders/neuronSelection.vert \
    shaders/neuronSelection.geom \
    shaders/depthOfField.frag \
    shaders/connectionBit.vert \
    shaders/connectionFloat.vert

RESOURCES += \
    core-profile.qrc

