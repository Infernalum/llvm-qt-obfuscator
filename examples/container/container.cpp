#include "container.h"

Structure::Structure(QObject *parent) : QObject(parent)
{

}

QString Structure::getFullInfo() const
{
    return QString("Full information from Structure %1").arg(m_number);
}

Container::Container(QObject *parent) : QObject(parent)
{

}

QString Container::getFullInfo() const
{
    return QString("Full information from Container %1").arg(m_number);
}

int Container::number() const
{
    return m_number;
}

QString Container::message() const
{
    return m_message;
}

void Container::setNumber(int number)
{
    if (m_number == number)
        return;

    m_number = number;
    emit numberChanged(m_number);
}

void Container::setMessage(QString message)
{
    if (m_message == message)
        return;

    m_message = message;
    emit messageChanged(m_message);
}
