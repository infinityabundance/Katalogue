import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import org.kde.Katalogue 1.0

Kirigami.Card {
    id: directoryCard
    anchors.margins: Kirigami.Units.largeSpacing

    property var navigationStack: []
    property string currentPath: "/"

    contentItem: Column {
        spacing: Kirigami.Units.smallSpacing

        Row {
            spacing: Kirigami.Units.smallSpacing
            width: parent.width

            Kirigami.Heading {
                text: qsTr("Directories")
                level: 3
                anchors.verticalCenter: parent.verticalCenter
            }

            Item { width: Kirigami.Units.largeSpacing; height: 1 }

            Button {
                text: "\u2B06"
                enabled: directoryCard.navigationStack.length > 0
                ToolTip.text: qsTr("Go to parent directory")
                ToolTip.visible: hovered
                onClicked: {
                    if (directoryCard.navigationStack.length > 0) {
                        var stack = directoryCard.navigationStack
                        var prev = stack.pop()
                        directoryCard.navigationStack = stack
                        directoryCard.currentPath = prev.path
                        KatalogueClient.selectedDirectoryId = prev.dirId
                    }
                }
            }

            Label {
                text: directoryCard.currentPath
                elide: Text.ElideMiddle
                color: Kirigami.Theme.disabledTextColor
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width * 0.5
            }
        }

        ListView {
            width: parent.width
            height: 200
            clip: true
            model: KatalogueClient.directoryEntries
            delegate: Item {
                width: parent ? parent.width : 0
                height: Kirigami.Units.gridUnit * 2
                property int dirId: modelData["id"]
                property bool isSelected: dirId === KatalogueClient.selectedDirectoryId

                Rectangle {
                    anchors.fill: parent
                    radius: Kirigami.Units.smallSpacing
                    color: isSelected ? Kirigami.Theme.highlightColor : "transparent"
                }

                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    anchors.leftMargin: Kirigami.Units.smallSpacing
                    spacing: Kirigami.Units.smallSpacing

                    Label {
                        text: modelData["name"]
                        color: isSelected ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        var stack = directoryCard.navigationStack
                        stack.push({
                            "dirId": KatalogueClient.selectedDirectoryId,
                            "path": directoryCard.currentPath
                        })
                        directoryCard.navigationStack = stack
                        directoryCard.currentPath = modelData["full_path"] || modelData["name"]
                        KatalogueClient.selectedDirectoryId = dirId
                    }
                }
            }
        }

        Label {
            visible: KatalogueClient.selectedVolumeId >= 0 &&
                     KatalogueClient.directoryEntries.length === 0
            text: qsTr("No subdirectories in this folder.")
            color: Kirigami.Theme.disabledTextColor
        }

        Label {
            visible: KatalogueClient.selectedVolumeId < 0
            text: qsTr("Select a volume to browse its directories.")
            color: Kirigami.Theme.disabledTextColor
        }
    }
}
