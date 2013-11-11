/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *     Send message operation implementation.
 */

#include "nmapiheaders.h"

namespace EmailClientApi 
{

NmApiOperationSendMessage::NmApiOperationSendMessage(const NmApiMessage &message, CFSMailBox &mailBox, QObject *parent)
    :NmApiOperation(parent, Succeeded, SendMessage),
     mMessage(message),
     mMailBox(mailBox),
     mRequestId(NmApiNoError)  
     
{
    NM_FUNCTION;
}

NmApiOperationSendMessage::~NmApiOperationSendMessage()
{
    NM_FUNCTION;
}

void NmApiOperationSendMessage::start()
{
    NM_FUNCTION;

    NmId id(mMessage.envelope().id());
    TFSMailMsgId fsId(id);

    RPointerArray<CFSMailPlugin> mailPlugins;
    NmApiEngine *engine = NmApiEngine::instance();
    if (engine) {
        engine->listMailPlugins(mailPlugins);
    }
    
    CFSMailPlugin *plugin = NULL;
    CFSMailMessage *fsMessage = NULL;
    TFSMailMsgId fsMailBoxId = mMailBox.GetId();
    TFSMailMsgId fsFolderId = TFSMailMsgId();
    TFSMailMsgId fsMessageId = TFSMailMsgId(fsId);
    
    for (int i = 0; i < mailPlugins.Count() && !fsMessage; i++){
        plugin = mailPlugins[i];
        if (plugin && (plugin->Id() == fsMailBoxId.PluginId())) {
            TRAPD(err, fsMessage = plugin->GetMessageByUidL(fsMailBoxId,
                    fsFolderId,
                    fsMessageId,
                    EFSMsgDataStructure));
            Q_UNUSED(err);
            if (fsMessage) {
                break;
            }
        }
    }
    
    if (fsMessage) {
        TRAPD(mail_err, mMailBox.SendMessageL(*fsMessage, *this));
        if (mail_err == KErrNone){
            mStatus = Pending;
        }
        else {
            mStatus = Failed;
            emit operationComplete(NmApiGeneralError);
        }
    } else {
        mStatus = Failed;
        emit operationComplete(NmApiGeneralError);
    }
    if (fsMessage) {
        delete fsMessage;
    }
    
    NmApiEngine::releaseInstance(engine);
}

void NmApiOperationSendMessage::cancel()
{
    NM_FUNCTION;
}

void NmApiOperationSendMessage::RequestResponseL( TFSProgress aEvent, TInt aRequestId )
{
    NM_FUNCTION;
    
    TFSProgress::TFSProgressStatus status = aEvent.iProgressStatus;

    if (aRequestId == mRequestId) {
        if (status == TFSProgress::EFSStatus_RequestComplete && aEvent.iParam) {
            mStatus = Succeeded;
            emit operationComplete(NmApiNoError);
        }
        else if (status == TFSProgress::EFSStatus_RequestCancelled) {
            mStatus = Canceled;
            emit operationComplete(NmApiCancelError);
        }
        else {
            mStatus = Failed;
            emit operationComplete(NmApiGeneralError);
        }
    }
}
}
