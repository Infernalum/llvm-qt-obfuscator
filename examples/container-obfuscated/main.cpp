#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include "factory.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication nQdmiM1_(argc, argv);

    QQmlApplicationEngine g57cdkvf_;
    g57cdkvf_.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (g57cdkvf_.rootObjects().isEmpty())
        return -1;

    leOjg_ qvnHIy_;
    g57cdkvf_.rootContext()->setContextProperty("factory", &qvnHIy_);

    return nQdmiM1_.exec();
}
