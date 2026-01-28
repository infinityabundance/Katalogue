import QtQuick
import QtQuick.Controls
import org.kde.kirigami as Kirigami

Kirigami.ApplicationWindow {
    id: root
    title: "Katalogue"
    width: 1024
    height: 720
    visible: true

    pageStack.initialPage: Kirigami.Page {
        title: "Catalog"

        Row {
            anchors.fill: parent
            spacing: Kirigami.Units.largeSpacing

            VolumeSidebar {
                width: 200
                height: parent.height
            }

            Column {
                spacing: Kirigami.Units.largeSpacing
                anchors.fill: parent

                DirectoryTree {
                    height: parent.height * 0.4
                    width: parent.width
                }

                FileView {
                    height: parent.height * 0.6
                    width: parent.width
                }
            }
        }
    }
}
