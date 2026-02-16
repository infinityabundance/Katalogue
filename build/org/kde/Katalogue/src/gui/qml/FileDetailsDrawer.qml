import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import org.kde.Katalogue 1.0

Kirigami.OverlayDrawer {
    id: drawer
    edge: Qt.RightEdge
    modal: false
    width: parent.width * 0.33

    property int fileId: -1
    property var fileInfo: ({})
    property var tagsModel: []

    function formatSize(bytes) {
        if (bytes < 1024) return bytes + " B"
        if (bytes < 1048576) return (bytes / 1024).toFixed(1) + " KB"
        if (bytes < 1073741824) return (bytes / 1048576).toFixed(1) + " MB"
        return (bytes / 1073741824).toFixed(2) + " GB"
    }

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
            text: fileInfo["name"] || fileInfo["fileName"] || qsTr("File details")
            level: 2
        }

        Label {
            text: qsTr("Volume: %1").arg(fileInfo["volume_label"] || fileInfo["volumeLabel"] || "")
            visible: fileId > 0
        }
        Label {
            text: qsTr("Path: %1").arg(fileInfo["full_path"] || fileInfo["fullPath"] || "")
            elide: Text.ElideMiddle
            visible: fileId > 0
        }
        Label {
            text: qsTr("Size: %1").arg(drawer.formatSize(fileInfo["size"] || 0))
            visible: fileId > 0
        }

        Kirigami.Heading {
            text: qsTr("Notes")
            level: 3
            visible: fileId > 0
        }
        TextArea {
            id: noteArea
            placeholderText: qsTr("Notes")
            visible: fileId > 0
        }
        Button {
            text: qsTr("Save")
            visible: fileId > 0
            onClicked: KatalogueClient.setFileNote(fileId, noteArea.text)
        }

        Kirigami.Heading {
            text: qsTr("Tags")
            level: 3
            visible: fileId > 0
        }
        Row {
            spacing: Kirigami.Units.smallSpacing
            visible: fileId > 0
            TextField {
                id: tagKeyField
                placeholderText: qsTr("Key")
                width: parent.width * 0.4
            }
            TextField {
                id: tagValueField
                placeholderText: qsTr("Value")
                width: parent.width * 0.4
            }
            Button {
                text: qsTr("Add")
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
