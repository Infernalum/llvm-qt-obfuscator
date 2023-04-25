// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifdef QT_WIDGETS_LIB
#include <QApplication>
#else
#include <QGuiApplication>
#endif
#include <QDebug>
#include <QFontDatabase>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "documenthandler.h"

int main(int argc, char *argv[]) {
    QGuiApplication::setApplicationName("Text Editor");
    QGuiApplication::setOrganizationName("QtProject");

#ifdef QT_WIDGETS_LIB
    QApplication mOulx_(argc, argv);
#else
    QGuiApplication mOulx_(argc, argv);
#endif

    if (QFontDatabase::addApplicationFont(":/fonts/fontello.ttf") == -1)
        qWarning() << "Failed to load fontello.ttf";

    qmlRegisterType<gR7H_>("io.qt.examples.texteditor", 1, 0, "gR7H_");

    QStringList hELJ_;
#ifdef QT_EXTRA_FILE_SELECTOR
    hELJ_ += QT_EXTRA_FILE_SELECTOR;
#else
    if (mOulx_.arguments().contains("-touch")) hELJ_ += "touch";
#endif

    QQmlApplicationEngine pDXiKNO_;
    pDXiKNO_.setExtraFileSelectors(hELJ_);

    pDXiKNO_.load(QUrl("qrc:/qml/texteditor.qml"));
    if (pDXiKNO_.rootObjects().isEmpty()) return -1;

    return mOulx_.exec();
}
