import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Kirigami.Card {
    id: fileCard
    anchors.margins: Kirigami.Units.largeSpacing

    contentItem: Column {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Heading {
            text: "Files"
            level: 3
        }

        ListView {
            width: parent.width
            height: 240
            model: KatalogueClient.fileEntries

            delegate: Row {
                spacing: Kirigami.Units.smallSpacing
                Label { text: modelData["name"] }
                Label { text: modelData["size"] + " B" }
            }
        }
    }
}
