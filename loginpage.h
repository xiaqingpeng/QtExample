#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage();

signals:
    void loginSuccess(const QString &token);

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onSwitchToLogin();
    void onSwitchToRegister();
    void onLoginReply();
    void onRegisterReply();

private:
    void setupLoginUI();
    void setupRegisterUI();
    void showError(const QString &message);
    void showSuccess(const QString &message);

    // 登录界面控件
    QLineEdit *m_loginEmail;
    QLineEdit *m_loginPassword;
    QPushButton *m_loginButton;
    QPushButton *m_switchToRegisterButton;
    QLabel *m_loginMessage;

    // 注册界面控件
    QLineEdit *m_registerUsername;
    QLineEdit *m_registerEmail;
    QLineEdit *m_registerPassword;
    QLineEdit *m_registerConfirmPassword;
    QPushButton *m_registerButton;
    QPushButton *m_switchToLoginButton;
    QLabel *m_registerMessage;

    // 页面切换
    QStackedWidget *m_pageStack;

    // 网络请求
    QNetworkAccessManager *m_networkManager;
};

#endif // LOGINPAGE_H