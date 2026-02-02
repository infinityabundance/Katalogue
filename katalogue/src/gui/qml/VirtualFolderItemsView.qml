import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Kirigami.Card {
    id: virtualItems
    anchors.margins: Kirigami.Units.largeSpacing

    contentItem: Column {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Heading {
            text: "Items"
            level: 3
        }

        ListView {
            width: parent.width
            height: 320
            model: KatalogueClient.virtualFolderItems
            delegate: Item {
                width: parent.width
                height: Kirigami.Units.gridUnit * 2

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 2
                    Label {
                        text: modelData["fileName"]
                        font.bold: true
                    }
                    Label {
                        text: modelData["volumeLabel"] + ": " + modelData["fullPath"]
                        color: "#888888"
                        elide: Text.ElideMiddle
                        width: parent.width
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onDoubleClicked: {
                        KatalogueClient.jumpToVirtualFolderItem(modelData["volumeId"],
                                                               modelData["directoryId"])
                    }
                }

                Button {
                    text: "Remove"
                    anchors.right: parent.right
                    onClicked: {
                        KatalogueClient.removeFileFromVirtualFolder(
                            KatalogueClient.selectedVirtualFolderId,
                            modelData["fileId"])
                    }
                }
            }
        }
    }
}
