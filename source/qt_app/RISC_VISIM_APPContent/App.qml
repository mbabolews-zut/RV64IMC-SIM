import QtQuick
import QtQuick.Window

Window {
    minimumWidth: 1400
    minimumHeight: 800
    visible: true

    Screen01 {
        id: screen
        anchors.fill: parent

        Connections {
            target: screen.toolbar
            function onBuildRequested() {
                backend.build(screen.mainEditor.text)
            }
        }
    }
}
