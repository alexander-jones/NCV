#ifndef PYTHONEDITOR_H
#define PYTHONEDITOR_H

#include <QWidget>

class QMenuBar;
class QVBoxLayout;
class QButtonion;
class QToolBar;
class QMenu;
class QStatusBar;
class QsciScintilla;
class QsciLexerPython;
class QsciAPIs;
class QToolButton;
#include "core/plugin-interfaces/ncswidgetplugin.h"

class PythonEditor : public NCSWidgetPlugin
{
    Q_OBJECT
public:
    explicit PythonEditor(QWidget *parent = 0);
    void loadProject(QString projectDir) ;
    QIcon icon() ;
    QString title() ;
    QString name();
    float version();

public slots:
    void initialize() ;
    void cleanup() ;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();

private:
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QsciScintilla *m_textEdit;
    QString m_curFile,m_projectDir;
    QToolBar *m_fileToolBar;
    QToolButton *m_newButton, *m_openButton, *m_saveButton, *m_saveAsButton;
    QToolButton *m_cutButton, *m_copyButton, *m_pasteButton;
    QsciLexerPython * m_lexer;
    QsciAPIs  * m_api;
    QVBoxLayout * m_layout;
    QStatusBar * m_statusBar;
};

#endif // PYTHONEDITOR_H
