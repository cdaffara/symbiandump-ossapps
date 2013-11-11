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

NmFwaStoreMessageOperation::NmFwaStoreMessageOperation(
        CFSMailMessage *message,
        CFSMailClient &mailClient) :
		mMessage(message),
        mMailClient(mailClient),
        mRequestId(NmNotFoundError),
        mStatus(EStoreHeader)
{
    NM_FUNCTION;
}

NmFwaStoreMessageOperation::~NmFwaStoreMessageOperation()
{
    NM_FUNCTION;
    
    doCancelOperation();
    delete mMessage;
}

void NmFwaStoreMessageOperation::doRunAsyncOperation()
{
    NM_FUNCTION;
    
    TInt err = KErrNone;

    if (mMessage) {
        switch (mStatus) {
            case EStoreHeader: {
                TRAP(err, mRequestId = mMessage->SaveMessageL(*this));
                if (err != KErrNone) {
                    completeOperation(NmGeneralError);
                }
                mStatus = EStoreSubParts;
                break;
            }
            case EStoreSubParts: {
                TRAP(err, mRequestId = mMessage->SaveMessagePartsL(*this));
                if (err != KErrNone) {
                    completeOperation(NmGeneralError);
                }
                mStatus = EComplete;
                break;
            }
            case EComplete: {
                completeOperation(NmNoError);
                break;
            }

            default: {
                completeOperation(NmNotFoundError);
                break;
            }
        }
    } else {
        completeOperation(NmGeneralError);
    }
}

/*!
    
 */
void NmFwaStoreMessageOperation::doCompleteOperation()
{
    NM_FUNCTION;
    
    mRequestId = NmNotFoundError;
}

void NmFwaStoreMessageOperation::doCancelOperation()
{
    NM_FUNCTION;
    
    if (mRequestId >= 0) {
        TRAP_IGNORE(mMailClient.CancelL(mRequestId));
        mRequestId = NmNotFoundError;
    }
}

/**
 * asynchronous request response message
 *
 * @param aEvent plugin event description
 * @param aRequestId request id of asyncronous operation
 */
void NmFwaStoreMessageOperation::RequestResponseL(TFSProgress aEvent,
                                                     TInt aRequestId)
{
    NM_FUNCTION;
    
    if (aRequestId == mRequestId) {
        if (aEvent.iProgressStatus ==
                TFSProgress::EFSStatus_RequestComplete) {
            if(mStatus != EComplete) {
                doRunAsyncOperation();
            } else {
                completeOperation(NmNoError);
            }
        }
        else if (aEvent.iProgressStatus ==
                TFSProgress::EFSStatus_RequestCancelled) {
            completeOperation(NmCancelError);
        }
        else {
            completeOperation(NmGeneralError);
        }
    }
}
