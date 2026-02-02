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
            model: []
            delegate: Label {
                text: modelData
            }
        }
    }
}
