#ifndef ECHARTSTAB_H
#define ECHARTSTAB_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebChannel>
#include <QVariant>
#include "networkmanager.h"
#include <QTimer>
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
        qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]" << "ECharts点击：" << name << "=" << value;
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

private:
    QWebEngineView *m_webView;  // 加载HTML的WebView
    QWebChannel *m_channel;
    ChartBridge *m_bridge;      // Qt与JS的桥接对象
    NetworkManager *m_networkManager;
    QTimer *m_apiTimer;         // API自动更新定时器
    
    // 筛选控件
    QComboBox *m_methodCombo;   // 方法选择下拉框
    QComboBox *m_platformCombo; // 平台选择下拉框
    QDateTimeEdit *m_startTimeEdit; // 开始时间选择器
    QDateTimeEdit *m_endTimeEdit;   // 结束时间选择器
    
    // 快捷时间按钮
    QPushButton *m_btnToday;
    QPushButton *m_btnYesterday;
    QPushButton *m_btnLast7Days;
    QPushButton *m_btnLast30Days;
    QPushButton *m_btnClearTime;
    
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