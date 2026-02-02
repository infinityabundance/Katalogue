import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Kirigami.Card {
    id: directoryCard
    anchors.margins: Kirigami.Units.largeSpacing

    contentItem: Column {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Heading {
            text: "Directories"
            level: 3
        }

        ListView {
            width: parent.width
            height: 200
            model: KatalogueClient.directoryEntries
            delegate: Item {
                width: parent.width
                height: Kirigami.Units.gridUnit * 2
                property int dirId: modelData["id"]

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: modelData["name"]
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        KatalogueClient.selectedDirectoryId = dirId
                    }
                }
            }
        }
    }
}
