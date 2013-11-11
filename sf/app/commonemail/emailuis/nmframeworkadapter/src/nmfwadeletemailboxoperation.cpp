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
#include "nmframeworkadapterheaders.h"

/*!
    \class NmFwaDeleteMailboxOperation
    
    \brief NmFwaDeleteMailboxOperation is an async operation which deletes a mailbox.
    
    NmFwaDeleteMailboxOperation is an async operation which deletes a mailbox. 
    \sa NmOperation
 */

/*!
    Constructor
    
    \param mailboxId Mailbox id to be deleted.
    \param mailClient Reference to mail client object.
 */
NmFwaDeleteMailboxOperation::NmFwaDeleteMailboxOperation(
    const NmId &mailboxId,
    CFSMailClient &mailClient) :
        mMailboxId(mailboxId),
        mMailClient(mailClient),
        mRequestId(NmNotFoundError)
{
    NM_FUNCTION;
    
    mMailClient.IncReferenceCount();
}

/*!
    Destructor
 */
NmFwaDeleteMailboxOperation::~NmFwaDeleteMailboxOperation()
{
    NM_FUNCTION;
    
    doCancelOperation();
    mMailClient.Close(); // decrease ref count
}

/*!
    Called after base object construction via timer event, runs the
    async operation.
    
    \sa NmOperation
 */
void NmFwaDeleteMailboxOperation::doRunAsyncOperation()
{
    NM_FUNCTION;
    
    TRAPD(err, deleteMailboxL());
        
    if (err != KErrNone) {
        completeOperation(NmGeneralError);
    }
}

/*!
    
 */
void NmFwaDeleteMailboxOperation::doCompleteOperation()
{
    NM_FUNCTION;
    
    mRequestId = NmNotFoundError;
}
   
/*!
    Asynchronous request response message.
    
    \param aEvent Plugin event description.
    \param aRequestId Request id of asyncronous operation.
 */
void NmFwaDeleteMailboxOperation::RequestResponseL(TFSProgress aEvent,
                                                    TInt aRequestId)
{
    NM_FUNCTION;
    
    TFSProgress::TFSProgressStatus status = aEvent.iProgressStatus;

    if (aRequestId == mRequestId) {
        if (status == TFSProgress::EFSStatus_RequestComplete && !aEvent.iError) {
            completeOperation(NmNoError);
        }
        else {
            completeOperation(NmGeneralError);
        }
    }
}

/*!
    Removes the message.
 */
void NmFwaDeleteMailboxOperation::deleteMailboxL()
{
    NM_FUNCTION;
    const TFSMailMsgId mailboxId(mMailboxId);
    mRequestId = mMailClient.DeleteMailBoxByUidL(mailboxId, *this);
}
