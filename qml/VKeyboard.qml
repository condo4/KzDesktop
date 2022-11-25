import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.VirtualKeyboard.Settings
import QtQuick.Window

InputPanel {
    id: panel
    visible: active
    y: active ? 0 : parent.height
    anchors.left: parent.left
    anchors.right: parent.right
    property int textInputHeight: InputContext.cursorRectangle.height
    property int textInputY: InputContext.cursorRectangle.y
}
