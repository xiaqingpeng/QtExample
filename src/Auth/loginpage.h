#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QNetworkReply>
#include <QCheckBox>
#include <QEvent>
#include "networkmanager.h"
#include "ILoginView.h"

// analytics.h 在 loginpage.cpp 中被大量使用，因此需要在头文件中包含
#include "analytics.h" // IWYU pragma: export

class LoginController;

class LoginPage : public QWidget, public ILoginView
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage();
    void clearUserInfo();
    void changeEvent(QEvent *event) override; // 处理语言变化事件

signals:
    void loginSuccess(const QString &token);

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onSwitchToLogin();
    void onSwitchToRegister();
    void onThemeChanged(); // 主题变化槽函数
    void retranslateUi(); // 多语言更新槽函数

private:
    void setupLoginUI();
    void setupRegisterUI();
    void applyThemeToWidget(QWidget* widget); // 应用主题到特定widget

public slots:
    void showError(const QString &message) override;
    void showSuccess(const QString &message) override;
    void applyTheme(); // 应用主题方法

public:
    void saveUserInfo(const QString &token, const QString &email, const QString &password,
                       const QString &userId = "", const QString &username = "",
                       const QString &avatar = "", const QString &createTime = "");
    void loadUserInfo(QString &token, QString &email, QString &password);
    bool checkAutoLogin();
    QString encryptPassword(const QString &password);
    QString decryptPassword(const QString &encrypted);

private:
    // ILoginView 接口实现
    QString email() const override;
    QString password() const override;
    bool rememberPasswordChecked() const override;

    void setEmail(const QString &value) override;
    void setPassword(const QString &value) override;

    void setBusy(bool busy) override;

private:
    // 登录界面控件
    QLineEdit *m_loginEmail;
    QLineEdit *m_loginPassword;
    QPushButton *m_loginButton;
    QPushButton *m_switchToRegisterButton;
    QLabel *m_loginMessage;
    QCheckBox *m_rememberPassword;

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
    NetworkManager *m_networkManager;

    // 控制器（MVC 中的 C）
    LoginController *m_loginController;
};

#endif // LOGINPAGE_H