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
 *     Draft message creation operation implementation.
 */

#include "nmapiheaders.h"


/*!
    Class constructor.
*/
NmApiOperationCreateDraftMessage::NmApiOperationCreateDraftMessage(const QVariant *initData, 
                                                                   CFSMailBox &mailBox,
                                                                   QObject *parent)
    :NmApiOperation(parent, NotStarted, CreateDraftMessage),
     mInitData(initData),
     mMailBox(mailBox),
     mRequestId(NmApiNoError)
{
    NM_FUNCTION;
}

/*!
    Class destructor.
*/
NmApiOperationCreateDraftMessage::~NmApiOperationCreateDraftMessage()
{
    NM_FUNCTION;
}

/*!
 \fn start 
 
 Starts create draft message operation for given mailboxid. 
 Created draft message signaled with operationComplete(int resultCode, QVariant data = QVariant()). 
 data parameter is type of NmApiMessage.
 */
void NmApiOperationCreateDraftMessage::start()
{
    NM_FUNCTION;
    TRAPD(err, mMailBox.CreateMessageToSendL(*this));
    if (err == KErrNone){
        mStatus = Pending;
    }
    else {
        mStatus = Failed;
        emit operationComplete(NmApiGeneralError);
    }
}

/*!
 \fn cancel 
 
 Cancel not supported in create draft message operation. 
 */
void NmApiOperationCreateDraftMessage::cancel()
{
    NM_FUNCTION;
}

/*!
    Asynchronous request response message.
    
    \param aEvent Plugin event description.
    \param aRequestId Request id of asyncronous operation.
 */
void NmApiOperationCreateDraftMessage::RequestResponseL(TFSProgress aEvent,
                                                     TInt aRequestId)
{
    NM_FUNCTION;
    
    TFSProgress::TFSProgressStatus status = aEvent.iProgressStatus;

    if (aRequestId == mRequestId) {
        if (status == TFSProgress::EFSStatus_RequestComplete && aEvent.iParam) {
            CFSMailMessage *fsMessage = static_cast<CFSMailMessage*>(aEvent.iParam);
            NmMessage *nmmessage = fsMessage->GetNmMessage();
            EmailClientApi::NmApiMessage apimessage = NmToApiConverter::NmMessage2NmApiMessage(*nmmessage);
            QVariant qmessage;
            qmessage.setValue(apimessage);
            mStatus = Succeeded;
            emit operationComplete(NmApiNoError, qmessage);
            
            delete nmmessage;
            nmmessage = NULL;
            
            delete fsMessage;
            fsMessage = NULL;
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

