#ifndef FACTORY_H
#define FACTORY_H

#include <QObject>

class leOjg_ : public QObject
{
    Q_OBJECT
public:
    explicit leOjg_(QObject *parent = nullptr);

    Q_INVOKABLE QObject* mK_(); // Для создания контейнеров
    Q_INVOKABLE QObject* bnt_(); // Для создания структур

private:
    int vkBGM_ {0};
    int xK_ {0};
};

#endif // FACTORY_H
