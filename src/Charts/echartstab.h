#ifndef ECHARTSTAB_H
#define ECHARTSTAB_H

#include <QMainWindow>
#ifdef WEBENGINE_AVAILABLE
#include <QWebEngineView>
#include <QWebChannel>
#endif
#include <QVariant>
#include "networkmanager.h"
#include "theme_manager.h"
// #include <QTimer>  // 已禁用定时器功能
#include <QComboBox>
#include <QDateTimeEdit>
#include <QPushButton>

// 用于JS与Qt交互的类（必须继承QObject）
class ChartBridge : public QObject
{
    Q_OBJECT
public:
    explicit ChartBridge(QObject *parent = nullptr) : QObject(parent) {}
    
public slots:
    // 接收ECharts的点击事件（供JS调用）
    void onChartClicked(const QString& name, const QVariant& value) {
        Q_UNUSED(name);
        Q_UNUSED(value);
        // ECharts点击事件
    }
};

class EChartsTab : public QMainWindow
{
    Q_OBJECT

public:
    EChartsTab(QWidget *parent = nullptr);
    ~EChartsTab();

private slots:
    // 当筛选条件改变时重新获取数据
    void onFilterChanged();
    // 当时间筛选条件改变时重新获取数据
    void onTimeFilterChanged();
    // 快捷时间按钮点击事件
    void onTimeShortcutClicked(int days);
    // WebView页面加载完成后获取API数据
    void onPageLoaded(bool ok);
    // 图表类型切换事件
    void onChartTypeChanged(int index);
    // 主题变化事件
    void applyTheme();

private:
#ifdef WEBENGINE_AVAILABLE
    QWebEngineView *m_webView;  // 加载HTML的WebView
    QWebChannel *m_channel;
#else
    QLabel *m_webView;  // 使用QLabel作为占位符
    QObject *m_channel; // 占位符
#endif
    ChartBridge *m_bridge;      // Qt与JS的桥接对象
    NetworkManager *m_networkManager;
    // QTimer *m_apiTimer;         // API自动更新定时器 - 已禁用
    
    // 筛选控件
    QComboBox *m_methodCombo;   // 方法选择下拉框
    QComboBox *m_platformCombo; // 平台选择下拉框
    QComboBox *m_chartTypeCombo; // 图表类型选择下拉框
    QDateTimeEdit *m_startTimeEdit; // 开始时间选择器
    QDateTimeEdit *m_endTimeEdit;   // 结束时间选择器
    
    // 快捷时间按钮
    QPushButton *m_btnToday;
    QPushButton *m_btnYesterday;
    QPushButton *m_btnLast7Days;
    QPushButton *m_btnLast30Days;
    QPushButton *m_btnClearTime;
    
    // 当前高亮的天数（用于主题切换时保持状态）
    int m_currentHighlightedDays;
    
    // API请求方法
    void fetchApiData();
    // 更新按钮高亮状态
    void updateButtonHighlight(int days);
    
    // JSON数据转换方法
    QString jsonArrayToString(const QStringList &list);
    QString jsonArrayToString(const QList<int> &list);
    QString jsonArrayToString(const QList<double> &list);
};

#endif // ECHARTSTAB_H