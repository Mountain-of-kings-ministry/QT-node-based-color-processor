import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import MOKM_Color_Processor

Window {
    id: splash
    width: 640
    height: 480
    visible: true
    title: qsTr("Starting MOKM Color Processor")

    color: Theme.background
    flags: Qt.FramelessWindowHint

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 16

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 120
            height: 120
            color: Theme.primary
            radius: 12
            border.color: Theme.primaryBorder

            Text {
                anchors.centerIn: parent
                text: "MOKM"
                color: Theme.background
                font.pixelSize: 28
                font.bold: true
            }
        }

        Text {
            text: qsTr("MOKM Color Processor")
            color: Theme.foreground
            font.pixelSize: 20
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            text: qsTr("Node-based color management")
            color: Theme.foregroundMuted
            font.pixelSize: 13
            horizontalAlignment: Text.AlignHCenter
        }
    }

    Loader {
        id: mainLoader
        anchors.fill: parent
        visible: false
    }

    Timer {
        interval: 1400
        running: true
        repeat: false
        onTriggered: {
            mainLoader.source = "Main.qml";
            mainLoader.asynchronous = false;
        }
    }

    Connections {
        target: mainLoader
        function onStatusChanged() {
            if (mainLoader.status === Loader.Ready && mainLoader.item && mainLoader.item.visible) {
                splash.visible = false;
            }
        }
    }
}
