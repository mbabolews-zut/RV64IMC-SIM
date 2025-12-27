import QtQuick
import QtQuick.Controls

Item {
    id: root
    property string text: "Button"

    implicitWidth: button.implicitWidth
    implicitHeight: button.implicitHeight

    Button {
        id: button
        font.styleName: "Medium"
        font.family: "Jura"
        topPadding: 9
        bottomPadding: 9
        rightPadding: 26
        leftPadding: 26
        padding: 12

        background: Rectangle {
            color: button.pressed ? "#161b3b" : (button.hovered ? "#1E2550" : "#2A3172")
            radius: 11
            height: root.height
            implicitHeight: root.implicitHeight
        }

        contentItem: Text {
            text: root.text
            font.pointSize: 14
            font.styleName: "SemiBold"
            font.family: "Jura"
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}
