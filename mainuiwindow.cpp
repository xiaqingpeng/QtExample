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
    
    // 用户头像容器（带渐变边框）
    QWidget *avatarContainer = new QWidget();
    avatarContainer->setFixedSize(60, 60);
    avatarContainer->setStyleSheet(
        "QWidget { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:1, "
        "    stop:0 rgba(255,255,255,0.3), stop:1 rgba(255,255,255,0.1)); "
        "    border-radius: 30px; "
        "}"
    );
    
    QVBoxLayout *avatarLayout = new QVBoxLayout(avatarContainer);
    avatarLayout->setContentsMargins(0, 0, 0, 0);
    
    // 用户头像
    avatarLabel = new QLabel();
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setFixedSize(54, 54);
    avatarLabel->setStyleSheet(
        "QLabel { "
        "    background-color: white; "
        "    border-radius: 27px; "
        "    border: 3px solid rgba(255,255,255,0.4); "
        "}"
    );
    
    // 设置默认头像
    QPixmap defaultAvatar(54, 54);
    defaultAvatar.fill(QColor("#e9ecef"));
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
    subMenuList->setMaximumWidth(200);
    subMenuList->setStyleSheet(
        "QListWidget { border: none; background-color: #f8f9fa; }"
        "QListWidget::item { padding: 10px; border-bottom: 1px solid #e9ecef; }"
        "QListWidget::item:hover { background-color: #e3f2fd; }"
        "QListWidget::item:selected { background-color: #2196f3; color: white; }"
        );
    connect(subMenuList, &QListWidget::itemClicked, this, &MainUIWindow::onSubMenuClicked);
    mainLayout->addWidget(subMenuList, 1, 1);

    // 内容区域（右侧）
    setupContent();
    mainLayout->addWidget(contentStack, 1, 2);

    // 状态栏（横跨3列）
    statusBar = new QStatusBar();
    statusBar->showMessage("就绪 | Qt版本: " + QString(qVersion()));
    mainLayout->addWidget(statusBar, 2, 0, 1, 3);

    // 设置布局间距
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 1);
    mainLayout->setColumnStretch(2, 5);
    mainLayout->setRowStretch(1, 1);
}

void MainUIWindow::setupMainMenu()
{
    mainMenuList = new QListWidget();
    mainMenuList->setMaximumWidth(150);
    mainMenuList->setStyleSheet(
        "QListWidget { border: none; background-color: #343a40; }"
        "QListWidget::item { padding: 15px; color: white; border-bottom: 1px solid #495057; }"
        "QListWidget::item:hover { background-color: #495057; }"
        "QListWidget::item:selected { background-color: #007bff; }"
        );

    // 添加一级菜单
    new QListWidgetItem("控件示例", mainMenuList);
    new QListWidgetItem("布局示例", mainMenuList);
    new QListWidgetItem("对话框示例", mainMenuList);
    new QListWidgetItem("图表示例", mainMenuList);
    new QListWidgetItem("个人中心", mainMenuList);

    connect(mainMenuList, &QListWidget::itemClicked, this, &MainUIWindow::onMainMenuClicked);

    // 默认选择第一个一级菜单
    mainMenuList->setCurrentRow(0);
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
    QString subMenu = item->text();

    // 清空内容区域
    while (contentStack->count() > 0) {
        QWidget *widget = contentStack->widget(0);
        contentStack->removeWidget(widget);
        widget->deleteLater();
    }

    // 根据二级菜单创建对应的内容
    QWidget *contentWidget = nullptr;
    if (subMenu == "基本控件") {
        contentWidget = new BasicControlsTab();
    } else if (subMenu == "高级控件") {
        contentWidget = new AdvancedControlsTab();
    } else if (subMenu == "数据显示") {
        contentWidget = new DataDisplayTab();
    } 
    
    else if (subMenu == "布局示例1") {
        contentWidget = new LayoutExamplesTab();
    } 

    else if (subMenu == "布局示例2") {
        contentWidget = new LayoutExamplesTab2();
    } 

    else if (subMenu == "布局示例3") {
        contentWidget = new LayoutExamplesTab3();
    } else if (subMenu == "对话框") {
        contentWidget = new DialogsTab();
    } else if (subMenu == "ECharts示例") {
        contentWidget = new EChartsTab();
    } else if (subMenu == "日志统计") {
        contentWidget = new LogStatsTab();
    } else if (subMenu == "用户信息") {
        contentWidget = new UserInfoPage();
    } else if (subMenu == "修改密码") {
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

        contentStack->addWidget(scrollArea);
        contentStack->setCurrentWidget(scrollArea);
        
        // 如果是用户信息页面，连接头像更新信号
        if (subMenu == "用户信息") {
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
    // 登录成功，切换到主界面
    mainStack->setCurrentIndex(1);
    statusBar->showMessage("登录成功 | Qt版本:" + QString(qVersion()));
    
    // 延迟100ms后更新用户信息显示，确保QSettings完全同步
    QTimer::singleShot(100, this, [this]() {
        updateUserInfo();
    });
}

void MainUIWindow::onLogoutClicked()
{
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
    
    // 返回登录页面
    mainStack->setCurrentIndex(0);
    statusBar->showMessage("已登出 | Qt版本: " + QString(qVersion()));
}

void MainUIWindow::updateUserInfo()
{
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
                    // 创建圆形头像（54px大小以匹配新的头像标签尺寸）
                    QPixmap circularPixmap = createCircularPixmap(pixmap, 54);
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
    
    // 缩放原始图片以适应圆形
    QPixmap scaledPixmap = pixmap.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    
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
