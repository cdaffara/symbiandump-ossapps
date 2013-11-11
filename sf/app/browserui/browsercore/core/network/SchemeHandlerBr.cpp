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
#ifndef QTHIGHWAY
#include "SchemeHandlerBrDefault_p.h"
#else // QTHIGHWAY defined
#include "SchemeHandlerBrQtHighway_p.h"
#endif

#include "SchemeHandlerBr.h"
#include "SchemeHandlerBr_p.h"
#include "bedrockprovisioning.h"


#define HTTP_SCHEME "http"
#define HTTPS_SCHEME "https"
#define FILE_SCHEME "file"


namespace WRT {

// ================= METHODS =======================

/*!
 * \class SchemeHandler
 * \brief Schemehandler class
 */
// ---------------------------------------------------------
// SchemeHandler::SchemeHandler()
// ---------------------------------------------------------
//
SchemeHandler::SchemeHandler() : QObject(), d(new SCHEME_HNDLR_PRIVATE())
{
}

SchemeHandler::~SchemeHandler()
{
    delete d;
}

/*!
    Handles specified URL if it is one of the known schemes. If supported, the
    url is sent to an external application to be handled.
*/
SchemeHandler::SchemeHandlerError SchemeHandler::HandleSpecialScheme(const QUrl &url)
{
    return d->HandleUrl(url, d->ReadSdConfirmDtmfValue());
}


// ================= SchemeHandlerPrivate METHODS =======================

/*!
 * \class SchemeHandlerPrivate
 * \brief SchemeHandlerPrivate class
 */
// ---------------------------------------------------------
// SchemeHandlerPrivate::SchemeHandlerPrivate()
// ---------------------------------------------------------
//
SchemeHandlerPrivate::SchemeHandlerPrivate()
{
}

SchemeHandlerPrivate::~SchemeHandlerPrivate()
{
}

/*!
    Returns ConfirmDTMF value as a bool.
*/
bool SchemeHandlerPrivate::ReadSdConfirmDtmfValue()
{
    return BEDROCK_PROVISIONING::BedrockProvisioning::createBedrockProvisioning()->value("ConfirmDTMF",false).toBool();
}

/*!
    Handles the specified URL. Calls the handler method corresponding to the 
    URL scheme.
*/
SchemeHandler::SchemeHandlerError SchemeHandlerPrivate::HandleUrl(const QUrl &url, bool /*confirmDTMF*/)
{
    QString scheme = url.scheme().toLower();
    SchemeHandler::SchemeHandlerError retVal = SchemeHandler::SchemeUnsupported;

    if ((scheme == HTTP_SCHEME) || (scheme == HTTPS_SCHEME) || 
        (scheme == FILE_SCHEME)) {
        // exit if scheme is http, https, or file
        retVal = SchemeHandler::SchemeNotHandled;
    } else {
        retVal = HandleScheme(url);
    }

    return retVal;
}

} // WRT
