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
            delegate: Label {
                text: modelData["label"] || "Unnamed volume"
            }
        }
    }
}
