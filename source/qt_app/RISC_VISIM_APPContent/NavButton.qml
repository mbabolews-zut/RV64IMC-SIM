import QtQuick
import QtQuick.Controls

Item {
    id: root
    property string text: "Button"
    property alias enabled: button.enabled
    signal clicked()

    implicitWidth: button.implicitWidth
    implicitHeight: button.implicitHeight

    Button {
        id: button
        anchors.fill: parent
        font.styleName: "Medium"
        font.family: "Jura"
        topPadding: 9
        bottomPadding: 9
        rightPadding: 18
        leftPadding: 18
        hoverEnabled: true

        onClicked: root.clicked()

        background: Rectangle {
            color: !button.enabled ? "#4A4A6C"
                : (button.pressed ? "#161b3b"
                : (button.hovered ? "#1E2550" : "#2A3172"))
            radius: 9
            antialiasing: true
        }

        contentItem: Text {
            text: root.text
            font.pixelSize: 18
            font.styleName: "SemiBold"
            font.family: "Jura"
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}
