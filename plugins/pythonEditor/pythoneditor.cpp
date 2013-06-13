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

    m_fileToolBar = new QToolBar(tr("File"));

    m_newButton = new QToolButton();
    m_newButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_newButton->setText("New");
    m_newButton->setIcon(QIcon(":/resources/images/new.png"));
    m_newButton->setShortcut(tr("Ctrl+N"));
    m_newButton->setStatusTip(tr("Create a new file"));
    connect(m_newButton, SIGNAL(clicked()), this, SLOT(newFile()));
    m_fileToolBar->addWidget(m_newButton);


    m_openButton = new QToolButton();
    m_openButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_openButton->setText("Open");
    m_openButton->setIcon(QIcon(":/resources/images/open.png"));
    m_openButton->setShortcut(tr("Ctrl+O"));
    m_openButton->setStatusTip(tr("Open an existing file"));
    connect(m_openButton, SIGNAL(clicked()), this, SLOT(open()));
    m_fileToolBar->addWidget(m_openButton);

    m_saveButton = new QToolButton();
    m_saveButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_saveButton->setText("Save");
    m_saveButton->setIcon(QIcon(":/resources/images/save.png"));
    m_saveButton->setShortcut(tr("Ctrl+S"));
    m_saveButton->setStatusTip(tr("Save the script to disk"));
    connect(m_saveButton, SIGNAL(clicked()), this, SLOT(save()));
    m_fileToolBar->addWidget(m_saveButton);

    m_saveAsButton = new QToolButton();
    m_saveAsButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_saveAsButton->setText("Save As");
    m_saveAsButton->setIcon(QIcon(":/resources/images/save.png"));
    m_saveAsButton->setStatusTip(tr("Save the document under a new name"));
    connect(m_saveAsButton, SIGNAL(clicked()), this, SLOT(saveAs()));
    m_fileToolBar->addWidget(m_saveAsButton);


    m_cutButton = new QToolButton();
    m_cutButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_cutButton->setText("Cut");
    m_cutButton->setIcon(QIcon(":/resources/images/cut.png"));
    m_cutButton->setShortcut(tr("Ctrl+X"));
    m_cutButton->setStatusTip(tr("Cut the current selection's contents to the "
                                 "clipboard"));
    connect(m_cutButton, SIGNAL(clicked()), m_textEdit, SLOT(cut()));
    m_fileToolBar->addWidget(m_cutButton);

    m_copyButton = new QToolButton();
    m_copyButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_copyButton->setText("Copy");
    m_copyButton->setIcon(QIcon(":/resources/images/copy.png"));
    m_copyButton->setShortcut(tr("Ctrl+C"));
    m_copyButton->setStatusTip(tr("Copy the current selection's contents to the "
                                 "clipboard"));
    connect(m_copyButton, SIGNAL(clicked()), m_textEdit, SLOT(copy()));
    m_fileToolBar->addWidget(m_copyButton);

    m_pasteButton = new QToolButton();
    m_pasteButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    m_pasteButton->setText("Paste");
    m_pasteButton->setIcon(QIcon(":/resources/images/paste.png"));
    m_pasteButton->setShortcut(tr("Ctrl+V"));
    m_pasteButton->setStatusTip(tr("Paste the clipboard's contents into the current "
                                   "selection"));
    connect(m_pasteButton, SIGNAL(clicked()), m_textEdit, SLOT(paste()));
    m_fileToolBar->addWidget(m_pasteButton);

    m_cutButton->setEnabled(false);
    m_copyButton->setEnabled(false);
    connect(m_textEdit, SIGNAL(copyAvailable(bool)),
            m_cutButton, SLOT(setEnabled(bool)));
    connect(m_textEdit, SIGNAL(copyAvailable(bool)),
            m_copyButton, SLOT(setEnabled(bool)));

    m_layout->addWidget(m_fileToolBar);

    m_layout->addWidget(m_textEdit);

    m_statusBar = new QStatusBar();
    m_layout->addWidget(m_statusBar);
    m_statusBar->showMessage(tr("Ready"));
    m_layout->addWidget(m_statusBar);

    m_lexer = new QsciLexerPython(this);
    m_lexer->setIndentationWarning(QsciLexerPython::Inconsistent);
    m_lexer->setFoldCompact(true);
    m_lexer->setFoldComments(true);
    m_lexer->setFoldQuotes(true);

    m_api = new QsciAPIs(m_lexer);

    m_api->load(":/resources/apis/ncspython.api");
    m_api->prepare();

    m_textEdit->setLexer(m_lexer);

    m_textEdit->setAutoCompletionThreshold(1);
    m_textEdit->setAutoCompletionSource(QsciScintilla::AcsAPIs);

    this->setLayout(m_layout);
    setCurrentFile("");
}


void PythonEditor::loadProject(QString projectDir)
{
    m_projectDir = projectDir;
}

QIcon PythonEditor::icon()
{
    return QIcon(":/resources/images/pythonEditor.png");
}
QString PythonEditor::title()
{
    return "Python Editor";
}

QString PythonEditor::name()
{
    return "pyhon-editor";
}

float PythonEditor::version()
{
    return 1.0;
}

void PythonEditor::initialize()
{
}
void PythonEditor::cleanup()
{
}

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
        QString fileName = QFileDialog::getOpenFileName(this,"Open Python Script",m_projectDir,tr("Python script (*.py)"));
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

    QString shownName;
    if (m_curFile.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = strippedName(m_curFile);

}

QString PythonEditor::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
