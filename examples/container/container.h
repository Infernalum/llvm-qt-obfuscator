#ifndef CONTAINER_H
#define CONTAINER_H

#include <QObject>

struct Structure : public QObject
{
    explicit Structure(QObject *parent = nullptr);

    int m_number;
    QString m_message;

private:
    Q_OBJECT
    // Сможем обращаться к полям из QML
    // параметр MEMBER указывает, что имеется возможность работать с этим полем и отслеживать его изменение в QML
    Q_PROPERTY(int number MEMBER m_number)
    Q_PROPERTY(QString message MEMBER m_message)

public:
    // А также вызвать в QML этот метод
    Q_INVOKABLE QString getFullInfo() const;
};

class Container : public QObject
{
    Q_OBJECT
    // Сможем обращаться к полям из QML
    Q_PROPERTY(int number READ number WRITE setNumber NOTIFY numberChanged)
    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)

public:
    explicit Container(QObject *parent = nullptr);

    // А также вызвать в QML этот метод
    Q_INVOKABLE QString getFullInfo() const;

    int number() const;
    QString message() const;

public slots:
    void setNumber(int number);
    void setMessage(QString message);

signals:
    void numberChanged(int number);
    void messageChanged(QString message);

private:
    int m_number;
    QString m_message;
};

#endif // CONTAINER_H
