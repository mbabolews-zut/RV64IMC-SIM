#ifndef APP_ENVIRONMENT_H
#define APP_ENVIRONMENT_H

#include <QGuiApplication>
#include <qqmlextensionplugin.h>

Q_IMPORT_QML_PLUGIN(RISC_VISIM_APPPlugin)
Q_IMPORT_QML_PLUGIN(RISC_VISIM_APPContentPlugin)

const char mainQmlFile[] = "qrc:/qt/qml/RISC_VISIM_APPContent/App.qml";

inline void set_qt_environment() {
    qputenv("QML_COMPAT_RESOLVE_URLS_ON_ASSIGNMENT", "1");
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");
    qputenv("QT_LOGGING_RULES", "qt.qml.connections=false");
    qputenv("QT_QUICK_CONTROLS_CONF", ":/qtquickcontrols2.conf");
}

#endif // APP_ENVIRONMENT_H
