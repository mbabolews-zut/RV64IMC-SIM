import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    property alias fileButton: fileBtn
    property alias saveButton: saveBtn
    property alias settingsButton: settingsBtn
    property alias buildButton: buildBtn
    property alias resetButton: resetBtn
    property alias stepButton: stepBtn
    property alias runButton: runBtn
    property bool editorHasContent: false

    signal buildRequested()

    color: "#f6b21a"

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        RowLayout {
            spacing: 10

            NavButton {
                id: fileBtn
                text: "File ▼"
                Layout.preferredHeight: 40
            }

            NavButton {
                id: saveBtn
                text: "Save"
                Layout.preferredHeight: 40
            }

            NavButton {
                id: settingsBtn
                text: "Settings"
                Layout.preferredHeight: 40
            }
        }

        Item { Layout.fillWidth: true }

        RowLayout {
            spacing: 10

            NavButton {
                id: buildBtn
                text: "Build"
                Layout.preferredHeight: 40
                enabled: root.editorHasContent && backend.buildingEnabled
                onClicked: {
                    root.buildRequested()
                }
            }

            NavButton {
                id: resetBtn
                text: "↻ Reset"
                Layout.preferredHeight: 40
                enabled: !backend.resetLocked
                onClicked: backend.reset()
            }

            NavButton {
                id: stepBtn
                text: "▶| Step"
                enabled: !backend.runLocked
                Layout.preferredHeight: 40
                onClicked: backend.step()
            }

            NavButton {
                id: runBtn
                text: "▶▶ Run"
                enabled: !backend.runLocked
                Layout.preferredHeight: 40
                onClicked: backend.run()
            }
        }
    }
}

