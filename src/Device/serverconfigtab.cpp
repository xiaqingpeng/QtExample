#include "serverconfigtab.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QJsonDocument>
#include <QJsonObject>

ServerConfigTab::ServerConfigTab(QWidget *parent)
    : QMainWindow(parent)
    , m_webView(nullptr)
    , m_channel(nullptr)
    , m_bridge(nullptr)
    , m_networkManager(new NetworkManager(this))
{
    setWindowTitle("服务器配置");
    resize(1200, 800);
    
    // 创建中央控件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);
    
    // 标题
    m_titleLabel = new QLabel("服务器配置", this);
    m_titleLabel->setObjectName("titleLabel");
    m_titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; padding: 10px 0;");
    mainLayout->addWidget(m_titleLabel);
    
    // 创建WebView用于显示图表
    m_webView = new QWebEngineView(this);
    m_webView->setObjectName("webView");
    
    // 创建WebChannel用于Qt与JS通信
    m_channel = new QWebChannel(this);
    m_bridge = new ServerConfigBridge(this);
    m_channel->registerObject("qtBridge", m_bridge);
    m_webView->page()->setWebChannel(m_channel);
    
    // 加载HTML文件
    QString htmlPath = "qrc:/src/ECharts/server_config.html";
    m_webView->load(QUrl(htmlPath));
    
    // 连接页面加载完成信号
    connect(m_webView, &QWebEngineView::loadFinished, this, &ServerConfigTab::onPageLoaded);
    
    mainLayout->addWidget(m_webView);
    
    // 应用主题
    applyTheme();
}

ServerConfigTab::~ServerConfigTab()
{
}

void ServerConfigTab::onPageLoaded(bool ok)
{
    if (ok) {
        qDebug() << "Server config page loaded successfully";
        // 页面加载完成后获取系统信息
        fetchSystemInfo();
    } else {
        qWarning() << "Failed to load server config page";
    }
}

void ServerConfigTab::fetchSystemInfo()
{
    qDebug() << "Fetching system info...";
    
    m_networkManager->get("/system/info", [this](const QJsonObject &response) {
        if (response["code"].toInt() == 0) {
            QJsonObject data = response["data"].toObject();
            qDebug() << "System info received:" << data;
            updateCharts(data);
        } else {
            qWarning() << "Failed to fetch system info:" << response["msg"].toString();
        }
    }, [](const QString &error) {
        qWarning() << "Network error fetching system info:" << error;
    });
}

void ServerConfigTab::updateCharts(const QJsonObject &data)
{
    if (!m_webView || !m_webView->page()) {
        qWarning() << "WebView not available";
        return;
    }
    
    QString script = QString("updateServerCharts(%1);").arg(QJsonDocument(data).toJson(QJsonDocument::Compact));
    m_webView->page()->runJavaScript(script);
}

void ServerConfigTab::applyTheme()
{
    ThemeManager *theme = ThemeManager::instance();
    
    QString styleSheet = QString(
        "QWidget { "
        "    background-color: %1; "
        "    color: %2; "
        "    font-family: %3; "
        "}"
    ).arg(theme->colors().BACKGROUND)
     .arg(theme->colors().TEXT_PRIMARY)
     .arg(ThemeManager::Typography::FONT_FAMILY);
    
    setStyleSheet(styleSheet);
    
    if (m_titleLabel) {
        m_titleLabel->setStyleSheet(QString(
            "QLabel { "
            "    font-size: %1px; "
            "    font-weight: bold; "
            "    color: %2; "
            "    padding: 10px 0; "
            "}"
        ).arg(ThemeManager::Typography::FONT_SIZE_XXL)
         .arg(theme->colors().TEXT_PRIMARY));
    }
    
    if (m_webView) {
        m_webView->setStyleSheet(QString(
            "QWebEngineView { "
            "    border: 1px solid %1; "
            "    border-radius: 8px; "
            "    background-color: %2; "
            "}"
        ).arg(theme->colors().BORDER)
         .arg(theme->colors().BACKGROUND));
    }
}