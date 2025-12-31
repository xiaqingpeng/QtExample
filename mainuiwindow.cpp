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
    
    // 创建标题栏容器（带渐变背景）
    QWidget *titleBarWidget = new QWidget();
    titleBarWidget->setFixedHeight(80);
    titleBarWidget->setStyleSheet(
        "QWidget { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #667eea, stop:1 #764ba2); "
        "}"
    );
    
    QHBoxLayout *titleBarLayout = new QHBoxLayout(titleBarWidget);
    titleBarLayout->setContentsMargins(20, 10, 20, 10);
    titleBarLayout->setSpacing(15);
    
    // 用户头像容器（带渐变边框和阴影）
    QWidget *avatarContainer = new QWidget();
    avatarContainer->setFixedSize(64, 64);
    avatarContainer->setStyleSheet(
        "QWidget { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 #667eea, stop:1 #764ba2); "
        "    border-radius: 32px; "
        "}"
    );
    
    QVBoxLayout *avatarLayout = new QVBoxLayout(avatarContainer);
    avatarLayout->setContentsMargins(3, 3, 3, 3);
    
    // 用户头像
    avatarLabel = new QLabel();
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setFixedSize(58, 58);
    // QQ风格头像：白色边框
    avatarLabel->setStyleSheet(
        "QLabel { "
        "    background-color: white; "
        "    border-radius: 29px; "
        "    border: 4px solid white; "
        "}"
    );
    // 添加阴影效果
    QGraphicsDropShadowEffect *avatarShadow = new QGraphicsDropShadowEffect(this);
    avatarShadow->setBlurRadius(10);
    avatarShadow->setColor(QColor(0, 0, 0, 60));
    avatarShadow->setOffset(0, 2);
    avatarLabel->setGraphicsEffect(avatarShadow);
    
    // QQ风格在线状态指示器
    QLabel *onlineIndicator = new QLabel();
    onlineIndicator->setFixedSize(16, 16);
    onlineIndicator->setStyleSheet(
        "QLabel { "
        "    background-color: #12B7F5; "
        "    border-radius: 8px; "
        "    border: 2px solid white; "
        "}"
    );
    // 添加阴影效果
    QGraphicsDropShadowEffect *indicatorShadow = new QGraphicsDropShadowEffect(this);
    indicatorShadow->setBlurRadius(6);
    indicatorShadow->setColor(QColor(0, 0, 0, 50));
    indicatorShadow->setOffset(0, 1);
    onlineIndicator->setGraphicsEffect(indicatorShadow);
    
    // 使用绝对定位将状态指示器放在头像右下角
    onlineIndicator->setParent(avatarContainer);
    onlineIndicator->move(48, 48);
    
    // 设置默认头像（使用更美观的渐变）
    QPixmap defaultAvatar(50, 50);
    defaultAvatar.fill(Qt::transparent);
    QPainter painter(&defaultAvatar);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // 创建圆形渐变背景
    QRadialGradient gradient(25, 25, 25);
    gradient.setColorAt(0, QColor("#f8f9fa"));
    gradient.setColorAt(1, QColor("#e9ecef"));
    painter.setBrush(QBrush(gradient));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, 50, 50);
    
    // 绘制用户图标
    painter.setBrush(QBrush(QColor("#adb5bd")));
    painter.drawEllipse(15, 13, 20, 20); // 头部
    painter.drawEllipse(7, 36, 36, 20); // 身体
    
    avatarLabel->setPixmap(defaultAvatar);
    
    avatarLayout->addWidget(avatarLabel, 0, Qt::AlignCenter);
    
    // 用户名
    usernameLabel = new QLabel("未登录");
    usernameLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 18px; "
        "    font-weight: bold; "
        "    color: white; "
        "    padding: 5px 10px; "
        "}"
    );
    
    titleBarLayout->addWidget(avatarContainer);
    titleBarLayout->addWidget(usernameLabel);
    titleBarLayout->addStretch();
    
    // 登出按钮
    logoutButton = new QPushButton("登出");
    logoutButton->setStyleSheet(
        "QPushButton { "
        "    background-color: rgba(255,255,255,0.2); "
        "    color: white; "
        "    border: 2px solid rgba(255,255,255,0.5); "
        "    padding: 8px 20px; "
        "    border-radius: 20px; "
        "    font-size: 14px; "
        "    font-weight: bold; "
        "}"
        "QPushButton:hover { "
        "    background-color: rgba(255,255,255,0.3); "
        "    border-color: rgba(255,255,255,0.8); "
        "}"
        "QPushButton:pressed { "
        "    background-color: rgba(255,255,255,0.4); "
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
    subMenuList->setMaximumWidth(220);
    subMenuList->setStyleSheet(
        "QListWidget { "
        "    border: none; "
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "    stop:0 #fdfbfb, stop:1 #ebedee); "
        "    padding: 10px; "
        "    border-radius: 12px; "
        "} "
        "QListWidget::item { "
        "    padding: 14px 18px; "
        "    margin: 4px 0; "
        "    border-radius: 10px; "
        "    font-size: 13px; "
        "    color: #495057; "
        "    background-color: rgba(255, 255, 255, 0.6); "
        "    border: 1px solid rgba(0, 0, 0, 0.05); "
        "} "
        "QListWidget::item:hover { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #a8edea, stop:1 #fed6e3); "
        "    color: #2d3436; "
        "    border-color: rgba(168, 237, 234, 0.3); "
        "} "
        "QListWidget::item:selected { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #667eea, stop:1 #764ba2); "
        "    color: white; "
        "    font-weight: bold; "
        "    border-color: transparent; "
        "}"
    );
    connect(subMenuList, &QListWidget::itemClicked, this, &MainUIWindow::onSubMenuClicked);
    mainLayout->addWidget(subMenuList, 1, 1);

    // 内容区域（右侧）
    setupContent();
    mainLayout->addWidget(contentStack, 1, 2);

    // 状态栏容器（横跨3列）
    QWidget *statusBarWidget = new QWidget();
    statusBarWidget->setFixedHeight(40);
    statusBarWidget->setStyleSheet(
        "QWidget { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #f8f9fa, stop:1 #e9ecef); "
        "    border-top: 1px solid #dee2e6; "
        "}"
    );
    
    QHBoxLayout *statusBarLayout = new QHBoxLayout(statusBarWidget);
    statusBarLayout->setContentsMargins(15, 5, 15, 5);
    statusBarLayout->setSpacing(20);
    
    // 状态指示器
    statusIndicator = new QLabel();
    statusIndicator->setFixedSize(12, 12);
    statusIndicator->setStyleSheet(
        "QLabel { "
        "    background-color: #28a745; "
        "    border-radius: 6px; "
        "}"
    );
    
    // 状态文本
    statusText = new QLabel("在线");
    statusText->setStyleSheet(
        "QLabel { "
        "    font-size: 12px; "
        "    color: #495057; "
        "    font-weight: 500; "
        "}"
    );
    
    // 分隔线
    QFrame *statusLine1 = new QFrame();
    statusLine1->setFrameShape(QFrame::VLine);
    statusLine1->setStyleSheet("QFrame { background-color: #dee2e6; }");
    
    // 网络状态
    networkStatus = new QLabel("网络: 已连接");
    networkStatus->setStyleSheet(
        "QLabel { "
        "    font-size: 12px; "
        "    color: #6c757d; "
        "}"
    );
    
    // 分隔线
    QFrame *statusLine2 = new QFrame();
    statusLine2->setFrameShape(QFrame::VLine);
    statusLine2->setStyleSheet("QFrame { background-color: #dee2e6; }");
    
    // 时间显示
    timeLabel = new QLabel();
    timeLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 12px; "
        "    color: #6c757d; "
        "    font-family: 'Monaco', 'Menlo', monospace; "
        "}"
    );
    
    // 更新时间的定时器
    QTimer *timeTimer = new QTimer(this);
    connect(timeTimer, &QTimer::timeout, [this]() {
        QDateTime currentTime = QDateTime::currentDateTime();
        timeLabel->setText(currentTime.toString("HH:mm:ss"));
    });
    timeTimer->start(1000);
    
    // 弹性空间
    statusBarLayout->addStretch();
    
    // 状态信息
    statusMessage = new QLabel("就绪");
    statusMessage->setStyleSheet(
        "QLabel { "
        "    font-size: 12px; "
        "    color: #6c757d; "
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
    mainMenuList->setMaximumWidth(180);
    mainMenuList->setStyleSheet(
        "QListWidget { "
        "    border: none; "
        "    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "    stop:0 #667eea, stop:0.5 #764ba2, stop:1 #f093fb); "
        "    padding: 8px; "
        "    border-right: 1px solid rgba(255, 255, 255, 0.1); "
        "} "
        "QListWidget::item { "
        "    padding: 14px 18px; "
        "    margin: 4px 0; "
        "    color: #ffffff; "
        "    border-radius: 10px; "
        "    font-size: 14px; "
        "    font-weight: 500; "
        "    background-color: rgba(255, 255, 255, 0.05); "
        "} "
        "QListWidget::item:hover { "
        "    background-color: rgba(255, 255, 255, 0.2); "
        "    color: #ffffff; "
        "} "
        "QListWidget::item:selected { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #00c6fb, stop:1 #005bea); "
        "    color: white; "
        "    font-weight: bold; "
        "    box-shadow: 0 4px 15px rgba(0, 91, 234, 0.3); "
        "}"
    );

    // 添加一级菜单（移除emoji图标以避免渲染崩溃）
    QListWidgetItem *item1 = new QListWidgetItem("控件示例");
    item1->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item1);

    QListWidgetItem *item2 = new QListWidgetItem("布局示例");
    item2->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item2);

    QListWidgetItem *item3 = new QListWidgetItem("对话框示例");
    item3->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item3);

    QListWidgetItem *item4 = new QListWidgetItem("图表示例");
    item4->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item4);

    QListWidgetItem *item5 = new QListWidgetItem("数据分析");
    item5->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainMenuList->addItem(item5);

    QListWidgetItem *item6 = new QListWidgetItem("个人中心");
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

    if (mainMenu == "控件示例") {
        new QListWidgetItem("基本控件", subMenuList);
        new QListWidgetItem("高级控件", subMenuList);
        new QListWidgetItem("数据显示", subMenuList);
    } else if (mainMenu == "布局示例") {
        new QListWidgetItem("布局示例1", subMenuList);
        new QListWidgetItem("布局示例2", subMenuList);
        new QListWidgetItem("布局示例3", subMenuList);

    } else if (mainMenu == "对话框示例") {
        new QListWidgetItem("对话框", subMenuList);
    } else if (mainMenu == "图表示例") {
        new QListWidgetItem("ECharts示例", subMenuList);
        new QListWidgetItem("日志统计", subMenuList);
    } else if (mainMenu == "数据分析") {
        new QListWidgetItem("用户画像", subMenuList);
        new QListWidgetItem("统计报表", subMenuList);
    } else if (mainMenu == "个人中心") {
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
    contentStack->setStyleSheet("QStackedWidget { background-color: white; border: none; }");
}

void MainUIWindow::onMainMenuClicked(QListWidgetItem *item)
{
    QString mainMenu = item->text();
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
            "    background-color: white; "
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
        "    background-color: #28a745; "
        "    border-radius: 6px; "
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
    
    qDebug() << "User logged out, user info cleared";
    
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
        "    background-color: #dc3545; "
        "    border-radius: 6px; "
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
    
    qDebug() << "=== Updating User Info in Main Window ===";
    qDebug() << "Username:" << username;
    qDebug() << "Avatar:" << avatar;
    
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
                    // 创建圆形头像（58px大小以匹配头像标签尺寸）
                    QPixmap circularPixmap = createCircularPixmap(pixmap, 58);
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
