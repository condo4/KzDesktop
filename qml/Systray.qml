import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.impl 2.15
import org.kazoe.xdg 1.0

Rectangle {
    color: "transparent"
    width: view.contentWidth
    height: systrayView.height

    StatusNotifierHost {
        id: systrayModel
    }

    ListView {
        id: view
        orientation: ListView.Horizontal
        anchors.fill: parent
        anchors.margins: 2
        model: systrayModel
        delegate: Rectangle {
            visible: iconName.length && status !== "Passive"
            height: view.height
            width: visible ? height : 0
            color: "transparent"

            IconImage {
                anchors.fill: parent
                name: iconName
                color: "transparent"
            }

            IconImage {
                id: overlay
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                width: parent.width / 2
                height: parent.height / 2
                visible: overlayIconName.length
                name: overlayIconName
                color: "transparent"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    systrayModel.activate(0, 0, idItem, 0)
                }
            }
        }
    }
}
