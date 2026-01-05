#include "mainuiwindow.h"
#include "../Utils/logger.h"
#include "../Charts/echartstab.h"
#include "../Charts/logstatstab.h"
#include "../UserProfile/userprofiletab.h"
#include "../Reports/reportstab.h"
#include "../Auth/loginpage.h"
#include "../Auth/changepasswordpage.h"
#include "../UserProfile/userinfopage.h"
#include "../Styles/theme_manager.h"
#include "../Core/ServiceManager.h"
#include "../Services/AuthenticationService.h"
#include <QFont>
#include <QListWidgetItem>
#include <QScrollArea>
#include <QMetaMethod>
#include <QSettings>
#include <QHBoxLayout>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPixmap>
#include <QBitmap>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QPainterPath>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDir>

MainUIWindow::MainUIWindow(QWidget *parent) : QWidget(parent)
    , m_networkService(nullptr)
    , m_authService(nullptr)
{
    setWindowTitle("Qt UI控件综合示例");
    resize(1200, 800);

    // 初始化企业级服务
    initializeEnterpriseServices();

    // 创建主页面栈
    mainStack = new QStackedWidget(this);

    // 创建登录页面
    loginPage = new LoginPage(this);
    connect(loginPage, &LoginPage::loginSuccess, this, &MainUIWindow::onLoginSuccess);
    mainStack->addWidget(loginPage);

    // 创建主界面
    QWidget *mainWidget = new QWidget();
    mainLayout = new QGridLayout(mainWidget);
    setupUI(mainWidget);
    mainStack->addWidget(mainWidget);

    // 默认显示登录页面
    mainStack->setCurrentIndex(0);

    QVBoxLayout *mainVBoxLayout = new QVBoxLayout(this);
    mainVBoxLayout->addWidget(mainStack);
}

void MainUIWindow::setupUI(QWidget *parent)
{
    Q_UNUSED(parent);
    
    // LOG_DEBUG("setupUI started");
    
    try {
        // 创建主布局
        if (mainLayout) {
            delete mainLayout;
        }
        mainLayout = new QGridLayout();
        
        // 1. 创建现代化的导航栏
        setupNavigationBar();
        mainLayout->addWidget(navigationBar, 0, 0, 1, 3);
        
        // 2. 创建一级菜单
        setupMainMenu();
        mainLayout->addWidget(mainMenuList, 1, 0);
        
        // 3. 创建二级菜单
        setupSubMenu();
        mainLayout->addWidget(subMenuList, 1, 1);
        
        // 4. 创建内容区域
        setupContent();
        mainLayout->addWidget(contentStack, 1, 2);
        
        // 5. 创建状态栏
        setupStatusBar();
        mainLayout->addWidget(statusBar, 2, 0, 1, 3);
        
        // 设置布局比例
        mainLayout->setColumnStretch(0, 1);  // 一级菜单
        mainLayout->setColumnStretch(1, 1);  // 二级菜单
        mainLayout->setColumnStretch(2, 4);  // 内容区域
        mainLayout->setRowStretch(1, 1);     // 主要内容行
        
        // 设置布局到父窗口
        if (parent && parent->layout()) {
            delete parent->layout();
        }
        if (parent) {
            parent->setLayout(mainLayout);
        }
        
        // 应用主题
        applyTheme();
        
        // 连接主题变化信号
        connect(ThemeManager::instance(), &ThemeManager::themeChanged,
                this, &MainUIWindow::applyTheme);
        
        // 初始化菜单
        if (mainMenuList && mainMenuList->count() > 0) {
            mainMenuList->setCurrentRow(0);
            setupSubMenuContent(mainMenuList->currentItem()->text());
        }
        
        // LOG_DEBUG("setupUI completed successfully");
        
    } catch (const std::exception& e) {
        LOG_ERROR("Exception in setupUI:" << e.what());
    } catch (...) {
        LOG_ERROR("Unknown exception in setupUI");
    }
}

void MainUIWindow::setupNavigationBar()
{
    navigationBar = new QWidget();
    navigationBar->setObjectName("navigationBar");
    navigationBar->setFixedHeight(70);
    
    QHBoxLayout *navLayout = new QHBoxLayout(navigationBar);
    navLayout->setContentsMargins(24, 12, 24, 12);
    navLayout->setSpacing(20);
    
    // 应用标题
    appTitle = new QLabel("Qt 现代化应用");
    appTitle->setObjectName("appTitle");
    appTitle->setStyleSheet(QString(
        "QLabel { "
        "    font-family: %1; "
        "    font-size: %2px; "
        "    font-weight: 700; "
        "    color: %3; "
        "    padding: 0 8px; "
        "}"
    ).arg(ThemeManager::Typography::FONT_FAMILY)
     .arg(ThemeManager::Typography::FONT_SIZE_XL)
     .arg(ThemeManager::instance()->colors().TEXT_PRIMARY));
    
    navLayout->addWidget(appTitle);
    navLayout->addStretch();
    
    // 用户信息区域容器
    QWidget *userInfoContainer = new QWidget();
    QHBoxLayout *userInfoLayout = new QHBoxLayout(userInfoContainer);
    userInfoLayout->setContentsMargins(0, 0, 0, 0);
    userInfoLayout->setSpacing(12);
    
    // 主题切换器
    themeComboBox = new QComboBox();
    themeComboBox->addItem("浅色主题", static_cast<int>(ThemeManager::LIGHT));
    themeComboBox->addItem("深色主题", static_cast<int>(ThemeManager::DARK));
    themeComboBox->addItem("蓝色主题", static_cast<int>(ThemeManager::BLUE));
    themeComboBox->addItem("绿色主题", static_cast<int>(ThemeManager::GREEN));
    themeComboBox->setMinimumWidth(120);
    themeComboBox->setMaximumWidth(140);
    
    // 设置当前主题
    themeComboBox->setCurrentIndex(static_cast<int>(ThemeManager::instance()->getCurrentTheme()));
    
    connect(themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [](int index) {
                ThemeManager::instance()->setTheme(static_cast<ThemeManager::ThemeType>(index));
            });
    
    userInfoLayout->addWidget(themeComboBox);
    
    // 分隔线
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setFixedHeight(30);
    separator->setStyleSheet(QString(
        "QFrame { "
        "    color: %1; "
        "    background-color: %2; "
        "}"
    ).arg(ThemeManager::instance()->colors().BORDER)
     .arg(ThemeManager::instance()->colors().BORDER));
    
    userInfoLayout->addWidget(separator);
    
    // 用户头像和用户名容器
    QWidget *userContainer = new QWidget();
    QHBoxLayout *userLayout = new QHBoxLayout(userContainer);
    userLayout->setContentsMargins(0, 0, 0, 0);
    userLayout->setSpacing(10);
    
    // 用户头像
    avatarLabel = new QLabel();
    avatarLabel->setObjectName("avatarLabel");
    avatarLabel->setFixedSize(44, 44);
    avatarLabel->setAlignment(Qt::AlignCenter);
    
    userLayout->addWidget(avatarLabel);
    
    // 用户名
    usernameLabel = new QLabel("未登录");
    usernameLabel->setObjectName("usernameLabel");
    usernameLabel->setMinimumWidth(80);
    usernameLabel->setMaximumWidth(150);
    usernameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    userLayout->addWidget(usernameLabel);
    
    userInfoLayout->addWidget(userContainer);
    
    // 登出按钮
    logoutButton = new QPushButton("登出");
    logoutButton->setMinimumWidth(60);
    logoutButton->setMaximumWidth(80);
    connect(logoutButton, &QPushButton::clicked, this, &MainUIWindow::onLogoutClicked);
    
    userInfoLayout->addWidget(logoutButton);
    
    navLayout->addWidget(userInfoContainer);
}

void MainUIWindow::setupMainMenu()
{
    mainMenuList = new QListWidget();
    mainMenuList->setMaximumWidth(220);

    // 添加一级菜单项
    QListWidgetItem *item1 = new QListWidgetItem("图表示例");
    item1->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item1);

    QListWidgetItem *item2 = new QListWidgetItem("数据分析");
    item2->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item2);

    QListWidgetItem *item3 = new QListWidgetItem("设备信息");
    item3->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item3);

    QListWidgetItem *item4 = new QListWidgetItem("个人中心");
    item4->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item4);

    connect(mainMenuList, &QListWidget::itemClicked, this, &MainUIWindow::onMainMenuClicked);

    // 默认选择第一个一级菜单
    mainMenuList->setCurrentRow(0);
}

void MainUIWindow::setupSubMenu()
{
    subMenuList = new QListWidget();
    subMenuList->setMaximumWidth(220);
    connect(subMenuList, &QListWidget::itemClicked, this, &MainUIWindow::onSubMenuClicked);
}

void MainUIWindow::setupSubMenuContent(const QString &mainMenu)
{
    subMenuList->clear();

    if (mainMenu.contains("图表示例")) {
        new QListWidgetItem("ECharts示例", subMenuList);
        new QListWidgetItem("日志统计", subMenuList);
    } else if (mainMenu.contains("数据分析")) {
        new QListWidgetItem("用户画像", subMenuList);
        new QListWidgetItem("统计报表", subMenuList);
    } else if (mainMenu.contains("设备信息")) {
        new QListWidgetItem("服务器配置", subMenuList);
        new QListWidgetItem("内容", subMenuList);
    } else if (mainMenu.contains("个人中心")) {
        new QListWidgetItem("用户信息", subMenuList);
        new QListWidgetItem("修改密码", subMenuList);
    }

    // 默认选择第一个二级菜单
    if (subMenuList->count() > 0) {
        subMenuList->setCurrentRow(0);
        // 触发二级菜单点击事件，显示对应的内容
        onSubMenuClicked(subMenuList->currentItem());
    }
}

void MainUIWindow::setupStatusBar()
{
    statusBar = new QWidget();
    statusBar->setObjectName("statusBar");
    statusBar->setFixedHeight(32);
    
    QHBoxLayout *statusLayout = new QHBoxLayout(statusBar);
    statusLayout->setContentsMargins(20, 6, 20, 6);
    statusLayout->setSpacing(12);
    
    statusIndicator = new QLabel();
    statusIndicator->setObjectName("statusIndicator");
    statusIndicator->setFixedSize(8, 8);
    statusIndicator->setStyleSheet("background-color: #10b981; border-radius: 4px;");
    
    statusText = new QLabel("离线");
    statusText->setObjectName("statusText");
    
    statusMessage = new QLabel("就绪");
    statusMessage->setObjectName("statusMessage");
    
    statusLayout->addWidget(statusIndicator);
    statusLayout->addWidget(statusText);
    statusLayout->addStretch();
    statusLayout->addWidget(statusMessage);
}

void MainUIWindow::applyTheme()
{
    ThemeManager *theme = ThemeManager::instance();
    
    // 应用整体背景
    this->setStyleSheet(QString(
        "QWidget { "
        "    background-color: %1; "
        "    color: %2; "
        "    font-family: %3; "
        "}"
    ).arg(theme->colors().BACKGROUND)
     .arg(theme->colors().TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_FAMILY));
    
    // 应用导航栏样式
    if (navigationBar) {
        navigationBar->setStyleSheet(theme->getNavigationStyle());
    }
    
    // 更新应用标题样式
    if (appTitle) {
        appTitle->setStyleSheet(QString(
            "QLabel { "
            "    font-family: %1; "
            "    font-size: %2px; "
            "    font-weight: 700; "
            "    color: %3; "
            "    padding: 0 8px; "
            "}"
        ).arg(ThemeManager::Typography::FONT_FAMILY)
         .arg(ThemeManager::Typography::FONT_SIZE_XL)
         .arg(theme->colors().TEXT_PRIMARY));
    }
    
    // 应用主题切换器样式并同步当前选择
    if (themeComboBox) {
        // 临时断开信号连接，避免递归调用
        themeComboBox->blockSignals(true);
        
        // 同步当前主题选择
        int currentThemeIndex = static_cast<int>(theme->getCurrentTheme());
        if (themeComboBox->currentIndex() != currentThemeIndex) {
            themeComboBox->setCurrentIndex(currentThemeIndex);
        }
        
        // 应用样式
        themeComboBox->setStyleSheet(theme->getThemeSwitcherStyle());
        
        // 重新连接信号
        themeComboBox->blockSignals(false);
    }
    
    // 应用按钮样式
    if (logoutButton) {
        logoutButton->setStyleSheet(theme->getButtonStyle("secondary"));
    }
    
    // 应用菜单样式
    if (mainMenuList) {
        mainMenuList->setStyleSheet(theme->getMenuStyle());
    }
    
    if (subMenuList) {
        subMenuList->setStyleSheet(theme->getListStyle());
    }
    
    // 应用状态栏样式
    if (statusBar) {
        statusBar->setStyleSheet(theme->getStatusBarStyle());
    }
    
    // 应用内容区域样式
    if (contentStack) {
        contentStack->setStyleSheet(theme->getScrollBarStyle());
    }
    
    // 更新默认头像以匹配新主题（仅当没有自定义头像时）
    if (avatarLabel && avatarLabel->pixmap().isNull()) {
        setDefaultAvatar();
    } else if (avatarLabel) {
        // 检查是否使用的是默认头像（通过检查用户设置）
        QSettings settings("YourCompany", "QtApp");
        QString localAvatar = settings.value("user/avatar_local", "").toString();
        QString networkAvatar = settings.value("user/avatar", "").toString();
        
        // 如果没有自定义头像，更新默认头像
        if (localAvatar.isEmpty() && networkAvatar.isEmpty()) {
            setDefaultAvatar();
        }
    }
    
    // 通知所有已创建的页面更新主题
    if (contentStack) {
        for (int i = 0; i < contentStack->count(); ++i) {
            QWidget *page = contentStack->widget(i);
            if (page) {
                // 先检查是否是滚动区域
                QScrollArea *scrollArea = qobject_cast<QScrollArea*>(page);
                QWidget *actualPage = scrollArea ? scrollArea->widget() : page;
                
                if (actualPage) {
                    // 使用Qt元对象系统动态调用applyTheme方法（如果存在）
                    const QMetaObject *metaObject = actualPage->metaObject();
                    int methodIndex = metaObject->indexOfMethod("applyTheme()");
                    if (methodIndex != -1) {
                        // 页面有applyTheme方法，调用它
                        QMetaMethod method = metaObject->method(methodIndex);
                        bool success = method.invoke(actualPage, Qt::DirectConnection);
                        if (success) {
                            // 强制重绘页面以确保主题立即生效
                            actualPage->update();
                            actualPage->repaint();
                            if (scrollArea) {
                                scrollArea->update();
                                scrollArea->repaint();
                            }
                        }
                    } else {
                        // 页面没有applyTheme方法，只更新基本样式
                        actualPage->setStyleSheet(QString(
                            "QWidget { "
                            "    background-color: %1; "
                            "    color: %2; "
                            "    font-family: %3; "
                            "}"
                        ).arg(theme->colors().BACKGROUND)
                         .arg(theme->colors().TEXT_PRIMARY)
                         .arg(ThemeManager::Typography::FONT_FAMILY));
                        actualPage->update();
                        actualPage->repaint();
                        if (scrollArea) {
                            scrollArea->update();
                            scrollArea->repaint();
                        }
                    }
                }
                
                // 如果是滚动区域，也要更新滚动区域的样式
                if (scrollArea) {
                    scrollArea->setStyleSheet(theme->getScrollBarStyle() + QString(
                        "QScrollArea { "
                        "    border: none; "
                        "    background-color: %1; "
                        "}"
                    ).arg(theme->colors().BACKGROUND));
                }
            }
        }
    }
    
    // 强制重绘整个窗口以确保所有更改立即生效
    this->update();
    this->repaint();
}

void MainUIWindow::setupSubMenu(const QString &mainMenu)
{
    subMenuList->clear();

    if (mainMenu.contains("图表示例")) {
        new QListWidgetItem("ECharts示例", subMenuList);
        new QListWidgetItem("日志统计", subMenuList);
    } else if (mainMenu.contains("数据分析")) {
        new QListWidgetItem("用户画像", subMenuList);
        new QListWidgetItem("统计报表", subMenuList);
    } else if (mainMenu.contains("设备信息")) {
        new QListWidgetItem("服务器配置", subMenuList);
        new QListWidgetItem("内容", subMenuList);
    } else if (mainMenu.contains("个人中心")) {
        new QListWidgetItem("用户信息", subMenuList);
        new QListWidgetItem("修改密码", subMenuList);
    }

    // 默认选择第一个二级菜单
    if (subMenuList->count() > 0) {
        subMenuList->setCurrentRow(0);
        // 触发二级菜单点击事件，显示对应的内容
        onSubMenuClicked(subMenuList->currentItem());
    }
}

void MainUIWindow::setupContent()
{
    contentStack = new QStackedWidget();
    contentStack->setStyleSheet(
        "QStackedWidget { "
        "    background-color: #ffffff; "
        "    border: none; "
        "    border-radius: 0; "
        "}"
    );
}

void MainUIWindow::onMainMenuClicked(QListWidgetItem *item)
{
    QString mainMenu = item->text();
    
    // 追踪一级菜单切换事件
    Analytics::SDK::instance()->track("main_menu_changed", {
        {"event_type", "click"},
        {"menu_name", mainMenu}
    });
    
    setupSubMenuContent(mainMenu);
}

void MainUIWindow::onSubMenuClicked(QListWidgetItem *item)
{
    // 安全检查：确保contentStack已初始化
    if (!contentStack) {
        qWarning() << "contentStack is not initialized!";
        return;
    }

    QString subMenu = item->text();
    
    // 追踪页面导航事件
    Analytics::SDK::instance()->track("page_navigated", {
        {"event_type", "view"},
        {"page_name", subMenu}
    });

    // 清空内容区域
    while (contentStack->count() > 0) {
        QWidget *widget = contentStack->widget(0);
        contentStack->removeWidget(widget);
        widget->deleteLater();
    }

    // 根据二级菜单创建对应的内容
    QWidget *contentWidget = nullptr;
    if (subMenu.contains("ECharts示例")) {
        contentWidget = new EChartsTab();
    } else if (subMenu.contains("日志统计")) {
        contentWidget = new LogStatsTab();
    } else if (subMenu.contains("用户画像")) {
        contentWidget = new UserProfileTab();
        // 获取当前登录用户的ID并传递给用户画像页面
        QSettings settings("YourCompany", "QtApp");
        QString userId = settings.value("user/id", "").toString();
        if (!userId.isEmpty()) {
            UserProfileTab *userProfileTab = qobject_cast<UserProfileTab*>(contentWidget);
            if (userProfileTab) {
                userProfileTab->setUserId(userId);
            }
        }
    } else if (subMenu.contains("统计报表")) {
        contentWidget = new ReportsTab();
    } else if (subMenu.contains("服务器配置")) {
        contentWidget = new ServerConfigTab();
    } else if (subMenu.contains("内容")) {
        contentWidget = new ContentTab();
    } else if (subMenu.contains("用户信息")) {
        contentWidget = new UserInfoPage();
    } else if (subMenu.contains("修改密码")) {
        contentWidget = new ChangePasswordPage();
    }

    // 如果创建了内容控件，添加到内容区域
    if (contentWidget) {
        // 确保新创建的页面应用当前主题
        const QMetaObject *metaObject = contentWidget->metaObject();
        int methodIndex = metaObject->indexOfMethod("applyTheme()");
        if (methodIndex != -1) {
            // 页面有applyTheme方法，调用它
            QMetaMethod method = metaObject->method(methodIndex);
            method.invoke(contentWidget, Qt::DirectConnection);
            // 强制重绘以确保主题立即生效
            contentWidget->update();
            contentWidget->repaint();
        } else {
            // 页面没有applyTheme方法，应用基本主题样式
            ThemeManager *theme = ThemeManager::instance();
            contentWidget->setStyleSheet(QString(
                "QWidget { "
                "    background-color: %1; "
                "    color: %2; "
                "    font-family: %3; "
                "}"
            ).arg(theme->colors().BACKGROUND)
             .arg(theme->colors().TEXT_PRIMARY)
             .arg(ThemeManager::Typography::FONT_FAMILY));
            contentWidget->update();
            contentWidget->repaint();
        }
        
        // 将内容控件放在滚动区域中
        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setWidget(contentWidget);
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        
        // 应用主题到滚动区域
        ThemeManager *theme = ThemeManager::instance();
        scrollArea->setStyleSheet(theme->getScrollBarStyle() + QString(
            "QScrollArea { "
            "    border: none; "
            "    background-color: %1; "
            "}"
        ).arg(theme->colors().BACKGROUND));

        contentStack->addWidget(scrollArea);
        contentStack->setCurrentWidget(scrollArea);
        
        // 如果是用户信息页面，连接头像更新信号
        if (subMenu.contains("用户信息")) {
            UserInfoPage *userInfoPage = qobject_cast<UserInfoPage*>(contentWidget);
            if (userInfoPage) {
                // LOG_DEBUG("Connecting avatarUpdated signal for UserInfoPage");
                connect(userInfoPage, &UserInfoPage::avatarUpdated, 
                        this, &MainUIWindow::updateUserInfoSafe, Qt::QueuedConnection);
                
                // 立即更新一次导航栏头像，确保同步（不使用定时器）
                // LOG_DEBUG("Immediate avatar sync for navigation bar");
                updateUserInfoSafe();
            } else {
                // LOG_DEBUG("Failed to cast to UserInfoPage");
            }
        }
    }
}

void MainUIWindow::onLoginSuccess(const QString &token)
{
    Q_UNUSED(token);
    
    // LOG_DEBUG("MainUIWindow::onLoginSuccess called");
    
    // 安全检查：确保所有UI元素都已初始化
    if (!mainStack || !statusIndicator || !statusText || !statusMessage) {
        qWarning() << "UI elements not initialized in onLoginSuccess!";
        return;
    }
    
    // LOG_DEBUG("Switching to main interface");
    
    try {
        // 登录成功，切换到主界面
        mainStack->setCurrentIndex(1);
        
        // LOG_DEBUG("Updating status bar");
        
        // 更新状态栏
        statusIndicator->setStyleSheet(
            "QLabel { "
            "    background-color: #10b981; "
            "    border-radius: 5px; "
            "}"
        );
        statusText->setText("在线");
        statusMessage->setText("登录成功");
        
        // LOG_DEBUG("Scheduling updateUserInfo");
        
        // 立即更新用户信息，不使用定时器避免崩溃
        try {
            // LOG_DEBUG("About to call updateUserInfo immediately");
            updateUserInfoSafe();
            // LOG_DEBUG("updateUserInfo completed successfully");
        } catch (const std::exception& e) {
            // LOG_DEBUG("Exception in updateUserInfo:" << e.what());
        } catch (...) {
            // LOG_DEBUG("Unknown exception in updateUserInfo");
        }
        
        // LOG_DEBUG("onLoginSuccess completed successfully");
        
    } catch (const std::exception& e) {
        // LOG_DEBUG("Exception in onLoginSuccess:" << e.what());
    } catch (...) {
        // LOG_DEBUG("Unknown exception in onLoginSuccess");
    }
}

void MainUIWindow::onLogoutClicked()
{
    // 使用企业级认证服务进行登出
    if (m_authService) {
        m_authService->logout();
        // onAuthenticationChanged 会被自动调用来更新UI
    } else {
        // 回退到原有的登出逻辑
        // 安全检查：确保所有UI元素都已初始化
        if (!mainStack || !usernameLabel || !avatarLabel || !statusIndicator || !statusText || !statusMessage) {
            qWarning() << "UI elements not initialized in onLogoutClicked!";
            return;
        }
        
        // 清除用户信息
        QSettings settings("YourCompany", "QtApp");
        settings.remove("user/token");
        settings.remove("user/email");
        settings.remove("user/password");
        settings.remove("user/remember");
        settings.remove("user/id");
        settings.remove("user/username");
        settings.remove("user/avatar");
        settings.remove("user/createTime");
        settings.sync();
        
        // 用户登出，清除用户信息
        
        // 清除登录页面的输入
        if (loginPage) {
            loginPage->clearUserInfo();
        }
        
        // 重置用户信息显示
        usernameLabel->setText("未登录");
        avatarLabel->clear();
        
        // 更新状态栏
        statusIndicator->setStyleSheet(
            "QLabel { "
            "    background-color: #ef4444; "
            "    border-radius: 5px; "
            "}"
        );
        statusText->setText("离线");
        statusMessage->setText("已登出");
        
        // 返回登录页面
        mainStack->setCurrentIndex(0);
    }
}

void MainUIWindow::updateUserInfoSafe()
{
    // LOG_DEBUG("MainUIWindow::updateUserInfoSafe() called");
    
    try {
        // 安全检查：确保所有UI元素都已初始化
        if (!usernameLabel || !avatarLabel) {
            qWarning() << "UI elements not initialized in updateUserInfoSafe!";
            return;
        }
        
        // LOG_DEBUG("UI elements check passed");
        
        QString username;
        QString localAvatar;
        QString networkAvatar;
        
        // 优先使用企业级认证服务获取用户信息
        if (m_authService && m_authService->isAuthenticated()) {
            UserInfo userInfo = m_authService->getCurrentUser();
            username = userInfo.username;
            networkAvatar = userInfo.avatar;
            // LOG_DEBUG("Using enterprise auth service - username:" << username);
        } else {
            // 回退到从设置中获取用户信息
            QSettings settings("YourCompany", "QtApp");
            username = settings.value("user/username", "").toString();
            localAvatar = settings.value("user/avatar_local", "").toString();
            networkAvatar = settings.value("user/avatar", "").toString();
            // LOG_DEBUG("Using settings - username:" << username);
        }
        
        // LOG_DEBUG("Local avatar:" << localAvatar);
        // LOG_DEBUG("Network avatar:" << networkAvatar);
        
        // 更新用户名
        if (!username.isEmpty()) {
            usernameLabel->setText(username);
            // LOG_DEBUG("Username updated to:" << username);
        } else {
            usernameLabel->setText("未知用户");
            // LOG_DEBUG("Username set to default");
        }
        
        // 优先使用本地头像
        if (!localAvatar.isEmpty() && QFileInfo::exists(localAvatar)) {
            // LOG_DEBUG("Loading local avatar from:" << localAvatar);
            
            QPixmap localPixmap(localAvatar);
            if (!localPixmap.isNull()) {
                // 为导航栏创建40px的圆形头像
                QPixmap circularPixmap = createCircularPixmap(localPixmap, 40);
                if (!circularPixmap.isNull()) {
                    avatarLabel->setPixmap(circularPixmap);
                    // LOG_DEBUG("Local avatar loaded successfully for navigation bar");
                    return; // 使用本地头像，不需要继续
                } else {
                    // LOG_DEBUG("Failed to create circular pixmap from local avatar");
                }
            } else {
                // LOG_DEBUG("Failed to load local avatar pixmap");
            }
        }
        
        // 如果没有本地头像，尝试加载网络头像
        if (!networkAvatar.isEmpty()) {
            // LOG_DEBUG("Loading network avatar from:" << networkAvatar);
            loadNetworkAvatar(networkAvatar);
            return; // 网络头像加载是异步的，直接返回
        }
        
        // 如果都没有，设置默认头像
        setDefaultAvatar();
        
        // LOG_DEBUG("updateUserInfoSafe completed successfully");
        
    } catch (const std::exception& e) {
        // LOG_DEBUG("Exception in updateUserInfoSafe:" << e.what());
        setDefaultAvatar();
    } catch (...) {
        // LOG_DEBUG("Unknown exception in updateUserInfoSafe");
        setDefaultAvatar();
    }
}

void MainUIWindow::forceAvatarSync()
{
    // LOG_DEBUG("forceAvatarSync called");
    updateUserInfoSafe();
}

void MainUIWindow::setDefaultAvatar()
{
    if (!avatarLabel) return;
    
    // 设置一个简单的默认头像
    QPixmap defaultAvatar(40, 40);
    defaultAvatar.fill(Qt::transparent);
    QPainter painter(&defaultAvatar);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 获取当前主题管理器
    ThemeManager *theme = ThemeManager::instance();
    ThemeManager::ThemeType currentTheme = theme->getCurrentTheme();
    
    // 根据主题设置头像颜色
    QString backgroundColor;
    QString iconColor;
    
    switch (currentTheme) {
        case ThemeManager::LIGHT:
            backgroundColor = "#e2e8f0";  // 浅灰色背景
            iconColor = "#94a3b8";       // 中灰色图标
            break;
        case ThemeManager::DARK:
            backgroundColor = "#374151";  // 深灰色背景
            iconColor = "#9ca3af";       // 浅灰色图标
            break;
        case ThemeManager::BLUE:
            backgroundColor = "#dbeafe";  // 浅蓝色背景
            iconColor = "#60a5fa";       // 蓝色图标
            break;
        case ThemeManager::GREEN:
            backgroundColor = "#dcfce7";  // 浅绿色背景
            iconColor = "#4ade80";       // 绿色图标
            break;
        default:
            backgroundColor = "#e2e8f0";
            iconColor = "#94a3b8";
            break;
    }
    
    // 绘制圆形背景
    painter.setBrush(QBrush(QColor(backgroundColor)));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 40, 40);
    
    // 绘制简单的用户图标
    painter.setBrush(QBrush(QColor(iconColor)));
    painter.drawEllipse(12, 8, 16, 16); // 头部
    painter.drawEllipse(6, 26, 28, 16); // 身体
    
    avatarLabel->setPixmap(defaultAvatar);
    // LOG_DEBUG("Theme-responsive default avatar set for theme:" << static_cast<int>(currentTheme));
}

void MainUIWindow::loadNetworkAvatar(const QString &avatarUrl)
{
    // LOG_DEBUG("loadNetworkAvatar called with URL:" << avatarUrl);
    
    if (avatarUrl.isEmpty()) {
        // LOG_DEBUG("Avatar URL is empty, setting default avatar");
        setDefaultAvatar();
        return;
    }
    
    try {
        // 创建网络管理器
        QNetworkAccessManager *manager = new QNetworkAccessManager(this);
        
        // 创建网络请求
        QUrl url(avatarUrl);
        QNetworkRequest request{url};
        request.setRawHeader("User-Agent", "Qt Application");
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
        
        // LOG_DEBUG("Sending network request for avatar");
        
        // 发送GET请求
        QNetworkReply *reply = manager->get(request);
        
        // 连接完成信号
        connect(reply, &QNetworkReply::finished, [this, reply, avatarUrl]() {
            // LOG_DEBUG("Network avatar request finished");
            
            try {
                if (reply->error() == QNetworkReply::NoError) {
                    QByteArray imageData = reply->readAll();
                    // LOG_DEBUG("Received image data size:" << imageData.size());
                    
                    if (!imageData.isEmpty()) {
                        QPixmap pixmap;
                        if (pixmap.loadFromData(imageData)) {
                            // LOG_DEBUG("Successfully loaded pixmap from network data");
                            
                            // 创建圆形头像
                            QPixmap circularPixmap = createCircularPixmap(pixmap, 40);
                            if (!circularPixmap.isNull()) {
                                avatarLabel->setPixmap(circularPixmap);
                                // LOG_DEBUG("Network avatar loaded successfully for navigation bar");
                                
                                // 可选：保存到本地缓存
                                QSettings settings("YourCompany", "QtApp");
                                QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
                                QDir().mkpath(cacheDir);
                                QString localPath = cacheDir + "/avatar_cache.png";
                                
                                if (pixmap.save(localPath, "PNG")) {
                                    settings.setValue("user/avatar_local", localPath);
                                    // LOG_DEBUG("Avatar cached locally at:" << localPath);
                                }
                            } else {
                                // LOG_DEBUG("Failed to create circular pixmap from network avatar");
                                setDefaultAvatar();
                            }
                        } else {
                            // LOG_DEBUG("Failed to load pixmap from network data");
                            setDefaultAvatar();
                        }
                    } else {
                        // LOG_DEBUG("Received empty image data");
                        setDefaultAvatar();
                    }
                } else {
                    // LOG_DEBUG("Network error loading avatar:" << reply->errorString());
                    setDefaultAvatar();
                }
            } catch (const std::exception& e) {
                // LOG_DEBUG("Exception in network avatar callback:" << e.what());
                setDefaultAvatar();
            } catch (...) {
                // LOG_DEBUG("Unknown exception in network avatar callback");
                setDefaultAvatar();
            }
            
            reply->deleteLater();
        });
        
        // 设置超时
        QTimer::singleShot(10000, reply, [reply]() {
            if (reply->isRunning()) {
                // LOG_DEBUG("Network avatar request timeout");
                reply->abort();
            }
        });
        
    } catch (const std::exception& e) {
        // LOG_DEBUG("Exception in loadNetworkAvatar:" << e.what());
        setDefaultAvatar();
    } catch (...) {
        // LOG_DEBUG("Unknown exception in loadNetworkAvatar");
        setDefaultAvatar();
    }
}

void MainUIWindow::updateUserInfo()
{
    // LOG_DEBUG("MainUIWindow::updateUserInfo() called - delegating to updateUserInfoSafe");
    updateUserInfoSafe();
}

QPixmap MainUIWindow::createCircularPixmap(const QPixmap &pixmap, int size)
{
    // LOG_DEBUG("createCircularPixmap called with size:" << size);
    
    try {
        if (pixmap.isNull()) {
            // LOG_DEBUG("Input pixmap is null");
            return QPixmap();
        }
        
        if (size <= 0) {
            // LOG_DEBUG("Invalid size:" << size);
            return QPixmap();
        }
        
        // LOG_DEBUG("Input pixmap size:" << pixmap.size());
        
        // 创建指定大小的圆形图片
        QPixmap result(size, size);
        result.fill(Qt::transparent);
        
        QPainter painter(&result);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        
        // 设置圆形裁剪区域
        QPainterPath clipPath;
        clipPath.addEllipse(0, 0, size, size);
        painter.setClipPath(clipPath);
        
        // 计算缩放，确保图片能够完全覆盖圆形区域
        qreal scaleX = static_cast<qreal>(size) / pixmap.width();
        qreal scaleY = static_cast<qreal>(size) / pixmap.height();
        qreal scale = qMax(scaleX, scaleY); // 使用较大的缩放比例确保完全覆盖
        
        int scaledWidth = static_cast<int>(pixmap.width() * scale);
        int scaledHeight = static_cast<int>(pixmap.height() * scale);
        
        // LOG_DEBUG("Scale:" << scale << "Scaled size:" << scaledWidth << "x" << scaledHeight);
        
        // 缩放图片
        QPixmap scaledPixmap = pixmap.scaled(scaledWidth, scaledHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        
        // 计算居中位置
        int x = (size - scaledWidth) / 2;
        int y = (size - scaledHeight) / 2;
        
        // LOG_DEBUG("Drawing at position:" << x << "," << y);
        
        // 绘制图片，确保完全填充圆形
        painter.drawPixmap(x, y, scaledPixmap);
        painter.end();
        
        // LOG_DEBUG("createCircularPixmap completed successfully");
        return result;
        
    } catch (const std::exception& e) {
        // LOG_DEBUG("Exception in createCircularPixmap:" << e.what());
        return QPixmap();
    } catch (...) {
        // LOG_DEBUG("Unknown exception in createCircularPixmap");
        return QPixmap();
    }
}

void MainUIWindow::initializeEnterpriseServices()
{
    // 获取企业级应用程序实例
    Application* app = Application::instance();
    if (!app) {
        qWarning() << "Enterprise Application not initialized!";
        return;
    }

    // 获取服务管理器
    ServiceManager* serviceManager = app->serviceManager();
    if (!serviceManager) {
        qWarning() << "Service Manager not available!";
        return;
    }

    // 获取网络服务
    m_networkService = serviceManager->getService<INetworkService>("NetworkService");
    if (!m_networkService) {
        qWarning() << "Network Service not available!";
    }

    // 获取认证服务
    m_authService = serviceManager->getService<IAuthenticationService>("AuthenticationService");
    if (m_authService) {
        // 获取具体的AuthenticationService实例以连接信号
        AuthenticationService* authServiceImpl = qobject_cast<AuthenticationService*>(
            serviceManager->getService("AuthenticationService"));
        
        if (authServiceImpl) {
            // 连接认证状态变化信号
            connect(authServiceImpl, &AuthenticationService::authenticationChanged,
                    this, &MainUIWindow::onAuthenticationChanged);
        }
        
        // 检查当前认证状态
        if (m_authService->isAuthenticated()) {
            // 如果已经认证，直接显示主界面
            QTimer::singleShot(100, [this]() {
                onAuthenticationChanged(true);
            });
        }
    } else {
        qWarning() << "Authentication Service not available!";
    }

    qDebug() << "Enterprise services initialized successfully";
}

void MainUIWindow::onAuthenticationChanged(bool authenticated)
{
    if (!mainStack) return;

    if (authenticated) {
        // 认证成功，切换到主界面
        mainStack->setCurrentIndex(1);
        
        // 更新状态栏
        if (statusIndicator && statusText && statusMessage) {
            statusIndicator->setStyleSheet(
                "QLabel { "
                "    background-color: #10b981; "
                "    border-radius: 5px; "
                "}"
            );
            statusText->setText("在线");
            statusMessage->setText("已连接企业服务");
        }
        
        // 更新用户信息显示
        updateUserInfoSafe();
    } else {
        // 认证失败或登出，显示登录页面
        mainStack->setCurrentIndex(0);
        
        // 更新状态栏
        if (statusIndicator && statusText && statusMessage) {
            statusIndicator->setStyleSheet(
                "QLabel { "
                "    background-color: #ef4444; "
                "    border-radius: 5px; "
                "}"
            );
            statusText->setText("离线");
            statusMessage->setText("未连接");
        }
        
        // 清除用户信息显示
        if (usernameLabel && avatarLabel) {
            usernameLabel->setText("未登录");
            avatarLabel->clear();
            setDefaultAvatar();
        }
    }
}