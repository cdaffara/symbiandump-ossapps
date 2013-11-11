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

#include "downloadcontroller.h"
#include "downloadcontroller_p.h"

#include "downloadproxy_p.h"

#include <QFileInfo>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QUrl>
#include <QWebPage>

#ifdef USE_DOWNLOAD_MANAGER
using namespace WRT;

static const char * downloadErrorToString(WRT::Error error)
{
    switch (error) {
    case WRT::NoError:
        return "WRT::NoError";
    case WRT::ConnectionRefusedError:
        return "WRT::ConnectionRefusedError";
    case WRT::RemoteHostClosedError:
        return "WRT::RemoteHostClosedError";
    case WRT::HostNotFoundError:
        return "WRT::HostNotFoundError";
    case WRT::TimeoutError:
        return "WRT::TimeoutError";
    case WRT::OperationCanceledError:
        return "WRT::OperationCanceledError";
    case WRT::SslHandshakeFailedError:
        return "WRT::SslHandshakeFailedError";
    case WRT::TemporaryNetworkFailureError:
        return "WRT::TemporaryNetworkFailureError";
    case WRT::UnknownNetworkError:
        return "WRT::UnknownNetworkError";
    case WRT::ProxyConnectionRefusedError:
        return "WRT::ProxyConnectionRefusedError";
    case WRT::ProxyConnectionClosedError:
        return "WRT::ProxyConnectionClosedError";
    case WRT::ProxyNotFoundError:
        return "WRT::ProxyNotFoundError";
    case WRT::ProxyTimeoutError:
        return "WRT::ProxyTimeoutError";
    case WRT::ProxyAuthenticationRequiredError:
        return "WRT::ProxyAuthenticationRequiredError";
    case WRT::UnknownProxyError:
        return "WRT::UnknownProxyError";
    case WRT::ContentAccessDenied:
        return "WRT::ContentAccessDenied";
    case WRT::ContentOperationNotPermittedError:
        return "WRT::ContentOperationNotPermittedError";
    case WRT::ContentNotFoundError:
        return "WRT::ContentNotFoundError";
    case WRT::AuthenticationRequiredError:
        return "WRT::AuthenticationRequiredError";
    case WRT::ContentReSendError:
        return "WRT::ContentReSendError";
    case WRT::UnknownContentError:
        return "WRT::UnknownContentError";
    case WRT::ProtocolUnknownError:
        return "WRT::ProtocolUnknownError";
    case WRT::ProtocolInvalidOperationError:
        return "WRT::ProtocolInvalidOperationError";
    case WRT::ProtocolFailure:
        return "WRT::ProtocolFailure";
    case WRT::ConnectionFailed:
        return "WRT::ConnectionFailed";
    case WRT::HttpRestartFailed:
        return "WRT::HttpRestartFailed";
    case WRT::ContentExpired:
        return "WRT::ContentExpired";
    case WRT::PartialContentModified:
        return "WRT::PartialContentModified";
    case WRT::ObjectNotFound:
        return "WRT::ObjectNotFound";
    case WRT::HttpUnhandled:
        return "WRT::HttpUnhandled";
    case WRT::DestinationFileInUse:
        return "WRT::DestinationFileInUse";
    case WRT::WrongDestinationFilename:
        return "WRT::WrongDestinationFilename";
    case WRT::BadUrl:
        return "WRT::BadUrl";
    case WRT::MediaRemoved:
        return "WRT::MediaRemoved";
    case WRT::FileWriteFailed:
        return "WRT::FileWriteFailed";
    case WRT::FileNotFound:
        return "WRT::FileNotFound";
    case WRT::InvalidDrive:
        return "WRT::InvalidDrive";
    case WRT::MoveFailed:
        return "WRT::MoveFailed";
    case WRT::DiskFull:
        return "WRT::DiskFull";
    case WRT::DiskError:
        return "WRT::DiskError";
    case WRT::TransactionFailed:
        return "WRT::TransactionFailed";
    case WRT::Internal:
        return "WRT::Internal";
    case WRT::General:
        return "WRT::General";
    case WRT::MaxRedirectionsReached:
        return "WRT::MaxRedirectionsReached";
    case WRT::InvalidDownloadDescriptor:
        return "WRT::InvalidDownloadDescriptor";
    default:
        return "???";
    }
}

static const char * downloadEventToString(WRT::DownloadEvent::Event type)
{
    switch (type) {
    case WRT::DownloadEvent::Created:
        return "DownloadEvent:Created";
    case WRT::DownloadEvent::Started:
        return "DownloadEvent::Started";
    case WRT::DownloadEvent::HeadersReceived:
        return "DownloadEvent::HeadersReceived";
    case WRT::DownloadEvent::InProgress:
        return "DownloadEvent::InProgress";
    case WRT::DownloadEvent::Paused:
        return "DownloadEvent::Paused";
    case WRT::DownloadEvent::Completed:
        return "DownloadEvent::Completed";
    case WRT::DownloadEvent::Failed:
        return "DownloadEvent::Failed";
    case WRT::DownloadEvent::Cancelled:
        return "DownloadEvent::Cancelled";
    case WRT::DownloadEvent::DescriptorUpdated:
        return "DownloadEvent::DescriptorUpdated";
    case WRT::DownloadEvent::NetworkLoss:
        return "DownloadEvent::NetworkLoss";
    case WRT::DownloadEvent::Error:
        return "DownloadEvent::Error";
    case WRT::DownloadEvent::DescriptorReady:
        return "DownloadEvent::DescriptorReady";
    case WRT::DownloadEvent::LicenseAcquiring:
        return "DownloadEvent::LicenseAcquiring";
    case WRT::DownloadEvent::MediaRemoved:
        return "DownloadEvent::MediaRemoved";
    case WRT::DownloadEvent::MediaInserted:
        return "DownloadEvent::MediaInserted";
    case WRT::DownloadEvent::RedirectedPermanently:
        return "DownloadEvent::RedirectedPermanently";
    case WRT::DownloadEvent::RedirectedTemporarily:
        return "DownloadEvent::RedirectedTemporarily";
    case WRT::DownloadEvent::NameChanged:
        return "DownloadEvent::NameChanged";
    case WRT::DownloadEvent::ContentTypeChanged:
        return "DownloadEvent::ContentTypeChanged";
    case WRT::DownloadEvent::CreatingConnection:
        return "DownloadEvent::CreatingConnection";
    case WRT::DownloadEvent::ConnectionNeeded:
        return "DownloadEvent::ConnectionNeeded";
    case WRT::DownloadEvent::ConnectionDisconnected:
        return "DownloadEvent::ConnectionDisconnected";
    case WRT::DownloadEvent::RightObjectsAcquired:
        return "DownloadEvent::RightObjectsAcquired";
    default:
        return 0;
    }
}

static void debugDownloadEvent(WRT::DownloadEvent::Event type)
{
    const char * name = downloadEventToString(type);
    if (name == 0) {
        return;
    }

    qDebug() << "Received event" << name;
}

// DownloadControllerPrivate implementation

DownloadControllerPrivate::DownloadControllerPrivate(
    DownloadController * downloadController,
    const QString & client,
    const QNetworkProxy & proxy)
{
    m_downloadController = downloadController;

    m_downloadManager = new WRT::DownloadManager(client);

    if (m_downloadManager->initialize() != 0) {
        return;
    }

    if (!connect(
                m_downloadManager,
                SIGNAL(downloadManagerEvent(DownloadManagerEvent *)),
                this,
                SLOT(handleDownloadManagerEvent(DownloadManagerEvent *)))) {
        //;;; how to handle error?
    }

    if (proxy.type() != QNetworkProxy::NoProxy)
        m_downloadManager->setProxy(proxy.hostName(), proxy.port());
}

DownloadControllerPrivate::~DownloadControllerPrivate()
{
    delete m_downloadManager;
}

static QString downloadFileName(QUrl url)
{
    QString scheme = url.scheme();

    // For http and https, let the download manager determine the filename.

    if (scheme == "http" || scheme == "https")
        return QString();

    // For data scheme (see http://en.wikipedia.org/wiki/Data_URI_scheme)
    // we don't have a file name per-se, so construct one from the content
    // type.

    if (scheme == "data") {
        // Typical example: data:image/png;base64,...

        QString path = url.path();
        QString type = path.section('/', 0, 0);
        QString subtype = path.section('/', 1, 1).section(';', 0, 0);

        // For now we just use type as base name and subtype
        // as extension.  For the common case of image/jpg
        // and stuff like that this works fine.

        return type + "." + subtype;
    }

    // For all other schemes, let the download manager determine the filename.

    return QString();
}

void DownloadControllerPrivate::startDownload(const QUrl & url, const QFileInfo & info)
{
    WRT::Download * download = m_downloadManager->createDownload(url.toString());

    download->setAttribute(WRT::DestinationPath, info.absolutePath());
    download->setAttribute(WRT::FileName, info.fileName());

    startDownload(download, url);
}

void DownloadControllerPrivate::startDownload(QNetworkReply * reply)
{
    QUrl url = reply->url();

    WRT::Download * download = m_downloadManager->createDownload(reply);

    startDownload(download, url);
}

void DownloadControllerPrivate::startDownload(const QNetworkRequest & request)
{
    QUrl url = request.url();

    WRT::Download * download = m_downloadManager->createDownload(url.toString());

    startDownload(download, url);
}

void DownloadControllerPrivate::startDownload(WRT::Download * download, const QUrl & url)
{
    // If necessary suggest an alternate file name.
    // The download manager will adjust the file name for us to handle
    // duplicates in the destination directory.

    QString file = downloadFileName(url);

    if (file.length() > 0) {
        QVariant value(file);
        download->setAttribute(WRT::FileName, value);
    }

    // Start download.

    DownloadProxy downloadProxy(new DownloadProxyData(download));

    emit m_downloadController->downloadCreated(downloadProxy);

    if (!connect(
                download,
                SIGNAL(downloadEvent(DownloadEvent *)),
                this,
                SLOT(handleDownloadEvent(DownloadEvent *)))) {
        //;;; how to handle error?
    }

    if (!connect(
                download,
                SIGNAL(downloadError(Error)),
                this,
                SLOT(handleDownloadError(Error)))) {
        //;;; how to handle error?
    }

    download->start();

}

void DownloadControllerPrivate::handleDownloadManagerEvent(
        DownloadManagerEvent * event)
{
    int type = event->type();

    switch (type) {
    case WRT::DownloadManagerEvent::Created:
        // Instead of waiting for the DownloadManager Created event
        // we emit downloadCreated in startDownload above so that we
        // can add a pointer to the download created as a parameter.
        return;

    case WRT::DownloadManagerEvent::Removed:
        if (m_downloadManager->currentDownloads().empty()) {
            emit m_downloadController->downloadsCleared();
        }
        return;

    default:
        qDebug() << "Unexpected download manager event:" << type;
        return;
    }
}

void DownloadControllerPrivate::handleDownloadEvent(
        DownloadEvent * event)
{
    int id = event->id();

    WRT::Download * download = m_downloadManager->findDownload(id);

    if (!download) {
        qDebug() << "Cannot find download with id" << id << " for error " << event;
        return;
    }

    int errorNum = download->attribute(WRT::LastError).toInt();

    const char * errorStr = downloadErrorToString(
            static_cast<WRT::Error>(errorNum));

    QString error;
    if (errorStr != 0)
        error = errorStr;

    DownloadProxy downloadProxy(new DownloadProxyData(download));

    switch (event->type())
    {

    case WRT::DownloadEvent::Created:
        // Nothing to do, we already emitted a signal to report
        // creation of a new Download object from startDownload().
        return;

    case WRT::DownloadEvent::Started:
        emit m_downloadController->downloadStarted(downloadProxy);
        return;

    case WRT::DownloadEvent::HeadersReceived:
        emit m_downloadController->downloadHeaderReceived(downloadProxy);
        return;

    case WRT::DownloadEvent::InProgress:
        emit m_downloadController->downloadProgress(downloadProxy);
        return;

    case WRT::DownloadEvent::Completed:
        emit m_downloadController->downloadFinished(downloadProxy);
        return;

    case WRT::DownloadEvent::Paused:
        emit m_downloadController->downloadPaused(downloadProxy, error);
        return;

    case WRT::DownloadEvent::Cancelled:
        emit m_downloadController->downloadCancelled(downloadProxy, error);
        return;

    case WRT::DownloadEvent::Failed:
        emit m_downloadController->downloadFailed(downloadProxy, error);
        return;

    case WRT::DownloadEvent::DescriptorUpdated:
        // FIXME - Update to support OMA and DRM.
        return;

    case WRT::DownloadEvent::NetworkLoss:
        emit m_downloadController->downloadNetworkLoss(downloadProxy, error);
        return;

    case WRT::DownloadEvent::Error:
        emit m_downloadController->downloadError(downloadProxy, error);
        return;

    case WRT::DownloadEvent::DescriptorReady:
        // FIXME - Update to support OMA and DRM.
        return;

    case WRT::DownloadEvent::LicenseAcquiring:
        // FIXME - Update to support OMA and DRM.
        return;

    case WRT::DownloadEvent::MediaRemoved:
        // FIXME ;;; what to do?
        emit m_downloadController->downloadMessage(
                downloadProxy,
                "Media Removed");
        return;

    case WRT::DownloadEvent::MediaInserted:
        // FIXME ;;; what to do?
        emit m_downloadController->downloadMessage(
                downloadProxy,
                "Media Inserted");
        return;

    case WRT::DownloadEvent::RedirectedPermanently:
        // FIXME ;;; what to do?
        emit m_downloadController->downloadMessage(
                downloadProxy,
                "Redirected Permanently");
        return;

    case WRT::DownloadEvent::RedirectedTemporarily:
        // FIXME ;;; what to do?
        emit m_downloadController->downloadMessage(
                downloadProxy,
                "Redirected Temporarily");
        return;

    case WRT::DownloadEvent::NameChanged:
        // FIXME ;;; what to do?
        emit m_downloadController->downloadMessage(
                downloadProxy,
                "Name Changed");
        return;

    case WRT::DownloadEvent::ContentTypeChanged:
        // FIXME ;;; what to do?
        emit m_downloadController->downloadMessage(
                downloadProxy,
                "Type Changed");
        return;

    case WRT::DownloadEvent::CreatingConnection:
        // FIXME ;;; what to do?
        emit m_downloadController->downloadMessage(
                downloadProxy,
                "Creating Connection");
        return;

    case WRT::DownloadEvent::ConnectionNeeded:
        // FIXME ;;; what to do?
        emit m_downloadController->downloadMessage(
                downloadProxy,
                "Connection Needed");
        return;

    case WRT::DownloadEvent::ConnectionDisconnected:
        // FIXME ;;; what to do?
        emit m_downloadController->downloadMessage(
                downloadProxy,
                "Connection Disconnected");
        return;

    case WRT::DownloadEvent::RightObjectsAcquired:
        // FIXME - Update to support OMA and DRM.
        return;

    default:
        qDebug() << "Unexpected download event:" << event->type();
        break;
    }
}

void DownloadControllerPrivate::handleDownloadError(Error error)
{
    // Expect the WRT::DownloadEvent::Error case in handleDownloadEvent()
    // above should handle anything likely to go through here.
}

// DownloadController implementation

DownloadController::DownloadController(
    const QString & client,
    const QNetworkProxy & proxy)
{
    d = new DownloadControllerPrivate(this, client, proxy);
}

DownloadController::~DownloadController()
{
    delete d;
}

void DownloadController::startDownload(const QUrl & url, const QFileInfo & info)
{
    qDebug() << "Download URL" << url;

    d->startDownload(url, info);
}

void DownloadController::startDownload(QNetworkReply * reply)
{
    QUrl url = reply->url();

    qDebug() << "Download unsupported content" << url;

    d->startDownload(reply);
}

void DownloadController::startDownload(const QNetworkRequest & request)
{
    QUrl url = request.url();

    qDebug() << "Save link or image" << url;

    d->startDownload(request);
}

#else // USE_DOWNLOAD_MANAGER

// Empty implementation for when DownloadManager is unsupported.

DownloadController::DownloadController(
    const QString & /*client*/,
    const QNetworkProxy & /*proxy*/)
{}

DownloadController::~DownloadController()
{}

void DownloadController::startDownload(const QUrl & url, const QFileInfo & info)
{
    Q_UNUSED(info)

    emit unsupportedDownload(url);
}

void DownloadController::startDownload(QNetworkReply * reply)
{
    QUrl url = reply->url();

    emit unsupportedDownload(url);
}

void DownloadController::startDownload(const QNetworkRequest & request)
{
    QUrl url = request.url();

    emit unsupportedDownload(url);
}

void DownloadControllerPrivate::handleDownloadManagerEvent(
        DownloadManagerEvent * event)
{
}

void DownloadControllerPrivate::handleDownloadEvent(
        DownloadEvent * event)
{
}

DownloadControllerPrivate::~DownloadControllerPrivate()
{}

#endif // USE_DOWNLOAD_MANAGER

bool DownloadController::handlePage(QWebPage * page)
{
    bool succeeded = true;

    // Handle click on link when the link type is not supported.
    page->setForwardUnsupportedContent(true);
    if (!connect(page, SIGNAL(unsupportedContent(QNetworkReply *)),
            this, SLOT(startDownload(QNetworkReply *)))) {
        succeeded = false;
    };

    // Handle Save Link and Save Image requests from the context menu.
    if (!connect(page, SIGNAL(downloadRequested(const QNetworkRequest &)),
            this, SLOT(startDownload(const QNetworkRequest &)))) {
        succeeded = false;
    }

    return succeeded;
}
