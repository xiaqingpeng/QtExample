#ifndef CONTENTTAB_H
#define CONTENTTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#ifdef WEBENGINE_AVAILABLE
#include <QWebEngineView>
#include <QWebChannel>
#endif
#include <QThread>
#include <QTimer>
#include "../Network/networkmanager.h"
#include "../Styles/theme_manager.h"

class ServerConfigBridge;

class ContentTab : public QWidget
{
    Q_OBJECT

public:
    ContentTab(QWidget *parent = nullptr);
    ~ContentTab();

private slots:
    void applyTheme();
    void onPageLoaded(bool ok);
    void updateCharts(const QJsonObject &data);
    void refreshSystemInfo();

private:
    NetworkManager *m_networkManager;
    QLabel *m_titleLabel;
    // 使用 QWidget* 作为基类指针，支持运行时类型检查
    QWidget *m_webView;
#ifdef WEBENGINE_AVAILABLE
    QWebChannel *m_channel;
#else
    QObject *m_channel; // 占位符
#endif
    ServerConfigBridge *m_bridge;
    bool m_pageLoaded;
    QJsonObject m_pendingData;
    QTimer *m_refreshTimer;
    
    void setupUI();
    void fetchSystemInfo();
};

#endif // CONTENTTAB_H