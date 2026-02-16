import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami
import org.kde.Katalogue 1.0

Kirigami.Card {
    id: sidebar
    anchors.margins: Kirigami.Units.largeSpacing

    contentItem: Column {
        spacing: Kirigami.Units.smallSpacing

        Kirigami.Heading {
            text: qsTr("Volumes")
            level: 3
        }

        Repeater {
            model: KatalogueClient.volumes
            delegate: Item {
                width: parent.width
                height: Kirigami.Units.gridUnit * 2
                property bool isSelected: modelData["id"] === KatalogueClient.selectedVolumeId

                Rectangle {
                    anchors.fill: parent
                    radius: Kirigami.Units.smallSpacing
                    color: isSelected ? Kirigami.Theme.highlightColor : "transparent"
                }

                Row {
                    anchors.fill: parent
                    spacing: Kirigami.Units.smallSpacing

                    Label {
                        id: volumeLabel
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData["label"] || qsTr("Unnamed volume")
                        width: parent.width * 0.6
                        elide: Text.ElideRight
                        color: isSelected ? Kirigami.Theme.highlightedTextColor : Kirigami.Theme.textColor
                    }

                    Button {
                        text: qsTr("Rescan")
                        onClicked: {
                            const path = modelData["physical_hint"] || ""
                            if (path.length > 0) {
                                KatalogueClient.startScan(path)
                            }
                        }
                    }

                    Button {
                        text: "⋮"
                        onClicked: volumeMenu.popup()
                    }
                }

                Menu {
                    id: volumeMenu
                    MenuItem {
                        text: qsTr("Rescan volume…")
                        onTriggered: {
                            const path = modelData["physical_hint"] || ""
                            if (path.length > 0) {
                                KatalogueClient.startScan(path)
                            }
                        }
                    }
                    MenuItem {
                        text: qsTr("Rename volume…")
                        onTriggered: renameDialog.open()
                    }
                    MenuItem {
                        text: qsTr("Reveal in file manager")
                        enabled: (modelData["physical_hint"] || "") !== ""
                        onTriggered: {
                            const path = modelData["physical_hint"] || ""
                            if (path.length > 0) {
                                Qt.openUrlExternally("file://" + path)
                            }
                        }
                    }
                }

                Dialog {
                    id: renameDialog
                    modal: true
                    title: qsTr("Rename volume")
                    standardButtons: Dialog.Ok | Dialog.Cancel
                    property string newName: ""
                    onAccepted: {
                        if (newName.length > 0) {
                            KatalogueClient.renameVolume(modelData["id"], newName)
                        }
                    }
                    contentItem: Column {
                        spacing: Kirigami.Units.smallSpacing
                        TextField {
                            id: renameField
                            text: modelData["label"] || ""
                            onTextChanged: renameDialog.newName = text
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: {
                        KatalogueClient.selectedVolumeId = modelData["id"]
                    }
                    onPressed: function(mouse) {
                        if (mouse.button === Qt.RightButton) {
                            volumeMenu.popup()
                        }
                    }
                }
            }
        }
    }
}
