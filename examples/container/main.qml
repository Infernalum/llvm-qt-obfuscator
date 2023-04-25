import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")

    // массив QML, в который можно поместить что угодно, в C++ это будет либо QVariantMap, либо QVariantList
    property var objectsArray: []

    Text {
        id: textView
        clip: true
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: parent.verticalCenter
            margins: 5
        }
    }

    Button {
        id: addOBjectStructure
        text: qsTr("Add Structure")
        anchors {
            right: parent.horizontalCenter
            left: parent.left
            bottom: addOBjectButton.top
            margins: 5
        }

        onClicked: {
            // Добавляем структуру в массив
            objectsArray.push(factory.createStructure())
        }
    }

    Button {
        id: addOBjectButton
        text: qsTr("Add Object")
        anchors {
            right: parent.horizontalCenter
            left: parent.left
            bottom: parent.bottom
            margins: 5
        }

        onClicked: {
            // Добавляем контейнер в массив
            objectsArray.push(factory.createContainer())
        }
    }

    Button {
        text: qsTr("Read info from Objects")
        anchors {
            right: parent.right
            left: parent.horizontalCenter
            bottom: parent.bottom
            margins: 5
        }

        onClicked: {
            // выводим текст из всех объектов массива
            textView.text = ""
            for (var i = 0; i < objectsArray.length; ++i)
            {
                // главное, чтобы все объекты имели методы с одинаковыми названиями
                var str = objectsArray[i].number + " " + objectsArray[i].message + "\n" + objectsArray[i].getFullInfo() + "\n"
                textView.text += str
            }
        }
    }
}
