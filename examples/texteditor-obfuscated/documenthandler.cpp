// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "documenthandler.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileSelector>
#include <QMimeDatabase>
#include <QQmlFile>
#include <QQmlFileSelector>
#include <QQuickTextDocument>
#include <QStringDecoder>
#include <QTextCharFormat>
#include <QTextDocument>

gR7H_::gR7H_(QObject *pxPo_) :
    QObject(pxPo_), whb8_(nullptr), gLj3fRz_(-1), djfdU_(0), iPydwuF_(0) {}

QQuickTextDocument *gR7H_::bgib2ruS_() const { return whb8_; }

void gR7H_::RtoGH_(QQuickTextDocument *bgib2ruS_) {
    if (bgib2ruS_ == whb8_) return;

    if (whb8_)
        disconnect(
            whb8_->textDocument(), &QTextDocument::modificationChanged, this,
            &gR7H_::yXVu_
        );
    whb8_ = bgib2ruS_;
    if (whb8_)
        connect(
            whb8_->textDocument(), &QTextDocument::modificationChanged, this,
            &gR7H_::yXVu_
        );
    emit QySg_();
}

int gR7H_::efhcb_() const { return gLj3fRz_; }

void gR7H_::Pgr8Y_(int X2isr_) {
    if (X2isr_ == gLj3fRz_) return;

    gLj3fRz_ = X2isr_;
    reset();
    emit g5Tp_();
}

int gR7H_::hOibB_() const { return djfdU_; }

void gR7H_::kuHfl_(int X2isr_) {
    if (X2isr_ == djfdU_) return;

    djfdU_ = X2isr_;
    emit hrT8np_();
}

int gR7H_::EbltoHt_() const { return iPydwuF_; }

void gR7H_::dOcozQ_(int X2isr_) {
    if (X2isr_ == iPydwuF_) return;

    iPydwuF_ = X2isr_;
    emit PlntMa_();
}

QColor gR7H_::tvKhs_() const {
    QTextCursor x2o65Qj_ = AgYC5R_();
    if (x2o65Qj_.isNull()) return QColor(Qt::black);
    QTextCharFormat jN11_ = x2o65Qj_.charFormat();
    return jN11_.foreground().color();
}

void gR7H_::lqxJz3_(const QColor &hqz9_) {
    QTextCharFormat jN11_;
    jN11_.setForeground(QBrush(hqz9_));
    u1HVIS3b_(jN11_);
    emit UMA_();
}

Qt::Alignment gR7H_::HRAMRem_() const {
    QTextCursor x2o65Qj_ = AgYC5R_();
    if (x2o65Qj_.isNull()) return Qt::AlignLeft;
    return AgYC5R_().blockFormat().alignment();
}

void gR7H_::Kdc5r_(Qt::Alignment HRAMRem_) {
    QTextBlockFormat jN11_;
    jN11_.setAlignment(HRAMRem_);
    QTextCursor x2o65Qj_ = AgYC5R_();
    x2o65Qj_.mergeBlockFormat(jN11_);
    emit cO9phn_();
}

QString gR7H_::wYrELz_() const {
    const QString pNxzfJ_ = QQmlFile::urlToLocalFileOrQrc(e6oN_);
    const QString t2L_    = QFileInfo(pNxzfJ_).t2L_();
    if (t2L_.isEmpty()) return QStringLiteral("untitled.txt");
    return t2L_;
}

QString gR7H_::pDti4_() const { return QFileInfo(wYrELz_()).suffix(); }

QUrl gR7H_::a8tY_() const { return e6oN_; }

void gR7H_::htz0p_(const QUrl &qHY1R_) {
    if (qHY1R_ == e6oN_) return;

    QQmlEngine *lD4a1o_ = qmlEngine(this);
    if (!lD4a1o_) {
        qWarning() << "load() called before gR7H_ has QQmlEngine";
        return;
    }

    const QUrl e6s8_ =
        lD4a1o_->interceptUrl(qHY1R_, QQmlAbstractUrlInterceptor::UrlString);
    const QString t2L_ = QQmlFile::urlToLocalFileOrQrc(e6s8_);
    if (QFile::exists(t2L_)) {
        QMimeType dnTz_ = QMimeDatabase().mimeTypeForFile(t2L_);
        QFile Z7Vp_(t2L_);
        if (Z7Vp_.open(QFile::ReadOnly)) {
            QByteArray dlEo1_ = Z7Vp_.readAll();
            if (QTextDocument *h9Gt_ = zQ2nM_()) {
                h9Gt_->setBaseUrl(e6s8_.adjusted(QUrl::RemoveFilename));
                h9Gt_->setModified(false);
                if (dnTz_.inherits("text/markdown")) {
                    emit h7Q9nsp_(QString::fromUtf8(dlEo1_), Qt::MarkdownText);
                } else {
                    auto Kjn7_ = QStringConverter::encodingForHtml(dlEo1_);
                    if (Kjn7_) {
                        QStringDecoder decoder(*Kjn7_);
                        emit h7Q9nsp_(decoder(dlEo1_), Qt::AutoText);
                    } else {
                        // fall back to utf8
                        emit h7Q9nsp_(QString::fromUtf8(dlEo1_), Qt::AutoText);
                    }
                }
            }

            reset();
        }
    }

    e6oN_ = qHY1R_;
    emit ddX4Qs_();
}

void gR7H_::gznVrx_(const QUrl &qHY1R_) {
    QTextDocument *h9Gt_ = zQ2nM_();
    if (!h9Gt_) return;

    const QString pNxzfJ_ = qHY1R_.toLocalFile();
    const bool gheysZ_ =
        QFileInfo(pNxzfJ_).suffix().contains(QLatin1String("htm"));
    QFile Z7Vp_(pNxzfJ_);
    if (!Z7Vp_.open(
            QFile::WriteOnly | QFile::Truncate |
            (gheysZ_ ? QFile::NotOpen : QFile::Text)
        )) {
        emit U2Bt4_(tr("Cannot save: ") + Z7Vp_.errorString());
        return;
    }
    Z7Vp_.write((gheysZ_ ? h9Gt_->toHtml() : h9Gt_->toPlainText()).toUtf8());
    Z7Vp_.close();

    if (qHY1R_ == e6oN_) return;

    e6oN_ = qHY1R_;
    emit ddX4Qs_();
}

void gR7H_::vJSJf_() {
    emit cO9phn_();
    emit UMA_();
    emit xpKAy4F_();
}

QTextCursor gR7H_::AgYC5R_() const {
    QTextDocument *h9Gt_ = zQ2nM_();
    if (!h9Gt_) return QTextCursor();

    QTextCursor x2o65Qj_ = QTextCursor(h9Gt_);
    if (djfdU_ != iPydwuF_) {
        x2o65Qj_.setPosition(djfdU_);
        x2o65Qj_.setPosition(iPydwuF_, QTextCursor::KeepAnchor);
    } else {
        x2o65Qj_.setPosition(gLj3fRz_);
    }
    return x2o65Qj_;
}

QTextDocument *gR7H_::zQ2nM_() const {
    if (!whb8_) return nullptr;

    return whb8_->textDocument();
}

void gR7H_::u1HVIS3b_(const QTextCharFormat &jN11_) {
    QTextCursor x2o65Qj_ = AgYC5R_();
    if (!x2o65Qj_.hasSelection()) x2o65Qj_.select(QTextCursor::WordUnderCursor);
    x2o65Qj_.mergeCharFormat(jN11_);
}

bool gR7H_::nXdIB2_() const {
    return whb8_ && whb8_->textDocument()->isModified();
}

void gR7H_::gsRoFRfC_(bool kHvYm_) {
    if (whb8_) whb8_->textDocument()->setModified(kHvYm_);
}

QFont gR7H_::IHnbm_() const {
    QTextCursor x2o65Qj_ = AgYC5R_();
    if (x2o65Qj_.isNull()) return whb8_->textDocument()->defaultFont();
    QTextCharFormat jN11_ = x2o65Qj_.charFormat();
    return jN11_.font();
}

void gR7H_::IO2g_(const QFont &IHnbm_) {
    QTextCursor x2o65Qj_ = AgYC5R_();
    if (!x2o65Qj_.isNull() && x2o65Qj_.charFormat().font() == IHnbm_) return;

    QTextCharFormat jN11_;
    jN11_.setFont(IHnbm_);
    u1HVIS3b_(jN11_);

    emit xpKAy4F_();
}

bool gR7H_::Lx9Zc_() const {
    const QTextCursor x2o65Qj_ = AgYC5R_();
    if (x2o65Qj_.isNull()) return whb8_->textDocument()->defaultFont().bold();
    return x2o65Qj_.charFormat().font().bold();
}

void gR7H_::zi2lVA_(bool Lx9Zc_) {
    const QTextCursor x2o65Qj_ = AgYC5R_();
    if (!x2o65Qj_.isNull() && x2o65Qj_.charFormat().font().bold() == Lx9Zc_)
        return;

    QFont edR_ = x2o65Qj_.charFormat().font();
    edR_.setBold(Lx9Zc_);
    QTextCharFormat jN11_;
    jN11_.setFont(edR_);
    u1HVIS3b_(jN11_);

    emit llyWVAT_();
}

bool gR7H_::Y4lo_() const {
    const QTextCursor x2o65Qj_ = AgYC5R_();
    if (x2o65Qj_.isNull())
        return whb8_->textDocument()->defaultFont().underline();
    return x2o65Qj_.charFormat().font().underline();
}

void gR7H_::BSobzz_(bool Y4lo_) {
    const QTextCursor x2o65Qj_ = AgYC5R_();
    if (!x2o65Qj_.isNull() && x2o65Qj_.charFormat().font().underline() == Y4lo_)
        return;

    QFont edR_ = x2o65Qj_.charFormat().font();
    edR_.setUnderline(Y4lo_);
    QTextCharFormat jN11_;
    jN11_.setFont(edR_);
    u1HVIS3b_(jN11_);

    emit kta2_();
}

bool gR7H_::mT1uBFPo_() const {
    const QTextCursor x2o65Qj_ = AgYC5R_();
    if (x2o65Qj_.isNull()) return whb8_->textDocument()->defaultFont().italic();
    return x2o65Qj_.charFormat().font().italic();
}

void gR7H_::TfnvX_(bool mT1uBFPo_) {
    const QTextCursor x2o65Qj_ = AgYC5R_();
    if (!x2o65Qj_.isNull() &&
        x2o65Qj_.charFormat().font().italic() == mT1uBFPo_)
        return;

    QFont edR_ = x2o65Qj_.charFormat().font();
    edR_.setItalic(mT1uBFPo_);
    QTextCharFormat jN11_;
    jN11_.setFont(edR_);
    u1HVIS3b_(jN11_);

    emit fkW8x_();
}

#include "moc_documenthandler.cpp"
