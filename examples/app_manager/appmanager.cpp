#include "appmanager.h"

#include <QTimer>
#include <QRandomGenerator>

void AppManager::performOperation()
{
    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, [this]() {
        const int result = QRandomGenerator::global()->generate();
        const QString &operationResult = result % 2 == 0
                ? "success"
                : "error";
        emit operationFinished(operationResult);
    });
    timer->start(5000);
}
