CONFIG += qsci

#linux specific settings
unix:!macx{
    LIBS += -lqscintilla2
}

#windows specific settings
win32{
    LIBS += -lqscintilla2
}

#mac specific settings
macx{
}
SOURCES += \
    plugins/pythonEditor/pythoneditor.cpp

HEADERS  += \
    plugins/pythonEditor/pythoneditor.h

OTHER_FILES += \
    plugins/pythonEditor/resources/ncspython.api

RESOURCES += \
    plugins/pythonEditor/pythonEditor.qrc

