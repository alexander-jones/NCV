#-------------------------------------------------
#
# Project created by QtCreator 2012-09-27T02:09:43
#
#-------------------------------------------------

#QMAKE_CXXFLAGS += -std=c++0x

CONFIG+= qxt
QXT += core gui
QT  += core network xml gui opengl
TARGET = ncv
TEMPLATE = app
ICON = media/ncv.png

#linux specific settings
unix:!macx{
    LIBS +=  -lssh -lGLU  -lGLEW
}

#windows specific settings
win32{
    LIBS += -lssh  -lglew32
}

#mac specific settings
macx{
}

SOURCES += \
    main.cpp \
    core/ncsneuronset.cpp \
    core/ncsdiscreteattribute.cpp \
    core/ncscontinuousattribute.cpp \
    core/ncsconnectionset.cpp \
    core/ncscluster.cpp \
    core/ncsattribute.cpp \
    core/ncscommandarguments.cpp\
    core/command/qsshsocket.cpp \
    core/command/ncsremotecommandbridge.cpp \
    core/command/ncslocalcommandbridge.cpp \
    core/reporting/qsocketconnection.cpp \
    core/reporting/qreportclient.cpp \
    core/reporting/networkupdatemanager.cpp \
    core/utilities/simplecrypt.cpp \
    gui/mainwindow.cpp \
    gui/ncsconnectionwizardpage.cpp \
    gui/remoteconnectionwidget.cpp \
    gui/utilities/qwidgetvector.cpp \
    gui/utilities/qswitch.cpp \
    gui/utilities/discretecolorselector.cpp \
    gui/utilities/continuouscolorselector.cpp \
    gui/utilities/combowidget.cpp \
    gui/utilities/colorbutton.cpp \
    gui/utilities/qgroupvector.cpp \
    plugins/lifLauncher/lifmodeldistributionwidget.cpp \
    plugins/clusterEditor/ncsclustereditor.cpp \
    plugins/izhLauncher/izhmodeldistributionwidget.cpp \
    plugins/visualizer/skysphere.cpp \
    plugins/visualizer/ncvwidget.cpp \
    plugins/visualizer/ncvrendertool.cpp \
    plugins/visualizer/ncvneuronset.cpp \
    plugins/visualizer/ncvdiscreteattribute.cpp \
    plugins/visualizer/ncvcontinuousattribute.cpp \
    plugins/visualizer/ncvconnectionset.cpp \
    plugins/visualizer/ncvcanvas.cpp \
    plugins/visualizer/qglx/qglxtexture3d.cpp \
    plugins/visualizer/qglx/qglxtexture2d.cpp \
    plugins/visualizer/qglx/qglxtexture1d.cpp \
    plugins/visualizer/qglx/qglxtexture.cpp \
    plugins/visualizer/qglx/qglxpainter.cpp \
    plugins/visualizer/qglx/qglxbuffertexture.cpp \
    plugins/visualizer/qglx/qglxframebufferobject.cpp \
    plugins/visualizer/qglx/qglxcamera.cpp \
    plugins/visualizer/qglx/qglxbuffer.cpp \
    plugins/visualizer/qglx/qglxboundingbox.cpp

HEADERS  += \
    core/ncsneuronset.h \
    core/ncsdiscreteattribute.h \
    core/ncscontinuousattribute.h \
    core/ncsconnectionset.h \
    core/ncscluster.h \
    core/ncsattribute.h \
    core/ncscommandbridge.h \
    core/ncscommandarguments.h \
    core/command/qsshsocket.h \
    core/command/ncsremotecommandbridge.h \
    core/command/ncslocalcommandbridge.h \
    core/reporting/qsocketconnection.h \
    core/reporting/qreportclient.h \
    core/reporting/sleeper.h \
    core/reporting/networkupdatemanager.h \
    core/utilities/simplecrypt.h \
    core/plugin-interfaces/ncssubscriberwidgetplugin.h \
    core/plugin-interfaces/ncswidgetplugin.h \
    core/plugin-interfaces/ncsdistributionwidgetplugin.h \
    core/plugin-interfaces/ncsapplicationwidgetplugin.h \
    gui/mainwindow.h \
    gui/ncsconnectionwizardpage.h \
    gui/remoteconnectionwidget.h \
    gui/utilities/qgroupvector.h \
    gui/utilities/qwidgetvector.h \
    gui/utilities/qswitch.h \
    gui/utilities/discretecolorselector.h \
    gui/utilities/continuouscolorselector.h \
    gui/utilities/combowidget.h \
    gui/utilities/colorbutton.h \
    plugins/clusterEditor/ncsclustereditor.h \
    plugins/izhLauncher/izhmodeldistributionwidget.h \
    plugins/lifLauncher/lifmodeldistributionwidget.h \
    plugins/visualizer/skysphere.h \
    plugins/visualizer/ncvwidget.h \
    plugins/visualizer/ncvrendertool.h \
    plugins/visualizer/ncvneuronset.h \
    plugins/visualizer/ncvdiscreteattribute.h \
    plugins/visualizer/ncvcontinuousattribute.h \
    plugins/visualizer/ncvconnectionset.h \
    plugins/visualizer/ncvcanvas.h \
    plugins/visualizer/ncvattribute.h \
    plugins/visualizer/qglx/qglxbuffertexture.h \
    plugins/visualizer/qglx/qglxtexture3d.h \
    plugins/visualizer/qglx/qglxtexture2d.h \
    plugins/visualizer/qglx/qglxtexture1d.h \
    plugins/visualizer/qglx/qglxtexture.h \
    plugins/visualizer/qglx/qglxpainter.h \
    plugins/visualizer/qglx/qglxframebufferobject.h \
    plugins/visualizer/qglx/qglxcamera.h \
    plugins/visualizer/qglx/qglxbuffer.h \
    plugins/visualizer/qglx/qglxboundingbox.h

OTHER_FILES += \
    Doxyfile \
    plugins/visualizer/shaders/silhouette.frag \
    plugins/visualizer/shaders/neuronSilhouette.vert \
    plugins/visualizer/shaders/neuronDiscrete.vert \
    plugins/visualizer/shaders/neuronContinuous.vert \
    plugins/visualizer/shaders/discrete.frag \
    plugins/visualizer/shaders/continuous.frag \
    plugins/visualizer/shaders/connectionSilhouette.vert \
    plugins/visualizer/shaders/connectionSilhouette.geom \
    plugins/visualizer/shaders/connectionDiscrete.vert \
    plugins/visualizer/shaders/connectionDiscrete.geom \
    plugins/visualizer/shaders/connectionContinuous.vert \
    plugins/visualizer/shaders/connectionContinuous.geom

RESOURCES += \
    core-profile.qrc
