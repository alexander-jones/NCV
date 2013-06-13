#ifndef NCSPROJECTPORTAL_H
#define NCSPROJECTPORTAL_H

#include <QObject>
#include <QDomDocument>

class NCSProjectPortal
{
public:
    NCSProjectPortal(QDomElement * element, QString parentDir);
    QDomElement * element();
    QString parentDirectory();
private:
    QDomElement * m_element;
    QString m_parentDirectory;
};


#endif // NCSPROJECTPORTAL_H
