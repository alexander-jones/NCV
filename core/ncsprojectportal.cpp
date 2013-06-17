#include "ncsprojectportal.h"

NCSProjectPortal::NCSProjectPortal(){}
NCSProjectPortal::NCSProjectPortal(QDomElement element,QDomDocument doc, QString parentDir )
{
    m_document = doc;
    m_element = element;
    m_parentDirectory = parentDir;

}

QDomElement NCSProjectPortal::rootElement()
{
    return m_element;
}

QDomElement NCSProjectPortal::newElement(QString tagName)
{
    return m_document.createElement(tagName);
}

QDomText NCSProjectPortal::newTextElement(QString value)
{
    return m_document.createTextNode(value);
}
QString NCSProjectPortal::parentDirectory()
{
    return m_parentDirectory;
}
