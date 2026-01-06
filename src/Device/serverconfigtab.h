#ifndef SERVERCONFIGTAB_H
#define SERVERCONFIGTAB_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebChannel>
#include <QVariant>
#include <QLabel>
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

private:
    QWebEngineView *m_webView;
    QWebChannel *m_channel;
    ServerConfigBridge *m_bridge;
    NetworkManager *m_networkManager;
    QLabel *m_titleLabel;
    
    void fetchSystemInfo();
    void updateCharts(const QJsonObject &data);
};

#endif // SERVERCONFIGTAB_H