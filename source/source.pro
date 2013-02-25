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
    LIBS +=  -lGLEW -lGLU  -lslugUtil -lslugSpec -lslugSim
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
    qglxbuffer.cpp \
    ncvwidget.cpp \
    spinbox3d.cpp \
    qglxtexture1d.cpp \
    qglxtexture3d.cpp \
    qglxtexture2d.cpp \
    qglxtexture.cpp \
    qglxframebufferobject.cpp \
    combowidget.cpp \
    qcustomplot.cpp \
    orientationbutton.cpp \
    qglxpainter.cpp \
    discretecolorselector.cpp \
    continuouscolorselector.cpp \
    ncvneuronset.cpp \
    ncvconnectionset.cpp \
    ncvcontinuousattribute.cpp \
    ncvdiscreteattribute.cpp \
    ncvcanvas.cpp \
    ncvsidebar.cpp \
    ncvbuildwidget.cpp \
    ncvgroupingset.cpp \
    distributewidget.cpp \
    slidingstackedwidget.cpp \
    ncsclustereditor.cpp

HEADERS  += \
    qglxcore.h \
    qglxcamera.h \
    qglxboundingbox.h \
    mainwindow.h \
    qglxbuffer.h \
    ncvwidget.h \
    qglxtexture.h \
    spinbox3d.h \
    qglxtexture1d.h \
    qglxtexture3d.h \
    qglxtexture2d.h \
    qglxframebufferobject.h \
    combowidget.h \
    qcustomplot.h \
    orientationbutton.h \
    qglxpainter.h \
    continuouscolorselector.h \
    discretecolorselector.h \
    ncvcore.h \
    ncvneuronset.h \
    ncvconnectionset.h \
    ncvcontinuousattribute.h \
    ncvdiscreteattribute.h \
    ncvcanvas.h \
    ncvsidebar.h \
    ncvbuildwidget.h \
    ncvgroupingset.h \
    distributewidget.h \
    slidingstackedwidget.h \
    colorbutton.h \
    ncsclustereditor.h

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
    assets/sphere.nff \
    assets/startTriangle.png \
    assets/endTriangle.png \
    shaders/neuronSelection.vert \
    shaders/neuronSelection.geom \
    shaders/silhouette.frag \
    shaders/connectionSilhouette.vert \
    shaders/connectionSilhouette.geom \
    shaders/neuronSilhouetteTest.vert \
    shaders/neuronSilhouetteTest.geom \
    shaders/neuronSilhouette.vert \
    shaders/connectionDiscrete.geom \
    shaders/connectionDiscrete.vert \
    shaders/discrete.frag \
    shaders/neuronDiscrete.vert \
    shaders/neuronDiscreteTest.vert \
    shaders/neuronDiscreteTest.geom \
    shaders/connectionContinuous.geom \
    shaders/connectionContinuous.vert \
    shaders/continuous.frag \
    shaders/neuronContinuous.vert \
    shaders/neuronContinuousTest.geom \
    shaders/neuronContinuousTest.vert

RESOURCES += \
    core-profile.qrc
