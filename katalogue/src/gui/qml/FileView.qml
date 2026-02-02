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
            text: "Files"
            level: 3
        }

        ListView {
            width: parent.width
            height: 240
            model: KatalogueClient.fileEntries

            delegate: Item {
                width: parent.width
                height: Kirigami.Units.gridUnit * 2

                Row {
                    spacing: Kirigami.Units.smallSpacing
                    anchors.verticalCenter: parent.verticalCenter
                    Label { text: modelData["name"] }
                    Label { text: modelData["size"] + " B" }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        fileCard.selectedFileId = modelData["id"]
                        fileCard.selectedFileInfo = modelData
                    }
                }
            }
        }

        Kirigami.Heading {
            text: "Details"
            level: 3
        }

        Column {
            spacing: Kirigami.Units.smallSpacing

            Label {
                text: fileCard.selectedFileId >= 0
                      ? "File: " + (fileCard.selectedFileInfo["name"] || "")
                      : "Select a file to edit notes and tags"
            }

            Label {
                visible: fileCard.selectedFileId >= 0
                text: "Size: " + (fileCard.selectedFileInfo["size"] || 0) + " B"
            }

            Label {
                visible: fileCard.selectedFileId >= 0
                text: "Volume: " + (fileCard.selectedFileInfo["volume_label"] || "")
            }

            Label {
                visible: fileCard.selectedFileId >= 0
                text: "Path: " + (fileCard.selectedFileInfo["full_path"] || "")
                elide: Text.ElideMiddle
            }

            TextArea {
                id: noteArea
                placeholderText: "Notes"
                visible: fileCard.selectedFileId >= 0
            }

            Row {
                spacing: Kirigami.Units.smallSpacing
                visible: fileCard.selectedFileId >= 0

                TextField {
                    id: tagKeyField
                    placeholderText: "Key"
                    width: parent.width * 0.4
                }
                TextField {
                    id: tagValueField
                    placeholderText: "Value"
                    width: parent.width * 0.4
                }
                Button {
                    text: "+"
                    onClicked: {
                        if (fileCard.selectedFileId >= 0 && tagKeyField.text.length > 0) {
                            KatalogueClient.addFileTag(fileCard.selectedFileId,
                                                       tagKeyField.text,
                                                       tagValueField.text)
                            tagKeyField.text = ""
                            tagValueField.text = ""
                            tagsModel = KatalogueClient.getFileTags(fileCard.selectedFileId)
                        }
                    }
                }
            }

            ListView {
                id: tagsList
                width: parent.width
                height: Math.min(120, contentHeight)
                model: tagsModel
                visible: fileCard.selectedFileId >= 0
                delegate: Row {
                    spacing: Kirigami.Units.smallSpacing
                    Label { text: modelData["key"] + "=" + modelData["value"] }
                    Button {
                        text: "×"
                        onClicked: {
                            KatalogueClient.removeFileTag(fileCard.selectedFileId,
                                                          modelData["key"],
                                                          modelData["value"])
                            tagsModel = KatalogueClient.getFileTags(fileCard.selectedFileId)
                        }
                    }
                }
            }

            Row {
                spacing: Kirigami.Units.smallSpacing
                visible: fileCard.selectedFileId >= 0
                Button {
                    text: "Save"
                    onClicked: {
                        KatalogueClient.setFileNote(fileCard.selectedFileId, noteArea.text)
                    }
                }
            }
        }
    }

    property var tagsModel: []

    onSelectedFileIdChanged: {
        if (selectedFileId >= 0) {
            noteArea.text = KatalogueClient.getFileNote(selectedFileId)
            tagsModel = KatalogueClient.getFileTags(selectedFileId)
        } else {
            noteArea.text = ""
            tagsModel = []
            selectedFileInfo = ({})
        }
    }
}
