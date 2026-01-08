import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import RISC_VISIM_APP

Rectangle {
    id: root
    width: Constants.width
    height: Constants.height
    color: Constants.backgroundColor

    property alias toolbar: toolbar
    property alias mainEditor: mainEditor
    property alias outputPanel: outputPanel
    property alias registerPanel: registerPanel
    readonly property bool editorHasContent: mainEditor.hasContent

    SettingsDialog {
        id: settingsDialog

        onSettingsApplied: {
            mainEditor.font.pixelSize = settingsDialog.editorFontSize
            mainEditor.highlightColor = settingsDialog.editorHighlightColor
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Toolbar {
            id: toolbar
            Layout.fillWidth: true
            Layout.preferredHeight: 56
            editorHasContent: mainEditor.hasContent

            settingsButton.onClicked: settingsDialog.open()
        }

        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal

            handle: Rectangle {
                implicitWidth: 4
                implicitHeight: 4
                color: SplitHandle.hovered ? "#e4e0d0" : "#e8e4d4"
            }

            SplitView {
                SplitView.fillWidth: true
                SplitView.minimumWidth: 300
                orientation: Qt.Vertical

                handle: Rectangle {
                    implicitWidth: 4
                    implicitHeight: 4
                    color: SplitHandle.hovered ? "#e4e0d0" : "#e8e4d4"
                }

                TextEditor {
                    id: mainEditor
                    highlightedLine: backend.currentLine
                    SplitView.fillHeight: true
                    SplitView.fillWidth: true
                    SplitView.minimumHeight: 150
                    font.family: "monospace"
                    font.pixelSize: settingsDialog.editorFontSize
                }

                OutputPanel {
                    id: outputPanel
                    SplitView.preferredHeight: 180
                    SplitView.minimumHeight: 80
                }
            }

            RegisterPanel {
                id: registerPanel
                SplitView.preferredWidth: 330
                SplitView.minimumWidth: 320
            }
        }
    }
}
