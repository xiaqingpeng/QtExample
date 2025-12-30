#include "userprofiletab.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <QTimer>

UserProfileTab::UserProfileTab(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void UserProfileTab::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // 工具栏
    toolbarLayout = new QHBoxLayout();
    toolbarLayout->setSpacing(15);

    QLabel *userIdLabel = new QLabel("用户ID:");
    userIdEdit = new QLineEdit();
    userIdEdit->setPlaceholderText("请输入用户ID");
    userIdEdit->setMaximumWidth(200);

    QLabel *startDateLabel = new QLabel("开始日期:");
    startDateEdit = new QDateEdit();
    startDateEdit->setCalendarPopup(true);
    startDateEdit->setDate(QDate::currentDate().addDays(-30));
    startDateEdit->setDisplayFormat("yyyy-MM-dd");

    QLabel *endDateLabel = new QLabel("结束日期:");
    endDateEdit = new QDateEdit();
    endDateEdit->setCalendarPopup(true);
    endDateEdit->setDate(QDate::currentDate());
    endDateEdit->setDisplayFormat("yyyy-MM-dd");

    searchButton = new QPushButton("查询");
    searchButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #667eea; "
        "    color: white; "
        "    border: none; "
        "    padding: 8px 20px; "
        "    border-radius: 5px; "
        "    font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "    background-color: #5568d3; "
        "}"
    );
    connect(searchButton, &QPushButton::clicked, this, &UserProfileTab::onDateRangeChanged);

    refreshButton = new QPushButton("刷新");
    refreshButton->setStyleSheet(
        "QPushButton { "
        "    background-color: #28a745; "
        "    color: white; "
        "    border: none; "
        "    padding: 8px 20px; "
        "    border-radius: 5px; "
        "    font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "    background-color: #218838; "
        "}"
    );
    connect(refreshButton, &QPushButton::clicked, this, &UserProfileTab::onRefreshClicked);

    toolbarLayout->addWidget(userIdLabel);
    toolbarLayout->addWidget(userIdEdit);
    toolbarLayout->addWidget(startDateLabel);
    toolbarLayout->addWidget(startDateEdit);
    toolbarLayout->addWidget(endDateLabel);
    toolbarLayout->addWidget(endDateEdit);
    toolbarLayout->addWidget(searchButton);
    toolbarLayout->addWidget(refreshButton);
    toolbarLayout->addStretch();

    mainLayout->addLayout(toolbarLayout);

    // 滚动区域
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setStyleSheet(
        "QScrollArea { "
        "    border: none; "
        "    background-color: #f8f9fa; "
        "}"
    );

    scrollWidget = new QWidget();
    scrollLayout = new QVBoxLayout(scrollWidget);
    scrollLayout->setSpacing(20);
    scrollLayout->setContentsMargins(10, 10, 10, 10);

    // 设置各个部分
    setupUserInfoCard();
    setupUserTags();
    setupBehaviorStats();
    setupInterestAnalysis();
    setupValueAssessment();

    scrollLayout->addStretch();

    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea);
}

void UserProfileTab::setupUserInfoCard()
{
    userInfoGroup = new QGroupBox("用户基础信息");
    userInfoGroup->setStyleSheet(
        "QGroupBox { "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "    color: #495057; "
        "    border: 2px solid #dee2e6; "
        "    border-radius: 10px; "
        "    margin-top: 10px; "
        "    padding-top: 15px; "
        "} "
        "QGroupBox::title { "
        "    subcontrol-origin: margin; "
        "    left: 20px; "
        "    padding: 0 10px; "
        "}"
    );

    QVBoxLayout *groupLayout = new QVBoxLayout(userInfoGroup);
    groupLayout->setSpacing(15);

    QHBoxLayout *userIdLayout = new QHBoxLayout();
    QLabel *userIdTitle = new QLabel("用户ID:");
    userIdTitle->setStyleSheet("font-weight: bold; color: #6c757d;");
    userIdLabel = new QLabel("-");
    userIdLabel->setStyleSheet("font-size: 14px; color: #212529;");
    userIdLayout->addWidget(userIdTitle);
    userIdLayout->addWidget(userIdLabel);
    userIdLayout->addStretch();

    QHBoxLayout *registerTimeLayout = new QHBoxLayout();
    QLabel *registerTimeTitle = new QLabel("注册时间:");
    registerTimeTitle->setStyleSheet("font-weight: bold; color: #6c757d;");
    registerTimeLabel = new QLabel("-");
    registerTimeLabel->setStyleSheet("font-size: 14px; color: #212529;");
    registerTimeLayout->addWidget(registerTimeTitle);
    registerTimeLayout->addWidget(registerTimeLabel);
    registerTimeLayout->addStretch();

    QHBoxLayout *lastActiveTimeLayout = new QHBoxLayout();
    QLabel *lastActiveTimeTitle = new QLabel("最后活跃时间:");
    lastActiveTimeTitle->setStyleSheet("font-weight: bold; color: #6c757d;");
    lastActiveTimeLabel = new QLabel("-");
    lastActiveTimeLabel->setStyleSheet("font-size: 14px; color: #212529;");
    lastActiveTimeLayout->addWidget(lastActiveTimeTitle);
    lastActiveTimeLayout->addWidget(lastActiveTimeLabel);
    lastActiveTimeLayout->addStretch();

    QHBoxLayout *lifecycleStatusLayout = new QHBoxLayout();
    QLabel *lifecycleStatusTitle = new QLabel("生命周期状态:");
    lifecycleStatusTitle->setStyleSheet("font-weight: bold; color: #6c757d;");
    lifecycleStatusLabel = new QLabel("-");
    lifecycleStatusLabel->setStyleSheet("font-size: 14px; color: #212529;");
    lifecycleStatusLayout->addWidget(lifecycleStatusTitle);
    lifecycleStatusLayout->addWidget(lifecycleStatusLabel);
    lifecycleStatusLayout->addStretch();

    groupLayout->addLayout(userIdLayout);
    groupLayout->addLayout(registerTimeLayout);
    groupLayout->addLayout(lastActiveTimeLayout);
    groupLayout->addLayout(lifecycleStatusLayout);

    scrollLayout->addWidget(userInfoGroup);
}

void UserProfileTab::setupUserTags()
{
    tagsGroup = new QGroupBox("用户标签");
    tagsGroup->setStyleSheet(
        "QGroupBox { "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "    color: #495057; "
        "    border: 2px solid #dee2e6; "
        "    border-radius: 10px; "
        "    margin-top: 10px; "
        "    padding-top: 15px; "
        "} "
        "QGroupBox::title { "
        "    subcontrol-origin: margin; "
        "    left: 20px; "
        "    padding: 0 10px; "
        "}"
    );

    QVBoxLayout *groupLayout = new QVBoxLayout(tagsGroup);
    groupLayout->setSpacing(15);

    QHBoxLayout *activityLayout = new QHBoxLayout();
    QLabel *activityTitle = new QLabel("活跃度:");
    activityTitle->setStyleSheet("font-weight: bold; color: #6c757d;");
    activityTagLabel = new QLabel("-");
    activityTagLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 14px; "
        "    padding: 5px 15px; "
        "    border-radius: 15px; "
        "    background-color: #e3f2fd; "
        "    color: #1976d2; "
        "    font-weight: bold; "
        "}"
    );
    activityLayout->addWidget(activityTitle);
    activityLayout->addWidget(activityTagLabel);
    activityLayout->addStretch();

    QHBoxLayout *loyaltyLayout = new QHBoxLayout();
    QLabel *loyaltyTitle = new QLabel("忠诚度:");
    loyaltyTitle->setStyleSheet("font-weight: bold; color: #6c757d;");
    loyaltyTagLabel = new QLabel("-");
    loyaltyTagLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 14px; "
        "    padding: 5px 15px; "
        "    border-radius: 15px; "
        "    background-color: #e8f5e9; "
        "    color: #388e3c; "
        "    font-weight: bold; "
        "}"
    );
    loyaltyLayout->addWidget(loyaltyTitle);
    loyaltyLayout->addWidget(loyaltyTagLabel);
    loyaltyLayout->addStretch();

    QHBoxLayout *valueLayout = new QHBoxLayout();
    QLabel *valueTitle = new QLabel("价值:");
    valueTitle->setStyleSheet("font-weight: bold; color: #6c757d;");
    valueTagLabel = new QLabel("-");
    valueTagLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 14px; "
        "    padding: 5px 15px; "
        "    border-radius: 15px; "
        "    background-color: #fff3e0; "
        "    color: #f57c00; "
        "    font-weight: bold; "
        "}"
    );
    valueLayout->addWidget(valueTitle);
    valueLayout->addWidget(valueTagLabel);
    valueLayout->addStretch();

    groupLayout->addLayout(activityLayout);
    groupLayout->addLayout(loyaltyLayout);
    groupLayout->addLayout(valueLayout);

    scrollLayout->addWidget(tagsGroup);
}

void UserProfileTab::setupBehaviorStats()
{
    behaviorGroup = new QGroupBox("用户行为统计");
    behaviorGroup->setStyleSheet(
        "QGroupBox { "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "    color: #495057; "
        "    border: 2px solid #dee2e6; "
        "    border-radius: 10px; "
        "    margin-top: 10px; "
        "    padding-top: 15px; "
        "} "
        "QGroupBox::title { "
        "    subcontrol-origin: margin; "
        "    left: 20px; "
        "    padding: 0 10px; "
        "}"
    );

    QVBoxLayout *groupLayout = new QVBoxLayout(behaviorGroup);
    groupLayout->setSpacing(15);

    QHBoxLayout *visitLayout = new QHBoxLayout();
    QLabel *visitTitle = new QLabel("访问次数:");
    visitTitle->setStyleSheet("font-weight: bold; color: #6c757d;");
    visitCountLabel = new QLabel("-");
    visitCountLabel->setStyleSheet("font-size: 14px; color: #212529; font-weight: bold;");
    visitLayout->addWidget(visitTitle);
    visitLayout->addWidget(visitCountLabel);
    visitLayout->addStretch();

    QHBoxLayout *sessionLayout = new QHBoxLayout();
    QLabel *sessionTitle = new QLabel("平均会话时长:");
    sessionTitle->setStyleSheet("font-weight: bold; color: #6c757d;");
    avgSessionLabel = new QLabel("-");
    avgSessionLabel->setStyleSheet("font-size: 14px; color: #212529; font-weight: bold;");
    sessionLayout->addWidget(sessionTitle);
    sessionLayout->addWidget(avgSessionLabel);
    sessionLayout->addStretch();

    groupLayout->addLayout(visitLayout);
    groupLayout->addLayout(sessionLayout);

    // 常用页面TOP10
    QLabel *topPagesTitle = new QLabel("常用页面 TOP10");
    topPagesTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #495057;");
    groupLayout->addWidget(topPagesTitle);

    topPagesTable = new QTableWidget();
    topPagesTable->setColumnCount(3);
    topPagesTable->setHorizontalHeaderLabels({"排名", "页面名称", "访问次数"});
    topPagesTable->horizontalHeader()->setStretchLastSection(true);
    topPagesTable->verticalHeader()->setVisible(false);
    topPagesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    topPagesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    topPagesTable->setMaximumHeight(200);
    topPagesTable->setStyleSheet(
        "QTableWidget { "
        "    border: 1px solid #dee2e6; "
        "    border-radius: 5px; "
        "    background-color: white; "
        "} "
        "QTableWidget::item { "
        "    padding: 8px; "
        "} "
        "QHeaderView::section { "
        "    background-color: #f8f9fa; "
        "    color: #495057; "
        "    font-weight: bold; "
        "    border: none; "
        "    padding: 8px; "
        "}"
    );
    groupLayout->addWidget(topPagesTable);

    // 常用功能TOP5
    QLabel *topFeaturesTitle = new QLabel("常用功能 TOP5");
    topFeaturesTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #495057;");
    groupLayout->addWidget(topFeaturesTitle);

    topFeaturesTable = new QTableWidget();
    topFeaturesTable->setColumnCount(3);
    topFeaturesTable->setHorizontalHeaderLabels({"排名", "功能名称", "使用次数"});
    topFeaturesTable->horizontalHeader()->setStretchLastSection(true);
    topFeaturesTable->verticalHeader()->setVisible(false);
    topFeaturesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    topFeaturesTable->setSelectionMode(QAbstractItemView::SingleSelection);
    topFeaturesTable->setMaximumHeight(150);
    topFeaturesTable->setStyleSheet(
        "QTableWidget { "
        "    border: 1px solid #dee2e6; "
        "    border-radius: 5px; "
        "    background-color: white; "
        "} "
        "QTableWidget::item { "
        "    padding: 8px; "
        "} "
        "QHeaderView::section { "
        "    background-color: #f8f9fa; "
        "    color: #495057; "
        "    font-weight: bold; "
        "    border: none; "
        "    padding: 8px; "
        "}"
    );
    groupLayout->addWidget(topFeaturesTable);

    scrollLayout->addWidget(behaviorGroup);
}

void UserProfileTab::setupInterestAnalysis()
{
    interestGroup = new QGroupBox("用户兴趣分析");
    interestGroup->setStyleSheet(
        "QGroupBox { "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "    color: #495057; "
        "    border: 2px solid #dee2e6; "
        "    border-radius: 10px; "
        "    margin-top: 10px; "
        "    padding-top: 15px; "
        "} "
        "QGroupBox::title { "
        "    subcontrol-origin: margin; "
        "    left: 20px; "
        "    padding: 0 10px; "
        "}"
    );

    QVBoxLayout *groupLayout = new QVBoxLayout(interestGroup);
    groupLayout->setSpacing(15);

    QLabel *pageInterestTitle = new QLabel("页面兴趣分布:");
    pageInterestTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #6c757d;");
    pageInterestLabel = new QLabel("暂无数据");
    pageInterestLabel->setStyleSheet("font-size: 14px; color: #212529;");
    pageInterestLabel->setWordWrap(true);
    groupLayout->addWidget(pageInterestTitle);
    groupLayout->addWidget(pageInterestLabel);

    QLabel *featurePreferenceTitle = new QLabel("功能偏好:");
    featurePreferenceTitle->setStyleSheet("font-size: 14px; font-weight: bold; color: #6c757d;");
    featurePreferenceLabel = new QLabel("暂无数据");
    featurePreferenceLabel->setStyleSheet("font-size: 14px; color: #212529;");
    featurePreferenceLabel->setWordWrap(true);
    groupLayout->addWidget(featurePreferenceTitle);
    groupLayout->addWidget(featurePreferenceLabel);

    scrollLayout->addWidget(interestGroup);
}

void UserProfileTab::setupValueAssessment()
{
    valueGroup = new QGroupBox("用户价值评估");
    valueGroup->setStyleSheet(
        "QGroupBox { "
        "    font-size: 16px; "
        "    font-weight: bold; "
        "    color: #495057; "
        "    border: 2px solid #dee2e6; "
        "    border-radius: 10px; "
        "    margin-top: 10px; "
        "    padding-top: 15px; "
        "} "
        "QGroupBox::title { "
        "    subcontrol-origin: margin; "
        "    left: 20px; "
        "    padding: 0 10px; "
        "}"
    );

    QVBoxLayout *groupLayout = new QVBoxLayout(valueGroup);
    groupLayout->setSpacing(15);

    // 活跃度评分
    QHBoxLayout *activityLayout = new QHBoxLayout();
    QLabel *activityTitle = new QLabel("活跃度评分:");
    activityTitle->setStyleSheet("font-weight: bold; color: #6c757d;");
    activityProgressBar = new QProgressBar();
    activityProgressBar->setRange(0, 100);
    activityProgressBar->setValue(0);
    activityProgressBar->setStyleSheet(
        "QProgressBar { "
        "    border: 2px solid #dee2e6; "
        "    border-radius: 5px; "
        "    text-align: center; "
        "    background-color: #f8f9fa; "
        "} "
        "QProgressBar::chunk { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #667eea, stop:1 #764ba2); "
        "    border-radius: 3px; "
        "}"
    );
    activityLayout->addWidget(activityTitle);
    activityLayout->addWidget(activityProgressBar);
    groupLayout->addLayout(activityLayout);

    // 忠诚度评分
    QHBoxLayout *loyaltyLayout = new QHBoxLayout();
    QLabel *loyaltyTitle = new QLabel("忠诚度评分:");
    loyaltyTitle->setStyleSheet("font-weight: bold; color: #6c757d;");
    loyaltyProgressBar = new QProgressBar();
    loyaltyProgressBar->setRange(0, 100);
    loyaltyProgressBar->setValue(0);
    loyaltyProgressBar->setStyleSheet(
        "QProgressBar { "
        "    border: 2px solid #dee2e6; "
        "    border-radius: 5px; "
        "    text-align: center; "
        "    background-color: #f8f9fa; "
        "} "
        "QProgressBar::chunk { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #28a745, stop:1 #20c997); "
        "    border-radius: 3px; "
        "}"
    );
    loyaltyLayout->addWidget(loyaltyTitle);
    loyaltyLayout->addWidget(loyaltyProgressBar);
    groupLayout->addLayout(loyaltyLayout);

    // 行为深度评分
    QHBoxLayout *behaviorLayout = new QHBoxLayout();
    QLabel *behaviorTitle = new QLabel("行为深度评分:");
    behaviorTitle->setStyleSheet("font-weight: bold; color: #6c757d;");
    behaviorProgressBar = new QProgressBar();
    behaviorProgressBar->setRange(0, 100);
    behaviorProgressBar->setValue(0);
    behaviorProgressBar->setStyleSheet(
        "QProgressBar { "
        "    border: 2px solid #dee2e6; "
        "    border-radius: 5px; "
        "    text-align: center; "
        "    background-color: #f8f9fa; "
        "} "
        "QProgressBar::chunk { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #ffc107, stop:1 #fd7e14); "
        "    border-radius: 3px; "
        "}"
    );
    behaviorLayout->addWidget(behaviorTitle);
    behaviorLayout->addWidget(behaviorProgressBar);
    groupLayout->addLayout(behaviorLayout);

    // 时间价值评分
    QHBoxLayout *timeLayout = new QHBoxLayout();
    QLabel *timeTitle = new QLabel("时间价值评分:");
    timeTitle->setStyleSheet("font-weight: bold; color: #6c757d;");
    timeProgressBar = new QProgressBar();
    timeProgressBar->setRange(0, 100);
    timeProgressBar->setValue(0);
    timeProgressBar->setStyleSheet(
        "QProgressBar { "
        "    border: 2px solid #dee2e6; "
        "    border-radius: 5px; "
        "    text-align: center; "
        "    background-color: #f8f9fa; "
        "} "
        "QProgressBar::chunk { "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #17a2b8, stop:1 #6f42c1); "
        "    border-radius: 3px; "
        "}"
    );
    timeLayout->addWidget(timeTitle);
    timeLayout->addWidget(timeProgressBar);
    groupLayout->addLayout(timeLayout);

    // 综合评分
    QHBoxLayout *totalLayout = new QHBoxLayout();
    QLabel *totalTitle = new QLabel("综合评分:");
    totalTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #495057;");
    totalScoreLabel = new QLabel("-");
    totalScoreLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #667eea;");
    totalLayout->addWidget(totalTitle);
    totalLayout->addWidget(totalScoreLabel);
    totalLayout->addStretch();
    groupLayout->addLayout(totalLayout);

    // 价值等级
    QHBoxLayout *levelLayout = new QHBoxLayout();
    QLabel *levelTitle = new QLabel("价值等级:");
    levelTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #495057;");
    levelLabel = new QLabel("-");
    levelLabel->setStyleSheet(
        "QLabel { "
        "    font-size: 18px; "
        "    font-weight: bold; "
        "    padding: 10px 20px; "
        "    border-radius: 20px; "
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "    stop:0 #667eea, stop:1 #764ba2); "
        "    color: white; "
        "}"
    );
    levelLayout->addWidget(levelTitle);
    levelLayout->addWidget(levelLabel);
    levelLayout->addStretch();
    groupLayout->addLayout(levelLayout);

    scrollLayout->addWidget(valueGroup);
}

void UserProfileTab::onRefreshClicked()
{
    QString userId = userIdEdit->text().trimmed();
    if (userId.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入用户ID");
        return;
    }
    loadUserProfile(userId);
}

void UserProfileTab::onDateRangeChanged()
{
    QString userId = userIdEdit->text().trimmed();
    if (userId.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入用户ID");
        return;
    }
    loadUserProfile(userId);
}

void UserProfileTab::onUserSelected(const QString &userId)
{
    userIdEdit->setText(userId);
    loadUserProfile(userId);
}

void UserProfileTab::loadUserProfile(const QString &userId)
{
    currentUserId = userId;
    
    // TODO: 调用后端API获取用户画像数据
    // 这里先使用模拟数据
    
    // 更新用户信息卡片
    userIdLabel->setText(userId);
    registerTimeLabel->setText("2025-01-01 10:00:00");
    lastActiveTimeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    lifecycleStatusLabel->setText("活跃");
    
    // 更新用户标签
    activityTagLabel->setText("高活跃");
    loyaltyTagLabel->setText("忠诚用户");
    valueTagLabel->setText("高价值");
    
    // 更新行为统计
    visitCountLabel->setText("1,234");
    avgSessionLabel->setText("15.5 分钟");
    
    // 更新常用页面表格
    topPagesTable->setRowCount(10);
    for (int i = 0; i < 10; i++) {
        topPagesTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        topPagesTable->setItem(i, 1, new QTableWidgetItem(QString("页面%1").arg(i + 1)));
        topPagesTable->setItem(i, 2, new QTableWidgetItem(QString::number(100 - i * 5)));
    }
    
    // 更新常用功能表格
    topFeaturesTable->setRowCount(5);
    for (int i = 0; i < 5; i++) {
        topFeaturesTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        topFeaturesTable->setItem(i, 1, new QTableWidgetItem(QString("功能%1").arg(i + 1)));
        topFeaturesTable->setItem(i, 2, new QTableWidgetItem(QString::number(50 - i * 5)));
    }
    
    // 更新兴趣分析
    pageInterestLabel->setText("首页: 35%, 个人中心: 25%, 设置页: 20%, 其他: 20%");
    featurePreferenceLabel->setText("登录: 30%, 数据查看: 25%, 报表导出: 20%, 其他: 25%");
    
    // 更新价值评估
    activityProgressBar->setValue(85);
    loyaltyProgressBar->setValue(90);
    behaviorProgressBar->setValue(75);
    timeProgressBar->setValue(80);
    totalScoreLabel->setText("82");
    levelLabel->setText("核心用户");
}

void UserProfileTab::updateUserInfoCard()
{
    // TODO: 从API数据更新用户信息卡片
}

void UserProfileTab::updateUserTags()
{
    // TODO: 从API数据更新用户标签
}

void UserProfileTab::updateBehaviorStats()
{
    // TODO: 从API数据更新行为统计
}

void UserProfileTab::updateInterestAnalysis()
{
    // TODO: 从API数据更新兴趣分析
}

void UserProfileTab::updateValueAssessment()
{
    // TODO: 从API数据更新价值评估
}