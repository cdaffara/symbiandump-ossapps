/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtGui>
#include <QWebView>
#include <QWebPage>
#include <QWebFrame>
#include <QTranslator>
#include <QLocale>
#include <QWebSettings>
#include <QGraphicsWebView>
#ifndef NO_QSTM_GESTURE
#include "WebGestureHelper.h"
#endif

#include <QNetworkProxyFactory>
#include "browser.h"

#include "bedrockprovisioning.h"

#include <QDebug>

#ifdef QTHIGHWAY
#include <xqserviceutil.h>
#include "FileService.h"
#endif

//#define HARDWARE_DEBUG_TRACE

#ifdef ENABLE_PERF_TRACE
#include "wrtperftracer.h"
#endif

#ifdef HARDWARE_DEBUG_TRACE

static const QString DebugLogPath("C:/Data/debug.txt");

static void initDebugOutput()
{
    QFile file(DebugLogPath);
    file.remove();
}

static void debugOutput(QtMsgType type, const char *msg)
{
    QFile file(DebugLogPath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;

    QTextStream out(&file);
    out << "\nDebug: " << msg;

    file.flush();
    file.close();
}
#endif


#ifdef NO_NETWORK_ACCESS_MANAGER


static QUrl urlFromUserInput(const QString& string)
{
    QString input(string);
    QFileInfo fi(input);
    if (fi.exists() && fi.isRelative())
        input = fi.absoluteFilePath();

#if QT_VERSION >= QT_VERSION_CHECK(4, 6, 0)
    return QUrl::fromUserInput(input);
#else
    return QUrl(input);
#endif
}


struct BrowserProxyFactory : public QNetworkProxyFactory
{
    QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query = QNetworkProxyQuery())
    {
        QList<QNetworkProxy> plist;

#if defined(__WINSCW__)
        plist.push_back(QNetworkProxy(QNetworkProxy::HttpProxy,QString("dawebproxy01.americas.nokia.com"), 8080));
#endif
        QUrl proxyUrl = urlFromUserInput(qgetenv("http_proxy"));

        if (proxyUrl.isValid() && !proxyUrl.host().isEmpty()) {
            int proxyPort = (proxyUrl.port() > 0) ? proxyUrl.port() : 8080;
            plist.push_back(QNetworkProxy(QNetworkProxy::HttpProxy, proxyUrl.host(), proxyPort));
        }

        plist.append(systemProxyForQuery(query));
        return plist;
    };
};

static BrowserProxyFactory proxies;
#endif //NO_NETWORK_ACCESS_MANAGER

#ifdef ORBIT_UI
#include <hbapplication.h>
#endif // ORBIT_UI

#ifdef OPENURL
#include "BrowserMainS60.h"

static CApaApplication *AppFactoryL()
{
    return(&CBrowserMainAppSingleton::Instance());
}
#endif // OPENURL


static void configureHtml5OfflineStorage()
{
    BEDROCK_PROVISIONING::BedrockProvisioning * provisioning =
        BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning();

    QWebSettings * globalSettings = QWebSettings::globalSettings();

    // Enable all kinds of persistent storage, then fine-tune using
    // bedrock provisioning attributes.

    QWebSettings::enablePersistentStorage();

    // Local and database storage (using the global javascript localStorage
    // and openDatabase items) is shared across pages and browser invocations,
    // but it tied to the domain of the page in which the javascript code is
    // running.  This allows webpages hosted at, say, cnn.com to store data
    // using the same keys as webpages hosted at bbc.co.uk without conflict.
    //
    // Note that local and database storage is persisted in directories
    // beneath the application's private directory, so it is not shared
    // with other browsers.

    bool localStorageEnabled =
        provisioning->valueAsInt("Html5LocalStorage") != 0;

    bool databaseStorageEnabled =
        provisioning->valueAsInt("Html5DatabaseStorage") != 0;

    globalSettings->setAttribute(
                QWebSettings::LocalStorageEnabled,
                localStorageEnabled);

    globalSettings->setAttribute(
                QWebSettings::OfflineStorageDatabaseEnabled,
                databaseStorageEnabled);

    // There is no separate QWebSettings method to configure
    // the default quota for local storage databases.
    // It appears that both local storage and database storage
    // use the same default quota.

    if (localStorageEnabled || databaseStorageEnabled) {
        globalSettings->setOfflineStorageDefaultQuota(
                provisioning->valueAsInt64("Html5DatabaseStorageMaxSize"));
    }

    // The application cache allows websites to specify a manifest file in
    // the outermost <html> element.  The manifest file defines which files
    // may be stored at the client and used when the network is inaccessible.
    //
    // Webkit stores the application cache in a single SQLite database
    // named ApplicationCache.db.  By default this database is located
    // in the application's private directory, so it is not shared with
    // other browsers.

    bool applicationCacheEnabled =
        provisioning->valueAsInt("Html5ApplicationCache") != 0;

    globalSettings->setAttribute(
                QWebSettings::OfflineWebApplicationCacheEnabled,
                applicationCacheEnabled);

    if (applicationCacheEnabled) {
        globalSettings->setOfflineWebApplicationCacheQuota(
                provisioning->valueAsInt64("Html5ApplicationCacheMaxSize"));
    }
}


int main(int argc, char * argv[])
{
#ifdef NO_NETWORK_ACCESS_MANAGER
    QNetworkProxyFactory::setApplicationProxyFactory(&proxies);
#endif //NO_NETWORK_ACCESS_MANAGER
    int res=0;

#ifdef HARDWARE_DEBUG_TRACE
    initDebugOutput();
    qInstallMsgHandler(debugOutput);
#endif

//    qDebug() << "main - before app, argc=" << argc;
//    for (int i = 0; i < argc; ++i)
//        {
//        qDebug() << "main - argv[" << i << "] = " << argv[i];
//        }

/* openurl should only work in Orbit UI application. */
#ifdef ORBIT_UI
#ifdef OPENURL
#ifdef NO_QSTM_GESTURE
    HbApplication app(AppFactoryL, argc, argv);
#else
    BrowserApp app(AppFactoryL, argc, argv);
#endif
#else /* !OPENURL */
#ifdef NO_QSTM_GESTURE
  HbApplication app(argc, argv);
#else // ORBIT_UI
  BrowserApp app(argc, argv);
#endif
#endif /* OPENURL */
#else
#ifdef NO_QSTM_GESTURE
  QApplication app(argc, argv);
#else
  BrowserApp app(argc, argv);
#endif
#endif // ORBIT_UI

#ifdef QTHIGHWAY
    //qDebug() << "ServiceInfo:" << (XQServiceUtil::isService() ? "Service" : "Normal") << "launch";
    //qDebug() << "  Embedded=" << XQServiceUtil::isEmbedded() << "Interface=" << XQServiceUtil::interfaceName() << "Operation=" << XQServiceUtil::operationName();

    // provide service for html mime type
    FileService *fileServiceProvider = new FileService();
#endif

//  qDebug() << "main - after app";
#ifdef Q_OS_SYMBIAN
    //Object cache settings. NB: these need to be tuned per device
    QWebSettings::globalSettings()->setObjectCacheCapacities(128*1024, 1024*1024, 1024*1024);
#endif

    if (BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("DnsPrefetchEnabled").toBool())
	{
	// Specifies whether QtWebkit will try to pre-fetch DNS entries to speed up browsing.
	// Without this technique, the DNS lookup is performed only when you click on the link,
	// adding a wait of 250ms (on average) before the page even starts to show up.
	// This only works as a global attribute.
	   QWebSettings::globalSettings()->setAttribute(QWebSettings::DnsPrefetchEnabled, true);
	}

    configureHtml5OfflineStorage();

    QString lang = QLocale::system().name();

    //install the common translator from platform
    QTranslator common;
    common.load("z:/resource/qt/translations/common_" + lang);
    QApplication::installTranslator(&common);


    //install the translator from Browser
    QTranslator translator;
    QString transFilePath = "Z:/resource/qt/translations/";
    QString transFile = QLatin1String("browserloc_") +  lang;
    translator.load(transFile, transFilePath);
    QApplication::installTranslator(&translator);

// To make the native urlsearch bar selection visible, the following lines have to be removed
// The keypad navigation still works for ginebra2 even without enabling keypad navigation
/*
#ifdef Q_OS_SYMBIAN
    QApplication::setKeypadNavigationEnabled(true);
#endif
*/
#ifdef OPENURL
    // Handle QDesktopServices.openUrl (when browser wasn't already running)
    QString url = CBrowserMainAppSingleton::Instance().InitialUrl();
    GinebraBrowser * browser = new GinebraBrowser(0, &url); // Pass the initial url so it will be loaded as soon as the browser chrome finishes
    // Set things up to handle QDesktopServices.openUrl calls when browser is already running
    CBrowserMainAppSingleton::Instance().setUrlHandler(browser);
#else
    GinebraBrowser * browser = new GinebraBrowser();
#endif


#ifdef ENABLE_PERF_TRACE
#if defined(NO_NETWORK_ACCESS_MANAGER)
    PERF_TRACE_OUT() << "NO_NETWORK_ACCESS_MANAGER\n";
#endif
#if defined(NO_RESIZE_ON_LOAD)
    PERF_TRACE_OUT() << "NO_RESIZE_ON_LOAD\n";
#endif
#if defined(NO_QSTM_GESTURE)
    PERF_TRACE_OUT() << "NO_QSTM_GESTURE\n";
#endif
#if defined(__gva_no_chrome__)
    PERF_TRACE_OUT() << "__gva_no_chrome__\n";
#endif
#if defined(NO_HISTORY)
    PERF_TRACE_OUT() << "NO_HISTORY\n";
#endif

#endif //ENABLE_PERF_TRACE

    browser->show();
    res = app.exec();
    delete browser;

#ifdef QTHIGHWAY
    delete fileServiceProvider;
#endif

#ifdef ENABLE_PERF_TRACE
      WrtPerfTracer::tracer()->close();
#endif //ENABLE_PERF_TRACE

    return res;
}
