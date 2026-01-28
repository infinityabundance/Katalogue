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

        TableView {
            width: parent.width
            height: 240
            model: 3

            delegate: Row {
                spacing: Kirigami.Units.smallSpacing
                Label { text: index === 0 ? "test_notes.txt" : index === 1 ? "image.png" : "music.ogg" }
                Label { text: index === 0 ? "4 KB" : index === 1 ? "2 MB" : "5 MB" }
            }
        }
    }
}
