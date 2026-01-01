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
    property bool editingAscii: false  // true = ASCII edit, false = hex edit
    property var asciiCursorAddr: -1   // cursor position in ASCII edit mode
    property bool canEdit: false
    property bool isDragging: false

    // Layout constants
    readonly property int addrWidth: 130
    readonly property int hexCellWidth: 28
    readonly property int asciiCellWidth: 10
    readonly property int separatorWidth: 16
    readonly property int leftMargin: 8
    readonly property int hexAreaStart: leftMargin + addrWidth
    readonly property int hexAreaWidth: 16 * hexCellWidth
    readonly property int asciiAreaStart: hexAreaStart + hexAreaWidth + separatorWidth

    signal addressSelected(var addr)
    signal byteModified(var addr, int value)

    property var pendingCommit: null

    function startEdit(addr, ascii) {
        editingAddress = addr
        editingAscii = ascii
        if (ascii) {
            asciiCursorAddr = addr
            asciiInput.forceActiveFocus()
        }
    }
    function stopEdit() {
        if (pendingCommit) {
            pendingCommit()
            pendingCommit = null
        }
        editingAddress = -1
        editingAscii = false
        asciiCursorAddr = -1
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

    function getSelectedAscii() {
        if (selectionStart < 0 || selectionEnd < 0) {
            if (selectedAddress >= 0) {
                let val = getByteFunc(selectedAddress - baseAddress)
                return val >= 32 && val < 127 ? String.fromCharCode(val) : "."
            }
            return ""
        }
        let min = Math.min(selectionStart, selectionEnd)
        let max = Math.max(selectionStart, selectionEnd)
        let chars = []
        for (let addr = min; addr <= max; addr++) {
            let val = getByteFunc(addr - baseAddress)
            chars.push(val >= 32 && val < 127 ? String.fromCharCode(val) : ".")
        }
        return chars.join('')
    }

    function byteToAscii(val) {
        return (val >= 32 && val < 127) ? String.fromCharCode(val) : "."
    }

    // Returns {addr, isAscii} or null if not on a cell
    function cellAtPosition(x, y) {
        let rowIndex = Math.floor((y + listView.contentY) / 22)
        if (rowIndex < 0 || rowIndex >= rowCount) return null

        // Check hex area
        if (x >= hexAreaStart && x < hexAreaStart + hexAreaWidth) {
            let colIndex = Math.floor((x - hexAreaStart) / hexCellWidth)
            if (colIndex >= 0 && colIndex < 16) {
                return { addr: baseAddress + rowIndex * 16 + colIndex, isAscii: false }
            }
        }

        // Check ASCII area
        if (x >= asciiAreaStart && x < asciiAreaStart + 16 * asciiCellWidth) {
            let colIndex = Math.floor((x - asciiAreaStart) / asciiCellWidth)
            if (colIndex >= 0 && colIndex < 16) {
                return { addr: baseAddress + rowIndex * 16 + colIndex, isAscii: true }
            }
        }

        return null
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

    // Hidden input for ASCII insert-mode editing
    TextInput {
        id: asciiInput
        visible: false
        focus: false

        Keys.onPressed: event => {
            if (!root.editingAscii || root.asciiCursorAddr < 0) return

            if (event.key === Qt.Key_Escape) {
                root.stopEdit()
                event.accepted = true
                return
            }

            if (event.key === Qt.Key_Left) {
                if (root.asciiCursorAddr > root.baseAddress) {
                    root.asciiCursorAddr--
                }
                event.accepted = true
                return
            }

            if (event.key === Qt.Key_Right) {
                let maxAddr = root.baseAddress + root.rowCount * 16 - 1
                if (root.asciiCursorAddr < maxAddr) {
                    root.asciiCursorAddr++
                }
                event.accepted = true
                return
            }

            if (event.key === Qt.Key_Up) {
                if (root.asciiCursorAddr >= root.baseAddress + 16) {
                    root.asciiCursorAddr -= 16
                }
                event.accepted = true
                return
            }

            if (event.key === Qt.Key_Down) {
                let maxAddr = root.baseAddress + root.rowCount * 16 - 1
                if (root.asciiCursorAddr + 16 <= maxAddr) {
                    root.asciiCursorAddr += 16
                }
                event.accepted = true
                return
            }

            if (event.key === Qt.Key_Backspace) {
                if (root.asciiCursorAddr > root.baseAddress) {
                    root.asciiCursorAddr--
                    root.byteModified(root.asciiCursorAddr, 0)
                }
                event.accepted = true
                return
            }

            if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                root.stopEdit()
                event.accepted = true
                return
            }

            // Printable character - write and advance
            if (event.text.length === 1) {
                let charCode = event.text.charCodeAt(0)
                if (charCode >= 32 && charCode < 127) {
                    root.byteModified(root.asciiCursorAddr, charCode)
                    let maxAddr = root.baseAddress + root.rowCount * 16 - 1
                    if (root.asciiCursorAddr < maxAddr) {
                        root.asciiCursorAddr++
                    }
                    event.accepted = true
                }
            }
        }

        onActiveFocusChanged: {
            if (!activeFocus && root.editingAscii) {
                Qt.callLater(root.stopEdit)
            }
        }
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
                anchors.leftMargin: root.leftMargin
                anchors.verticalCenter: parent.verticalCenter
                spacing: 0

                Text {
                    width: root.addrWidth
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
                        width: root.hexCellWidth
                        height: 24
                        color: "#990000"
                        text: index.toString(16).toUpperCase().padStart(2, '0')
                        font.bold: true
                        font.family: "Courier New"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Item { width: root.separatorWidth; height: 1 }

                Text {
                    width: 16 * root.asciiCellWidth
                    height: 24
                    color: "#990000"
                    text: "ASCII"
                    font.bold: true
                    font.family: "Courier New"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
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
                        anchors.leftMargin: root.leftMargin
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 0

                        Text {
                            width: root.addrWidth
                            height: 22
                            color: "#990000"
                            text: rowDelegate.rowAddr.toString(16).toUpperCase().padStart(16, '0')
                            font.family: "Courier New"
                            font.pointSize: 10
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        // Hex cells
                        Repeater {
                            model: 16
                            Rectangle {
                                id: hexCell
                                property var addr: rowDelegate.rowAddr + modelData
                                property int offset: rowDelegate.rowIndex * 16 + modelData
                                property bool isSelected: root.isInSelection(addr)
                                property bool isEditing: addr === root.editingAddress && !root.editingAscii

                                width: root.hexCellWidth
                                height: 22
                                color: isSelected ? "#cce5ff" : "transparent"
                                border.color: isSelected ? "#0066cc" : "transparent"
                                border.width: 1
                                radius: 2

                                Text {
                                    anchors.centerIn: parent
                                    visible: !hexCell.isEditing
                                    color: "#333333"
                                    text: {
                                        void(root.revision)
                                        let val = root.getByteFunc(hexCell.offset)
                                        return val >= 0 ? val.toString(16).toUpperCase().padStart(2, '0') : "??"
                                    }
                                    font.family: "Courier New"
                                    font.pointSize: 10
                                }

                                TextInput {
                                    id: hexEditInput
                                    anchors.centerIn: parent
                                    width: parent.width - 4
                                    visible: hexCell.isEditing
                                    horizontalAlignment: Text.AlignHCenter
                                    font.family: "Courier New"
                                    font.pointSize: 10
                                    maximumLength: 2
                                    validator: RegularExpressionValidator { regularExpression: /[0-9A-Fa-f]{0,2}/ }
                                    selectByMouse: true
                                    property string originalValue: ""

                                    onVisibleChanged: {
                                        if (visible) {
                                            originalValue = root.getByteFunc(hexCell.offset).toString(16).toUpperCase().padStart(2, '0')
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
                                            root.byteModified(hexCell.addr, parseInt(text, 16))
                                        }
                                    }
                                }
                            }
                        }

                        Item { width: root.separatorWidth; height: 1 }

                        // ASCII cells (display only, editing handled by asciiEditor)
                        Repeater {
                            model: 16
                            Rectangle {
                                id: asciiCell
                                property var addr: rowDelegate.rowAddr + modelData
                                property int offset: rowDelegate.rowIndex * 16 + modelData
                                property bool isSelected: root.isInSelection(addr)
                                property bool isCursor: root.editingAscii && addr === root.asciiCursorAddr

                                width: root.asciiCellWidth
                                height: 22
                                color: isCursor ? "#ffcc00" : (isSelected ? "#cce5ff" : "transparent")

                                Text {
                                    anchors.centerIn: parent
                                    color: "#333333"
                                    text: {
                                        void(root.revision)
                                        let val = root.getByteFunc(asciiCell.offset)
                                        return root.byteToAscii(val)
                                    }
                                    font.family: "Courier New"
                                    font.pointSize: 10
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
                    let result = root.cellAtPosition(mouse.x, mouse.y)
                    if (result) {
                        root.isDragging = true
                        root.selectedAddress = result.addr
                        root.selectionStart = result.addr
                        root.selectionEnd = result.addr
                        root.addressSelected(result.addr)

                        // Single click on ASCII starts editing immediately
                        if (result.isAscii && root.canEdit) {
                            root.startEdit(result.addr, true)
                        }
                    }
                }

                onPositionChanged: mouse => {
                    if (root.isDragging && !root.editingAscii) {
                        let result = root.cellAtPosition(mouse.x, mouse.y)
                        if (result) {
                            root.selectionEnd = result.addr
                        }
                    }
                }

                onReleased: {
                    root.isDragging = false
                }

                onDoubleClicked: mouse => {
                    let result = root.cellAtPosition(mouse.x, mouse.y)
                    if (result && root.canEdit && !result.isAscii) {
                        // Double-click only for hex editing
                        root.startEdit(result.addr, false)
                    }
                }
            }
        }
    }
}
