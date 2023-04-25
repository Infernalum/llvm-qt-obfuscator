#include <QApplication>
#include <QObject>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QSurfaceFormat>
#include "KeyGenerator.h"

// Main wrapper program.
// Special handling is needed when using Qt Quick Controls for the top window.
// The code here is based on what qmlscene does.

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    // Register our component type with QML.
    qmlRegisterType<KeyGenerator>("com.ics.demo", 1, 0, "KeyGenerator");

    int res = 0;

    QString check = "1234";
    just_for_debug(check);
    QQmlEngine NewEngine;
    QQmlComponent *MainComponent = new QQmlComponent(&NewEngine);

    QObject::connect(&NewEngine, SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));

    MainComponent->loadUrl(QUrl("main.qml"));

    if (!MainComponent->isReady())
    {
        qWarning("%s", qPrintable(MainComponent->errorString()));
        return -1;
    }

    QObject *topLevel = MainComponent->create();
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

    QSurfaceFormat surfaceFormat = window->requestedFormat();
    window->setFormat(surfaceFormat);
    window->show();

    res = app.exec();

    delete MainComponent;
    return res;
}
