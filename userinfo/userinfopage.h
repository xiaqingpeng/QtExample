#ifndef USERINFOPAGE_H
#define USERINFOPAGE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QNetworkReply>
#include "networkmanager.h"
#include "analytics.h"

class UserInfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit UserInfoPage(QWidget *parent = nullptr);
    ~UserInfoPage();
    void loadUserInfo();

signals:
    void avatarUpdated(); // 头像更新成功信号

private:
    void setupUI();
    QWidget* createInfoItem(const QString &label, const QString &value);
    void showError(const QString &message);
    void onUploadAvatarClicked();
    void uploadAvatar(const QString &filePath);
    void onAvatarUploadFinished(QNetworkReply *reply);
    QPixmap createCircularPixmap(const QPixmap &pixmap, int size);
    void onThemeChanged(); // 主题变化槽函数
    
    // UI控件
    QLabel *m_avatarLabel;
    QLabel *m_usernameLabel;
    QLabel *m_emailLabel;
    QLabel *m_idLabel;
    QLabel *m_createTimeLabel;
    QLabel *m_statusLabel;
    QPushButton *m_uploadAvatarButton;
    
    // 网络请求
    NetworkManager *m_networkManager;
};

#endif // USERINFOPAGE_H