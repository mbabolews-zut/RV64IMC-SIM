import QtQuick
import QtQuick.Controls

Row {
    id: root
    spacing: 0

    readonly property var formats: [
        { label: "Dec", format: 10, radius: [7, 0, 0, 7] },
        { label: "Hex", format: 16, radius: [0, 0, 0, 0] },
        { label: "Bin", format: 2, radius: [0, 7, 7, 0] }
    ]

    Repeater {
        model: root.formats

        TabButton {
            required property var modelData
            required property int index

            text: modelData.label
            checkable: true
            checked: registerModel.format === modelData.format
            implicitWidth: 50
            implicitHeight: 28

            onClicked: registerModel.format = modelData.format

            background: Rectangle {
                color: parent.checked ? "#2E43FF" : "#6B6B6B"
                topLeftRadius: modelData.radius[0]
                topRightRadius: modelData.radius[1]
                bottomRightRadius: modelData.radius[2]
                bottomLeftRadius: modelData.radius[3]
            }

            contentItem: Text {
                text: parent.text
                color: "white"
                font.pointSize: 10
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
