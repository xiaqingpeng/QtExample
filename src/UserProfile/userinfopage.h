#ifndef USERINFOPAGE_H
#define USERINFOPAGE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QNetworkReply>
#include <QEvent>
#include <QMouseEvent>
#include <QStackedWidget>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include "networkmanager.h"
#include "../Analytics/analytics.h"

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
    void setupViewMode();  // 设置查看模式UI
    void setupEditMode();  // 设置编辑模式UI
    QWidget* createInfoItem(const QString &label, const QString &value);
    void showError(const QString &message);
    void onEditProfileClicked();
    void onSaveProfileClicked();  // 保存用户信息
    void onCancelEditClicked();   // 取消编辑
    void onUploadAvatarClicked();
    void uploadAvatar(const QString &filePath);
    void onAvatarUploadFinished(QNetworkReply *reply);
    QPixmap createCircularPixmap(const QPixmap &pixmap, int size);
    void setAvatarPixmap(const QPixmap &pixmap); // 辅助方法：同时更新标签和按钮
    void onThemeChanged(); // 主题变化槽函数
    bool eventFilter(QObject *obj, QEvent *event) override; // 事件过滤器
    QPixmap roundAvatar(const QPixmap &pixmap, int size);  // 创建圆形头像

public slots:
    void applyTheme(); // 应用主题方法

private:
    // 主要布局控件
    QStackedWidget *m_stackedWidget;  // 用于切换查看和编辑模式
    QWidget *m_viewWidget;            // 查看模式页面
    QWidget *m_editWidget;            // 编辑模式页面
    
    // 查看模式UI控件
    QLabel *m_avatarLabel;  // 保留用于兼容性
    QPushButton *m_avatarButton;  // 新的可点击头像按钮
    QLabel *m_usernameLabel;
    QLabel *m_emailLabel;
    QLabel *m_idLabel;
    QLabel *m_createTimeLabel;
    QLabel *m_statusLabel;
    QPushButton *m_editProfileButton;
    
    // 编辑模式UI控件
    QLabel *m_editAvatarLabel;
    QLineEdit *m_nicknameEdit;
    QLineEdit *m_douBaoIdEdit;
    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;
    
    // 网络请求
    NetworkManager *m_networkManager;
};

#endif // USERINFOPAGE_H