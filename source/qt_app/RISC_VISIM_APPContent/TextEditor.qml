import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    // --- Public Properties ---
    property alias text: codeEditor.text
    property alias font: codeEditor.font
    readonly property bool hasContent: codeEditor.text.trim().length > 0
    property int highlightedLine: -1  // -1 means no highlight
    property color highlightColor: "#fcec0d"  // #fcec0d yellow highlight

    // Breakpoint state - single source of truth
    property var breakpoints: ({})

    // Default size so it's visible
    implicitWidth: 400
    implicitHeight: 300

    color: "#ffffff"
    border.color: "#cccccc"
    border.width: 1

    // Calculate actual line height matching TextEdit's lineCount
    property real lineHeight: {
        if (codeEditor.lineCount === 0) return 20
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

                Column {
                    id: gutterColumn
                    width: parent.width
                    spacing: 0

                    Repeater {
                        id: gutterRepeater
                        model: codeEditor.lineCount > 0 ? codeEditor.lineCount : 1

                        delegate: Rectangle {
                            id: gutterLine
                            required property int index
                            property bool hasBreakpoint: root.breakpoints[index] === true

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
                                        visible: gutterLine.hasBreakpoint
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            backend.toggleBreakpoint(gutterLine.index)
                                        }
                                    }
                                }

                                // Line Number
                                Text {
                                    width: 30
                                    height: parent.height
                                    text: gutterLine.index + 1
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
            interactive: false

            contentWidth: codeEditor.contentWidth
            contentHeight: Math.max(codeEditor.contentHeight, editorFlickable.height)

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
                interactive: true
            }
            ScrollBar.horizontal: ScrollBar {
                policy: ScrollBar.AsNeeded
                interactive: true
            }

            onContentYChanged: {
                gutterFlickable.contentY = contentY
            }

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
                height: Math.max(contentHeight, editorFlickable.height)

                readOnly: backend.editorLocked

                textFormat: TextEdit.PlainText
                font.family: "Courier New"
                font.pointSize: 12

                leftPadding: 10
                topPadding: 0
                bottomPadding: 0
                rightPadding: 0

                selectByMouse: true
                wrapMode: TextEdit.NoWrap

                onCursorRectangleChanged: {
                    if (cursorRectangle.y + cursorRectangle.height > editorFlickable.contentY + editorFlickable.height) {
                        editorFlickable.contentY = cursorRectangle.y + cursorRectangle.height - editorFlickable.height
                    }
                    else if (cursorRectangle.y < editorFlickable.contentY) {
                        editorFlickable.contentY = cursorRectangle.y
                    }

                    if (cursorRectangle.x + cursorRectangle.width > editorFlickable.contentX + editorFlickable.width) {
                        editorFlickable.contentX = cursorRectangle.x + cursorRectangle.width - editorFlickable.width + 20
                    }
                    else if (cursorRectangle.x < editorFlickable.contentX) {
                        editorFlickable.contentX = Math.max(0, cursorRectangle.x - 20)
                    }
                }
            }
        }
    }

    Connections {
        target: backend
        function onBreakpointToggled(line, enabled) {
            var newBreakpoints = Object.assign({}, root.breakpoints)
            if (enabled) {
                newBreakpoints[line] = true
            } else {
                delete newBreakpoints[line]
            }
            root.breakpoints = newBreakpoints
        }
    }
}
