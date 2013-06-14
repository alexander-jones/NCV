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

    QFontMetrics fontmetrics = QFontMetrics(this->font());
    m_textEdit->setMarginsFont(this->font());
    m_textEdit->setMarginWidth(0,fontmetrics.width("00000") );
    m_textEdit->setMarginsBackgroundColor(QColor("#e8e5e5"));
    m_textEdit->setMarginsForegroundColor(QColor("#dd4814"));
    m_textEdit->setMarginLineNumbers(0,true);

    m_fileToolBar = new QToolBar(tr("File"));

    m_newButton = m_createButton("New",QIcon(":/resources/images/new.png"),tr("Ctrl+N"),tr("Open an new file."));
    connect(m_newButton, SIGNAL(clicked()), this, SLOT(newFile()));
    m_fileToolBar->addWidget(m_newButton);


    m_openButton = m_createButton("Open",QIcon(":/resources/images/open.png"),tr("Ctrl+O"),tr("Open an existing file."));
    connect(m_openButton, SIGNAL(clicked()), this, SLOT(open()));
    m_fileToolBar->addWidget(m_openButton);

    m_saveButton = m_createButton("Save",QIcon(":/resources/images/save.png"),tr("Ctrl+S"),tr("Save the file."));
    connect(m_saveButton, SIGNAL(clicked()), this, SLOT(save()));
    m_fileToolBar->addWidget(m_saveButton);

    m_saveAsButton = m_createButton("Save As",QIcon(":/resources/images/save.png"),tr("Ctrl+Shift+S"),tr("Save the file under a new name."));
    connect(m_saveAsButton, SIGNAL(clicked()), this, SLOT(saveAs()));
    m_fileToolBar->addWidget(m_saveAsButton);

    m_cutButton = m_createButton("Cut",QIcon(":/resources/images/cut.png"),tr("Ctrl+X"),tr("Cut the selection to clipboard."));
    m_cutButton->setEnabled(false);
    connect(m_cutButton, SIGNAL(clicked()), m_textEdit, SLOT(cut()));
    connect(m_textEdit, SIGNAL(copyAvailable(bool)), m_cutButton, SLOT(setEnabled(bool)));
    m_fileToolBar->addWidget(m_cutButton);

    m_copyButton = m_createButton("Copy",QIcon(":/resources/images/copy.png"),tr("Ctrl+C"),tr("Copy the selection to clipboard."));
    m_copyButton->setEnabled(false);
    connect(m_copyButton, SIGNAL(clicked()), m_textEdit, SLOT(copy()));
    connect(m_textEdit, SIGNAL(copyAvailable(bool)), m_copyButton, SLOT(setEnabled(bool)));
    m_fileToolBar->addWidget(m_copyButton);

    m_pasteButton = m_createButton("Paste",QIcon(":/resources/images/paste.png"),tr("Ctrl+V"),tr("Paste the clipboard's contents."));
    connect(m_pasteButton, SIGNAL(clicked()), m_textEdit, SLOT(paste()));
    m_fileToolBar->addWidget(m_pasteButton);


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

    m_api->load(":/resources/apis/builtin.api");
    m_api->load(":/resources/apis/keywords.api");
    m_api->prepare();

    m_textEdit->setLexer(m_lexer);

    m_textEdit->setAutoCompletionThreshold(1);
    m_textEdit->setAutoCompletionSource(QsciScintilla::AcsAPIs);

    this->setLayout(m_layout);
    setCurrentFile("");
}

QToolButton * PythonEditor::m_createButton(QString text, QIcon icon, QKeySequence seq, QString statusTip)
{
    QToolButton* button = new QToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setText(text);
    button->setIcon(icon);
    button->setShortcut(seq);
    button->setStatusTip(statusTip);
    return button;
}

void PythonEditor::loadProject(NCSProjectPortal project)
{
    m_projectDir = project.parentDirectory();
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
