#ifndef SERVERCONFIGTAB_H
#define SERVERCONFIGTAB_H

#include <QMainWindow>
#ifdef WEBENGINE_AVAILABLE
#include <QWebEngineView>
#include <QWebChannel>
#endif
#include <QVariant>
#include <QLabel>
#include <QTimer>
#include "networkmanager.h"
#include "theme_manager.h"

class ServerConfigBridge : public QObject
{
    Q_OBJECT
public:
    explicit ServerConfigBridge(QObject *parent = nullptr) : QObject(parent) {}
    
public slots:
    void onChartClicked(const QString& name, const QVariant& value) {
        Q_UNUSED(name);
        Q_UNUSED(value);
    }
    
    void refreshSystemInfo() {
        emit refreshRequested();
    }
    
signals:
    void refreshRequested();
};

class ServerConfigTab : public QWidget
{
    Q_OBJECT

public:
    ServerConfigTab(QWidget *parent = nullptr);
    ~ServerConfigTab();

private slots:
    void onPageLoaded(bool ok);
    void applyTheme();
    void refreshSystemInfo();

private:
#ifdef WEBENGINE_AVAILABLE
    QWebEngineView *m_webView;
    QWebChannel *m_channel;
#else
    QLabel *m_webView;  // 使用QLabel作为占位符
    QObject *m_channel; // 占位符
#endif
    ServerConfigBridge *m_bridge;
    NetworkManager *m_networkManager;
    QLabel *m_titleLabel;
    QTimer *m_refreshTimer;
    
    void fetchSystemInfo();
    void updateCharts(const QJsonObject &data);
};

#endif // SERVERCONFIGTAB_H