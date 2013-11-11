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
    \class NmFwaAddAttachmentsOperation
    
    \brief NmFwaAddAttachmentsOperation is an async operation which adds new
           attachments into the message.
           
    NmFwaAddAttachmentsOperation is an async operation which adds new attachments into the message.
    \sa NmOperation
 */

/*!
    Constructor
    
    \param message Message where new attachment to be attached.
    \param fileList File name list of the attached files
    \param mailClient Reference to mail client object.
 */
NmFwaAddAttachmentsOperation::NmFwaAddAttachmentsOperation(
    const NmMessage &message,
    const QList<QString> &fileList,
    CFSMailClient &mailClient) :
        mMailClient(mailClient)
{
    NM_FUNCTION;
    
    // Take own copy of the file list.
    mFileList.clear();
    for (int i=0; i<fileList.count(); ++i) {
        mFileList.append(fileList.at(i));
    }
    mFSMessage = CFSMailMessage::NewL(message);
    mRequestId = NmNotFoundError;
}

/*!
    Destructor
 */
NmFwaAddAttachmentsOperation::~NmFwaAddAttachmentsOperation()
{
    NM_FUNCTION;
    
    // Cancel all running operations.
    doCancelOperation();
    mFileList.clear(); 
    delete mFSMessage;
}

/*!
    Called after base object construction, runs the
    async operation.
    
    \sa NmOperation
 */
void NmFwaAddAttachmentsOperation::doRunAsyncOperation()
{
    NM_FUNCTION;
    
    TRAPD(err, doRunAsyncOperationL());
    // Trap harness catches an error.
    if (err != KErrNone) {
        // File not found, inform UI.
        if (err == KErrNotFound) {
            completeOperationPart(mFileList.takeFirst(),
                                  NULL,
                                  NmNotFoundError);
            // Let's continue, if files still left.
            if (mFileList.count() > 0) {
                doRunAsyncOperation();
            }
            else {
                // We do not want to report whole operation error,
                // when the last file is, or files, are missing.
                completeOperation(NmNoError);
            }
        }
        else {
            // Complete operation part, because of unknown error.
            // File list is never empty in this situation.
            completeOperationPart(mFileList.takeFirst(),
                                  NULL,
                                  NmGeneralError);
            // Complete whole operation, because of unknown error.
            mFileList.clear();
            mRequestId = NmNotFoundError ;
            completeOperation(NmGeneralError);
        }
    }
}

/*!
    Leaving version of doRunAsyncOperationL
 */
void NmFwaAddAttachmentsOperation::doRunAsyncOperationL()
{
    NM_FUNCTION;
    
    if (mFileList.count() > 0) {
        // Add new attachment from first file in the list.
        HBufC *fileName = NmConverter::qstringToHBufCLC(mFileList.first());
        mRequestId = mFSMessage->AddNewAttachmentL(*fileName, *this);
        CleanupStack::PopAndDestroy(fileName);   
    } else {
        completeOperation(NmNoError);
    }
        
}

/*!
    Asynchronous request response message.
    
    \param aEvent Plugin event description.
    \param aRequestId Request id of asyncronous operation.
 */
void NmFwaAddAttachmentsOperation::RequestResponseL(TFSProgress aEvent,
                                                    TInt aRequestId)
{
    NM_FUNCTION;
    
    int err = NmNoError;

    // Request id should always be valid. If not,
    // then report general error and complete operation.
    if (aRequestId == mRequestId) {

        // Request completed so request id used.
        mRequestId = KErrNotFound;
       
        // If request completed we can process the results.
        //   Otherwise, we report about cancellation or general error
        // and stop the operation.
        if (aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete) {
            
            // Request completed. Let's check the result. If no error
            // and parameter not NULL, report UI about a file added.
            //   Else complete operation part with general error,
            // but continue if a file or files still left in the list.
            if (aEvent.iError == KErrNone && aEvent.iParam) {
                
                // Cast the parameter to FS message part object, since
                // it can't be anything else.
                CFSMailMessagePart *fsMessagePart =
                    static_cast<CFSMailMessagePart *>(aEvent.iParam);

                completeOperationPart(
                    mFileList.takeFirst(),
                    fsMessagePart->GetPartId().GetNmId(),
                    NmNoError);
                    
                delete fsMessagePart;
                fsMessagePart = NULL;
            }
            else {
                completeOperationPart(mFileList.takeFirst(),
                                      NULL,
                                      NmGeneralError);
            }
        }
        else {
            completeOperationPart(mFileList.takeFirst(),
                                  NULL,
                                  NmGeneralError);
            mFileList.clear();
            err = NmGeneralError;
        }
    } // if (aRequestId == mRequestId) 
    else {
        // Just to be sure.
        if (mFileList.count() > 0) {
            completeOperationPart(mFileList.takeFirst(),
                                  NULL,
                                  NmGeneralError);   
        }
        mFileList.clear();
        err = NmGeneralError;
    }
    
    // Complete operation if there are no files left.
    // Otherwise, continue with next file.
    if (!mFileList.count()) {
        completeOperation(err);
    } else {
        doRunAsyncOperation();
    }
}

/*!
 * Complete the operation
 */
void NmFwaAddAttachmentsOperation::doCompleteOperation()
{
    NM_FUNCTION;
    
    mRequestId = NmNotFoundError;
}

/*!
    Cancels the async operation. \sa NmOperation
 */
void NmFwaAddAttachmentsOperation::doCancelOperation()
{
    NM_FUNCTION;
    
    if (mRequestId >= 0) {
        TRAP_IGNORE(mMailClient.CancelL(mRequestId));
        mRequestId = NmNotFoundError;
    }
}
