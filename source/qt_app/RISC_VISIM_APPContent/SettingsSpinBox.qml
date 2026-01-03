import QtQuick
import QtQuick.Controls

SpinBox {
    id: control
    editable: true

    readonly property color cText: "#2A3172"
    readonly property color cMuted: "#5C6580"
    readonly property color cAccent: "#CC5500"
    readonly property color cBorder: "#DCE0E8"
    readonly property color cInputBg: "#F7F8FA"

    contentItem: TextInput {
        text: control.textFromValue(control.value, control.locale)
        color: control.cText
        font.pixelSize: 12
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
    }

    background: Rectangle {
        implicitWidth: 100
        implicitHeight: 30
        color: control.cInputBg
        border.color: control.activeFocus ? control.cAccent : control.cBorder
        radius: 3
    }

    up.indicator: Item {
        x: parent.width - 28
        width: 28
        height: 28
        Text { text: "+"; color: control.cMuted; anchors.centerIn: parent; font.bold: true }
    }

    down.indicator: Item {
        x: 0
        width: 28
        height: 28
        Text { text: "-"; color: control.cMuted; anchors.centerIn: parent; font.bold: true }
    }
}
