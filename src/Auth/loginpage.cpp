#include "loginpage.h"
#include "../Styles/theme_manager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>
#include <QNetworkProxy>
#include <QSettings>
#include <QCheckBox>
#include <QCryptographicHash>
#include <QByteArray>
#include <QDebug>
#include <QElapsedTimer>

LoginPage::LoginPage(QWidget *parent) : QWidget(parent)
{
    m_networkManager = new NetworkManager(this);

    m_pageStack = new QStackedWidget(this);

    setupLoginUI();
    setupRegisterUI();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_pageStack);

    // 默认显示登录页面
    m_pageStack->setCurrentIndex(0);
    
    // 初始化Analytics SDK
    Analytics::Config config;
    config.serverUrl = "http://120.48.95.51:7001/api/analytics/events";
    config.appId = "qt-example-app";
    config.enableDebug = true;
    config.batchSize = 1;  // 降低批量大小，每个事件立即发送
    config.flushInterval = 300;  // 增加刷新间隔到5分钟，减少网络请求频率
    config.enablePersistence = true;
    Analytics::SDK::instance()->initialize(config);
    
    // 追踪登录页面浏览事件
    Analytics::SDK::instance()->trackView("login_page", {
        {"page_title", "登录页面"}
    });
    
    // 检查是否有保存的登录信息，如果有则自动登录
    checkAutoLogin();
    
    // 连接主题变化信号
    connect(ThemeManager::instance(), &ThemeManager::themeChanged, 
            this, &LoginPage::onThemeChanged);
    
    // 应用当前主题
    applyTheme();
}

LoginPage::~LoginPage()
{
}

void LoginPage::setupLoginUI()
{
    // 主容器
    QWidget *loginWidget = new QWidget();
    loginWidget->setObjectName("loginWidget");
    QVBoxLayout *mainLayout = new QVBoxLayout(loginWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 顶部装饰区域
    QWidget *topBar = new QWidget();
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(80);
    
    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(24, 0, 24, 0);
    
    // Logo标签
    QLabel *logoLabel = new QLabel("Qt 现代化应用");
    logoLabel->setObjectName("logoLabel");
    topBarLayout->addStretch();
    topBarLayout->addWidget(logoLabel);
    topBarLayout->addStretch();
    
    mainLayout->addWidget(topBar);
    
    // 中心内容区域
    QWidget *centerWidget = new QWidget();
    QVBoxLayout *centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setContentsMargins(40, 40, 40, 40);
    centerLayout->setAlignment(Qt::AlignCenter);
    
    // 登录卡片
    QWidget *cardWidget = new QWidget();
    cardWidget->setObjectName("loginCard");
    cardWidget->setMaximumWidth(400);
    cardWidget->setMinimumWidth(350);
    
    QVBoxLayout *cardLayout = new QVBoxLayout(cardWidget);
    cardLayout->setSpacing(24);
    cardLayout->setContentsMargins(40, 40, 40, 40);

    // 标题
    QLabel *titleLabel = new QLabel("欢迎登录");
    titleLabel->setObjectName("loginTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(titleLabel);

    // 表单区域
    QVBoxLayout *formLayout = new QVBoxLayout();
    formLayout->setSpacing(20);

    // 邮箱输入
    m_loginEmail = new QLineEdit();
    m_loginEmail->setObjectName("loginEmail");
    m_loginEmail->setPlaceholderText("邮箱/手机号");
    m_loginEmail->setMinimumHeight(44);
    formLayout->addWidget(m_loginEmail);

    // 密码输入
    m_loginPassword = new QLineEdit();
    m_loginPassword->setObjectName("loginPassword");
    m_loginPassword->setPlaceholderText("密码");
    m_loginPassword->setEchoMode(QLineEdit::Password);
    m_loginPassword->setMinimumHeight(44);
    formLayout->addWidget(m_loginPassword);

    // 记住密码复选框
    m_rememberPassword = new QCheckBox("记住密码");
    m_rememberPassword->setObjectName("rememberPassword");
    formLayout->addWidget(m_rememberPassword);

    cardLayout->addLayout(formLayout);

    // 登录按钮
    m_loginButton = new QPushButton("登录");
    m_loginButton->setObjectName("loginButton");
    m_loginButton->setMinimumHeight(44);
    cardLayout->addWidget(m_loginButton);

    // 消息标签
    m_loginMessage = new QLabel();
    m_loginMessage->setObjectName("loginMessage");
    m_loginMessage->setAlignment(Qt::AlignCenter);
    m_loginMessage->setWordWrap(true);
    cardLayout->addWidget(m_loginMessage);

    // 切换到注册按钮
    m_switchToRegisterButton = new QPushButton("还没有账号？立即注册");
    m_switchToRegisterButton->setObjectName("switchButton");
    cardLayout->addWidget(m_switchToRegisterButton);

    centerLayout->addWidget(cardWidget);
    mainLayout->addWidget(centerWidget);
    
    // 连接信号
    connect(m_loginButton, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(m_switchToRegisterButton, &QPushButton::clicked, this, &LoginPage::onSwitchToRegister);
    
    // 添加到页面栈
    m_pageStack->addWidget(loginWidget);
    
    // 应用主题样式到登录页面
    QTimer::singleShot(0, this, [this, loginWidget]() {
        applyThemeToWidget(loginWidget);
    });
}

void LoginPage::setupRegisterUI()
{
    // 主容器
    QWidget *registerWidget = new QWidget();
    registerWidget->setObjectName("registerWidget");
    QVBoxLayout *mainLayout = new QVBoxLayout(registerWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 顶部装饰区域
    QWidget *topBar = new QWidget();
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(80);
    
    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(24, 0, 24, 0);
    
    // Logo标签
    QLabel *logoLabel = new QLabel("Qt 现代化应用");
    logoLabel->setObjectName("logoLabel");
    topBarLayout->addStretch();
    topBarLayout->addWidget(logoLabel);
    topBarLayout->addStretch();
    
    mainLayout->addWidget(topBar);
    
    // 中心内容区域
    QWidget *centerWidget = new QWidget();
    QVBoxLayout *centerLayout = new QVBoxLayout(centerWidget);
    centerLayout->setContentsMargins(40, 40, 40, 40);
    centerLayout->setAlignment(Qt::AlignCenter);
    
    // 注册卡片
    QWidget *cardWidget = new QWidget();
    cardWidget->setObjectName("registerCard");
    cardWidget->setMaximumWidth(400);
    cardWidget->setMinimumWidth(350);
    
    QVBoxLayout *cardLayout = new QVBoxLayout(cardWidget);
    cardLayout->setSpacing(24);
    cardLayout->setContentsMargins(40, 40, 40, 40);

    // 标题
    QLabel *titleLabel = new QLabel("注册账号");
    titleLabel->setObjectName("registerTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(titleLabel);

    // 表单区域
    QVBoxLayout *formLayout = new QVBoxLayout();
    formLayout->setSpacing(20);

    // 用户名输入
    m_registerUsername = new QLineEdit();
    m_registerUsername->setObjectName("registerUsername");
    m_registerUsername->setPlaceholderText("昵称");
    m_registerUsername->setMinimumHeight(44);
    formLayout->addWidget(m_registerUsername);

    // 邮箱输入
    m_registerEmail = new QLineEdit();
    m_registerEmail->setObjectName("registerEmail");
    m_registerEmail->setPlaceholderText("邮箱");
    m_registerEmail->setMinimumHeight(44);
    formLayout->addWidget(m_registerEmail);

    // 密码输入
    m_registerPassword = new QLineEdit();
    m_registerPassword->setObjectName("registerPassword");
    m_registerPassword->setPlaceholderText("密码");
    m_registerPassword->setEchoMode(QLineEdit::Password);
    m_registerPassword->setMinimumHeight(44);
    formLayout->addWidget(m_registerPassword);

    // 确认密码输入
    m_registerConfirmPassword = new QLineEdit();
    m_registerConfirmPassword->setObjectName("registerConfirmPassword");
    m_registerConfirmPassword->setPlaceholderText("确认密码");
    m_registerConfirmPassword->setEchoMode(QLineEdit::Password);
    m_registerConfirmPassword->setMinimumHeight(44);
    formLayout->addWidget(m_registerConfirmPassword);

    cardLayout->addLayout(formLayout);

    // 注册按钮
    m_registerButton = new QPushButton("注册");
    m_registerButton->setObjectName("registerButton");
    m_registerButton->setMinimumHeight(44);
    cardLayout->addWidget(m_registerButton);

    // 消息标签
    m_registerMessage = new QLabel();
    m_registerMessage->setObjectName("registerMessage");
    m_registerMessage->setAlignment(Qt::AlignCenter);
    m_registerMessage->setWordWrap(true);
    cardLayout->addWidget(m_registerMessage);

    // 切换到登录按钮
    m_switchToLoginButton = new QPushButton("已有账号？立即登录");
    m_switchToLoginButton->setObjectName("switchButton");
    cardLayout->addWidget(m_switchToLoginButton);

    centerLayout->addWidget(cardWidget);
    mainLayout->addWidget(centerWidget);
    
    // 连接信号
    connect(m_registerButton, &QPushButton::clicked, this, &LoginPage::onRegisterClicked);
    connect(m_switchToLoginButton, &QPushButton::clicked, this, &LoginPage::onSwitchToLogin);
    
    // 添加到页面栈
    m_pageStack->addWidget(registerWidget);
    
    // 应用主题样式到注册页面
    QTimer::singleShot(0, this, [this, registerWidget]() {
        applyThemeToWidget(registerWidget);
    });
}

void LoginPage::onLoginClicked()
{
    // 追踪登录按钮点击事件
    Analytics::SDK::instance()->trackClick("login_button", {
        {"page", "login"},
        {"button_text", "登录"}
    });
    
    QString email = m_loginEmail->text().trimmed();
    QString password = m_loginPassword->text();

    // 处理登录输入

    if (email.isEmpty() || password.isEmpty()) {
        // 追踪表单验证错误
        Analytics::SDK::instance()->trackError("validation_error", "邮箱和密码不能为空", {
            {"page", "login"},
            {"error_field", email.isEmpty() ? "email" : "password"}
        });
        showError("邮箱和密码不能为空");
        return;
    }

    // 开始性能计时
    QElapsedTimer timer;
    timer.start();
    
    m_loginButton->setEnabled(false);
    m_loginButton->setText("登录中...");

    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    // 使用NetworkManager发送登录请求，自动添加平台识别头
    m_networkManager->post("http://120.48.95.51:7001/login",
                           json,
                           [this, timer](const QJsonObject &response) {
        // 记录登录性能
        qint64 loginTime = timer.elapsed();
        
        // 成功回调
        int code = response["code"].toInt();
        QString msg = response["msg"].toString();

        // 处理登录响应

        if (code == 0) {
            QString cookie;
            if (response.contains("token")) {
                cookie = response["token"].toString();
            }

            // 处理Cookie

            // 获取用户信息
            QJsonObject data = response["data"].toObject();
            QString userId = QString::number(data["id"].toInt());
            QString username = data["username"].toString();
            QString email = data["email"].toString();
            QString avatar = data["avatar"].toString();
            QString createTime = data["createTime"].toString();

            // 保存用户信息用于自动登录和用户信息页面
            QString emailInput = m_loginEmail->text().trimmed();
            QString password = m_rememberPassword->isChecked() ? m_loginPassword->text() : "";
            saveUserInfo(cookie, emailInput, password, userId, username, avatar, createTime);
            
            // 先设置用户ID到Analytics SDK
            Analytics::SDK::instance()->setUserId(userId);
            
            // 然后追踪登录成功事件（此时会使用正确的用户ID）
            Analytics::SDK::instance()->track("login_success", {
                {"login_time", loginTime},
                {"email", m_loginEmail->text().trimmed()}
            });
            
            // 追踪登录性能
            Analytics::SDK::instance()->trackPerformance("login_request_time", loginTime, {
                {"page", "login"},
                {"status", "success"}
            });
            
            showSuccess("登录成功！");
            emit loginSuccess(cookie);
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
            
            showError(msg);
        }

        m_loginButton->setEnabled(true);
        m_loginButton->setText("登录");
    },
    [this, timer](const QString &errorMsg) {
        // 记录登录失败性能
        qint64 loginTime = timer.elapsed();
        
        // 追踪网络错误
        Analytics::SDK::instance()->trackError("network_error", errorMsg, {
            {"page", "login"},
            {"login_time", loginTime}
        });
        
        // 错误回调
        // 登录错误
        showError("网络错误：" + errorMsg);

        m_loginButton->setEnabled(true);
        m_loginButton->setText("登录");
    });
}

void LoginPage::onRegisterClicked()
{
    // 追踪注册按钮点击事件
    Analytics::SDK::instance()->trackClick("register_button", {
        {"page", "register"},
        {"button_text", "注册"}
    });
    
    QString username = m_registerUsername->text().trimmed();
    QString email = m_registerEmail->text().trimmed();
    QString password = m_registerPassword->text();
    QString confirmPassword = m_registerConfirmPassword->text();

    if (username.isEmpty() || email.isEmpty() || password.isEmpty()) {
        // 追踪表单验证错误
        Analytics::SDK::instance()->trackError("validation_error", "用户名、邮箱和密码不能为空", {
            {"page", "register"}
        });
        showError("用户名、邮箱和密码不能为空");
        return;
    }

    if (password != confirmPassword) {
        // 追踪表单验证错误
        Analytics::SDK::instance()->trackError("validation_error", "两次输入的密码不一致", {
            {"page", "register"},
            {"error_field", "password_confirm"}
        });
        showError("两次输入的密码不一致");
        return;
    }

    // 开始性能计时
    QElapsedTimer timer;
    timer.start();
    
    m_registerButton->setEnabled(false);
    m_registerButton->setText("注册中...");

    QJsonObject json;
    json["username"] = username;
    json["email"] = email;
    json["password"] = password;
    json["confirmPassword"] = confirmPassword;

    // 使用NetworkManager发送注册请求，自动添加平台识别头
    m_networkManager->post("http://120.48.95.51:7001/register",
                           json,
                           [this, timer](const QJsonObject &response) {
        // 记录注册性能
        qint64 registerTime = timer.elapsed();
        
        // 成功回调
        int code = response["code"].toInt();
        QString msg = response["msg"].toString();

        if (code == 0) {
            // 追踪注册成功事件
            Analytics::SDK::instance()->track("register_success", {
                {"register_time", registerTime},
                {"email", m_registerEmail->text().trimmed()},
                {"username", m_registerUsername->text().trimmed()}
            });
            
            // 追踪注册性能
            Analytics::SDK::instance()->trackPerformance("register_request_time", registerTime, {
                {"page", "register"},
                {"status", "success"}
            });
            
            showSuccess("注册成功！请登录");
            // 延迟1秒后切换到登录页面
            QTimer::singleShot(1000, this, &LoginPage::onSwitchToLogin);
        } else {
            // 追踪注册失败事件
            Analytics::SDK::instance()->trackError("register_failed", msg, {
                {"page", "register"},
                {"error_code", QString::number(code)},
                {"register_time", registerTime}
            });
            
            // 追踪注册性能
            Analytics::SDK::instance()->trackPerformance("register_request_time", registerTime, {
                {"page", "register"},
                {"status", "failed"},
                {"error_code", QString::number(code)}
            });
            
            showError(msg);
        }

        m_registerButton->setEnabled(true);
        m_registerButton->setText("注册");
    },
    [this, timer](const QString &errorMsg) {
        // 记录注册失败性能
        qint64 registerTime = timer.elapsed();
        
        // 追踪网络错误
        Analytics::SDK::instance()->trackError("network_error", errorMsg, {
            {"page", "register"},
            {"register_time", registerTime}
        });
        
        // 错误回调
        // 注册错误
        showError("网络错误：" + errorMsg);

        m_registerButton->setEnabled(true);
        m_registerButton->setText("注册");
    });
}

void LoginPage::onSwitchToLogin()
{
    // 追踪切换到登录页面事件
    Analytics::SDK::instance()->trackView("login_page", {
        {"from", "register"}
    });
    
    m_pageStack->setCurrentIndex(0);
    m_registerMessage->clear();
}

void LoginPage::onSwitchToRegister()
{
    // 追踪切换到注册页面事件
    Analytics::SDK::instance()->trackView("register_page", {
        {"from", "login"}
    });
    
    m_pageStack->setCurrentIndex(1);
    m_loginMessage->clear();
}

void LoginPage::showError(const QString &message)
{
    m_loginMessage->setText(message);
    m_loginMessage->setStyleSheet("color: #dc3545; font-size: 12px;");
    m_registerMessage->setText(message);
    m_registerMessage->setStyleSheet("color: #dc3545; font-size: 12px;");
}

void LoginPage::showSuccess(const QString &message)
{
    m_loginMessage->setText(message);
    m_loginMessage->setStyleSheet("color: #28a745; font-size: 12px;");
    m_registerMessage->setText(message);
    m_registerMessage->setStyleSheet("color: #28a745; font-size: 12px;");
}

void LoginPage::saveUserInfo(const QString &token, const QString &email, const QString &password,
                                    const QString &userId, const QString &username,
                                    const QString &avatar, const QString &createTime)
{
    QSettings settings("YourCompany", "QtApp");
    
    // 保存用户信息
    
    settings.setValue("user/token", token);
    settings.setValue("user/email", email);
    
    // 保存用户详细信息
    settings.setValue("user/id", userId);
    settings.setValue("user/username", username);
    settings.setValue("user/avatar", avatar);
    settings.setValue("user/createTime", createTime);
    
    if (!password.isEmpty()) {
        // 加密并保存密码
        QString encryptedPassword = encryptPassword(password);
        settings.setValue("user/password", encryptedPassword);
        settings.setValue("user/remember", true);
        // 密码已加密保存
    } else {
        // 清除保存的密码
        settings.remove("user/password");
        settings.setValue("user/remember", false);
        // 密码已清除
    }
    
    // 同步设置，确保立即写入
    settings.sync();
    
    // 验证保存的值
    QString savedToken = settings.value("user/token", "").toString();
    QString savedEmail = settings.value("user/email", "").toString();
    
    // 用户信息已保存
}

void LoginPage::loadUserInfo(QString &token, QString &email, QString &password)
{
    QSettings settings("YourCompany", "QtApp");
    token = settings.value("user/token", "").toString();
    email = settings.value("user/email", "").toString();
    
    QString encryptedPassword = settings.value("user/password", "").toString();
    if (!encryptedPassword.isEmpty()) {
        password = decryptPassword(encryptedPassword);
        // 密码已加载并解密
    } else {
        password.clear();
        // 未找到密码
    }
    
    // 用户信息已加载
}

bool LoginPage::checkAutoLogin()
{
    QString token, email, password;
    loadUserInfo(token, email, password);
    
    // 检查自动登录
    
    if (!email.isEmpty()) {
        // 填充登录表单
        m_loginEmail->setText(email);
        
        if (!password.isEmpty()) {
            // 如果保存了密码，自动填充并触发登录
            m_loginPassword->setText(password);
            m_rememberPassword->setChecked(true);
            showSuccess("正在自动登录...");
            // 触发自动登录
            // 延迟500ms后自动触发登录
            QTimer::singleShot(500, this, &LoginPage::onLoginClicked);
        } else {
            // 没有保存密码，只填充邮箱
            m_rememberPassword->setChecked(false);
            showSuccess("检测到已登录用户，请输入密码继续");
            // 邮箱已填充，但未保存密码
        }
        return true;
    }
    // 未找到保存的登录信息
    return false;
}

void LoginPage::clearUserInfo()
{
    QSettings settings("YourCompany", "QtApp");
    settings.remove("user/token");
    settings.remove("user/email");
    settings.remove("user/password");
    settings.remove("user/remember");
    // 用户信息已清除
}

QString LoginPage::encryptPassword(const QString &password)
{
    // 使用简单的XOR加密，配合固定密钥
    QString key = "QtAppSecretKey2024";
    QByteArray data = password.toUtf8();
    QByteArray keyData = key.toUtf8();
    
    for (int i = 0; i < data.size(); ++i) {
        data[i] = data[i] ^ keyData[i % keyData.size()];
    }
    
    // 转换为Base64字符串存储
    return QString::fromLatin1(data.toBase64());
}

QString LoginPage::decryptPassword(const QString &encrypted)
{
    QString key = "QtAppSecretKey2024";
    QByteArray data = QByteArray::fromBase64(encrypted.toLatin1());
    QByteArray keyData = key.toUtf8();
    
    for (int i = 0; i < data.size(); ++i) {
        data[i] = data[i] ^ keyData[i % keyData.size()];
    }
    
    return QString::fromUtf8(data);
}
void LoginPage::applyTheme()
{
    // 应用主题到所有页面
    for (int i = 0; i < m_pageStack->count(); ++i) {
        QWidget* widget = m_pageStack->widget(i);
        if (widget) {
            applyThemeToWidget(widget);
        }
    }
}

void LoginPage::applyThemeToWidget(QWidget* widget)
{
    if (!widget) return;
    
    ThemeManager* themeManager = ThemeManager::instance();
    const auto& colors = themeManager->colors();
    
    // 应用主题到页面
    QString pageStyle = QString(
        "QWidget#loginWidget, QWidget#registerWidget { "
        "    background-color: %1; "
        "} "
        "QWidget#topBar { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 %2, stop:1 %3); "
        "} "
        "QLabel#logoLabel { "
        "    color: white; "
        "    background-color: transparent; "
        "    font-family: %4; "
        "    font-size: %5px; "
        "    font-weight: 700; "
        "    border: none; "
        "    padding: 0px; "
        "    margin: 0px; "
        "} "
        "QWidget#loginCard, QWidget#registerCard { "
        "    background-color: %6; "
        "    border: 1px solid %7; "
        "    border-radius: %8px; "
        "} "
        "QLabel#loginTitle, QLabel#registerTitle { "
        "    color: %9; "
        "    font-family: %10; "
        "    font-size: %11px; "
        "    font-weight: 700; "
        "    margin-bottom: %12px; "
        "}"
    ).arg(colors.BACKGROUND)
     .arg(colors.PRIMARY)
     .arg(colors.PRIMARY_HOVER)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(ThemeManager::Typography::FONT_SIZE_XL)
     .arg(colors.CARD)
     .arg(colors.BORDER)
     .arg(ThemeManager::BorderRadius::LG)
     .arg(colors.TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(ThemeManager::Typography::FONT_SIZE_XXL)
     .arg(ThemeManager::Spacing::MD);
    
    // 应用输入框样式
    QString inputStyle = themeManager->getInputStyle();
    QString buttonPrimaryStyle = themeManager->getButtonStyle("primary");
    QString buttonSecondaryStyle = themeManager->getButtonStyle("ghost");
    
    // 消息标签样式
    QString messageStyle = QString(
        "QLabel#loginMessage, QLabel#registerMessage { "
        "    font-family: %1; "
        "    font-size: %2px; "
        "    padding: %3px; "
        "    border-radius: %4px; "
        "    background-color: transparent; "
        "}"
    ).arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(ThemeManager::Typography::FONT_SIZE_SM)
     .arg(ThemeManager::Spacing::SM)
     .arg(ThemeManager::BorderRadius::SM);
    
    // 复选框样式
    QString checkboxStyle = QString(
        "QCheckBox#rememberPassword { "
        "    color: %1; "
        "    font-family: %2; "
        "    font-size: %3px; "
        "} "
        "QCheckBox#rememberPassword::indicator { "
        "    width: 16px; "
        "    height: 16px; "
        "    border: 1px solid %4; "
        "    border-radius: %5px; "
        "    background-color: %6; "
        "} "
        "QCheckBox#rememberPassword::indicator:checked { "
        "    background-color: %7; "
        "    border-color: %8; "
        "}"
    ).arg(colors.TEXT_SECONDARY)
     .arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(ThemeManager::Typography::FONT_SIZE_SM)
     .arg(colors.BORDER)
     .arg(ThemeManager::BorderRadius::SM)
     .arg(colors.BACKGROUND)
     .arg(colors.PRIMARY)
     .arg(colors.PRIMARY);
    
    // 应用所有样式到widget
    widget->setStyleSheet(pageStyle + inputStyle + buttonPrimaryStyle + buttonSecondaryStyle + messageStyle + checkboxStyle);
    
    // 为按钮设置特定的样式
    QList<QPushButton*> primaryButtons = widget->findChildren<QPushButton*>();
    for (QPushButton* button : primaryButtons) {
        if (button->objectName() == "loginButton" || button->objectName() == "registerButton") {
            button->setStyleSheet(buttonPrimaryStyle);
        } else if (button->objectName() == "switchButton") {
            button->setStyleSheet(buttonSecondaryStyle);
        }
    }
}

void LoginPage::onThemeChanged()
{
    applyTheme();
}