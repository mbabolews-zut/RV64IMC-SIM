import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#ffffff"
    focus: true

    required property var baseAddress
    required property int rowCount
    required property var getByteFunc
    required property int revision

    property var selectedAddress: -1
    property var selectionStart: -1
    property var selectionEnd: -1
    property var editingAddress: -1
    property bool canEdit: false
    property bool isDragging: false

    signal addressSelected(var addr)
    signal byteModified(var addr, int value)

    property var pendingCommit: null

    function startEdit(addr) { editingAddress = addr }
    function stopEdit() {
        if (pendingCommit) {
            pendingCommit()
            pendingCommit = null
        }
        editingAddress = -1
    }

    function isInSelection(addr) {
        if (selectionStart < 0 || selectionEnd < 0) return addr === selectedAddress
        let min = Math.min(selectionStart, selectionEnd)
        let max = Math.max(selectionStart, selectionEnd)
        return addr >= min && addr <= max
    }

    function getSelectedBytes() {
        if (selectionStart < 0 || selectionEnd < 0) {
            if (selectedAddress >= 0) {
                let val = getByteFunc(selectedAddress - baseAddress)
                return val >= 0 ? val.toString(16).toUpperCase().padStart(2, '0') : ""
            }
            return ""
        }
        let min = Math.min(selectionStart, selectionEnd)
        let max = Math.max(selectionStart, selectionEnd)
        let bytes = []
        for (let addr = min; addr <= max; addr++) {
            let val = getByteFunc(addr - baseAddress)
            if (val >= 0) bytes.push(val.toString(16).toUpperCase().padStart(2, '0'))
        }
        return bytes.join(' ')
    }

    function cellAtPosition(x, y) {
        // y is relative to listView container
        let rowIndex = Math.floor((y + listView.contentY) / 22)
        if (rowIndex < 0 || rowIndex >= rowCount) return -1

        // Fixed layout: 8px margin + 130px address + 16 cells * 28px each
        let colX = x - 8 - 130
        if (colX < 0) return -1

        let colIndex = Math.floor(colX / 28)
        if (colIndex < 0 || colIndex >= 16) return -1

        return baseAddress + rowIndex * 16 + colIndex
    }

    Keys.onPressed: event => {
        if (event.key === Qt.Key_C && (event.modifiers & Qt.ControlModifier)) {
            let text = getSelectedBytes()
            if (text.length > 0) {
                clipboard.setText(text)
            }
            event.accepted = true
        }
    }

    TextEdit {
        id: clipboard
        visible: false
        function setText(t) { text = t; selectAll(); copy() }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 24
            color: "#f0f0f0"
            border.color: "#cccccc"
            border.width: 1

            Row {
                anchors.left: parent.left
                anchors.leftMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                spacing: 0

                Text {
                    width: 130
                    height: 24
                    color: "#990000"
                    text: "Address"
                    font.bold: true
                    font.family: "Courier New"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                Repeater {
                    model: 16
                    Text {
                        width: 28
                        height: 24
                        color: "#990000"
                        text: index.toString(16).toUpperCase().padStart(2, '0')
                        font.bold: true
                        font.family: "Courier New"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ListView {
                id: listView
                anchors.fill: parent
                clip: true
                cacheBuffer: 100
                model: root.rowCount || 256
                interactive: !root.isDragging

                delegate: Rectangle {
                    id: rowDelegate
                    property int rowIndex: index
                    property var rowAddr: root.baseAddress + rowIndex * 16
                    width: listView.width
                    height: 22
                    color: rowIndex % 2 === 0 ? "#ffffff" : "#f8f8f8"

                    Row {
                        anchors.left: parent.left
                        anchors.leftMargin: 8
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 0

                        Text {
                            width: 130
                            height: 22
                            color: "#990000"
                            text: rowDelegate.rowAddr.toString(16).toUpperCase().padStart(16, '0')
                            font.family: "Courier New"
                            font.pointSize: 10
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        Repeater {
                            model: 16
                            Rectangle {
                                id: cell
                                property var addr: rowDelegate.rowAddr + modelData
                                property int offset: rowDelegate.rowIndex * 16 + modelData
                                property bool isSelected: root.isInSelection(addr)
                                property bool isEditing: addr === root.editingAddress

                                width: 28
                                height: 22
                                color: isSelected ? "#cce5ff" : "transparent"
                                border.color: isSelected ? "#0066cc" : "transparent"
                                border.width: 1
                                radius: 2

                                Text {
                                    anchors.centerIn: parent
                                    visible: !cell.isEditing
                                    color: "#333333"
                                    text: {
                                        void(root.revision)
                                        let val = root.getByteFunc(cell.offset)
                                        return val >= 0 ? val.toString(16).toUpperCase().padStart(2, '0') : "??"
                                    }
                                    font.family: "Courier New"
                                    font.pointSize: 10
                                }

                                TextInput {
                                    id: editInput
                                    anchors.centerIn: parent
                                    width: parent.width - 4
                                    visible: cell.isEditing
                                    horizontalAlignment: Text.AlignHCenter
                                    font.family: "Courier New"
                                    font.pointSize: 10
                                    maximumLength: 2
                                    validator: RegularExpressionValidator { regularExpression: /[0-9A-Fa-f]{0,2}/ }
                                    selectByMouse: true
                                    property string originalValue: ""

                                    onVisibleChanged: {
                                        if (visible) {
                                            originalValue = root.getByteFunc(cell.offset).toString(16).toUpperCase().padStart(2, '0')
                                            text = originalValue
                                            selectAll()
                                            forceActiveFocus()
                                            root.pendingCommit = commitValue
                                        }
                                    }

                                    Keys.onReturnPressed: { commitValue(); root.stopEdit() }
                                    Keys.onEnterPressed: { commitValue(); root.stopEdit() }
                                    Keys.onEscapePressed: { root.pendingCommit = null; root.stopEdit() }

                                    function commitValue() {
                                        if (text.length > 0 && text !== originalValue) {
                                            root.byteModified(cell.addr, parseInt(text, 16))
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            MouseArea {
                id: dragArea
                anchors.fill: parent

                onPressed: mouse => {
                    // Commit any active edit first
                    if (root.editingAddress >= 0) {
                        root.stopEdit()
                    }

                    root.forceActiveFocus()
                    let addr = root.cellAtPosition(mouse.x, mouse.y)
                    if (addr >= 0) {
                        root.isDragging = true
                        root.selectedAddress = addr
                        root.selectionStart = addr
                        root.selectionEnd = addr
                        root.addressSelected(addr)
                    }
                }

                onPositionChanged: mouse => {
                    if (root.isDragging) {
                        let addr = root.cellAtPosition(mouse.x, mouse.y)
                        if (addr >= 0) {
                            root.selectionEnd = addr
                        }
                    }
                }

                onReleased: {
                    root.isDragging = false
                }

                onDoubleClicked: mouse => {
                    let addr = root.cellAtPosition(mouse.x, mouse.y)
                    if (addr >= 0 && root.canEdit) {
                        root.startEdit(addr)
                    }
                }
            }
        }
    }
}
