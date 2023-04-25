#ifndef KEYGENERATOR_H
#define KEYGENERATOR_H
#include <QObject>
#include <QString>
#include <QStringList>
class KeyGenerator : public QObject
{
    Q_OBJECT
    // НЕ МЕНЯЕТСЯ ПРИ ПРОСТОМ ОБХОДЕ!!!
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QStringList types READ types NOTIFY typesChanged)
    Q_PROPERTY(QString filename READ filename WRITE seFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString passphrase READ passphrase WRITE setPassphrase NOTIFY passphraseChanged)
public:
    KeyGenerator();
    ~KeyGenerator();
    QString type();
    void setType(const QString &type_);
    QStringList types();
    QString filename();
    void seFilename(const QString &filename_);
    QString passphrase();
    void setPassphrase(const QString &passhprase_);
public slots:
    void generateKey();
signals:
    void typeChanged();
    void typesChanged();
    void filenameChanged();
    void passphraseChanged();
    void keyGenerated(bool changed);
private:
    QString _type;
    QString _filename;
    QString _passphrase;
    QStringList _types;
};

QString just_for_debug(QString& str);

#endif
