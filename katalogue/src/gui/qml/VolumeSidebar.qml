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

                Row {
                    anchors.fill: parent
                    spacing: Kirigami.Units.smallSpacing

                    Label {
                        id: volumeLabel
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData["label"] || "Unnamed volume"
                        width: parent.width * 0.6
                        elide: Text.ElideRight
                    }

                    MouseArea {
                        anchors.fill: volumeLabel
                        onClicked: {
                            KatalogueClient.selectedVolumeId = modelData["id"]
                        }
                    }

                    Button {
                        text: "Rescan"
                        onClicked: {
                            const path = modelData["physical_hint"] || ""
                            if (path.length > 0) {
                                KatalogueClient.startScan(path)
                            }
                        }
                    }
                }
            }
        }
    }
}
