#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebChannel>
#include <QVariant>

// 用于JS与Qt交互的类（必须继承QObject）
class ChartBridge : public QObject
{
    Q_OBJECT
public:
    explicit ChartBridge(QObject *parent = nullptr) : QObject(parent) {}
    
public slots:
    // 接收ECharts的点击事件（供JS调用）
    void onChartClicked(const QString& name, const QVariant& value) {
        qDebug() << "ECharts点击：" << name << "=" << value;
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 模拟按钮点击，更新ECharts数据
    void onUpdateDataClicked();

private:
    QWebEngineView *m_webView;  // 加载HTML的WebView
    ChartBridge *m_bridge;      // Qt与JS的桥接对象
};

#endif // MAINWINDOW_H