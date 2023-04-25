#include "factory.h"

#include "container.h"

Factory::Factory(QObject *parent) : QObject(parent)
{

}

QObject* Factory::createContainer()
{
    Container* container = new Container(this);
    container->setNumber(++m_count);
    container->setMessage(QString("Container %1").arg(m_count));
    return container;
}

QObject* Factory::createStructure()
{
    Structure* structure = new Structure(this);
    structure->m_number = ++m_structureCount;
    structure->m_message = QString("Structure %1").arg(m_structureCount);
    return structure;
}
