
/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import RISC_VISIM_APP

Rectangle {
    id: root
    width: Constants.width
    height: Constants.height
    color: Constants.backgroundColor

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Top Toolbar
        Rectangle {
            id: toolbar
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            color: "#f6b21a"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                anchors.topMargin: 8
                anchors.bottomMargin: 10
                spacing: 10

                // Left side buttons
                RowLayout {
                    spacing: 10

                    NavButton {
                        text: "File ▼"
                        Layout.preferredHeight: 44
                    }

                    NavButton {
                        text: "Save"
                        Layout.preferredHeight: 44
                    }

                    NavButton {
                        text: "Settings"
                        Layout.preferredHeight: 44
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                // Right side buttons
                RowLayout {
                    spacing: 10

                    NavButton {
                        Layout.preferredHeight: 44
                        text: "Build"
                    }

                    NavButton {
                        Layout.preferredHeight: 44
                        text: "↻ Reset"
                    }

                    NavButton {
                        Layout.preferredHeight: 44
                        text: "▶| Step"
                    }

                    NavButton {
                        Layout.preferredHeight: 44
                        text: "▶▶ Run"
                    }
                }
            }
        }

        // Main content area
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal

            // Left side: Code editor + Output console
            SplitView {
                SplitView.fillWidth: true
                SplitView.minimumWidth: 300
                orientation: Qt.Vertical

                // Code Editor - directly in SplitView
                TextEditor {
                    id: mainEditor
                    SplitView.fillHeight: true
                    SplitView.fillWidth: true
                    SplitView.minimumHeight: 150
                    font.family: "Courier New"
                    font.pointSize: 12
                }

                // Bottom panel: Output/Memory tabs
                Rectangle {
                    SplitView.preferredHeight: 180
                    SplitView.minimumHeight: 80
                    color: "#ffffff"
                    border.color: "#1a1a5e"
                    border.width: 2

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 0

                        // Tab content
                        StackLayout {
                            id: bottomStack
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            currentIndex: tabBar.currentIndex

                            // Output tab
                            ScrollView {
                                TextArea {
                                    id: outputArea
                                    readOnly: true
                                    font.family: "Courier New"
                                    font.pointSize: 11
                                    placeholderText: "Output will appear here..."
                                }
                            }

                            // Memory tab
                            ScrollView {
                                TextArea {
                                    id: memoryArea
                                    readOnly: true
                                    font.family: "Courier New"
                                    font.pointSize: 11
                                    placeholderText: "Memory view..."
                                }
                            }
                        }

                        // Tab bar at bottom
                        TabBar {
                            id: tabBar
                            Layout.fillWidth: true
                            Layout.preferredHeight: 30
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 10

                            TabButton {
                                text: "output"
                                width: implicitWidth + 20
                            }

                            TabButton {
                                text: "memory"
                                width: implicitWidth + 20
                            }
                        }
                    }
                }
            }

            // Right side: Register panel
            Rectangle {
                SplitView.preferredWidth: 520
                SplitView.minimumWidth: 400
                color: "#f0f0f0"
                border.color: "#cccccc"
                border.width: 1

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 8

                    // Format toggle buttons (Dec/Hex/Bin)
                    TabBar {
                        Layout.alignment: Qt.AlignRight
                        spacing: 2

                        TabButton {
                            id: decBtn
                            text: "Dec"
                            checkable: true
                            checked: true
                            implicitWidth: 50
                            implicitHeight: 28

                            background: Rectangle {
                                color: decBtn.checked ? "#2E43FF" : "#6B6B6B"
                                bottomLeftRadius: 7
                                topLeftRadius: 7
                            }

                            contentItem: Text {
                                text: decBtn.text
                                color: "white"
                                font.pointSize: 10
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }

                        TabButton {
                            id: hexBtn
                            text: "Hex"
                            checkable: true
                            implicitWidth: 50
                            implicitHeight: 28

                            background: Rectangle {
                                color: hexBtn.checked ? "#2E43FF" : "#6B6B6B"
                            }

                            contentItem: Text {
                                text: hexBtn.text
                                color: "white"
                                font.pointSize: 10
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }

                        TabButton {
                            id: binBtn
                            text: "Bin"
                            checkable: true
                            implicitWidth: 50
                            implicitHeight: 28

                            background: Rectangle {
                                color: binBtn.checked ? "#2E43FF" : "#6B6B6B"
                                bottomRightRadius: 7
                                topRightRadius: 7
                            }

                            contentItem: Text {
                                text: binBtn.text
                                color: "white"
                                font.pointSize: 10
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }

                    // Register table with header
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "#ffffff"
                        border.color: "#cccccc"
                        border.width: 1

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 0

                            // Header row
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 30
                                color: "#e0e0e0"

                                Row {
                                    anchors.fill: parent
                                    anchors.leftMargin: 8
                                    anchors.rightMargin: 8
                                    spacing: 0

                                    Text {
                                        width: 82
                                        height: parent.height
                                        text: "Reg"
                                        font.bold: true
                                        font.family: "Courier New"
                                        font.pointSize: 11
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignLeft
                                    }

                                    Text {
                                        width: 98
                                        height: parent.height
                                        text: "ABI"
                                        font.bold: true
                                        font.family: "Courier New"
                                        font.pointSize: 11
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignLeft
                                    }

                                    Text {
                                        width: parent.width - 110
                                        height: parent.height
                                        text: "Value"
                                        font.bold: true
                                        font.family: "Courier New"
                                        font.pointSize: 11
                                        verticalAlignment: Text.AlignVCenter
                                        horizontalAlignment: Text.AlignLeft
                                    }
                                }
                            }

                            // Register list
                            ListView {
                                id: registerList
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                clip: true
                                boundsBehavior: Flickable.StopAtBounds

                                ScrollBar.vertical: ScrollBar {
                                    active: true
                                    policy: ScrollBar.AsNeeded
                                }

                                model: ListModel {
                                    ListElement {
                                        reg: "x0"
                                        abi: "zero"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x1"
                                        abi: "ra"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x2"
                                        abi: "sp"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x3"
                                        abi: "gp"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x4"
                                        abi: "tp"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x5"
                                        abi: "t0"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x6"
                                        abi: "t1"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x7"
                                        abi: "t2"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x8"
                                        abi: "s0/fp"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x9"
                                        abi: "s1"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x10"
                                        abi: "a0"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x11"
                                        abi: "a1"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x12"
                                        abi: "a2"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x13"
                                        abi: "a3"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x14"
                                        abi: "a4"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x15"
                                        abi: "a5"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x16"
                                        abi: "a6"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x17"
                                        abi: "a7"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x18"
                                        abi: "s2"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x19"
                                        abi: "s3"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x20"
                                        abi: "s4"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x21"
                                        abi: "s5"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x22"
                                        abi: "s6"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x23"
                                        abi: "s7"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x24"
                                        abi: "s8"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x25"
                                        abi: "s9"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x26"
                                        abi: "s10"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x27"
                                        abi: "s11"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x28"
                                        abi: "t3"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x29"
                                        abi: "t4"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x30"
                                        abi: "t5"
                                        value: "0x0000000000000000"
                                    }
                                    ListElement {
                                        reg: "x31"
                                        abi: "t6"
                                        value: "0x0000000000000000"
                                    }
                                }

                                delegate: Rectangle {
                                    width: registerList.width
                                    height: 28
                                    color: index % 2 === 0 ? "#ffffff" : "#f8f8f8"

                                    Row {
                                        anchors.fill: parent
                                        anchors.leftMargin: 8
                                        anchors.rightMargin: 8
                                        spacing: 34

                                        Text {
                                            width: 50
                                            height: parent.height
                                            text: model.reg
                                            font.family: "Courier New"
                                            font.pointSize: 11
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignLeft
                                        }

                                        Text {
                                            width: 60
                                            height: parent.height
                                            text: model.abi
                                            font.family: "Courier New"
                                            font.pointSize: 11
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignLeft
                                        }

                                        Text {
                                            width: parent.width - 118
                                            height: parent.height
                                            text: model.value
                                            font.family: "Courier New"
                                            font.pointSize: 11
                                            verticalAlignment: Text.AlignVCenter
                                            horizontalAlignment: Text.AlignLeft
                                            elide: Text.ElideRight
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
