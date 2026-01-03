import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

SettingsPage {
    id: root
    title: "MEMORY SETTINGS"

    readonly property color cMuted: "#5C6580"

    property alias stackSizeKB: stackSizeSpinBox.value

    RowLayout {
        spacing: 15

        Label { text: "Stack Size"; color: root.cMuted }

        SettingsSpinBox {
            id: stackSizeSpinBox
            from: 16
            to: 4096
            value: 64
        }

        Label { text: "KB"; color: root.cMuted }
    }
}
