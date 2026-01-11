#ifndef MAINUIWINDOW_H
#define MAINUIWINDOW_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QStatusBar>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QGraphicsDropShadowEffect>

#include "userprofiletab/userprofiletab.h"
#include "reportstab/reportstab.h"
#include "loginpage.h"
#include "changepasswordpage.h"
#include "styles/theme_manager.h"

class MainUIWindow : public QWidget
{
    Q_OBJECT

public:
    MainUIWindow(QWidget *parent = nullptr);
    void forceAvatarSync(); // 公共方法，用于强制头像同步

private slots:
    void onMainMenuClicked(QListWidgetItem *item);
    void onSubMenuClicked(QListWidgetItem *item);
    void onLoginSuccess(const QString &token);
    void onLogoutClicked();
    void updateUserInfo();
    void updateUserInfoSafe();
    void applyTheme();

private:
    void setupUI(QWidget *parent = nullptr);
    void setupNavigationBar();
    void setupMainMenu();
    void setupSubMenu();
    void setupSubMenu(const QString &mainMenu);
    void setupSubMenuContent(const QString &mainMenu);
    void setupContent();
    void setupStatusBar();
    QPixmap createCircularPixmap(const QPixmap &pixmap, int size);
    void setDefaultAvatar();
    void loadNetworkAvatar(const QString &avatarUrl);

    QGridLayout *mainLayout;
    
    // 导航栏组件
    QWidget *navigationBar;
    QLabel *avatarLabel;
    QLabel *usernameLabel;
    QPushButton *logoutButton;
    QComboBox *themeComboBox;
    
    // 菜单组件
    QListWidget *mainMenuList;
    QListWidget *subMenuList;
    
    // 内容区域
    QStackedWidget *contentStack;
    
    // 状态栏组件
    QWidget *statusBar;
    QLabel *statusIndicator;
    QLabel *statusText;
    QLabel *statusMessage;
    
    // 其他组件
    LoginPage *loginPage;
    QStackedWidget *mainStack;
};

#endif // MAINUIWINDOW_H
