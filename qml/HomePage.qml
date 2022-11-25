import QtQuick

Rectangle {
  id: homepage
  clip: true
  property string selected: "tests"

  ListView {
    id: category
    anchors.fill: parent
    model: xdgEntries.categories
    delegate: Rectangle {
      Component.onCompleted: {
        console.debug("MANAGE CATEGORY [" + modelData + "]")
      }

      property bool isshow: modelData !== "System" && modelData !== "Debug"
      width: homepage.width
      height: (isshow) ? (entriesView.nbrow > 0 ? entriesView.height + categoryTitle.height : 0) : 0
      color: currentTheme.base
      visible: isshow && entriesView.nbrow > 0

      Rectangle {
        id: categoryTitle
        height: 20
        width: parent.width - 2
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 4
        anchors.topMargin: 4
        color: currentTheme.mid


        Text {
          anchors.top: parent.top
          anchors.left: parent.left
          anchors.bottom: parent.bottom
          text: modelData
          font.pixelSize: 15
          font.bold: true
          color: currentTheme.windowText
        }
      }

      DesktopGridView {
        id: entriesView
        category: modelData
        anchors.top: categoryTitle.bottom
        anchors.left: parent.left
        anchors.right: parent.right
      }
    }
  }
}
