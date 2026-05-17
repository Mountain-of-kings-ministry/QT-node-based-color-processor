import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MOKM_Color_Processor

Rectangle {
    id: root

    visible: true
    width: 640
    height: 480

    color: Theme.surface
    border.color: Theme.border
    border.width: 1
    radius: 6

    // Title bar for dragging
    Rectangle {
        id: titleBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 32
        color: Theme.secondary
        radius: 6

        Text {
            id: titleText
            text: "Preview"
            color: Theme.foreground
            font.pixelSize: 13
            font.bold: true
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: _displayHelper ? _displayHelper.info : ""
            color: Theme.foregroundSubtle
            font.pixelSize: 10
            anchors.left: titleText.right
            anchors.leftMargin: 12
            anchors.verticalCenter: parent.verticalCenter
        }

        // Close button
        Button {
            anchors.right: parent.right
            anchors.rightMargin: 4
            anchors.verticalCenter: parent.verticalCenter
            width: 22
            height: 22
            text: "\u2715"
            font.pixelSize: 10
            flat: true
            onClicked: root.close()

            background: Rectangle {
                color: parent.hovered ? Theme.errorSoft : "transparent"
                radius: 4
            }

            contentItem: Text {
                text: parent.text
                color: parent.hovered ? Theme.error : Theme.foregroundSubtle
                font: parent.font
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        // Drag area
        MouseArea {
            anchors.fill: parent
            anchors.rightMargin: 28
            drag.target: root
            drag.axis: Drag.XAndYAxis
            cursorShape: Qt.OpenHandCursor
        }
    }

    // Image area
    Rectangle {
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "#0B1120"
        clip: true

        Image {
            id: previewImage
            anchors.centerIn: parent
            width: parent.width
            height: parent.height
            fillMode: Image.PreserveAspectFit
            mipmap: true
            source: _displayHelper && _displayHelper.refreshToken !== "" ? "image://mokmpreview/display?" + _displayHelper.refreshToken : ""
        }

        Text {
            anchors.centerIn: parent
            text: "No image connected"
            color: Theme.mutedForeground
            font.pixelSize: 14
            visible: previewImage.status !== Image.Ready
        }
    }

    // Resize handle (bottom-right corner)
    Item {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        width: 14
        height: 14

        Rectangle {
            x: 2; y: 8; width: 10; height: 2; color: Theme.mutedForeground; radius: 1
        }
        Rectangle {
            x: 2; y: 10; width: 8; height: 2; color: Theme.mutedForeground; radius: 1
        }
        Rectangle {
            x: 2; y: 4; width: 10; height: 2; color: Theme.mutedForeground; radius: 1
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.SizeFDiagCursor
            onPositionChanged: {
                root.width = Math.max(300, root.width + mouse.x)
                root.height = Math.max(200, root.height + mouse.y)
            }
        }
    }

    function close() {
        root.visible = false
    }
}
