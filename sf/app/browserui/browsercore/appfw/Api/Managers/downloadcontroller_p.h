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

#ifndef __DOWNLOAD_CONTROLLER_PRIVATE_H__
#define __DOWNLOAD_CONTROLLER_PRIVATE_H__

#include <QObject>
#include "BWFGlobal.h"

#ifdef USE_DOWNLOAD_MANAGER
#include "download.h"
#include "downloadmanager.h"


// This seems to be necessary to use unqualified download manager
// class names in the slots declarations below.  We need to use
// unqualified names because the signals they get connected to
// use unqualified names and moc doesn't realize that DownloadXXX
// and WRT::DownloadXXX are equivalent for some values of XXX.
using namespace WRT;
#else
class Download;
class DownloadEvent;
class DownloadManager;
class DownloadManagerEvent;
class Error;
class QNetworkReply;
#endif
class QFileInfo;
class QNetworkProxy;
class QNetworkRequest;
class QString;
class QUrl;

class DownloadController;

class BWF_EXPORT DownloadControllerPrivate : public QObject
{
    Q_OBJECT

public:
    DownloadControllerPrivate(
            DownloadController * controller,
            const QString & client,
            const QNetworkProxy & proxy);

    ~DownloadControllerPrivate();

    void startDownload(const QUrl & url, const QFileInfo & info);
    void startDownload(QNetworkReply * reply);
    void startDownload(const QNetworkRequest & request);

private:
#ifdef USE_DOWNLOAD_MANAGER
    void startDownload(
            WRT::Download * download,
            const QUrl & url);
#else
    void startDownload(
            Download * download,
            const QUrl & url);
#endif

public slots:
    void handleDownloadManagerEvent(DownloadManagerEvent * event);
    void handleDownloadEvent(DownloadEvent * event);
#ifdef USE_DOWNLOAD_MANAGER
    void handleDownloadError(Error error);
#endif

private:
    DownloadController * m_downloadController;
#ifdef USE_DOWNLOAD_MANAGER
    WRT::DownloadManager * m_downloadManager; // owned
#else
    DownloadManager * m_downloadManager; // owned
#endif
};

#endif // __DOWNLOAD_CONTROLLER_PRIVATE_H__
