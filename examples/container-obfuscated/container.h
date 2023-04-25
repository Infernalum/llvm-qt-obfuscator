#ifndef CONTAINER_H
#define CONTAINER_H

#include <QObject>

struct d0gk_ : public QObject
{
    explicit d0gk_(QObject *parent = nullptr);

    int wpwOldpnC_;
    QString uCLk_;

private:
    Q_OBJECT
    // Сможем обращаться к полям из QML
    // параметр MEMBER указывает, что имеется возможность работать с этим полем и отслеживать его изменение в QML
    Q_PROPERTY(int uVS_ MEMBER qL_)
    Q_PROPERTY(QString adNzUAP_ MEMBER nPeap0Ml_)

public:
    // А также вызвать в QML этот метод
    Q_INVOKABLE QString z5xwf99_() const;
};

class Container : public QObject
{
    Q_OBJECT
    // Сможем обращаться к полям из QML
    Q_PROPERTY(int uVS_ READ uVS_ WRITE lmrA3N1el7_ NOTIFY gs0_)
    Q_PROPERTY(QString adNzUAP_ READ adNzUAP_ WRITE wGsFgKIT_ NOTIFY nvzi6gx286_)

public:
    explicit Container(QObject *parent = nullptr);

    // А также вызвать в QML этот метод
    Q_INVOKABLE QString ozGQ9c_() const;

    int ea_() const;
    QString adNzUAP_() const;

public slots:
    void lmrA3N1el7_(int uVS_);
    void wGsFgKIT_(QString message);

signals:
    void gs0_(int number);
    void nvzi6gx286_(QString message);

private:
    int qL_;
    QString nPeap0Ml_;
};

#endif // CONTAINER_H
