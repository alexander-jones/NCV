#linux specific settings
unix:!macx{
    LIBS +=  -lGLU  -lGLEW
}

#windows specific settings
win32{
    LIBS +=  -lglew32
}

#mac specific settings
macx{
}

SOURCES += \
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
    plugins/visualizer/qglx/qglxboundingbox.cpp \
    plugins/visualizer/discretecolorselector.cpp \
    plugins/visualizer/continuouscolorselector.cpp \
    plugins/visualizer/colorbutton.cpp

HEADERS  += \
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
    plugins/visualizer/qglx/qglxboundingbox.h \
    plugins/visualizer/discretecolorselector.h \
    plugins/visualizer/continuouscolorselector.h \
    plugins/visualizer/colorbutton.h


OTHER_FILES += \
    plugins/visualizer/resources/shaders/neuronDiscrete.vert \
    plugins/visualizer/resources/shaders/neuronContinuous.vert \
    plugins/visualizer/resources/shaders/discrete.frag \
    plugins/visualizer/resources/shaders/continuous.frag \
    plugins/visualizer/resources/shaders/connectionDiscrete.vert \
    plugins/visualizer/resources/shaders/connectionDiscrete.geom \
    plugins/visualizer/resources/shaders/connectionContinuous.vert \
    plugins/visualizer/resources/shaders/connectionContinuous.geom \
    plugins/visualizer/resources/shaders/ssao.frag \
    plugins/visualizer/resources/shaders/postProcess.vert

RESOURCES += \
    plugins/visualizer/visualizer.qrc
