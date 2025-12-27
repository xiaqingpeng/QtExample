#include "loginpage.h"
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
    
    // 检查是否有保存的登录信息，如果有则自动登录
    checkAutoLogin();
}

LoginPage::~LoginPage()
{
}

void LoginPage::setupLoginUI()
{
    // 主容器
    QWidget *loginWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(loginWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 设置QQ风格背景
    loginWidget->setStyleSheet(
        "QWidget {"
        "    background-color: #f5f5f5;"
        "}"
    );
    
    // 顶部装饰条
    QWidget *topBar = new QWidget();
    topBar->setFixedHeight(60);
    topBar->setStyleSheet(
        "QWidget {"
        "    background-color: #0099FF;"
        "}"
    );
    
    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(20, 0, 20, 0);
    
    // Logo标签
    QLabel *logoLabel = new QLabel("QT系统");
    logoLabel->setStyleSheet(
        "QLabel {"
        "    color: white;"
        "    font-size: 28px;"
        "    font-weight: bold;"
        "}"
    );
    topBarLayout->addStretch();
    topBarLayout->addWidget(logoLabel);
    topBarLayout->addStretch();
    
    mainLayout->addWidget(topBar);
    
    // 登录卡片
    QWidget *cardWidget = new QWidget();
    cardWidget->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border-radius: 8px;"
        "}"
    );
    cardWidget->setMaximumWidth(380);
    
    QVBoxLayout *cardLayout = new QVBoxLayout(cardWidget);
    cardLayout->setSpacing(20);
    cardLayout->setContentsMargins(30, 30, 30, 30);

    // 标题
    QLabel *titleLabel = new QLabel("欢迎登录");
    titleLabel->setAlignment(Qt::AlignLeft);
    QFont titleFont("Helvetica Neue", 24, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet(
        "color: #333333;"
        "margin-bottom: 5px;"
    );
    cardLayout->addWidget(titleLabel);

    // 表单
    QVBoxLayout *formLayout = new QVBoxLayout();
    formLayout->setSpacing(15);

    // 邮箱输入
    m_loginEmail = new QLineEdit();
    m_loginEmail->setPlaceholderText("邮箱/手机号");
    m_loginEmail->setStyleSheet(
        "QLineEdit {"
        "    padding: 10px;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 4px;"
        "    font-size: 14px;"
        "    background-color: #ffffff;"
        "    color: #333333;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #0099FF;"
        "}"
        "QLineEdit:hover {"
        "    border: 1px solid #0099FF;"
        "}"
    );
    formLayout->addWidget(m_loginEmail);

    // 密码输入
    m_loginPassword = new QLineEdit();
    m_loginPassword->setPlaceholderText("密码");
    m_loginPassword->setEchoMode(QLineEdit::Password);
    m_loginPassword->setStyleSheet(
        "QLineEdit {"
        "    padding: 10px;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 4px;"
        "    font-size: 14px;"
        "    background-color: #ffffff;"
        "    color: #333333;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #0099FF;"
        "}"
        "QLineEdit:hover {"
        "    border: 1px solid #0099FF;"
        "}"
    );
    formLayout->addWidget(m_loginPassword);

    // 记住密码复选框
    m_rememberPassword = new QCheckBox("记住密码");
    m_rememberPassword->setStyleSheet(
        "QCheckBox {"
        "    font-size: 13px;"
        "    color: #666666;"
        "}"
        "QCheckBox::indicator {"
        "    width: 16px;"
        "    height: 16px;"
        "}"
    );
    formLayout->addWidget(m_rememberPassword);

    cardLayout->addLayout(formLayout);

    // 登录按钮
    m_loginButton = new QPushButton("登录");
    m_loginButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #0099FF;"
        "    color: white;"
        "    border: none;"
        "    padding: 12px;"
        "    border-radius: 4px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0088EE;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0077DD;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #cccccc;"
        "}"
    );
    connect(m_loginButton, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    cardLayout->addWidget(m_loginButton);

    // 切换到注册按钮
    m_switchToRegisterButton = new QPushButton("注册账号");
    m_switchToRegisterButton->setStyleSheet(
        "QPushButton {"
        "    background-color: transparent;"
        "    color: #0099FF;"
        "    border: none;"
        "    padding: 8px;"
        "    font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "    color: #0088EE;"
        "    text-decoration: underline;"
        "}"
    );
    connect(m_switchToRegisterButton, &QPushButton::clicked, this, &LoginPage::onSwitchToRegister);
    cardLayout->addWidget(m_switchToRegisterButton);

    // 消息提示
    m_loginMessage = new QLabel();
    m_loginMessage->setAlignment(Qt::AlignCenter);
    m_loginMessage->setWordWrap(true);
    m_loginMessage->setStyleSheet(
        "QLabel {"
        "    padding: 8px;"
        "    border-radius: 4px;"
        "    font-size: 13px;"
        "}"
    );
    cardLayout->addWidget(m_loginMessage);

    cardLayout->addStretch();

    // 居中显示卡片
    QHBoxLayout *centerLayout = new QHBoxLayout();
    centerLayout->addStretch();
    centerLayout->addWidget(cardWidget);
    centerLayout->addStretch();
    
    mainLayout->addStretch();
    mainLayout->addLayout(centerLayout);
    mainLayout->addStretch();

    m_pageStack->addWidget(loginWidget);
}

void LoginPage::setupRegisterUI()
{
    // 主容器
    QWidget *registerWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(registerWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    // 设置QQ风格背景
    registerWidget->setStyleSheet(
        "QWidget {"
        "    background-color: #f5f5f5;"
        "}"
    );
    
    // 顶部装饰条
    QWidget *topBar = new QWidget();
    topBar->setFixedHeight(60);
    topBar->setStyleSheet(
        "QWidget {"
        "    background-color: #0099FF;"
        "}"
    );
    
    QHBoxLayout *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(20, 0, 20, 0);
    
    // Logo标签
    QLabel *logoLabel = new QLabel("QQ");
    logoLabel->setStyleSheet(
        "QLabel {"
        "    color: white;"
        "    font-size: 28px;"
        "    font-weight: bold;"
        "}"
    );
    topBarLayout->addStretch();
    topBarLayout->addWidget(logoLabel);
    topBarLayout->addStretch();
    
    mainLayout->addWidget(topBar);
    
    // 注册卡片
    QWidget *cardWidget = new QWidget();
    cardWidget->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border-radius: 8px;"
        "}"
    );
    cardWidget->setMaximumWidth(380);
    
    QVBoxLayout *cardLayout = new QVBoxLayout(cardWidget);
    cardLayout->setSpacing(15);
    cardLayout->setContentsMargins(30, 30, 30, 30);

    // 标题
    QLabel *titleLabel = new QLabel("注册账号");
    titleLabel->setAlignment(Qt::AlignLeft);
    QFont titleFont("Microsoft YaHei", 24, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet(
        "color: #333333;"
        "margin-bottom: 5px;"
    );
    cardLayout->addWidget(titleLabel);

    // 表单
    QVBoxLayout *formLayout = new QVBoxLayout();
    formLayout->setSpacing(12);

    // 用户名输入
    m_registerUsername = new QLineEdit();
    m_registerUsername->setPlaceholderText("昵称");
    m_registerUsername->setStyleSheet(
        "QLineEdit {"
        "    padding: 10px;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 4px;"
        "    font-size: 14px;"
        "    background-color: #ffffff;"
        "    color: #333333;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #0099FF;"
        "}"
        "QLineEdit:hover {"
        "    border: 1px solid #0099FF;"
        "}"
    );
    formLayout->addWidget(m_registerUsername);

    // 邮箱输入
    m_registerEmail = new QLineEdit();
    m_registerEmail->setPlaceholderText("邮箱/手机号");
    m_registerEmail->setStyleSheet(
        "QLineEdit {"
        "    padding: 10px;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 4px;"
        "    font-size: 14px;"
        "    background-color: #ffffff;"
        "    color: #333333;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #0099FF;"
        "}"
        "QLineEdit:hover {"
        "    border: 1px solid #0099FF;"
        "}"
    );
    formLayout->addWidget(m_registerEmail);

    // 密码输入
    m_registerPassword = new QLineEdit();
    m_registerPassword->setPlaceholderText("密码（6-16位）");
    m_registerPassword->setEchoMode(QLineEdit::Password);
    m_registerPassword->setStyleSheet(
        "QLineEdit {"
        "    padding: 10px;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 4px;"
        "    font-size: 14px;"
        "    background-color: #ffffff;"
        "    color: #333333;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #0099FF;"
        "}"
        "QLineEdit:hover {"
        "    border: 1px solid #0099FF;"
        "}"
    );
    formLayout->addWidget(m_registerPassword);

    // 确认密码输入
    m_registerConfirmPassword = new QLineEdit();
    m_registerConfirmPassword->setPlaceholderText("确认密码");
    m_registerConfirmPassword->setEchoMode(QLineEdit::Password);
    m_registerConfirmPassword->setStyleSheet(
        "QLineEdit {"
        "    padding: 10px;"
        "    border: 1px solid #e0e0e0;"
        "    border-radius: 4px;"
        "    font-size: 14px;"
        "    background-color: #ffffff;"
        "    color: #333333;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #0099FF;"
        "}"
        "QLineEdit:hover {"
        "    border: 1px solid #0099FF;"
        "}"
    );
    formLayout->addWidget(m_registerConfirmPassword);

    cardLayout->addLayout(formLayout);

    // 注册按钮
    m_registerButton = new QPushButton("注册");
    m_registerButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #0099FF;"
        "    color: white;"
        "    border: none;"
        "    padding: 12px;"
        "    border-radius: 4px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #0088EE;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #0077DD;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #cccccc;"
        "}"
    );
    connect(m_registerButton, &QPushButton::clicked, this, &LoginPage::onRegisterClicked);
    cardLayout->addWidget(m_registerButton);

    // 切换到登录按钮
    m_switchToLoginButton = new QPushButton("已有账号？登录");
    m_switchToLoginButton->setStyleSheet(
        "QPushButton {"
        "    background-color: transparent;"
        "    color: #0099FF;"
        "    border: none;"
        "    padding: 8px;"
        "    font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "    color: #0088EE;"
        "    text-decoration: underline;"
        "}"
    );
    connect(m_switchToLoginButton, &QPushButton::clicked, this, &LoginPage::onSwitchToLogin);
    cardLayout->addWidget(m_switchToLoginButton);

    // 消息提示
    m_registerMessage = new QLabel();
    m_registerMessage->setAlignment(Qt::AlignCenter);
    m_registerMessage->setWordWrap(true);
    m_registerMessage->setStyleSheet(
        "QLabel {"
        "    padding: 8px;"
        "    border-radius: 4px;"
        "    font-size: 13px;"
        "}"
    );
    cardLayout->addWidget(m_registerMessage);

    cardLayout->addStretch();

    // 居中显示卡片
    QHBoxLayout *centerLayout = new QHBoxLayout();
    centerLayout->addStretch();
    centerLayout->addWidget(cardWidget);
    centerLayout->addStretch();
    
    mainLayout->addStretch();
    mainLayout->addLayout(centerLayout);
    mainLayout->addStretch();

    m_pageStack->addWidget(registerWidget);
}

void LoginPage::onLoginClicked()
{
    QString email = m_loginEmail->text().trimmed();
    QString password = m_loginPassword->text();

    qDebug() << "Login Input - Email:" << email;
    qDebug() << "Login Input - Password length:" << password.length();
    qDebug() << "Login Input - Email trimmed:" << email.trimmed();

    if (email.isEmpty() || password.isEmpty()) {
        showError("邮箱和密码不能为空");
        return;
    }

    m_loginButton->setEnabled(false);
    m_loginButton->setText("登录中...");

    QJsonObject json;
    json["email"] = email;
    json["password"] = password;

    // 使用NetworkManager发送登录请求，自动添加平台识别头
    m_networkManager->post("http://120.48.95.51:7001/login",
                           json,
                           [this](const QJsonObject &response) {
        // 成功回调
        int code = response["code"].toInt();
        QString msg = response["msg"].toString();

        qDebug() << "=== Login Response ===";
        qDebug() << "Full response:" << QJsonDocument(response).toJson(QJsonDocument::Indented);
        qDebug() << "Code:" << code;
        qDebug() << "Message:" << msg;
        qDebug() << "Token field exists:" << response.contains("token");
        if (response.contains("token")) {
            qDebug() << "Token value:" << response["token"].toString();
        }
        qDebug() << "All keys in response:" << response.keys();

        if (code == 0) {
            QString cookie;
            if (response.contains("token")) {
                cookie = response["token"].toString();
            }

            qDebug() << "Cookie received:" << cookie;

            // 保存用户信息用于自动登录
            QString email = m_loginEmail->text().trimmed();
            QString password = m_rememberPassword->isChecked() ? m_loginPassword->text() : "";
            saveUserInfo(cookie, email, password);
            showSuccess("登录成功！");
            emit loginSuccess(cookie);
        } else {
            showError(msg);
        }

        m_loginButton->setEnabled(true);
        m_loginButton->setText("登录");
    },
    [this](const QString &errorMsg) {
        // 错误回调
        qDebug() << "Login Error:" << errorMsg;
        showError("网络错误：" + errorMsg);

        m_loginButton->setEnabled(true);
        m_loginButton->setText("登录");
    });
}

void LoginPage::onRegisterClicked()
{
    QString username = m_registerUsername->text().trimmed();
    QString email = m_registerEmail->text().trimmed();
    QString password = m_registerPassword->text();
    QString confirmPassword = m_registerConfirmPassword->text();

    if (username.isEmpty() || email.isEmpty() || password.isEmpty()) {
        showError("用户名、邮箱和密码不能为空");
        return;
    }

    if (password != confirmPassword) {
        showError("两次输入的密码不一致");
        return;
    }

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
                           [this](const QJsonObject &response) {
        // 成功回调
        int code = response["code"].toInt();
        QString msg = response["msg"].toString();

        if (code == 0) {
            showSuccess("注册成功！请登录");
            // 延迟1秒后切换到登录页面
            QTimer::singleShot(1000, this, &LoginPage::onSwitchToLogin);
        } else {
            showError(msg);
        }

        m_registerButton->setEnabled(true);
        m_registerButton->setText("注册");
    },
    [this](const QString &errorMsg) {
        // 错误回调
        qDebug() << "Register Error:" << errorMsg;
        showError("网络错误：" + errorMsg);

        m_registerButton->setEnabled(true);
        m_registerButton->setText("注册");
    });
}

void LoginPage::onSwitchToLogin()
{
    m_pageStack->setCurrentIndex(0);
    m_registerMessage->clear();
}

void LoginPage::onSwitchToRegister()
{
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

void LoginPage::saveUserInfo(const QString &token, const QString &email, const QString &password)
{
    QSettings settings("YourCompany", "QtApp");
    
    qDebug() << "=== Saving User Info ===";
    qDebug() << "Token to save:" << token;
    qDebug() << "Email to save:" << email;
    qDebug() << "Password to save:" << (password.isEmpty() ? "empty" : "exists");
    
    settings.setValue("user/token", token);
    settings.setValue("user/email", email);
    
    if (!password.isEmpty()) {
        // 加密并保存密码
        QString encryptedPassword = encryptPassword(password);
        settings.setValue("user/password", encryptedPassword);
        settings.setValue("user/remember", true);
        qDebug() << "Password encrypted and saved for:" << email;
    } else {
        // 清除保存的密码
        settings.remove("user/password");
        settings.setValue("user/remember", false);
        qDebug() << "Password cleared for:" << email;
    }
    
    // 同步设置，确保立即写入
    settings.sync();
    
    // 验证保存的值
    QString savedToken = settings.value("user/token", "").toString();
    QString savedEmail = settings.value("user/email", "").toString();
    
    qDebug() << "=== User Info Saved ===";
    qDebug() << "Email:" << email;
    qDebug() << "Token saved:" << (savedToken.isEmpty() ? "empty" : savedToken);
    qDebug() << "Password saved:" << !password.isEmpty();
    qDebug() << "Settings file:" << settings.fileName();
}

void LoginPage::loadUserInfo(QString &token, QString &email, QString &password)
{
    QSettings settings("YourCompany", "QtApp");
    token = settings.value("user/token", "").toString();
    email = settings.value("user/email", "").toString();
    
    QString encryptedPassword = settings.value("user/password", "").toString();
    if (!encryptedPassword.isEmpty()) {
        password = decryptPassword(encryptedPassword);
        qDebug() << "Password loaded and decrypted for:" << email;
    } else {
        password.clear();
        qDebug() << "No password found for:" << email;
    }
    
    qDebug() << "=== User Info Loaded ===";
    qDebug() << "Settings file:" << settings.fileName();
    qDebug() << "Email:" << email;
    qDebug() << "Token:" << (token.isEmpty() ? "empty" : "exists");
    qDebug() << "Password:" << (password.isEmpty() ? "empty" : "exists");
}

bool LoginPage::checkAutoLogin()
{
    QString token, email, password;
    loadUserInfo(token, email, password);
    
    qDebug() << "=== Auto Login Check ===";
    qDebug() << "Token:" << (token.isEmpty() ? "empty" : "exists");
    qDebug() << "Email:" << (email.isEmpty() ? "empty" : email);
    qDebug() << "Password:" << (password.isEmpty() ? "empty" : "exists");
    
    if (!email.isEmpty()) {
        // 填充登录表单
        m_loginEmail->setText(email);
        
        if (!password.isEmpty()) {
            // 如果保存了密码，自动填充并触发登录
            m_loginPassword->setText(password);
            m_rememberPassword->setChecked(true);
            showSuccess("正在自动登录...");
            qDebug() << "Auto login triggered for:" << email;
            // 延迟500ms后自动触发登录
            QTimer::singleShot(500, this, &LoginPage::onLoginClicked);
        } else {
            // 没有保存密码，只填充邮箱
            m_rememberPassword->setChecked(false);
            showSuccess("检测到已登录用户，请输入密码继续");
            qDebug() << "Email filled, but no password saved for:" << email;
        }
        return true;
    }
    qDebug() << "No saved login info found";
    return false;
}

void LoginPage::clearUserInfo()
{
    QSettings settings("YourCompany", "QtApp");
    settings.remove("user/token");
    settings.remove("user/email");
    settings.remove("user/password");
    settings.remove("user/remember");
    qDebug() << "User info cleared";
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