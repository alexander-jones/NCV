#-------------------------------------------------
#
# Project created by QtCreator 2012-09-27T02:09:43
#
#-------------------------------------------------

#QMAKE_CXXFLAGS += -std=c++0x

QT  += core network xml gui opengl wwwidgets
TARGET = ncv
TEMPLATE = app
CONFIG+= wwwidgets
ICON = assets/ncv.png

#linux specific settings
unix:!macx{
    LIBS +=  -lssh -lGLU  -lGLEW
}

#windows specific settings
win32{
    LIBS += -lssh  -lglew32  -lwwwidgets4d
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
    ncvgroupingset.cpp \
    ncsclustereditor.cpp \
    simplecrypt.cpp \
    sshsocket.cpp \
    ncvrendertool.cpp \
    qswitch.cpp \
    qwidgetvector.cpp \
    skysphere.cpp \
    remoteconnectionwidget.cpp \
    ncslocalcommandbridge.cpp \
    ncsmodeldistributionwidget.cpp \
    ncscommandfileargument.cpp \
    ncsconnectionwidget.cpp \
    ncsremotecommandbridge.cpp \
    ncsdatasource.cpp \
    reportclient/qsocketconnection.cpp \
    reportclient/qreportclient.cpp \
    networkupdatethread.cpp \
    networkupdatemanager.cpp \
    qglxcanvas.cpp \
    qgroupvector.cpp \
    izhmodeldistributionwidget.cpp

HEADERS  += \
    qglxcore.h \
    qglxcamera.h \
    qglxboundingbox.h \
    mainwindow.h \
    qglxbuffer.h \
    ncvwidget.h \
    qglxtexture.h \
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
    ncvgroupingset.h \
    colorbutton.h \
    ncsclustereditor.h \
    simplecrypt.h \
    sshsocket.h \
    ncvrendertool.h \
    qswitch.h \
    qwidgetvector.h \
    skysphere.h \
    remoteconnectionwidget.h \
    ncscommandbridge.h \
    ncslocalcommandbridge.h \
    ncsmodeldistributionwidget.h \
    ncswidgetplugin.h \
    ncsapplicationwidgetplugin.h \
    ncsdistributionwidgetplugin.h \
    ncscommandfileargument.h \
    ncsconnectionwidget.h \
    ncsremotecommandbridge.h \
    ncsdatasource.h \
    reportclient/qsocketconnection.h \
    reportclient/qreportclient.h \
    networkupdatethread.h \
    networkupdatemanager.h \
    qglxcanvas.h \
    qgroupvector.h \
    izhmodeldistributionwidget.h

FORMS    += \
    mainwindow.ui

OTHER_FILES += \
    Doxyfile \
    assets/cube.nff \
    shaders/synapse.frag \
    shaders/synapse.vert \
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
    shaders/neuronSilhouette.vert \
    shaders/connectionDiscrete.geom \
    shaders/connectionDiscrete.vert \
    shaders/discrete.frag \
    shaders/connectionContinuous.geom \
    shaders/connectionContinuous.vert \
    shaders/continuous.frag \
    shaders/neuronDiscrete.vert \
    shaders/neuronContinuous.vert \
    styles/switch.qss

RESOURCES += \
    core-profile.qrc
