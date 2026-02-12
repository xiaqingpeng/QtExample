#ifndef NETWORKMANAGERADAPTER_H
#define NETWORKMANAGERADAPTER_H

#include "NetworkService.h"
#include <QObject>
#include <QJsonObject>
#include <functional>
#include <QUrlQuery>

class NetworkManagerAdapter : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManagerAdapter(NetworkService *networkService, QObject *parent = nullptr);
    ~NetworkManagerAdapter();

    using SuccessCallback = std::function<void(const QJsonObject &)>;
    using ErrorCallback = std::function<void(const QString &)>;

    void get(const QString &url,
             const SuccessCallback &successCallback,
             const ErrorCallback &errorCallback = nullptr,
             const QUrlQuery &queryParams = QUrlQuery());

    void post(const QString &url,
              const QJsonObject &data,
              const SuccessCallback &successCallback,
              const ErrorCallback &errorCallback = nullptr);

    void put(const QString &url,
             const QJsonObject &data,
             const SuccessCallback &successCallback,
             const ErrorCallback &errorCallback = nullptr);

    void deleteResource(const QString &url,
                        const SuccessCallback &successCallback,
                        const ErrorCallback &errorCallback = nullptr);

    void uploadFile(const QString &url,
                   const QString &filePath,
                   const QString &fileFieldName,
                   const SuccessCallback &successCallback,
                   const ErrorCallback &errorCallback = nullptr);

    static QString getPlatform();
    void setBaseUrl(const QString &baseUrl);

private:
    NetworkService *m_networkService;
};

#endif // NETWORKMANAGERADAPTER_H