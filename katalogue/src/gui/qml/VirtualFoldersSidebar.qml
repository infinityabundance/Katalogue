import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Kirigami.Card {
    id: virtualSidebar
    anchors.margins: Kirigami.Units.largeSpacing

    contentItem: Column {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Heading {
            text: qsTr("Virtual Folders")
            level: 3
        }

        ListView {
            width: parent.width
            height: 260
            model: KatalogueClient.virtualFolders
            delegate: Item {
                width: parent.width
                height: Kirigami.Units.gridUnit * 2
                property int folderId: modelData["id"]
                property bool isSelected: folderId === KatalogueClient.selectedVirtualFolderId

                Rectangle {
                    anchors.fill: parent
                    radius: Kirigami.Units.smallSpacing
                    color: isSelected ? Kirigami.Theme.highlightColor : "transparent"
                }

                Row {
                    anchors.fill: parent
                    spacing: Kirigami.Units.smallSpacing
                    Label {
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData["name"]
                        color: isSelected ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                    }
                    Button {
                        text: qsTr("Delete")
                        onClicked: {
                            if (KatalogueClient.uiConfirmVirtualFolderDelete) {
                                confirmDialog.folderId = folderId
                                confirmDialog.open()
                            } else {
                                KatalogueClient.deleteVirtualFolder(folderId)
                            }
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        KatalogueClient.selectedVirtualFolderId = folderId
                    }
                }
            }
        }

        Row {
            spacing: Kirigami.Units.smallSpacing
            TextField {
                id: newFolderField
                placeholderText: qsTr("New folder")
                width: parent.width * 0.7
            }
            Button {
                text: qsTr("Create")
                onClicked: {
                    if (newFolderField.text.length > 0) {
                        KatalogueClient.createVirtualFolder(newFolderField.text, -1)
                        newFolderField.text = ""
                    }
                }
            }
        }
    }

    Dialog {
        id: confirmDialog
        modal: true
        title: qsTr("Delete virtual folder")
        standardButtons: Dialog.Ok | Dialog.Cancel
        property int folderId: -1
        onAccepted: {
            if (folderId >= 0) {
                KatalogueClient.deleteVirtualFolder(folderId)
            }
        }
        contentItem: Column {
            spacing: Kirigami.Units.smallSpacing
            Label { text: qsTr("Are you sure you want to delete this virtual folder?") }
        }
    }
}
