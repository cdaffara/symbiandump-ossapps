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
*
*/

#include "nmuiheaders.h"

static const int NmAttachmentManagerInitialProgressPercent = 5;

/*!
    \class NmAttachmentManager
    \brief Attachment download manager, shares attachment download between editor and viewer
*/

/*!

*/
NmAttachmentManager::NmAttachmentManager(NmUiEngine &uiEngine) :
    mUiEngine(uiEngine),
    mFetchOperation(NULL),
    mMsgFetchOperation(NULL),
    mFetchObserver(NULL),
    mFetchMsg(NULL),
    mAttaId(0),
    mProgressValue(0),
    mIsFetching(false)
{
    NM_FUNCTION;
}

/*!

*/
NmAttachmentManager::~NmAttachmentManager()
{
    NM_FUNCTION;
    
    // fetch operation deleted in cancel fetch
    cancelFetch();
    
    delete mFetchMsg;
    mFetchMsg = NULL;
}

/*!
    Starts attachment fetch. If earlier fetch operation exist it is cancelled.
    Set observer with setObserver method to get process and complete events
*/
void NmAttachmentManager::fetchAttachment(
        const NmId &mailboxId, 
        const NmId &folderId, 
        const NmId &messageId, 
        const NmId &messagePartId)
{
    NM_FUNCTION;
    // cancel old fetch operation, Does nothing if fetch not ongoing
    cancelFetch();

    mFetchOperation = mUiEngine.fetchMessagePart(
            mailboxId, folderId, messageId, messagePartId);
    
    if (mFetchOperation) {
        mAttaId = messagePartId;
        mIsFetching = true;
        QObject::connect(mFetchOperation, SIGNAL(operationCompleted(int)),
                this, SLOT(completeAttachmentFetch(int)));
        
        QObject::connect(mFetchOperation, SIGNAL(operationProgressChanged(int)),
                this, SLOT(changeProgress(int)));
        // set progress to 5 % already in start
        changeProgress(NmAttachmentManagerInitialProgressPercent);
    }
}

/*!
    Fetch attachments to a message. 
    Set observer with setObserver method to get progress and complete events
    
    \return bool true if fetch started, otherwise false.
*/
bool NmAttachmentManager::fetchAttachments(
        const NmId &mailboxId, 
        const NmId &folderId, 
        const NmId &messageId,
        QList<NmId> &messagePartIds)
{
    NM_FUNCTION;
    bool result(false);
    // cancel old fetch operation, Does nothing if fetch not ongoing
    cancelFetch();
    
    if (messagePartIds.count() > 0) {
        mFetchOperation = mUiEngine.fetchMessageParts(
            mailboxId,
            folderId,
            messageId,
            messagePartIds);
        
        if (mFetchOperation) {
            mAttaId = 0;
            mIsFetching = true;
            QObject::connect(mFetchOperation, SIGNAL(operationCompleted(int)),
                    this, SLOT(completeAttachmentFetch(int)));
            
            QObject::connect(mFetchOperation, SIGNAL(operationProgressChanged(int)),
                    this, SLOT(changeProgress(int)));
        
            result = true;
        }
    }
    return result;
}

/*!
    Fetch all message parts to a message. Fetches also message part data structure,
    if not fetched already.
      If earlier fetch operation exist it is cancelled and deleted.
    Set observer with setObserver method to get progress and complete events
*/
void NmAttachmentManager::fetchAllMessageParts(
        const NmId &mailboxId, 
        const NmId &folderId, 
        const NmId &messageId)
{
    NM_FUNCTION;
    mFetchMsg = mUiEngine.message(mailboxId,folderId,messageId);
    
    // Check if we have part data structure.    
    if (mFetchMsg && mFetchMsg->childParts().count() == 0 &&
        mFetchMsg->fetchedSize() < mFetchMsg->size()) {

        // cancel old fetch operation, Does nothing if fetch not ongoing
        cancelFetch();
        
        // Fetch the message.
        mMsgFetchOperation = mUiEngine.fetchMessage(
            mFetchMsg->envelope().mailboxId(),
            mFetchMsg->envelope().folderId(),
            mFetchMsg->envelope().messageId());
        
        mAttaId = 0;
        mIsFetching = true;

        if (mMsgFetchOperation) {
            QObject::connect(mMsgFetchOperation,
                    SIGNAL(operationCompleted(int)),
                    this,
                    SLOT(completeMessageFetch(int)));
        }
        else {
            completeMessageFetch(NmGeneralError);
        }
        
    }
    else {
        completeMessageFetch(NmNoError);
    }
}

/*!
    Retruns true if fetch operation is ongoing
*/
bool NmAttachmentManager::isFetching() const
{
    NM_FUNCTION;
    
    return mIsFetching;
}

/*!
    Retrunrs part id of attachment if fetch operation is ongoing. Zero id is returned otherwise
*/
NmId NmAttachmentManager::partIdUnderFetch() const
{
    NM_FUNCTION;
    
    return mAttaId;
}

/*!
    Cancels fetch operation. Does nothing if fetch not ongoing
*/
void NmAttachmentManager::cancelFetch()
{
    NM_FUNCTION;
    if (mFetchOperation && mFetchOperation->isRunning()) { 
        mFetchOperation->cancelOperation();
    }
    if (mMsgFetchOperation && mMsgFetchOperation->isRunning()) {
        mMsgFetchOperation->cancelOperation();
    }
    mIsFetching = false;
    mAttaId = 0;
    mProgressValue = 0;
}

/*!
    Used by message part fetch operation
*/
void NmAttachmentManager::changeProgress(int value)
{
    NM_FUNCTION;
    
    if (mFetchObserver && value > mProgressValue) {
        mProgressValue = value;
        mFetchObserver->progressChanged(value);
    }
}

/*!
    Used by message part fetch operation
*/
void NmAttachmentManager::completeAttachmentFetch(int result)
{
    NM_FUNCTION;
    
    if (mFetchObserver) {
        mFetchObserver->fetchCompleted(result);
    }
    mAttaId = 0;
    mProgressValue = 0;
    mIsFetching = false;
}

/*!
    Used by message fetch operation
*/
void NmAttachmentManager::completeMessageFetch(int result)
{
    NM_FUNCTION;
    
    if (result == NmNoError) {
        
        if (mFetchMsg) {
            
            NmId mailboxId = mFetchMsg->envelope().mailboxId();
            NmId folderId = mFetchMsg->envelope().folderId();
            NmId messageId = mFetchMsg->envelope().messageId();
            
            // Delete object
            delete mFetchMsg;
            mFetchMsg = NULL;
            
            // Reload message
            mFetchMsg = mUiEngine.message(mailboxId,folderId,messageId);
            
            if (mFetchMsg) {
                QList<NmId> partIds;
                NmMessagePart *part;
                foreach (part, mFetchMsg->childParts()) {
                    if (part->size() > part->fetchedSize()) {
                        partIds.append(part->partId());
                    }
                }
                if (partIds.count() > 0) {
                    mMsgFetchOperation = NULL;
                    fetchAttachments(
                        mFetchMsg->envelope().mailboxId(),
                        mFetchMsg->envelope().folderId(),
                        mFetchMsg->envelope().messageId(),
                        partIds);
                }
                else {
                    mFetchObserver->fetchCompleted(NmNoError);
                }
                // Delete object
                delete mFetchMsg;
                mFetchMsg = NULL;
            }
            else {
                mFetchObserver->fetchCompleted(NmNotFoundError);
            }
        }
        else {
            mFetchObserver->fetchCompleted(NmNotFoundError);
        }
    }
    else {
        mFetchObserver->fetchCompleted(result);
    }
}

/*!
    Sets fetch observer
*/
void NmAttachmentManager::setObserver(NmAttachmentFetchObserver *observer)
{
    NM_FUNCTION;
    
    mFetchObserver = observer;
    // send progress event wheng observer changes if fetch ongoing 
    // to get progress bar updating
    if (mIsFetching) {
        changeProgress(mProgressValue);
    }
}

/*!
    Clear observer
*/
void NmAttachmentManager::clearObserver()
{
    NM_FUNCTION;
    
    mFetchObserver = NULL;
}

/*!
    Returns progress value if fetch ongoing. Otherwise returns 0.
*/
int NmAttachmentManager::progressValue() const
{
    NM_FUNCTION;
    
    return mProgressValue;
}
