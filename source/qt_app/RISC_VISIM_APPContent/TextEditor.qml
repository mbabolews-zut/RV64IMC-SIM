import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property alias text: codeEditor.text
    property alias font: codeEditor.font
    readonly property bool hasContent: codeEditor.text.trim().length > 0
    property int highlightedLine: -1
    property color highlightColor: "#fcec0d"

    implicitWidth: 400
    implicitHeight: 300
    color: "#ffffff"
    border.color: "#cccccc"
    border.width: 1

    readonly property real lineHeight: codeEditor.lineCount > 0
        ? codeEditor.contentHeight / codeEditor.lineCount
        : 20

    property var breakpointLines: ({})
    property int _lastLength: 0

    signal edited()

    function resetLengthTracking() {
        _lastLength = codeEditor.text.length
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 60
            color: "#f5f5f5"
            border.color: "#e0e0e0"
            border.width: 1

            Flickable {
                id: gutterFlickable
                anchors.fill: parent
                contentY: editorFlickable.contentY
                contentHeight: gutterColumn.height
                interactive: false
                clip: true

                Column {
                    id: gutterColumn
                    width: parent.width

                    Repeater {
                        model: Math.max(1, codeEditor.lineCount)

                        Rectangle {
                            id: gutterLine
                            required property int index
                            width: gutterColumn.width
                            height: root.lineHeight
                            color: "transparent"

                            Row {
                                anchors.fill: parent

                                Item {
                                    width: 25
                                    height: parent.height

                                    Rectangle {
                                        width: 12
                                        height: 12
                                        radius: 6
                                        color: "#ff4444"
                                        anchors.centerIn: parent
                                        visible: root.breakpointLines[gutterLine.index] === true
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: backend.toggleBreakpoint(gutterLine.index)
                                    }
                                }

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

        Flickable {
            id: editorFlickable
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            interactive: true
            contentWidth: Math.max(codeEditor.contentWidth + 20, width)
            contentHeight: Math.max(codeEditor.contentHeight, height)
            boundsBehavior: Flickable.StopAtBounds
            flickableDirection: Flickable.AutoFlickIfNeeded

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded
                interactive: true
            }
            ScrollBar.horizontal: ScrollBar {
                policy: ScrollBar.AsNeeded
                interactive: true
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
                width: Math.max(contentWidth + 20, editorFlickable.width)
                height: Math.max(contentHeight, editorFlickable.height)
                readOnly: backend.editorLocked
                textFormat: TextEdit.PlainText
                font.family: "monospace"
                font.pixelSize: 15
                leftPadding: 10
                selectByMouse: true
                wrapMode: TextEdit.NoWrap

                Keys.onPressed: (event) => {
                    if (readOnly) return
                    // Detect text-modifying keys
                    if (event.text.length > 0 ||
                        event.key === Qt.Key_Backspace ||
                        event.key === Qt.Key_Delete ||
                        event.key === Qt.Key_Return ||
                        event.key === Qt.Key_Enter ||
                        event.key === Qt.Key_Tab ||
                        (event.modifiers & Qt.ControlModifier && (event.key === Qt.Key_V || event.key === Qt.Key_X))) {
                        root.edited()
                    }
                }

                onTextChanged: {
                    // Detect length changes (e.g., context menu paste)
                    if (text.length !== root._lastLength) {
                        root._lastLength = text.length
                        root.edited()
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.IBeamCursor
                    acceptedButtons: Qt.NoButton
                    onWheel: (wheel) => {
                        if (wheel.modifiers & Qt.ShiftModifier) {
                            // Horizontal scroll with Shift+wheel
                            let delta = wheel.angleDelta.y / 3
                            editorFlickable.contentX = Math.max(0,
                                Math.min(editorFlickable.contentWidth - editorFlickable.width,
                                    editorFlickable.contentX - delta))
                        } else {
                            // Vertical scroll
                            let delta = wheel.angleDelta.y / 3
                            editorFlickable.contentY = Math.max(0,
                                Math.min(editorFlickable.contentHeight - editorFlickable.height,
                                    editorFlickable.contentY - delta))
                        }
                    }
                }

                onCursorRectangleChanged: {
                    let cr = cursorRectangle
                    if (cr.y + cr.height > editorFlickable.contentY + editorFlickable.height)
                        editorFlickable.contentY = cr.y + cr.height - editorFlickable.height
                    else if (cr.y < editorFlickable.contentY)
                        editorFlickable.contentY = cr.y

                    if (cr.x + cr.width > editorFlickable.contentX + editorFlickable.width)
                        editorFlickable.contentX = cr.x + cr.width - editorFlickable.width + 20
                    else if (cr.x < editorFlickable.contentX)
                        editorFlickable.contentX = Math.max(0, cr.x - 20)
                }
            }
        }
    }

    Connections {
        target: backend
        function onBreakpointToggled(line, enabled) {
            let updated = Object.assign({}, root.breakpointLines)
            if (enabled)
                updated[line] = true
            else
                delete updated[line]
            root.breakpointLines = updated
        }
    }
}
