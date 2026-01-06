qt_add_library(RISC_VISIM_APPContent STATIC)
qt6_add_qml_module(RISC_VISIM_APPContent
    URI "RISC_VISIM_APPContent"
    VERSION 1.0
    RESOURCE_PREFIX "/qt/qml"
    QML_FILES
        "App.qml"
        "DataTypesPanel.qml"
        "FormatToggle.qml"
        "MemoryGrid.qml"
        "NavButton.qml"
        "OutputPanel.qml"
        "RegisterPanel.qml"
        "RegisterRow.qml"
        "RegisterTableHeader.qml"
        "Screen01.ui.qml"
        "SettingsDialog.qml"
        "TextEditor.qml"
        "Toolbar.qml"
        # Settings components
        "SettingsTextField.qml"
        "SettingsSpinBox.qml"
        "SettingsComboBox.qml"
        "SettingsPage.qml"
        "EditorSettingsPage.qml"
        "MemorySettingsPage.qml"
        "SystemSettingsPage.qml"
)

# Hide Qt-generated internal targets from Visual Studio solution
set_target_properties(RISC_VISIM_APPContent PROPERTIES FOLDER "QML Modules")
if(TARGET RISC_VISIM_APPContentplugin)
    set_target_properties(RISC_VISIM_APPContentplugin PROPERTIES FOLDER "QML Modules/Internal")
endif()
if(TARGET RISC_VISIM_APPContentplugin_init)
    set_target_properties(RISC_VISIM_APPContentplugin_init PROPERTIES FOLDER "QML Modules/Internal")
endif()
if(TARGET RISC_VISIM_APPContent_resources_1)
    set_target_properties(RISC_VISIM_APPContent_resources_1 PROPERTIES FOLDER "QML Modules/Internal")
endif()
if(TARGET RISC_VISIM_APPContent_resources_2)
    set_target_properties(RISC_VISIM_APPContent_resources_2 PROPERTIES FOLDER "QML Modules/Internal")
endif()

