#-------------------------------------------------
#
# Project created by QtCreator 2012-09-27T02:09:43
#
#-------------------------------------------------

QMAKE_CXXFLAGS += "-std=gnu++0x"

QT  += core gui opengl
TARGET = source
TEMPLATE = app
CONFIG+= wwwidgets


#linux specific settings
unix:!macx{
    LIBS +=  -lGLEW -lGLU #/home/alex/Dropbox/brain-viz/source/lib/libslugSim.so /home/alex/Dropbox/brain-viz/source/lib/libslugSpec.so /home/alex/Dropbox/brain-viz/source/lib/libslugUtil.so
}

#windows specific settings
win32{
    LIBS += -lglew32 -lwwwidgets4d
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
    colorstatewidget.cpp

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
    managementcombowidget.h \
    cameracombowidget.h \
    spinbox3d.h \
    qglxtexture1d.h \
    qglxtexture3d.h \
    qglxtexture2d.h \
    qglxframebufferobject.h \
    colorrangewidget.h \
    attributewidget.h \
    colorbitwidget.h \
    combowidget.h \
    managementSidebar.h \
    camerasidebar.h \
    qcustomplot.h \
    orientationbutton.h \
    distributewidget.h \
    ncvattribute.h \
    colorstatewidget.h

FORMS    += \
    mainwindow.ui

OTHER_FILES += \
    Doxyfile \
    assets/cube.nff \
    shaders/synapse.frag \
    shaders/synapse.vert \
    shaders/neuron.vert \
    shaders/neuron.frag \
    shaders/synapse.geom \
    shaders/neuronFloat.vert \
    shaders/neuronBit.vert \
    shaders/bit.frag \
    shaders/float.frag \
    shaders/connectionBit.geom \
    shaders/connectionFloat.geom \
    assets/sphere.nff \
    assets/startTriangle.png \
    assets/endTriangle.png \
    shaders/neuronSelection.vert \
    shaders/neuronSelection.geom \
    shaders/connectionBit.vert \
    shaders/connectionFloat.vert \
    shaders/silhouette.frag \
    shaders/connectionSilhouette.vert \
    shaders/connectionSilhouette.geom \
    shaders/neuronSilhouette.vert \
    shaders/neuronSilhouette.geom

RESOURCES += \
    core-profile.qrc
