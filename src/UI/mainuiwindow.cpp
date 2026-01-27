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
#include "../Localization/LocalizationManager.h"
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
#include <functional>

MainUIWindow::MainUIWindow(QWidget *parent) : QWidget(parent)
    , appTitle(nullptr)
    , m_networkService(nullptr)
    , m_authService(nullptr)
    , themePopover(nullptr)
    , languagePopover(nullptr)
{
    setWindowTitle(tr("Qt UI控件综合示例"));
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

    // 连接语言变化信号，动态更新界面文本
    LocalizationManager *localizationManager = LocalizationManager::instance();
    if (localizationManager) {
        // 确保 LocalizationManager 已初始化
        if (!localizationManager->currentLanguage().isEmpty()) {
            // 已经初始化
        } else {
            localizationManager->initialize();
        }
        
        connect(localizationManager, &LocalizationManager::languageChanged,
                this, &MainUIWindow::retranslateUi, Qt::QueuedConnection);
    }

    // 按当前语言初始化界面文本
    retranslateUi();
}

void MainUIWindow::setupUI(QWidget *parent)
{
    Q_UNUSED(parent);
    
    // LOG_DEBUG("setupUI started");
    
    try {
        // 创建主布局 - Docker Desktop风格：顶部导航 + 左侧导航 + 主内容区
        if (mainLayout) {
            delete mainLayout;
        }
        mainLayout = new QGridLayout();
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);
        
        // 1. 创建顶部导航栏 (Docker Desktop风格)
        setupNavigationBar();
        mainLayout->addWidget(navigationBar, 0, 0, 1, 2);
        
        // 2. 创建左侧主导航菜单 (类似Docker Desktop侧边栏)
        setupMainMenu();
        mainLayout->addWidget(mainMenuList, 1, 0);
        
        // 3. 创建主内容区域 (移除二级菜单，直接显示内容)
        setupContent();
        mainLayout->addWidget(contentStack, 1, 1);
        
        // 4. 创建状态栏
        setupStatusBar();
        mainLayout->addWidget(statusBar, 2, 0, 1, 2);
        
        // 设置布局比例 - Docker Desktop风格
        mainLayout->setColumnStretch(0, 0);  // 左侧导航固定宽度
        mainLayout->setColumnStretch(1, 1);  // 主内容区域自适应
        mainLayout->setRowStretch(0, 0);     // 顶部导航固定高度
        mainLayout->setRowStretch(1, 1);     // 主要内容行自适应
        mainLayout->setRowStretch(2, 0);     // 状态栏固定高度
        
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
        ThemeManager *themeManager = ThemeManager::instance();
        if (themeManager) {
            connect(themeManager, &ThemeManager::themeChanged,
                    this, &MainUIWindow::applyTheme);
        }
        
        // 初始化菜单 - 直接显示第一个菜单项的内容
        if (mainMenuList && mainMenuList->count() > 0) {
            mainMenuList->setCurrentRow(0);
            onMainMenuClicked(mainMenuList->currentItem());
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
    navigationBar->setFixedHeight(60); // Docker Desktop风格的导航栏高度
    
    QHBoxLayout *navLayout = new QHBoxLayout(navigationBar);
    navLayout->setContentsMargins(20, 0, 20, 0);
    navLayout->setSpacing(16);
    
    // 应用标题和图标 - Docker Desktop风格
    QWidget *titleContainer = new QWidget();
    QHBoxLayout *titleLayout = new QHBoxLayout(titleContainer);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(12);
    
    // 应用图标
    appIcon = new QLabel();
    appIcon->setObjectName("appIcon");
    appIcon->setFixedSize(32, 32);
    appIcon->setText("Q");
    appIcon->setAlignment(Qt::AlignCenter);
    
    // 应用标题 - 使用翻译
    appTitle = new QLabel(tr("Qt Enterprise App"));
    appTitle->setObjectName("appTitle");
    
    titleLayout->addWidget(appIcon);
    titleLayout->addWidget(appTitle);
    titleLayout->addStretch();
    
    navLayout->addWidget(titleContainer);
    navLayout->addStretch();
    
    // 右侧控制区域 - Docker Desktop风格
    QWidget *controlsContainer = new QWidget();
    QHBoxLayout *controlsLayout = new QHBoxLayout(controlsContainer);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->setSpacing(12);
    
    // 语言切换按钮 - Docker Desktop风格
    languageButton = new QPushButton();
    languageButton->setObjectName("dockerStyleButton");
    languageButton->setMinimumWidth(110);
    languageButton->setMaximumWidth(130);
    languageButton->setMinimumHeight(32);
    languageButton->setCursor(Qt::PointingHandCursor);

    LocalizationManager *lm = LocalizationManager::instance();
    const QString currentLang = lm->currentLanguage();
    const QString displayName = lm->getLanguageDisplayName(currentLang);
    languageButton->setText(displayName);

    connect(languageButton, &QPushButton::clicked, this, &MainUIWindow::showLanguagePopover);
    
    // 主题切换按钮 - Docker Desktop风格
    themeButton = new QPushButton();
    themeButton->setObjectName("dockerStyleButton");
    themeButton->setMinimumWidth(110);
    themeButton->setMaximumWidth(130);
    themeButton->setMinimumHeight(32);
    themeButton->setCursor(Qt::PointingHandCursor);
    
    // 设置当前主题显示
    ThemeManager *themeManager = ThemeManager::instance();
    if (themeManager) {
        QString themeText;
        switch (themeManager->getCurrentTheme()) {
            case ThemeManager::LIGHT: themeText = tr("浅色主题"); break;
            case ThemeManager::DARK: themeText = tr("深色主题"); break;
            case ThemeManager::BLUE: themeText = tr("蓝色主题"); break;
            case ThemeManager::GREEN: themeText = tr("绿色主题"); break;
        }
        themeButton->setText(themeText);
    }
    
    connect(themeButton, &QPushButton::clicked, this, &MainUIWindow::showThemePopover);
    
    // 分隔线
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::VLine);
    separator->setFrameShadow(QFrame::Sunken);
    separator->setFixedHeight(24);
    
    // 用户信息区域 - Docker Desktop风格
    QWidget *userContainer = new QWidget();
    QHBoxLayout *userLayout = new QHBoxLayout(userContainer);
    userLayout->setContentsMargins(0, 0, 0, 0);
    userLayout->setSpacing(8);
    
    // 用户头像
    avatarLabel = new QLabel();
    avatarLabel->setObjectName("avatarLabel");
    avatarLabel->setFixedSize(32, 32);
    avatarLabel->setAlignment(Qt::AlignCenter);
    
    // 用户名
    usernameLabel = new QLabel("未登录");
    usernameLabel->setObjectName("usernameLabel");
    usernameLabel->setMinimumWidth(60);
    usernameLabel->setMaximumWidth(120);
    usernameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    // 登出按钮
    logoutButton = new QPushButton(tr("登出"));
    logoutButton->setMinimumWidth(60);
    logoutButton->setMaximumWidth(80);
    connect(logoutButton, &QPushButton::clicked, this, &MainUIWindow::onLogoutClicked);
    
    userLayout->addWidget(avatarLabel);
    userLayout->addWidget(usernameLabel);
    userLayout->addWidget(logoutButton);
    
    controlsLayout->addWidget(languageButton);
    controlsLayout->addWidget(themeButton);
    controlsLayout->addWidget(separator);
    controlsLayout->addWidget(userContainer);
    
    navLayout->addWidget(controlsContainer);
}

void MainUIWindow::setupMainMenu()
{
    mainMenuList = new QListWidget();
    mainMenuList->setObjectName("dockerStyleSidebar");
    mainMenuList->setFixedWidth(200); // Docker Desktop风格的固定宽度侧边栏

    // 添加主菜单项 - 使用安全的文本图标替代emoji
    // 使用 setData 存储菜单标识符，避免依赖翻译文本匹配
    QListWidgetItem *item1 = new QListWidgetItem("■ " + tr("图表示例"));
    item1->setData(Qt::UserRole, "charts");  // 使用标识符而不是文本
    item1->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    item1->setSizeHint(QSize(200, 48)); // Docker Desktop风格的项目高度
    mainMenuList->addItem(item1);

    QListWidgetItem *item2 = new QListWidgetItem("▲ " + tr("数据分析"));
    item2->setData(Qt::UserRole, "data_analysis");  // 使用标识符而不是文本
    item2->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    item2->setSizeHint(QSize(200, 48));
    mainMenuList->addItem(item2);

    QListWidgetItem *item3 = new QListWidgetItem("● " + tr("设备信息"));
    item3->setData(Qt::UserRole, "device_info");  // 使用标识符而不是文本
    item3->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    item3->setSizeHint(QSize(200, 48));
    mainMenuList->addItem(item3);

    QListWidgetItem *item4 = new QListWidgetItem("◆ " + tr("个人中心"));
    item4->setData(Qt::UserRole, "personal_center");  // 使用标识符而不是文本
    item4->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    item4->setSizeHint(QSize(200, 48));
    mainMenuList->addItem(item4);

    connect(mainMenuList, &QListWidget::itemClicked, this, &MainUIWindow::onMainMenuClicked);

    // 默认选择第一个菜单项
    mainMenuList->setCurrentRow(0);
}

void MainUIWindow::setupSubMenu()
{
    // Docker Desktop风格：移除二级菜单，直接在主内容区显示子页面选择
    // 这个方法保留为空，以保持接口兼容性
}

void MainUIWindow::retranslateUi()
{
  //  qDebug() << "retranslateUi called, current language:" << LocalizationManager::instance()->currentLanguage();
    
    // 窗口标题
    setWindowTitle(tr("Qt UI控件综合示例"));
    
    // 应用标题（导航栏中的标题）
    if (appTitle) {
        appTitle->setText(tr("Qt Enterprise App"));
    }
    
    // 应用标题（导航栏中的标题）
    if (appTitle) {
        appTitle->setText(tr("Qt Enterprise App"));
    }

    // 主题切换按钮文本
    if (themeButton) {
        ThemeManager *themeManager = ThemeManager::instance();
        if (themeManager) {
            QString themeText;
            switch (themeManager->getCurrentTheme()) {
                case ThemeManager::LIGHT: themeText = tr("浅色主题"); break;
                case ThemeManager::DARK: themeText = tr("深色主题"); break;
                case ThemeManager::BLUE: themeText = tr("蓝色主题"); break;
                case ThemeManager::GREEN: themeText = tr("绿色主题"); break;
            }
            themeButton->setText(themeText);
        }
    }

    // 登出按钮
    if (logoutButton) {
        logoutButton->setText(tr("登出"));
    }

    // 状态栏文本（根据当前状态更新）
    if (statusText) {
        QString currentStatus = statusText->text();
        if (currentStatus.isEmpty() || currentStatus == tr("离线") || currentStatus == "离线") {
            statusText->setText(tr("离线"));
        } else if (currentStatus == tr("在线") || currentStatus == "在线") {
            statusText->setText(tr("在线"));
        } else {
            statusText->setText(tr("离线"));
        }
    }
    if (statusMessage) {
        QString currentMsg = statusMessage->text();
        if (currentMsg.isEmpty() || currentMsg == tr("就绪") || currentMsg == "就绪") {
            statusMessage->setText(tr("就绪"));
        } else {
            statusMessage->setText(tr("就绪"));
        }
    }

    // 主菜单 - Docker Desktop风格，使用安全的文本图标
    // 更新文本时保持标识符不变，确保多语言下都能正确匹配
    if (mainMenuList && mainMenuList->count() >= 4) {
        mainMenuList->blockSignals(true);
        mainMenuList->item(0)->setText("■ " + tr("图表示例"));
        if (mainMenuList->item(0)->data(Qt::UserRole).toString().isEmpty()) {
            mainMenuList->item(0)->setData(Qt::UserRole, "charts");
        }
        mainMenuList->item(1)->setText("▲ " + tr("数据分析"));
        if (mainMenuList->item(1)->data(Qt::UserRole).toString().isEmpty()) {
            mainMenuList->item(1)->setData(Qt::UserRole, "data_analysis");
        }
        mainMenuList->item(2)->setText("● " + tr("设备信息"));
        if (mainMenuList->item(2)->data(Qt::UserRole).toString().isEmpty()) {
            mainMenuList->item(2)->setData(Qt::UserRole, "device_info");
        }
        mainMenuList->item(3)->setText("◆ " + tr("个人中心"));
        if (mainMenuList->item(3)->data(Qt::UserRole).toString().isEmpty()) {
            mainMenuList->item(3)->setData(Qt::UserRole, "personal_center");
        }
        mainMenuList->blockSignals(false);
    }

    // 语言切换按钮显示名来自 LocalizationManager
    if (languageButton) {
        LocalizationManager *lm = LocalizationManager::instance();
        const QString currentLang = lm->currentLanguage();
        const QString displayName = lm->getLanguageDisplayName(currentLang);
        languageButton->setText(displayName);
    }
    
    // 用户名标签（如果已登录）
    if (usernameLabel) {
        QString currentUsername = usernameLabel->text();
        if (currentUsername == tr("未登录") || currentUsername == "未登录") {
            usernameLabel->setText(tr("未登录"));
        } else if (currentUsername == tr("未知用户") || currentUsername == "未知用户") {
            usernameLabel->setText(tr("未知用户"));
        }
    }
    
   // qDebug() << "retranslateUi completed";
   
   // 如果当前有显示的内容页面，需要刷新它以应用新的翻译
   // 获取当前选中的菜单项，重新创建内容页面
   if (mainMenuList && mainMenuList->currentItem()) {
       QListWidgetItem *currentItem = mainMenuList->currentItem();
       // 延迟刷新，确保翻译已加载
       QTimer::singleShot(100, this, [this, currentItem]() {
           if (currentItem) {
               onMainMenuClicked(currentItem);
           }
       });
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
    
    statusText = new QLabel(tr("离线"));
    statusText->setObjectName("statusText");
    
    statusMessage = new QLabel(tr("就绪"));
    statusMessage->setObjectName("statusMessage");
    
    statusLayout->addWidget(statusIndicator);
    statusLayout->addWidget(statusText);
    statusLayout->addStretch();
    statusLayout->addWidget(statusMessage);
}

void MainUIWindow::applyTheme()
{
    ThemeManager *theme = ThemeManager::instance();
    if (!theme) {
        qWarning() << "ThemeManager instance is null!";
        return;
    }
    
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
    
    // 应用Docker Desktop风格的导航栏样式
    if (navigationBar) {
        navigationBar->setStyleSheet(QString(
            "QWidget#navigationBar { "
            "    background-color: %1; "
            "    border-bottom: 1px solid %2; "
            "} "
            "QLabel#appTitle { "
            "    font-size: 18px; "
            "    font-weight: 600; "
            "    color: %3; "
            "} "
            "QLabel#appIcon { "
            "    background-color: %4; "
            "    border-radius: 6px; "
            "    color: %5; "
            "    font-weight: bold; "
            "    font-size: 16px; "
            "} "
        ).arg(theme->colors().SURFACE)
         .arg(theme->colors().BORDER)
         .arg(theme->colors().TEXT_PRIMARY)
         .arg(theme->colors().PRIMARY)
         .arg(theme->colors().SURFACE));
    }
    
    // 应用Docker Desktop风格的侧边栏样式
    if (mainMenuList) {
        mainMenuList->setStyleSheet(QString(
            "QListWidget#dockerStyleSidebar { "
            "    background-color: %1; "
            "    border: none; "
            "    border-right: 1px solid %2; "
            "    outline: none; "
            "} "
            "QListWidget#dockerStyleSidebar::item { "
            "    padding: 12px 16px; "
            "    border: none; "
            "    color: %3; "
            "    font-size: 14px; "
            "    font-weight: 500; "
            "} "
            "QListWidget#dockerStyleSidebar::item:selected { "
            "    background-color: %4; "
            "    color: %5; "
            "    border-left: 3px solid %6; "
            "} "
            "QListWidget#dockerStyleSidebar::item:hover { "
            "    background-color: %7; "
            "} "
        ).arg(theme->colors().SURFACE)
         .arg(theme->colors().BORDER)
         .arg(theme->colors().TEXT_SECONDARY)
         .arg(theme->colors().PRIMARY_LIGHT)
         .arg(theme->colors().PRIMARY)
         .arg(theme->colors().PRIMARY)
         .arg(theme->colors().GRAY_100));
    }
    
    // 应用Docker Desktop风格的按钮样式
    if (themeButton) {
        themeButton->setStyleSheet(QString(
            "QPushButton#dockerStyleButton { "
            "    background-color: %1; "
            "    border: 1px solid %2; "
            "    border-radius: 6px; "
            "    padding: 6px 12px; "
            "    color: %3; "
            "    font-size: 13px; "
            "    font-weight: 500; "
            "} "
            "QPushButton#dockerStyleButton:hover { "
            "    background-color: %4; "
            "    border-color: %5; "
            "} "
            "QPushButton#dockerStyleButton:pressed { "
            "    background-color: %6; "
            "} "
        ).arg(theme->colors().SURFACE)
         .arg(theme->colors().BORDER)
         .arg(theme->colors().TEXT_PRIMARY)
         .arg(theme->colors().GRAY_100)
         .arg(theme->colors().PRIMARY_LIGHT)
         .arg(theme->colors().PRIMARY_LIGHT));
    }

    // 应用Docker Desktop风格的按钮样式
    if (languageButton) {
        languageButton->setStyleSheet(QString(
            "QPushButton#dockerStyleButton { "
            "    background-color: %1; "
            "    border: 1px solid %2; "
            "    border-radius: 6px; "
            "    padding: 6px 12px; "
            "    color: %3; "
            "    font-size: 13px; "
            "    font-weight: 500; "
            "} "
            "QPushButton#dockerStyleButton:hover { "
            "    background-color: %4; "
            "    border-color: %5; "
            "} "
            "QPushButton#dockerStyleButton:pressed { "
            "    background-color: %6; "
            "} "
        ).arg(theme->colors().SURFACE)
         .arg(theme->colors().BORDER)
         .arg(theme->colors().TEXT_PRIMARY)
         .arg(theme->colors().GRAY_100)
         .arg(theme->colors().PRIMARY_LIGHT)
         .arg(theme->colors().PRIMARY_LIGHT));
    }
    
    // 应用按钮样式
    if (logoutButton) {
        logoutButton->setStyleSheet(theme->getButtonStyle("secondary"));
    }
    
    // 应用状态栏样式
    if (statusBar) {
        statusBar->setStyleSheet(theme->getStatusBarStyle());
    }
    
    // 应用Docker Desktop风格的内容区域样式
    if (contentStack) {
        contentStack->setStyleSheet(QString(
            "QStackedWidget#dockerStyleContent { "
            "    background-color: %1; "
            "    border: none; "
            "} "
        ).arg(theme->colors().BACKGROUND) + theme->getScrollBarStyle());
    }
    
    // 更新默认头像以匹配新主题（仅当没有自定义头像时）
    if (avatarLabel && avatarLabel->pixmap().isNull()) {
        setDefaultAvatar();
    } else if (avatarLabel) {
        QSettings settings("YourCompany", "QtApp");
        QString localAvatar = settings.value("user/avatar_local", "").toString();
        QString networkAvatar = settings.value("user/avatar", "").toString();
        
        if (localAvatar.isEmpty() && networkAvatar.isEmpty()) {
            setDefaultAvatar();
        }
    }
    
    // 通知所有已创建的页面更新主题
    if (contentStack) {
        for (int i = 0; i < contentStack->count(); ++i) {
            QWidget *page = contentStack->widget(i);
            if (page) {
                QScrollArea *scrollArea = qobject_cast<QScrollArea*>(page);
                QWidget *actualPage = scrollArea ? scrollArea->widget() : page;
                
                if (actualPage) {
                    const QMetaObject *metaObject = actualPage->metaObject();
                    int methodIndex = metaObject->indexOfMethod("applyTheme()");
                    if (methodIndex != -1) {
                        QMetaMethod method = metaObject->method(methodIndex);
                        bool success = method.invoke(actualPage, Qt::DirectConnection);
                        if (success) {
                            actualPage->update();
                            actualPage->repaint();
                            if (scrollArea) {
                                scrollArea->update();
                                scrollArea->repaint();
                            }
                        }
                    } else {
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

void MainUIWindow::setupContent()
{
    contentStack = new QStackedWidget();
    contentStack->setObjectName("dockerStyleContent");
    contentStack->setStyleSheet(
        "QStackedWidget#dockerStyleContent { "
        "    background-color: #ffffff; "
        "    border: none; "
        "    border-radius: 0; "
        "}"
    );
}

void MainUIWindow::onMainMenuClicked(QListWidgetItem *item)
{
    if (!item) {
        return;
    }
    
    // 从 item 的 data 中获取菜单标识符（不依赖翻译文本）
    QString menuId = item->data(Qt::UserRole).toString();
    
    // 如果没有标识符，尝试从文本中提取（向后兼容）
    QString menuText;
    if (menuId.isEmpty()) {
        QString mainMenu = item->text();
        // 移除图标前缀，获取纯文本
        if (mainMenu.contains(" ")) {
            menuText = mainMenu.split(" ", Qt::SkipEmptyParts).last();
        } else {
            menuText = mainMenu;
        }
    } else {
        // 使用标识符，但为了显示标题，仍然需要获取翻译后的文本
        menuText = item->text();
        if (menuText.contains(" ")) {
            menuText = menuText.split(" ", Qt::SkipEmptyParts).last();
        }
    }
    
    // 追踪一级菜单切换事件
    Analytics::SDK::instance()->track("main_menu_changed", {
        {"event_type", "click"},
        {"menu_name", menuText},
        {"menu_id", menuId}
    });
    
    // Docker Desktop风格：直接显示对应的内容页面，创建子页面选择界面
    // 传递标识符而不是翻译文本，确保多语言下都能正确匹配
    setupMainMenuContent(menuId.isEmpty() ? menuText : menuId, menuText);
}

void MainUIWindow::setupMainMenuContent(const QString &mainMenuId, const QString &menuDisplayText)
{
    // 安全检查：确保contentStack已初始化
    if (!contentStack) {
        qWarning() << "contentStack is not initialized!";
        return;
    }

    // 清空内容区域
    while (contentStack->count() > 0) {
        QWidget *widget = contentStack->widget(0);
        contentStack->removeWidget(widget);
        widget->deleteLater();
    }

    // 创建Docker Desktop风格的子页面选择界面
    QWidget *menuContentWidget = new QWidget();
    QVBoxLayout *menuLayout = new QVBoxLayout(menuContentWidget);
    menuLayout->setContentsMargins(32, 32, 32, 32);
    menuLayout->setSpacing(24);
    
    // 页面标题 - 使用传入的显示文本
    QLabel *titleLabel = new QLabel(menuDisplayText);
    titleLabel->setObjectName("pageTitle");
    titleLabel->setStyleSheet(
        "QLabel#pageTitle { "
        "    font-size: 28px; "
        "    font-weight: 600; "
        "    color: #1f2937; "
        "    margin-bottom: 16px; "
        "}"
    );
    menuLayout->addWidget(titleLabel);
    
    // 子页面网格容器
    QWidget *gridContainer = new QWidget();
    QGridLayout *gridLayout = new QGridLayout(gridContainer);
    gridLayout->setSpacing(20);
    
    // 根据主菜单标识符创建子页面卡片（只使用标识符匹配，不依赖翻译文本）
    QStringList subPages;
    QStringList subPageIcons;
    QStringList subPageDescriptions;
    
    if (mainMenuId == "charts") {
        subPages << tr("ECharts示例") << tr("日志统计");
        subPageIcons << "■" << "▣";
        subPageDescriptions << tr("交互式图表展示") << tr("系统日志分析");
    } else if (mainMenuId == "data_analysis") {
        subPages << tr("用户画像") << tr("统计报表");
        subPageIcons << "◉" << "▲";
        subPageDescriptions << tr("用户行为分析") << tr("数据统计报告");
    } else if (mainMenuId == "device_info") {
        subPages << tr("服务器配置监控") << tr("电脑本机配置监控");
        subPageIcons << "●" << "○";
        subPageDescriptions << tr("服务器状态监控") << tr("本地设备信息");
    } else if (mainMenuId == "personal_center") {
        subPages << tr("用户信息") << tr("修改密码");
        subPageIcons << "◆" << "◇";
        subPageDescriptions << tr("个人资料管理") << tr("账户安全设置");
    }
    
    // 创建子页面卡片
    for (int i = 0; i < subPages.size(); ++i) {
        QPushButton *cardButton = createSubPageCard(
            subPageIcons[i], 
            subPages[i], 
            subPageDescriptions[i]
        );
        
        // 连接点击事件
        connect(cardButton, &QPushButton::clicked, [this, subPages, i]() {
            showSubPageContent(subPages[i]);
        });
        
        gridLayout->addWidget(cardButton, i / 2, i % 2);
    }
    
    menuLayout->addWidget(gridContainer);
    menuLayout->addStretch();
    
    // 将内容控件放在滚动区域中
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidget(menuContentWidget);
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
}

QPushButton* MainUIWindow::createSubPageCard(const QString &icon, const QString &title, const QString &description)
{
    QPushButton *card = new QPushButton();
    card->setObjectName("subPageCard");
    card->setFixedSize(280, 120);
    card->setCursor(Qt::PointingHandCursor);
    
    // 创建卡片内容
    QWidget *cardContent = new QWidget(card);
    QVBoxLayout *cardLayout = new QVBoxLayout(cardContent);
    cardLayout->setContentsMargins(20, 16, 20, 16);
    cardLayout->setSpacing(8);
    
    // 图标和标题行
    QWidget *headerWidget = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(12);
    
    QLabel *iconLabel = new QLabel(icon);
    iconLabel->setStyleSheet("font-size: 24px;");
    
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(
        "font-size: 16px; "
        "font-weight: 600; "
        "color: #1f2937;"
    );
    
    headerLayout->addWidget(iconLabel);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    
    // 描述文本
    QLabel *descLabel = new QLabel(description);
    descLabel->setStyleSheet(
        "font-size: 14px; "
        "color: #6b7280; "
        "line-height: 1.4;"
    );
    descLabel->setWordWrap(true);
    
    cardLayout->addWidget(headerWidget);
    cardLayout->addWidget(descLabel);
    cardLayout->addStretch();
    
    // 设置卡片样式
    card->setStyleSheet(
        "QPushButton#subPageCard { "
        "    background-color: #ffffff; "
        "    border: 1px solid #e5e7eb; "
        "    border-radius: 8px; "
        "    text-align: left; "
        "} "
        "QPushButton#subPageCard:hover { "
        "    background-color: #f9fafb; "
        "    border-color: #d1d5db; "
        "    box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1); "
        "} "
        "QPushButton#subPageCard:pressed { "
        "    background-color: #f3f4f6; "
        "}"
    );
    
    return card;
}

void MainUIWindow::showSubPageContent(const QString &subPage)
{
    // 安全检查：确保contentStack已初始化
    if (!contentStack) {
        qWarning() << "contentStack is not initialized!";
        return;
    }
    
    // 追踪页面导航事件
    Analytics::SDK::instance()->track("page_navigated", {
        {"event_type", "view"},
        {"page_name", subPage}
    });

    // 清空内容区域
    while (contentStack->count() > 0) {
        QWidget *widget = contentStack->widget(0);
        contentStack->removeWidget(widget);
        widget->deleteLater();
    }

    // 根据子页面创建对应的内容
    QWidget *contentWidget = nullptr;
    if (subPage.contains("ECharts示例")) {
        contentWidget = new EChartsTab();
    } else if (subPage.contains("日志统计")) {
        contentWidget = new LogStatsTab();
    } else if (subPage.contains("用户画像")) {
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
    } else if (subPage.contains("统计报表")) {
        contentWidget = new ReportsTab();
    } else if (subPage.contains("服务器配置监控")) {
        try {
            contentWidget = new ServerConfigTab(this);
        } catch (const std::exception& e) {
            qCritical() << "Exception creating ServerConfigTab:" << e.what();
            contentWidget = new QLabel(tr("加载页面失败: ") + QString::fromStdString(e.what()), this);
        } catch (...) {
            qCritical() << "Unknown exception creating ServerConfigTab";
            contentWidget = new QLabel(tr("加载页面失败: 未知错误"), this);
        }
    } else if (subPage.contains("电脑本机配置监控")) {
        try {
            contentWidget = new ContentTab(this);
        } catch (const std::exception& e) {
            qCritical() << "Exception creating ContentTab:" << e.what();
            contentWidget = new QLabel(tr("加载页面失败: ") + QString::fromStdString(e.what()), this);
        } catch (...) {
            qCritical() << "Unknown exception creating ContentTab";
            contentWidget = new QLabel(tr("加载页面失败: 未知错误"), this);
        }
    } else if (subPage.contains("用户信息")) {
        contentWidget = new UserInfoPage();
    } else if (subPage.contains("修改密码")) {
        contentWidget = new ChangePasswordPage();
    }

    // 如果创建了内容控件，添加到内容区域
    if (contentWidget) {
        // 确保新创建的页面应用当前主题
        const QMetaObject *metaObject = contentWidget->metaObject();
        int methodIndex = metaObject->indexOfMethod("applyTheme()");
        if (methodIndex != -1) {
            QMetaMethod method = metaObject->method(methodIndex);
            method.invoke(contentWidget, Qt::QueuedConnection);
            contentWidget->update();
            contentWidget->repaint();
        } else {
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
        if (subPage.contains("用户信息")) {
            UserInfoPage *userInfoPage = qobject_cast<UserInfoPage*>(contentWidget);
            if (userInfoPage) {
                connect(userInfoPage, &UserInfoPage::avatarUpdated, 
                        this, &MainUIWindow::updateUserInfoSafe, Qt::QueuedConnection);
                updateUserInfoSafe();
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
        statusText->setText(tr("在线"));
        statusMessage->setText(tr("登录成功"));
        
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
        usernameLabel->setText(tr("未登录"));
        avatarLabel->clear();
        
        // 更新状态栏
        statusIndicator->setStyleSheet(
            "QLabel { "
            "    background-color: #ef4444; "
            "    border-radius: 5px; "
            "}"
        );
        statusText->setText(tr("离线"));
        statusMessage->setText(tr("已登出"));
        
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
            usernameLabel->setText(tr("未知用户"));
            // LOG_DEBUG("Username set to default");
        }
        
        // 优先使用本地头像
        if (!localAvatar.isEmpty()) {
            // 先检查路径是否有效
            QFileInfo fileInfo(localAvatar);
            if (fileInfo.exists() && fileInfo.isReadable()) {
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
            } else {
                // 如果路径无效，清除本地头像设置
                QSettings settings("YourCompany", "QtApp");
                settings.remove("user/avatar_local");
                // LOG_DEBUG("Invalid local avatar path, cleared setting");
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
    
    // 设置一个简单的默认头像 - Docker Desktop风格，更小尺寸
    QPixmap defaultAvatar(32, 32);
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
    painter.drawEllipse(0, 0, 32, 32);
    
    // 绘制简单的用户图标 - 调整为32px尺寸
    painter.setBrush(QBrush(QColor(iconColor)));
    painter.drawEllipse(10, 6, 12, 12); // 头部
    painter.drawEllipse(5, 20, 22, 12); // 身体
    
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
                                if (cacheDir.isEmpty()) {
                                    // 如果无法获取缓存目录，使用应用数据目录作为备选
                                    cacheDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
                                }
                                if (!cacheDir.isEmpty()) {
                                    QDir().mkpath(cacheDir);
                                    QString localPath = cacheDir + "/avatar_cache.png";
                                    // 确保路径是有效的
                                    QFileInfo fileInfo(localPath);
                                    QFileInfo dirInfo(fileInfo.dir().path());
                                    if (fileInfo.dir().exists() && dirInfo.isWritable()) {
                                        if (pixmap.save(localPath, "PNG")) {
                                            settings.setValue("user/avatar_local", localPath);
                                            // LOG_DEBUG("Avatar cached locally at:" << localPath);
                                        }
                                    }
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

    // qDebug() << "Enterprise services initialized successfully";
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
            statusText->setText(tr("在线"));
            statusMessage->setText(tr("已连接企业服务"));
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
            statusText->setText(tr("离线"));
            statusMessage->setText(tr("未连接"));
        }
        
        // 清除用户信息显示
        if (usernameLabel && avatarLabel) {
            usernameLabel->setText(tr("未登录"));
            avatarLabel->clear();
            setDefaultAvatar();
        }
    }
}

void MainUIWindow::showLanguagePopover()
{
    hidePopovers(); // 先隐藏其他弹窗
    
    LocalizationManager *lm = LocalizationManager::instance();
    const QStringList languages = lm->availableLanguages();
    const QString currentLang = lm->currentLanguage();
    
    QStringList displayNames;
    QStringList codes;
    
    for (const QString &code : languages) {
        displayNames << lm->getLanguageDisplayName(code);
        codes << code;
    }
    
    languagePopover = createPopover(displayNames, codes, currentLang, 
        [this, lm](const QString &code) {
            if (lm) {
                lm->setLanguage(code);
                const QString displayName = lm->getLanguageDisplayName(code);
                if (languageButton) {
                    languageButton->setText(displayName);
                }
            }
            // 不需要手动调用hidePopovers，弹窗会自动关闭
        });
    
    if (languagePopover && languageButton) {
        // 定位弹窗到按钮下方
        QPoint buttonPos = languageButton->mapToGlobal(QPoint(0, languageButton->height()));
        languagePopover->move(buttonPos);
        languagePopover->show();
    }
}

void MainUIWindow::showThemePopover()
{
    hidePopovers(); // 先隐藏其他弹窗
    
    QStringList themeNames = {tr("浅色主题"), tr("深色主题"), tr("蓝色主题"), tr("绿色主题")};
    QStringList themeValues = {"0", "1", "2", "3"};
    
    ThemeManager *themeManager = ThemeManager::instance();
    QString currentTheme = QString::number(static_cast<int>(themeManager->getCurrentTheme()));
    
    themePopover = createPopover(themeNames, themeValues, currentTheme,
        [this](const QString &themeIndex) {
            ThemeManager *tm = ThemeManager::instance();
            if (tm) {
                int index = themeIndex.toInt();
                tm->setTheme(static_cast<ThemeManager::ThemeType>(index));
                
                if (themeButton) {
                    QString themeText;
                    switch (index) {
                        case 0: themeText = tr("浅色主题"); break;
                        case 1: themeText = tr("深色主题"); break;
                        case 2: themeText = tr("蓝色主题"); break;
                        case 3: themeText = tr("绿色主题"); break;
                    }
                    themeButton->setText(themeText);
                }
            }
            // 不需要手动调用hidePopovers，弹窗会自动关闭
        });
    
    if (themePopover && themeButton) {
        // 定位弹窗到按钮下方
        QPoint buttonPos = themeButton->mapToGlobal(QPoint(0, themeButton->height()));
        themePopover->move(buttonPos);
        themePopover->show();
    }
}

void MainUIWindow::hidePopovers()
{
    // 使用QPointer的安全检查，避免访问已删除的对象
    if (languagePopover) {
        languagePopover->close(); // 使用close()而不是hide()
        languagePopover = nullptr; // 不调用deleteLater，让Qt自动管理
    }
    
    if (themePopover) {
        themePopover->close(); // 使用close()而不是hide()
        themePopover = nullptr; // 不调用deleteLater，让Qt自动管理
    }
}

QWidget* MainUIWindow::createPopover(const QStringList &items, const QStringList &values, 
                                   const QString &currentValue, std::function<void(const QString&)> onSelect)
{
    QWidget *popover = new QWidget(nullptr);
    popover->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    popover->setAttribute(Qt::WA_DeleteOnClose, true);
    
    // Docker Desktop风格的弹窗样式
    ThemeManager *theme = ThemeManager::instance();
    popover->setStyleSheet(QString(
        "QWidget { "
        "    background-color: %1; "
        "    border: 1px solid %2; "
        "    border-radius: 12px; "
        "    padding: 8px 0px; "
        "} "
        "QPushButton { "
        "    background-color: transparent; "
        "    border: none; "
        "    padding: 10px 20px; "
        "    text-align: left; "
        "    color: %3; "
        "    font-size: 14px; "
        "    font-weight: 500; "
        "    border-radius: 8px; "
        "    margin: 2px 8px; "
        "} "
        "QPushButton:hover { "
        "    background-color: %4; "
        "    color: %5; "
        "} "
        "QPushButton:pressed { "
        "    background-color: %6; "
        "} "
        "QPushButton[selected=\"true\"] { "
        "    background-color: %7; "
        "    color: %8; "
        "    font-weight: 600; "
        "} "
    ).arg(theme->colors().SURFACE)
     .arg(theme->colors().BORDER)
     .arg(theme->colors().TEXT_PRIMARY)
     .arg(theme->colors().GRAY_100)
     .arg(theme->colors().TEXT_PRIMARY)
     .arg(theme->colors().PRIMARY_LIGHT)
     .arg(theme->colors().PRIMARY)
     .arg("#ffffff"));
    
    QVBoxLayout *layout = new QVBoxLayout(popover);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    for (int i = 0; i < items.size(); ++i) {
        QPushButton *item = new QPushButton(items[i]);
        item->setCursor(Qt::PointingHandCursor);
        item->setMinimumWidth(140);
        item->setMinimumHeight(36);
        
        // Docker Desktop风格的选中状态
        if (values[i] == currentValue) {
            item->setProperty("selected", true);
        }
        
        // 连接点击事件
        connect(item, &QPushButton::clicked, [onSelect, values, i, popover]() {
            onSelect(values[i]);
            if (popover) {
                popover->close();
            }
        });
        
        layout->addWidget(item);
    }
    
    // Docker Desktop风格的阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 25));
    shadow->setOffset(0, 8);
    popover->setGraphicsEffect(shadow);
    
    // 设置最小尺寸
    popover->setMinimumWidth(160);
    
    return popover;
}