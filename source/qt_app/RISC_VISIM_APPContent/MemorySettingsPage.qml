import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

SettingsPage {
    id: root
    title: "MEMORY SETTINGS"

    readonly property color cMuted: "#5C6580"

    GridLayout {
        columns: 3
        rowSpacing: 18
        columnSpacing: 15

        Label { text: "Stack Size"; color: root.cMuted }
        SettingsSpinBox {
            id: stackSizeSpinBox
            from: 16
            to: 4096
            value: settingsManager.stackSizeKiB
            onValueModified: settingsManager.stackSizeKiB = value
        }
        Label { text: "KiB"; color: root.cMuted }

        Label { text: "Data Address"; color: root.cMuted }
        SettingsTextField {
            id: dataAddrField
            text: settingsManager.dataAddress
            onTextEdited: settingsManager.dataAddress = text
        }
        Item { width: 1 }

        Label { text: "Stack Address"; color: root.cMuted }
        SettingsTextField {
            id: stackAddrField
            text: settingsManager.stackAddress
            onTextEdited: settingsManager.stackAddress = text
        }
        Item { width: 1 }
    }
}
