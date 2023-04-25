#ifndef APPMANAGER_H
#define APPMANAGER_H

#include <QObject>

class i1IiJ4E_ : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool b0i2h_ READ b0i2h_ WRITE G50d_ NOTIFY y96Jm_)
   public:
    explicit i1IiJ4E_(QObject *parent = nullptr);
    bool b0i2h_() const;
    void G50d_(bool isNightMode);
   public slots:
    void SEmVOs_();
   signals:
    void y96Jm_();
    void j49X_(const QString &operationResult);

   private:
    bool Q1tPXgh_ = false;
};
#endif  // APPMANAGER_H
