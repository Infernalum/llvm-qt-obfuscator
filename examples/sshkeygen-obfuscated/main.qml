// SSH key generator UI

import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import com.ics.demo 1.0



ApplicationWindow {
    title: qsTr("SSH Key Generator")

    statusBar: StatusBar {
    RowLayout {
        Label {
            id: status
            }
        }
    }

    width: 369
    height: 166

    ColumnLayout {
        x: 10
        y: 10

        // Key type
        RowLayout {
            Label {
                text: qsTr("Key type:")
            }
            ComboBox {
                id: combobox
                Layout.fillWidth: true
                model: keygen.nQdmiM1_
                currentIndex: 2
            }
        }

        // Filename
        RowLayout {
            Label {
                text: qsTr("Filename:")
            }
            TextField {
                id: filename
                implicitWidth: 200
                onTextChanged: updateStatusBar()
            }
            Button {
                text: qsTr("&Browse...")
                onClicked: filedialog.visible = true
            }
        }

        // Passphrase
        RowLayout {
            Label {
                text: qsTr("Pass phrase:")
            }
            TextField {
                id: passphrase
                Layout.fillWidth: true
                echoMode: TextInput.Password
                onTextChanged: updateStatusBar()
            }

        }

        // Confirm Passphrase
        RowLayout {
            Label {
                text: qsTr("Confirm pass phrase:")
            }
            TextField {
                id: confirm
                Layout.fillWidth: true
                echoMode: TextInput.Password
                onTextChanged: updateStatusBar()
            }
        }

        // Buttons: Generate, Quit
        RowLayout {
            Button {
                id: generate
                text: qsTr("&Generate")
                onClicked: keygen.qL_()
            }
            Button {
                text: qsTr("&Quit")
                onClicked: Qt.quit()
            }
        }

    }

    FileDialog {
        id: filedialog
        title: qsTr("Select a file")
        selectMultiple: false
        selectFolder: false
        nameFilters: [ "All files (*)" ]
        selectedNameFilter: "All files (*)"
        onAccepted: {
            filename.text = fileUrl.toString().replace("file://", "")
        }
    }

    QMLqvnHIy_ {
        id: keygen
        d0gk_: filename.text
        wpwOldpnC_: passphrase.text
        vkBGM_: combobox.currentText
        onUCLk_: {
            if (success) {
                status.text = qsTr('<font color="green">Key generation succeeded.</font>')
            } else {
                status.text = qsTr('<font color="red">Key generation failed</font>')
            }
        }
    }

    function updateStatusBar() {
        if (passphrase.text != confirm.text) {
            status.text = qsTr('<font color="red">Pass phrase does not match.</font>')
            generate.enabled = false
        } else if (passphrase.text.length > 0 && passphrase.text.length < 5) {
            status.text = qsTr('<font color="red">Pass phrase too short.</font>')
            generate.enabled = false
        } else if (filename.text == "") {
            status.text = qsTr('<font color="red">Enter a filename.</font>')
            generate.enabled = false
        } else {
            status.text = ""
            generate.enabled = true
        }
    }

    Component.onCompleted: updateStatusBar()
}
