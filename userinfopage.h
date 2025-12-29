#ifndef USERINFOPAGE_H
#define USERINFOPAGE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QNetworkReply>
#include "networkmanager.h"

class UserInfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit UserInfoPage(QWidget *parent = nullptr);
    ~UserInfoPage();
    void loadUserInfo();

private:
    void setupUI();
    QWidget* createInfoItem(const QString &label, const QString &value);
    void showError(const QString &message);
    
    // UI控件
    QLabel *m_avatarLabel;
    QLabel *m_usernameLabel;
    QLabel *m_emailLabel;
    QLabel *m_idLabel;
    QLabel *m_createTimeLabel;
    
    // 网络请求
    NetworkManager *m_networkManager;
};

#endif // USERINFOPAGE_H