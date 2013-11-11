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
 * Description:Message service launch utility
 *
 */

// SYSTEM INCLUDES
#include <xqaiwrequest.h>
#include <xqrequestinfo.h>
#include <xqappmgr.h>
#include <hbglobal.h>
#include <xqconversions.h>

#include "msgservicelaunchutil.h"
#include "msgcontactsutil.h"
#include "msgservicelaunchutilprivate.h"
#include "debugtraces.h"

// LOCAL CONSTANTS
#define LOC_TITLE   hbTrId("txt_messaging_title_messaging")
const QString VCARD_MIMETYPE("text/X-vCard");

//--------------------------------------------------------------
// MsgServiceLaunchUtil::MsgServiceLaunchUtil
// Constructor
//--------------------------------------------------------------
MsgServiceLaunchUtil::MsgServiceLaunchUtil(QObject* parent):
QObject(parent)
{
    //nothing required
}

//--------------------------------------------------------------
// MsgServiceLaunchUtil::~MsgServiceLaunchUtil
// Destructor
//--------------------------------------------------------------
MsgServiceLaunchUtil::~MsgServiceLaunchUtil()
{
    //nothing required
}

//---------------------------------------------------------------
// MsgServiceLaunchUtil::launchContentViewer
// @see header file
//---------------------------------------------------------------
void MsgServiceLaunchUtil::launchContentViewer(
        const QString &mimeType, 
        const QString &filePath)
{
    if (mimeType.contains(VCARD_MIMETYPE, Qt::CaseInsensitive)) {
        MsgContactsUtil::launchVCardViewer(filePath);
    }
    else {
        MsgServiceLaunchUtil::launchViaSharableFile(filePath);
    }
}

//---------------------------------------------------------------
// MsgServiceLaunchUtil::launchContentViewer
// @see header file
//---------------------------------------------------------------
void MsgServiceLaunchUtil::launchContentViewer(
        int messageId)
{
    // get the mimeType and filepath from messageId
    CMsgServiceLaunchUtilPrivate* util_p = 0;
        
    TRAPD(error, util_p = CMsgServiceLaunchUtilPrivate::NewL());
    
    if(error == KErrNone)
        {
        HBufC* fileName = 0;
        fileName = util_p->GetMessagePath(messageId, error);
        
        if(error == KErrNone)
            {
            QString attachmentFName = XQConversions::s60DescToQString(
                    fileName->Des());
            QString mimeType = XQConversions::s60Desc8ToQString(
                    util_p->GetMimeType()->Des());
            
            launchContentViewer(mimeType, attachmentFName);
                
            }
        if (fileName){
            delete fileName;
        }
        }
    if(util_p){
        delete util_p; 
    }    
}

//---------------------------------------------------------------
// MsgServiceLaunchUtil::launchViaSharableFile
// @see header file
//---------------------------------------------------------------
void MsgServiceLaunchUtil::launchViaSharableFile(
        const QString &filePath)
{
    XQSharableFile sf;
    XQAiwRequest* request = 0;

    if (!sf.open(filePath)) {
        return;
    }

    // Get handlers
    XQApplicationManager appManager;
    QList<XQAiwInterfaceDescriptor> fileHandlers = appManager.list(sf);
    if (fileHandlers.count() > 0) {
        XQAiwInterfaceDescriptor d = fileHandlers.first();
        request = appManager.create(sf, d);

        if (!request) {
            sf.close();
            return;
        }
    }
    else {
        sf.close();
        return;
    }

    // Result handlers
    connect(request, 
            SIGNAL(requestOk(const QVariant&)), 
            this, 
            SLOT(handleOk(const QVariant&)),
            Qt::UniqueConnection);
    connect(request, 
            SIGNAL(requestError(int,const QString&)), 
            this,
            SLOT(handleError(int,const QString&)), 
            Qt::UniqueConnection);

    request->setEmbedded(true);
    request->setSynchronous(false);

    // Fill args
    QList<QVariant> args;
    args << qVariantFromValue(sf);
    request->setArguments(args);

    // Fill headers
    QString key("WindowTitle");
    QVariant value(QString(LOC_TITLE));
    XQRequestInfo info;
    info.setInfo(key, value);
    request->setInfo(info);

    request->send();

    // Cleanup
    sf.close();
    delete request;
}

//---------------------------------------------------------------
// MsgServiceLaunchUtil::handleOk
// @see header file
//---------------------------------------------------------------
void MsgServiceLaunchUtil::handleOk(
        const QVariant& result)
{
   //do nothing
    Q_UNUSED(result)
}

//---------------------------------------------------------------
// MsgServiceLaunchUtil::handleError
// @see header file
//---------------------------------------------------------------
void MsgServiceLaunchUtil::handleError(
        int errorCode, 
        const QString& errorMessage)
{
    //do nothing
    Q_UNUSED(errorCode)
    Q_UNUSED(errorMessage)
}

//eof
