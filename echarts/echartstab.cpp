#include "echartstab.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QWebChannel>
#include <QWebEngineView>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QList>

EChartsTab::EChartsTab(QWidget *parent)
    : QMainWindow(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    // 1. 设置窗口基本属性
    this->setWindowTitle("Qt + ECharts Demo");
    this->resize(800, 600);

    // 2. 创建中心部件和布局
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    this->setCentralWidget(centralWidget);

    // 3. 创建WebEngineView，加载ECharts HTML页面
    m_webView = new QWebEngineView(this);
    layout->addWidget(m_webView);

    // 4. 创建桥接对象，实现Qt与JS交互
    m_bridge = new ChartBridge(this);
    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject("qt", m_bridge);  // 注册为window.qt对象（供JS调用）
    m_webView->page()->setWebChannel(channel);

    // 5. 加载本地HTML文件（使用绝对路径确保文件能被找到）
    QString htmlPath = "/Applications/qingpengxia/qt/qt6/example/echarts/ECharts/chart.html";
    qDebug() << "HTML文件路径:" << htmlPath;
    QFile file(htmlPath);
    if (!file.exists()) {
        qDebug() << "HTML文件不存在:" << htmlPath;
        return;
    }
    
    // 连接WebView加载完成信号
    connect(m_webView, &QWebEngineView::loadFinished, this, &EChartsTab::onPageLoaded);
    
    m_webView->load(QUrl::fromLocalFile(htmlPath));

    // 6. 创建筛选控件
    QHBoxLayout *filterLayout = new QHBoxLayout();
    
    // Method选择下拉框
    m_methodCombo = new QComboBox(this);
    m_methodCombo->addItem("All", "");  // All表示不筛选
    m_methodCombo->addItem("Post", "Post");
    m_methodCombo->addItem("Get", "Get");
    m_methodCombo->setCurrentIndex(0);  // 默认选择All
    filterLayout->addWidget(new QLabel("Method:"));
    filterLayout->addWidget(m_methodCombo);
    
    // Platform选择下拉框
    m_platformCombo = new QComboBox(this);
    m_platformCombo->addItem("All", "");  // All表示不筛选
    m_platformCombo->addItem("Web", "web");
    m_platformCombo->addItem("Android", "android");
    m_platformCombo->addItem("IOS", "ios");
    m_platformCombo->addItem("Windows", "windows");
    m_platformCombo->addItem("Mac", "mac");
    m_platformCombo->setCurrentIndex(0);  // 默认选择All
    filterLayout->addWidget(new QLabel("Platform:"));
    filterLayout->addWidget(m_platformCombo);
    
    layout->addLayout(filterLayout);
    
    // 7. 创建API数据获取按钮
    QPushButton *btnFetchApi = new QPushButton("获取API数据", this);
    layout->addWidget(btnFetchApi);
    
    connect(btnFetchApi, &QPushButton::clicked, this, &EChartsTab::fetchApiData);
    
    // 连接筛选条件改变信号
    connect(m_methodCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &EChartsTab::onFilterChanged);
    connect(m_platformCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &EChartsTab::onFilterChanged);
    
    // 创建定时器（但不启动，等待首次手动获取数据后才启动）
    m_apiTimer = new QTimer(this);
    connect(m_apiTimer, &QTimer::timeout, this, &EChartsTab::fetchApiData);
}

EChartsTab::~EChartsTab()
{
}

void EChartsTab::onFilterChanged()
{
    // 当筛选条件改变时，自动重新获取数据
    qDebug() << "筛选条件已改变，重新获取数据...";
    fetchApiData();
}

void EChartsTab::onPageLoaded(bool ok)
{
    if (ok) {
        qDebug() << "WebView页面加载完成，开始获取API数据";
        // 页面加载完成后，延迟一小段时间确保JavaScript环境准备好
        QTimer::singleShot(500, this, &EChartsTab::fetchApiData);
    } else {
        qDebug() << "WebView页面加载失败";
    }
}

void EChartsTab::fetchApiData()
{
    qDebug() << "开始请求API数据...";
    
    // 如果定时器未启动，则启动定时器（首次手动获取数据后）
    if (!m_apiTimer->isActive()) {
        m_apiTimer->start(30000); // 30秒
        qDebug() << "启动自动更新定时器";
    }
    
    // 从下拉框获取筛选条件
    QString method = m_methodCombo->currentData().toString();
    QString platform = m_platformCombo->currentData().toString();
    
    // 构建带筛选条件的API URL
    QString apiUrl = "http://120.48.95.51:7001/system/logs/stats";
    
    // 添加查询参数（只添加非空的参数）
    QList<QString> params;
    if (!method.isEmpty()) {
        params.append(QString("method=%1").arg(method));
    }
    if (!platform.isEmpty()) {
        params.append(QString("platform=%1").arg(platform));
    }
    
    if (!params.isEmpty()) {
        apiUrl += "?" + params.join("&");
    }
    
    qDebug() << "API URL:" << apiUrl;
    
    QNetworkRequest request{QUrl(apiUrl)};
    request.setRawHeader("Content-Type", "application/json");
    
    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, &EChartsTab::onApiDataReceived);
    connect(reply, &QNetworkReply::errorOccurred,
            [reply](QNetworkReply::NetworkError error) {
                qDebug() << "API请求错误:" << error << reply->errorString();
                // 不在这里删除reply，让onApiDataReceived统一处理
            });
}

void EChartsTab::onApiDataReceived()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        
        if (doc.isObject()) {
            QJsonObject rootObj = doc.object();
            if (rootObj["code"].toInt() == 0) {
                QJsonArray rows = rootObj["rows"].toArray();
                
                // 统计各路径的访问次数和响应时间
                QMap<QString, int> pathCounts;
                QMap<QString, QList<int>> pathDurations;
                
                for (const QJsonValue &rowValue : rows) {
                    QJsonObject rowObj = rowValue.toObject();
                    QString path = rowObj["path"].toString();
                    int duration = rowObj["durationMs"].toInt();
                    
                    pathCounts[path]++;
                    pathDurations[path].append(duration);
                }
                
                // 准备图表数据
                QStringList categories;
                QList<int> counts;
                QList<double> avgDurations;
                
                // 按访问次数排序，取前10个
                QList<QPair<QString, int>> sortedPaths;
                for (auto it = pathCounts.begin(); it != pathCounts.end(); ++it) {
                    sortedPaths.append(qMakePair(it.key(), it.value()));
                }
                std::sort(sortedPaths.begin(), sortedPaths.end(), 
                         [](const QPair<QString, int> &a, const QPair<QString, int> &b) {
                             return a.second > b.second;
                         });
                
                int maxItems = qMin(10, sortedPaths.size());
                for (int i = 0; i < maxItems; ++i) {
                    QString path = sortedPaths[i].first;
                    categories.append(path.length() > 20 ? path.left(20) + "..." : path);
                    counts.append(sortedPaths[i].second);
                    
                    // 计算平均响应时间
                    QList<int> durations = pathDurations[path];
                    double avgDuration = 0;
                    for (int d : durations) {
                        avgDuration += d;
                    }
                    avgDuration /= durations.size();
                    avgDurations.append(avgDuration);
                }
                
                // 生成JavaScript代码更新图表
                QString jsCode = QString(
                    "if (window.updateApiChart) {"
                    "    window.updateApiChart(%1, %2, %3);"
                    "} else {"
                    "    console.log('updateApiChart函数未找到');"
                    "}"
                ).arg(jsonArrayToString(categories))
                 .arg(jsonArrayToString(counts))
                 .arg(jsonArrayToString(avgDurations));
                
                m_webView->page()->runJavaScript(jsCode);
                qDebug() << "API数据已更新到图表";
                
            } else {
                qDebug() << "API返回错误:" << rootObj["msg"].toString();
            }
        } else {
            qDebug() << "API返回的不是有效的JSON格式";
        }
    } else {
        qDebug() << "API请求失败:" << reply->errorString();
        // 调用JavaScript显示网络错误提示
        QString jsCode = QString(
            "if (window.showNetworkErrorMessage) {"
            "    window.showNetworkErrorMessage();"
            "} else {"
            "    console.log('showNetworkErrorMessage函数未找到');"
            "}"
        );
        m_webView->page()->runJavaScript(jsCode);
    }
    
    reply->deleteLater();
}

QString EChartsTab::jsonArrayToString(const QStringList &list)
{
    QString result = "[";
    for (int i = 0; i < list.size(); ++i) {
        if (i > 0) result += ",";
        result += "\"" + list[i] + "\"";
    }
    result += "]";
    return result;
}

QString EChartsTab::jsonArrayToString(const QList<int> &list)
{
    QString result = "[";
    for (int i = 0; i < list.size(); ++i) {
        if (i > 0) result += ",";
        result += QString::number(list[i]);
    }
    result += "]";
    return result;
}

QString EChartsTab::jsonArrayToString(const QList<double> &list)
{
    QString result = "[";
    for (int i = 0; i < list.size(); ++i) {
        if (i > 0) result += ",";
        result += QString::number(list[i], 'f', 2);
    }
    result += "]";
    return result;
}