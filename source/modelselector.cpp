#include "modelselector.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

ModelSelector::ModelSelector(QWidget *parent) :
    QWidget(parent)
{
    m_layout = new QVBoxLayout();

    m_layout->setAlignment(Qt::AlignCenter);
    m_modelLabel = new QLabel("Model to Simulate: No Model Selected");
    m_modelLabel->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_modelLabel);

    m_recentModelsLabel = new QLabel("Recent Models:");
    m_recentModelsLabel->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_recentModelsLabel);

    m_recentModels = new QListWidget();
    QStringList allRecentModels;
    QFile recentModels("./user/recentModelFiles.txt");

    if (!recentModels.exists())
    {
        QFile file("./user/recentModelFiles.txt");
        file.open(QIODevice::ReadWrite);
        file.close();
    }
    else
    {
        recentModels.open(QIODevice::ReadOnly);
        allRecentModels = QString(recentModels.readAll()).split(" ");
        recentModels.close();
    }

    QFile::resize("./user/recentModelFiles.txt",0);
    recentModels.open(QIODevice::WriteOnly);
    for (int i = 0; i < allRecentModels.size(); i ++)
    {
        if (QFile(allRecentModels[i]).exists())
        {
            recentModels.write(QString(allRecentModels[i] + " ").toAscii());
            m_recentModels->addItem(new QListWidgetItem(QString(allRecentModels[i])));
        }
    }
    recentModels.close();
    connect(m_recentModels,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(m_loadModel(QListWidgetItem*)));
    m_layout->addWidget(m_recentModels);

    m_loadModelButton = new QPushButton("Browse for Model");
    connect(m_loadModelButton,SIGNAL(clicked()),this,SLOT(m_loadModel()));
    m_layout->addWidget(m_loadModelButton);

    this->setLayout(m_layout);

}


void ModelSelector::m_loadModel(QListWidgetItem* item)
{
    QString fileName = item->text().mid(0,item->text().size());
    loadModel(fileName);
}

void  ModelSelector::m_loadModel()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Load NCS Model"));
    if (loadModel(fileName))
        m_addToRecentModels(fileName);
}

void ModelSelector::m_addToRecentModels(QString fileName)
{

    for (int i = 0; i < m_recentModels->count(); i ++)
    {
        if(m_recentModels->item(i)->text() == fileName)
            return;
    }

    QFile recentModels("./user/recentModelFiles.txt");
    if (!recentModels.open(QIODevice::Append))
        return;
    recentModels.write((fileName + " ").toAscii());
    recentModels.close();

    m_recentModels->addItem(new QListWidgetItem(fileName));
}

bool  ModelSelector::loadModel(QString filename)
{
    if (filename == "")
        return false;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray bytes = file.readAll();

    bool isModelFile = true;
    if (!isModelFile)
    {
        QMessageBox msgBox;
        msgBox.addButton(tr("Ok"), QMessageBox::ActionRole);
        msgBox.setText("The file you selected is not a ncs model file. Select a file saved via the brain builder tab.");
        msgBox.exec();
        return false;
    }

    m_modelLabel->setText("Model to Simulate: "+  filename);
    modelLoaded(filename);
    return true;
}
