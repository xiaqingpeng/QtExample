#include "echartstab.h"
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QHBoxLayout>

EChartsTab::EChartsTab(QWidget *parent) : QWidget(parent)
{
    // 创建主布局
    mainLayout = new QVBoxLayout(this);
    
    // 创建标签页控件
    chartTabs = new QTabWidget(this);
    mainLayout->addWidget(chartTabs);
    
    // 创建各种图表
    createBarChart();
    createLineChart();
    createPieChart();
    createScatterChart();
    
    // 设置布局
    setLayout(mainLayout);
}

EChartsTab::~EChartsTab()
{
    // 清理资源
}

void EChartsTab::createBarChart()
{
    // 创建WebEngineView
    barChartView = new QWebEngineView(this);
    
    // 启用JavaScript
    QWebEngineSettings *settings = barChartView->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    
    // 创建HTML内容
    QString html = R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <title>ECharts 柱状图示例</title>
            <script src="https://cdn.jsdelivr.net/npm/echarts@5.4.3/dist/echarts.min.js"></script>
        </head>
        <body>
            <div id="chart" style="width: 100%; height: 500px;"></div>
            <script>
                // 初始化图表
                var chart = echarts.init(document.getElementById('chart'));
                
                // 配置选项
                var option = {
                    title: {
                        text: '柱状图示例'
                    },
                    tooltip: {},
                    legend: {
                        data: ['销量']
                    },
                    xAxis: {
                        data: ['衬衫', '羊毛衫', '雪纺衫', '裤子', '高跟鞋', '袜子']
                    },
                    yAxis: {},
                    series: [{
                        name: '销量',
                        type: 'bar',
                        data: [5, 20, 36, 10, 10, 20]
                    }]
                };
                
                // 设置选项
                chart.setOption(option);
                
                // 响应窗口大小变化
                window.addEventListener('resize', function() {
                    chart.resize();
                });
            </script>
        </body>
        </html>
    )";
    
    // 加载HTML内容
    barChartView->setHtml(html);
    
    // 添加到标签页
    chartTabs->addTab(barChartView, "柱状图");
}

void EChartsTab::createLineChart()
{
    // 创建WebEngineView
    lineChartView = new QWebEngineView(this);
    
    // 启用JavaScript
    QWebEngineSettings *settings = lineChartView->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    
    // 创建HTML内容
    QString html = R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <title>ECharts 折线图示例</title>
            <script src="https://cdn.jsdelivr.net/npm/echarts@5.4.3/dist/echarts.min.js"></script>
        </head>
        <body>
            <div id="chart" style="width: 100%; height: 500px;"></div>
            <script>
                // 初始化图表
                var chart = echarts.init(document.getElementById('chart'));
                
                // 配置选项
                var option = {
                    title: {
                        text: '折线图示例'
                    },
                    tooltip: {},
                    legend: {
                        data: ['销量']
                    },
                    xAxis: {
                        data: ['周一', '周二', '周三', '周四', '周五', '周六', '周日']
                    },
                    yAxis: {},
                    series: [{
                        name: '销量',
                        type: 'line',
                        data: [150, 230, 224, 218, 135, 147, 260]
                    }]
                };
                
                // 设置选项
                chart.setOption(option);
                
                // 响应窗口大小变化
                window.addEventListener('resize', function() {
                    chart.resize();
                });
            </script>
        </body>
        </html>
    )";
    
    // 加载HTML内容
    lineChartView->setHtml(html);
    
    // 添加到标签页
    chartTabs->addTab(lineChartView, "折线图");
}

void EChartsTab::createPieChart()
{
    // 创建WebEngineView
    pieChartView = new QWebEngineView(this);
    
    // 启用JavaScript
    QWebEngineSettings *settings = pieChartView->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    
    // 创建HTML内容
    QString html = R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <title>ECharts 饼图示例</title>
            <script src="https://cdn.jsdelivr.net/npm/echarts@5.4.3/dist/echarts.min.js"></script>
        </head>
        <body>
            <div id="chart" style="width: 100%; height: 500px;"></div>
            <script>
                // 初始化图表
                var chart = echarts.init(document.getElementById('chart'));
                
                // 配置选项
                var option = {
                    title: {
                        text: '饼图示例',
                        left: 'center'
                    },
                    tooltip: {
                        trigger: 'item'
                    },
                    legend: {
                        orient: 'vertical',
                        left: 'left'
                    },
                    series: [{
                        name: '访问来源',
                        type: 'pie',
                        radius: '50%',
                        data: [
                            {value: 1048, name: '搜索引擎'},
                            {value: 735, name: '直接访问'},
                            {value: 580, name: '邮件营销'},
                            {value: 484, name: '联盟广告'},
                            {value: 300, name: '视频广告'}
                        ],
                        emphasis: {
                            itemStyle: {
                                shadowBlur: 10,
                                shadowOffsetX: 0,
                                shadowColor: 'rgba(0, 0, 0, 0.5)'
                            }
                        }
                    }]
                };
                
                // 设置选项
                chart.setOption(option);
                
                // 响应窗口大小变化
                window.addEventListener('resize', function() {
                    chart.resize();
                });
            </script>
        </body>
        </html>
    )";
    
    // 加载HTML内容
    pieChartView->setHtml(html);
    
    // 添加到标签页
    chartTabs->addTab(pieChartView, "饼图");
}

void EChartsTab::createScatterChart()
{
    // 创建WebEngineView
    scatterChartView = new QWebEngineView(this);
    
    // 启用JavaScript
    QWebEngineSettings *settings = scatterChartView->settings();
    settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    
    // 创建HTML内容
    QString html = R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <title>ECharts 散点图示例</title>
            <script src="https://cdn.jsdelivr.net/npm/echarts@5.4.3/dist/echarts.min.js"></script>
        </head>
        <body>
            <div id="chart" style="width: 100%; height: 500px;"></div>
            <script>
                // 初始化图表
                var chart = echarts.init(document.getElementById('chart'));
                
                // 生成随机数据
                var data = [];
                for (var i = 0; i < 100; i++) {
                    data.push([
                        Math.random() * 100,
                        Math.random() * 100
                    ]);
                }
                
                // 配置选项
                var option = {
                    title: {
                        text: '散点图示例'
                    },
                    tooltip: {},
                    xAxis: {
                        type: 'value'
                    },
                    yAxis: {
                        type: 'value'
                    },
                    series: [{
                        name: '数据点',
                        type: 'scatter',
                        data: data
                    }]
                };
                
                // 设置选项
                chart.setOption(option);
                
                // 响应窗口大小变化
                window.addEventListener('resize', function() {
                    chart.resize();
                });
            </script>
        </body>
        </html>
    )";
    
    // 加载HTML内容
    scatterChartView->setHtml(html);
    
    // 添加到标签页
    chartTabs->addTab(scatterChartView, "散点图");
}