
/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
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
            mainEditor.font.pointSize = settingsDialog.editorFontSize
            mainEditor.highlightColor = settingsDialog.highlightColor
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

            settingsButton.onClicked: settingsDialog.show()
        }

        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal

            SplitView {
                SplitView.fillWidth: true
                SplitView.minimumWidth: 300
                orientation: Qt.Vertical

                TextEditor {
                    id: mainEditor
                    highlightedLine: backend.currentLine
                    SplitView.fillHeight: true
                    SplitView.fillWidth: true
                    SplitView.minimumHeight: 150
                    font.family: "Courier New"
                    font.pointSize: settingsDialog.editorFontSize
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
