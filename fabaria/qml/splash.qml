import QtQuick 2.4
import QtQuick.Layouts 1.1

Item {
    id: splash
    property int timeoutInterval: 2000
    property string messaggio
    signal timeout
    width: splashImage.width
    height: splashImage.height

    Image {
        id: splashImage
        source: "qrc:/images/logo4.png"
    }

    Timer {
        interval: timeoutInterval; running: true; repeat: false
        onTriggered: {
            visible = false
            splash.timeout()
        }
    }

    Row{
      anchors.bottom: parent.bottom
      Text {
          color: "white"
          font.pixelSize: 24
          verticalAlignment: Text.AlignVCenter
          antialiasing: true
          text: messaggio
      }
    }
    Component.onCompleted: visible = true
}
