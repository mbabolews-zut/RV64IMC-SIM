import QtQuick
import QtQuick.Window

Window {
    id: mainWindow
    minimumWidth: 800
    minimumHeight: 500
    width: 1300
    height: 800
    visible: true
    title: backend.windowTitle

    // Keyboard shortcuts
    Shortcut {
        sequence: "Ctrl+S"
        onActivated: {
            if (backend.currentFile !== "")
                backend.saveFile(screen.mainEditor.text)
            else
                screen.toolbar.openSaveAsDialog()
        }
    }

    Shortcut {
        sequence: "Ctrl+B"
        onActivated: {
            if (screen.mainEditor.hasContent) {
                if (!backend.buildingEnabled)
                    backend.reset()
                backend.build(screen.mainEditor.text)
            }
        }
    }

    Shortcut {
        sequence: "F7"
        onActivated: {
            if (!backend.runLocked)
                backend.step()
        }
    }

    Shortcut {
        sequence: "F5"
        onActivated: {
            if (!backend.runLocked)
                backend.run()
        }
    }

    Screen01 {
        id: screen
        anchors.fill: parent

        Connections {
            target: screen.toolbar
            function onBuildRequested() {
                backend.build(screen.mainEditor.text)
            }
            function onNewFile() {
                backend.newFile()
            }
            function onOpenFile(fileUrl) {
                backend.openFile(fileUrl)
            }
            function onSaveFile() {
                backend.saveFile(screen.mainEditor.text)
            }
            function onSaveFileAs(fileUrl) {
                backend.saveFileAs(fileUrl, screen.mainEditor.text)
            }
        }

        Connections {
            target: backend
            function onFileLoaded(content) {
                screen.mainEditor._lastLength = content.length
                screen.mainEditor.text = content
            }
            function onFileCleared() {
                screen.mainEditor._lastLength = 0
                screen.mainEditor.text = ""
            }
        }

        Connections {
            target: screen.mainEditor
            function onEdited() {
                backend.setFileModified(true)
            }
        }
    }
}