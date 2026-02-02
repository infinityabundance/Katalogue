import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Kirigami.Card {
    id: sidebar
    anchors.margins: Kirigami.Units.largeSpacing

    contentItem: Column {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Heading {
            text: "Volumes"
            level: 3
        }

        Repeater {
            model: KatalogueClient.volumes
            delegate: Item {
                width: parent.width
                height: Kirigami.Units.gridUnit * 2

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    text: modelData["label"] || "Unnamed volume"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        KatalogueClient.selectedVolumeId = modelData["id"]
                    }
                }
            }
        }
    }
}
