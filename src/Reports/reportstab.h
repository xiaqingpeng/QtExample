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
#ifdef WEBENGINE_AVAILABLE
#ifdef WEBENGINE_AVAILABLE
#include <QWebEngineView>
#endif
#endif
#include <QJsonObject>
#include <QJsonArray>
#include "../Styles/theme_manager.h"

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

public slots:
    // 主题应用
    void applyTheme();

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
    void updateRealTimeStats(const QJsonArray &stats);
    
    // 图表渲染方法
    void renderTrendChart(const QJsonArray &trendData, const QString &title, const QString &chartType);
    QWidget *createToolbarWidget();
    QWidget *createKeyMetricsWidget();
    QWidget *createTrendChartsWidget();
    QWidget *createTopPagesWidget();
    QWidget *createTopEventsWidget();
    QWidget *createTopUsersWidget();
    QWidget *createRealTimeStatsWidget();
    QGroupBox *createGroupBox(const QString &title, QWidget *content);
    QWidget *createModernCard(const QString &title, QWidget *content, bool showTitle = true);
    QWidget *createSeparator();
    
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
    QLabel *m_totalEventsLabel;
    
    // 趋势图表组件
#ifdef WEBENGINE_AVAILABLE
    QWebEngineView *m_trendChartView;
    QWebEngineView *m_activityChartView;
#else
    QLabel *m_trendChartView;  // 使用QLabel作为占位符
    QLabel *m_activityChartView;
#endif
    
    // 排行榜表格组件
    QTableWidget *m_topPagesTable;
    QTableWidget *m_topEventsTable;
    QTableWidget *m_topUsersTable;
};

#endif // REPORTSTAB_H