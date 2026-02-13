#ifndef PERSISTENTCOOKIEJAR_H
#define PERSISTENTCOOKIEJAR_H

#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QSettings>
#include <QBuffer>
#include <QDebug>

class PersistentCookieJar : public QNetworkCookieJar
{
    Q_OBJECT
public:
    explicit PersistentCookieJar(QObject *parent = nullptr);
    
    QList<QNetworkCookie> cookiesForUrl(const QUrl &url) const override;
    bool setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url) override;
    
private:
    void saveCookies();
    void loadCookies();
};

#endif // PERSISTENTCOOKIEJAR_H
