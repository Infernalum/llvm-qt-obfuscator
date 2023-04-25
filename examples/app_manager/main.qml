import QtQuick 2.0
import QtQuick.Controls 2.0
Window {
  id: root
  readonly property color darkColor: "#218165"
  readonly property color lightColor: "#EBEBEB"
  width: 280
  height: 150
  visible: true
  title: qsTr("Expose C++ object test")
  color: root.lightColor
  Column {
    anchors.centerIn: parent
    spacing: 20
    Text {
      id: resultText
      color: root.darkColor
    }
    Button {
      anchors.horizontalCenter: parent.horizontalCenter
      text: qsTr("Start operation")
      palette.buttonText: root.darkColor
      onClicked: {
        appManager.performOperation()
      }
    }
  }
  Connections {
    target: appManager
    function onOperationFinished(result) {
      resultText.text = "Operation result: " + result
    }
  }
}
