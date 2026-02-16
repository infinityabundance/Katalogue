import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Kirigami.Card {
    id: placeholder
    anchors.margins: Kirigami.Units.largeSpacing

    signal scanRequested()

    contentItem: Column {
        spacing: Kirigami.Units.smallSpacing
        Kirigami.Heading {
            text: qsTr("No volumes cataloged yet")
            level: 2
        }
        Label {
            text: qsTr("Use Scan… to index a drive, folder, or disc into this catalog.")
            wrapMode: Text.WordWrap
        }
        Button {
            text: qsTr("Scan now")
            onClicked: placeholder.scanRequested()
        }
    }
}
