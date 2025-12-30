import QtQuick

Rectangle {
    id: root

    required property int index
    required property string reg
    required property string abi
    required property string value
    required property bool modified
    required property string originalValue

    property int regColumnWidth: 50
    property int abiColumnWidth: 60
    property int spacing: 8

    width: ListView.view ? ListView.view.width : 200
    height: 28
    color: index % 2 === 0 ? "#ffffff" : "#f8f8f8"

    Row {
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        spacing: root.spacing

        Text {
            width: root.regColumnWidth
            height: parent.height
            text: root.reg
            font { family: "Courier New"; pointSize: 11 }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            width: root.abiColumnWidth
            height: parent.height
            text: root.abi
            font { family: "Courier New"; pointSize: 11 }
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        Item {
            width: parent.width - root.regColumnWidth - root.abiColumnWidth - root.spacing * 2
            height: parent.height

            Text {
                id: valueDisplay
                anchors.fill: parent
                text: root.value
                font { family: "Courier New"; pointSize: 11; bold: root.modified }
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideLeft
                visible: !valueEdit.visible

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        valueEdit.visible = true
                        valueEdit.text = root.value
                        valueEdit.forceActiveFocus()
                        valueEdit.selectAll()
                    }
                }
            }

            TextInput {
                id: valueEdit
                anchors.fill: parent
                visible: false
                font { family: "Courier New"; pointSize: 11; bold: root.modified }
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                selectByMouse: true
                maximumLength: backend.regDisplayFormat === 2 ? 64 :
                               (backend.regDisplayFormat === 16 ? 18 : 20) // 0x + 16 hex digits, or 20 dec digits

                validator: RegularExpressionValidator {
                    regularExpression: backend.regDisplayFormat === 2 ? /[01]+/ :
                                      (backend.regDisplayFormat === 16 ? /0?[xX]?[0-9a-fA-F]*/ : /-?[0-9]*/)
                }

                onEditingFinished: {
                    visible = false
                    if (text !== root.value) {
                        if (backend.modifyRegister(root.index, text)) {
                            var model = root.ListView.view.model
                            model.setModified(root.index, text !== root.originalValue)
                        }
                    }
                }

                onActiveFocusChanged: {
                    if (!activeFocus) {
                        visible = false
                    }
                }

                Keys.onEscapePressed: {
                    text = root.value
                    visible = false
                }
            }
        }
    }
}

