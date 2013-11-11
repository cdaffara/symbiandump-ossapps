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

static const char *NMUI_NET_REPLY_CONTENT_ID = "cid";

/*!
    \class NmViewerViewNetReply
    \brief Mail viewer network reply class
*/

/*!
    Constructor
*/
NmViewerViewNetReply::NmViewerViewNetReply(QVariant data, NmUiEngine &uiEngine)
    : QNetworkReply(),
      mDataArray(data.toByteArray()),
      mUiEngine(uiEngine),
      mOperation(NULL),
      mReadIndex(0)
{
    NM_FUNCTION;
    
    open(ReadWrite);
    setReadBufferSize(mDataArray.length());
    // QNetworkAccessManager starts listening the signals only
    // after the construction, so we cannot signal reply to be
    // ready instantly. We need to emit the signals after the
    // construction.
    QMetaObject::invokeMethod(this, "signalReady", Qt::QueuedConnection);
}

/*!

*/
NmViewerViewNetReply::NmViewerViewNetReply(QVariant data, NmUiEngine &uiEngine, 
        const NmId &mailboxId, const NmId &folderId, const NmId &messageId, 
        const NmId &messagePartId)
    : QNetworkReply(),
      mDataArray(data.toByteArray()),
      mUiEngine(uiEngine),
      mMailboxId(mailboxId),
      mFolderId(folderId),
      mMessageId(messageId),
      mMessagePartId(messagePartId),
      mOperation(NULL),
      mReadIndex(0)
{
    mOperation = uiEngine.fetchMessagePart(mMailboxId, mFolderId, mMessageId, mMessagePartId);
    if (mOperation) {
        connect(mOperation, SIGNAL(operationCompleted(int)),
            this, SLOT(fetchCompleted(int)));
        connect(mOperation, SIGNAL(operationCancelled()),
            this, SLOT(fetchCancelled()));
    }
    else {
        open(ReadWrite);
        setReadBufferSize(mDataArray.length());
        QMetaObject::invokeMethod(this, "signalReady", Qt::QueuedConnection);
    }
}

/*!
    Destructor
*/
NmViewerViewNetReply::~NmViewerViewNetReply()
{
    NM_FUNCTION;
}

/*!
    signalReady
*/
void NmViewerViewNetReply::signalReady()
{
    NM_FUNCTION;
    
    // Insert embedded images into cache manually
    if(manager()) {
        if(manager()->cache() && request().url().scheme() == NMUI_NET_REPLY_CONTENT_ID) {
            // Store url to use for reply in access manager finished emitting. 
            setUrl(request().url());
            
            // Metadata required for inserted data
            QNetworkCacheMetaData metaData;
            metaData.setUrl(request().url());
            metaData.setSaveToDisk(true);
            
            // Insert data into cache
            QIODevice *device = manager()->cache()->prepare(metaData);
            if(device) {
                device->write(mDataArray);
                manager()->cache()->insert(device);
            }
        }
    }
    // Emit these signals to properly emulate downloading from URL
    emit readyRead();
    emit finished();
}

/*!
    Slot. Called when fetch operation completes
*/
void NmViewerViewNetReply::fetchCompleted(int result)
{
    NM_FUNCTION;
    
    Q_UNUSED(result);
    NmMessage *message = mUiEngine.message(
                        mMailboxId, mFolderId, mMessageId);
    if (message) {
        QList<NmMessagePart*> partList;
        message->attachmentList(partList);
        NmMessagePart *part(NULL);
        for (int i = 0; !part && i < partList.count(); i++) {
            if (partList[i]->partId() == mMessagePartId) {
                part = partList[i];
                break;
            }
        }
        if (part) {
            int error = mUiEngine.contentToMessagePart(mMailboxId, mFolderId, mMessageId, *part);
            if (error == NmNoError) {
                mDataArray = part->binaryContent();
            }
        }
    }
    delete message;
    message = NULL;
    open(ReadWrite);
    setReadBufferSize(mDataArray.length());
    QMetaObject::invokeMethod(this, "signalReady", Qt::QueuedConnection);
}

/*!
    Slot. Called if fetch operation is cancelled
*/
void NmViewerViewNetReply::fetchCancelled()
{
    NM_FUNCTION;
    
    // just call fetch completed
    fetchCompleted(NmCancelError);
}

/*!
    setOriginalRequest. This function is created to provide access to call
    base class' protected function setRequest
*/
void NmViewerViewNetReply::setOriginalRequest(const QNetworkRequest &request)
{
    NM_FUNCTION;
    
    setRequest(request);
}

/*!
    readData
*/
qint64 NmViewerViewNetReply::readData(char *data, qint64 maxlen)
{
    NM_FUNCTION;
    
    qint64 i = 0;
    const qint64 dataLength(mDataArray.length());

    if (0 >= dataLength || !data) {
        return -1;
    }

    for (i = 0; i < maxlen && mReadIndex < dataLength; i++, mReadIndex++) {
        data[i] = mDataArray.at(mReadIndex);
    }
    return i;
}

/*!
    readBufferSize. Returns the size of the read buffer, in bytes.
*/
qint64 NmViewerViewNetReply::readBufferSize() const
{
    NM_FUNCTION;
    
    return mDataArray.length();
}

/*!
    bytesAvailable. Returns the number of bytes that are available for reading.
    This function is commonly used with sequential devices to determine the number of bytes to
    allocate in a buffer before reading. Base implementation is called in order to
    include the size of QIODevices' buffer.
*/
qint64 NmViewerViewNetReply::bytesAvailable() const
{
    NM_FUNCTION;
    
    return mDataArray.length() + QIODevice::bytesAvailable();
}

/*!
    isSequential. Returns true if this device is sequential.
*/
bool NmViewerViewNetReply::isSequential () const
{
    NM_FUNCTION;
    
    return false;
}

/*!
    abort. Aborts the operation immediately and close down any
    network connections still open
*/
void NmViewerViewNetReply::abort()
{
    NM_FUNCTION;
}

