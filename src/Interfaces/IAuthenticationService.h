#ifndef IAUTHENTICATIONSERVICE_H
#define IAUTHENTICATIONSERVICE_H

#include <QObject>
#include <QJsonObject>
#include <QFuture>
#include <QString>

/**
 * @brief 认证结果
 */
struct AuthResult {
    bool success;
    QString message;
    QString token;
    QJsonObject userData;
    
    AuthResult(bool s = false, const QString& msg = QString(), const QString& t = QString())
        : success(s), message(msg), token(t) {}
};

/**
 * @brief 用户信息
 */
struct UserInfo {
    QString id;
    QString username;
    QString email;
    QString avatar;
    QString createTime;
    QJsonObject additionalData;
    
    UserInfo() = default;
    UserInfo(const QString& userId, const QString& name, const QString& mail)
        : id(userId), username(name), email(mail) {}
};

/**
 * @brief 认证服务接口
 * 
 * 定义用户认证相关的标准接口
 */
class IAuthenticationService
{
public:
    virtual ~IAuthenticationService() = default;

    /**
     * @brief 用户登录
     * @param email 邮箱
     * @param password 密码
     * @return 认证结果
     */
    virtual QFuture<AuthResult> login(const QString& email, const QString& password) = 0;

    /**
     * @brief 用户注册
     * @param userInfo 用户信息
     * @param password 密码
     * @return 认证结果
     */
    virtual QFuture<AuthResult> registerUser(const UserInfo& userInfo, const QString& password) = 0;

    /**
     * @brief 用户登出
     */
    virtual void logout() = 0;

    /**
     * @brief 检查是否已认证
     * @return 如果已认证返回true
     */
    virtual bool isAuthenticated() const = 0;

    /**
     * @brief 获取当前用户信息
     * @return 用户信息
     */
    virtual UserInfo getCurrentUser() const = 0;

    /**
     * @brief 获取认证令牌
     * @return 令牌
     */
    virtual QString getToken() const = 0;

    /**
     * @brief 刷新令牌
     * @return 新的认证结果
     */
    virtual QFuture<AuthResult> refreshToken() = 0;

    /**
     * @brief 修改密码
     * @param oldPassword 旧密码
     * @param newPassword 新密码
     * @return 操作结果
     */
    virtual QFuture<AuthResult> changePassword(const QString& oldPassword, const QString& newPassword) = 0;

    /**
     * @brief 重置密码
     * @param email 邮箱
     * @return 操作结果
     */
    virtual QFuture<AuthResult> resetPassword(const QString& email) = 0;
};

Q_DECLARE_INTERFACE(IAuthenticationService, "com.company.qtapp.IAuthenticationService/1.0")

// 注册元类型，用于信号槽
Q_DECLARE_METATYPE(AuthResult)
Q_DECLARE_METATYPE(UserInfo)

#endif // IAUTHENTICATIONSERVICE_H