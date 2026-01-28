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
            model: ["Archive A", "Backup B", "Media C"]
            delegate: Label {
                text: modelData
            }
        }
    }
}
