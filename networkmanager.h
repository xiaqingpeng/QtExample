#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <functional>
#include <QUrlQuery>

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    // 回调函数类型定义
    using SuccessCallback = std::function<void(const QJsonObject &)>;
    using ErrorCallback = std::function<void(const QString &)>;

    // GET请求
    void get(const QString &url,
             const SuccessCallback &successCallback,
             const ErrorCallback &errorCallback = nullptr,
             const QUrlQuery &queryParams = QUrlQuery());

    // POST请求
    void post(const QString &url,
              const QJsonObject &data,
              const SuccessCallback &successCallback,
              const ErrorCallback &errorCallback = nullptr);

    // PUT请求
    void put(const QString &url,
             const QJsonObject &data,
             const SuccessCallback &successCallback,
             const ErrorCallback &errorCallback = nullptr);

    // DELETE请求
    void deleteResource(const QString &url,
                        const SuccessCallback &successCallback,
                        const ErrorCallback &errorCallback = nullptr);

    // 获取平台标识
    static QString getPlatform();

    // 创建带有通用请求头的QNetworkRequest
    QNetworkRequest createRequest(const QString &url);

private:
    QNetworkAccessManager *m_networkManager;

    // 处理响应
    void handleResponse(QNetworkReply *reply,
                       const SuccessCallback &successCallback,
                       const ErrorCallback &errorCallback);
};

#endif // NETWORKMANAGER_H