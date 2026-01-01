import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root

    required property int regIndex
    required property string regName
    required property string abiName
    required property string regValue
    required property bool isModified

    height: 28
    color: regIndex % 2 === 0 ? "#ffffff" : "#f8f8f8"

    readonly property font monoFont: Qt.font({
        family: "Courier New",
        pointSize: 11,
        bold: isModified
    })

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 8
        anchors.rightMargin: 8
        spacing: 8

        Text {
            Layout.preferredWidth: 50
            text: root.regName
            font: root.monoFont
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Text {
            Layout.preferredWidth: 60
            text: root.abiName
            font: root.monoFont
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Text {
                id: valueDisplay
                anchors.fill: parent
                visible: !valueEdit.visible
                text: root.regValue
                font: root.monoFont
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideLeft

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        valueEdit.text = root.regValue
                        valueEdit.visible = true
                        valueEdit.forceActiveFocus()
                        valueEdit.selectAll()
                    }
                }
            }

            TextInput {
                id: valueEdit
                anchors.fill: parent
                visible: false
                font: root.monoFont
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                selectByMouse: true
                maximumLength: registerModel.maxInputLength()

                validator: RegularExpressionValidator {
                    id: regexValidator
                }

                Connections {
                    target: registerModel
                    function onFormatChanged() {
                        regexValidator.regularExpression = new RegExp(registerModel.validatorPattern())
                    }
                }

                Component.onCompleted: {
                    regexValidator.regularExpression = new RegExp(registerModel.validatorPattern())
                }

                onEditingFinished: commit()
                onActiveFocusChanged: if (!activeFocus) visible = false
                Keys.onEscapePressed: { visible = false }

                function commit() {
                    visible = false
                    if (text !== root.regValue)
                        registerModel.modify(root.regIndex, text)
                }
            }
        }
    }
}
