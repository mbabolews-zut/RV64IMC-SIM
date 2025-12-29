import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property alias formatToggle: formatToggle
    property alias registerList: registerListView
    property alias registerModel: regModel

    color: "#f0f0f0"
    border.color: "#cccccc"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        FormatToggle {
            id: formatToggle
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

                RegisterTableHeader {
                    listView: registerListView
                }

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

                    model: RegisterModel { id: regModel }

                    delegate: RegisterRow {}
                }
            }
        }
    }

    Connections {
        target: backend
        function onRegistersChanged() {
            regModel.updateValues();
        }
    }
}

