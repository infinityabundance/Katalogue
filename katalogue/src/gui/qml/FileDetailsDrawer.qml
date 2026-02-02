import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Kirigami.OverlayDrawer {
    id: drawer
    edge: Qt.RightEdge
    modal: false
    width: parent.width * 0.33

    property int fileId: -1
    property var fileInfo: ({})
    property var tagsModel: []

    onFileIdChanged: {
        if (fileId > 0) {
            fileInfo = KatalogueClient.getFileInfo(fileId)
            noteArea.text = KatalogueClient.getFileNote(fileId)
            tagsModel = KatalogueClient.getFileTags(fileId)
        } else {
            fileInfo = ({})
            noteArea.text = ""
            tagsModel = []
        }
    }

    contentItem: Column {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Heading {
            text: fileInfo["name"] || fileInfo["fileName"] || "File details"
            level: 2
        }

        Label {
            text: "Volume: " + (fileInfo["volume_label"] || fileInfo["volumeLabel"] || "")
            visible: fileId > 0
        }
        Label {
            text: "Path: " + (fileInfo["full_path"] || fileInfo["fullPath"] || "")
            elide: Text.ElideMiddle
            visible: fileId > 0
        }
        Label {
            text: "Size: " + (fileInfo["size"] || 0) + " B"
            visible: fileId > 0
        }

        Kirigami.Heading {
            text: "Notes"
            level: 3
            visible: fileId > 0
        }
        TextArea {
            id: noteArea
            placeholderText: "Notes"
            visible: fileId > 0
        }
        Button {
            text: "Save"
            visible: fileId > 0
            onClicked: KatalogueClient.setFileNote(fileId, noteArea.text)
        }

        Kirigami.Heading {
            text: "Tags"
            level: 3
            visible: fileId > 0
        }
        Row {
            spacing: Kirigami.Units.smallSpacing
            visible: fileId > 0
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
                text: "Add"
                onClicked: {
                    if (tagKeyField.text.length > 0) {
                        KatalogueClient.addFileTag(fileId, tagKeyField.text, tagValueField.text)
                        tagKeyField.text = ""
                        tagValueField.text = ""
                        tagsModel = KatalogueClient.getFileTags(fileId)
                    }
                }
            }
        }
        ListView {
            width: parent.width
            height: Math.min(160, contentHeight)
            model: tagsModel
            visible: fileId > 0
            delegate: Row {
                spacing: Kirigami.Units.smallSpacing
                Label { text: modelData["key"] + "=" + modelData["value"] }
                Button {
                    text: "×"
                    onClicked: {
                        KatalogueClient.removeFileTag(fileId, modelData["key"], modelData["value"])
                        tagsModel = KatalogueClient.getFileTags(fileId)
                    }
                }
            }
        }
    }
}
