#include <QAction>
#include <QVBoxLayout>
#include <QToolBar>
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

#include <Qsci/qsciapis.h>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerpython.h>
#include "pythoneditor.h"

PythonEditor::PythonEditor(QWidget * parent)
    :NCSWidgetPlugin(parent)
{

    m_layout = new QVBoxLayout();

    m_textEdit = new QsciScintilla(this);
    m_textEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    createActions();
    createMenus();
    createToolBars();

    m_layout->addWidget(m_textEdit);

    createStatusBar();

    m_lexer = new QsciLexerPython(this);

    m_api = new QsciAPIs(m_lexer);

    m_api->load(":/resources/apis/ncspython.api");
    m_api->prepare();

    m_textEdit->setLexer(m_lexer);

    m_textEdit->setAutoCompletionThreshold(1);
    m_textEdit->setAutoCompletionSource(QsciScintilla::AcsAPIs);


    this->setLayout(m_layout);
    readSettings();

    connect(m_textEdit, SIGNAL(textChanged()),
            this, SLOT(documentWasModified()));

    setCurrentFile("");
}


void PythonEditor::loadProject(QString projectDir)
{
}

QIcon PythonEditor::icon()
{
    return QIcon(":/resources/images/pythonEditor.png");
}
QString PythonEditor::title()
{
    return "Python Editor";
}

void PythonEditor::initialize()
{
}
void PythonEditor::cleanup()
{
}

/*void PythonEditor::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}*/

void PythonEditor::newFile()
{
    if (maybeSave()) {
        m_textEdit->clear();
        setCurrentFile("");
    }
}

void PythonEditor::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool PythonEditor::save()
{
    if (m_curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(m_curFile);
    }
}

bool PythonEditor::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void PythonEditor::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt, with a menu bar, "
               "toolbars, and a status bar."));
}

void PythonEditor::documentWasModified()
{
    setWindowModified(m_textEdit->isModified());
}

void PythonEditor::createActions()
{
    m_newAct = new QAction(QIcon(":/resources/images/new.png"), tr("&New"), this);
    m_newAct->setShortcut(tr("Ctrl+N"));
    m_newAct->setStatusTip(tr("Create a new file"));
    connect(m_newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    m_openAct = new QAction(QIcon(":/resources/images/open.png"), tr("&Open..."), this);
    m_openAct->setShortcut(tr("Ctrl+O"));
    m_openAct->setStatusTip(tr("Open an existing file"));
    connect(m_openAct, SIGNAL(triggered()), this, SLOT(open()));

    m_saveAct = new QAction(QIcon(":/resources/images/save.png"), tr("&Save"), this);
    m_saveAct->setShortcut(tr("Ctrl+S"));
    m_saveAct->setStatusTip(tr("Save the document to disk"));
    connect(m_saveAct, SIGNAL(triggered()), this, SLOT(save()));

    m_saveAsAct = new QAction(tr("Save &As..."), this);
    m_saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(m_saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    m_exitAct = new QAction(tr("E&xit"), this);
    m_exitAct->setShortcut(tr("Ctrl+Q"));
    m_exitAct->setStatusTip(tr("Exit the application"));
    connect(m_exitAct, SIGNAL(triggered()), this, SLOT(close()));

    m_cutAct = new QAction(QIcon(":/resources/images/cut.png"), tr("Cu&t"), this);
    m_cutAct->setShortcut(tr("Ctrl+X"));
    m_cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(m_cutAct, SIGNAL(triggered()), m_textEdit, SLOT(cut()));

    m_copyAct = new QAction(QIcon(":/resources/images/copy.png"), tr("&Copy"), this);
    m_copyAct->setShortcut(tr("Ctrl+C"));
    m_copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(m_copyAct, SIGNAL(triggered()), m_textEdit, SLOT(copy()));

    m_pasteAct = new QAction(QIcon(":/resources/images/paste.png"), tr("&Paste"), this);
    m_pasteAct->setShortcut(tr("Ctrl+V"));
    m_pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(m_pasteAct, SIGNAL(triggered()), m_textEdit, SLOT(paste()));

    m_aboutAct = new QAction(tr("&About"), this);
    m_aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(m_aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    m_aboutQtAct = new QAction(tr("About &Qt"), this);
    m_aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(m_aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    m_cutAct->setEnabled(false);
    m_copyAct->setEnabled(false);
    connect(m_textEdit, SIGNAL(copyAvailable(bool)),
            m_cutAct, SLOT(setEnabled(bool)));
    connect(m_textEdit, SIGNAL(copyAvailable(bool)),
            m_copyAct, SLOT(setEnabled(bool)));
}

void PythonEditor::createMenus()
{

    m_menuBar = new QMenuBar();
    m_fileMenu = m_menuBar->addMenu(tr("&File"));
    m_fileMenu->addAction(m_newAct);
    m_fileMenu->addAction(m_openAct);
    m_fileMenu->addAction(m_saveAct);
    m_fileMenu->addAction(m_saveAsAct);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAct);

    m_editMenu = m_menuBar->addMenu(tr("&Edit"));
    m_editMenu->addAction(m_cutAct);
    m_editMenu->addAction(m_copyAct);
    m_editMenu->addAction(m_pasteAct);

    m_menuBar->addSeparator();

    m_helpMenu = m_menuBar->addMenu(tr("&Help"));
    m_helpMenu->addAction(m_aboutAct);
    m_helpMenu->addAction(m_aboutQtAct);
    m_layout->addWidget(m_menuBar);
}

void PythonEditor::createToolBars()
{
    m_fileToolBar = new QToolBar(tr("File"));
    m_fileToolBar->addAction(m_newAct);
    m_fileToolBar->addAction(m_openAct);
    m_fileToolBar->addAction(m_saveAct);
    m_fileToolBar->addAction(m_cutAct);
    m_fileToolBar->addAction(m_copyAct);
    m_fileToolBar->addAction(m_pasteAct);
    m_layout->addWidget(m_fileToolBar);
}

void PythonEditor::createStatusBar()
{
    m_statusBar = new QStatusBar();
    m_layout->addWidget(m_statusBar);
    m_statusBar->showMessage(tr("Ready"));
    m_layout->addWidget(m_statusBar);
}

void PythonEditor::readSettings()
{
    QSettings settings("Trolltech", "Application Example");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void PythonEditor::writeSettings()
{
    QSettings settings("Trolltech", "Application Example");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool PythonEditor::maybeSave()
{
    if (m_textEdit->isModified()) {
        int ret = QMessageBox::warning(this, tr("Application"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No,
                     QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void PythonEditor::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_textEdit->setText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    m_statusBar->showMessage(tr("File loaded"), 2000);
}

bool PythonEditor::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << m_textEdit->text();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    m_statusBar->showMessage(tr("File saved"), 2000);
    return true;
}

void PythonEditor::setCurrentFile(const QString &fileName)
{
    m_curFile = fileName;
    m_textEdit->setModified(false);
    setWindowModified(false);

    QString shownName;
    if (m_curFile.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = strippedName(m_curFile);

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Application")));
}

QString PythonEditor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
