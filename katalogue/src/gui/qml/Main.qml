import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs as Dialogs
import org.kde.kirigami as Kirigami
import org.kde.Katalogue 1.0

Kirigami.ApplicationWindow {
    id: root
    title: qsTr("Katalogue")
    width: 1024
    height: 720
    visible: true

    Component.onCompleted: {
        KatalogueClient.refreshVolumes()
        KatalogueClient.refreshProjectInfo()
        rebuildVolumeFilterModel()
        KatalogueClient.loadVirtualFolderChildren(-1)
    }

    pageStack.initialPage: Kirigami.Page {
        title: qsTr("Catalog")

        Column {
            anchors.fill: parent
            spacing: Kirigami.Units.largeSpacing

            Row {
                spacing: Kirigami.Units.largeSpacing
                anchors.fill: parent

                Column {
                    spacing: Kirigami.Units.largeSpacing
                    width: parent.width
                    height: parent.height

                    Row {
                        spacing: Kirigami.Units.largeSpacing

                        Kirigami.ActionTextField {
                            id: searchField
                            width: parent.width * 0.4
                            placeholderText: qsTr("Search files")
                            onAccepted: performSearch()
                        }

                        ComboBox {
                            id: volumeFilter
                            width: parent.width * 0.2
                            model: volumeFilterModel
                            textRole: "label"
                        }

                        Kirigami.ActionTextField {
                            id: fileTypeField
                            width: parent.width * 0.15
                            placeholderText: qsTr("Type (e.g. mp3)")
                            onAccepted: performSearch()
                        }

                        Button {
                            text: qsTr("Search")
                            onClicked: performSearch()
                        }

                        Kirigami.ActionTextField {
                            id: scanField
                            width: parent.width * 0.25
                            placeholderText: qsTr("Scan root path")
                            onAccepted: KatalogueClient.startScan(text)
                        }

                        Button {
                            text: qsTr("Scan...")
                            onClicked: scanDialog.open()
                        }

                        Button {
                            text: qsTr("Open catalog...")
                            onClicked: openCatalogDialog.open()
                        }

                        Button {
                            text: qsTr("Preferences")
                            onClicked: root.pageStack.push(Qt.resolvedUrl("PreferencesPage.qml"))
                        }

                        Button {
                            text: qsTr("About Katalogue")
                            onClicked: root.pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
                        }
                    }

                    Row {
                        spacing: Kirigami.Units.smallSpacing

                        Label {
                            text: qsTr("Catalog:")
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
                                return qsTr(" — %1 volumes, %2 files").arg(info["volumeCount"]).arg(info["fileCount"])
                            }
                            color: Kirigami.Theme.disabledTextColor
                        }
                    }

                    Kirigami.Card {
                        visible: KatalogueClient.activeScans.length > 0
                        width: parent.width

                        contentItem: Column {
                            spacing: Kirigami.Units.smallSpacing

                            Kirigami.Heading {
                                text: qsTr("Scans")
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
                                        text: qsTr("Cancel")
                                        visible: modelData["status"] === "running"
                                        onClicked: KatalogueClient.cancelScan(modelData["id"])
                                    }
                                }
                            }
                        }
                    }

                    TabBar {
                        id: viewTabs
                        width: parent.width
                        TabButton { text: qsTr("Physical") }
                        TabButton { text: qsTr("Virtual") }
                    }

                    StackLayout {
                        width: parent.width
                        height: parent.height - 220
                        currentIndex: viewTabs.currentIndex

                        Item {
                            Loader {
                                anchors.fill: parent
                                active: KatalogueClient.volumes.length === 0
                                sourceComponent: EmptyCatalogPlaceholder {
                                    onScanRequested: scanDialog.open()
                                }
                            }

                            Loader {
                                anchors.fill: parent
                                active: KatalogueClient.volumes.length > 0
                                sourceComponent: Component {
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

                                                Kirigami.Card {
                                                    visible: KatalogueClient.searchResults.length > 0 ||
                                                             searchField.text.length > 0
                                                    width: parent.width

                                                    contentItem: Column {
                                                        spacing: Kirigami.Units.smallSpacing

                                                        Kirigami.Heading {
                                                            text: qsTr("Search Results")
                                                            level: 3
                                                        }

                                                        ListView {
                                                            width: parent.width
                                                            height: Math.min(200, contentHeight)
                                                            model: KatalogueClient.searchResults
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
                                                                    onClicked: {
                                                                        KatalogueClient.jumpToResult(modelData["volumeId"],
                                                                                                     modelData["directoryId"])
                                                                    }
                                                                }
                                                                Button {
                                                                    text: qsTr("Add")
                                                                    anchors.right: parent.right
                                                                    enabled: KatalogueClient.selectedVirtualFolderId >= 0
                                                                    onClicked: {
                                                                        KatalogueClient.addFileToVirtualFolder(
                                                                            KatalogueClient.selectedVirtualFolderId,
                                                                            modelData["fileId"])
                                                                    }
                                                                }
                                                            }
                                                        }

                                                        Label {
                                                            visible: searchField.text.length > 0 &&
                                                                     KatalogueClient.searchResults.length === 0
                                                            text: qsTr("No results found for \"%1\"").arg(searchField.text)
                                                            color: Kirigami.Theme.disabledTextColor
                                                            horizontalAlignment: Text.AlignHCenter
                                                            width: parent.width
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
                            }
                        }

                        Item {
                            Row {
                                anchors.fill: parent
                                spacing: Kirigami.Units.largeSpacing

                                VirtualFoldersSidebar {
                                    width: 260
                                    height: parent.height
                                }

                                VirtualFolderItemsView {
                                    width: parent.width - 260
                                    height: parent.height
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Dialogs.FolderDialog {
        id: scanDialog
        title: qsTr("Select root folder to scan")
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
        title: qsTr("Open Katalogue catalog")
        nameFilters: [qsTr("Katalogue catalogs (*.kdcatalog)"), qsTr("All files (*)")]
        onAccepted: {
            const url = selectedFile.toString()
            if (url.length > 0) {
                const path = url.replace("file://", "")
                KatalogueClient.openProject(path)
            }
        }
    }

    ListModel {
        id: volumeFilterModel
    }

    Connections {
        target: KatalogueClient
        function onVolumesChanged() {
            rebuildVolumeFilterModel()
        }
    }

    function rebuildVolumeFilterModel() {
        volumeFilterModel.clear()
        volumeFilterModel.append({"label": qsTr("All volumes"), "id": -1})
        for (let i = 0; i < KatalogueClient.volumes.length; i += 1) {
            const entry = KatalogueClient.volumes[i]
            volumeFilterModel.append({
                "label": entry["label"] || qsTr("Unnamed volume"),
                "id": entry["id"]
            })
        }
    }

    function performSearch() {
        const query = searchField.text
        const fileType = fileTypeField.text.trim()
        let volumeId = -1
        if (volumeFilter.currentIndex >= 0) {
            const entry = volumeFilterModel.get(volumeFilter.currentIndex)
            volumeId = entry.id
        }
        KatalogueClient.search(query, volumeId, fileType)
    }
}
