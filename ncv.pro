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
ICON =

#linux specific settings
unix:!macx{
    LIBS +=  -lssh
}

#windows specific settings
win32{
    LIBS += -lssh
}

#mac specific settings
macx{
}

include(plugins/clusterEditor/clusterEditor.pri)
include(plugins/izhLauncher/izhLauncher.pri)
include(plugins/lifLauncher/lifLauncher.pri)
include(plugins/visualizer/visualizer.pri)
include(plugins/pythonEditor/pythonEditor.pri)

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
    core/command/ncsremoteapplicationbridge.cpp \
    core/command/ncslocalapplicationbridge.cpp\
    core/reporting/qsocketconnection.cpp \
    core/reporting/qreportclient.cpp \
    core/reporting/networkupdatemanager.cpp \
    core/utilities/simplecrypt.cpp \
    core/ncsproject.cpp \
    core/ncsprojectportal.cpp \
    gui/ncsinstallationdialog.cpp \
    gui/ncsdialog.cpp \
    gui/mainwindow.cpp \
    gui/utilities/qwidgetvector.cpp \
    gui/utilities/qswitch.cpp \
    gui/utilities/combowidget.cpp \
    gui/utilities/qgroupvector.cpp

HEADERS  += \
    core/ncsneuronset.h \
    core/ncsdiscreteattribute.h \
    core/ncscontinuousattribute.h \
    core/ncsconnectionset.h \
    core/ncscluster.h \
    core/ncsattribute.h \
    core/ncscommandbridg e.h \
    core/ncscommandarguments.h \
    core/ncsapplicationbridge.h \
    core/command/ncsremoteapplicationbridge.h \
    core/command/ncslocalapplicationbridge.h \
    core/command/ncsinternalcommandbridge.h \
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
    core/ncsproject.h \
    core/ncsprojectportal.h \
    core/ncscommandbridge.h \
    gui/mainwindow.h \
    gui/ncsinstallationdialog.h \
    gui/ncsdialog.h \
    gui/utilities/qgroupvector.h \
    gui/utilities/qwidgetvector.h \
    gui/utilities/qswitch.h \
    gui/utilities/combowidget.h


OTHER_FILES += \
    Doxyfile \
    README.md

RESOURCES += \
    ncv.qrc
