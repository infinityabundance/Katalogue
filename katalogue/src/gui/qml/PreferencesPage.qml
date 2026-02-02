import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami

Kirigami.ScrollablePage {
    id: root
    title: "Preferences"

    ColumnLayout {
        anchors.margins: Kirigami.Units.largeSpacing
        anchors.fill: parent
        spacing: Kirigami.Units.largeSpacing

        Kirigami.Heading {
            text: "Scanning"
            level: 2
        }

        CheckBox {
            text: "Include hidden files"
            checked: KatalogueClient.scannerIncludeHidden
            onToggled: KatalogueClient.scannerIncludeHidden = checked
        }

        CheckBox {
            text: "Follow symlinks"
            checked: KatalogueClient.scannerFollowSymlinks
            onToggled: KatalogueClient.scannerFollowSymlinks = checked
        }

        CheckBox {
            text: "Compute file hashes"
            checked: KatalogueClient.scannerComputeHashes
            onToggled: KatalogueClient.scannerComputeHashes = checked
        }

        RowLayout {
            spacing: Kirigami.Units.smallSpacing
            Label { text: "Maximum depth (–1 = unlimited)" }
            SpinBox {
                from: -1
                to: 64
                value: KatalogueClient.scannerMaxDepth
                onValueModified: KatalogueClient.scannerMaxDepth = value
            }
        }

        Label { text: "Exclude patterns (one per line, wildcards allowed)" }
        TextArea {
            text: KatalogueClient.scannerExcludePatternsString
            wrapMode: TextArea.NoWrap
            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 6
            onTextChanged: KatalogueClient.scannerExcludePatternsString = text
        }

        Kirigami.Separator { }

        Kirigami.Heading {
            text: "User Interface"
            level: 2
        }

        CheckBox {
            text: "Confirm before deleting virtual folders"
            checked: KatalogueClient.uiConfirmVirtualFolderDelete
            onToggled: KatalogueClient.uiConfirmVirtualFolderDelete = checked
        }
    }
}
