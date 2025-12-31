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
    property var editingAddress: -1
    property bool canEdit: false

    signal addressSelected(var addr)
    signal byteModified(var addr, int value)

    function startEdit(addr) { editingAddress = addr }
    function stopEdit() { editingAddress = -1 }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 24
            color: "#f0f0f0"
            border.color: "#cccccc"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                spacing: 0

                Text {
                    Layout.preferredWidth: 130
                    color: "#990000"
                    text: "Address"
                    font.bold: true
                    font.family: "Courier New"
                    horizontalAlignment: Text.AlignHCenter
                }

                Repeater {
                    model: 16
                    Text {
                        Layout.preferredWidth: 28
                        color: "#990000"
                        text: index.toString(16).toUpperCase().padStart(2, '0')
                        font.bold: true
                        font.family: "Courier New"
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }
        }

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            cacheBuffer: 100
            model: root.rowCount || 256

            delegate: Rectangle {
                id: rowDelegate
                property int rowIndex: index
                property var rowAddr: root.baseAddress + rowIndex * 16
                width: listView.width
                height: 22
                color: rowIndex % 2 === 0 ? "#ffffff" : "#f8f8f8"

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8
                    spacing: 0

                    Text {
                        Layout.preferredWidth: 130
                        color: "#990000"
                        text: rowDelegate.rowAddr.toString(16).toUpperCase().padStart(16, '0')
                        font.family: "Courier New"
                        font.pointSize: 10
                        horizontalAlignment: Text.AlignHCenter
                    }

                    Repeater {
                        model: 16
                        Rectangle {
                            id: cell
                            property var addr: rowDelegate.rowAddr + modelData
                            property int offset: rowDelegate.rowIndex * 16 + modelData
                            property bool isSelected: addr === root.selectedAddress
                            property bool isEditing: addr === root.editingAddress

                            Layout.preferredWidth: 28
                            Layout.fillHeight: true
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
                                    }
                                }

                                Keys.onReturnPressed: commit()
                                Keys.onEnterPressed: commit()
                                Keys.onEscapePressed: root.stopEdit()

                                onActiveFocusChanged: if (!activeFocus) Qt.callLater(commit)

                                function commit() {
                                    if (text.length > 0 && text !== originalValue) {
                                        root.byteModified(cell.addr, parseInt(text, 16))
                                    }
                                    root.stopEdit()
                                }
                            }

                            MouseArea {
                                anchors.fill: parent
                                enabled: !cell.isEditing
                                onClicked: {
                                    root.forceActiveFocus()
                                    root.selectedAddress = cell.addr
                                    root.addressSelected(cell.addr)
                                }
                                onDoubleClicked: {
                                    if (root.canEdit) root.startEdit(cell.addr)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}