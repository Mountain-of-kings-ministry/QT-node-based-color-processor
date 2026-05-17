import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import NodeEditor
import MOKM_Color_Processor

Window {
    id: root
    width: 1280
    height: 800
    visible: true
    title: qsTr("MOKM Color Processor")

    color: Theme.background

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Left sidebar
        Rectangle {
            Layout.preferredWidth: 48
            Layout.fillHeight: true
            color: Theme.surface
            border.width: 1
            border.color: Theme.border

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                ToolButton {
                    Layout.preferredWidth: 48
                    Layout.preferredHeight: 48
                    icon.source: "qrc:/icons/outline/folder.svg"
                    icon.color: Theme.foregroundMuted
                    hoverEnabled: true
                    background: Rectangle {
                        color: parent.hovered ? Theme.surfaceHover : "transparent"
                    }
                    ToolTip.visible: hovered
                    ToolTip.text: "Open Config"
                    onClicked: configDialog.open()
                }

                ToolButton {
                    Layout.preferredWidth: 48
                    Layout.preferredHeight: 48
                    icon.source: "qrc:/icons/outline/adjustments-horizontal.svg"
                    icon.color: Theme.foregroundMuted
                    hoverEnabled: true
                    background: Rectangle {
                        color: parent.hovered ? Theme.surfaceHover : "transparent"
                    }
                    ToolTip.visible: hovered
                    ToolTip.text: "Add Node"
                    onClicked: {
                        if (workspace.activeTab)
                            workspace.openAddNodePopup()
                    }
                }

                ToolButton {
                    Layout.preferredWidth: 48
                    Layout.preferredHeight: 48
                    icon.source: "qrc:/icons/outline/player-play.svg"
                    icon.color: Theme.foregroundMuted
                    hoverEnabled: true
                    background: Rectangle {
                        color: parent.hovered ? Theme.surfaceHover : "transparent"
                    }
                    ToolTip.visible: hovered
                    ToolTip.text: "Process All"
                    onClicked: {
                        if (workspace.activeTab)
                            workspace.activeTab.engine.processAll()
                    }
                }

                Item { Layout.fillHeight: true }

                ToolButton {
                    Layout.preferredWidth: 48
                    Layout.preferredHeight: 48
                    icon.source: "qrc:/icons/outline/settings.svg"
                    icon.color: Theme.foregroundMuted
                    hoverEnabled: true
                    background: Rectangle {
                        color: parent.hovered ? Theme.surfaceHover : "transparent"
                    }
                    ToolTip.visible: hovered
                    ToolTip.text: "Settings"
                }
            }
        }

        // Node editor workspace
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Theme.background

            NodeEditorWorkspace {
                id: workspace
                anchors.fill: parent
                showTopBar: true
                showBottomBar: true
            }
        }

    }

    // Floating preview display panel
    DisplayPanel {
        id: displayPanel
        x: parent.width - width - 20
        y: 50
        z: 100
    }

    // OCIO Config file dialog
    FileDialog {
        id: configDialog
        title: "Open OCIO Config"
        nameFilters: ["OCIO Configs (*.ocio)", "All Files (*)"]
        onAccepted: {
            var file = configDialog.selectedFile.toString()
            file = file.replace(/^(file:\/{2})/, "")
            if (workspace.activeTab)
                workspace.activeTab.engine.processAll()
        }
    }
}
