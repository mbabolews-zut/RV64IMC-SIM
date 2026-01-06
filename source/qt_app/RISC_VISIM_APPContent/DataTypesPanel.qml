import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#f8f8fc"
    focus: true

    property string selectedAddress: ""  // hex string or empty
    property bool canEdit: false
    property var values: []

    signal valueEdited(string address, int typeIndex, string value)

    property int editingIndex: -1
    property string editingAddress: ""

    ListModel {
        id: typesModel
        ListElement { name: "bin8" }
        ListElement { name: "i8" }
        ListElement { name: "u8" }
        ListElement { name: "i16" }
        ListElement { name: "u16" }
        ListElement { name: "i32" }
        ListElement { name: "u32" }
        ListElement { name: "i64" }
        ListElement { name: "u64" }
    }

    ListView {
        id: listView
        anchors.fill: parent
        clip: true
        spacing: 0

        header: Column {
            width: listView.width
            spacing: 0

            Rectangle {
                width: parent.width
                height: 20
                color: "#e8e8f0"

                Text {
                    anchors.centerIn: parent
                    text: root.selectedAddress !== "" ? root.selectedAddress : "Select address"
                    color: "#990000"
                    font.family: "monospace"
                    font.pixelSize: 12
                }
            }

            Rectangle {
                width: parent.width
                height: 16
                color: "#d8d8e8"

                Text {
                    anchors.centerIn: parent
                    text: memoryController.isLittleEndian ? "LE (little-endian)" : "BE (big-endian)"
                    color: "#666688"
                    font.family: "monospace"
                    font.pixelSize: 10
                }
            }
        }

        model: typesModel

        delegate: Rectangle {
            id: rowDelegate
            width: listView.width
            height: 24
            color: index % 2 === 0 ? "#f8f8fc" : "#ffffff"

            property int typeIndex: index
            property string displayValue: typeIndex < root.values.length ? root.values[typeIndex] : "-"
            property bool isEditing: root.editingIndex === typeIndex

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 4
                anchors.rightMargin: 4
                spacing: 4

                Text {
                    Layout.preferredWidth: 36
                    text: model.name
                    color: "#666688"
                    font.family: "monospace"
                    font.pixelSize: 12
                }

                Text {
                    Layout.fillWidth: true
                    visible: !rowDelegate.isEditing
                    text: rowDelegate.displayValue
                    color: "#333333"
                    font.family: "monospace"
                    font.pixelSize: 12
                    elide: Text.ElideRight
                    horizontalAlignment: Text.AlignRight

                    MouseArea {
                        anchors.fill: parent
                        enabled: root.canEdit && root.selectedAddress !== ""
                        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                        onClicked: root.forceActiveFocus()
                        onDoubleClicked: {
                            root.editingAddress = root.selectedAddress
                            root.editingIndex = rowDelegate.typeIndex
                        }
                    }
                }

                TextInput {
                    id: editInput
                    Layout.fillWidth: true
                    visible: rowDelegate.isEditing
                    horizontalAlignment: Text.AlignRight
                    font.family: "monospace"
                    font.pixelSize: 12
                    selectByMouse: true

                    onVisibleChanged: {
                        if (visible) {
                            text = rowDelegate.displayValue
                            selectAll()
                            forceActiveFocus()
                        }
                    }

                    Keys.onReturnPressed: commit()
                    Keys.onEnterPressed: commit()
                    Keys.onEscapePressed: { root.editingIndex = -1; root.editingAddress = "" }

                    onActiveFocusChanged: if (!activeFocus) Qt.callLater(commit)

                    function commit() {
                        if (text.length > 0 && root.editingAddress !== "") {
                            root.valueEdited(root.editingAddress, rowDelegate.typeIndex, text)
                        }
                        root.editingIndex = -1
                        root.editingAddress = ""
                    }
                }
            }
        }
    }
}