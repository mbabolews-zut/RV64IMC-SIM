set_source_files_properties(Constants.qml
    PROPERTIES
        QT_QML_SINGLETON_TYPE true
)



qt_add_library(RISC_VISIM_APP STATIC)
qt6_add_qml_module(RISC_VISIM_APP
    URI "RISC_VISIM_APP"
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    QML_FILES
        "Constants.qml"
)


