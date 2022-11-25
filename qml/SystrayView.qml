import QtQuick
import QtQuick.Controls
import QtWayland.Compositor
import org.kazoe.desktop
import org.kazoe.xdg

ListView {
    id: systrayView
    width: systrayView.contentWidth
    orientation: ListView.Horizontal
    model: systray
    delegate: Item {
        id: trayitem

        Component.onCompleted: {
            var qmlcode
            var c
            if(exec.startsWith("qml://")) {
                var ur = exec.split("/")
                qmlcode = "import " + ur[2] + " " + ur[3] + "; " + ur[4] + " {}"
                c = Qt.createQmlObject(qmlcode, trayitem, ur[4])
            }
            else
            {
                console.debug("Item not supported for systray:" + exec)
            }

            trayitem.width = Qt.binding(() => {return c.width})
            trayitem.height = Qt.binding(() => {return c.height})
        }
    }
}
