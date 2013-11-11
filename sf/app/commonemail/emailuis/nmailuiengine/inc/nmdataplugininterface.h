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

#ifndef NMDATAPLUGININTERFACE_H_
#define NMDATAPLUGININTERFACE_H_

#include <QList>
#include <QPointer>
#include <xqsharablefile.h>

#include "nmcommon.h"

class NmMailbox;
class NmMessage;
class NmMessagePart;
class NmFolder;
class NmMessageEnvelope;
class NmOperation;
class NmMessageCreationOperation;
class NmStoreEnvelopesOperation;
class NmAddAttachmentsOperation;
class NmMessageSendingOperation;

/*!
    \class NmDataPluginInterface
    \brief The class NmDataPluginInterface provides an interface to access email data from
    protocol plugins.
    Implementation classes of this interface are expected to emit following signals:
    void mailboxEvent(NmMailboxEvent event,
                      const QList<NmId> &mailboxIds);
    void messageEvent(NmMessageEvent event,
                      const NmId &folderId,
                      const QList<NmId> &messageIds);
                      const NmId& mailboxId);
 */
class NmDataPluginInterface
{
public:

    virtual ~NmDataPluginInterface() {};
    
    virtual int listMailboxIds(QList<NmId> &mailboxIdList) = 0;
    
    virtual int listMailboxes(QList<NmMailbox*> &mailboxList) = 0;
    
    virtual int getMailboxById(const NmId &id, NmMailbox *&mailbox) = 0;
    
    virtual QPointer<NmOperation> deleteMailboxById(const NmId &id) = 0;
    
    virtual int getMessageById(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        NmMessage *&message) = 0;
		
    virtual int getFolderById(
        const NmId& mailboxId, 
        const NmId& folderId, 
        NmFolder*& folder ) = 0;
    
    virtual int listFolders(
        const NmId &mailboxId,
        QList<NmFolder*> &folderList) = 0;
    
    virtual int listMessages(
        const NmId &mailboxId,
        const NmId &folderId,
        QList<NmMessageEnvelope*> &messageEnvelopeList) = 0;  
    
    virtual int listMessages(
        const NmId &mailboxId,
        const NmId &folderId,
        QList<NmMessageEnvelope*> &messageEnvelopeList, 
		const int maxAmountOfEnvelopes ) = 0;
    
    virtual int listMessages(
        const NmId &mailboxId,
        const NmId &folderId,
        QList<NmMessage*> &messageList, 
        const int maxAmountOfMessages = NmMaxItemsInMessageList) = 0;
    
    virtual QPointer<NmOperation> fetchMessage( 
        const NmId &mailboxId, 
        const NmId &folderId,
        const NmId &messageId ) = 0;
    
    virtual QPointer<NmOperation> fetchMessagePart( 
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        const NmId &messagePartId) = 0;
 
     virtual  QPointer<NmOperation> fetchMessageParts( 
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        const QList<NmId> &messagePartIds) = 0;
    
    virtual XQSharableFile messagePartFile(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        const NmId &messagePartId) = 0;
    
    virtual NmId getStandardFolderId(
        const NmId &mailbox,
        NmFolderType folderType ) = 0;
    
    virtual int refreshMailbox(NmId mailboxId) = 0;
    
    virtual int goOnline(const NmId &mailboxId) = 0;
    
    virtual int goOffline(const NmId &mailboxId) = 0;
    
    virtual int contentToMessagePart(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        NmMessagePart &messagePart) = 0;
    
    virtual int deleteMessages(
        const NmId &mailboxId,
        const NmId &folderId,
        const QList<NmId> &messageIdList) = 0;

    virtual QPointer<NmStoreEnvelopesOperation> storeEnvelopes(
        const NmId &mailboxId,
        const NmId &folderId,
        const QList<const NmMessageEnvelope*> &envelopeList) = 0;

    virtual QPointer<NmMessageCreationOperation> createNewMessage(const NmId &mailboxId) = 0;

    virtual QPointer<NmMessageCreationOperation> createForwardMessage(
        const NmId &mailboxId,
        const NmId &originalMessageId) = 0;

    virtual QPointer<NmMessageCreationOperation> createReplyMessage(
        const NmId &mailboxId,
        const NmId &originalMessageId,
        const bool replyAll) = 0;

    virtual int saveMessage(const NmMessage &message) = 0;

    virtual QPointer<NmOperation> saveMessageWithSubparts(const NmMessage &message) = 0;

    virtual int removeMessage(
        const NmId& mailboxId,
        const NmId& folderId,
        const NmId& messageId) = 0;

    virtual void subscribeMailboxEvents(const NmId& mailboxId) = 0;
    
    virtual void unsubscribeMailboxEvents(const NmId& mailboxId) = 0;

    virtual QPointer<NmMessageSendingOperation> sendMessage(NmMessage *message) = 0;
    
    virtual QPointer<NmAddAttachmentsOperation> addAttachments(
        const NmMessage &message,
        const QList<QString> &fileList) = 0;
    
    virtual QPointer<NmOperation> removeAttachment(
        const NmMessage &message,
        const NmId &attachmentPartId) = 0;

    virtual NmSyncState syncState(const NmId& mailboxId) const = 0;
    
    virtual NmConnectState connectionState(const NmId& mailboxId) const = 0;

    virtual int search(const NmId &mailboxId,
        const QStringList &searchStrings) = 0;

    virtual int cancelSearch(const NmId &mailboxId) = 0;

    virtual QPointer<NmOperation> removeDraftMessage(NmMessage *message) = 0;

    virtual int copyMessages(
        const NmId &mailboxId,
        const QList<quint64> &messageIds, 
        const NmId &sourceFolderId,
        const NmId &destinationFolderId) = 0;

    virtual int getSignature(const NmId &mailboxId, QString *&signature) = 0;
};

Q_DECLARE_INTERFACE(NmDataPluginInterface, "sf.app.commonmail.emailuis.nmailuiengine.NmDataPluginInterface/1.0")


#endif /* NMDATAPLUGININTERFACE_H_ */


