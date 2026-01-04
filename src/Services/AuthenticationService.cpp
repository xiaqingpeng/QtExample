#include "AuthenticationService.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QCryptographicHash>
#include <QDebug>
#include <QPromise>

AuthenticationService::AuthenticationService(INetworkService* networkService, QObject *parent)
    : QObject(parent)
    , m_networkService(networkService)
    , m_authenticated(false)
    , m_settings(new QSettings("YourCompany", "QtEnterpriseApp", this))
{
    loadUserInfo();
}

AuthenticationService::~AuthenticationService()
{
}

QFuture<AuthResult> AuthenticationService::login(const QString& email, const QString& password)
{
    QJsonObject loginData;
    loginData["email"] = email;
    loginData["password"] = password;
    
    // 简化实现，直接返回成功结果用于测试
    QPromise<AuthResult> promise;
    promise.start();
    
    AuthResult result;
    result.success = true;
    result.message = "登录成功（测试模式）";
    result.token = "test_token_123";
    
    // 设置测试用户信息
    m_token = result.token;
    m_currentUser.id = "1";
    m_currentUser.username = "测试用户";
    m_currentUser.email = email;
    m_authenticated = true;
    
    emit authenticationChanged(true);
    emit userInfoChanged(m_currentUser);
    
    promise.addResult(result);
    promise.finish();
    
    return promise.future();
}

QFuture<AuthResult> AuthenticationService::registerUser(const UserInfo& userInfo, const QString& password)
{
    QPromise<AuthResult> promise;
    promise.start();
    
    AuthResult result;
    result.success = true;
    result.message = "注册成功（测试模式）";
    
    promise.addResult(result);
    promise.finish();
    
    return promise.future();
}

void AuthenticationService::logout()
{
    m_token.clear();
    m_currentUser = UserInfo();
    m_authenticated = false;
    
    clearUserInfo();
    
    emit authenticationChanged(false);
    emit userInfoChanged(m_currentUser);
}

bool AuthenticationService::isAuthenticated() const
{
    return m_authenticated && !m_token.isEmpty();
}

UserInfo AuthenticationService::getCurrentUser() const
{
    return m_currentUser;
}

QString AuthenticationService::getToken() const
{
    return m_token;
}

QFuture<AuthResult> AuthenticationService::refreshToken()
{
    QPromise<AuthResult> promise;
    promise.start();
    
    AuthResult result;
    result.success = false;
    result.message = "令牌刷新功能尚未实现";
    
    promise.addResult(result);
    promise.finish();
    
    return promise.future();
}

QFuture<AuthResult> AuthenticationService::changePassword(const QString& oldPassword, const QString& newPassword)
{
    QPromise<AuthResult> promise;
    promise.start();
    
    AuthResult result;
    result.success = false;
    result.message = "密码修改功能尚未实现";
    
    promise.addResult(result);
    promise.finish();
    
    return promise.future();
}

QFuture<AuthResult> AuthenticationService::resetPassword(const QString& email)
{
    QPromise<AuthResult> promise;
    promise.start();
    
    AuthResult result;
    result.success = false;
    result.message = "密码重置功能尚未实现";
    
    promise.addResult(result);
    promise.finish();
    
    return promise.future();
}

void AuthenticationService::saveUserInfo(const QString& token, const UserInfo& userInfo)
{
    m_settings->setValue("user/token", token);
    m_settings->setValue("user/id", userInfo.id);
    m_settings->setValue("user/username", userInfo.username);
    m_settings->setValue("user/email", userInfo.email);
    m_settings->setValue("user/avatar", userInfo.avatar);
    m_settings->setValue("user/createTime", userInfo.createTime);
    
    QJsonDocument doc(userInfo.additionalData);
    m_settings->setValue("user/additionalData", doc.toJson());
    
    m_settings->sync();
}

void AuthenticationService::loadUserInfo()
{
    m_token = m_settings->value("user/token", "").toString();
    
    if (!m_token.isEmpty()) {
        m_currentUser.id = m_settings->value("user/id", "").toString();
        m_currentUser.username = m_settings->value("user/username", "").toString();
        m_currentUser.email = m_settings->value("user/email", "").toString();
        m_currentUser.avatar = m_settings->value("user/avatar", "").toString();
        m_currentUser.createTime = m_settings->value("user/createTime", "").toString();
        
        QByteArray additionalDataBytes = m_settings->value("user/additionalData", "").toByteArray();
        if (!additionalDataBytes.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(additionalDataBytes);
            m_currentUser.additionalData = doc.object();
        }
        
        m_authenticated = true;
    }
}

void AuthenticationService::clearUserInfo()
{
    m_settings->remove("user/token");
    m_settings->remove("user/id");
    m_settings->remove("user/username");
    m_settings->remove("user/email");
    m_settings->remove("user/avatar");
    m_settings->remove("user/createTime");
    m_settings->remove("user/additionalData");
    m_settings->sync();
}

QString AuthenticationService::encryptPassword(const QString& password)
{
    QString key = "QtEnterpriseAppSecretKey2024";
    QByteArray data = password.toUtf8();
    QByteArray keyData = key.toUtf8();
    
    for (int i = 0; i < data.size(); ++i) {
        data[i] = data[i] ^ keyData[i % keyData.size()];
    }
    
    return QString::fromLatin1(data.toBase64());
}

QString AuthenticationService::decryptPassword(const QString& encrypted)
{
    QString key = "QtEnterpriseAppSecretKey2024";
    QByteArray data = QByteArray::fromBase64(encrypted.toLatin1());
    QByteArray keyData = key.toUtf8();
    
    for (int i = 0; i < data.size(); ++i) {
        data[i] = data[i] ^ keyData[i % keyData.size()];
    }
    
    return QString::fromUtf8(data);
}