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
    m_webView->load(QUrl::fromLocalFile(htmlPath));

    // 6. 创建测试按钮
    QPushButton *btnUpdate = new QPushButton("更新图表数据", this);
    QPushButton *btnFetchApi = new QPushButton("获取API数据", this);
    layout->addWidget(btnUpdate);
    layout->addWidget(btnFetchApi);
    
    connect(btnUpdate, &QPushButton::clicked, this, &EChartsTab::onUpdateDataClicked);
    connect(btnFetchApi, &QPushButton::clicked, this, &EChartsTab::fetchApiData);
    
    // 7. 自动获取API数据（每30秒更新一次）
    QTimer *apiTimer = new QTimer(this);
    connect(apiTimer, &QTimer::timeout, this, &EChartsTab::fetchApiData);
    apiTimer->start(30000); // 30秒
    
    // 8. 初始加载API数据
    fetchApiData();
}

EChartsTab::~EChartsTab()
{
}

void EChartsTab::onUpdateDataClicked()
{
    // 模拟新数据
    QList<int> newData = {10, 30, 20, 15, 25, 18};

    // 将QList<int>转换为JavaScript数组格式
    QString jsArray = "[";
    for (int i = 0; i < newData.size(); ++i) {
        if (i > 0) jsArray += ",";
        jsArray += QString::number(newData[i]);
    }
    jsArray += "]";

    // 调用JS的updateChartData函数，传递数据给ECharts
    m_webView->page()->runJavaScript(QString("updateChartData(%1)").arg(jsArray));

    // 可选：更新X轴类别
    m_webView->page()->runJavaScript("updateXAxis(['苹果', '香蕉', '橙子', '葡萄', '芒果', '西瓜'])");
}

void EChartsTab::fetchApiData()
{
    qDebug() << "开始请求API数据...";
    
    QNetworkRequest request(QUrl("http://120.48.95.51:7001/system/logs/stats"));
    request.setRawHeader("Content-Type", "application/json");
    
    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, &EChartsTab::onApiDataReceived);
    connect(reply, &QNetworkReply::errorOccurred,
            [reply](QNetworkReply::NetworkError error) {
                qDebug() << "API请求错误:" << error << reply->errorString();
                reply->deleteLater();
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