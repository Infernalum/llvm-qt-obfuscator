#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include "factory.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    Factory factory;
    engine.rootContext()->setContextProperty("factory", &factory);

    return app.exec();
}
