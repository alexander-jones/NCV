#ifndef PYTHONEDITOR_H
#define PYTHONEDITOR_H

#include <QAction>
#include <QVBoxLayout>
#include <QToolBar>
#include <QToolButton>
#include <QStatusBar>
#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPoint>
#include <QSettings>
#include <QSize>
#include <QStatusBar>
#include <QTextStream>
#include <QToolBar>
#include <QCheckBox>
#include <QLabel>
#include <Qsci/qsciapis.h>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerpython.h>
#include "core/plugin-interfaces/ncswidgetplugin.h"
#include "gui/utilities/qwidgetvector.h"

class PythonEditor : public NCSWidgetPlugin
{
    Q_OBJECT
public:
    explicit PythonEditor(QWidget *parent = 0);
    void openPortal(NCSProjectPortal project) ;
    QIcon icon() ;
    QString title() ;
    QString name();
    float version();
    void closePortal();
public slots:
    void initialize() ;
    void cleanup() ;

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void m_textChanged(bool changed);

private:
    QToolButton * m_createButton( QString title, QIcon icon, QKeySequence seq = QKeySequence(),QString statusTip = "");

    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);

    QString m_unsavedChanges;
    NCSProjectPortal m_portal;
    QDomElement m_lastFileElement;
    QWidgetVector * m_homeButtonMenu;
    QsciScintilla *m_textEdit;
    QString m_projectDir;
    QToolBar *m_fileToolBar;
    QToolButton *m_newButton, *m_openButton, *m_saveButton, *m_saveAsButton;
    QToolButton *m_cutButton, *m_copyButton, *m_pasteButton;
    QsciLexerPython * m_lexer;
    QsciAPIs  * m_api;
    QVBoxLayout * m_layout;
    QStatusBar * m_statusBar;
    QWidget * m_spacer;
    QLabel * m_fileLabel;
};

#endif // PYTHONEDITOR_H
