/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NMFRAMEWORKADAPTER_H
#define NMFRAMEWORKADAPTER_H

#include <nmcommon.h>
#include <nmdataplugininterface.h>
#include <nmapplicationstateinterface.h>
#include <CFSMailCommon.h>
#include <MFSMailEventObserver.h>

#include <xqsharablefile.h>

class NmMailbox;
class NmMessage;
class NmFolder;
class NmMessageEnvelope;
class NmMessagePart;
class NmOperation;
class NmMessageCreationOperation;
class CFSMailClient;
class CFSMailBox;
class CFSMailFolder;
class CFSMailMessage;
class CFSMailMessagePart;
class NmStoreEnvelopesOperation;
class NmAddAttachmentsOperation;
class NmMailboxSearchObserver;
class NmMessageSendingOperation;
class CEmailExtension;


class NmFrameworkAdapter :
    public QObject,
    public NmDataPluginInterface,
    public NmApplicationStateInterface,
    public MFSMailEventObserver
{
    Q_OBJECT
    Q_INTERFACES(NmDataPluginInterface NmApplicationStateInterface)

public:

    NmFrameworkAdapter();

    ~NmFrameworkAdapter();

    int listMailboxIds(QList<NmId>& mailboxIdList);

    int listMailboxes(QList<NmMailbox*>& mailboxList);

    int getMailboxById(const NmId& id, NmMailbox*& mailbox);

    QPointer<NmOperation> deleteMailboxById(const NmId& id);

    int getMessageById(
            const NmId& mailboxId,
            const NmId& folderId,
            const NmId& messageId,
            NmMessage*& message);

    int listFolders(
            const NmId& mailboxId,
            QList<NmFolder*> &folderList);
   
    int listMessages(
            const NmId& mailboxId,
            const NmId& folderId,
            QList<NmMessageEnvelope*> &messageMetaDataList);

    int listMessages(
            const NmId& mailboxId,
            const NmId& folderId,
            QList<NmMessageEnvelope*> &messageMetaDataList,
            const int maxAmountOfEnvelopes);

    int listMessages(
            const NmId& mailboxId,
            const NmId& folderId,
            QList<NmMessage*> &messageList,
            const int maxAmountOfMessages = NmMaxItemsInMessageList);

    QPointer<NmOperation> fetchMessage( 
            const NmId &mailboxId, 
            const NmId &folderId,
            const NmId &messageId);
    
    QPointer<NmOperation> fetchMessagePart( 
            const NmId &mailboxId,
            const NmId &folderId,
            const NmId &messageId,
            const NmId &messagePartId);
			
    QPointer<NmOperation> fetchMessageParts( 
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        const QList<NmId> &messagePartIds);
    
    XQSharableFile messagePartFile(
            const NmId &mailboxId,
            const NmId &folderId,
            const NmId &messageId,
            const NmId &messagePartId);
           
    NmId getStandardFolderId(
            const NmId& mailbox,
            NmFolderType folderType );

    int refreshMailbox(NmId mailboxId);

    int goOnline(const NmId& mailboxId);
    
    int goOffline(const NmId& mailboxId);

    int contentToMessagePart(
            const NmId &mailboxId,
            const NmId &folderId,
            const NmId &messageId,
            NmMessagePart &messagePart);
 
    int deleteMessages(
            const NmId &mailboxId,
            const NmId &folderId,
            const QList<NmId> &messageIdList);

    QPointer<NmStoreEnvelopesOperation> storeEnvelopes(
            const NmId &mailboxId,
            const NmId &folderId,
            const QList<const NmMessageEnvelope*> &envelopeList);

    QPointer<NmMessageCreationOperation> createNewMessage(
            const NmId &mailboxId);

    QPointer<NmMessageCreationOperation> createForwardMessage(
            const NmId &mailboxId,
            const NmId &originalMessageId);
     
    QPointer<NmMessageCreationOperation> createReplyMessage(
            const NmId &mailboxId,
            const NmId &originalMessageId,
            const bool replyAll);
    
    int saveMessage(const NmMessage &message);

    QPointer<NmOperation> saveMessageWithSubparts(const NmMessage &message);

    void EventL(
            TFSMailEvent event,
            TFSMailMsgId mailbox,
            TAny* param1,
            TAny* param2,
            TAny* param3);

    int removeMessage(const NmId& mailboxId, const NmId& folderId, const NmId& messageId);
    
    void subscribeMailboxEvents(const NmId& mailboxId);
    
    void unsubscribeMailboxEvents(const NmId& mailboxId);

    QPointer<NmMessageSendingOperation> sendMessage(NmMessage *message);
    
    QPointer<NmAddAttachmentsOperation> addAttachments(
            const NmMessage &message, 
            const QList<QString> &fileList);

    QPointer<NmOperation> removeAttachment(
            const NmMessage &message, 
            const NmId &attachmentPartId);

    NmSyncState syncState(const NmId& mailboxId) const;
    
    NmConnectState connectionState(const NmId& mailboxId) const;
    
    int getFolderById(
            const NmId &mailboxId, 
            const NmId &folderId, 
            NmFolder *&folder );

    int search(const NmId &mailboxId,
               const QStringList &searchStrings);

    int cancelSearch(const NmId &mailboxId);

    void updateActiveFolder(const NmId &mailboxId, const NmId &folderId);
    
    QPointer<NmOperation> removeDraftMessage(NmMessage *message);
	
    int copyMessages(
        const NmId &mailboxId,
        const QList<quint64> &messageIds,
        const NmId &sourceFolderId,
        const NmId &destinationFolderId);
    
    int getSignature(const NmId &mailboxId, QString *&signature);

signals:

    void mailboxEvent(NmMailboxEvent event, const QList<NmId> &mailboxIds);

    void messageEvent(
            NmMessageEvent event,
            const NmId &folderId,
            const QList<NmId> &messageIds,
            const NmId& mailboxId);

      void folderEvent(
            NmFolderEvent event,
            const QList<NmId> &folderIds,
            const NmId& mailboxId);

    void syncStateEvent(
            NmSyncState state,
            const NmOperationCompletionEvent &event );

    void connectionEvent(NmConnectState state, const NmId mailboxId, int errorCode);

    void matchFound(const NmId &messageId, const NmId &folderId);

    void searchComplete();


private:
    
    void getMessageByIdL(
            const NmId& mailboxId,
            const NmId& folderId,
            const NmId& messageId,
            NmMessage*& message);

    void contentToMessagePartL(
            const NmId &mailboxId,
            const NmId &folderId,
            const NmId &messageId,
            NmMessagePart &messagePart);

    void getMessagesFromFolderL(CFSMailFolder *folder,
                                QList<NmMessageEnvelope*> &messageEnvelopeList,
                                const int maxEnvelopeCount);

    void listMessagesL(  
            const NmId &mailboxId,
            const NmId &folderId,
            QList<NmMessageEnvelope*> &messageMetaDataList, 
            const int maxAmountOfEnvelopes);
    
    void listMessagesL(  
            const NmId &mailboxId,
            const NmId &folderId,
            QList<NmMessage*> &messageList, 
            const int maxAmountOfMessages);

    void searchL(const NmId &mailboxId,
                 const QList<NmId> &folderIds,
                 const QStringList &searchStrings,
                 NmMailboxSearchObserver &searchObserver);

    void removeMessageL(const NmId& mailboxId,
                        const NmId& folderId,
                        const NmId& messageId);

    CFSMailMessage* mailMessageFromEnvelopeL( 
        const NmMessageEnvelope& envelope);

    void childrenToNmMessagePartL(
            CFSMailMessagePart *cfsParent, 
            NmMessagePart *nmParent);

    void resetCache(const NmId &mailboxId);

    int RefreshMailboxL(NmId mailboxId);
    
    int GoOnlineL(const NmId &mailboxId);

    int GoOfflineL(const NmId &mailboxId);

    void handleMailboxEvent(
            TFSMailMsgId mailbox,
            NmMailboxEvent event);
    
    void handleMessageEvent(TAny* param1, TAny* param2, NmMessageEvent event, TFSMailMsgId mailbox);
    
    void handleMailMoved(TAny* param1,TAny* param2,TAny* param3, TFSMailMsgId mailbox);
    
    void handleMailCopied(TAny* param1,TAny* param2, TFSMailMsgId mailbox);
    
    void handleSyncstateEvent(TAny* param1, TFSMailMsgId mailbox);
    
    void handleFoldersEvent(TAny* param1,TAny* param2, NmFolderEvent event, TFSMailMsgId mailbox);
    
    void getFolderByIdL(
            const NmId& mailboxId, 
            const NmId& folderId, 
            NmFolder*& unreadCount );
    
    void doUpdateActiveFolderL(const NmId &mailboxId, const NmId &folderId);
    
    CEmailExtension* getEMailStateExtensionL();  
	
    void copyMessagesL(
        const NmId &mailboxId, 
        const QList<quint64> &messageIds,
        const NmId &sourceFolderId,
        const NmId &destinationFolderId);
	
	void deleteMailboxByIdL(const NmId &mailboxId);    

    void getSignatureL(const NmId &mailboxId, QString *&signature);

private: // Data

    CFSMailClient* mFSfw; // Singleton, not owned
    NmMailboxSearchObserver *mSearchObserver; // Owned
    CFSMailBox* mCurrentMailBox; // Owned
    CEmailExtension* mStateExtension; // not owned

    NmId mCachedMailboxId;
    NmId mCachedInboxId;
};


#endif // NMFRAMEWORKADAPTER_H
