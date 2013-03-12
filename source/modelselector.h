#ifndef MODELSELECTOR_H
#define MODELSELECTOR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QListWidget>

class ModelSelector : public QWidget
{
    Q_OBJECT
public:
    explicit ModelSelector(QWidget *parent = 0);

signals:
    void modelLoaded(QString filename);
public slots:
    bool loadModel(QString filename);

private slots:
    void m_loadModel(QListWidgetItem* item);
    void m_loadModel();


private:
    void m_addToRecentModels(QString fileName);
    QLabel * m_modelLabel,* m_recentModelsLabel;
    QListWidget *m_recentModels ;
    QPushButton * m_loadModelButton;
    QVBoxLayout * m_layout;
    
};

#endif // MODELSELECTOR_H
