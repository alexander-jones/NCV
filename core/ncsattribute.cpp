#include "ncsattribute.h"

NCSAttribute::NCSAttribute(QObject *parent) :
    QObject(parent)
{
    m_reportable = true;
}
bool NCSAttribute::reportable() const
{
    return m_reportable;
}
void NCSAttribute::setReportable(bool value)
{
    m_reportable = value;
}
