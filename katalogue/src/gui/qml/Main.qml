import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs as Dialogs
import org.kde.kirigami as Kirigami
import org.kde.Katalogue 1.0

Kirigami.ApplicationWindow {
    id: root
    title: "Katalogue"
    width: 1024
    height: 720
    visible: true

    Component.onCompleted: {
        KatalogueClient.refreshVolumes()
        KatalogueClient.refreshProjectInfo()
    }

    pageStack.initialPage: Kirigami.Page {
        title: "Catalog"

        Column {
            anchors.fill: parent
            spacing: Kirigami.Units.largeSpacing

            Row {
                spacing: Kirigami.Units.largeSpacing
                anchors.fill: parent

                VolumeSidebar {
                    width: 220
                    height: parent.height
                }

                Column {
                    spacing: Kirigami.Units.largeSpacing
                    width: parent.width - 220
                    height: parent.height

                    Row {
                        spacing: Kirigami.Units.largeSpacing

                        Kirigami.ActionTextField {
                            id: searchField
                            width: parent.width * 0.6
                            placeholderText: "Search files"
                            onAccepted: KatalogueClient.searchByName(text)
                        }

                        Kirigami.ActionTextField {
                            id: scanField
                            width: parent.width * 0.25
                            placeholderText: "Scan root path"
                            onAccepted: KatalogueClient.startScan(text)
                        }

                        Button {
                            text: "Scan..."
                            onClicked: scanDialog.open()
                        }

                        Button {
                            text: "Open catalog..."
                            onClicked: openCatalogDialog.open()
                        }
                    }

                    Row {
                        spacing: Kirigami.Units.smallSpacing

                        Label {
                            text: "Catalog:"
                            font.bold: true
                        }

                        Label {
                            text: KatalogueClient.projectPath.length > 0
                                  ? KatalogueClient.projectPath
                                  : "(default catalog)"
                            elide: Text.ElideMiddle
                            width: parent.width * 0.5
                        }

                        Label {
                            text: {
                                const info = KatalogueClient.projectInfo
                                if (!info || !info["ok"]) {
                                    return ""
                                }
                                return " — " + info["volumeCount"] + " volumes, " + info["fileCount"] + " files"
                            }
                            color: "#888888"
                        }
                    }

                    Kirigami.Card {
                        visible: KatalogueClient.activeScans.length > 0
                        width: parent.width

                        contentItem: Column {
                            spacing: Kirigami.Units.smallSpacing

                            Kirigami.Heading {
                                text: "Scans"
                                level: 3
                            }

                            ListView {
                                width: parent.width
                                height: Math.min(160, contentHeight)
                                model: KatalogueClient.activeScans
                                delegate: Row {
                                    spacing: Kirigami.Units.smallSpacing
                                    Label {
                                        text: modelData["rootPath"]
                                        elide: Text.ElideMiddle
                                        width: parent.width * 0.6
                                    }
                                    Label { text: modelData["status"] }
                                    Label {
                                        text: modelData["percent"] >= 0
                                              ? modelData["percent"] + "%"
                                              : ""
                                    }
                                    Button {
                                        text: "Cancel"
                                        visible: modelData["status"] === "running"
                                        onClicked: KatalogueClient.cancelScan(modelData["id"])
                                    }
                                }
                            }
                        }
                    }

                    DirectoryTree {
                        height: parent.height * 0.4
                        width: parent.width
                    }

                    FileView {
                        height: parent.height * 0.6
                        width: parent.width
                    }
                }
            }
        }
    }

    Dialogs.FolderDialog {
        id: scanDialog
        title: "Select root folder to scan"
        onAccepted: {
            const url = selectedFolder.toString()
            if (url.length > 0) {
                const path = url.replace("file://", "")
                KatalogueClient.startScan(path)
            }
        }
    }

    Dialogs.FileDialog {
        id: openCatalogDialog
        title: "Open Katalogue catalog"
        nameFilters: ["Katalogue catalogs (*.kdcatalog)", "All files (*)"]
        onAccepted: {
            const url = selectedFile.toString()
            if (url.length > 0) {
                const path = url.replace("file://", "")
                KatalogueClient.openProject(path)
            }
        }
    }
}
