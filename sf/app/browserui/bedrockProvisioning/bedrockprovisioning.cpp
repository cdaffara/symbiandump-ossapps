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

#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QProcessEnvironment>
#include <QWebSettings>
#include "bedrockprovisioning.h" 

namespace BEDROCK_PROVISIONING {

BedrockProvisioning* BedrockProvisioning::m_BedrockProvisioning = 0;

BedrockProvisioning* BedrockProvisioning::createBedrockProvisioning()
{
    if (m_BedrockProvisioning == 0)
        m_BedrockProvisioning = new BedrockProvisioning();
    return m_BedrockProvisioning;
}

BedrockProvisioning::BedrockProvisioning( QObject* parent, QString uid ) :
      QSettings(IniFormat, UserScope, BEDROCK_ORGANIZATION_NAME, uid, parent)
{
    setObjectName(QString::fromUtf8("BedrockProvisioning"));
    m_appuid = uid;
    init();
}

void BedrockProvisioning::init()
{
    QString key;
    beginGroup(m_appuid);
    {
        if (!QSettings::contains("BedrockVersion"))
            QSettings::setValue("BedrockVersion", QString(BEDROCK_VERSION_DEFAULT));

        if (!QSettings::contains("ZoomTextOnly"))
            QSettings::setValue("ZoomTextOnly", "0");

        if (!QSettings::contains("PopupBlocking"))
            QSettings::setValue("PopupBlocking", "1");

        // Base directory for ROM files (if the browser isn't in ROM this will be the same as DataBaseDirectory)
        if (!QSettings::contains("ROMBaseDirectory")) {
#if defined Q_OS_SYMBIAN
#ifndef QT_NO_DESKTOPSERVICES
            QString baseDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
            if (baseDir.length() > 2 && baseDir.at(1) == ':')
                baseDir = baseDir.mid(2);
#else /* QT_NO_DESKTOPSERVICES */
            QString baseDir = QDir::homePath(); // Not sure if this is valid
#endif /* QT_NO_DESKTOPSERVICES */
#elif defined Q_WS_MAEMO_5
            QString baseDir = "/opt/browser";
#else /* Win or Linux */
            QString baseDir = "."; /* Should this also be homePath()? */
#endif
            QSettings::setValue("ROMBaseDirectory", baseDir + "/");
        }

        // Base directory for writeable data files, not database directory
        if (!QSettings::contains("DataBaseDirectory")) {
#if defined Q_OS_SYMBIAN
#ifndef QT_NO_DESKTOPSERVICES
            QString baseDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#else /* QT_NO_DESKTOPSERVICES */
            QString baseDir = QDir::homePath();
#endif /* QT_NO_DESKTOPSERVICES */
#elif defined Q_WS_MAEMO_5
            QString baseDir = QDir::homePath();
#else /* Win or Linux */
            QString baseDir = "."; /* Should this also be homePath()? */
#endif
            QSettings::setValue("DataBaseDirectory", baseDir + "/");
        }

        if (!QSettings::contains("ChromeBaseDirectory")) {
            QString chromeBaseDir = ":/chrome/";
#ifndef Q_OS_SYMBIAN
            // Maemo, Linux, Windows can override using an env var
            static const QString envChromeBaseDir = QProcessEnvironment::systemEnvironment().value("BROWSER_CHROME"); // allow env var overriding for dev
            if (envChromeBaseDir != "")
                chromeBaseDir = envChromeBaseDir;
#endif
          QSettings::setValue("ChromeBaseDirectory", chromeBaseDir); 
        }        	

        if (!QSettings::contains("LocalPagesBaseDirectory")) {
#ifdef Q_OS_SYMBIAN
            QString localpagesBaseDir = QSettings::value("ROMBaseDirectory").toString() + "localpages/";
#else        
            QString localpagesBaseDir = QSettings::value("ChromeBaseDirectory").toString() + "localpages/";
            // Maemo, Linux, Windows can override using an env var
            static const QString envBaseDir = QProcessEnvironment::systemEnvironment().value("BROWSER_LOCALPAGES"); // allow env var overriding for dev
            if (envBaseDir != "")
                localpagesBaseDir = envBaseDir;
#endif
          QSettings::setValue("LocalPagesBaseDirectory", localpagesBaseDir); 
        }
        
        if (!QSettings::contains("StartUpChrome")) {
#ifdef  BROWSER_LAYOUT_TENONE
          QSettings::setValue("StartUpChrome", "bedrockchrome/chrome_tenone.html"); 
#else
          QSettings::setValue("StartUpChrome", "bedrockchrome/chrome.html"); 
#endif
        }        	

        if (!QSettings::contains("SplashImage")) {
          QSettings::setValue("SplashImage", "localpages/bedrock_splash.png"); 
        }        	

        if (!QSettings::contains("NetworkProxy")) {
// For s60 arm and maemo arm (i.e. not x86 emulator build) we need to set no proxy
#if (defined(Q_OS_SYMBIAN)  && !defined(Q_CC_NOKIAX86)) || (defined(Q_WS_MAEMO_5) && !defined(QT_ARCH_I386))
          // empty proxy only for ARMV5 Symbian targets
	        QSettings::setValue("NetworkProxy", QString()); 
// everything else, linux, win, s60 emulator, maemo emulator needs proxy
#else
  	      QSettings::setValue("NetworkProxy", "bswebproxy01.americas.nokia.com");
#endif
        }

        if (!QSettings::contains("NetworkPort")) {
// For s60 arm and maemo arm (i.e. not x86 emulator build) we need to set no proxy
#if (defined(Q_OS_SYMBIAN)  && !defined(Q_CC_NOKIAX86)) || (defined(Q_WS_MAEMO_5) && !defined(QT_ARCH_I386))
          // empty proxy only for ARMV5 Symbian targets
	        QSettings::setValue("NetworkPort", QString()); 
// everything else, linux, win, s60 emulator, maemo emulator needs proxy
#else
  	      QSettings::setValue("NetworkPort", "8080");
#endif
        }
        if (!QSettings::contains("DiskCacheEnabled"))
            QSettings::setValue("DiskCacheEnabled", "1");

        if (!QSettings::contains("DiskCacheMaxSize"))
            QSettings::setValue("DiskCacheMaxSize", "4194304");

        if (!QSettings::contains("MaxPagesInCache"))
            QSettings::setValue("MaxPagesInCache", "3");

        if (!QSettings::contains("DnsPrefetchEnabled"))
            QSettings::setValue("DnsPrefetchEnabled", "0");

#ifdef Q_OS_SYMBIAN
        const QString diskCacheBaseDir = "d:/system/";
#else
        const QString diskCacheBaseDir = QSettings::value("DataBaseDirectory").toString();
#endif

        if (!QSettings::contains("DiskCacheDirectoryPath")) {
            QSettings::setValue("DiskCacheDirectoryPath", diskCacheBaseDir + "brCache");
        }

        if (!QSettings::contains("Cookies"))
            QSettings::setValue("Cookies", "1");

        if (!QSettings::contains("StartPage"))
        {
            QSettings::setValue("StartPage", "startpage.html");
        }

        if (!QSettings::contains("SaveSession"))
            QSettings::setValue("SaveSession", "1");

        if (!QSettings::contains("SaveHistory"))
            QSettings::setValue("SaveHistory", "1");

        if (!QSettings::contains("BrowserEncoding"))
            QSettings::setValue("BrowserEncoding", QString("iso-8859-1"));

        if (!QSettings::contains("Html5LocalStorage")) {
            QSettings::setValue("Html5LocalStorage", "1");
        }

        if (!QSettings::contains("Html5DatabaseStorage")) {
            QSettings::setValue("Html5DatabaseStorage", "1");
        }

        if (!QSettings::contains("Html5DatabaseStorageMaxSize")) {
            // This quota applies to each individual persistent
            // store (local storage, database storage) database.
            // There is a separate instance for each security origin
            // that uses the feature.  See QWebSecurityOrigin.
            //
            // Limit: 5MB per origin
            QString maxSize = QString::number(5 * 1024 * 1024); // 5MB per origin
            QSettings::setValue("Html5DatabaseStorageMaxSize", maxSize);
        }

        if (!QSettings::contains("Html5ApplicationCache")) {
            QSettings::setValue("Html5ApplicationCache", "1");
        }

        if (!QSettings::contains("Html5ApplicationCacheMaxSize")) {
            // This quota applies to the single database instance
            // used to store ALL application cache data.  It should
            // be fairly large, as the application cache is used to
            // store entire files -- html/css/javascript text, image
            // files, etc.
            //
            // Limit: 200MB = 5MB per origin * 40 origins
            QString maxSize = QString::number(200 * 1024 * 1024);
            QSettings::setValue("Html5ApplicationCacheMaxSize", maxSize);
        }
        
        // reserved entries for local bookmarks
        if (!QSettings::contains("Bookmark0Title")) {
            QSettings::setValue("Bookmark0Title", "Browser Welcome Page");
        }
        
        if (!QSettings::contains("Bookmark0Url")) {
            QSettings::setValue("Bookmark0Url", "startpage.html");
        }
        
        if (!QSettings::contains("Bookmark1Title")) {
            QSettings::setValue("Bookmark1Title", "");
        }
        
        if (!QSettings::contains("Bookmark1Url")) {
            QSettings::setValue("Bookmark1Url", "");
        }
        
        if (!QSettings::contains("Bookmark2Title")) {
            QSettings::setValue("Bookmark2Title", "");
        }
        
        if (!QSettings::contains("Bookmark2Url")) {
            QSettings::setValue("Bookmark2Url", "");
        }
    }
	
	        // userAgentStringSetup, default empty.  
        if (!QSettings::contains("UserAgentString"))
        {
            QSettings::setValue("UserAgentString", QString());
        }          				
    endGroup(); // m_appuid
    sync();
}


QString BedrockProvisioning::valueAsString(const QString &key, const QVariant &defaultValue)
{
    return value(key, defaultValue).toString();
}

int BedrockProvisioning::valueAsInt(const QString &key, const QVariant &defaultValue)
{
    return value(key, defaultValue).toInt();
}

qint64 BedrockProvisioning::valueAsInt64(const QString &key, const QVariant &defaultValue)
{
    return value(key, defaultValue).toLongLong();
}

double BedrockProvisioning::valueAsDouble(const QString &key, const QVariant &defaultValue)
{
    return value(key, defaultValue).toDouble();
}

QVariant BedrockProvisioning::value(const QString &key, const QVariant &defaultValue)
{
    bool appMissing (!key.startsWith(m_appuid));
    if (appMissing)
        beginGroup(m_appuid);

    QVariant val = QSettings::value(key, defaultValue);

    if (appMissing)
        endGroup();

    return val;
}

int BedrockProvisioning::setValue(const QString &key, const int value)
{
    return setValue(key, QVariant(value));
}

int BedrockProvisioning::setValue(const QString &key, const double value)
{
    return setValue(key, QVariant(value));
}

int BedrockProvisioning::setValue(const QString &key, const QString &value)
{
    return setValue(key, QVariant(value));
}

int BedrockProvisioning::setValue(const QString &key, const QVariant &val)
{
    int ret(0);
    bool appMissing (!key.startsWith(m_appuid));
    if (appMissing)
        beginGroup(m_appuid);

    if (contains(key)){
        QSettings::setValue(key, val);
        sync();
        }
    else
        ret = BEDROCK_PROVISIONING_NOT_FOUND;// not found

    if (appMissing)
        endGroup();

    return ret;
}

} // end of namespace BEDROCK_PROVISIONING
