#include "echartstab.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QWebChannel>
#include <QWebEngineView>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
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

    // 6. 创建测试按钮：更新ECharts数据
    QPushButton *btnUpdate = new QPushButton("更新图表数据", this);
    layout->addWidget(btnUpdate);
    connect(btnUpdate, &QPushButton::clicked, this, &MainWindow::onUpdateDataClicked);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onUpdateDataClicked()
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

    // 调用JS的updateChart函数，传递数据给ECharts
    m_webView->page()->runJavaScript(QString("updateChart(%1)").arg(jsArray));

    // 可选：更新X轴类别
    m_webView->page()->runJavaScript("updateXAxis(['苹果', '香蕉', '橙子', '葡萄', '芒果', '西瓜'])");
}