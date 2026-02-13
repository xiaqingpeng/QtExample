#include "PersistentCookieJar.h"

PersistentCookieJar::PersistentCookieJar(QObject *parent)
    : QNetworkCookieJar(parent) {
  loadCookies();
}

QList<QNetworkCookie>
PersistentCookieJar::cookiesForUrl(const QUrl &url) const {
  QList<QNetworkCookie> cookies = QNetworkCookieJar::cookiesForUrl(url);
  return cookies;
}

bool PersistentCookieJar::setCookiesFromUrl(
    const QList<QNetworkCookie> &cookieList, const QUrl &url) {
  QList<QNetworkCookie> modifiedCookies;
  for (const QNetworkCookie &cookie : std::as_const(cookieList)) {
    QNetworkCookie modifiedCookie = cookie;

    QString domain = url.host();
    modifiedCookie.setDomain(domain);

    modifiedCookie.setPath("/");

    modifiedCookies.append(modifiedCookie);
  }

  bool result = QNetworkCookieJar::setCookiesFromUrl(modifiedCookies, url);
  if (result) {
    saveCookies();
  }
  return result;
}

void PersistentCookieJar::saveCookies() {
  QSettings settings("YourCompany", "QtApp");
  QList<QNetworkCookie> allCookies = QNetworkCookieJar::allCookies();

  QByteArray cookieData;
  QBuffer buffer(&cookieData);
  buffer.open(QIODevice::WriteOnly);

  for (const QNetworkCookie &cookie : std::as_const(allCookies)) {
    buffer.write(cookie.toRawForm());
    buffer.write("\n");
  }

  settings.setValue("network/cookies",
                    QString::fromUtf8(cookieData.toBase64()));
}

void PersistentCookieJar::loadCookies() {
  QSettings settings("YourCompany", "QtApp");
  QString cookieString = settings.value("network/cookies", "").toString();

  if (cookieString.isEmpty()) {
    return;
  }

  QByteArray cookieData = QByteArray::fromBase64(cookieString.toUtf8());
  QList<QByteArray> cookieList = cookieData.split('\n');

  QList<QNetworkCookie> cookies;
  for (const QByteArray &cookieData : std::as_const(cookieList)) {
    if (!cookieData.isEmpty()) {
      QList<QNetworkCookie> parsedCookies =
          QNetworkCookie::parseCookies(cookieData);
      cookies.append(parsedCookies);
    }
  }

  setAllCookies(cookies);
}
