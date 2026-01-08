import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Rectangle {
    id: root

    property alias settingsButton: settingsBtn
    property alias buildButton: buildBtn
    property alias resetButton: resetBtn
    property alias stepButton: stepBtn
    property alias runButton: runBtn
    property bool editorHasContent: false
    property bool showStopButton: false

    signal buildRequested()
    signal newFile()
    signal openFile(url fileUrl)
    signal saveFile()
    signal saveFileAs(url fileUrl)

    function openSaveAsDialog() {
        saveFileDialog.open()
    }

    color: "#f6b21a"

    Timer {
        id: runButtonDelayTimer
        interval: 20
        repeat: false
        onTriggered: {
            root.showStopButton = true
        }
    }

    Connections {
        target: backend
        function onAppStateChanged(newState) {
            if (newState === 3) {  // AppState::Running
                runButtonDelayTimer.start()
            } else {
                runButtonDelayTimer.stop()
                root.showStopButton = false
            }
        }
    }

    FileDialog {
        id: openFileDialog
        title: "Open Assembly File"
        nameFilters: ["Assembly files (*.s *.asm)", "All files (*)"]
        onAccepted: root.openFile(selectedFile)
    }

    FileDialog {
        id: saveFileDialog
        title: "Save Assembly File"
        fileMode: FileDialog.SaveFile
        nameFilters: ["Assembly files (*.s *.asm)", "All files (*)"]
        onAccepted: root.saveFileAs(selectedFile)
    }

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
                onClicked: fileMenu.open()

                Menu {
                    id: fileMenu
                    y: fileBtn.height

                    MenuItem {
                        text: "New"
                        onTriggered: root.newFile()
                    }
                    MenuItem {
                        text: "Open..."
                        onTriggered: openFileDialog.open()
                    }
                    MenuItem {
                        text: "Save As..."
                        onTriggered: saveFileDialog.open()
                    }
                }
            }

            NavButton {
                id: saveBtn
                text: "Save"
                Layout.preferredHeight: 40
                enabled: backend.fileModified
                onClicked: {
                    if (backend.currentFile !== "")
                        root.saveFile()
                    else
                        saveFileDialog.open()
                }
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
                text: root.showStopButton ? "||︎ Stop" : "▶▶ Run"
                enabled: !backend.runLocked || root.showStopButton
                Layout.preferredHeight: 40
                onClicked: {
                    if (root.showStopButton) {
                        backend.stop()
                    } else {
                        backend.run()
                    }
                }
            }
        }
    }
}