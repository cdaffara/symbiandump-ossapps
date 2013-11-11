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
* Description:
*
*/


#include "nmuiheaders.h"

static const char *NMUI_DOC_DEFAULT_IMAGE = ":/trolltech/styles/commonstyle/images/file-16.png";

/*!
    \class NmEditorTextDocument
    \brief Text document with URL image fetching support
*/

/*!
    Constructor. Caller needs to ensure manager is valid reference.
*/
NmEditorTextDocument::NmEditorTextDocument(QNetworkAccessManager &manager) :
    mManager(manager)
{
    NM_FUNCTION;
    
    connect(&mManager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(replyFinished(QNetworkReply*)));
}

/*!
    Destructor
*/
NmEditorTextDocument::~NmEditorTextDocument()
{
    NM_FUNCTION;
    
    foreach(QNetworkReply *reply, mReplyList) {
        if(reply) {
            reply->abort();
            reply->deleteLater();
        }
    }
}

/*!
    replyFinished. Signaled when the image has been loaded from Internet.
*/
void NmEditorTextDocument::replyFinished(QNetworkReply *reply)
{
    NM_FUNCTION;
    
    if(reply) {
        if(reply->error() == QNetworkReply::NoError) {
            QPixmap image;
            if(image.loadFromData(reply->readAll())) { 
                addResource(QTextDocument::ImageResource, reply->url(), QVariant(image));
                emit documentLayoutChanged();
            }
        }
        // If this has created the request, then this needs to handle deletion also.
        if (mReplyList.removeAll(reply)) {
            reply->deleteLater();
        }
    }
}

/*!
    loadResource. Starts fetching image from given URL.
*/
QVariant NmEditorTextDocument::loadResource(int type, const QUrl &name)
{
    NM_FUNCTION;
    
    QVariant retVal;
    
    if(type == QTextDocument::ImageResource) {    
        // Load image data from the cache if it exists
        if(mManager.cache()) {
            QIODevice *ioDevice = mManager.cache()->data(name);
            if(ioDevice) {
                QPixmap image;
                if(image.loadFromData(ioDevice->readAll())) {
                    addResource(QTextDocument::ImageResource, name, QVariant(image));
                    retVal = QVariant(image);
                }
                delete ioDevice;
            }
        }
        
        // Loading from cache failed, try from internet.
        if(retVal.isNull()) {
            // Make a request to load the image from Internet. Store the reply
            // to be able to abort it if necessary.
            QNetworkRequest request(name);
            mReplyList.append(mManager.get(request));
            
            // Load Qt's default image to prevent multiple calls to loadResource.
            // This function keeps on getting called repeatedly until 
            // resource is added. 
            QPixmap image = QPixmap(QLatin1String(NMUI_DOC_DEFAULT_IMAGE));
            addResource(QTextDocument::ImageResource, name, QVariant(image));
            retVal = image;
        }
    }
    
    // Could not find the resource -> try Qt implementation
    if(retVal.isNull()) {
        retVal = QTextDocument::loadResource(type, name);
    }
    return retVal;
}
