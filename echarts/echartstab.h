#ifndef ECHARTSTAB_H
#define ECHARTSTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTabWidget>

// 注意：需要在项目中启用Qt WebEngine模块
class QWebEngineView;

class EChartsTab : public QWidget
{
    Q_OBJECT

public:
    explicit EChartsTab(QWidget *parent = nullptr);
    ~EChartsTab() override;

private:
    void createBarChart();
    void createLineChart();
    void createPieChart();
    void createScatterChart();

    QVBoxLayout *mainLayout;
    QTabWidget *chartTabs;
    QWebEngineView *barChartView;
    QWebEngineView *lineChartView;
    QWebEngineView *pieChartView;
    QWebEngineView *scatterChartView;
};

#endif // ECHARTSTAB_H