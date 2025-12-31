#ifndef REPORTSTAB_H
#define REPORTSTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QDateEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QWebEngineView>
#include <QJsonObject>
#include <QJsonArray>

class ReportsTab : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsTab(QWidget *parent = nullptr);
    ~ReportsTab();

private slots:
    void onReportTypeChanged(int index);
    void onChartTypeChanged(int index);
    void onDateRangeChanged();
    void exportReport();
    void exportToCSV();
    void exportToExcel();
    void exportToPDF();

private:
    // UI初始化方法
    void setupUI();
    void setupToolbar();
    void setupKeyMetrics();
    void setupTrendCharts();
    void setupTopRankings();
    void setupRealTimeStats();
    
    // 数据加载方法
    void refreshReports();
    void loadActivityStats();
    void loadRetentionStats();
    void loadPageViewStats();
    void loadEventStats();
    void loadTrendAnalysis();
    void loadTopPages();
    void loadTopEvents();
    void loadTopUsers();
    void loadRealTimeStats();
    
    // UI更新方法
    void updateKeyMetricsDisplay(const QJsonObject &metrics);
    void updateTrendChart(const QJsonArray &trendData, const QString &title);
    void updateTopPagesTable(const QJsonArray &pages);
    void updateTopEventsTable(const QJsonArray &events);
    void updateTopUsersTable(const QJsonArray &users);
    void updateRealTimeStats(const QJsonObject &stats);
    
    // 图表渲染方法
    void renderTrendChart(const QJsonArray &trendData, const QString &title, const QString &chartType);
    
    // 辅助方法
    QWidget *createToolbarWidget();
    QWidget *createKeyMetricsWidget();
    QWidget *createTrendChartsWidget();
    QWidget *createTopPagesWidget();
    QWidget *createTopEventsWidget();
    QWidget *createTopUsersWidget();
    QWidget *createRealTimeStatsWidget();
    QGroupBox *createGroupBox(const QString &title, QWidget *content);
    
    // 工具栏组件
    QDateEdit *m_startDateEdit;
    QDateEdit *m_endDateEdit;
    QComboBox *m_reportTypeCombo;
    QComboBox *m_chartTypeCombo;
    QPushButton *m_refreshButton;
    QPushButton *m_exportButton;
    QPushButton *m_exportCSVButton;
    QPushButton *m_exportExcelButton;
    QPushButton *m_exportPDFButton;
    
    // 关键指标显示组件
    QLabel *m_dauLabel;
    QLabel *m_mauLabel;
    QLabel *m_retentionLabel;
    QLabel *m_conversionLabel;
    QLabel *m_onlineUsersLabel;
    QLabel *m_todayEventsLabel;
    
    // 趋势图表组件
    QWebEngineView *m_trendChartView;
    QWebEngineView *m_activityChartView;
    
    // 排行榜表格组件
    QTableWidget *m_topPagesTable;
    QTableWidget *m_topEventsTable;
    QTableWidget *m_topUsersTable;
};

#endif // REPORTSTAB_H