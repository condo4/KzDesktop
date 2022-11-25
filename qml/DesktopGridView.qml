import QtQuick
import QtQuick.Controls
import org.kazoe.xdg

ListView {
    id: desktopgridview
    property string category
    property int nbrow
    clip: true
    orientation: ListView.Horizontal
    anchors.margins: 5
    height: 150
    spacing: 15


    model: DesktopFilter {
        id: filtermodel
        model: xdgEntries
        category: desktopgridview.category
        onRowsInserted: {
            desktopgridview.nbrow = rowCount()
        }

        onRowsRemoved: {
            desktopgridview.nbrow = rowCount()
        }

        Component.onCompleted: {
            desktopgridview.nbrow = rowCount()
        }
    }

    delegate: Rectangle {
        property string applicationIcon: appIcon
        property string applicationTitle: name
        property string xdgPath: path
        property alias highlighted: btn.highlighted
        id: entryItem
        width: desktopgridview.height * 0.8
        height: desktopgridview.height * 0.8
        highlighted: keywords.includes("running")
        color: (btn.highlighted)?(currentTheme.highlight):(currentTheme.window)
        radius: 4

        Button {
            id: btn
            icon.source: "file://" + entryItem.applicationIcon
            icon.name: entryItem.applicationIcon
            icon.width: btn.width
            icon.height: btn.height

            font.pixelSize: 12
            font.bold: true
            height: entryItem.height * 0.5
            width: entryItem.width * 0.5
            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter

            background: Rectangle {
                color: "transparent"
            }
        }

        Text {
            anchors.top: btn.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: entryItem.height * 0.4
            text: entryItem.applicationTitle
            font: btn.font
            fontSizeMode: Text.Fit
            opacity: enabled ? 1.0 : 0.3
            color: (btn.highlighted)?("white"):("grey")
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        MouseArea {
            id: zone
            anchors.fill: parent
            enabled: parent.enabled
            property bool locked: false

            onClicked: {
                if(zone.locked)
                    zone.locked = false
                else
                    xdgEntries.start(entryItem.xdgPath)
            }


            Timer {
                id: longPressTimer

                interval: 1000
                repeat: false
                running: false

                onTriggered: {
                    xdgEntries.stop(entryItem.xdgPath)
                    zone.locked = true
                }
            }

            onPressedChanged: {
                if ( pressed ) {
                    longPressTimer.running = true;
                } else {
                    longPressTimer.running = false;
                }
            }
        }
    }
}
