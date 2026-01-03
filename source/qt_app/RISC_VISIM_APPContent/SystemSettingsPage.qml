import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

SettingsPage {
    id: root
    title: "RISC-V SIMULATOR"

    readonly property color cAccent: "#CC5500"
    readonly property color cBorder: "#DCE0E8"
    readonly property color cMuted: "#5C6580"

    property string version: "1.0.4-Stable"
    property string description: "64-bit Instruction Set Architecture environment for education and testing."

    ColumnLayout {
        spacing: 12
        anchors.left: parent.left
        anchors.right: parent.right

        Label {
            text: "Version " + root.version
            color: root.cAccent
            font.pixelSize: 12
            font.bold: true
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: root.cBorder
        }

        Text {
            text: root.description
            color: root.cMuted
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
            font.pixelSize: 13
            lineHeight: 1.4
        }
    }
}
