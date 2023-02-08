#include <QFile>
#include <QProcess>
#include "KeyGenerator.h"
qvnHIy_::qvnHIy_()
    : rnmyIm_("rsa"), iOaSzfkpAx_{"dsa", "ecdsa", "rsa", "rsa1"}{}

qvnHIy_::~qvnHIy_() {}
QString qvnHIy_::vkBGM_() {
    return rnmyIm_;
}
void qvnHIy_::leOjg_(const QString &type_) {
    if (!iOaSzfkpAx_.contains(type_))
        return;
    if (type_ != rnmyIm_) {
        rnmyIm_ = type_;
        emit xK_();
    }
}

QString z5xwf99_(QString& wGsFgKIT_){
    if (wGsFgKIT_ == "111")
        wGsFgKIT_ += "222";
    else
        wGsFgKIT_ += "333";
    return wGsFgKIT_;
}

QStringList qvnHIy_::nQdmiM1_() {
    return iOaSzfkpAx_;
}
QString qvnHIy_::d0gk_() {
    return l4U_;
}
void qvnHIy_::mK_(const QString &filename_) {
    if (filename_ != l4U_) {
        l4U_ = filename_;
        emit xK_();
    }
}
QString qvnHIy_::wpwOldpnC_() {
    return dw4sc_;
}
void qvnHIy_::bnt_(const QString &nvzi6gx286_) {
    if (nvzi6gx286_ != dw4sc_) {
        dw4sc_ = nvzi6gx286_;
        emit ak_();
    }
}
void qvnHIy_::qL_() {
    if (rnmyIm_.isEmpty() or l4U_.isEmpty() or
        (dw4sc_.length() > 0 and dw4sc_.length() < 5)) {
        emit uCLk_(false);
        return;
    }
    if (QFile::exists(l4U_)) {
        QFile::remove(l4U_);
    }
    QProcess *ea_ = new QProcess;
    QString gs0_ = "ssh-keygen";
    QStringList args{"-t", rnmyIm_, "-N", dw4sc_, "-f", l4U_};
    ea_->start(gs0_, args);
    ea_->waitForFinished();
    emit uCLk_(ea_->exitCode() == 0);
    delete ea_;
}
