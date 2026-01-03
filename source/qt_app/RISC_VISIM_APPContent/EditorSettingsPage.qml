import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

SettingsPage {
    id: root
    title: "EDITOR SETTINGS"

    readonly property color cMuted: "#5C6580"

    property alias fontSize: fontSizeSpinBox.value
    property alias fontFamily: fontFamilyCombo.currentIndex
    property alias highlightColor: colorInput.text

    property var fontFamilyModel: ["Courier New", "Consolas", "Monospace"]

    GridLayout {
        columns: 2
        rowSpacing: 18
        columnSpacing: 15

        Label { text: "Font Size"; color: root.cMuted }
        SettingsSpinBox {
            id: fontSizeSpinBox
            from: 8
            to: 32
            value: 12
        }

        Label { text: "Font Family"; color: root.cMuted }
        SettingsComboBox {
            id: fontFamilyCombo
            model: root.fontFamilyModel
        }

        Label { text: "Highlight Hex"; color: root.cMuted }
        SettingsTextField {
            id: colorInput
            text: "#F6B21A"
        }
    }
}
