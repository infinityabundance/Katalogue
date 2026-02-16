import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.Katalogue 1.0

Kirigami.ScrollablePage {
    id: root
    title: qsTr("Preferences")

    ColumnLayout {
        anchors.margins: Kirigami.Units.largeSpacing
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing

        Kirigami.Heading {
            text: qsTr("Scanning")
            level: 2
        }

        CheckBox {
            text: qsTr("Include hidden files")
            checked: KatalogueClient.scannerIncludeHidden
            onToggled: KatalogueClient.scannerIncludeHidden = checked
        }

        CheckBox {
            text: qsTr("Follow symlinks")
            checked: KatalogueClient.scannerFollowSymlinks
            onToggled: KatalogueClient.scannerFollowSymlinks = checked
        }

        CheckBox {
            text: qsTr("Compute file hashes")
            checked: KatalogueClient.scannerComputeHashes
            onToggled: KatalogueClient.scannerComputeHashes = checked
        }

        RowLayout {
            spacing: Kirigami.Units.smallSpacing
            Label { text: qsTr("Maximum depth (–1 = unlimited)") }
            SpinBox {
                from: -1
                to: 64
                value: KatalogueClient.scannerMaxDepth
                onValueModified: KatalogueClient.scannerMaxDepth = value
            }
        }

        Label { text: qsTr("Exclude patterns (one per line, wildcards allowed)") }
        TextArea {
            text: KatalogueClient.scannerExcludePatternsString
            wrapMode: TextArea.NoWrap
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 6
            onTextChanged: KatalogueClient.scannerExcludePatternsString = text
        }

        Kirigami.Separator { }

        Kirigami.Heading {
            text: qsTr("User Interface")
            level: 2
        }

        CheckBox {
            text: qsTr("Confirm before deleting virtual folders")
            checked: KatalogueClient.uiConfirmVirtualFolderDelete
            onToggled: KatalogueClient.uiConfirmVirtualFolderDelete = checked
        }
    }
}
