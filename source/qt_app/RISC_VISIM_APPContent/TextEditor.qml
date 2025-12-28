import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "TextEditorController.js" as Logic

Rectangle {
    id: root

    // --- Public Properties ---
    property alias text: codeEditor.text
    property alias font: codeEditor.font
    readonly property bool hasContent: codeEditor.text.trim().length > 0
    property int highlightedLine: -1  // -1 means no highlight
    property color highlightColor: "#fcec0d"  // #fcec0d yellow highlight

    // Default size so it's visible
    implicitWidth: 400
    implicitHeight: 300

    color: "#ffffff"
    border.color: "#cccccc"
    border.width: 1

    // --- Internal State ---
    property var breakpointState: ({})

    // Calculate actual line height matching TextEdit's lineCount
    property real lineHeight: {
        if (codeEditor.lineCount === 0) return 20
        // Calculate based on actual content height divided by line count
        return codeEditor.contentHeight / Math.max(1, codeEditor.lineCount)
    }

    // --- Main Layout ---
    RowLayout {
        id: mainRow
        anchors.fill: parent
        spacing: 0

        // ------------------------------------------------
        // GUTTER AREA
        // ------------------------------------------------
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 60
            color: "#f5f5f5"
            border.color: "#e0e0e0"
            border.width: 1

            Flickable {
                id: gutterFlickable
                anchors.fill: parent
                contentHeight: gutterColumn.height
                interactive: false
                clip: true

                // contentY is now synced from editorFlickable.onContentYChanged

                Column {
                    id: gutterColumn
                    width: parent.width
                    spacing: 0

                    Repeater {
                        id: gutterRepeater
                        model: ListModel { id: internalGutterModel }

                        delegate: Rectangle {
                            width: gutterColumn.width
                            height: root.lineHeight
                            color: "transparent"

                            Row {
                                anchors.fill: parent
                                spacing: 0

                                // Breakpoint Interaction Zone
                                Item {
                                    width: 25
                                    height: parent.height

                                    // The Red Dot
                                    Rectangle {
                                        width: 12
                                        height: 12
                                        radius: 6
                                        color: "#ff4444"
                                        anchors.centerIn: parent
                                        visible: model.hasBreakpoint
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            root.breakpointState = Logic.toggleBreakpoint(root.breakpointState, index)
                                            Logic.updateGutterModel(internalGutterModel.count, root.breakpointState, internalGutterModel)
                                        }
                                    }
                                }

                                // Line Number
                                Text {
                                    width: 30
                                    height: parent.height
                                    text: model.lineNumber
                                    color: "#999999"
                                    font: codeEditor.font
                                    horizontalAlignment: Text.AlignRight
                                    verticalAlignment: Text.AlignVCenter
                                    rightPadding: 4
                                }
                            }
                        }
                    }
                }
            }
        }

        // ------------------------------------------------
        // TEXT EDITOR AREA
        // ------------------------------------------------
        Flickable {
            id: editorFlickable
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            contentWidth: codeEditor.contentWidth
            contentHeight: Math.max(codeEditor.contentHeight, editorFlickable.height)

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
            }
            ScrollBar.horizontal: ScrollBar {
                policy: ScrollBar.AsNeeded
            }

            // Sync gutter scrolling when editor scrolls
            onContentYChanged: {
                gutterFlickable.contentY = contentY
            }

            // Background MouseArea to catch clicks below text
            MouseArea {
                width: editorFlickable.width
                height: editorFlickable.contentHeight
                cursorShape: Qt.IBeamCursor
                onClicked: function(mouse) {
                    codeEditor.forceActiveFocus()
                    // Move cursor to end of text
                    codeEditor.cursorPosition = codeEditor.length
                }
            }

            // Line highlight rectangle
            Rectangle {
                id: lineHighlight
                visible: root.highlightedLine >= 0
                x: 0
                y: root.highlightedLine * root.lineHeight
                width: Math.max(codeEditor.contentWidth, editorFlickable.width)
                height: root.lineHeight
                color: root.highlightColor
                opacity: 0.6
            }

            TextEdit {
                id: codeEditor
                width: editorFlickable.width

                readOnly: backend.editorLocked

                textFormat: TextEdit.PlainText
                font.family: "Courier New"
                font.pointSize: 12

                // Critical: Remove all padding to align with gutter
                leftPadding: 10
                topPadding: 0
                bottomPadding: 0
                rightPadding: 0

                selectByMouse: true
                wrapMode: TextEdit.NoWrap

                onTextChanged: {
                    var lines = text.split("\n").length
                    if (lines !== internalGutterModel.count) {
                        Logic.updateGutterModel(lines, root.breakpointState, internalGutterModel)
                    }
                }

                // Auto-scroll on new line
                onCursorRectangleChanged: {
                    // Check if cursor is below visible area
                    if (cursorRectangle.y + cursorRectangle.height > editorFlickable.contentY + editorFlickable.height) {
                        editorFlickable.contentY = cursorRectangle.y + cursorRectangle.height - editorFlickable.height
                    }
                    // Check if cursor is above visible area
                    else if (cursorRectangle.y < editorFlickable.contentY) {
                        editorFlickable.contentY = cursorRectangle.y
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        Logic.updateGutterModel(1, root.breakpointState, internalGutterModel)
    }
}
