#ifndef CONTENTTAB_H
#define CONTENTTAB_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWebEngineView>
#include <QWebChannel>
#include "networkmanager.h"
#include "theme_manager.h"

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

private:
    NetworkManager *m_networkManager;
    QLabel *m_titleLabel;
    QWebEngineView *m_webView;
    QWebChannel *m_channel;
    ServerConfigBridge *m_bridge;
    
    void setupUI();
    void fetchSystemInfo();
    void updateCharts(const QJsonObject &data);
};

#endif // CONTENTTAB_H