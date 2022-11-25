import QtQuick
import QtQuick.Controls
import org.kazoe.desktop
import org.kazoe.xdg

Rectangle {
    id: statusBar
    height: 46
    property int batteryLevelWarning: 15

    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right

    gradient: Gradient {
        GradientStop { position: 0.0; color: currentTheme.window }
        GradientStop { position: 1.0; color: currentTheme.window }
    }

    Rectangle {
        id: bottomline
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 1
        width: undefined
        color: "black"
    }

    Button {
        id: backHome
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        height: comp.statusBarHeight
        anchors.margins: 4
        palette.buttonText: "white"
        font.pointSize: 9
        highlighted: comp.showHomepage
        display: AbstractButton.IconOnly

        icon.name: "home"

        background: Rectangle {
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            width: backHome.width
            height: backHome.height
            border.color: backHome.highlighted ? currentTheme.highlight : currentTheme.alternateBase
            border.width: backHome.highlighted ? 2 : 1
            radius: 4
            color: currentTheme.mid
        }

        onClicked: {
            comp.showHomepage = true
        }
    }

    ListView {
        id: taskbar
        anchors.left: backHome.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: systrayview.left
        anchors.leftMargin: 4
        anchors.topMargin: 4
        clip: true
        model: entries
        orientation: Qt.Horizontal
        spacing: 4

        delegate: Button {
            id: taskapp
            palette.buttonText: "white"
            font.pointSize: 9
            highlighted: (comp.showHomepage === false) && (manager.currentSurface === wl_surface)

            height: taskbar.height - 4
            display: AbstractButton.TextBesideIcon
            /*
            * TODO: Find app icon with wayland
            icon.name: wl_surface.surface
            icon.color: "#0aa4da"
            */
            Entry {
                id: xdgapp
                desktopFile: "/usr/share/applications/" + appId + ".desktop"
            }
            icon.name: xdgapp.icon
            icon.color: "transparent"

            background: Rectangle {
                anchors.top: parent.top
                anchors.horizontalCenter: parent.horizontalCenter
                width: taskapp.width
                height: taskapp.height
                border.color: taskapp.highlighted ? currentTheme.highlight : currentTheme.alternateBase
                border.width: taskapp.highlighted ? 2 : 1
                radius: 4
                color: currentTheme.mid
            }
            text: title

            onClicked: {
                manager.currentSurface = wl_surface
                comp.showHomepage = false
            }

            Timer {
                id: longPressTimer

                interval: 1000
                repeat: false
                running: false

                onTriggered: {
                    if(manager.currentSurface == wl_surface)
                    {
                        comp.showHomepage = true
                    }

                    entries.stop(pid)
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

    SystrayView {
        id: systrayview
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.topMargin: 2
        anchors.bottomMargin: 2
        anchors.rightMargin: 2

        width: systrayview.contentWidth
        spacing: 1
    }
}
