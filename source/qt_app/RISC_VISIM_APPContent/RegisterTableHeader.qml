import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property int regColumnWidth: 50
    property int abiColumnWidth: 60
    property int spacing: 8

    Layout.fillWidth: true
    Layout.preferredHeight: 30
    color: "#e0e0e0"

    Row {
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        spacing: root.spacing

        Text {
            width: root.regColumnWidth
            height: parent.height
            text: "Reg"
            font { bold: true; family: "monospace"; pixelSize: 14 }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            width: root.abiColumnWidth
            height: parent.height
            text: "ABI"
            font { bold: true; family: "monospace"; pixelSize: 14 }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            width: parent.width - root.regColumnWidth - root.abiColumnWidth - root.spacing * 2
            height: parent.height
            text: "Value"
            font { bold: true; family: "monospace"; pixelSize: 14 }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }
}

