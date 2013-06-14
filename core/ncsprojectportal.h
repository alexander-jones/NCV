#ifndef NCSPROJECTPORTAL_H
#define NCSPROJECTPORTAL_H

#include <QObject>
#include <QDomDocument>

class NCSProjectPortal
{
public:
    NCSProjectPortal(QDomElement element, QDomDocument doc,QString parentDir);
    QDomElement rootElement();
    QDomElement newElement(QString tagName);
    QString parentDirectory();
private:
    QDomDocument m_document;
    QDomElement m_element;
    QString m_parentDirectory;
};


#endif // NCSPROJECTPORTAL_H
