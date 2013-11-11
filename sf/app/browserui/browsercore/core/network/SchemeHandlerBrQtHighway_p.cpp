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

// INCLUDE FILES
#include <QUrl>
#include <QList>
#include <xqaiwdecl.h>

#include "SchemeHandlerBrQtHighway_p.h"


namespace WRT {


QtHighwaySchemeHandlerPrivate::QtHighwaySchemeHandlerPrivate()
{
}

QtHighwaySchemeHandlerPrivate::~QtHighwaySchemeHandlerPrivate()
{
}

SchemeHandler::SchemeHandlerError QtHighwaySchemeHandlerPrivate::HandleScheme(const QUrl &url)
{
    return SendStandardUriViewRequest(url);
}


/*!
    Creates request with "com.nokia.symbian.IUriView" interface name and 
    "view(QString)" operation. QtHighway uses the \a url parameter to
    determine the service application and the service application 
    uses it as data. The request is asynchronous. Application launched
    as stand-alone app, not embedded.
*/
SchemeHandler::SchemeHandlerError 
    QtHighwaySchemeHandlerPrivate::SendStandardUriViewRequest(const QUrl& url)
{
    SchemeHandler::SchemeHandlerError retVal = SchemeHandler::NoError;
    XQAiwRequest *request = NULL; // Application Interworking request
    bool embedded = false;  // window groups not chained

    // Create request - Apply first implementation of url, "com.nokia.symbian.IUriView"
    // interface name and "view(QString)" operation.
    request = mAiwMgr.create(url, embedded);
    
    if (request) {
        // Debug - what service and interface are we using?
        //XQAiwInterfaceDescriptor const &desc = request->descriptor();
        //qDebug() << "HandleTelScheme: sn=" << desc.serviceName() << "if=" << desc.interfaceName();
    
        // Set function parameters
        QList<QVariant> args;
        args << url.toString();
        request->setArguments(args);
        request->setEmbedded(embedded);
        request->setSynchronous(false); // asynchronous request

        // Send the request
        if (!request->send()) {
            // to debug get error code from request->lastError()
            // or connect to requestError() signal
            retVal = SchemeHandler::LaunchFailed;
        }
    } else {
        // failed to create request - maybe scheme is unsupported
        retVal = SchemeHandler::SchemeUnsupported;
    }
    
    // if need to get service result connect to requestOk() signal
    
    // currently not interested in requestOk() or requestError() signals so
    // OK to delete request now
    delete request;
    
    return retVal;
}

} // WRT
