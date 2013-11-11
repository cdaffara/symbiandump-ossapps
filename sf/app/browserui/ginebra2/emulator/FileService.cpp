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

#include "FileService.h"
#include <xqserviceutil.h>
#include "webpagecontroller.h"
#include "Utilities.h"


FileService::FileService(QObject* parent)
    : XQServiceProvider(QLatin1String("NokiaBrowser.com.nokia.symbian.IFileView"), parent)
{
    publishAll();
}


FileService::~FileService()
{
}


// Handles loadFinished signal emitted by WebPageController. This signal
// indicates the completion of the request.
void FileService::completeAsyncRequest(bool ok)
{
    // completing all requests on current page load not technically correct but
    // not required to service more than one request at a time
    foreach (int reqId, mAsyncReqIds) {
        if (!completeRequest(reqId, QVariant(ok))) {
            // failed to complete request
            RemoveAsyncReqId();
        }
        // else remove async request ID on returnValueDelivered() signal
    }
    
    // disconnect slots connected to WebPageController signals
    disconnect(WebPageController::getSingleton(), 0, this, 0);
}


// View operation / slot for non-data-caged file.
bool FileService::view(QString file)
{
    XQRequestInfo info = requestInfo();

    if (!info.isSynchronous()) {
        // not required to service more than one request at a time
        // but just in case we use a map for request IDs
        // request ID needed to complete request
        mAsyncReqIds.insertMulti(info.clientSecureId(), setCurrentRequestAsync());
        safe_connect(this, SIGNAL(returnValueDelivered()), this, SLOT(handleAnswerDelivered()));
        safe_connect(this, SIGNAL(clientDisconnected()), this, SLOT(handleClientDisconnect()));
        safe_connect(WebPageController::getSingleton(), SIGNAL(loadFinished(bool)), this, SLOT(completeAsyncRequest(bool)));
    }
    
    // Load specified file in current window.
    file.prepend("file:///"); // create full URL from file path
    WebPageController::getSingleton()->loadInitialUrlFromOtherApp(file);
    
    return true;
}


// Removes request from asynchronous IDs. This should be done after request
// handled or on client disconnect.
void FileService::RemoveAsyncReqId()
{
    XQRequestInfo info = requestInfo();
    
    // Remove request from asynchronous IDs.
    mAsyncReqIds.remove(info.clientSecureId());
    
    // Disconnect signal from this slot if no more asynchronous requests.
    if (!asyncAnswer()) {
        // Disconnect all signals from this object to slots in this object.
        disconnect(this, 0, this, 0);
    }
}

// Handles clientDisconnected signal emitted by base class, XQServiceProvider.
// It's emitted if client accessing a service application terminates. 
void FileService::handleClientDisconnect()
{
    RemoveAsyncReqId();
}


// Handles returnValueDelivered signal emitted by base class, XQServiceProvider.
// It's emitted when asynchronous request has been completed and its return 
// value has been delivered to the service client.
void FileService::handleAnswerDelivered()
{
    RemoveAsyncReqId();
}


