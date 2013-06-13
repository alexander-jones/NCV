#include "ncsprojectportal.h"

NCSProjectPortal::NCSProjectPortal(QDomElement * element, QString parentDir )
{
    m_element = element;
    m_parentDirectory = parentDir;
}

QDomElement * NCSProjectPortal::element()
{
    return m_element;
}

QString NCSProjectPortal::parentDirectory()
{
    return m_parentDirectory;
}
