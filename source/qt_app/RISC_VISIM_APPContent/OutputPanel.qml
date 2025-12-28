import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property alias outputArea: outputTextArea
    property alias memoryArea: memoryTextArea
    property alias currentTabIndex: tabBar.currentIndex

    color: "#1a1a5e"
    border.color: "#1a1a5e"
    border.width: 4

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: root.border.width
        spacing: 2

        StackLayout {
            id: contentStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            ScrollView {
                TextArea {
                    background: Rectangle {
                        color: "#ffffff"
                    }
                    id: outputTextArea
                    readOnly: true
                    textFormat: TextEdit.RichText
                    font.family: "Courier New"
                    font.pointSize: 11
                    placeholderText: "Output will appear here..."
                    text: backend.output
                }
            }

            ScrollView {
                TextArea {
                    background: Rectangle {
                        color: "#ffffff"
                    }

                    id: memoryTextArea
                    readOnly: true
                    font.family: "Courier New"
                    font.pointSize: 11
                    placeholderText: "Memory view..."
                }
            }
        }

        Rectangle {
            Layout.preferredHeight: 30
            Layout.fillWidth: true
            color: "#f0f0f0"
            clip: true

            TabBar {
                id: tabBar
                contentHeight: parent.Layout.preferredHeight
                width: contentWidth + 1

                background: Rectangle {
                    color: "#000000"
                }

                TabButton {
                    text: "output"
                    width: implicitWidth + 20
                    background : Rectangle {
                        color: parent.checked ? "#f6f6f6" : "#54547b"
                    }
                }

                TabButton {
                    text: "memory"
                    width: implicitWidth + 20
                    background : Rectangle {
                        color: parent.checked ? "#f6f6f6" : "#54547b"
                    }
                }
            }
        }
    }
}

