import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import org.kde.Katalogue 1.0

Kirigami.ScrollablePage {
    id: root
    title: qsTr("About Katalogue")

    ColumnLayout {
        anchors.margins: Kirigami.Units.largeSpacing
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing

        Kirigami.Heading {
            text: qsTr("Katalogue")
            level: 1
        }

        Label {
            text: qsTr("Version %1").arg(KatalogueClient.appVersion)
        }

        Label {
            text: qsTr("Modern disk catalog for KDE / Qt 6.\nScan drives and discs, search offline, and organize with virtual folders, metadata, and tags.")
            wrapMode: Text.Wrap
        }

        Kirigami.Separator { }

        Label {
            text: qsTr("Author:")
            font.bold: true
        }
        Label {
            text: "Riaan de Beer"
        }

        Kirigami.Separator { }

        Label {
            text: qsTr("Project links:")
            font.bold: true
        }

        Kirigami.BasicListItem {
            label: qsTr("Source code (GitHub)")
            onClicked: Qt.openUrlExternally("https://github.com/infinityabundance/Katalogue")
        }

        Kirigami.BasicListItem {
            label: qsTr("Issue tracker")
            onClicked: Qt.openUrlExternally("https://github.com/infinityabundance/Katalogue/issues")
        }

        Kirigami.Separator { }

        Label {
            text: qsTr("License:")
            font.bold: true
        }
        Label {
            text: "Apache-2.0"
        }
    }
}
