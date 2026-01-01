// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Backend.hpp"
#include <ui.hpp>

#include "autogen/environment.h"

int main(int argc, char *argv[])
{
    set_qt_environment();
    QApplication app(argc, argv);

    // Set up backend and UI callbacks
    Backend backend;
    ui::set_output_callback([&](auto sv) {
        backend.appendOutput(QString(sv.data()));
    });
    ui::set_error_msg_callback([&](auto sv) {
        backend.appendError(QString(sv.data()));
    });

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("backend", &backend);
    engine.rootContext()->setContextProperty("registerModel", backend.registerModel());
    engine.rootContext()->setContextProperty("memoryController", backend.memoryController());

    const QUrl url(mainQmlFile);
    QObject::connect(
                &engine, &QQmlApplicationEngine::objectCreated, &app,
                [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.addImportPath(QCoreApplication::applicationDirPath() + "/qml");
    engine.addImportPath(":/");
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
