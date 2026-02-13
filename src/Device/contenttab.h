#ifndef CONTENTTAB_H
#define CONTENTTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QJsonObject>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include "../Services/ApiService.h"
#include "../Charts/gaugewidget.h"
#include "../Styles/theme_manager.h"

class ContentTab : public QWidget
{
    Q_OBJECT

public:
    ContentTab(QWidget *parent = nullptr);
    ~ContentTab();

private slots:
    void applyTheme();
    void updateCharts(const QJsonObject &data);
    void refreshSystemInfo();

private:
    ApiService *m_apiService;
    QTimer *m_refreshTimer;
    
    // 系统信息标签
    QLabel *m_serverIpLabel;
    QLabel *m_osInfoLabel;
    QLabel *m_uptimeLabel;
    
    // Gauge 图表（CPU、内存、磁盘）
    GaugeWidget *m_cpuGauge;
    GaugeWidget *m_memoryGauge;
    GaugeWidget *m_diskGauge;
    
    // 柱状图（系统负载、网络流量）
    QChartView *m_loadChartView;
    QChartView *m_networkChartView;
    
    QChart *m_loadChart;
    QChart *m_networkChart;
    
    QBarSeries *m_loadSeries;
    QBarSeries *m_networkSeries;
    
    void setupUI();
    void fetchSystemInfo();
    void updateGauge(GaugeWidget *gauge, double value, const QStringList &details);
    void updateBarChart(QChart *chart, QBarSeries *series, const QList<double> &data, const QStringList &categories, const QString &title);
};

#endif // CONTENTTAB_H