import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Kirigami.Card {
    id: directoryCard
    anchors.margins: Kirigami.Units.largeSpacing

    contentItem: Column {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Heading {
            text: qsTr("Directories")
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
                property bool isSelected: dirId === KatalogueClient.selectedDirectoryId

                Rectangle {
                    anchors.fill: parent
                    radius: Kirigami.Units.smallSpacing
                    color: isSelected ? Kirigami.Theme.highlightColor : "transparent"
                }

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: modelData["name"]
                    color: isSelected ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        KatalogueClient.selectedDirectoryId = dirId
                    }
                }
            }
        }

        Label {
            visible: KatalogueClient.selectedDirectoryId < 0
            text: qsTr("Select a folder to see its contents.")
            color: Kirigami.Theme.disabledTextColor
        }
    }
}
