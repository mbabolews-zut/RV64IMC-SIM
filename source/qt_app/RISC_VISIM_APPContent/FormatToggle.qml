import QtQuick
import QtQuick.Controls

TabBar {
    id: root

    enum Format { Decimal, Hex, Binary }

    property int selectedFormat: FormatToggle.Format.Decimal

    spacing: 2

    TabButton {
        id: decBtn
        text: "Dec"
        checkable: true
        checked: root.selectedFormat === FormatToggle.Format.Decimal
        implicitWidth: 50
        implicitHeight: 28
        onClicked: {
            root.selectedFormat = FormatToggle.Format.Decimal
            backend.setRegDisplayFormat(10)
        }

        background: Rectangle {
            color: decBtn.checked ? "#2E43FF" : "#6B6B6B"
            topLeftRadius: 7
            bottomLeftRadius: 7
        }

        contentItem: Text {
            text: decBtn.text
            color: "white"
            font.pointSize: 10
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    TabButton {
        id: hexBtn
        text: "Hex"
        checkable: true
        checked: root.selectedFormat === FormatToggle.Format.Hex
        implicitWidth: 50
        implicitHeight: 28
        onClicked: {
            root.selectedFormat = FormatToggle.Format.Hex
            backend.setRegDisplayFormat(16)
        }

        background: Rectangle {
            color: hexBtn.checked ? "#2E43FF" : "#6B6B6B"
        }

        contentItem: Text {
            text: hexBtn.text
            color: "white"
            font.pointSize: 10
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    TabButton {
        id: binBtn
        text: "Bin"
        checkable: true
        checked: root.selectedFormat === FormatToggle.Format.Binary
        implicitWidth: 50
        implicitHeight: 28
        onClicked: {
            root.selectedFormat = FormatToggle.Format.Binary
            backend.setRegDisplayFormat(2)
        }

        background: Rectangle {
            color: binBtn.checked ? "#2E43FF" : "#6B6B6B"
            topRightRadius: 7
            bottomRightRadius: 7
        }

        contentItem: Text {
            text: binBtn.text
            color: "white"
            font.pointSize: 10
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}

