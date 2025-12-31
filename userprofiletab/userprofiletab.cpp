#include "userprofiletab.h"
#include "../networkmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>

UserProfileTab::UserProfileTab(QWidget *parent)
    : QWidget(parent)
    , m_currentUserId("")
    , m_currentUserIndex(0)
{
    setupUI();
    // 默认加载第一个用户的数据
    if (!m_userIdList.isEmpty()) {
        setUserId(m_userIdList.first());
    }
}

UserProfileTab::~UserProfileTab()
{
}

void UserProfileTab::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    
    // 工具栏
    setupToolbar();
    mainLayout->addWidget(createGroupBox("操作", createToolbarWidget()));
    
    // 用户信息卡片
    setupUserInfoCard();
    mainLayout->addWidget(createGroupBox("用户基本信息", createUserInfoWidget()));
    
    // 用户标签
    setupUserTags();
    mainLayout->addWidget(createGroupBox("用户标签", createUserTagsWidget()));
    
    // 行为统计
    setupBehaviorStats();
    mainLayout->addWidget(createGroupBox("行为统计", createBehaviorStatsWidget()));
    
    // 兴趣分析和价值评估（并排显示）
    setupInterestAnalysis();
    setupValueAssessment();
    QHBoxLayout *chartsLayout = new QHBoxLayout();
    chartsLayout->addWidget(createGroupBox("兴趣分析", createInterestAnalysisWidget()));
    chartsLayout->addWidget(createGroupBox("价值评估", createValueAssessmentWidget()));
    mainLayout->addLayout(chartsLayout);
}

void UserProfileTab::setupToolbar()
{
    m_refreshButton = new QPushButton("刷新");
    m_exportButton = new QPushButton("导出");
    m_prevUserButton = new QPushButton("上一个用户");
    m_nextUserButton = new QPushButton("下一个用户");
    
    connect(m_refreshButton, &QPushButton::clicked, this, &UserProfileTab::refreshUserProfile);
    connect(m_exportButton, &QPushButton::clicked, this, &UserProfileTab::exportUserProfile);
    connect(m_prevUserButton, &QPushButton::clicked, this, [this]() {
        if (m_currentUserIndex > 0) {
            m_currentUserIndex--;
            setUserId(m_userIdList[m_currentUserIndex]);
        }
    });
    connect(m_nextUserButton, &QPushButton::clicked, this, [this]() {
        if (m_currentUserIndex < m_userIdList.size() - 1) {
            m_currentUserIndex++;
            setUserId(m_userIdList[m_currentUserIndex]);
        }
    });
}

void UserProfileTab::setupUserInfoCard()
{
    m_userIdLabel = new QLabel("用户ID: -");
    m_registerTimeLabel = new QLabel("注册时间: -");
    m_lastActiveTimeLabel = new QLabel("最后活跃: -");
    m_totalEventsLabel = new QLabel("总事件数: -");
    m_activeDaysLabel = new QLabel("活跃天数: -");
    
    QGridLayout *infoLayout = new QGridLayout();
    infoLayout->addWidget(m_userIdLabel, 0, 0);
    infoLayout->addWidget(m_registerTimeLabel, 0, 1);
    infoLayout->addWidget(m_lastActiveTimeLabel, 1, 0);
    infoLayout->addWidget(m_totalEventsLabel, 1, 1);
    infoLayout->addWidget(m_activeDaysLabel, 2, 0, 1, 2);
}

void UserProfileTab::setupUserTags()
{
    m_activityTagLabel = new QLabel("活跃度: -");
    m_loyaltyTagLabel = new QLabel("忠诚度: -");
    m_valueTagLabel = new QLabel("价值: -");
    
    // 设置标签样式
    QString tagStyle = "QLabel { padding: 8px 16px; border-radius: 15px; font-weight: bold; }";
    m_activityTagLabel->setStyleSheet(tagStyle);
    m_loyaltyTagLabel->setStyleSheet(tagStyle);
    m_valueTagLabel->setStyleSheet(tagStyle);
    
    QHBoxLayout *tagsLayout = new QHBoxLayout();
    tagsLayout->addWidget(m_activityTagLabel);
    tagsLayout->addWidget(m_loyaltyTagLabel);
    tagsLayout->addWidget(m_valueTagLabel);
    tagsLayout->addStretch();
}

void UserProfileTab::setupBehaviorStats()
{
    m_visitCountLabel = new QLabel("访问次数: -");
    m_avgStayTimeLabel = new QLabel("平均停留时长: -");
    m_topPagesLabel = new QLabel("常用页面: -");
    m_topFeaturesLabel = new QLabel("常用功能: -");
    
    QGridLayout *statsLayout = new QGridLayout();
    statsLayout->addWidget(m_visitCountLabel, 0, 0);
    statsLayout->addWidget(m_avgStayTimeLabel, 0, 1);
    statsLayout->addWidget(m_topPagesLabel, 1, 0);
    statsLayout->addWidget(m_topFeaturesLabel, 1, 1);
}

void UserProfileTab::setupInterestAnalysis()
{
    m_interestChartView = new QWebEngineView();
    m_interestChartView->setMinimumHeight(300);
}

void UserProfileTab::setupValueAssessment()
{
    m_valueRadarView = new QWebEngineView();
    m_valueRadarView->setMinimumHeight(300);
}

void UserProfileTab::setUserId(const QString &userId)
{
    m_currentUserId = userId;
    loadUserProfileData();
}

void UserProfileTab::refreshUserProfile()
{
    if (!m_currentUserId.isEmpty()) {
        loadUserProfileData();
    }
}

void UserProfileTab::loadUserProfileData()
{
    NetworkManager *networkManager = new NetworkManager(this);
    
    // 获取用户画像数据
    networkManager->getUserProfile(m_currentUserId,
        [this](const QJsonObject &response) {
            if (response["success"].toBool()) {
                QJsonObject data = response["data"].toObject();
                if (!data.isEmpty()) {
                    updateUserInfoDisplay(data);
                } else {
                    qWarning() << "用户画像数据为空";
                }
            } else {
                qWarning() << "获取用户画像失败:" << response["message"].toString();
            }
        },
        [this](const QString &error) {
            QMessageBox::warning(this, "错误", "加载用户画像失败: " + error);
        });
    
    // 获取用户标签
    networkManager->getUserTags(m_currentUserId,
        [this](const QJsonObject &response) {
            if (response["success"].toBool()) {
                QJsonArray data = response["data"].toArray();
                updateUserTagsDisplay(data);
            } else {
                qWarning() << "获取用户标签失败:" << response["message"].toString();
            }
        },
        [this](const QString &error) {
            qWarning() << "加载用户标签失败:" << error;
        });
    
    // 获取用户行为特征
    networkManager->getUserBehaviorStats(m_currentUserId,
        [this](const QJsonObject &response) {
            if (response["success"].toBool()) {
                QJsonObject data = response["data"].toObject();
                if (!data.isEmpty()) {
                    updateBehaviorStatsDisplay(data);
                } else {
                    qWarning() << "用户行为统计数据为空";
                }
            } else {
                qWarning() << "获取用户行为统计失败:" << response["message"].toString();
            }
        },
        [this](const QString &error) {
            qWarning() << "加载行为统计失败:" << error;
        });
    
    // 获取用户兴趣画像
    networkManager->getUserInterestProfile(m_currentUserId,
        [this](const QJsonObject &response) {
            if (response["success"].toBool()) {
                QJsonArray data = response["data"].toArray();
                if (!data.isEmpty()) {
                    updateInterestAnalysisDisplay(data);
                } else {
                    qWarning() << "用户兴趣画像数据为空";
                }
            } else {
                qWarning() << "获取用户兴趣画像失败:" << response["message"].toString();
            }
        },
        [this](const QString &error) {
            qWarning() << "加载兴趣分析失败:" << error;
        });
    
    // 获取用户价值评估
    networkManager->getUserValueAssessment(m_currentUserId,
        [this](const QJsonObject &response) {
            if (response["success"].toBool()) {
                QJsonObject data = response["data"].toObject();
                if (!data.isEmpty()) {
                    updateValueAssessmentDisplay(data);
                } else {
                    qWarning() << "用户价值评估数据为空";
                }
            } else {
                qWarning() << "获取用户价值评估失败:" << response["message"].toString();
            }
        },
        [this](const QString &error) {
            qWarning() << "加载价值评估失败:" << error;
        });
}

void UserProfileTab::updateUserInfoDisplay(const QJsonObject &userInfo)
{
    m_userIdLabel->setText("用户ID: " + userInfo["userId"].toString());
    
    QString registerTime = userInfo["registerTime"].toString();
    if (!registerTime.isEmpty()) {
        QDateTime dateTime = QDateTime::fromString(registerTime, Qt::ISODate);
        m_registerTimeLabel->setText("注册时间: " + dateTime.toString("yyyy-MM-dd hh:mm:ss"));
    }
    
    QString lastActive = userInfo["lastActiveTime"].toString();
    if (!lastActive.isEmpty()) {
        QDateTime dateTime = QDateTime::fromString(lastActive, Qt::ISODate);
        m_lastActiveTimeLabel->setText("最后活跃: " + dateTime.toString("yyyy-MM-dd hh:mm:ss"));
    }
    
    m_totalEventsLabel->setText("总事件数: " + QString::number(userInfo["totalEvents"].toInt()));
    m_activeDaysLabel->setText("活跃天数: " + QString::number(userInfo["activeDays"].toInt()));
}

void UserProfileTab::updateUserTagsDisplay(const QJsonArray &tags)
{
    for (const QJsonValue &tagValue : tags) {
        QJsonObject tag = tagValue.toObject();
        QString type = tag["type"].toString();
        QString name = tag["name"].toString();
        
        QString color;
        if (type == "activity") {
            if (name == "高活跃") color = "#28a745";
            else if (name == "中活跃") color = "#ffc107";
            else color = "#dc3545";
            m_activityTagLabel->setText("活跃度: " + name);
            m_activityTagLabel->setStyleSheet("QLabel { padding: 8px 16px; border-radius: 15px; font-weight: bold; background-color: " + color + "; color: white; }");
        } else if (type == "loyalty") {
            if (name == "忠诚用户") color = "#17a2b8";
            else if (name == "普通用户") color = "#6c757d";
            else color = "#007bff";
            m_loyaltyTagLabel->setText("忠诚度: " + name);
            m_loyaltyTagLabel->setStyleSheet("QLabel { padding: 8px 16px; border-radius: 15px; font-weight: bold; background-color: " + color + "; color: white; }");
        } else if (type == "value") {
            if (name == "高价值") color = "#6610f2";
            else if (name == "中价值") color = "#fd7e14";
            else color = "#20c997";
            m_valueTagLabel->setText("价值: " + name);
            m_valueTagLabel->setStyleSheet("QLabel { padding: 8px 16px; border-radius: 15px; font-weight: bold; background-color: " + color + "; color: white; }");
        }
    }
}

void UserProfileTab::updateBehaviorStatsDisplay(const QJsonObject &behaviorStats)
{
    qDebug() << "行为统计数据:" << QJsonDocument(behaviorStats).toJson(QJsonDocument::Compact);
    
    QJsonObject visitFrequency = behaviorStats["visitFrequency"].toObject();
    m_visitCountLabel->setText("访问次数: " + QString::number(visitFrequency["totalVisits"].toInt()));
    
    double avgDailyVisits = visitFrequency["avgDailyVisits"].toDouble();
    m_avgStayTimeLabel->setText("平均每日访问: " + QString::number(avgDailyVisits, 'f', 2));
    
    // 获取常用页面
    QJsonArray pagePreference = behaviorStats["pagePreference"].toArray();
    // 处理页面偏好数据
    QStringList topPages;
    for (int i = 0; i < qMin(5, pagePreference.size()); ++i) {
        QJsonObject page = pagePreference[i].toObject();
        // 处理单个页面数据
        topPages << page["pageName"].toString();
    }
    m_topPagesLabel->setText("常用页面: " + (topPages.isEmpty() ? "无数据" : topPages.join(", ")));
    
    // 获取常用功能
    QJsonArray featureUsage = behaviorStats["featureUsage"].toArray();
    // 处理功能使用数据
    QStringList topFeatures;
    for (int i = 0; i < qMin(5, featureUsage.size()); ++i) {
        QJsonObject feature = featureUsage[i].toObject();
        // 处理单个功能数据
        topFeatures << feature["eventName"].toString();
    }
    m_topFeaturesLabel->setText("常用功能: " + (topFeatures.isEmpty() ? "无数据" : topFeatures.join(", ")));
}

void UserProfileTab::updateInterestAnalysisDisplay(const QJsonArray &interests)
{
    // 构建ECharts饼图配置
    QString chartData;
    for (const QJsonValue &interestValue : interests) {
        QJsonObject interest = interestValue.toObject();
        chartData += QString("{name: '%1', value: %2},")
            .arg(interest["name"].toString())
            .arg(interest["score"].toDouble());
    }
    if (!chartData.isEmpty()) {
        chartData.chop(1); // 移除最后一个逗号
    }
    
    QString html = QString(R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <script src="qrc:/echarts/ECharts/echarts.min.js"></script>
        </head>
        <body style="margin:0;padding:0;">
            <div id="chart" style="width:100%%;height:300px;"></div>
            <script>
                var chart = echarts.init(document.getElementById('chart'));
                var option = {
                    title: { text: '用户兴趣分布', left: 'center' },
                    tooltip: { trigger: 'item' },
                    legend: { orient: 'vertical', left: 'left' },
                    series: [{
                        name: '兴趣',
                        type: 'pie',
                        radius: '50%%',
                        data: [%1],
                        emphasis: {
                            itemStyle: {
                                shadowBlur: 10,
                                shadowOffsetX: 0,
                                shadowColor: 'rgba(0, 0, 0, 0.5)'
                            }
                        }
                    }]
                };
                chart.setOption(option);
            </script>
        </body>
        </html>
    )").arg(chartData);
    
    m_interestChartView->setHtml(html);
}

void UserProfileTab::updateValueAssessmentDisplay(const QJsonObject &valueAssessment)
{
    renderValueRadarChart(valueAssessment);
}

void UserProfileTab::renderValueRadarChart(const QJsonObject &valueData)
{
    QString html = QString(R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <script src="qrc:/echarts/ECharts/echarts.min.js"></script>
        </head>
        <body style="margin:0;padding:0;">
            <div id="chart" style="width:100%%;height:300px;"></div>
            <script>
                var chart = echarts.init(document.getElementById('chart'));
                var option = {
                    title: { text: '用户价值评估', left: 'center' },
                    tooltip: {},
                    legend: { data: ['评分'], top: 30 },
                    radar: {
                        indicator: [
                            { name: '活跃度', max: 100 },
                            { name: '忠诚度', max: 100 },
                            { name: '行为深度', max: 100 },
                            { name: '时间价值', max: 100 },
                            { name: '综合评分', max: 100 }
                        ]
                    },
                    series: [{
                        name: '价值评估',
                        type: 'radar',
                        data: [{
                            value: [%1, %2, %3, %4, %5],
                            name: '评分'
                        }]
                    }]
                };
                chart.setOption(option);
            </script>
        </body>
        </html>
    )").arg(valueData["activityScore"].toInt())
      .arg(valueData["loyaltyScore"].toInt())
      .arg(valueData["behaviorScore"].toInt())
      .arg(valueData["timeScore"].toInt())
      .arg(valueData["totalScore"].toInt());
    
    m_valueRadarView->setHtml(html);
}

void UserProfileTab::exportUserProfile()
{
    QString fileName = QFileDialog::getSaveFileName(this, "导出用户画像", 
        QString("user_profile_%1.csv").arg(m_currentUserId), "CSV文件 (*.csv)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件: " + fileName);
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    // 写入CSV内容
    out << "用户ID," << m_currentUserId << "\n";
    out << "注册时间," << m_registerTimeLabel->text().replace("注册时间: ", "") << "\n";
    out << "最后活跃," << m_lastActiveTimeLabel->text().replace("最后活跃: ", "") << "\n";
    out << "总事件数," << m_totalEventsLabel->text().replace("总事件数: ", "") << "\n";
    out << "活跃天数," << m_activeDaysLabel->text().replace("活跃天数: ", "") << "\n";
    out << "\n";
    out << "标签类型,标签名称\n";
    out << "活跃度," << m_activityTagLabel->text().replace("活跃度: ", "") << "\n";
    out << "忠诚度," << m_loyaltyTagLabel->text().replace("忠诚度: ", "") << "\n";
    out << "价值," << m_valueTagLabel->text().replace("价值: ", "") << "\n";
    
    file.close();
    QMessageBox::information(this, "成功", "用户画像导出成功!");
}

QWidget *UserProfileTab::createToolbarWidget()
{
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(m_prevUserButton);
    layout->addWidget(m_nextUserButton);
    layout->addStretch();
    layout->addWidget(m_refreshButton);
    layout->addWidget(m_exportButton);
    
    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    return widget;
}

QWidget *UserProfileTab::createUserInfoWidget()
{
    QWidget *widget = new QWidget();
    QGridLayout *layout = new QGridLayout(widget);
    layout->addWidget(m_userIdLabel, 0, 0);
    layout->addWidget(m_registerTimeLabel, 0, 1);
    layout->addWidget(m_lastActiveTimeLabel, 1, 0);
    layout->addWidget(m_totalEventsLabel, 1, 1);
    layout->addWidget(m_activeDaysLabel, 2, 0, 1, 2);
    return widget;
}

QWidget *UserProfileTab::createUserTagsWidget()
{
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->addWidget(m_activityTagLabel);
    layout->addWidget(m_loyaltyTagLabel);
    layout->addWidget(m_valueTagLabel);
    layout->addStretch();
    return widget;
}

QWidget *UserProfileTab::createBehaviorStatsWidget()
{
    QWidget *widget = new QWidget();
    QGridLayout *layout = new QGridLayout(widget);
    layout->addWidget(m_visitCountLabel, 0, 0);
    layout->addWidget(m_avgStayTimeLabel, 0, 1);
    layout->addWidget(m_topPagesLabel, 1, 0);
    layout->addWidget(m_topFeaturesLabel, 1, 1);
    return widget;
}

QWidget *UserProfileTab::createInterestAnalysisWidget()
{
    return m_interestChartView;
}

QWidget *UserProfileTab::createValueAssessmentWidget()
{
    return m_valueRadarView;
}

QGroupBox *UserProfileTab::createGroupBox(const QString &title, QWidget *content)
{
    QGroupBox *groupBox = new QGroupBox(title);
    QVBoxLayout *layout = new QVBoxLayout(groupBox);
    layout->addWidget(content);
    return groupBox;
}