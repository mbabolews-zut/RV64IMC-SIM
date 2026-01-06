import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property int selectedAddress: -1
    property var dataTypesValues: []
    readonly property bool canEdit: !backend.runLocked

    color: "#1a1a5e"
    border.color: "#1a1a5e"
    border.width: 4

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: root.border.width
        spacing: 2

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            ScrollView {
                TextArea {
                    background: Rectangle { color: "#ffffff" }
                    readOnly: true
                    textFormat: TextEdit.RichText
                    font.family: "monospace"
                    font.pixelSize: 14
                    placeholderText: "Output will appear here..."
                    text: backend.output
                }
            }

            SplitView {
                orientation: Qt.Horizontal

                handle: Rectangle {
                    implicitWidth: 4
                    implicitHeight: 4
                    color: SplitHandle.hovered ? "#e4e0d0" : "#d8d4e4"
                }

                MemoryGrid {
                    SplitView.fillWidth: true
                    SplitView.fillHeight: true
                    SplitView.minimumWidth: 200
                    baseAddress: memoryController.dataBaseAddress
                    rowCount: memoryController.dataRowCount
                    revision: memoryController.revision
                    getByteFunc: addr => memoryController.getByte(memoryController.dataBaseAddress + addr)
                    selectedAddress: root.selectedAddress
                    canEdit: root.canEdit

                    onAddressSelected: addr => {
                        root.selectedAddress = addr
                        memoryController.loadDataTypes(addr)
                    }
                    onByteModified: (addr, value) => {
                        memoryController.modifyByte(addr, value)
                        if (root.selectedAddress >= 0)
                            memoryController.loadDataTypes(root.selectedAddress)
                    }
                }

                DataTypesPanel {
                    SplitView.preferredWidth: 220
                    SplitView.minimumWidth: 180
                    SplitView.maximumWidth: 300
                    selectedAddress: root.selectedAddress
                    canEdit: root.canEdit
                    values: root.dataTypesValues

                    onValueEdited: (addr, typeIndex, value) => {
                        memoryController.modifyValue(addr, typeIndex, value)
                        memoryController.loadDataTypes(root.selectedAddress >= 0 ? root.selectedAddress : addr)
                    }
                }
            }

            SplitView {
                orientation: Qt.Horizontal

                handle: Rectangle {
                    implicitWidth: 4
                    implicitHeight: 4
                    color: SplitHandle.hovered ? "#e4e0d0" : "#d8d4e4"
                }

                MemoryGrid {
                    SplitView.fillWidth: true
                    SplitView.fillHeight: true
                    SplitView.minimumWidth: 200
                    baseAddress: memoryController.stackBaseAddress
                    rowCount: memoryController.stackRowCount
                    revision: memoryController.revision
                    getByteFunc: addr => memoryController.getByte(memoryController.stackBaseAddress + addr)
                    selectedAddress: root.selectedAddress
                    canEdit: root.canEdit

                    onAddressSelected: addr => {
                        root.selectedAddress = addr
                        memoryController.loadDataTypes(addr)
                    }
                    onByteModified: (addr, value) => {
                        memoryController.modifyByte(addr, value)
                        if (root.selectedAddress >= 0)
                            memoryController.loadDataTypes(root.selectedAddress)
                    }
                }

                DataTypesPanel {
                    SplitView.preferredWidth: 220
                    SplitView.minimumWidth: 180
                    SplitView.maximumWidth: 300
                    selectedAddress: root.selectedAddress
                    canEdit: root.canEdit
                    values: root.dataTypesValues

                    onValueEdited: (addr, typeIndex, value) => {
                        memoryController.modifyValue(addr, typeIndex, value)
                        memoryController.loadDataTypes(root.selectedAddress >= 0 ? root.selectedAddress : addr)
                    }
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
                contentHeight: 30
                width: contentWidth + 1
                background: Rectangle { color: "#000000" }
                spacing: 1

                Repeater {
                    model: ["output", "memory:data", "memory:stack"]
                    TabButton {
                        required property string modelData
                        text: modelData
                        width: implicitWidth + 20
                        font.pixelSize: 16
                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: parent.checked ? "black" : "white"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            color: parent.checked ? "#f6f6f6" : "#54547b"
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: memoryController
        function onDataTypesLoaded(values) {
            root.dataTypesValues = values
        }
    }
}
