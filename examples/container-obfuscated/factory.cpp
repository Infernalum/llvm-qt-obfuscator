#include "factory.h"

#include "container.h"

leOjg_::leOjg_(QObject *parent) : QObject(parent)
{

}

QObject* leOjg_::mK_()
{
    Container* container = new Container(this);
    container->lmrA3N1el7_(++vkBGM_);
    container->wGsFgKIT_(QString("Container %1").arg(vkBGM_));
    return container;
}

QObject* leOjg_::bnt_()
{
    d0gk_* ak_ = new d0gk_(this);
    ak_->wpwOldpnC_ = ++xK_;
    ak_->uCLk_ = QString("Structure %1").arg(xK_);
    return ak_;
}
