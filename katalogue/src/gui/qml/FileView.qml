import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Kirigami.Card {
    id: fileCard
    anchors.margins: Kirigami.Units.largeSpacing
    property int selectedFileId: -1
    property var selectedFileInfo: ({})

    contentItem: Column {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Heading {
            text: qsTr("Files")
            level: 3
        }

        ListView {
            width: parent.width
            height: 240
            model: KatalogueClient.fileEntries

            delegate: Item {
                width: parent.width
                height: Kirigami.Units.gridUnit * 2
                property bool isSelected: modelData["id"] === KatalogueClient.selectedFileId

                Rectangle {
                    anchors.fill: parent
                    radius: Kirigami.Units.smallSpacing
                    color: isSelected ? Kirigami.Theme.highlightColor : "transparent"
                }

                Row {
                    spacing: Kirigami.Units.smallSpacing
                    anchors.verticalCenter: parent.verticalCenter
                    Label { text: modelData["name"] }
                    Label { text: modelData["size"] + " B" }
                    Button {
                        text: qsTr("Add")
                        enabled: KatalogueClient.selectedVirtualFolderId >= 0
                        onClicked: {
                            KatalogueClient.addFileToVirtualFolder(
                                KatalogueClient.selectedVirtualFolderId,
                                modelData["id"])
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        fileCard.selectedFileId = modelData["id"]
                        fileCard.selectedFileInfo = modelData
                        KatalogueClient.selectedFileId = modelData["id"]
                        fileDetailsDrawer.fileId = modelData["id"]
                        fileDetailsDrawer.open()
                    }
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onPressed: function(mouse) {
                        if (mouse.button === Qt.RightButton) {
                            KatalogueClient.selectedFileId = modelData["id"]
                            fileContextMenu.popup()
                        }
                    }
                }

                Menu {
                    id: fileContextMenu
                MenuItem {
                        text: qsTr("Copy full path")
                        onTriggered: {
                            const path = modelData["full_path"] || ""
                            if (path.length > 0) {
                                KatalogueClient.copyTextToClipboard(path)
                            }
                        }
                    }
                    MenuItem {
                        text: qsTr("Add to virtual folder…")
                        enabled: KatalogueClient.selectedVirtualFolderId >= 0
                        onTriggered: {
                            KatalogueClient.addFileToVirtualFolder(
                                KatalogueClient.selectedVirtualFolderId,
                                modelData["id"])
                        }
                    }
                }
            }
        }
        Label {
            visible: KatalogueClient.selectedDirectoryId < 0
            text: qsTr("Select a folder from the left to see its files.")
            color: Kirigami.Theme.disabledTextColor
        }

        Label {
            visible: KatalogueClient.selectedDirectoryId >= 0 &&
                     KatalogueClient.fileEntries.length === 0
            text: qsTr("This folder is empty.")
            color: Kirigami.Theme.disabledTextColor
        }
    }

    FileDetailsDrawer {
        id: fileDetailsDrawer
        fileId: KatalogueClient.selectedFileId
    }

}
