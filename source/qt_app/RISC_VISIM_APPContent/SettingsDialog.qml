import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: root
    title: "Settings"
    width: 500
    height: 400
    minimumWidth: 400
    minimumHeight: 300
    modality: Qt.ApplicationModal
    flags: Qt.Dialog

    property int editorFontSize: 12
    property int stackSizeKB: 64
    property string highlightColor: "#fcec0d"

    signal settingsApplied()

    function applySettings() {
        editorFontSize = fontSizeSpinBox.value
        stackSizeKB = stackSizeSpinBox.value
        highlightColor = colorInput.text
        settingsApplied()
    }

    onVisibleChanged: {
        if (visible) {
            fontSizeSpinBox.value = editorFontSize
            stackSizeSpinBox.value = stackSizeKB
            colorInput.text = highlightColor
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        TabBar {
            id: tabBar
            Layout.fillWidth: true

            TabButton { text: "Editor" }
            TabButton { text: "Memory" }
            TabButton { text: "About" }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex

            // Editor Tab
            ScrollView {
                contentWidth: availableWidth

                Pane {
                    anchors.fill: parent

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 20

                        GridLayout {
                            columns: 2
                            columnSpacing: 20
                            rowSpacing: 12
                            Layout.fillWidth: true

                            Label {
                                text: "Font Size:"
                                Layout.alignment: Qt.AlignRight
                            }
                            SpinBox {
                                id: fontSizeSpinBox
                                from: 8
                                to: 32
                                value: root.editorFontSize
                                editable: true
                            }

                            Label {
                                text: "Font Family:"
                                Layout.alignment: Qt.AlignRight
                            }
                            ComboBox {
                                id: fontFamilyCombo
                                model: ["Courier New", "Consolas", "Monaco", "Monospace"]
                                Layout.preferredWidth: 150
                            }

                            Label {
                                text: "Highlight Color:"
                                Layout.alignment: Qt.AlignRight
                            }
                            RowLayout {
                                spacing: 10
                                TextField {
                                    id: colorInput
                                    text: root.highlightColor
                                    Layout.preferredWidth: 100
                                    validator: RegularExpressionValidator {
                                        regularExpression: /^#[0-9A-Fa-f]{6}$/
                                    }
                                }
                                Rectangle {
                                    width: 28
                                    height: 28
                                    color: colorInput.acceptableInput ? colorInput.text : "#cccccc"
                                    border.color: "#666666"
                                    border.width: 1
                                    radius: 4
                                }
                            }

                            Label {
                                text: "Tab Width:"
                                Layout.alignment: Qt.AlignRight
                            }
                            SpinBox {
                                id: tabWidthSpinBox
                                from: 2
                                to: 8
                                value: 4
                                editable: true
                            }
                        }

                        Item { Layout.fillHeight: true }
                    }
                }
            }

            // Memory Tab
            ScrollView {
                contentWidth: availableWidth

                Pane {
                    anchors.fill: parent

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 20

                        GridLayout {
                            columns: 2
                            columnSpacing: 20
                            rowSpacing: 12
                            Layout.fillWidth: true

                            Label {
                                text: "Stack Size:"
                                Layout.alignment: Qt.AlignRight
                            }
                            RowLayout {
                                spacing: 10
                                SpinBox {
                                    id: stackSizeSpinBox
                                    from: 16
                                    to: 4096
                                    stepSize: 16
                                    value: root.stackSizeKB
                                    editable: true
                                }
                                Label { text: "KB" }
                            }

                            Label {
                                text: "Data Segment Base:"
                                Layout.alignment: Qt.AlignRight
                            }
                            TextField {
                                text: "0x400000"
                                readOnly: true
                                Layout.preferredWidth: 120
                            }

                            Label {
                                text: "Stack Base:"
                                Layout.alignment: Qt.AlignRight
                            }
                            TextField {
                                text: "0x7FF00000"
                                readOnly: true
                                Layout.preferredWidth: 120
                            }
                        }

                        Label {
                            text: "Note: Memory layout changes apply on next build."
                            font.italic: true
                            color: "#666666"
                        }

                        Item { Layout.fillHeight: true }
                    }
                }
            }

            // About Tab
            ScrollView {
                contentWidth: availableWidth

                Pane {
                    anchors.fill: parent

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 16

                        Label {
                            text: "RISC-V 64 Simulator"
                            font.pointSize: 18
                            font.bold: true
                        }

                        Label {
                            text: "Version 1.0"
                            font.pointSize: 12
                        }

                        Label {
                            text: "A visual RISC-V 64-bit instruction set simulator with assembly editor, memory viewer, and register inspection."
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }

                        Item { Layout.preferredHeight: 20 }

                        Label {
                            text: "Features:"
                            font.bold: true
                        }

                        Label {
                            text: "• Assembly code editor with syntax highlighting\n• Step-by-step execution with breakpoints\n• Memory viewer with hex and ASCII editing\n• Register display in Dec/Hex/Bin formats\n• Data type interpretation panel"
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                        }

                        Item { Layout.fillHeight: true }
                    }
                }
            }
        }

        // Bottom buttons
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#f0f0f0"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                Button {
                    text: "Reset to Defaults"
                    onClicked: {
                        fontSizeSpinBox.value = 12
                        stackSizeSpinBox.value = 64
                        colorInput.text = "#fcec0d"
                        tabWidthSpinBox.value = 4
                        fontFamilyCombo.currentIndex = 0
                    }
                }

                Item { Layout.fillWidth: true }

                Button {
                    text: "Cancel"
                    onClicked: root.close()
                }

                Button {
                    text: "Apply"
                    onClicked: root.applySettings()
                }

                Button {
                    text: "OK"
                    highlighted: true
                    onClicked: {
                        root.applySettings()
                        root.close()
                    }
                }
            }
        }
    }
}
