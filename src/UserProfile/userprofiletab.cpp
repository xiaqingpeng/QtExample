#include "userprofiletab.h"
#include "../Network/networkmanager.h"
#include "common.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QStandardPaths>
#include <QPrinter>
#include <QTextDocument>
#include <QPushButton>
#include <QCoreApplication>

UserProfileTab::UserProfileTab(QWidget *parent)
    : QWidget(parent)
    , m_currentUserId("")
    , m_currentUserIndex(0)
{
    setupUI();
    // 加载用户列表
    loadUserList();
    
    // 设置定时刷新（每30秒）
    m_refreshTimer = new QTimer(this);
    connect(m_refreshTimer, &QTimer::timeout, this, &UserProfileTab::refreshUserProfile);
    m_refreshTimer->start(30000);
    
    // 连接主题变化信号
    connect(ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &UserProfileTab::applyTheme);
}

UserProfileTab::~UserProfileTab()
{
}

void UserProfileTab::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(20);
    
    // 工具栏 - 简约设计
    setupToolbar();
    mainLayout->addWidget(createModernCard("", createToolbarWidget(), false));
    
    // 用户信息卡片 - 现代卡片设计
    setupUserInfoCard();
    mainLayout->addWidget(createModernCard("用户概览", createUserInfoWidget()));
    
    // 用户标签 - 标签云设计
    setupUserTags();
    mainLayout->addWidget(createModernCard("用户标签", createUserTagsWidget()));
    
    // 行为统计 - 网格布局
    setupBehaviorStats();
    mainLayout->addWidget(createModernCard("行为分析", createBehaviorStatsWidget()));
    
    // 兴趣分析和价值评估（并排显示）- 图表卡片
    setupInterestAnalysis();
    setupValueAssessment();
    QHBoxLayout *chartsLayout = new QHBoxLayout();
    chartsLayout->setSpacing(20);
    chartsLayout->addWidget(createModernCard("兴趣分布", createInterestAnalysisWidget()));
    chartsLayout->addWidget(createModernCard("价值评估", createValueAssessmentWidget()));
    mainLayout->addLayout(chartsLayout);
    
    // 应用主题
    applyTheme();
}

void UserProfileTab::setupToolbar()
{
    m_refreshButton = new QPushButton();
    m_exportButton = new QPushButton();
    m_prevUserButton = new QPushButton();
    m_nextUserButton = new QPushButton();
    
    // 设置对象名称用于主题样式
    m_refreshButton->setObjectName("primaryButton");
    m_exportButton->setObjectName("primaryButton");
    m_prevUserButton->setObjectName("secondaryButton");
    m_nextUserButton->setObjectName("secondaryButton");
    
    // 使用安全的文本设置
    m_refreshButton->setText("刷新");
    m_exportButton->setText("导出");
    m_prevUserButton->setText("< 上一个");
    m_nextUserButton->setText("下一个 >");
    
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
    
    // 设置对象名称用于主题样式
    m_userIdLabel->setObjectName("infoLabel");
    m_registerTimeLabel->setObjectName("infoLabel");
    m_lastActiveTimeLabel->setObjectName("infoLabel");
    m_totalEventsLabel->setObjectName("infoLabel");
    m_activeDaysLabel->setObjectName("infoLabel");
}

void UserProfileTab::setupUserTags()
{
    m_activityTagLabel = new QLabel("活跃度: -");
    m_loyaltyTagLabel = new QLabel("忠诚度: -");
    m_valueTagLabel = new QLabel("价值: -");
    
    // 设置对象名称用于主题样式
    m_activityTagLabel->setObjectName("tagLabel");
    m_loyaltyTagLabel->setObjectName("tagLabel");
    m_valueTagLabel->setObjectName("tagLabel");
    
    // 设置标签对齐
    m_activityTagLabel->setAlignment(Qt::AlignCenter);
    m_loyaltyTagLabel->setAlignment(Qt::AlignCenter);
    m_valueTagLabel->setAlignment(Qt::AlignCenter);
}

void UserProfileTab::setupBehaviorStats()
{
    m_visitCountLabel = new QLabel("访问次数: -");
    m_avgStayTimeLabel = new QLabel("平均每日访问: -");
    m_topPagesLabel = new QLabel("常用页面: -");
    m_topFeaturesLabel = new QLabel("常用功能: -");
    
    // 设置对象名称用于主题样式
    m_visitCountLabel->setObjectName("statsLabel");
    m_avgStayTimeLabel->setObjectName("statsLabel");
    m_topPagesLabel->setObjectName("statsLabel");
    m_topFeaturesLabel->setObjectName("statsLabel");
    
    // 设置文本换行
    m_topPagesLabel->setWordWrap(true);
    m_topFeaturesLabel->setWordWrap(true);
}

void UserProfileTab::setupInterestAnalysis()
{
#ifdef WEBENGINE_AVAILABLE
    m_interestChartView = new QWebEngineView();
    m_interestChartView->setMinimumHeight(350);
    m_interestChartView->setObjectName("chartView");
#else
    m_interestChartView = new QLabel("WebEngine 不可用 - 兴趣分析图表已禁用");
    m_interestChartView->setMinimumHeight(350);
    m_interestChartView->setObjectName("chartView");
    m_interestChartView->setAlignment(Qt::AlignCenter);
    m_interestChartView->setStyleSheet("QLabel { color: #666; font-size: 14px; }");
#endif
}

void UserProfileTab::setupValueAssessment()
{
#ifdef WEBENGINE_AVAILABLE
    m_valueRadarView = new QWebEngineView();
    m_valueRadarView->setMinimumHeight(350);
    m_valueRadarView->setObjectName("chartView");
#else
    m_valueRadarView = new QLabel("WebEngine 不可用 - 价值评估图表已禁用");
    m_valueRadarView->setMinimumHeight(350);
    m_valueRadarView->setObjectName("chartView");
    m_valueRadarView->setAlignment(Qt::AlignCenter);
    m_valueRadarView->setStyleSheet("QLabel { color: #666; font-size: 14px; }");
#endif
}

void UserProfileTab::loadUserList()
{
    NetworkManager *networkManager = new NetworkManager(this);
    
    // 获取用户列表（第一页，每页100个用户）
    networkManager->getTopUsers(1, 100,
        [this](const QJsonObject &response) {
            if (response["success"].toBool()) {
                QJsonObject data = response["data"].toObject();
                QJsonArray users;
                
                // 尝试获取users或list字段
                if (data.contains("list")) {
                    users = data["list"].toArray();
                } else if (data.contains("users")) {
                    users = data["users"].toArray();
                }
                
                // 提取用户ID列表
                m_userIdList.clear();
                for (const QJsonValue &userValue : users) {
                    QJsonObject user = userValue.toObject();
                    QString userId = user["userId"].toString();
                    if (!userId.isEmpty()) {
                        m_userIdList.append(userId);
                    }
                }
                
                // 默认加载第一个用户的数据
                if (!m_userIdList.isEmpty()) {
                    m_currentUserIndex = 0;
                    setUserId(m_userIdList.first());
                } else {
                    qWarning() << "用户列表为空";
                    QMessageBox::warning(this, "提示", "未找到用户数据，请检查后端API或数据库");
                }
            } else {
                qWarning() << "获取用户列表失败:" << response["message"].toString();
                QMessageBox::warning(this, "错误", "获取用户列表失败: " + response["message"].toString());
            }
        },
        [this](const QString &error) {
            qWarning() << "加载用户列表失败:" << error;
            QMessageBox::warning(this, "错误", "加载用户列表失败: " + error);
        });
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
        [](const QString &error) {
            qWarning() << "加载用户标签失败:" << error;
        });
    
    // 获取用户行为特征
    networkManager->getUserBehaviorStats(m_currentUserId,
        [this](const QJsonObject &response) {
          //  // LOG_DEBUG() << "用户行为特征API返回数据:" << QJsonDocument(response).toJson(QJsonDocument::Compact);
            if (response["success"].toBool()) {
                QJsonObject data = response["data"].toObject();
           //     // LOG_DEBUG() << "用户行为特征data对象:" << QJsonDocument(data).toJson(QJsonDocument::Compact);
                if (!data.isEmpty()) {
                    updateBehaviorStatsDisplay(data);
                } else {
                    qWarning() << "用户行为统计数据为空";
                }
            } else {
                qWarning() << "获取用户行为统计失败:" << response["message"].toString();
            }
        },
        [](const QString &error) {
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
        [](const QString &error) {
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
        [](const QString &error) {
            qWarning() << "加载价值评估失败:" << error;
        });
}

void UserProfileTab::updateUserInfoDisplay(const QJsonObject &userInfo)
{
   // // LOG_DEBUG() << "用户画像数据:" << QJsonDocument(userInfo).toJson(QJsonDocument::Compact);
    
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
    
    // totalEvents和activeDays可能是字符串或数字
    int totalEvents = 0;
    int activeDays = 0;
    
    if (userInfo["totalEvents"].isString()) {
        totalEvents = userInfo["totalEvents"].toString().toInt();
    } else {
        totalEvents = userInfo["totalEvents"].toInt();
    }
    
    if (userInfo["activeDays"].isString()) {
        activeDays = userInfo["activeDays"].toString().toInt();
    } else {
        activeDays = userInfo["activeDays"].toInt();
    }
    
  //  // LOG_DEBUG() << "总事件数:" << totalEvents << "活跃天数:" << activeDays;
    
    m_totalEventsLabel->setText("总事件数: " + QString::number(totalEvents));
    m_activeDaysLabel->setText("活跃天数: " + QString::number(activeDays));
}

void UserProfileTab::updateUserTagsDisplay(const QJsonArray &tags)
{
    for (const QJsonValue &tagValue : tags) {
        QJsonObject tag = tagValue.toObject();
        QString type = tag["type"].toString();
        QString name = tag["name"].toString();
        
        QString color;
        QString textColor = "white";
        
        if (type == "activity") {
            if (name == "高活跃") {
                color = "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #28a745, stop:1 #20c997)";
            } else if (name == "中活跃") {
                color = "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #ffc107, stop:1 #fd7e14)";
            } else {
                color = "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #dc3545, stop:1 #e83e8c)";
            }
            m_activityTagLabel->setText("活跃度: " + name);
            m_activityTagLabel->setStyleSheet(QString(R"(
                QLabel {
                    padding: 12px 20px;
                    border-radius: 20px;
                    font-weight: 600;
                    font-size: 13px;
                    color: %1;
                    background: %2;
                    margin: 8px;
                    min-width: 100px;
                }
            )").arg(textColor).arg(color));
        } else if (type == "loyalty") {
            if (name == "忠诚用户") {
                color = "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #17a2b8, stop:1 #007bff)";
            } else if (name == "普通用户") {
                color = "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6c757d, stop:1 #495057)";
            } else {
                color = "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #007bff, stop:1 #6610f2)";
            }
            m_loyaltyTagLabel->setText("忠诚度: " + name);
            m_loyaltyTagLabel->setStyleSheet(QString(R"(
                QLabel {
                    padding: 12px 20px;
                    border-radius: 20px;
                    font-weight: 600;
                    font-size: 13px;
                    color: %1;
                    background: %2;
                    margin: 8px;
                    min-width: 100px;
                }
            )").arg(textColor).arg(color));
        } else if (type == "value") {
            if (name == "高价值") {
                color = "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #6610f2, stop:1 #e83e8c)";
            } else if (name == "中价值") {
                color = "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #fd7e14, stop:1 #ffc107)";
            } else {
                color = "qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #20c997, stop:1 #28a745)";
            }
            m_valueTagLabel->setText("价值: " + name);
            m_valueTagLabel->setStyleSheet(QString(R"(
                QLabel {
                    padding: 12px 20px;
                    border-radius: 20px;
                    font-weight: 600;
                    font-size: 13px;
                    color: %1;
                    background: %2;
                    margin: 8px;
                    min-width: 100px;
                }
            )").arg(textColor).arg(color));
        }
    }
}

void UserProfileTab::updateBehaviorStatsDisplay(const QJsonObject &behaviorStats)
{
 //   // LOG_DEBUG() << "行为统计数据:" << QJsonDocument(behaviorStats).toJson(QJsonDocument::Compact);
    
    QJsonObject visitFrequency = behaviorStats["visitFrequency"].toObject();
  //  // LOG_DEBUG() << "访问频率数据:" << QJsonDocument(visitFrequency).toJson(QJsonDocument::Compact);
    
    int totalVisits = visitFrequency["totalVisits"].toInt();
    m_visitCountLabel->setText("访问次数: " + QString::number(totalVisits));
    
    // avgDailyVisits可能是字符串或数字
    double avgDailyVisits = 0.0;
    if (visitFrequency["avgDailyVisits"].isString()) {
        avgDailyVisits = visitFrequency["avgDailyVisits"].toString().toDouble();
    } else {
        avgDailyVisits = visitFrequency["avgDailyVisits"].toDouble();
    }
  //  // LOG_DEBUG() << "平均每日访问:" << avgDailyVisits;
    m_avgStayTimeLabel->setText("平均每日访问: " + QString::number(avgDailyVisits, 'f', 2));
    
    // 获取常用页面
    QJsonArray pagePreference = behaviorStats["pagePreference"].toArray();
   // // LOG_DEBUG() << "页面偏好数据:" << QJsonDocument(pagePreference).toJson(QJsonDocument::Compact);
    // 处理页面偏好数据
    QStringList topPages;
    for (int i = 0; i < qMin(5, pagePreference.size()); ++i) {
        QJsonObject page = pagePreference[i].toObject();
        // 支持两种字段名格式：驼峰(pageName)和下划线(page_name)
        QString pageName = page["pageName"].toString();
        if (pageName.isEmpty()) {
            pageName = page["page_name"].toString();
        }
        if (!pageName.isEmpty()) {
            topPages << pageName;
        }
    }
    m_topPagesLabel->setText("常用页面: " + (topPages.isEmpty() ? "无数据" : topPages.join(", ")));
    
    // 获取常用功能
    QJsonArray featureUsage = behaviorStats["featureUsage"].toArray();
   // // LOG_DEBUG() << "功能使用数据:" << QJsonDocument(featureUsage).toJson(QJsonDocument::Compact);
    // 处理功能使用数据
    QStringList topFeatures;
    for (int i = 0; i < qMin(5, featureUsage.size()); ++i) {
        QJsonObject feature = featureUsage[i].toObject();
        // 支持两种字段名格式：驼峰(eventName)和下划线(event_name)
        QString eventName = feature["eventName"].toString();
        if (eventName.isEmpty()) {
            eventName = feature["event_name"].toString();
        }
        if (!eventName.isEmpty()) {
            topFeatures << eventName;
        }
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
            <script src="qrc:/src/ECharts/echarts.min.js"></script>
        </head>
        <body style="margin:0;padding:0;background:#ffffff;">
            <div id="chart" style="width:100%%;height:350px;"></div>
            <script>
                var chart = echarts.init(document.getElementById('chart'));
                var option = {
                    title: { 
                        text: '兴趣分布',
                        left: 'center',
                        top: 20,
                        textStyle: {
                            fontSize: 16,
                            fontWeight: '600',
                            color: '#2c3e50'
                        }
                    },
                    tooltip: { 
                        trigger: 'item',
                        formatter: '{a} <br/>{b}: {c} ({d}%%)'
                    },
                    legend: { 
                        orient: 'vertical', 
                        left: 'left',
                        top: 'middle',
                        textStyle: {
                            fontSize: 12,
                            color: '#495057'
                        }
                    },
                    series: [{
                        name: '兴趣',
                        type: 'pie',
                        radius: ['40%%', '70%%'],
                        center: ['60%%', '50%%'],
                        data: [%1],
                        emphasis: {
                            itemStyle: {
                                shadowBlur: 10,
                                shadowOffsetX: 0,
                                shadowColor: 'rgba(0, 0, 0, 0.5)'
                            }
                        },
                        itemStyle: {
                            borderRadius: 8,
                            borderColor: '#fff',
                            borderWidth: 2
                        },
                        label: {
                            show: false
                        },
                        labelLine: {
                            show: false
                        }
                    }],
                    color: ['#007bff', '#28a745', '#ffc107', '#dc3545', '#17a2b8', '#6610f2', '#fd7e14', '#20c997']
                };
                chart.setOption(option);
                
                // 响应式调整
                window.addEventListener('resize', function() {
                    chart.resize();
                });
            </script>
        </body>
        </html>
    )").arg(chartData);
    
#ifdef WEBENGINE_AVAILABLE
    m_interestChartView->setHtml(html);
#else
    QLabel* chartLabel = qobject_cast<QLabel*>(m_interestChartView);
    if (chartLabel) {
        chartLabel->setText("WebEngine 不可用\n兴趣分析图表已禁用");
    }
#endif
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
            <script src="qrc:/src/ECharts/echarts.min.js"></script>
        </head>
        <body style="margin:0;padding:0;background:#ffffff;">
            <div id="chart" style="width:100%%;height:350px;"></div>
            <script>
                var chart = echarts.init(document.getElementById('chart'));
                var option = {
                    title: { 
                        text: '价值评估',
                        left: 'center',
                        top: 20,
                        textStyle: {
                            fontSize: 16,
                            fontWeight: '600',
                            color: '#2c3e50'
                        }
                    },
                    tooltip: {
                        trigger: 'item',
                        formatter: function(params) {
                            return params.name + ': ' + params.value;
                        }
                    },
                    legend: { 
                        data: ['评分'], 
                        top: 50,
                        textStyle: {
                            fontSize: 12,
                            color: '#495057'
                        }
                    },
                    radar: {
                        center: ['50%%', '60%%'],
                        radius: '60%%',
                        indicator: [
                            { name: '活跃度', max: 100 },
                            { name: '忠诚度', max: 100 },
                            { name: '行为深度', max: 100 },
                            { name: '时间价值', max: 100 },
                            { name: '综合评分', max: 100 }
                        ],
                        name: {
                            textStyle: {
                                color: '#495057',
                                fontSize: 12
                            }
                        },
                        splitArea: {
                            areaStyle: {
                                color: ['rgba(0, 123, 255, 0.1)', 'rgba(0, 123, 255, 0.05)']
                            }
                        },
                        splitLine: {
                            lineStyle: {
                                color: 'rgba(0, 123, 255, 0.2)'
                            }
                        }
                    },
                    series: [{
                        name: '价值评估',
                        type: 'radar',
                        data: [{
                            value: [%2, %3, %4, %5, %6],
                            name: '评分',
                            itemStyle: {
                                color: '#007bff'
                            },
                            areaStyle: {
                                color: 'rgba(0, 123, 255, 0.2)'
                            },
                            lineStyle: {
                                color: '#007bff',
                                width: 2
                            }
                        }]
                    }]
                };
                chart.setOption(option);
                
                // 响应式调整
                window.addEventListener('resize', function() {
                    chart.resize();
                });
            </script>
        </body>
        </html>
    )").arg(valueData["activityScore"].toInt())
      .arg(valueData["loyaltyScore"].toInt())
      .arg(valueData["behaviorScore"].toInt())
      .arg(valueData["timeScore"].toInt())
      .arg(valueData["totalScore"].toInt());
    
#ifdef WEBENGINE_AVAILABLE
    m_valueRadarView->setHtml(html);
#else
    QLabel* chartLabel = qobject_cast<QLabel*>(m_valueRadarView);
    if (chartLabel) {
        chartLabel->setText("WebEngine 不可用\n价值评估图表已禁用");
    }
#endif
}


void UserProfileTab::exportUserProfile()
{
    // qDebug() << "开始用户画像导出...";
    
    // 检查当前用户ID是否有效
    if (m_currentUserId.isEmpty()) {
        // qDebug() << "当前用户ID为空，无法导出";
        QMessageBox::warning(this, "错误", "当前没有选择用户，无法导出用户画像");
        return;
    }
    
    // qDebug() << "当前用户ID:" << m_currentUserId;
    
    // 创建导出格式选择对话框
    QMessageBox formatDialog(this);
    formatDialog.setWindowTitle("选择导出格式");
    formatDialog.setText("请选择要导出的文件格式：");
    formatDialog.setIcon(QMessageBox::Question);
    
    QPushButton *csvButton = formatDialog.addButton("CSV格式", QMessageBox::ActionRole);
    QPushButton *excelButton = formatDialog.addButton("Excel格式", QMessageBox::ActionRole);
    QPushButton *pdfButton = formatDialog.addButton("PDF格式", QMessageBox::ActionRole);
    QPushButton *cancelButton = formatDialog.addButton("取消", QMessageBox::RejectRole);
    
    formatDialog.exec();
    
    if (formatDialog.clickedButton() == cancelButton) {
        // qDebug() << "用户取消了格式选择";
        return;
    }
    
    // 根据选择的格式调用相应的导出方法
    if (formatDialog.clickedButton() == csvButton) {
        exportUserProfileToCSV();
    } else if (formatDialog.clickedButton() == excelButton) {
        exportUserProfileToExcel();
    } else if (formatDialog.clickedButton() == pdfButton) {
        exportUserProfileToPDF();
    }
}

void UserProfileTab::exportUserProfileToCSV()
{
    // qDebug() << "开始CSV格式用户画像导出...";
    
    // 设置默认文件名和路径
    QString defaultFileName = QString("user_profile_%1.csv").arg(m_currentUserId);
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + defaultFileName;
    
    QString fileName = QFileDialog::getSaveFileName(
        this, 
        "导出用户画像为CSV", 
        defaultPath,
        "CSV文件 (*.csv);;所有文件 (*.*)",
        nullptr,
        QFileDialog::DontUseNativeDialog
    );
    
    if (fileName.isEmpty()) {
        // qDebug() << "用户取消了CSV文件选择";
        return;
    }
    
    // qDebug() << "开始写入CSV文件:" << fileName;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // qDebug() << "无法打开CSV文件:" << fileName << "错误:" << file.errorString();
        QMessageBox::warning(this, "错误", "无法打开文件: " + fileName + "\n错误: " + file.errorString());
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    // 写入导出信息
    out << "用户画像导出时间," << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
    out << "\n";
    
    // 写入基本信息
    out << "基本信息\n";
    out << "用户ID," << m_currentUserId << "\n";
    
    // 安全地获取标签文本，避免空指针异常
    QString registerTime = m_registerTimeLabel ? m_registerTimeLabel->text().replace("注册时间: ", "") : "无数据";
    QString lastActive = m_lastActiveTimeLabel ? m_lastActiveTimeLabel->text().replace("最后活跃: ", "") : "无数据";
    QString totalEvents = m_totalEventsLabel ? m_totalEventsLabel->text().replace("总事件数: ", "") : "无数据";
    QString activeDays = m_activeDaysLabel ? m_activeDaysLabel->text().replace("活跃天数: ", "") : "无数据";
    
    out << "注册时间," << registerTime << "\n";
    out << "最后活跃," << lastActive << "\n";
    out << "总事件数," << totalEvents << "\n";
    out << "活跃天数," << activeDays << "\n";
    out << "\n";
    
    // 写入用户标签
    out << "用户标签\n";
    out << "标签类型,标签名称\n";
    
    QString activityTag = m_activityTagLabel ? m_activityTagLabel->text().replace("活跃度: ", "") : "无数据";
    QString loyaltyTag = m_loyaltyTagLabel ? m_loyaltyTagLabel->text().replace("忠诚度: ", "") : "无数据";
    QString valueTag = m_valueTagLabel ? m_valueTagLabel->text().replace("价值: ", "") : "无数据";
    
    out << "活跃度," << activityTag << "\n";
    out << "忠诚度," << loyaltyTag << "\n";
    out << "价值," << valueTag << "\n";
    out << "\n";
    
    // 写入行为统计
    out << "行为统计\n";
    QString visitCount = m_visitCountLabel ? m_visitCountLabel->text().replace("访问次数: ", "") : "无数据";
    QString avgStayTime = m_avgStayTimeLabel ? m_avgStayTimeLabel->text().replace("平均每日访问: ", "") : "无数据";
    QString topPages = m_topPagesLabel ? m_topPagesLabel->text().replace("常用页面: ", "") : "无数据";
    QString topFeatures = m_topFeaturesLabel ? m_topFeaturesLabel->text().replace("常用功能: ", "") : "无数据";
    
    out << "访问次数," << visitCount << "\n";
    out << "平均每日访问," << avgStayTime << "\n";
    out << "常用页面," << topPages << "\n";
    out << "常用功能," << topFeatures << "\n";
    
    file.close();
    // qDebug() << "CSV用户画像导出完成，文件保存到:" << fileName;
    QMessageBox::information(this, "成功", "用户画像导出成功!\n文件保存到: " + fileName);
}

void UserProfileTab::exportUserProfileToExcel()
{
    // qDebug() << "开始Excel格式用户画像导出...";
    
    // 设置默认文件名和路径
    QString defaultFileName = QString("user_profile_%1.xlsx").arg(m_currentUserId);
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + defaultFileName;
    
    QString fileName = QFileDialog::getSaveFileName(
        this, 
        "导出用户画像为Excel", 
        defaultPath,
        "Excel文件 (*.xlsx);;CSV文件 (*.csv);;所有文件 (*.*)",
        nullptr,
        QFileDialog::DontUseNativeDialog
    );
    
    if (fileName.isEmpty()) {
        // qDebug() << "用户取消了Excel文件选择";
        return;
    }
    
    // qDebug() << "开始写入Excel文件:" << fileName;
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件: " + fileName);
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    // 写入UTF-8 BOM以确保Excel正确显示中文
    out << "\xEF\xBB\xBF";
    
    // 写入导出信息
    out << "用户画像导出时间," << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "\n";
    out << "\n";
    
    // 写入基本信息
    out << "基本信息\n";
    out << "用户ID," << m_currentUserId << "\n";
    
    QString registerTime = m_registerTimeLabel ? m_registerTimeLabel->text().replace("注册时间: ", "") : "无数据";
    QString lastActive = m_lastActiveTimeLabel ? m_lastActiveTimeLabel->text().replace("最后活跃: ", "") : "无数据";
    QString totalEvents = m_totalEventsLabel ? m_totalEventsLabel->text().replace("总事件数: ", "") : "无数据";
    QString activeDays = m_activeDaysLabel ? m_activeDaysLabel->text().replace("活跃天数: ", "") : "无数据";
    
    out << "注册时间," << registerTime << "\n";
    out << "最后活跃," << lastActive << "\n";
    out << "总事件数," << totalEvents << "\n";
    out << "活跃天数," << activeDays << "\n";
    out << "\n";
    
    // 写入用户标签
    out << "用户标签\n";
    out << "标签类型,标签名称\n";
    
    QString activityTag = m_activityTagLabel ? m_activityTagLabel->text().replace("活跃度: ", "") : "无数据";
    QString loyaltyTag = m_loyaltyTagLabel ? m_loyaltyTagLabel->text().replace("忠诚度: ", "") : "无数据";
    QString valueTag = m_valueTagLabel ? m_valueTagLabel->text().replace("价值: ", "") : "无数据";
    
    out << "活跃度," << activityTag << "\n";
    out << "忠诚度," << loyaltyTag << "\n";
    out << "价值," << valueTag << "\n";
    out << "\n";
    
    // 写入行为统计
    out << "行为统计\n";
    QString visitCount = m_visitCountLabel ? m_visitCountLabel->text().replace("访问次数: ", "") : "无数据";
    QString avgStayTime = m_avgStayTimeLabel ? m_avgStayTimeLabel->text().replace("平均每日访问: ", "") : "无数据";
    QString topPages = m_topPagesLabel ? m_topPagesLabel->text().replace("常用页面: ", "") : "无数据";
    QString topFeatures = m_topFeaturesLabel ? m_topFeaturesLabel->text().replace("常用功能: ", "") : "无数据";
    
    out << "访问次数," << visitCount << "\n";
    out << "平均每日访问," << avgStayTime << "\n";
    out << "常用页面," << topPages << "\n";
    out << "常用功能," << topFeatures << "\n";
    
    file.close();
    // qDebug() << "Excel用户画像导出完成，文件保存到:" << fileName;
    QMessageBox::information(this, "成功", "用户画像导出成功!\n文件保存到: " + fileName + "\n\n注意：文件采用CSV格式保存为.xlsx扩展名，可被Excel打开。");
}

void UserProfileTab::exportUserProfileToPDF()
{
    // qDebug() << "开始PDF格式用户画像导出...";
    
    // 设置默认文件名和路径
    QString defaultFileName = QString("user_profile_%1.pdf").arg(m_currentUserId);
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/" + defaultFileName;
    
    QString fileName = QFileDialog::getSaveFileName(
        this, 
        "导出用户画像为PDF", 
        defaultPath,
        "PDF文件 (*.pdf);;所有文件 (*.*)",
        nullptr,
        QFileDialog::DontUseNativeDialog
    );
    
    if (fileName.isEmpty()) {
        // qDebug() << "用户取消了PDF文件选择";
        return;
    }
    
    // qDebug() << "开始生成PDF文件:" << fileName;
    
    // 获取用户数据
    QString registerTime = m_registerTimeLabel ? m_registerTimeLabel->text().replace("注册时间: ", "") : "无数据";
    QString lastActive = m_lastActiveTimeLabel ? m_lastActiveTimeLabel->text().replace("最后活跃: ", "") : "无数据";
    QString totalEvents = m_totalEventsLabel ? m_totalEventsLabel->text().replace("总事件数: ", "") : "无数据";
    QString activeDays = m_activeDaysLabel ? m_activeDaysLabel->text().replace("活跃天数: ", "") : "无数据";
    
    QString activityTag = m_activityTagLabel ? m_activityTagLabel->text().replace("活跃度: ", "") : "无数据";
    QString loyaltyTag = m_loyaltyTagLabel ? m_loyaltyTagLabel->text().replace("忠诚度: ", "") : "无数据";
    QString valueTag = m_valueTagLabel ? m_valueTagLabel->text().replace("价值: ", "") : "无数据";
    
    QString visitCount = m_visitCountLabel ? m_visitCountLabel->text().replace("访问次数: ", "") : "无数据";
    QString avgStayTime = m_avgStayTimeLabel ? m_avgStayTimeLabel->text().replace("平均每日访问: ", "") : "无数据";
    QString topPages = m_topPagesLabel ? m_topPagesLabel->text().replace("常用页面: ", "") : "无数据";
    QString topFeatures = m_topFeaturesLabel ? m_topFeaturesLabel->text().replace("常用功能: ", "") : "无数据";
    
    // 创建HTML内容
    QString html = QString(R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <style>
                body { font-family: "Helvetica Neue", "Helvetica", "Arial"; margin: 20px; }
                h1 { color: #333; border-bottom: 3px solid #007bff; padding-bottom: 10px; }
                h2 { color: #666; margin-top: 30px; border-bottom: 1px solid #ddd; padding-bottom: 5px; }
                .info { background-color: #e3f2fd; padding: 15px; border-left: 6px solid #2196F3; margin-bottom: 20px; border-radius: 5px; }
                .tag-section { background-color: #f8f9fa; padding: 15px; border-radius: 8px; margin: 15px 0; }
                .tag { display: inline-block; background-color: #007bff; color: white; padding: 8px 16px; border-radius: 20px; margin: 5px; font-weight: bold; }
                .behavior-item { margin: 10px 0; padding: 10px; background-color: #ffffff; border: 1px solid #e9ecef; border-radius: 5px; }
                .label { font-weight: bold; color: #495057; }
                .value { color: #007bff; font-weight: 500; }
            </style>
        </head>
        <body>
            <h1>用户画像报告</h1>
            
            <div class="info">
                <strong>导出时间:</strong> %1<br>
                <strong>用户ID:</strong> %2
            </div>
            
            <h2>基本信息</h2>
            <div class="behavior-item">
                <span class="label">注册时间:</span> <span class="value">%3</span>
            </div>
            <div class="behavior-item">
                <span class="label">最后活跃:</span> <span class="value">%4</span>
            </div>
            <div class="behavior-item">
                <span class="label">总事件数:</span> <span class="value">%5</span>
            </div>
            <div class="behavior-item">
                <span class="label">活跃天数:</span> <span class="value">%6</span>
            </div>
            
            <h2>用户标签</h2>
            <div class="tag-section">
                <div class="tag">活跃度: %7</div>
                <div class="tag">忠诚度: %8</div>
                <div class="tag">价值: %9</div>
            </div>
            
            <h2>行为统计</h2>
            <div class="behavior-item">
                <span class="label">访问次数:</span> <span class="value">%10</span>
            </div>
            <div class="behavior-item">
                <span class="label">平均每日访问:</span> <span class="value">%11</span>
            </div>
            <div class="behavior-item">
                <span class="label">常用页面:</span> <span class="value">%12</span>
            </div>
            <div class="behavior-item">
                <span class="label">常用功能:</span> <span class="value">%13</span>
            </div>
        </body>
        </html>
    )").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
      .arg(m_currentUserId)
      .arg(registerTime)
      .arg(lastActive)
      .arg(totalEvents)
      .arg(activeDays)
      .arg(activityTag)
      .arg(loyaltyTag)
      .arg(valueTag)
      .arg(visitCount)
      .arg(avgStayTime)
      .arg(topPages)
      .arg(topFeatures);
    
    // 创建PDF文档
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(20, 20, 20, 20), QPageLayout::Millimeter);
    
    // 使用QTextDocument渲染HTML为PDF
    QTextDocument document;
    document.setHtml(html);
    document.print(&printer);
    
    // qDebug() << "PDF用户画像导出完成，文件保存到:" << fileName;
    QMessageBox::information(this, "成功", "用户画像导出成功!\n文件保存到: " + fileName);
}

void UserProfileTab::applyTheme()
{
    const auto& colors = ThemeManager::instance()->colors();
    
    // 直接为按钮设置样式，避免选择器问题
    QString primaryButtonStyle = QString(R"(
        background-color: %1;
        color: white;
        border: none;
        border-radius: %2px;
        padding: 12px 24px;
        font-weight: 500;
        font-size: %3px;
        min-width: 80px;
    )").arg(colors.PRIMARY)
       .arg(ThemeManager::BorderRadius::MD)
       .arg(ThemeManager::Typography::FONT_SIZE_SM);
    
    QString secondaryButtonStyle = QString(R"(
        background-color: %1;
        color: white;
        border: none;
        border-radius: %2px;
        padding: 12px 24px;
        font-weight: 500;
        font-size: %3px;
        min-width: 80px;
    )").arg(colors.GRAY_600)
       .arg(ThemeManager::BorderRadius::MD)
       .arg(ThemeManager::Typography::FONT_SIZE_SM);
    
    // 直接设置按钮样式
    if (m_refreshButton) m_refreshButton->setStyleSheet(primaryButtonStyle);
    if (m_exportButton) m_exportButton->setStyleSheet(primaryButtonStyle);
    if (m_prevUserButton) m_prevUserButton->setStyleSheet(secondaryButtonStyle);
    if (m_nextUserButton) m_nextUserButton->setStyleSheet(secondaryButtonStyle);
    
    // 应用信息标签样式
    QString infoLabelStyle = QString(R"(
        QLabel#infoLabel {
            color: %1;
            font-size: %2px;
            font-weight: 500;
            padding: 12px 16px;
            background-color: %3;
            border-radius: %4px;
            border-left: 4px solid %5;
            margin: 4px 0;
        }
    )").arg(colors.TEXT_PRIMARY)
       .arg(ThemeManager::Typography::FONT_SIZE_SM)
       .arg(colors.SURFACE)
       .arg(ThemeManager::BorderRadius::MD)
       .arg(colors.PRIMARY);
    
    // 应用标签样式
    QString tagLabelStyle = QString(R"(
        QLabel#tagLabel {
            padding: 12px 20px;
            border-radius: 20px;
            font-weight: 600;
            font-size: %1px;
            color: white;
            background-color: %2;
            margin: 8px;
            min-width: 100px;
        }
    )").arg(ThemeManager::Typography::FONT_SIZE_SM)
       .arg(colors.GRAY_600);
    
    // 应用统计标签样式
    QString statsLabelStyle = QString(R"(
        QLabel#statsLabel {
            color: %1;
            font-size: %2px;
            font-weight: 500;
            padding: 16px;
            background-color: %3;
            border: 1px solid %4;
            border-radius: %5px;
            margin: 4px;
        }
    )").arg(colors.TEXT_PRIMARY)
       .arg(ThemeManager::Typography::FONT_SIZE_SM)
       .arg(colors.CARD)
       .arg(colors.BORDER)
       .arg(ThemeManager::BorderRadius::MD);
    
    // 应用图表视图样式
    QString chartViewStyle = QString(R"(
        QWebEngineView#chartView {
            border: none;
            border-radius: %1px;
            background-color: %2;
        }
    )").arg(ThemeManager::BorderRadius::MD)
       .arg(colors.CARD);
    
    // 设置主窗口样式，包含所有组件样式（除了按钮）
    QString mainStyle = QString(R"(
        QWidget {
            background-color: %1;
            font-family: %2;
            color: %3;
        }
    )").arg(colors.BACKGROUND)
       .arg(ThemeManager::Typography::FONT_FAMILY)
       .arg(colors.TEXT_PRIMARY);
    
    // 组合非按钮样式并应用
    QString combinedStyle = mainStyle + infoLabelStyle + tagLabelStyle + statsLabelStyle + chartViewStyle;
    
    setStyleSheet(combinedStyle);
}

QWidget *UserProfileTab::createToolbarWidget()
{
    const auto& colors = ThemeManager::instance()->colors();
    
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setSpacing(12);
    layout->setContentsMargins(16, 12, 16, 12);
    
    // 左侧导航按钮
    QHBoxLayout *navLayout = new QHBoxLayout();
    navLayout->setSpacing(8);
    navLayout->addWidget(m_prevUserButton);
    navLayout->addWidget(m_nextUserButton);
    
    // 右侧操作按钮
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->setSpacing(8);
    actionLayout->addWidget(m_refreshButton);
    actionLayout->addWidget(m_exportButton);
    
    layout->addLayout(navLayout);
    layout->addStretch();
    layout->addLayout(actionLayout);
    
    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    widget->setStyleSheet(QString(R"(
        QWidget {
            background-color: %1;
            border-radius: %2px;
            border: 1px solid %3;
        }
    )").arg(colors.CARD)
       .arg(ThemeManager::BorderRadius::LG)
       .arg(colors.BORDER));
    return widget;
}

QWidget *UserProfileTab::createUserInfoWidget()
{
    QWidget *widget = new QWidget();
    QGridLayout *layout = new QGridLayout(widget);
    layout->setSpacing(12);
    layout->setContentsMargins(16, 16, 16, 16);
    
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
    layout->setSpacing(16);
    layout->setContentsMargins(16, 16, 16, 16);
    
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
    layout->setSpacing(12);
    layout->setContentsMargins(16, 16, 16, 16);
    
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
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(content);
    return groupBox;
}

// 新增现代卡片创建方法
QWidget *UserProfileTab::createModernCard(const QString &title, QWidget *content, bool showTitle)
{
    const auto& colors = ThemeManager::instance()->colors();
    
    QWidget *card = new QWidget();
    card->setStyleSheet(QString(R"(
        QWidget {
            background-color: %1;
            border-radius: %2px;
            border: 1px solid %3;
        }
    )").arg(colors.CARD)
       .arg(ThemeManager::BorderRadius::LG)
       .arg(colors.BORDER));
    
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);
    
    if (showTitle && !title.isEmpty()) {
        QLabel *titleLabel = new QLabel(title);
        titleLabel->setStyleSheet(QString(R"(
            QLabel {
                font-weight: 600;
                font-size: %1px;
                color: %2;
                padding: 16px 20px 8px 20px;
                background-color: transparent;
                border: none;
            }
        )").arg(ThemeManager::Typography::FONT_SIZE_LG)
           .arg(colors.TEXT_PRIMARY));
        cardLayout->addWidget(titleLabel);
    }
    
    cardLayout->addWidget(content);
    return card;
}

