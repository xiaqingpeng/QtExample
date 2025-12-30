#ifndef REPORTSTAB_H
#define REPORTSTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QTableWidget>
#include <QLineEdit>

class ReportsTab : public QWidget
{
    Q_OBJECT

public:
    explicit ReportsTab(QWidget *parent = nullptr);

private slots:
    void onGenerateReport();
    void onExportReport();
    void onChartTypeChanged(int index);
    void onSearchTextChanged(const QString &text);
    void onPrevPage();
    void onNextPage();

private:
    void setupUI();
    void setupReportSummary();
    void setupTrendChart();
    void setupDataTable();
    void generateMockReport();
    void updateChart(const QString &chartType);
    void updateDataTable();
    void updatePaginationControls();
    void filterDataTable(const QString &text);

    // UI组件
    QVBoxLayout *mainLayout;
    QHBoxLayout *toolbarLayout;
    QScrollArea *scrollArea;
    QWidget *scrollWidget;
    QVBoxLayout *scrollLayout;

    // 工具栏组件
    QComboBox *reportTypeCombo;
    QDateEdit *startDateEdit;
    QDateEdit *endDateEdit;
    QPushButton *generateButton;
    QPushButton *exportButton;

    // 报表摘要组件
    QGroupBox *summaryGroup;
    QLabel *totalVisitsLabel;
    QLabel *activeUsersLabel;
    QLabel *avgSessionLabel;
    QLabel *conversionLabel;

    // 趋势图表组件
    QGroupBox *trendGroup;
    QComboBox *chartTypeCombo;
    QWidget *chartWidget;

    // 数据表格组件
    QGroupBox *tableGroup;
    QLineEdit *searchEdit;
    QTableWidget *dataTable;
    QLabel *currentPageLabel;
    QPushButton *prevButton;
    QPushButton *nextButton;

    // 分页状态
    int currentPage = 1;
    int totalPages = 1;
};

#endif // REPORTSTAB_H