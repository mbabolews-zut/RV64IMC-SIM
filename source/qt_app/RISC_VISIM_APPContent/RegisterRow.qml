import QtQuick

Rectangle {
    id: root

    required property int index
    required property string reg
    required property string abi
    required property string value

    property int regColumnWidth: 50
    property int abiColumnWidth: 60

    width: ListView.view ? ListView.view.width : 200
    height: 28
    color: index % 2 === 0 ? "#ffffff" : "#f8f8f8"

    Row {
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        spacing: 34

        Text {
            width: root.regColumnWidth
            height: parent.height
            text: root.reg
            font { family: "Courier New"; pointSize: 11 }
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            width: root.abiColumnWidth
            height: parent.height
            text: root.abi
            font { family: "Courier New"; pointSize: 11 }
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            width: parent.width - root.regColumnWidth - root.abiColumnWidth - 68
            height: parent.height
            text: root.value
            font { family: "Courier New"; pointSize: 11 }
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }
}

