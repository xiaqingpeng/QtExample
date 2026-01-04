#ifndef AUTHENTICATIONSERVICE_H
#define AUTHENTICATIONSERVICE_H

#include "../Interfaces/IAuthenticationService.h"
#include "../Interfaces/INetworkService.h"
#include <QObject>
#include <QSettings>

/**
 * @brief 认证服务实现
 * 
 * 实现IAuthenticationService接口，提供用户认证功能
 */
class AuthenticationService : public QObject, public IAuthenticationService
{
    Q_OBJECT
    Q_INTERFACES(IAuthenticationService)

public:
    explicit AuthenticationService(INetworkService* networkService, QObject *parent = nullptr);
    ~AuthenticationService();

    // IAuthenticationService接口实现
    QFuture<AuthResult> login(const QString& email, const QString& password) override;
    QFuture<AuthResult> registerUser(const UserInfo& userInfo, const QString& password) override;
    void logout() override;
    bool isAuthenticated() const override;
    UserInfo getCurrentUser() const override;
    QString getToken() const override;
    QFuture<AuthResult> refreshToken() override;
    QFuture<AuthResult> changePassword(const QString& oldPassword, const QString& newPassword) override;
    QFuture<AuthResult> resetPassword(const QString& email) override;

signals:
    void authenticationChanged(bool authenticated);
    void userInfoChanged(const UserInfo& userInfo);

private:
    void saveUserInfo(const QString& token, const UserInfo& userInfo);
    void loadUserInfo();
    void clearUserInfo();
    QString encryptPassword(const QString& password);
    QString decryptPassword(const QString& encrypted);

private:
    INetworkService* m_networkService;
    QString m_token;
    UserInfo m_currentUser;
    bool m_authenticated;
    QSettings* m_settings;
};

#endif // AUTHENTICATIONSERVICE_H