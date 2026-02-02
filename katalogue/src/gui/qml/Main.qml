import QtQuick
import QtQuick.Controls
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
                            width: parent.width * 0.4
                            placeholderText: "Scan root path"
                            onAccepted: KatalogueClient.startScan(text)
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
