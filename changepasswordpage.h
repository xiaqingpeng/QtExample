#ifndef CHANGEPASSWORDPAGE_H
#define CHANGEPASSWORDPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class ChangePasswordPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChangePasswordPage(QWidget *parent = nullptr);

signals:
    void passwordChanged();

private slots:
    void onChangePasswordClicked();
    void onChangePasswordReply();

private:
    QLineEdit *m_oldPasswordEdit;
    QLineEdit *m_newPasswordEdit;
    QLineEdit *m_confirmPasswordEdit;
    QPushButton *m_changePasswordBtn;
    QNetworkAccessManager *m_networkManager;
};

#endif // CHANGEPASSWORDPAGE_H