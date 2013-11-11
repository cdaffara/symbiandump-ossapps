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

#ifndef __WEBNETWORKACCESSMANAGER_H__
#define __WEBNETWORKACCESSMANAGER_H__

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "messageboxproxy.h"
#include "SchemeHandlerBr.h"

#if QT_VERSION >= 0x040500
#include <QNetworkDiskCache>

#endif

namespace WRT {

class WrtBrowserContainer;
class CookieJar;


class WebNetworkAccessManager : public QNetworkAccessManager,
                                public MessageBoxProxy
{
    Q_OBJECT

public:
    WebNetworkAccessManager(WrtBrowserContainer* page, QObject* parent = 0);
    virtual ~WebNetworkAccessManager();

    void onMessageBoxResponse(int retValue);
    int activeNetworkInterfaces(); 
    
public slots:

protected:
    virtual QNetworkReply *createRequest(Operation op, const QNetworkRequest &request,
                                         QIODevice *outgoingData = 0);
private:
    void setupCache();
    void setupNetworkProxy();
    //Handle connection request.
    QNetworkReply* createRequestHelper(Operation op, const QNetworkRequest &request, QIODevice *outgoingData = 0);

private slots:
    void onfinished(QNetworkReply* reply);
    
private:
    WrtBrowserContainer* m_browserContainer;
    CookieJar* m_cookieJar;
    QNetworkReply* m_reply;
    QNetworkRequest* m_req;
    SchemeHandler::SchemeHandlerError m_schemeError;

#if QT_VERSION >= 0x040500
    QNetworkDiskCache *qDiskCache;

#endif

signals:
    void showMessageBox(WRT::MessageBoxProxy* data);
    void networkErrorHappened(const QString & msg); 
    void networkErrorUrl(const QUrl & url); 
};
}
#endif
