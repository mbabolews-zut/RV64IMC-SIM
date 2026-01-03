import QtQuick
import QtQuick.Controls

TextField {
    id: control

    readonly property color cText: "#2A3172"
    readonly property color cAccent: "#CC5500"
    readonly property color cBorder: "#DCE0E8"
    readonly property color cInputBg: "#F7F8FA"

    color: cText
    font.pixelSize: 12
    background: Rectangle {
        implicitWidth: 160
        implicitHeight: 30
        color: control.cInputBg
        border.color: control.activeFocus ? control.cAccent : control.cBorder
        radius: 3
    }
}
