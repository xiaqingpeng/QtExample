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
#include "../Charts/echartstab.h"
#include "../Charts/logstatstab.h"
#include "../UserProfile/userprofiletab.h"
#include "../Reports/reportstab.h"
#include "../Auth/loginpage.h"
#include "../Auth/changepasswordpage.h"
#include "../UserProfile/userinfopage.h"
#include "../Device/serverconfigtab.h"
#include "../Device/contenttab.h"
#include "../Styles/theme_manager.h"
#include "../Localization/LocalizationManager.h"

// 企业级架构集成
#include "../Core/Application.h"
#include "../Interfaces/INetworkService.h"
#include "../Interfaces/IAuthenticationService.h"

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
    void onAuthenticationChanged(bool authenticated); // 企业级认证状态变化

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
    void initializeEnterpriseServices(); // 初始化企业级服务
    void retranslateUi();                // 根据当前语言刷新界面文案

    QGridLayout *mainLayout;
    
    // 导航栏组件
    QWidget *navigationBar;
    QLabel *appTitle;
    QLabel *avatarLabel;
    QLabel *usernameLabel;
    QPushButton *logoutButton;
    QComboBox *themeComboBox;
    QComboBox *languageComboBox;
    
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
    
    // 企业级服务
    INetworkService *m_networkService;
    IAuthenticationService *m_authService;
};

#endif // MAINUIWINDOW_H
