#include <QFile>
#include <QProcess>
#include "KeyGenerator.h"
KeyGenerator::KeyGenerator()
    : _type("rsa"), _types{"dsa", "ecdsa", "rsa", "rsa1"}{}

KeyGenerator::~KeyGenerator() {}
QString KeyGenerator::type() {
    return _type;
}
void KeyGenerator::setType(const QString &type_) {
    if (!_types.contains(type_))
        return;
    if (type_ != _type) {
        _type = type_;
        emit typeChanged();
    }
}

QString just_for_debug(QString& str_){
    if (str_ == "111")
        str_ += "222";
    else
        str_ += "333";
    return str_;
}

QStringList KeyGenerator::types() {
    return _types;
}
QString KeyGenerator::filename() {
    return _filename;
}
void KeyGenerator::seFilename(const QString &filename_) {
    if (filename_ != _filename) {
        _filename = filename_;
        emit typeChanged();
    }
}
QString KeyGenerator::passphrase() {
    return _passphrase;
}
void KeyGenerator::setPassphrase(const QString &passphrase_) {
    if (passphrase_ != _passphrase) {
        _passphrase = passphrase_;
        emit passphraseChanged();
    }
}
void KeyGenerator::generateKey() {
    if (_type.isEmpty() or _filename.isEmpty() or
        (_passphrase.length() > 0 and _passphrase.length() < 5)) {
        emit keyGenerated(false);
        return;
    }
    if (QFile::exists(_filename)) {
        QFile::remove(_filename);
    }
    QProcess *proc = new QProcess;
    QString prog = "ssh-keygen";
    QStringList args{"-t", _type, "-N", _passphrase, "-f", _filename};
    proc->start(prog, args);
    proc->waitForFinished();
    emit keyGenerated(proc->exitCode() == 0);
    delete proc;
}
