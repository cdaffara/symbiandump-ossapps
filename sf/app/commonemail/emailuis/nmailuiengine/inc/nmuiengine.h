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

#ifndef NMUIENGINE_H
#define NMUIENGINE_H

#include <QObject>
#include <QList>
#include <xqsharablefile.h>

#include "nmcommon.h"
#include "nmuienginedef.h"

class QAbstractItemModel;
class NmMailboxListModel;
class NmMessageListModel;
class NmDataManager;
class NmDataPluginFactory;
class NmMailboxMetaData;
class NmMessage;
class NmMessagePart;
class NmMessageEnvelope;
class NmOperation;
class NmMessageCreationOperation;
class NmStoreEnvelopesOperation;
class NmAddAttachmentsOperation;
class NmMessageSendingOperation;


class NMUIENGINE_EXPORT NmUiEngine: public QObject
{
    Q_OBJECT

public:

    static NmUiEngine *instance();

    static void releaseInstance(NmUiEngine *&instance);

    NmMailboxListModel &mailboxListModel();

    void refreshMailboxListModel();

    NmMessageListModel &messageListModel(const NmId &mailboxId,
                                         const NmId &folderId);

    NmMessageListModel &messageListModelForSearch(const NmId &mailboxId);

    NmId standardFolderId(const NmId &mailboxId, NmFolderType folderType);

    NmMessage *message(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId);

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

    NmMailboxMetaData *mailboxById(const NmId &mailboxId);

    int contentToMessagePart(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        NmMessagePart &messagePart);

    NmMessage *createMessage(
        const NmId &mailboxId,
        const NmId &folderId);

    int deleteMessages(
        const NmId &mailboxId,
        const NmId &folderId,
        const QList<NmId> &messageIdList);

    QPointer<NmStoreEnvelopesOperation> setEnvelopes(
        const NmId &mailboxId,
        const NmId &folderId,
        NmEnvelopeProperties property,
        const QList<const NmMessageEnvelope*> &envelopeList);

    QPointer<NmMessageCreationOperation> createNewMessage(const NmId &mailboxId);

    QPointer<NmMessageCreationOperation> createForwardMessage(
        const NmId &mailboxId,
        const NmId &originalMessageId);

    QPointer<NmMessageCreationOperation> createReplyMessage(
        const NmId &mailboxId,
        const NmId &originalMessageId,
        bool replyAll);

    int saveMessage(const NmMessage &message);

    int refreshMailbox(const NmId &mailboxId);

    int goOnline(const NmId &mailboxId);

    int goOffline(const NmId &mailboxId);

    int removeMessage(const NmId &mailboxId,
                      const NmId &folderId,
                      const NmId &messageId);

    void removeDraftMessage(NmMessage *message);

    void saveDraftMessage(NmMessage *message,
                          const QList<NmOperation*> &preliminaryOperations);

    void sendMessage(NmMessage *message,
                     const QList<NmOperation*> &preliminaryOperations);

    bool isSendingMessage() const;

    const NmMessage *messageBeingSent() const;

    QPointer<NmAddAttachmentsOperation> addAttachments(
        const NmMessage &message,
        const QList<QString> &fileList);

    QPointer<NmOperation> removeAttachment(
        const NmMessage &message,
        const NmId &attachmentPartId);

    NmSyncState syncState(const NmId& mailboxId);

    NmConnectState connectionState(const NmId& mailboxId);

    int search(const NmId &mailboxId,
               const QStringList &searchStrings);

    int cancelSearch(const NmId &mailboxId);

    NmFolderType folderTypeById(NmId mailboxId, NmId folderId);

    void updateActiveFolder(const NmId &mailboxId, const NmId &folderId);

    void enableSyncIndicator(bool enabled);

    int getSignature(const NmId &mailboxId, QString *&signature);

public slots:

    void handleCompletedSendOperation();
    void handleCompletedRemoveDraftOperation();
    void handleCompletedSaveDraftOperation();

    void handleSyncStateEvent(NmSyncState syncState,
                              const NmOperationCompletionEvent &event);

    void handleConnectEvent(NmConnectState connectState,
                            const NmId &mailboxId,
                            const int errorCode);


private slots:

    void handleMessageEvent(NmMessageEvent event,
                            const NmId &folderId,
                            const QList<NmId> &messageIds,
                            const NmId& mailboxId);

    void handleMailboxEvent(NmMailboxEvent event,
                            const QList<NmId> &mailboxIds);

    void handleFolderEvent(NmFolderEvent event, const QList<NmId> &folderIds,
                            const NmId& mailboxId);

    void handleMatchFound(const NmId &messageId, const NmId &folderId);

    void messageEventForListModel(NmMessageEvent event,
                            const NmId &folderId,
                            const QList<NmId> &messageIds,
                            const NmId& mailboxId);


signals:

    void syncStateEvent(NmSyncState, const NmId &);
    void connectionEvent(NmConnectState, const NmId &);
    void operationCompleted(const NmOperationCompletionEvent &event);
    void sendOperationCompleted();
    void messageDeleted(const NmId &mailboxId, const NmId &folderId, const NmId &messageId);
    void mailboxDeleted(const NmId &mailboxId);
    void matchFound(const NmId &, const NmId &);
    void searchComplete();


private:

    NmUiEngine();

    virtual ~NmUiEngine();

    void handleFolderDeletedEvent(const QList<NmId> &folderIds,
                            const NmId& mailboxId);

    void handleFolderCreatedEvent(const QList<NmId> &folderIds,
                            const NmId& mailboxId);

    void createMessageListModel(
            NmMessageListModel **model,
            const NmId &mailboxId,
            const NmId &folderId);


private: // Data

    static NmUiEngine *mInstance;
    static int	mReferenceCount;

    NmDataPluginFactory *mPluginFactory;
    NmDataManager *mDataManager;                // Owned
    NmMailboxListModel *mMailboxListModel;      // Owned
    NmMessageListModel *mInboxListModel;      // Owned
    NmMessageListModel *mMessageListModel;      // Owned
    NmMessageListModel *mMessageSearchListModel; // Owned
    QPointer<NmMessageSendingOperation> mSendOperation;  // Not owned
    QPointer<NmOperation> mRemoveDraftOperation; // not owned
    QPointer<NmOperation> mSaveDraftOperation;   // not owned
    NmMessage *mDraftMessage; // owned
};


#endif /* NMUIENGINE_H */

