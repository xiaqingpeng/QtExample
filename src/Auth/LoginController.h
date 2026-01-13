#ifndef LOGINCONTROLLER_H
#define LOGINCONTROLLER_H

#include <QObject>
#include <QElapsedTimer>
#include <QJsonObject>

#include "ILoginView.h"
#include "networkmanager.h"

// 登录控制器（MVC 中的 C）：承载登录与自动登录的业务逻辑
class LoginController : public QObject
{
    Q_OBJECT
public:
    explicit LoginController(ILoginView *view,
                             NetworkManager *networkManager,
                             QObject *parent = nullptr);

public slots:
    // 用户点击登录按钮时调用
    void handleLoginRequested();
    // 检查并执行自动登录（对应原来的 checkAutoLogin）
    void handleAutoLoginCheck();

signals:
    // 登录成功后发出，向外部（例如 MainUIWindow）暴露 token
    void loginSucceeded(const QString &token);

private:
    ILoginView *m_view;               // 由外部（LoginPage）管理生命周期
    NetworkManager *m_networkManager; // 由外部管理生命周期
};

#endif // LOGINCONTROLLER_H
