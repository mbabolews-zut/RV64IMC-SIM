import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

SettingsPage {
    id: root
    title: "SYSTEM SETTINGS"

    readonly property color cAccent: "#CC5500"
    readonly property color cBorder: "#DCE0E8"
    readonly property color cMuted: "#5C6580"

    ColumnLayout {
        spacing: 18
        anchors.left: parent.left
        anchors.right: parent.right

        GridLayout {
            columns: 2
            rowSpacing: 18
            columnSpacing: 15

            Label { text: "Stack Pointer Init"; color: root.cMuted }
            SettingsComboBox {
                id: spPosCombo
                model: ["Zero", "Stack Bottom", "Stack Top"]
                currentIndex: settingsManager.spPosIndex
                onCurrentIndexChanged: settingsManager.spPosIndex = currentIndex
            }

            Label { text: "Endianness"; color: root.cMuted }
            SettingsComboBox {
                id: endiannessCombo
                model: ["Little Endian", "Big Endian"]
                currentIndex: settingsManager.endiannessIndex
                onCurrentIndexChanged: settingsManager.endiannessIndex = currentIndex
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.topMargin: 10
            height: 1
            color: root.cBorder
        }

        Label {
            text: "Version 1.0.4-Stable"
            color: root.cAccent
            font.pixelSize: 12
            font.bold: true
        }

        Text {
            text: "RISC-V 64-bit ISA simulator for education and testing."
            color: root.cMuted
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            font.pixelSize: 13
            lineHeight: 1.4
        }
    }
}
