import QtQuick
import QtQuick.Controls

Row {
    id: root
    spacing: 0

    readonly property var formats: [
        { label: "Dec", format: 10 },
        { label: "Hex", format: 16 },
        { label: "Bin", format: 2 }
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

            background: Item {
                clip: true
                Rectangle {
                    width: parent.width + (index === 1 ? 0 : 7)
                    height: parent.height
                    x: index === 2 ? -7 : 0
                    color: parent.parent.checked ? "#2E43FF" : "#6B6B6B"
                    radius: index === 1 ? 0 : 7
                }
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
