#include <appmanager.h>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[]) {
    QGuiApplication x1t_(argc, argv);
    QQmlApplicationEngine uccNq_;
    // exposing C++ object to Qml
    i1IiJ4E_ *G2JYwkDg_ = new i1IiJ4E_(&x1t_);
    uccNq_.rootContext()->setContextProperty("appManager", G2JYwkDg_);
    const QUrl E0Im_(u"qrc:/testapp/main.qml"_qs);
    QObject::connect(
        &uccNq_, &QQmlApplicationEngine::objectCreated, &x1t_,
        [E0Im_](QObject *Dz4Kk_, const QUrl &wf_Zl_) {
            if (!Dz4Kk_ && E0Im_ == wf_Zl_) QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection
    );
    uccNq_.load(E0Im_);
    return x1t_.exec();
}
