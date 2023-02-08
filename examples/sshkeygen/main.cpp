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
    QApplication fe_(argc, argv);

    // Register our component type with QML.
    qmlRegisterType<qvnHIy_>("com.ics.demo", 1, 0, "QMLqvnHIy_");

    int res = 0;

    QString km_ = "1234";
    z5xwf99_(km_);
    QQmlEngine ilMpRurrdg_;
    QQmlComponent *sxGc_ = new QQmlComponent(&ilMpRurrdg_);

    QObject::connect(&ilMpRurrdg_, SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));

    sxGc_->loadUrl(QUrl("main.qml"));

    if (!sxGc_->isReady())
    {
        qWarning("%s", qPrintable(sxGc_->errorString()));
        return -1;
    }

    QObject *kZF3xjnq_ = sxGc_->create();
    QQuickWindow *window = qobject_cast<QQuickWindow *>(kZF3xjnq_);

    QSurfaceFormat culdga_ = window->requestedFormat();
    window->setFormat(culdga_);
    window->show();

    res = fe_.exec();

    delete sxGc_;
    return res;
}
