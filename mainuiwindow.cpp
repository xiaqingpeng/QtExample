#include "mainuiwindow.h"
#include "advancedcontrols/advancedcontrolstab.h"
#include "basiccontrols/basiccontrolstab.h"
#include "datadisplay/datadisplaytab.h"
#include "dialogs/dialogstab.h"
#include "layoutexamples1/layoutexamplestab1.h"
#include "layoutexamples2/layoutexamplestab2.h"
#include "layoutexamples3/layoutexamplestab3.h"
#include "echarts/echartstab.h"
#include "echarts/logstatstab.h"
#include "userprofiletab/userprofiletab.h"
#include "reportstab/reportstab.h"
#include "loginpage.h"
#include "changepasswordpage.h"
#include "userinfopage.h"
#include <QFont>
#include <QListWidgetItem>
#include <QScrollArea>
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

MainUIWindow::MainUIWindow(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("Qt UI控件综合示例");
    resize(1200, 800);

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
    
    // 创建标题栏容器（现代简约设计）
    QWidget *titleBarWidget = new QWidget();
    titleBarWidget->setFixedHeight(72);
    titleBarWidget->setStyleSheet(
        "QWidget { "
        "    background: #ffffff; "
        "    border-bottom: 1px solid #f1f5f9; "
        "}"
    );
    
    QHBoxLayout *titleBarLayout = new QHBoxLayout(titleBarWidget);
    titleBarLayout->setContentsMargins(24, 12, 24, 12);
    titleBarLayout->setSpacing(20);
    
    // 用户头像容器（现代简约设计）
    QWidget *avatarContainer = new QWidget();
    avatarContainer->setFixedSize(56, 56);
    avatarContainer->setStyleSheet(
        "QWidget { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #3b82f6, stop:1 #1d4ed8); "
        "    border-radius: 28px; "
        "}"
    );
    
    QVBoxLayout *avatarLayout = new QVBoxLayout(avatarContainer);
    avatarLayout->setContentsMargins(2, 2, 2, 2);
    
    // 用户头像
    avatarLabel = new QLabel();
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setFixedSize(52, 52);
    // 现代简约头像样式
    avatarLabel->setStyleSheet(
        "QLabel { "
        "    background-color: #f8fafc; "
        "    border-radius: 26px; "
        "    border: 2px solid #ffffff; "
        "}"
    );
    // 添加微妙阴影效果
    QGraphicsDropShadowEffect *avatarShadow = new QGraphicsDropShadowEffect(this);
    avatarShadow->setBlurRadius(8);
    avatarShadow->setColor(QColor(0, 0, 0, 20));
    avatarShadow->setOffset(0, 2);
    avatarLabel->setGraphicsEffect(avatarShadow);
    
    // 在线状态指示器（现代设计）
    QLabel *onlineIndicator = new QLabel();
    onlineIndicator->setFixedSize(14, 14);
    onlineIndicator->setStyleSheet(
        "QLabel { "
        "    background-color: #10b981; "
        "    border-radius: 7px; "
        "    border: 2px solid white; "
        "}"
    );
    // 添加微妙阴影
    QGraphicsDropShadowEffect *indicatorShadow = new QGraphicsDropShadowEffect(this);
    indicatorShadow->setBlurRadius(4);
    indicatorShadow->setColor(QColor(0, 0, 0, 30));
    indicatorShadow->setOffset(0, 1);
    onlineIndicator->setGraphicsEffect(indicatorShadow);
    
    // 使用绝对定位将状态指示器放在头像右下角
    onlineIndicator->setParent(avatarContainer);
    onlineIndicator->move(42, 42);
    
    // 设置默认头像（现代简约风格）
    QPixmap defaultAvatar(48, 48);
    defaultAvatar.fill(Qt::transparent);
    QPainter painter(&defaultAvatar);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 创建现代简约的渐变背景
    QRadialGradient gradient(24, 24, 24);
    gradient.setColorAt(0, QColor("#f1f5f9"));
    gradient.setColorAt(1, QColor("#e2e8f0"));
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 48, 48);
    
    // 绘制现代用户图标
    painter.setBrush(QBrush(QColor("#94a3b8")));
    painter.drawEllipse(14, 12, 20, 20); // 头部
    painter.drawEllipse(8, 32, 32, 18); // 身体
    
    avatarLabel->setPixmap(defaultAvatar);
    
    avatarLayout->addWidget(avatarLabel, 0, Qt::AlignCenter);
    
    // 用户名（现代简约风格）
    usernameLabel = new QLabel("未登录");
    usernameLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 16px; "
        "    font-weight: 600; "
        "    color: #1e293b; "
        "    padding: 6px 12px; "
        "}"
    );
    
    titleBarLayout->addWidget(avatarContainer);
    titleBarLayout->addWidget(usernameLabel);
    titleBarLayout->addStretch();
    
    // 登出按钮（现代简约设计）
    logoutButton = new QPushButton("登出");
    logoutButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #f8fafc; "
        "    color: #475569; "
        "    border: 1px solid #e2e8f0; "
        "    padding: 10px 24px; "
        "    border-radius: 8px; "
        "    font-size: 14px; "
        "    font-weight: 500; "
        "}"
        "QPushButton:hover { "
        "    background-color: #f1f5f9; "
        "    border-color: #cbd5e1; "
        "    color: #334155; "
        "}"
        "QPushButton:pressed { "
        "    background-color: #e2e8f0; "
        "    border-color: #94a3b8; "
        "}"
    );
    connect(logoutButton, &QPushButton::clicked, this, &MainUIWindow::onLogoutClicked);
    
    titleBarLayout->addWidget(logoutButton);
    
    mainLayout->addWidget(titleBarWidget, 0, 0, 1, 3);

    // 一级菜单（左侧）
    setupMainMenu();
    mainLayout->addWidget(mainMenuList, 1, 0);

    // 二级菜单（中间）
    subMenuList = new QListWidget();
    subMenuList->setMaximumWidth(240);
    subMenuList->setStyleSheet(
        "QListWidget { "
        "    border: none; "
        "    background: #ffffff; "
        "    padding: 16px 12px; "
        "    border-radius: 0; "
        "    border-right: 1px solid #f1f5f9; "
        "} "
        "QListWidget::item { "
        "    padding: 14px 18px; "
        "    margin: 3px 0; "
        "    border-radius: 10px; "
        "    font-size: 14px; "
        "    color: #64748b; "
        "    background-color: transparent; "
        "    border: 1px solid transparent; "
        "    font-weight: 500; "
        "} "
        "QListWidget::item:hover { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #f8fafc, stop:1 #f1f5f9); "
        "    color: #475569; "
        "    border: 1px solid #e2e8f0; "
        "} "
        "QListWidget::item:selected { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #eff6ff, stop:1 #dbeafe); "
        "    color: #1e40af; "
        "    font-weight: 600; "
        "    border: 1px solid #bfdbfe; "
        "}"
    );
    connect(subMenuList, &QListWidget::itemClicked, this, &MainUIWindow::onSubMenuClicked);
    mainLayout->addWidget(subMenuList, 1, 1);

    // 内容区域（右侧）
    setupContent();
    mainLayout->addWidget(contentStack, 1, 2);

    // 状态栏容器（现代简约设计）
    QWidget *statusBarWidget = new QWidget();
    statusBarWidget->setFixedHeight(36);
    statusBarWidget->setStyleSheet(
        "QWidget { "
        "    background: #f8fafc; "
        "    border-top: 1px solid #e2e8f0; "
        "}"
    );
    
    QHBoxLayout *statusBarLayout = new QHBoxLayout(statusBarWidget);
    statusBarLayout->setContentsMargins(20, 6, 20, 6);
    statusBarLayout->setSpacing(16);
    
    // 状态指示器（现代设计）
    statusIndicator = new QLabel();
    statusIndicator->setFixedSize(10, 10);
    statusIndicator->setStyleSheet(
        "QLabel { "
        "    background-color: #10b981; "
        "    border-radius: 5px; "
        "}"
    );
    
    // 状态文本
    statusText = new QLabel("在线");
    statusText->setStyleSheet(
        "QLabel { "
        "    font-size: 12px; "
        "    color: #64748b; "
        "    font-weight: 500; "
        "}"
    );
    
    // 分隔线（现代设计）
    QFrame *statusLine1 = new QFrame();
    statusLine1->setFrameShape(QFrame::VLine);
    statusLine1->setStyleSheet("QFrame { background-color: #cbd5e1; }");
    
    // 网络状态
    networkStatus = new QLabel("网络: 已连接");
    networkStatus->setStyleSheet(
        "QLabel { "
        "    font-size: 12px; "
        "    color: #64748b; "
        "}"
    );
    
    // 分隔线
    QFrame *statusLine2 = new QFrame();
    statusLine2->setFrameShape(QFrame::VLine);
    statusLine2->setStyleSheet("QFrame { background-color: #cbd5e1; }");
    
    // 时间显示
    timeLabel = new QLabel();
    timeLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 12px; "
        "    color: #64748b; "
        "    font-family: 'SF Mono', 'Monaco', 'Menlo', monospace; "
        "    font-weight: 500; "
        "}"
    );
    
    // 更新时间的定时器
    QTimer *timeTimer = new QTimer(this);
    connect(timeTimer, &QTimer::timeout, [this]() {
        QDateTime currentTime = QDateTime::currentDateTime();
        timeLabel->setText(currentTime.toString("HH:mm:ss"));
    });
    timeTimer->start(1000);
    
    // 状态信息
    statusMessage = new QLabel("就绪");
    statusMessage->setStyleSheet(
        "QLabel { "
        "    font-size: 12px; "
        "    color: #64748b; "
        "}"
    );
    
    statusBarLayout->addWidget(statusIndicator);
    statusBarLayout->addWidget(statusText);
    statusBarLayout->addWidget(statusLine1);
    statusBarLayout->addWidget(networkStatus);
    statusBarLayout->addWidget(statusLine2);
    statusBarLayout->addWidget(timeLabel);
    statusBarLayout->addStretch();
    statusBarLayout->addWidget(statusMessage);
    
    mainLayout->addWidget(statusBarWidget, 2, 0, 1, 3);

    // 设置布局间距
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 1);
    mainLayout->setColumnStretch(2, 5);
    mainLayout->setRowStretch(1, 1);
    
    // 初始化二级菜单（在contentStack初始化之后）
    setupSubMenu(mainMenuList->currentItem()->text());
}

void MainUIWindow::setupMainMenu()
{
    mainMenuList = new QListWidget();
    mainMenuList->setMaximumWidth(200);
    mainMenuList->setStyleSheet(
        "QListWidget { "
        "    border: none; "
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "    stop:0 #f8fafc, stop:1 #e2e8f0); "
        "    padding: 16px 8px; "
        "    border-right: 1px solid #e2e8f0; "
        "} "
        "QListWidget::item { "
        "    padding: 16px 20px; "
        "    margin: 2px 0; "
        "    color: #475569; "
        "    border-radius: 12px; "
        "    font-size: 15px; "
        "    font-weight: 500; "
        "    background-color: transparent; "
        "    border: 1px solid transparent; "
        "} "
        "QListWidget::item:hover { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #f1f5f9, stop:1 #e2e8f0); "
        "    color: #334155; "
        "    border: 1px solid #cbd5e1; "
        "    transform: translateY(-1px); "
        "} "
        "QListWidget::item:selected { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #3b82f6, stop:1 #1d4ed8); "
        "    color: white; "
        "    font-weight: 600; "
        "    border: 1px solid #2563eb; "
        "    box-shadow: 0 4px 12px rgba(59, 130, 246, 0.3); "
        "}"
    );

    // 添加一级菜单项（使用现代图标字符）
    QListWidgetItem *item1 = new QListWidgetItem("⚙️  控件示例");
    item1->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item1);

    QListWidgetItem *item2 = new QListWidgetItem("📐  布局示例");
    item2->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item2);

    QListWidgetItem *item3 = new QListWidgetItem("💬  对话框示例");
    item3->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item3);

    QListWidgetItem *item4 = new QListWidgetItem("📊  图表示例");
    item4->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item4);

    QListWidgetItem *item5 = new QListWidgetItem("📈  数据分析");
    item5->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item5);

    QListWidgetItem *item6 = new QListWidgetItem("👤  个人中心");
    item6->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item6);

    connect(mainMenuList, &QListWidget::itemClicked, this, &MainUIWindow::onMainMenuClicked);

    // 默认选择第一个一级菜单，但不立即触发二级菜单设置
    mainMenuList->setCurrentRow(0);
    // 注意：setupSubMenu将在setupUI函数末尾调用，确保contentStack已初始化
}

void MainUIWindow::setupSubMenu(const QString &mainMenu)
{
    subMenuList->clear();

    if (mainMenu.contains("控件示例")) {
        new QListWidgetItem("🔧  基本控件", subMenuList);
        new QListWidgetItem("⚡  高级控件", subMenuList);
        new QListWidgetItem("📋  数据显示", subMenuList);
    } else if (mainMenu.contains("布局示例")) {
        new QListWidgetItem("📐  布局示例1", subMenuList);
        new QListWidgetItem("🎯  布局示例2", subMenuList);
        new QListWidgetItem("🎨  布局示例3", subMenuList);
    } else if (mainMenu.contains("对话框示例")) {
        new QListWidgetItem("💬  对话框", subMenuList);
    } else if (mainMenu.contains("图表示例")) {
        new QListWidgetItem("📊  ECharts示例", subMenuList);
        new QListWidgetItem("📈  日志统计", subMenuList);
    } else if (mainMenu.contains("数据分析")) {
        new QListWidgetItem("👤  用户画像", subMenuList);
        new QListWidgetItem("📊  统计报表", subMenuList);
    } else if (mainMenu.contains("个人中心")) {
        new QListWidgetItem("ℹ️  用户信息", subMenuList);
        new QListWidgetItem("🔒  修改密码", subMenuList);
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
    
    setupSubMenu(mainMenu);
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
    if (subMenu.contains("基本控件")) {
        contentWidget = new BasicControlsTab();
    } else if (subMenu.contains("高级控件")) {
        contentWidget = new AdvancedControlsTab();
    } else if (subMenu.contains("数据显示")) {
        contentWidget = new DataDisplayTab();
    } 
    else if (subMenu.contains("布局示例1")) {
        contentWidget = new LayoutExamplesTab();
    } 
    else if (subMenu.contains("布局示例2")) {
        contentWidget = new LayoutExamplesTab2();
    } 
    else if (subMenu.contains("布局示例3")) {
        contentWidget = new LayoutExamplesTab3();
    } else if (subMenu.contains("对话框")) {
        contentWidget = new DialogsTab();
    } else if (subMenu.contains("ECharts示例")) {
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
    } else if (subMenu.contains("用户信息")) {
        contentWidget = new UserInfoPage();
    } else if (subMenu.contains("修改密码")) {
        contentWidget = new ChangePasswordPage();
    }

    // 如果创建了内容控件，添加到内容区域
    if (contentWidget) {
        // 将内容控件放在滚动区域中
        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setWidget(contentWidget);
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setStyleSheet(
            "QScrollArea { "
            "    border: none; "
            "    background-color: #ffffff; "
            "}"
            "QScrollBar:vertical { "
            "    background: #f8fafc; "
            "    width: 8px; "
            "    border-radius: 4px; "
            "    margin: 0; "
            "}"
            "QScrollBar::handle:vertical { "
            "    background: #cbd5e1; "
            "    border-radius: 4px; "
            "    min-height: 20px; "
            "}"
            "QScrollBar::handle:vertical:hover { "
            "    background: #94a3b8; "
            "}"
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { "
            "    height: 0px; "
            "}"
            "QScrollBar:horizontal { "
            "    background: #f8fafc; "
            "    height: 8px; "
            "    border-radius: 4px; "
            "    margin: 0; "
            "}"
            "QScrollBar::handle:horizontal { "
            "    background: #cbd5e1; "
            "    border-radius: 4px; "
            "    min-width: 20px; "
            "}"
            "QScrollBar::handle:horizontal:hover { "
            "    background: #94a3b8; "
            "}"
            "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { "
            "    width: 0px; "
            "}"
        );

        contentStack->addWidget(scrollArea);
        contentStack->setCurrentWidget(scrollArea);
        
        // 如果是用户信息页面，连接头像更新信号
        if (subMenu.contains("用户信息")) {
            UserInfoPage *userInfoPage = qobject_cast<UserInfoPage*>(contentWidget);
            if (userInfoPage) {
                connect(userInfoPage, &UserInfoPage::avatarUpdated, 
                        this, &MainUIWindow::updateUserInfo);
            }
        }
    }
}

void MainUIWindow::onLoginSuccess(const QString &token)
{
    Q_UNUSED(token);
    
    // 安全检查：确保所有UI元素都已初始化
    if (!mainStack || !statusIndicator || !statusText || !statusMessage) {
        qWarning() << "UI elements not initialized in onLoginSuccess!";
        return;
    }
    
    // 登录成功，切换到主界面
    mainStack->setCurrentIndex(1);
    
    // 更新状态栏
    statusIndicator->setStyleSheet(
        "QLabel { "
        "    background-color: #10b981; "
        "    border-radius: 5px; "
        "}"
    );
    statusText->setText("在线");
    statusMessage->setText("登录成功");
    
    // 延迟100ms后更新用户信息显示，确保QSettings完全同步
    QTimer::singleShot(100, this, [this]() {
        updateUserInfo();
    });
}

void MainUIWindow::onLogoutClicked()
{
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

void MainUIWindow::updateUserInfo()
{
    // 安全检查：确保所有UI元素都已初始化
    if (!usernameLabel || !avatarLabel) {
        qWarning() << "UI elements not initialized in updateUserInfo!";
        return;
    }
    
    // 从设置中获取用户信息
    QSettings settings("YourCompany", "QtApp");
    QString username = settings.value("user/username", "").toString();
    QString avatar = settings.value("user/avatar", "").toString();
    
    // 更新用户信息
    
    // 更新用户名
    if (!username.isEmpty()) {
        usernameLabel->setText(username);
    } else {
        usernameLabel->setText("未知用户");
    }
    
    // 加载头像
    if (!avatar.isEmpty()) {
        QNetworkAccessManager *networkMgr = new QNetworkAccessManager(this);
        QNetworkRequest request{QUrl(avatar)};
        QNetworkReply *reply = networkMgr->get(request);
        
        connect(reply, &QNetworkReply::finished, [this, reply]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray imageData = reply->readAll();
                QPixmap pixmap;
                if (pixmap.loadFromData(imageData)) {
                    // 创建圆形头像（52px大小以匹配头像标签尺寸）
                    QPixmap circularPixmap = createCircularPixmap(pixmap, 52);
                    avatarLabel->setPixmap(circularPixmap);
                }
            }
            reply->deleteLater();
        });
    }
}

QPixmap MainUIWindow::createCircularPixmap(const QPixmap &pixmap, int size)
{
    // 创建指定大小的圆形图片
    QPixmap circularPixmap(size, size);
    circularPixmap.fill(Qt::transparent);
    
    // 缩放原始图片以适应圆形（忽略宽高比，确保大小完全匹配）
    QPixmap scaledPixmap = pixmap.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    
    // 创建圆形遮罩
    QBitmap mask(size, size);
    mask.fill(Qt::transparent);
    
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, size, size);
    painter.end();
    
    // 应用遮罩
    scaledPixmap.setMask(mask);
    
    // 将遮罩后的图片绘制到透明背景上
    QPainter finalPainter(&circularPixmap);
    finalPainter.setRenderHint(QPainter::Antialiasing);
    finalPainter.drawPixmap(0, 0, scaledPixmap);
    finalPainter.end();
    
    return circularPixmap;
}
