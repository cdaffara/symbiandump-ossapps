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
 *     Email message related operations
 */
 
#include "nmapiheaders.h"


/*!
    \class NmApiMessageManager
    \brief A class for performing various operations on messages.
 */
 
 
namespace EmailClientApi
{

NmApiMessageManager::NmApiMessageManager(QObject *parent, quint64 mailboxId)
	:QObject(parent)
{
	d = new NmApiMessageManagerPrivate(this, mailboxId);
}

NmApiMessageManager::~NmApiMessageManager()
{
}
   
/*!
 \fn createDraftMessage 
 \param initData Initializing data. Can contain NULL value.
 \return Create draft message operation.
 
 Creates a new email message. Returns create draft message operation. 
 */
QPointer<NmApiOperation> NmApiMessageManager::createDraftMessage(const QVariant *initData)
{
    NM_FUNCTION;
    return d->createDraftMessage(initData);
}
    
    // creates fw message
    // signaled with forwardMessageCreated(NmApiMessage *message,int result) 
    //    * ownership transferred
QPointer<NmApiOperation> NmApiMessageManager::createForwardMessage(NmApiMessage *orig,const QVariant *initData)
 {
    NM_FUNCTION;
    Q_UNUSED(initData);
    Q_UNUSED(orig);
	return 0;
 }
    
    // creates reply message
    // signaled with replyMessageCreated(NmApiMessage *message,int result) 
    //    * ownership transferred
QPointer<NmApiOperation> NmApiMessageManager::createReplyMessage(const NmApiMessage *orig,
        const QVariant *initData,bool replyAll)
{
    NM_FUNCTION;
    Q_UNUSED(orig);
    Q_UNUSED(initData);
    Q_UNUSED(replyAll);
	return 0;
}
 
/*!
 \fn moveMessages 
 \param messageIds Id list of source messages.
 \param sourceFolderId Id of the source folder.
 \param targetFolderId Id of the target folder.
 \return true if operation was successfully started.
 
 Starts async move operation for given messages.  
 Completion signalled with messagesMoved(int result).
 */
QPointer<NmApiOperation> NmApiMessageManager::moveMessages(const QList<quint64> messageIds,
									quint64 sourceFolderId,
									quint64 targetFolderId)
{    
    NM_FUNCTION;
    Q_UNUSED(messageIds);
    Q_UNUSED(sourceFolderId);
    Q_UNUSED(targetFolderId);
    return 0;
	//return d->moveMessages(messageIds,sourceFolderId,targetFolderId);
}
    
/*!
 \fn saveMessage 
 \param message Message to be saved (note that message must exists to complete successfully).
 \return Save message operation.
 
 Saves email message. Message must be created before this operation with createDraftMessage/createForwardMessage/
 createReplyMessage operation. Operation will run unsuccessfully, if message haven't created before. 
 Returns save message operation. 
 */
QPointer<NmApiOperation> NmApiMessageManager::saveMessage(const NmApiMessage &message)
{
    return d->saveMessage(message);
}
    
// deletes message
// signaled with messagesDeleted(int result)
QPointer<NmApiOperation> NmApiMessageManager::deleteMessages(const QList<quint64> messageIds)
{
    Q_UNUSED(messageIds);
	return 0;
}
    
    // starts fetching rest of message body from server
    // signaled with messageFetched(quint64 messageId, int result)
QPointer<NmApiOperation> NmApiMessageManager::fetchMessage(quint64 messageId)
{
    Q_UNUSED(messageId);
	return 0;
}
    
/*!
 \fn sendMessage 
 \param message Message to be sent (note that message must exists to complete successfully).
 \return Send message operation.
 
 Sends email message. Message must be created before this operation with createDraftMessage/createForwardMessage/
 createReplyMessage operation. Operation will run unsuccessfully, if message haven't created before. 
 Returns send message operation. 
 */
QPointer<NmApiOperation> NmApiMessageManager::sendMessage(const NmApiMessage &message)
{
    NM_FUNCTION;
    return d->sendMessage(message);
}

// creates new attachment for a message. Currently attachment can be specified as file name (attachmentSpec is QString)
// signaled with attachmentCreated(quint64 attachemntId)
//  * 
QPointer<NmApiOperation> NmApiMessageManager::createAttachment(NmApiMessage &message,
										const QVariant &attachmenSpec)
{
    Q_UNUSED(message);
    Q_UNUSED(attachmenSpec);
    return 0;
}
                                                      
// removes attachment from a message
// signaled with attachmentRemoved(int result)
QPointer<NmApiOperation> NmApiMessageManager::removeAttachment(NmApiMessage &message,
quint64 attachmentId)
{    
    Q_UNUSED(message);
    Q_UNUSED(attachmentId);
    return 0;
}

QPointer<NmApiOperation> NmApiMessageManager::fetchAttachment(const NmApiMessage &relatedMessage,
        quint64 attachmentId)
{
    Q_UNUSED(relatedMessage);
    Q_UNUSED(attachmentId);
    return 0;
}
 
} // namespace EmailClientApi


