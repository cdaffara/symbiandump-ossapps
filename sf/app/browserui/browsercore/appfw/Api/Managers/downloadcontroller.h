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

#ifndef __DOWNLOAD_CONTROLLER_H__
#define __DOWNLOAD_CONTROLLER_H__

#include <QObject>
#include "BWFGlobal.h"

#include "downloadproxy.h"

class QFileInfo;
class QNetworkProxy;
class QNetworkReply;
class QNetworkRequest;
class QString;
class QUrl;
class QWebPage;

class DownloadControllerPrivate;

class BWF_EXPORT DownloadController : public QObject
{
    Q_OBJECT

public:
    DownloadController(const QString & client, const QNetworkProxy & proxy);
    ~DownloadController();

public slots:
    bool handlePage(QWebPage * page);

    void startDownload(const QUrl & url, const QFileInfo & info);

private slots:
    void startDownload(QNetworkReply * reply);
    void startDownload(const QNetworkRequest & request);

signals:
    void downloadCreated(DownloadProxy downloadProxy);

    void downloadStarted(DownloadProxy downloadProxy);

    void downloadHeaderReceived(DownloadProxy downloadProxy);

    void downloadProgress(DownloadProxy downloadProxy);

    void downloadFinished(DownloadProxy downloadProxy);

    void downloadPaused(DownloadProxy downloadProxy, const QString & error);

    void downloadCancelled(DownloadProxy downloadProxy, const QString & error);

    void downloadFailed(DownloadProxy downloadProxy, const QString & error);

    void downloadNetworkLoss(DownloadProxy downloadProxy, const QString & error);

    void downloadError(DownloadProxy downloadProxy, const QString & error);

    void downloadMessage(DownloadProxy downloadProxy, const QString & message);

    void downloadsCleared();

    void unsupportedDownload(const QUrl & url);

private:
    DownloadControllerPrivate * d;

    // Signals are protected, so they can't normally be emitted from methods
    // in unrelated classes.  Therefore to emit DownloadController signals
    // from methods in DownloadControllerPrivate, DownloadController must
    // declare DownloadControllerPrivate as a friend.

    friend class DownloadControllerPrivate;
};

#endif // __DOWNLOAD_CONTROLLER_H__
