/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, 
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

#include "webcookiejar.h"
#include "bedrockprovisioning.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMetaObject>
#include <QSettings>
#include <QUrl>

#include <QDebug>

static const unsigned int JAR_VERSION = 1;

// for debugging webcookiejar, uncomment this (and have QT debug enabled)
//#define DEBUG_WEBCOOKIEJAR 1

QT_BEGIN_NAMESPACE
QDataStream &operator<<(QDataStream &stream, const QList<QNetworkCookie> &list)
{
    stream << JAR_VERSION;
    stream << quint32(list.size());
    for (int i = 0; i < list.size(); ++i)
        stream << list.at(i).toRawForm();
    return stream;
}

QDataStream &operator>>(QDataStream &stream, QList<QNetworkCookie> &list)
{
    list.clear();

    quint32 version;
    stream >> version;

    if (version != JAR_VERSION)
        return stream;

    quint32 count;
    stream >> count;
    for (quint32 i = 0; i < count; ++i) {
        QByteArray value;
        stream >> value;
        QList<QNetworkCookie> newCookies = QNetworkCookie::parseCookies(value);
        if (newCookies.count() == 0 && value.length() != 0) {
            qWarning() << "CookieJar: Unable to parse saved cookie:" << value;
        }
        for (int j = 0; j < newCookies.count(); ++j)
            list.append(newCookies.at(j));
        if (stream.atEnd())
            break;
    }
    return stream;
}
QT_END_NAMESPACE

namespace WRT {

CookieJar::CookieJar(QObject *parent)
    : QNetworkCookieJar(parent)
    , m_loaded(false)
{
    m_cookiesDir = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsString("DataBaseDirectory");
    m_cookiesFile = m_cookiesDir + QLatin1String("cookies.ini");
}

CookieJar::~CookieJar()
{
    save();
}

void CookieJar::clear()
{
    setAllCookies(QList<QNetworkCookie>());

    if (!QFile::exists(m_cookiesFile))
        return;

    QFile::remove(m_cookiesFile);
}

void CookieJar::load()
{
    if (m_loaded)
        return;
    // load cookies
    qRegisterMetaTypeStreamOperators<QList<QNetworkCookie> >("QList<QNetworkCookie>");

    QSettings cookieSettings(m_cookiesFile, QSettings::IniFormat);
    QList<QNetworkCookie> lst = qvariant_cast<QList<QNetworkCookie> >(cookieSettings.value(QLatin1String("cookies")));
    setAllCookies(lst);
    QList<QNetworkCookie>::Iterator it = lst.begin();
    QList<QNetworkCookie>::Iterator end = lst.end();
    m_loaded = true;
}

void CookieJar::save()
{
    if (!m_loaded)
        return;

    purgeOldCookies();
    QList<QNetworkCookie> cookies = allCookies();
    for (int i = cookies.count() - 1; i >= 0; --i)
        if (cookies.at(i).isSessionCookie())
            cookies.removeAt(i);

#ifdef DEBUG_WEBCOOKIEJAR
    qDebug() << "number of saved cookies:" << cookies.size();
#endif

    if (!QFile::exists(m_cookiesDir)) {
        QDir dir;
        dir.mkpath(m_cookiesDir);
    }
    QSettings cookieSettings(m_cookiesFile, QSettings::IniFormat);
    cookieSettings.setValue(QLatin1String("cookies"), qVariantFromValue<QList<QNetworkCookie> >(cookies));
}

void CookieJar::purgeOldCookies()
{
    QList<QNetworkCookie> cookies = allCookies();
    if (cookies.isEmpty())
        return;
    int oldCount = cookies.count();
    QDateTime now = QDateTime::currentDateTime();
    for (int i = cookies.count() - 1; i >= 0; --i) {
        if (cookies.at(i).expirationDate().isValid()
            && cookies.at(i).expirationDate() < now) {
            cookies.removeAt(i);
        }
    }
    if (oldCount == cookies.count())
        return;
    setAllCookies(cookies);
}

QList<QNetworkCookie> CookieJar::cookiesForUrl(const QUrl &url) const
{
    QList<QNetworkCookie> cookies;
    bool enabled = (bool) BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("Cookies");
    if (!enabled)
        return cookies;

    CookieJar *that = const_cast<CookieJar*>(this);
    if (!m_loaded)
        that->load();

    cookies = QNetworkCookieJar::cookiesForUrl(url);

#ifdef DEBUG_WEBCOOKIEJAR
    qDebug() << "============================================================";
    qDebug() << "cookie list to send for url:" << url;
#endif
    QList<QNetworkCookie>::Iterator it = cookies.begin();
    // we might erase it in the loop, cookie.end() is not constant
    for ( ; it != cookies.end(); ) {
        // can't send secure cookie over http connection
        if (it->isSecure()
            && url.scheme().compare("https") != 0) {
            // after erase() is called, iterator automatically points to the next item
            it = cookies.erase(it);
            continue;
        }

#ifdef DEBUG_WEBCOOKIEJAR
        qDebug() << it->name() << it->value() << it->expirationDate() << it->domain() << it->path() << it->isSecure();
#endif
        ++it;
    }

    return cookies;
}

bool CookieJar::setCookiesFromUrl(const QList<QNetworkCookie> &cookieList, const QUrl &url)
{
    bool addedCookies = false;

    bool enabled = (bool) BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->valueAsInt("Cookies");
    if (!enabled)
        return addedCookies;

    if (!m_loaded)
        load();

#ifdef DEBUG_WEBCOOKIEJAR
        qDebug() << "============================================================";
#endif

    // domain of the url
    QString urlHost = url.host();
    QString urlPath = url.path();
    QList<QNetworkCookie> lst;
    foreach (QNetworkCookie cookie, cookieList) {
        QString domain = cookie.domain();
        // set default domain
        if (domain.compare(QString()) != 0
            && domain.compare(urlHost) !=0 
            && !domain.startsWith(QLatin1Char('.'))) {
        // if domain doesn't start with .
            QString domainStartWithDot = domain.prepend(QLatin1Char('.'));
            cookie.setDomain(domainStartWithDot);
        }

        // reject if domain is like ".com"
        // (i.e., reject if domain does not contain embedded dots, see RFC 2109 section 4.3.2)
        if (domain.lastIndexOf(QLatin1Char('.')) == 0)
           continue;           // not accepted
           
        // set default path
        if (cookie.path().compare(QString()) == 0)
            cookie.setPath(urlPath.left(urlPath.lastIndexOf(QLatin1Char('/'))));

#ifdef DEBUG_WEBCOOKIEJAR
            qDebug() << "cookie:" << cookie.name() << cookie.domain() << cookie.path() << cookie.expirationDate();
#endif

        // only https connection can set secure cookie
        if (cookie.isSecure()
            && url.scheme().compare("https") != 0)
            continue;

        // 4k is allowed size of name and value
        int sizeName = cookie.name().size();
        int sizeValue = cookie.value().size();
        int sizeNameAndValue = sizeName + sizeValue;
        if (sizeName > 4096)
            continue;
        else if (sizeNameAndValue >= 4096) {
            int sizeToTruncate = sizeNameAndValue-4096;
            int posToTruncate = sizeValue - sizeToTruncate;
            QByteArray tmpValue = cookie.value();
            tmpValue.truncate(posToTruncate);
            cookie.setValue(tmpValue);
        }

        lst += cookie;
    }

    if (QNetworkCookieJar::setCookiesFromUrl(lst, url)) {
#ifdef DEBUG_WEBCOOKIEJAR
        qDebug() << "cookie list set";
#endif
        addedCookies = true;
    }

    // 20 cookies per domain
    QList<QNetworkCookie> cookies = allCookies();
    QList<QNetworkCookie>::Iterator it = cookies.begin();
    int countPerDomain = 0;
    bool removeCookieFromSameDomain = false;
#ifdef DEBUG_WEBCOOKIEJAR
    qDebug() << "set limit of 20 for the host" << urlHost;
#endif
    // we might erase it in the loop, cookie.end() is not constant
    for ( ; it != cookies.end(); ) {
        bool erased = false;
        // tail matching
        if (urlHost.endsWith(it->domain())) {
            QDateTime now = QDateTime::currentDateTime();
            countPerDomain++;
            // when limit reaches, kick out the old ones and the expired ones
            if (countPerDomain > 20 
                || (!it->isSessionCookie() && it->expirationDate() < now)) {
                // after erase() is called, iterator automatically points to the next item
                it = cookies.erase(it);
                countPerDomain--;
                removeCookieFromSameDomain = true;
                erased = true;
            }
        }

        if (!erased)
            ++it;
    }
    if (removeCookieFromSameDomain)
        setAllCookies(cookies);

    return addedCookies;
}

}
