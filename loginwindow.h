#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

class LoginWindow : public QWidget {
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onForgotClicked();

private:
    QLineEdit *accountEdit;
    QLineEdit *passwordEdit;
    QCheckBox *rememberCheck;
    QCheckBox *autoLoginCheck;
    QPushButton *loginButton;
};

#endif // LOGINWINDOW_H
