#include "LoginController.h"
#include "../Analytics/analytics.h"
#include <QSettings>
#include <QTimer>

LoginController::LoginController(ILoginView *view,
                                 NetworkManager *networkManager,
                                 QObject *parent)
    : QObject(parent)
    , m_view(view)
    , m_networkManager(networkManager)
{
}

void LoginController::handleLoginRequested()
{
    if (!m_view || !m_networkManager) {
        return;
    }

    const QString email = m_view->email().trimmed();
    const QString password = m_view->password();

    if (email.isEmpty() || password.isEmpty()) {
        const QString errorMsg = QObject::tr("邮箱和密码不能为空");
        // 埋点与错误提示
        Analytics::SDK::instance()->trackError("validation_error", errorMsg, {
            {"page", "login"},
            {"error_field", email.isEmpty() ? "email" : "password"}
        });
        m_view->showError(errorMsg);
        return;
    }

    // 开始性能计时
    QElapsedTimer timer;
    timer.start();

    // 设置视图为忙碌状态
    m_view->setBusy(true);

    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    m_networkManager->post("http://120.48.95.51:7001/login",
                           json,
                           [this, timer, email](const QJsonObject &response) {
        const qint64 loginTime = timer.elapsed();

        const int code = response["code"].toInt();
        const QString msg = response["msg"].toString();

        if (code == 0) {
            QString token;
            if (response.contains("token")) {
                token = response["token"].toString();
            }

            // 获取用户信息
            QJsonObject data = response["data"].toObject();
            const QString userId = QString::number(data["id"].toInt());

            // 先设置用户ID到Analytics SDK
            Analytics::SDK::instance()->setUserId(userId);

            // 然后追踪登录成功事件
            Analytics::SDK::instance()->track("login_success", {
                {"login_time", loginTime},
                {"email", email}
            });

            // 追踪登录性能
            Analytics::SDK::instance()->trackPerformance("login_request_time", loginTime, {
                {"page", "login"},
                {"status", "success"}
            });

            m_view->showSuccess(QObject::tr("登录成功！"));
            emit loginSucceeded(token);
        } else {
            // 追踪登录失败事件
            Analytics::SDK::instance()->trackError("login_failed", msg, {
                {"page", "login"},
                {"error_code", QString::number(code)},
                {"login_time", loginTime}
            });

            // 追踪登录性能
            Analytics::SDK::instance()->trackPerformance("login_request_time", loginTime, {
                {"page", "login"},
                {"status", "failed"},
                {"error_code", QString::number(code)}
            });

            m_view->showError(msg);
        }

        m_view->setBusy(false);
    },
    [this, timer](const QString &errorMsg) {
        const qint64 loginTime = timer.elapsed();

        // 追踪网络错误
        Analytics::SDK::instance()->trackError("network_error", errorMsg, {
            {"page", "login"},
            {"login_time", loginTime}
        });

        m_view->showError(QObject::tr("网络错误：%1").arg(errorMsg));
        m_view->setBusy(false);
    });
}

void LoginController::handleAutoLoginCheck()
{
    if (!m_view) {
        return;
    }

    QString token;
    QString email;
    QString password;

    QSettings settings("YourCompany", "QtApp");
    token = settings.value("user/token", "").toString();
    email = settings.value("user/email", "").toString();

    const QString encryptedPassword = settings.value("user/password", "").toString();
    if (!encryptedPassword.isEmpty()) {
        // 自动登录时，LoginPage 会负责解密密码并设置到视图
        password.clear(); // 控制器这里不解密，只负责流程
    }

    if (!email.isEmpty()) {
        // 填充视图中的邮箱
        m_view->setEmail(email);

        if (!encryptedPassword.isEmpty()) {
            // 如果保存了密码，视图会在自身逻辑中填充并触发登录
            m_view->showSuccess(QObject::tr("正在自动登录..."));
            // 延迟500ms后自动触发登录
            QTimer::singleShot(500, this, &LoginController::handleLoginRequested);
        } else {
            m_view->showSuccess(QObject::tr("检测到已登录用户，请输入密码继续"));
        }
    }
}
