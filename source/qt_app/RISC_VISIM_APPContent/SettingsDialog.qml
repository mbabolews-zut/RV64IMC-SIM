import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Basic

Dialog {
    id: root
    width: 600
    height: 450
    modal: true
    padding: 0
    anchors.centerIn: Overlay.overlay

    property int activeTab: 0

    // --- PALETTE ---
    readonly property color cBack:       "#FFFFFF"
    readonly property color cSidebar:    "#F5F7FA"
    readonly property color cHeader:     "#2A3172"
    readonly property color cText:       "#2A3172"
    readonly property color cAccent:     "#CC5500"
    readonly property color cBorder:     "#DCE0E8"
    readonly property color cMuted:      "#5C6580"

    // Exposed settings values (bind to page properties)
    property alias editorFontSize: editorPage.fontSize
    property alias editorHighlightColor: editorPage.highlightColor

    signal settingsApplied()

    function applySettings() {
        settingsManager.apply()
        settingsApplied()
    }

    function cancelSettings() {
        settingsManager.cancel()
    }

    //onOpened: settingsManager.loadFromVM()

    background: Rectangle {
        color: root.cBack
        border.color: root.cBorder
        radius: 6
    }

    // --- HEADER WITH SHADOW ---
    header: Rectangle {
        height: 48
        color: root.cHeader
        radius: 6
        Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 10; color: parent.color }

        Label {
            id: sourceLabel
            text: "SETTINGS"
            visible: false
            font.family: "Jura"; font.pixelSize: 13; font.letterSpacing: 2; font.bold: true
        }

        Label {
            text: "SETTINGS"
            anchors.centerIn: parent
            font.family: "Jura"; font.pixelSize: 13; font.letterSpacing: 2; font.bold: true
            color: "white"
        }
    }

    contentItem: RowLayout {
        spacing: 0

        // Sidebar
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 160
            color: root.cSidebar
            Rectangle { anchors.right: parent.right; width: 1; height: parent.height; color: root.cBorder }

            ColumnLayout {
                anchors.fill: parent
                anchors.topMargin: 10
                spacing: 2
                ButtonGroup { id: navGroup }

                Repeater {
                    model: tabModel
                    delegate: Button {
                        id: navBtn
                        checkable: true
                        checked: root.activeTab === index
                        ButtonGroup.group: navGroup
                        Layout.fillWidth: true
                        implicitHeight: 45
                        onClicked: root.activeTab = index

                        contentItem: Text {
                            text: modelData
                            font.family: "Jura"
                            font.pixelSize: 13
                            color: navBtn.checked ? root.cAccent : root.cMuted
                            font.bold: navBtn.checked
                            leftPadding: 20
                            verticalAlignment: Text.AlignVCenter
                        }

                        background: Rectangle {
                            color: navBtn.checked ? "white" : "transparent"
                            Rectangle { visible: navBtn.checked; width: 4; height: parent.height; color: root.cAccent }
                            Rectangle { visible: navBtn.checked; anchors.right: parent.right; anchors.rightMargin: -1; width: 2; height: parent.height; color: "white" }
                        }
                    }
                }

                Item { Layout.fillHeight: true }
            }
        }

        // Main Content
        StackLayout {
            id: mainStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.margins: 30
            currentIndex: root.activeTab

            EditorSettingsPage { id: editorPage }
            MemorySettingsPage { id: memoryPage }
            SystemSettingsPage { id: systemPage }
        }
    }

    footer: Pane {
        padding: 15
        background: Rectangle {
            color: root.cSidebar
            Rectangle { width: parent.width; height: 1; color: root.cBorder; anchors.top: parent.top }
        }
        contentItem: RowLayout {
            spacing: 12
            Item { Layout.fillWidth: true }

            Repeater {
                model: [
                    { text: "Cancel", highlighted: false, action: function() { root.cancelSettings(); root.reject() } },
                    { text: "Apply",  highlighted: false, action: function() { root.applySettings() } },
                    { text: "OK",     highlighted: true,  action: function() { root.applySettings(); root.accept() } }
                ]
                delegate: Button {
                    id: footerBtn
                    text: modelData.text
                    implicitWidth: 90
                    implicitHeight: 32
                    onClicked: modelData.action()

                    contentItem: Text {
                        text: footerBtn.text
                        font.pixelSize: 12
                        font.bold: true
                        color: modelData.highlighted ? "white" : root.cText
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        color: modelData.highlighted ? root.cAccent : (footerBtn.hovered ? "white" : "transparent")
                        border.color: modelData.highlighted ? root.cAccent : (footerBtn.hovered ? root.cAccent : root.cBorder)
                        radius: 4
                    }
                }
            }
        }
    }

    // Tab configuration - add new tabs here
    readonly property var tabModel: ["Text Editor", "Memory", "System"]
}
