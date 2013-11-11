/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: NMail viewer net manager implementation.
*              This class is needed to separate cid
*              images from url-based images
*/

#include "nmuiheaders.h"

static const QString NmViewerViewNetManagerScheme("cid");

/*!
	\class NmViewerViewNetManager
	\brief Mail viewer network access manager class
*/


/*!
    Constructor
*/
NmViewerViewNetManager::NmViewerViewNetManager(NmUiEngine &uiEngine)
    : QNetworkAccessManager(),
      mUiEngine(uiEngine),
      mMessageView(NULL)
{
    NM_FUNCTION;
}

/*!
    Destructor
*/
NmViewerViewNetManager::~NmViewerViewNetManager()
{
    NM_FUNCTION;
}

/*!
    Setter for view.
*/
void NmViewerViewNetManager::setView(NmViewerView *viewerView)
{
    NM_FUNCTION;
    
    mMessageView=viewerView;
}

/*!
    createRequest
*/
QNetworkReply *NmViewerViewNetManager::createRequest(
    Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    NM_FUNCTION;
    
    QNetworkRequest myRequest(request);
    // Set request attribute to prefer cachevar  
    const QVariant cacheControl((int)QNetworkRequest::PreferCache);
    myRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, 
                         cacheControl);
    const QUrl requestUrl = myRequest.url();
    // Check whether request is for embedded image
    if (mMessageView&&mMessageView->webView()&&op==QNetworkAccessManager::GetOperation
        && requestUrl.scheme()==NmViewerViewNetManagerScheme) {
        QString id = requestUrl.path();
        NmId partId;
        bool isFetched(false);
        NmMessage *message = mMessageView->message();
        if (message) {
            QVariant data = mMessageView->webView()->loadResource(
                    QTextDocument::ImageResource, requestUrl, partId, isFetched);
            NmViewerViewNetReply* reply(NULL);
            if (isFetched) {
                reply = new NmViewerViewNetReply(data, mUiEngine);
            }
            else {
                const NmMessageEnvelope &env = message->envelope();
                reply = new NmViewerViewNetReply(data, mUiEngine,
                        env.mailboxId(), env.folderId(),
                        env.messageId(), partId);
            }
            reply->setOriginalRequest(myRequest);
            return reply;
        }
    }
    // If request is not for embedded image, forward to base class
    return QNetworkAccessManager::createRequest(op, myRequest, outgoingData);
}
