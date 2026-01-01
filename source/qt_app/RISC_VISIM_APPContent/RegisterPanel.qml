import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    color: "#f0f0f0"
    border.color: "#cccccc"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        FormatToggle {
            Layout.alignment: Qt.AlignRight
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#ffffff"
            border.color: "#cccccc"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                RegisterTableHeader {}

                ListView {
                    id: registerListView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    boundsBehavior: Flickable.StopAtBounds

                    ScrollBar.vertical: ScrollBar {
                        active: true
                        policy: ScrollBar.AsNeeded
                    }

                    model: registerModel

                    delegate: RegisterRow {
                        required property int index
                        required property string reg
                        required property string abi
                        required property string value
                        required property bool modified

                        regIndex: index
                        regName: reg
                        abiName: abi
                        regValue: value
                        isModified: modified
                        width: registerListView.width
                    }
                }
            }
        }
    }
}
