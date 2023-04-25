#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>


class AppManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isNightMode READ isNightMode WRITE setIsNightMode NOTIFY isNightModeChanged)
public:
    explicit AppManager(QObject *parent = nullptr);
    bool isNightMode() const;
    void setIsNightMode(bool isNightMode);
public slots:
    void performOperation();
signals:
    void isNightModeChanged();
    void operationFinished(const QString &operationResult);
private:
    bool m_isNightMode = false;
};
#endif // APPMANAGER_H
