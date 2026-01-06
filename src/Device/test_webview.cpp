#include <QApplication>
#include <QWebEngineView>
#include <QDebug>
#include <QVBoxLayout>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 创建主窗口
    QWidget window;
    window.setWindowTitle("WebEngineView测试");
    window.resize(800, 600);
    
    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(&window);
    
    // 创建WebEngineView
    QWebEngineView *webView = new QWebEngineView(&window);
    
    // 配置WebEngineView设置
    webView->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    webView->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    webView->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    
    // 连接信号用于调试
    QObject::connect(webView->page(), &QWebEnginePage::loadStarted, []() {
        // qDebug() << "页面加载开始";
    });
    
    QObject::connect(webView->page(), &QWebEnginePage::loadProgress, [](int progress) {
        // qDebug() << "页面加载进度:" << progress << "%";
    });
    
    QObject::connect(webView, &QWebEngineView::loadFinished, [webView](bool ok) {
        // qDebug() << "页面加载完成, 状态:" << ok;
        // qDebug() << "页面标题:" << webView->title();
        // qDebug() << "页面URL:" << webView->url().toString();
        
        // 获取页面内容
        webView->page()->runJavaScript("document.body.innerHTML", [](const QVariant &result) {
            QString content = result.toString();
            // qDebug() << "页面内容:" << content;
        });
    });
    
    QObject::connect(webView->page(), &QWebEnginePage::loadError, [](const QUrl &url, QWebEnginePage::WebEngineErrorDomain domain, int error, const QString &errorString) {
        qWarning() << "页面加载错误:" << url << "- 错误" << error << ":" << errorString;
    });
    
    // 加载简单的HTML内容
    QString htmlContent = "<html><head><title>测试页面</title></head><body><h1>WebEngineView测试</h1><p>页面加载成功！</p></body></html>";
    webView->setHtml(htmlContent);
    
    // 添加WebView到布局
    layout->addWidget(webView);
    
    // 显示窗口
    window.show();
    
    return app.exec();
}