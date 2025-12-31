import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property alias outputArea: outputTextArea
    property alias currentTabIndex: tabBar.currentIndex

    property var selectedAddress: -1
    property var dataTypesValues: []
    property bool canEdit: !backend.runLocked

    implicitWidth: 940
    implicitHeight: 600

    color: "#1a1a5e"
    border.color: "#1a1a5e"
    border.width: 4

    function onAddressSelected(addr) {
        selectedAddress = addr
        backend.loadDataTypesForAddress(addr)
    }

    function onByteModified(addr, value) {
        backend.modifyMemoryByte(addr, value)
        if (selectedAddress >= 0) backend.loadDataTypesForAddress(selectedAddress)
    }

    function onTypeValueEdited(address, typeIndex, value) {
        if (address >= 0) {
            backend.modifyMemoryValue(address, typeIndex, value)
            backend.loadDataTypesForAddress(selectedAddress >= 0 ? selectedAddress : address)
        }
    }

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
                    id: outputTextArea
                    background: Rectangle { color: "#ffffff" }
                    readOnly: true
                    textFormat: TextEdit.RichText
                    font.family: "Courier New"
                    font.pointSize: 11
                    placeholderText: "Output will appear here..."
                    text: backend.output
                }
            }

            SplitView {
                orientation: Qt.Horizontal

                MemoryGrid {
                    SplitView.fillWidth: true
                    SplitView.fillHeight: true
                    SplitView.minimumWidth: 200
                    baseAddress: backend.dataBaseAddress
                    rowCount: backend.dataRowCount
                    revision: backend.memoryRevision
                    getByteFunc: offset => backend.getDataByte(offset)
                    selectedAddress: root.selectedAddress
                    canEdit: root.canEdit
                    onAddressSelected: addr => root.onAddressSelected(addr)
                    onByteModified: (addr, value) => root.onByteModified(addr, value)
                }

                DataTypesPanel {
                    SplitView.preferredWidth: 220
                    SplitView.minimumWidth: 180
                    SplitView.maximumWidth: 300
                    selectedAddress: root.selectedAddress
                    canEdit: root.canEdit
                    values: root.dataTypesValues
                    onValueEdited: (addr, typeIndex, value) => root.onTypeValueEdited(addr, typeIndex, value)
                }
            }

            SplitView {
                orientation: Qt.Horizontal

                MemoryGrid {
                    SplitView.fillWidth: true
                    SplitView.fillHeight: true
                    SplitView.minimumWidth: 200
                    baseAddress: backend.stackBaseAddress
                    rowCount: backend.stackRowCount
                    revision: backend.memoryRevision
                    getByteFunc: offset => backend.getStackByte(offset)
                    selectedAddress: root.selectedAddress
                    canEdit: root.canEdit
                    onAddressSelected: addr => root.onAddressSelected(addr)
                    onByteModified: (addr, value) => root.onByteModified(addr, value)
                }

                DataTypesPanel {
                    SplitView.preferredWidth: 220
                    SplitView.minimumWidth: 180
                    SplitView.maximumWidth: 300
                    selectedAddress: root.selectedAddress
                    canEdit: root.canEdit
                    values: root.dataTypesValues
                    onValueEdited: (addr, typeIndex, value) => root.onTypeValueEdited(addr, typeIndex, value)
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

                background: Rectangle { color: "#000000" }

                TabButton {
                    text: "output"
                    width: implicitWidth + 20
                    background: Rectangle { color: parent.checked ? "#f6f6f6" : "#54547b" }
                }

                TabButton {
                    text: "memory:data"
                    width: implicitWidth + 20
                    background: Rectangle { color: parent.checked ? "#f6f6f6" : "#54547b" }
                }

                TabButton {
                    text: "memory:stack"
                    width: implicitWidth + 20
                    background: Rectangle { color: parent.checked ? "#f6f6f6" : "#54547b" }
                }
            }
        }
    }

    Connections {
        target: backend
        function onDataTypesLoaded(values) {
            root.dataTypesValues = values
        }
    }
}