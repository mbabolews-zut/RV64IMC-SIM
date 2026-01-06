set_source_files_properties(Constants.qml
    PROPERTIES
        QT_QML_SINGLETON_TYPE true
)

if(WIN32 AND CMAKE_SOURCE_DIR MATCHES " ")
    message(WARNING
            "\n=========================== WARNING ===========================\n"
            "The source directory path contains spaces:\n"
            "  ${CMAKE_SOURCE_DIR}\n\n"
            "Qt6 QML is known to fail on Windows when paths contain spaces.\n"
            "Please move the project to a path without spaces."
            "\n===============================================================\n"
    )
endif()



qt_add_library(RISC_VISIM_APP STATIC)
qt6_add_qml_module(RISC_VISIM_APP
    URI "RISC_VISIM_APP"
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    QML_FILES
        "Constants.qml"
)


# Hide Qt-generated internal targets from Visual Studio solution
set_target_properties(RISC_VISIM_APP PROPERTIES FOLDER "QML Modules")
if(TARGET RISC_VISIM_APPplugin)
    set_target_properties(RISC_VISIM_APPplugin PROPERTIES FOLDER "QML Modules/Internal")
endif()
if(TARGET RISC_VISIM_APPplugin_init)
    set_target_properties(RISC_VISIM_APPplugin_init PROPERTIES FOLDER "QML Modules/Internal")
endif()
if(TARGET RISC_VISIM_APP_resources_1)
    set_target_properties(RISC_VISIM_APP_resources_1 PROPERTIES FOLDER "QML Modules/Internal")
endif()
if(TARGET RISC_VISIM_APP_resources_2)
    set_target_properties(RISC_VISIM_APP_resources_2 PROPERTIES FOLDER "QML Modules/Internal")
endif()



