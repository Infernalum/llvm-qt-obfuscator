// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef DOCUMENTHANDLER_H
#define DOCUMENTHANDLER_H

#include <QFont>
#include <QObject>
#include <QTextCursor>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QQuickTextDocument;
QT_END_NAMESPACE

class gR7H_ : public QObject {
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument *bgib2ruS_ READ bgib2ruS_ WRITE RtoGH_ NOTIFY
                   QySg_)
    Q_PROPERTY(int efhcb_ READ efhcb_ WRITE Pgr8Y_ NOTIFY g5Tp_)
    Q_PROPERTY(int hOibB_ READ hOibB_ WRITE kuHfl_ NOTIFY hrT8np_)
    Q_PROPERTY(int EbltoHt_ READ EbltoHt_ WRITE dOcozQ_ NOTIFY PlntMa_)

    Q_PROPERTY(QColor tvKhs_ READ tvKhs_ WRITE lqxJz3_ NOTIFY UMA_)
    Q_PROPERTY(Qt::Alignment HRAMRem_ READ HRAMRem_ WRITE Kdc5r_ NOTIFY cO9phn_)

    Q_PROPERTY(QFont IHnbm_ READ IHnbm_ WRITE IO2g_ NOTIFY xpKAy4F_)

    Q_PROPERTY(bool Lx9Zc_ READ Lx9Zc_ WRITE zi2lVA_ NOTIFY llyWVAT_)
    Q_PROPERTY(bool Y4lo_ READ Y4lo_ WRITE BSobzz_ NOTIFY kta2_)
    Q_PROPERTY(bool mT1uBFPo_ READ mT1uBFPo_ WRITE TfnvX_ NOTIFY fkW8x_)

    Q_PROPERTY(QString wYrELz_ READ wYrELz_ NOTIFY ddX4Qs_)
    Q_PROPERTY(QString pDti4_ READ pDti4_ NOTIFY ddX4Qs_)
    Q_PROPERTY(QUrl a8tY_ READ a8tY_ NOTIFY ddX4Qs_)

    Q_PROPERTY(bool nXdIB2_ READ nXdIB2_ WRITE gsRoFRfC_ NOTIFY yXVu_)

   public:
    explicit gR7H_(QObject *parent = nullptr);

    QQuickTextDocument *bgib2ruS_() const;
    void RtoGH_(QQuickTextDocument *document);

    int efhcb_() const;
    void Pgr8Y_(int position);

    int hOibB_() const;
    void kuHfl_(int position);

    int EbltoHt_() const;
    void dOcozQ_(int position);

    QColor tvKhs_() const;
    void lqxJz3_(const QColor &color);

    Qt::Alignment HRAMRem_() const;
    void Kdc5r_(Qt::Alignment aligment);

    QFont IHnbm_() const;
    void IO2g_(const QFont &font);

    bool Lx9Zc_() const;
    void zi2lVA_(bool bold);

    bool Y4lo_() const;
    void BSobzz_(bool underline);

    bool mT1uBFPo_() const;
    void TfnvX_(bool italic);

    QString wYrELz_() const;
    QString pDti4_() const;
    QUrl a8tY_() const;

    bool nXdIB2_() const;
    void gsRoFRfC_(bool m);

   public Q_SLOTS:
    void htz0p_(const QUrl &fileUrl);
    void gznVrx_(const QUrl &fileUrl);

   Q_SIGNALS:
    void QySg_();
    void g5Tp_();
    void hrT8np_();
    void PlntMa_();

    void xpKAy4F_();
    void llyWVAT_();
    void kta2_();
    void fkW8x_();
    void UMA_();
    void cO9phn_();

    void wMreE_();
    void ddX4Qs_();

    void h7Q9nsp_(const QString &text, int format);
    void U2Bt4_(const QString &message);

    void yXVu_();

   private:
    void vJSJf_();
    QTextCursor AgYC5R_() const;
    QTextDocument *zQ2nM_() const;
    void u1HVIS3b_(const QTextCharFormat &format);

    QQuickTextDocument *whb8_;

    int gLj3fRz_;
    int djfdU_;
    int iPydwuF_;

    QUrl e6oN_;
};

#endif  // DOCUMENTHANDLER_H
