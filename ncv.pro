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
    LIBS +=  -lssh -lGLU  /usr/lib64/libGLEW.so.1.9.0
}

#windows specific settings
win32{
    LIBS += -lssh  -lglew32  -lwwwidgets4d
}

#mac specific settings
macx{
}

SOURCES += \
    core/ncsneuronset.cpp \
    core/ncsdiscreteattribute.cpp \
    core/ncscontinuousattribute.cpp \
    core/ncsconnectionset.cpp \
    core/ncscluster.cpp \
    core/ncsattribute.cpp \
    core/gui/utilities/qwidgetvector.cpp \
    core/gui/utilities/qswitch.cpp \
    core/gui/utilities/discretecolorselector.cpp \
    core/gui/utilities/continuouscolorselector.cpp \
    core/gui/utilities/combowidget.cpp \
    core/networking/command/ncsremotecommandbridge.cpp \
    core/networking/command/ncslocalcommandbridge.cpp \
    core/networking/reporting/qsocketconnection.cpp \
    core/networking/reporting/qreportclient.cpp \
    core/networking/reporting/networkupdatemanager.cpp \
    core/utilities/simplecrypt.cpp \
    mainwindow.cpp \
    plugins/clusterEditor/ncsclustereditor.cpp \
    plugins/connection/remoteconnectionwidget.cpp \
    plugins/connection/ncsconnectionwidget.cpp \
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
    plugins/visualizer/qglx/qglxframebufferobject.cpp \
    plugins/visualizer/qglx/qglxcamera.cpp \
    plugins/visualizer/qglx/qglxbuffer.cpp \
    plugins/visualizer/qglx/qglxboundingbox.cpp \
    main.cpp \
    core/gui/utilities/colorbutton.cpp \
    plugins/lifLauncher/lifmodeldistributionwidget.cpp \
    core/networking/command/qsshsocket.cpp \
    plugins/visualizer/qglx/qglxbuffertexture.cpp \
    core/gui/utilities/qgroupvector.cpp \
    core/networking/command/ncscommandbridge.cpp

HEADERS  += \
    core/ncsneuronset.h \
    core/ncsdiscreteattribute.h \
    core/ncscontinuousattribute.h \
    core/ncsconnectionset.h \
    core/ncscluster.h \
    core/ncsattribute.h \
    core/gui/plugin-interfaces/ncswidgetplugin.h \
    core/gui/plugin-interfaces/ncsdistributionwidgetplugin.h \
    core/gui/plugin-interfaces/ncsapplicationwidgetplugin.h \
    core/gui/utilities/qwidgetvector.h \
    core/gui/utilities/qswitch.h \
    core/gui/utilities/discretecolorselector.h \
    core/gui/utilities/continuouscolorselector.h \
    core/gui/utilities/combowidget.h \
    core/gui/utilities/colorbutton.h \
    core/networking/command/ncsremotecommandbridge.h \
    core/networking/command/ncslocalcommandbridge.h \
    core/networking/command/ncscommandbridge.h \
    core/networking/reporting/qsocketconnection.h \
    core/networking/reporting/qreportclient.h \
    core/networking/reporting/networkupdatemanager.h \
    core/utilities/simplecrypt.h \
    plugins/clusterEditor/ncsclustereditor.h \
    plugins/connection/remoteconnectionwidget.h \
    plugins/connection/ncsconnectionwidget.h \
    plugins/izhLauncher/izhmodeldistributionwidget.h \
    plugins/visualizer/skysphere.h \
    plugins/visualizer/ncvwidget.h \
    plugins/visualizer/ncvrendertool.h \
    plugins/visualizer/ncvneuronset.h \
    plugins/visualizer/ncvdiscreteattribute.h \
    plugins/visualizer/ncvcontinuousattribute.h \
    plugins/visualizer/ncvconnectionset.h \
    plugins/visualizer/ncvcanvas.h \
    plugins/visualizer/qglx/qglxtexture3d.h \
    plugins/visualizer/qglx/qglxtexture2d.h \
    plugins/visualizer/qglx/qglxtexture1d.h \
    plugins/visualizer/qglx/qglxtexture.h \
    plugins/visualizer/qglx/qglxpainter.h \
    plugins/visualizer/qglx/qglxframebufferobject.h \
    plugins/visualizer/qglx/qglxcamera.h \
    plugins/visualizer/qglx/qglxbuffer.h \
    plugins/visualizer/qglx/qglxboundingbox.h \
    mainwindow.h \
    plugins/lifLauncher/lifmodeldistributionwidget.h \
    core/gui/plugin-interfaces/ncsconnectionwidgetplugin.h \
    core/gui/plugin-interfaces/ncssubscriberwidgetplugin.h \
    core/networking/command/qsshsocket.h \
    plugins/visualizer/qglx/qglxbuffertexture.h \
    plugins/visualizer/ncvattribute.h \
    core/gui/utilities/qgroupvector.h

FORMS    += \
    mainwindow.ui \
    mainwindow.ui

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
