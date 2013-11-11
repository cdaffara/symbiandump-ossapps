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

#include "downloadproxy_p.h"

#include <QNetworkReply>
#include <QtDebug>

#ifdef USE_DOWNLOAD_MANAGER
#include "download.h"
#include "downloadmanager.h"
#endif

DownloadProxyData::DownloadProxyData(WRT::Download * download)
: m_download(download)
{}

DownloadProxyData::~DownloadProxyData()
{
    // Nothing to do, Download * is owned by DownloadManager.
}


// Helper functions for translating various download attribute enum values.
#ifdef USE_DOWNLOAD_MANAGER
static const char * downloadState(int state)
{
    switch (state) {
    case WRT::Download::Created:
	return "Created";
    case WRT::Download::Started:
	return "Started";
    case WRT::Download::InProgress:
	return "InProgress";
    case WRT::Download::Paused:
	return "Paused";
    case WRT::Download::Completed:
	return "Completed";
    case WRT::Download::Failed:
	return "Failed";
    default:
        return "???";
    }
}

static const char * downloadError(QNetworkReply::NetworkError error)
{
    switch (error) {
    case QNetworkReply::NoError:
        return "QNetworkReply::NoError";
    case QNetworkReply::ConnectionRefusedError:
        return "QNetworkReply::ConnectionRefusedError";
    case QNetworkReply::RemoteHostClosedError:
        return "QNetworkReply::RemoteHostClosedError";
    case QNetworkReply::HostNotFoundError:
        return "QNetworkReply::HostNotFoundError";
    case QNetworkReply::TimeoutError:
        return "QNetworkReply::TimeoutError";
    case QNetworkReply::OperationCanceledError:
        return "QNetworkReply::OperationCanceledError";
    case QNetworkReply::SslHandshakeFailedError:
        return "QNetworkReply::SslHandshakeFailedError";
    case QNetworkReply::ProxyConnectionRefusedError:
        return "QNetworkReply::ProxyConnectionRefusedError";
    case QNetworkReply::ProxyConnectionClosedError:
        return "QNetworkReply::ProxyConnectionClosedError";
    case QNetworkReply::ProxyNotFoundError:
        return "QNetworkReply::ProxyNotFoundError";
    case QNetworkReply::ProxyTimeoutError:
        return "QNetworkReply::ProxyTimeoutError";
    case QNetworkReply::ProxyAuthenticationRequiredError:
        return "QNetworkReply::ProxyAuthenticationRequiredError";
    case QNetworkReply::ContentAccessDenied:
        return "QNetworkReply::ContentAccessDenied";
    case QNetworkReply::ContentOperationNotPermittedError:
        return "QNetworkReply::ContentOperationNotPermittedError";
    case QNetworkReply::ContentNotFoundError:
        return "QNetworkReply::ContentNotFoundError";
    case QNetworkReply::AuthenticationRequiredError:
        return "QNetworkReply::AuthenticationRequiredError";
    case QNetworkReply::ContentReSendError:
        return "QNetworkReply::ContentReSendError";
    case QNetworkReply::ProtocolUnknownError:
        return "QNetworkReply::ProtocolUnknownError";
    case QNetworkReply::ProtocolInvalidOperationError:
        return "QNetworkReply::ProtocolInvalidOperationError";
    case QNetworkReply::UnknownNetworkError:
        return "QNetworkReply::UnknownNetworkError";
    case QNetworkReply::UnknownProxyError:
        return "QNetworkReply::UnknownProxyError";
    case QNetworkReply::UnknownContentError:
        return "QNetworkReply::UnknownContentError";
    case QNetworkReply::ProtocolFailure:
        return "QNetworkReply::ProtocolFailure";
    default:
        return "???";
    }
}

// Helper functions to get download attribute of a particular type.

static int intAttribute(WRT::Download * download, WRT::DownloadAttribute which)
{
    return download->attribute(which).toInt();
}

static uint uintAttribute(WRT::Download * download, WRT::DownloadAttribute which)
{
    return download->attribute(which).toUInt();
}

static QString stringAttribute(WRT::Download * download, WRT::DownloadAttribute which)
{
    return download->attribute(which).toString();
}

// Helper functions for reporting download attributes.

template<typename T>
void debugDownloadAttribute(
        WRT::Download * download,
        char const * name,
        const T & value)
{
    qDebug() << "DL" << download->id() << name << value;
}

static void debugDownloadInt(
    WRT::Download * download,
    WRT::DownloadAttribute which,
    const char * name)
{
    int value = intAttribute(download, which);

    debugDownloadAttribute(download, name, value);
}

static void debugDownloadUInt(
    WRT::Download * download,
    WRT::DownloadAttribute which,
    const char * name)
{
    uint value = uintAttribute(download, which);

    debugDownloadAttribute(download, name, value);
}

static void debugDownloadStr(
    WRT::Download * download,
    WRT::DownloadAttribute which,
    const char * name)
{
    QString value = stringAttribute(download, which);

    debugDownloadAttribute(download, name, value);
}

static void debugDownloadState(WRT::Download * download)
{
    int num = intAttribute(download, WRT::State);

    const char * state = downloadState(num);

    debugDownloadAttribute(download, "DownloadState", state);

}

static void debugDownloadError(WRT::Download * download)
{
    int num = intAttribute(download, WRT::LastError);

    const char * error = downloadError(static_cast<QNetworkReply::NetworkError>(num));

    debugDownloadAttribute(download, "DownloadError", error);
}

// Helper function for implementing DownloadProxyData::debug().

static void debugDownload(WRT::Download * download)
{
    debugDownloadState(download);

    debugDownloadError(download);

    debugDownloadStr(download,
            WRT::LastErrorString,
            "LastErrorString");

    debugDownloadStr(download,
            WRT::SourceUrl,
            "SourceUrl");

    debugDownloadStr(download,
            WRT::ContentType,
            "ContentType");

    debugDownloadStr(download,
            WRT::DestinationPath,
            "DestinationPath");

    debugDownloadStr(download,
            WRT::FileName,
            "FileName");

    debugDownloadInt(download,
            WRT::DownloadedSize,
            "DownloadedSize");

    debugDownloadInt(download,
            WRT::TotalSize,
            "TotalSize");

    debugDownloadInt(download,
            WRT::LastPausedSize,
            "LastPausedSize");

    debugDownloadInt(download,
            WRT::Percentage,
            "Percentage");

    debugDownloadStr(download,
            WRT::StartTime,
            "StartTime");

    debugDownloadStr(download,
            WRT::EndTime,
            "EndTime");

    debugDownloadUInt(download,
            WRT::ElapsedTime,
            "ElapsedTime");

    debugDownloadStr(download,
            WRT::RemainingTime,
            "RemainingTime");

    debugDownloadStr(download,
            WRT::Speed,
            "Speed");

    debugDownloadInt(download,
            WRT::ProgressInterval,
            "ProgressInterval");
}
#endif

void DownloadProxyData::debug()
{
#ifdef USE_DOWNLOAD_MANAGER
    if (m_download == 0) {
        return;
    }

    int id = m_download->id();

    WRT::DownloadManager * manager = m_download->downloadManager();

    WRT::Download * download = manager->findDownload(id);

    if (download != 0) {
        debugDownload(download);
    }
#endif
}

void DownloadProxyData::remove()
{
#ifdef USE_DOWNLOAD_MANAGER
    if (m_download == 0) {
        return;
    }

    WRT::DownloadManager * manager = m_download->downloadManager();

    manager->removeOne(m_download);

    // Download is no longer valid.

    m_download = 0;
#endif
}

QString DownloadProxyData::fileName()
{
#ifdef USE_DOWNLOAD_MANAGER
    if (m_download == 0) {
        return QString();
    }

    return stringAttribute(m_download, WRT::FileName);
#else
    return QString();
#endif
}
