import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    required property ListView listView

    property int regColumnWidth: 82
    property int abiColumnWidth: 98

    Layout.fillWidth: true
    Layout.preferredHeight: 30
    color: "#e0e0e0"

    Row {
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8

        Text {
            width: root.regColumnWidth
            height: parent.height
            text: "Reg"
            font { bold: true; family: "Courier New"; pointSize: 11 }
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            width: root.abiColumnWidth
            height: parent.height
            text: "ABI"
            font { bold: true; family: "Courier New"; pointSize: 11 }
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            width: parent.width - root.regColumnWidth - root.abiColumnWidth
            height: parent.height
            text: "Value"
            font { bold: true; family: "Courier New"; pointSize: 11 }
            verticalAlignment: Text.AlignVCenter
        }
    }
}

