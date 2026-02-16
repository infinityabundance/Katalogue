import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import org.kde.Katalogue 1.0

Kirigami.Card {
    id: virtualItems
    anchors.margins: Kirigami.Units.largeSpacing

    contentItem: Column {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Heading {
            text: qsTr("Items")
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
                        color: Kirigami.Theme.disabledTextColor
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
                    text: qsTr("Remove")
                    anchors.right: parent.right
                    onClicked: {
                        KatalogueClient.removeFileFromVirtualFolder(
                            KatalogueClient.selectedVirtualFolderId,
                            modelData["fileId"])
                    }
                }
            }
        }

        Label {
            visible: KatalogueClient.selectedVirtualFolderId < 0
            text: qsTr("Select a virtual folder to see its items.")
            color: Kirigami.Theme.disabledTextColor
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
        }

        Label {
            visible: KatalogueClient.selectedVirtualFolderId >= 0 &&
                     KatalogueClient.virtualFolderItems.length === 0
            text: qsTr("This virtual folder is empty. Add files from the Physical tab.")
            color: Kirigami.Theme.disabledTextColor
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            width: parent.width
        }
    }
}
