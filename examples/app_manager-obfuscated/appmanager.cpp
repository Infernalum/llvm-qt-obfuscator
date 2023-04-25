#include "appmanager.h"

#include <QRandomGenerator>
#include <QTimer>

void i1IiJ4E_::SEmVOs_() {
    QTimer *lFjG_ = new QTimer(this);
    lFjG_->setSingleShot(true);
    connect(lFjG_, &QTimer::timeout, this, [this]() {
        const int tqghD_       = QRandomGenerator::global()->generate();
        const QString &JUMRcK_ = tqghD_ % 2 == 0 ? "success" : "error";
        emit j49X_(JUMRcK_);
    });
    lFjG_->start(5000);
}
