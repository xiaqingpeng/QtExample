#ifndef CONTENTTAB_H
#define CONTENTTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWebEngineView>
#include <QWebChannel>
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
    QWebEngineView *m_webView;
    QWebChannel *m_channel;
    ServerConfigBridge *m_bridge;
    bool m_pageLoaded;
    QJsonObject m_pendingData;
    QTimer *m_refreshTimer;
    
    void setupUI();
    void fetchSystemInfo();
};

#endif // CONTENTTAB_H