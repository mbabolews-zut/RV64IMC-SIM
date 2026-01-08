import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

SettingsPage {
    id: root
    title: "EDITOR SETTINGS"

    readonly property color cMuted: "#5C6580"

    property alias fontSize: fontSizeSpinBox.value
    property alias highlightColor: colorInput.text


    GridLayout {
        columns: 2
        rowSpacing: 18
        columnSpacing: 15

        Label { text: "Font Size (px)"; color: root.cMuted }
        SettingsSpinBox {
            id: fontSizeSpinBox
            from: 10
            to: 40
            value: 15
        }

        Label { text: "Highlight Hex"; color: root.cMuted }
        SettingsTextField {
            id: colorInput
            text: "#F6B21A"
        }
    }
}
