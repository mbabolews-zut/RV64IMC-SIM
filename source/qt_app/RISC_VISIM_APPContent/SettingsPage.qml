import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: root
    spacing: 20

    readonly property color cText: "#2A3172"

    property alias title: titleLabel.text
    default property alias content: contentContainer.children

    Label {
        id: titleLabel
        font.bold: true
        color: root.cText
        font.pixelSize: 15
    }

    Item {
        id: contentContainer
        Layout.fillWidth: true
        Layout.fillHeight: true
        clip: true
        implicitHeight: childrenRect.height
        implicitWidth: childrenRect.width
    }
}
