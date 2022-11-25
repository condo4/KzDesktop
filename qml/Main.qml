import QtQuick
import QtWayland.Compositor
import QtWayland.Compositor.XdgShell
import QtQuick.Window
import QtQuick.Controls
import org.kazoe.desktop
import org.kazoe.xdg

WaylandCompositor {
    id: comp
    property string defaultTitle: ""
    property int appHeight: win.height - statusBar.height
    property int appWidth: win.width
    property bool showHomepage: true
    property string statusBarModel: "StatusBar"
    property alias statusBarHeight: statusBar.height

    SystemPalette {
        id: currentTheme
        colorGroup: SystemPalette.Active
    }

    DesktopEntries {
        id: xdgEntries

        onRaiseProcess: {
            manager.currentSurface = waylandEntries.getSurface(pid)
        }
    }

    AutoStart {
        id: autostarter
        Component.onCompleted: {
            autostarter.start("Desktop Entry/X-DESKTOPMGR-AUTOSTART", "true")
        }
    }

    DesktopFilter {
        id: sysbar
        section: "X-DESKTOPMGR-SYSBAR"
        showAll: true
        model: xdgEntries
    }

    DesktopFilter {
        id: systray
        section: "X-DESKTOPMGR-SYSTRAY"
        showAll: true
        model: xdgEntries
    }

    WaylandEntries {
        id: entries
    }

    Connections {
        target: manager
        function onCurrentSurfaceChanged() {
            comp.showHomepage = false
        }

        function onGotoHome() {
            comp.showHomepage = true
        }
    }


    WaylandOutput {
        id: output
        sizeFollowsWindow: true
        transform: WaylandOutput.Transform90
        scaleFactor: 4
        window: Window {
            id: win
            visible: true
            width: 800
            height: 480

            VKeyboard {
                id: keyboard
                anchors.fill: parent
            }

            Flickable {
                anchors.fill: parent
                id: appZoneView
                clip: true
                contentWidth: win.width
                contentHeight: win.height
                flickableDirection: Flickable.VerticalFlick
                interactive: false

                property int textInputY: keyboard.textInputY
                property int textInputHeight: keyboard.textInputHeight
                property bool keyboardActive: keyboard.active
                property int keyboardHeight: keyboard.implicitHeight
                property int wantOffset: (appZoneView.height - keyboardHeight) / 2

                onTextInputYChanged: {
                    if(appZoneView.keyboardActive && appZoneView.contentY == 0)
                    {
                        if(textInputY > (appZoneView.height - keyboardHeight))
                        {
                            appZoneView.contentY = textInputY - textInputHeight - wantOffset
                        }
                    }
                }

                onKeyboardActiveChanged: {
                    appZoneView.contentY = 0
                }

                StatusBar {
                    id: statusBar
                    width: parent.width
                    z: 20
                }

                HomePage {
                    id: homepageview
                    anchors.top: statusBar.bottom
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    visible: comp.showHomepage
                }

                ShellSurfaceItem {
                    id: appZone
                    visible: !comp.showHomepage
                    anchors.top: statusBar.bottom
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right

                    shellSurface: manager.currentSurface
                    autoCreatePopupItems: true

                    onWidthChanged: {
                        if(manager.currentSurface)
                            manager.currentSurface.toplevel.sendResizing(Qt.size(appZone.width, appZone.height))
                    }

                    onHeightChanged: {
                        if(manager.currentSurface)
                            manager.currentSurface.toplevel.sendResizing(Qt.size(appZone.width, appZone.height))
                    }
                }
            }
        }
    }

    XdgShell {
        onToplevelCreated: function(toplevel, xdgSurface) {
            toplevel.sendResizing(Qt.size(appZone.width, appZone.height));
            entries.addSurface(xdgSurface)
            manager.currentSurface = xdgSurface
        }
    }

    XdgDecorationManagerV1 {
        preferredMode: XdgToplevel.ServerSideDecoration
    }

    // Extension for Input Method (QT_IM_MODULE) support at compositor-side
    TextInputManager {}
}
