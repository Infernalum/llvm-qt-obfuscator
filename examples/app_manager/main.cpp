#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <appmanager.h>


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    // exposing C++ object to Qml
    AppManager *appManager = new AppManager(&app);
    engine.rootContext()->setContextProperty("appManager", appManager);
    const QUrl url(u"qrc:/testapp/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
    return app.exec();
}
