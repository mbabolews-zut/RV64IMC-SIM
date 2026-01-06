import QtQuick
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#f8f8fc"
    focus: true

    property var selectedAddress: -1
    property bool canEdit: false
    property var values: []

    signal valueEdited(var address, int typeIndex, string value)

    property int editingIndex: -1
    property var editingAddress: -1

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

        header: Rectangle {
            width: listView.width
            height: 20
            color: "#e8e8f0"

            Text {
                anchors.centerIn: parent
                text: root.selectedAddress >= 0
                    ? root.selectedAddress.toString(16).toUpperCase().padStart(16, '0')
                    : "Select address"
                color: "#990000"
                font.family: "monospace"
                font.pixelSize: 12
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
                        enabled: root.canEdit && root.selectedAddress >= 0
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
                    Keys.onEscapePressed: { root.editingIndex = -1; root.editingAddress = -1 }

                    onActiveFocusChanged: if (!activeFocus) Qt.callLater(commit)

                    function commit() {
                        if (text.length > 0 && root.editingAddress >= 0) {
                            root.valueEdited(root.editingAddress, rowDelegate.typeIndex, text)
                        }
                        root.editingIndex = -1
                        root.editingAddress = -1
                    }
                }
            }
        }
    }
}