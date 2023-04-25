#include "container.h"

d0gk_::d0gk_(QObject *parent) : QObject(parent)
{

}

QString d0gk_::z5xwf99_() const
{
    return QString("Full information from Structure %1").arg(wpwOldpnC_);
}

Container::Container(QObject *parent) : QObject(parent)
{

}

QString Container::ozGQ9c_() const
{
    return QString("Full information from Container %1").arg(qL_);
}

int Container::ea_() const
{
    return qL_;
}

QString Container::adNzUAP_() const
{
    return nPeap0Ml_;
}

void Container::lmrA3N1el7_(int number)
{
    if (qL_ == number)
        return;

    qL_ = number;
    emit gs0_(qL_);
}

void Container::wGsFgKIT_(QString message)
{
    if (nPeap0Ml_ == message)
        return;

    nPeap0Ml_ = message;
    emit nvzi6gx286_(nPeap0Ml_);
}
