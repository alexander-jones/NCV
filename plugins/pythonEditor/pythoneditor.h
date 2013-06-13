#ifndef PYTHONEDITOR_H
#define PYTHONEDITOR_H

#include <QWidget>

class QMenuBar;
class QVBoxLayout;
class QAction;
class QToolBar;
class QMenu;
class QStatusBar;
class QsciScintilla;
class QsciLexerPython;
class QsciAPIs;
#include "core/plugin-interfaces/ncswidgetplugin.h"

class PythonEditor : public NCSWidgetPlugin
{
    Q_OBJECT
public:
    explicit PythonEditor(QWidget *parent = 0);
    void loadProject(QString projectDir) ;
    QIcon icon() ;
    QString title() ;

public slots:
    void initialize() ;
    void cleanup() ;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QsciScintilla *m_textEdit;
    QString m_curFile;
    QMenuBar * m_menuBar;
    QMenu *m_fileMenu, *m_editMenu, *m_helpMenu;
    QToolBar *m_fileToolBar;
    QAction *m_newAct, *m_openAct, *m_saveAct, *m_saveAsAct, *m_exitAct;
    QAction *m_cutAct, *m_copyAct, *m_pasteAct, *m_aboutAct, *m_aboutQtAct;
    QsciLexerPython * m_lexer;
    QsciAPIs  * m_api;
    QVBoxLayout * m_layout;
    QStatusBar * m_statusBar;
};

#endif // PYTHONEDITOR_H
