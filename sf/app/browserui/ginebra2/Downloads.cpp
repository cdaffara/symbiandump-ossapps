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

#include "Downloads.h"
#include "Utilities.h"

#include <QtDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QWebPage>

#include "bedrockprovisioning.h"
#include "downloadcontroller.h"
#include "downloadproxy.h"

namespace GVA {

Downloads::Downloads()
: m_downloadController(0)
{
    setObjectName("downloads");
}

Downloads::~Downloads()
{
    delete m_downloadController;
}

void Downloads::handlePage(QWebPage * page)
{
    if (m_downloadController == 0) {
        QString client = BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->applicationName();

        QNetworkAccessManager * accessManager = page->networkAccessManager();
        QNetworkProxy proxy = accessManager->proxy();

        m_downloadController = new DownloadController(client, proxy);

        safe_connect(m_downloadController, SIGNAL(downloadCreated(DownloadProxy)),
                this, SLOT(reportDownloadCreated(DownloadProxy)));

        safe_connect(m_downloadController, SIGNAL(downloadStarted(DownloadProxy)),
                this, SLOT(reportDownloadStarted(DownloadProxy)));

        safe_connect(m_downloadController, SIGNAL(downloadFinished(DownloadProxy)),
                this, SLOT(reportDownloadSuccess(DownloadProxy)));

        safe_connect(m_downloadController, SIGNAL(downloadFailed(DownloadProxy, const QString &)),
                this, SLOT(reportDownloadFailure(DownloadProxy, const QString &)));

        safe_connect(m_downloadController, SIGNAL(downloadPaused(DownloadProxy, const QString &)),
                this, SLOT(reportDownloadFailure(DownloadProxy, const QString &)));

        safe_connect(m_downloadController, SIGNAL(downloadCancelled(DownloadProxy, const QString &)),
                this, SLOT(reportDownloadFailure(DownloadProxy, const QString &)));

        safe_connect(m_downloadController, SIGNAL(downloadNetworkLoss(DownloadProxy, const QString &)),
                this, SLOT(reportDownloadFailure(DownloadProxy, const QString &)));

        // There is no argument to extract data from so we may as well
        // just connect signal to signal without going through another slot.
        safe_connect(m_downloadController, SIGNAL(downloadsCleared()),
                this, SIGNAL(downloadsCleared()));

        safe_connect(m_downloadController, SIGNAL(unsupportedDownload(const QUrl &)),
                this, SLOT(reportUnsupportedDownload(const QUrl &)));
    }

    m_downloadController->handlePage(page);
}

static QString imageFileName(const QUrl & url)
{
    QFileInfo info(url.path());

    QString fileName = info.fileName();

    if (fileName.isEmpty()) {
        return "image"; // ;;; localize?
    }

    return fileName;
}

static bool getSaveFileForImage(const QUrl & url, QFileInfo & saveInfo)
{
    QDir defaultDir = QDesktopServices::storageLocation(QDesktopServices::PicturesLocation);

    QString defaultFile = imageFileName(url);

    QString saveFile = QFileDialog::getSaveFileName(
            0,          // parent
            QString(),  // caption (doesn't show on Symbian)
            defaultDir.filePath(defaultFile),
            QString(),  // filter
            0,          // selected filter
            QFileDialog::DontConfirmOverwrite);

    if (saveFile.isEmpty()) {
        return false;
    }

    saveInfo.setFile(saveFile);
    return true;
}

void Downloads::downloadImage(const QString & imageUrl)
{
    QUrl url(imageUrl);

    QFileInfo saveInfo;

    if (!getSaveFileForImage(url, saveInfo)) {
        return;
    }

    m_downloadController->startDownload(url, saveInfo);
}

void Downloads::reportDownloadCreated(DownloadProxy downloadProxy)
{
    // Localize dialog message.

    QString fmt = qtTrId("txt_browser_downloading_file");
    QString msg = fmt.arg(downloadProxy.fileName());

    emit downloadCreated(msg);
}

void Downloads::reportDownloadStarted(DownloadProxy downloadProxy)
{
    // Localize dialog message.

    QString fmt = qtTrId("txt_browser_downloading_file");
    QString msg = fmt.arg(downloadProxy.fileName());

    emit downloadCreated(msg);
}

void Downloads::reportDownloadSuccess(DownloadProxy downloadProxy)
{
    // Localize dialog message.

    QString fmt = qtTrId("txt_browser_file_has_finished_downloading");
    QString msg = fmt.arg(downloadProxy.fileName());

    emit downloadSuccess(msg);

    // Don't forget to remove the download; otherwise the download
    // controller won't know to emit the DownloadsCleared signal.

    downloadProxy.remove();
}

void Downloads::reportDownloadFailure(DownloadProxy downloadProxy, const QString & error)
{
    // What went wrong?

    downloadProxy.debug();

    // Localize dialog message.

    QString fmt = qtTrId("txt_browser_tag_error_tag_file_could_not_be_downloaded");
    QString msg = fmt.arg(
            "<span style=\"color:red\">",
            "</span>",
            downloadProxy.fileName());

    emit downloadFailure(msg);

    // Don't forget to remove the download; otherwise the download
    // controller won't know to emit the DownloadsCleared signal.

    downloadProxy.remove();
}

void Downloads::reportUnsupportedDownload(const QUrl & url)
{
    emit unsupportedDownload("Unsupported content"); // ;;; localize? or not b/c this is temporary?
}

} // namespace GVA
