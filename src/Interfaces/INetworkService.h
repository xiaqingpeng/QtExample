#ifndef INETWORKSERVICE_H
#define INETWORKSERVICE_H

#include <QObject>
#include <QJsonObject>
#include <QUrlQuery>
#include <QFuture>
#include <QNetworkReply>

/**
 * @brief 网络服务接口
 * 
 * 定义网络请求的标准接口
 */
class INetworkService
{
public:
    virtual ~INetworkService() = default;

    /**
     * @brief 发送GET请求
     * @param url 请求URL
     * @param params 查询参数
     * @return 异步结果
     */
    virtual QFuture<QJsonObject> get(const QString& url, const QUrlQuery& params = {}) = 0;

    /**
     * @brief 发送POST请求
     * @param url 请求URL
     * @param data 请求数据
     * @return 异步结果
     */
    virtual QFuture<QJsonObject> post(const QString& url, const QJsonObject& data) = 0;

    /**
     * @brief 发送PUT请求
     * @param url 请求URL
     * @param data 请求数据
     * @return 异步结果
     */
    virtual QFuture<QJsonObject> put(const QString& url, const QJsonObject& data) = 0;

    /**
     * @brief 发送DELETE请求
     * @param url 请求URL
     * @return 异步结果
     */
    virtual QFuture<QJsonObject> deleteResource(const QString& url) = 0;

    /**
     * @brief 上传文件
     * @param url 上传URL
     * @param filePath 文件路径
     * @param fieldName 字段名
     * @return 异步结果
     */
    virtual QFuture<QJsonObject> uploadFile(const QString& url, const QString& filePath, const QString& fieldName = "file") = 0;

    /**
     * @brief 设置请求头
     * @param headers 请求头
     */
    virtual void setDefaultHeaders(const QHash<QString, QString>& headers) = 0;

    /**
     * @brief 设置超时时间
     * @param timeoutMs 超时时间（毫秒）
     */
    virtual void setTimeout(int timeoutMs) = 0;

    /**
     * @brief 设置重试策略
     * @param maxRetries 最大重试次数
     * @param delayMs 重试延迟（毫秒）
     */
    virtual void setRetryPolicy(int maxRetries, int delayMs) = 0;
};

Q_DECLARE_INTERFACE(INetworkService, "com.company.qtapp.INetworkService/1.0")

#endif // INETWORKSERVICE_H