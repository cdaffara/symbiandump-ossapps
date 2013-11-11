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


#include "nmuiengineheaders.h"

/*!
    \class NmUiEngine
    \brief NmUiEngine provides a controller-type services for the email UI.

*/

NmUiEngine *NmUiEngine::mInstance;
int NmUiEngine::mReferenceCount;

const QString syncIndicatorName = "com.nokia.nmail.indicatorplugin.sync/1.0";

/*!
    Constructor
*/
NmUiEngine::NmUiEngine()
: mMailboxListModel(NULL),
  mInboxListModel(NULL),
  mMessageListModel(NULL),
  mMessageSearchListModel(NULL),
  mSendOperation(NULL),
  mRemoveDraftOperation(NULL),
  mSaveDraftOperation(NULL),
  mDraftMessage(NULL)
{
    mPluginFactory = NmDataPluginFactory::instance();
    mDataManager = new NmDataManager();
    // Connect to the plugins to receive change notifications
    QList<QObject*> *dataPlugins = mPluginFactory->pluginInstances();
    for (int i(0); i < dataPlugins->count(); i++) {
        QObject *plugin = (*dataPlugins)[i];
        if (plugin) {
            // connet mailbox events
            QObject::connect(plugin, SIGNAL(mailboxEvent(NmMailboxEvent, const QList<NmId> &)),
                 this, SLOT(handleMailboxEvent(NmMailboxEvent, const QList<NmId> &)),
                 Qt::UniqueConnection);
            // connect message events
            QObject::connect(plugin,
                SIGNAL(messageEvent(NmMessageEvent, const NmId &, const QList<NmId> &, const NmId&)),
                this, SLOT(handleMessageEvent(NmMessageEvent, const NmId &, const QList<NmId> &, const NmId &)),
                Qt::UniqueConnection);
            // connect connection events
            QObject::connect(plugin,
                SIGNAL(connectionEvent(NmConnectState, const NmId &, const int)),
                this,
                SLOT(handleConnectEvent(NmConnectState, const NmId &, const int)),
                Qt::UniqueConnection);
            // connect folder events            
            QObject::connect(plugin,
                SIGNAL(folderEvent(NmFolderEvent, const QList<NmId> &, const NmId &)),
                this, SLOT(handleFolderEvent(NmFolderEvent, const QList<NmId> &, const NmId &)),
                Qt::UniqueConnection);           

            // do the subscriptions also
            NmDataPluginInterface *pluginInterface = mPluginFactory->interfaceInstance(plugin);
            if (pluginInterface) {
                QList<NmId> mailboxIdList;
                pluginInterface->listMailboxIds(mailboxIdList);
                for (int j(0); j < mailboxIdList.count(); j++) {
                    pluginInterface->subscribeMailboxEvents(mailboxIdList[j]);
                }
                mailboxIdList.clear();
            }
        }
    }
}


/*!
    Class destructor.
*/
NmUiEngine::~NmUiEngine()
{
    enableSyncIndicator(false);

    if (mMessageSearchListModel) {
        delete mMessageSearchListModel;
        mMessageSearchListModel = NULL;
    }

    if (mInboxListModel) {
        delete mInboxListModel;
        mInboxListModel = NULL;
    }

    if (mMessageListModel) {
        delete mMessageListModel;
        mMessageListModel = NULL;
    }

    if (mMailboxListModel) {
        delete mMailboxListModel;
        mMailboxListModel = NULL;
    }

    // Do the unsubscriptions.
    QList<NmId> mailboxIdList;
    mDataManager->listMailboxIds(mailboxIdList);

    for (int i(0); i < mailboxIdList.count(); i++) {
        NmId id = mailboxIdList[i];
        NmDataPluginInterface *pluginInterface =
            mPluginFactory->interfaceInstance(id);

        if (pluginInterface) {
            pluginInterface->unsubscribeMailboxEvents(id);
        }
    }

    mailboxIdList.clear();

    NmDataPluginFactory::releaseInstance(mPluginFactory);

    delete mDataManager;

    // Cancel all operations.
    if (mSendOperation && mSendOperation->isRunning()) {
        mSendOperation->cancelOperation();
    }

    if (mRemoveDraftOperation && mRemoveDraftOperation->isRunning()) {
        mRemoveDraftOperation->cancelOperation();
    }

    if (mSaveDraftOperation && mSaveDraftOperation->isRunning()) {
        mSaveDraftOperation->cancelOperation();
    }

    // Call processEvents() to ensure that the cancelled operations get the time
    // they need to destroy themselves.
    qApp->processEvents();

    if(mDraftMessage) {
        delete mDraftMessage;
        mDraftMessage = NULL;
    }
}


/*!

*/
NmUiEngine *NmUiEngine::instance()
{
    if (!mInstance) {
    	mInstance = new NmUiEngine();
    }
    mReferenceCount++;
    return mInstance;
}

/*!

*/
void NmUiEngine::releaseInstance(NmUiEngine *&instance)
{
    //can't have passed out instances if we don't have any
    if (mInstance) {
        if(instance == mInstance) {
            instance = NULL;
            mReferenceCount--;
        }
        if (0 >= mReferenceCount) {
            delete mInstance;
            mInstance = NULL;
        }
    }
}

/*!
    Returns a list model populated with the mailbox and top level folder objects. The model is
    updated dynamically. The ownership of the model object is not moved to the caller.
*/
NmMailboxListModel &NmUiEngine::mailboxListModel()
{
    if (!mMailboxListModel) {
        refreshMailboxListModel(); // creates the model too
    }

    return *mMailboxListModel;
}

/*!
    Populate the list model with the mailbox and top level folder objects. The model is
    updated dynamically. Creates the model if needed.
*/
void NmUiEngine::refreshMailboxListModel()
{
    if (!mMailboxListModel) {
        mMailboxListModel = new NmMailboxListModel(*mDataManager);

        // Connect the model to the plugins to receive change notifications
        QList<QObject*> *dataPlugins = mPluginFactory->pluginInstances();
        for (int i = 0; i < dataPlugins->count(); i++) {
            QObject *plugin = (*dataPlugins)[i];
            if (plugin) {
                connect(plugin, SIGNAL(mailboxEvent(NmMailboxEvent, const QList<NmId> &)),
                    mMailboxListModel, SLOT(handleMailboxEvent(NmMailboxEvent, const QList<NmId> &)), 
                    Qt::UniqueConnection);
            }
        }
    } else {
        mMailboxListModel->clear();
    }

    QList<NmMailbox*> mailboxList;
    mDataManager->listMailboxes(mailboxList);
    mMailboxListModel->refresh(mailboxList);
    while (!mailboxList.isEmpty()) {
        delete mailboxList.takeFirst();
    }
}


/*!
    Returns a message list model for a folder identified by \a mailboxId and
    \a folderId. The model is updated dynamically. The ownership of the model
    object is not moved to the caller.
*/
NmMessageListModel &NmUiEngine::messageListModel(const NmId &mailboxId,
                                                 const NmId &folderId)
{
    NM_TIMESTAMP("NmUiEngine::messageListModel begins ***");
    
    QObject *plugin = mPluginFactory->pluginInstance(mailboxId);
    bool isInbox(false);
    if (standardFolderId(mailboxId,NmFolderInbox)==folderId){
        isInbox=true;
    }
    if (plugin) {
        // Connect plugin message events to handler slot messageEventForListModel
        QObject::connect(plugin, SIGNAL(messageEvent(NmMessageEvent, const NmId &,
                                const QList<NmId> &, const NmId&)),
                                this, SLOT(messageEventForListModel(NmMessageEvent,
                                const NmId &, const QList<NmId> &, const NmId&)), Qt::UniqueConnection );
        // Create inbox list model only once when entering to inbox for the first time
        // or re-create inbox list model when mailbox has changed
        if ((!mInboxListModel&&isInbox)||
            (mInboxListModel&&isInbox&&mailboxId!=mInboxListModel->currentMailboxId())){
            createMessageListModel(&mInboxListModel, mailboxId, folderId);
        }
        // Selected folder is not inbox folder for the mailbox
        else if (!isInbox){
            // Recreate model for other folders when needed, previous model
            // items are deleted from the memory with the old model
            createMessageListModel(&mMessageListModel, mailboxId, folderId);
        }
    }

    // Connect sync state changed
    if (plugin){
        QObject::connect(
            plugin, SIGNAL(syncStateEvent(NmSyncState, const NmOperationCompletionEvent &)),
            this, SLOT(handleSyncStateEvent(NmSyncState, const NmOperationCompletionEvent &)),
            Qt::UniqueConnection);
    }
    NmMessageListModel* ret(NULL);
    if (isInbox){
        ret = mInboxListModel;
        // Inbox list model is queried, other model is not
        // needed anymore and memory should be freeded
        if (mMessageListModel){
            delete mMessageListModel;
            mMessageListModel=NULL;
        }
    }
    else {
        ret = mMessageListModel;
    }
    
    NM_TIMESTAMP("NmUiEngine::messageListModel ends ***");
    
    return *ret;    
}


/*!
    Returns a message list model used in the search view.

    \param mailboxId The ID of the mailbox to search messages from.

    \return A message list model.
*/
NmMessageListModel &NmUiEngine::messageListModelForSearch(const NmId &mailboxId)
{
    Q_UNUSED(mailboxId);

    if (!mMessageSearchListModel) {
        mMessageSearchListModel = new NmMessageListModel(*mDataManager);
        mMessageSearchListModel->setIgnoreFolderIds(true);
    }
    else {
        mMessageSearchListModel->clear();
    }

    QObject *plugin = mPluginFactory->pluginInstance(mailboxId);

    if (plugin) {
        QObject::connect(plugin,
            SIGNAL(messageEvent(NmMessageEvent, const NmId &, const QList<NmId> &, const NmId &)),
            mMessageSearchListModel,
            SLOT(handleMessageEvent(NmMessageEvent, const NmId &, const QList<NmId> &, const NmId &)),
            Qt::UniqueConnection);
    }

    // Refresh to set the mailbox ID.
    QList<NmMessageEnvelope*> messageEnvelopeList;
    mMessageSearchListModel->refresh(mailboxId, 0, messageEnvelopeList);

    return *mMessageSearchListModel;
}


/*!
    Get the identifier of the standard folder of a type \a folderType
    from the mailbox \a mailboxId.
*/
NmId NmUiEngine::standardFolderId(
    const NmId &mailboxId,
    NmFolderType folderType)
{
    NmId value;
    if (folderType != NmFolderOther) {
        NmDataPluginInterface *plugin =
            mPluginFactory->interfaceInstance(mailboxId);
        if(plugin) {
            value = plugin->getStandardFolderId(mailboxId, folderType);
        }
    }
    return value;
}

/*!
    Returns a message identified by \a mailboxId,  \a folderId and \amessageId.
    The ownership of the  object is moved to the caller.
    Returns null pointer if the message is not found.
*/
NmMessage *NmUiEngine::message(const NmId &mailboxId,
                      const NmId &folderId,
                      const NmId &messageId)
{
    NmMessage *message = mDataManager->message(mailboxId, folderId, messageId);
    return message;
}

/*!

*/
QPointer<NmOperation> NmUiEngine::fetchMessage( const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId )
{
    QPointer<NmOperation> value(NULL);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        value = plugin->fetchMessage(mailboxId, folderId, messageId);
    }
    return value;
}

/*!

*/
QPointer<NmOperation> NmUiEngine::fetchMessagePart(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    const NmId &messagePartId)
{
    QPointer<NmOperation> value(NULL);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        value = plugin->fetchMessagePart(mailboxId, folderId, messageId, messagePartId);
    }
    return value;
}

/*!

*/
QPointer<NmOperation> NmUiEngine::fetchMessageParts(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    const QList<NmId> &messagePartIds)
{
    QPointer<NmOperation> value(NULL);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        value = plugin->fetchMessageParts(mailboxId, folderId, messageId, messagePartIds);
    }
    return value;
}

/*!

*/
XQSharableFile NmUiEngine::messagePartFile(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        const NmId &messagePartId)
{
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        return plugin->messagePartFile(mailboxId, folderId, messageId, messagePartId);
    }
    else {
        // empty file handle
        return XQSharableFile();
    }
}
/*!
    Get content to message part
*/
int NmUiEngine::contentToMessagePart(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    NmMessagePart &messagePart)
{
    return mDataManager->contentToMessagePart(mailboxId, folderId, messageId, messagePart);
}


/*!
    Deletes messages from the model and forwards the call to plugin.

    \param mailboxId The ID of the mailbox which contains the deleted message(s).
    \param folderId The ID of the folder which contains the deleted message(s).
    \param messageIdList A list containing the IDs of the message to be deleted.

    \return A possible error code (returned by the plugin).
*/
int NmUiEngine::deleteMessages(const NmId &mailboxId,
                               const NmId &folderId,
                               const QList<NmId> &messageIdList)
{
    NM_FUNCTION;
    
    int result(NmNotFoundError);

    bool isInbox(false);
    if (standardFolderId(mailboxId,NmFolderInbox)==folderId){
        isInbox=true;
    }
    if (isInbox&&mInboxListModel){
        mInboxListModel->handleMessageEvent(NmMessageDeleted, folderId,
                                               messageIdList, mailboxId);
    }
    else if (mMessageListModel) {
	    mMessageListModel->handleMessageEvent(NmMessageDeleted, folderId,
                                              messageIdList, mailboxId);
	}

	// If the search list model exists and contains message, remove the
	// message from it too.
	if (mMessageSearchListModel && mMessageSearchListModel->rowCount()) {
        mMessageSearchListModel->handleMessageEvent(NmMessageDeleted, folderId,
                                                    messageIdList, mailboxId);
	}

    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);

    if (plugin) {
          result = plugin->deleteMessages(mailboxId, folderId, messageIdList);
    }
    
    // Because the asynchronous delete operation may take some time to
    // finish, we will not wait for the NmMessageDeleted events from the
    // protocol plugin. Instead, handleMessageEvent() is invoked directly.
    // This causes the viewer to exit even though the asychronous delete
    // operation would take a bit longer to finish.
    handleMessageEvent(NmMessageDeleted,folderId,messageIdList,mailboxId);
    
    return result;
}


/*!
    Sets the envelope property for the given envelopes.
    The operation is automatically deleted after the completion or cancelling.

    \param mailboxId The ID of the mailbox containing the envelope(s).
    \param folderId The ID of the folder containing the envelope(s).
    \param property The property to set.
    \param envelopeList The list containing the envelopes.

    \return The constructed operation.
*/
QPointer<NmStoreEnvelopesOperation> NmUiEngine::setEnvelopes(
        const NmId &mailboxId,
        const NmId &folderId,
        NmEnvelopeProperties property,
        const QList<const NmMessageEnvelope*> &envelopeList)
{
    QPointer<NmStoreEnvelopesOperation> operation(NULL);
    NmMessageListModel *theMessageListModel = mMessageListModel;

    if (!theMessageListModel) {
        theMessageListModel = &messageListModel(mailboxId, folderId);
    }

    if (theMessageListModel && envelopeList.count()) {
        QList<NmId> messageIdList;

        for (int i(0); i < envelopeList.count(); i++){
            messageIdList.append(envelopeList[i]->messageId());
        }

        theMessageListModel->setEnvelopeProperties(property, messageIdList);

        if (mMessageSearchListModel && mMessageSearchListModel->rowCount()) {
            // Update the envelopes in the search list model as well.
            mMessageSearchListModel->setEnvelopeProperties(property,
                                                           messageIdList);
        }

        // Store the new envelopes to plugin.
        NmDataPluginInterface *plugin =
            mPluginFactory->interfaceInstance(mailboxId);

        if (plugin) {
            operation =
                plugin->storeEnvelopes(mailboxId, folderId, envelopeList);
        }

        messageIdList.clear();
    }

    return operation;
}


/*!
    Returns a mailbox meta data object from model with the ID \a mailboxId.
    Ownership of the return value is not moved to the caller.
    Returns NULL if the mailbox is not found.
*/
NmMailboxMetaData *NmUiEngine::mailboxById(const NmId &mailboxId)
{
    NmMailboxMetaData *meta(NULL);
    if (mMailboxListModel) {
	    for (int i(0); i < mMailboxListModel->rowCount(); i++) {
	        QModelIndex index = mMailboxListModel->index(i,0);
	        NmMailboxMetaData *mailbox =
	            mMailboxListModel->data(index, Qt::DisplayRole).value<NmMailboxMetaData*>();
	        if (mailbox && mailbox->id() == mailboxId) {
	            meta = mailbox;
	            break;
	        }
	    }
    }
    return meta;
}


/*!
    Creates a new message (into Drafts-folder).
    Operation is automatically deleted after completion or cancelling.
*/
QPointer<NmMessageCreationOperation> NmUiEngine::createNewMessage(const NmId &mailboxId)
{
    QPointer<NmMessageCreationOperation> value(NULL);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        value = plugin->createNewMessage(mailboxId);
    }
    return value;
}

/*!
    Creates a new forward message (into Drafts-folder).
    Operation is automatically deleted after completion or cancelling.
*/
QPointer<NmMessageCreationOperation> NmUiEngine::createForwardMessage(
        const NmId &mailboxId,
        const NmId &originalMessageId)
{
    QPointer<NmMessageCreationOperation> value(NULL);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        value = plugin->createForwardMessage(mailboxId, originalMessageId);
    }
    return value;
}

/*!
    Creates a new reply message (into Drafts-folder).
    Operation is automatically deleted after completion or cancelling.
*/
QPointer<NmMessageCreationOperation> NmUiEngine::createReplyMessage(
        const NmId &mailboxId,
        const NmId &originalMessageId,
        bool replyAll)
{
    QPointer<NmMessageCreationOperation> value(NULL);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        value = plugin->createReplyMessage(mailboxId, originalMessageId, replyAll);
    }
    return value;
}

/*!
    Saves a message (into message store).
*/
int NmUiEngine::saveMessage(const NmMessage &message)
{
    NM_FUNCTION;
    
    const NmId &mailboxId = message.envelope().mailboxId();
    int ret(NmNotFoundError);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        ret = plugin->saveMessage(message);
    }
    return ret;
}

/*!
    Refreshes mailbox.
*/
int NmUiEngine::refreshMailbox(const NmId &mailboxId )
{
    int ret(NmNotFoundError);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        ret = plugin->refreshMailbox(mailboxId);
        if (NmNoError == ret) {
            enableSyncIndicator(true);
        }
    }
    return ret;
}

/*!
    Online mailbox.
*/
int NmUiEngine::goOnline(const NmId &mailboxId )
{
    int ret(NmNotFoundError);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        ret = plugin->goOnline(mailboxId);
    }
    return ret;
}

/*!
    Offline mailbox.
*/
int NmUiEngine::goOffline(const NmId &mailboxId )
{
    int ret(NmNotFoundError);
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        ret = plugin->goOffline(mailboxId);
    }
    return ret;
}

/*!
    Removes message from given mailbox and folder
    - routes call to plugin
*/
int NmUiEngine::removeMessage(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId)
{
    NM_FUNCTION;
    
    int result(NmNotFoundError);
    NmDataPluginInterface *plugin =
            mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
          result = plugin->removeMessage(mailboxId, folderId, messageId);
    }
    return result;
}


/*!
  Handles draft message deletion after editor has closed, takes ownership of message.
 */
void NmUiEngine::removeDraftMessage(NmMessage *message)
{
    NM_FUNCTION;
    
    if (message) {
        NmDataPluginInterface *plugin =
            mPluginFactory->interfaceInstance(message->envelope().mailboxId());

        if (plugin) {
            // to be on the safer side:
            // we shouldn't even be here if mRemoveDraftOperation != NULL
            if (mRemoveDraftOperation && mRemoveDraftOperation->isRunning()) {
                mRemoveDraftOperation->cancelOperation();
            }
            // ownership of message changes
            mRemoveDraftOperation = plugin->removeDraftMessage(message);

            if (mRemoveDraftOperation) {
                connect(mRemoveDraftOperation,
                        SIGNAL(operationCompleted(int)),
                        this,
                        SLOT(handleCompletedRemoveDraftOperation()));
            }
        }
    }
}

/*!
    Handles draft message saving after editor has closed, takes ownership of message.
 */
void NmUiEngine::saveDraftMessage(NmMessage *message,
                                  const QList<NmOperation*> &preliminaryOperations)
{
    NM_FUNCTION;
    
    if (message) {
        NmDataPluginInterface *plugin =
            mPluginFactory->interfaceInstance(message->envelope().mailboxId());

        if (plugin) {
            // to be on the safer side:
            // we shouldn't even be here if mSaveDraftOperation != NULL
            if (mSaveDraftOperation && mSaveDraftOperation->isRunning()) {
                mSaveDraftOperation->cancelOperation();
            }

            mSaveDraftOperation = plugin->saveMessageWithSubparts(*message);

            if (mSaveDraftOperation) {
                // Ownership of message changes but saveMessageWithSubparts operation only makes a
                // copy so we handle the msg object deletion in engine. mDraftMessage is deleted
                // in handleCompletedSaveDraftOperation once operation finishes.
                if(mDraftMessage) {
                    delete mDraftMessage;
                    mDraftMessage = NULL;
                }
                mDraftMessage = message;
                message = NULL;

                for (int i(0); i < preliminaryOperations.count(); i++ ) {
                    QPointer<NmOperation> op = preliminaryOperations[i];
                    mSaveDraftOperation->addPreliminaryOperation(op);
                }

                connect(mSaveDraftOperation,
                        SIGNAL(operationCompleted(int)),
                        this,
                        SLOT(handleCompletedSaveDraftOperation()));

            }
        }
    }
}

/*!
    Sends the given message.
 */
void NmUiEngine::sendMessage(NmMessage *message, const QList<NmOperation *> &preliminaryOperations)
{
    NM_FUNCTION;
    
    //First trigger message storing
    if (message) {
        NmDataPluginInterface *plugin =
            mPluginFactory->interfaceInstance(message->envelope().mailboxId());

        if (plugin) {
            // to be on the safer side:
            // we shouldn't even be here if mSendOperation != NULL
            if (mSendOperation && mSendOperation->isRunning()) {
                mSendOperation->cancelOperation();
            }
            // ownership of message changes
            mSendOperation = plugin->sendMessage(message);
            // don't put this to mOperations as we need to handle this
            // operation separately
            if (mSendOperation) {
                for (int i(0); i < preliminaryOperations.count(); i++ ) {
                    QPointer<NmOperation> op = preliminaryOperations[i];
                    mSendOperation->addPreliminaryOperation(op);
                }

                connect(mSendOperation,
                        SIGNAL(operationCompleted(int)),
                        this,
                        SLOT(handleCompletedSendOperation()));
            }
        }
    }
}

/*!
    Is sending operation in progress.
 */
bool NmUiEngine::isSendingMessage() const
{
    int ret(false);
    if (mSendOperation && mSendOperation->isRunning()) {
        ret = true;
    }
    return ret;
}

/*!
   Returns a pointer to the message that is being sent. Returns NULL if not sending.
 */
const NmMessage *NmUiEngine::messageBeingSent() const
{
    const NmMessage *message = NULL;

    if (mSendOperation) {
        message = mSendOperation->getMessage();
    }

    return message;
}

/*!
    Add file attachment into given message. Return the operation object for
    observing/cancelling. Operation is automatically deleted after completion or cancelling.
 */
QPointer<NmAddAttachmentsOperation> NmUiEngine::addAttachments(
    const NmMessage &message,
    const QList<QString> &fileList)
{
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(message.envelope().mailboxId());

    QPointer<NmAddAttachmentsOperation> ret(NULL);
    if (plugin) {
        ret = plugin->addAttachments(message, fileList);
    }
    return ret;
}

/*!
    Remove attached file from given message. Return the operation object for
    observing/cancelling. Operation is automatically deleted after completion or cancelling.
 */
QPointer<NmOperation> NmUiEngine::removeAttachment(
    const NmMessage &message,
    const NmId &attachmentPartId)
{
    NmDataPluginInterface *plugin =
        mPluginFactory->interfaceInstance(message.envelope().mailboxId());

    QPointer<NmOperation> ret(NULL);
    if (plugin) {
        ret = plugin->removeAttachment(message, attachmentPartId);
    }
    return ret;
}

/*!
    Returns the current sync state of the mailbox
 */
NmSyncState NmUiEngine::syncState(const NmId& mailboxId)
{
    NmDataPluginInterface *plugin =
                mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        return plugin->syncState(mailboxId);
    }
    else {
        return SyncComplete;
    }
}

/*!
    Returns the current connection state of the mailbox
 */
NmConnectState NmUiEngine::connectionState(const NmId& mailboxId)
{
    NmDataPluginInterface *plugin =
                mPluginFactory->interfaceInstance(mailboxId);
    if (plugin) {
        return plugin->connectionState(mailboxId);
    }
    else {
        return Disconnected;
    }
}


/*!
    Starts the search.

    \param mailboxId The ID of the mailbox to search from.
    \param searchStrings The strings to search with.

    \return A possible error code.
*/
int NmUiEngine::search(const NmId &mailboxId,
                       const QStringList &searchStrings)
{
    NM_FUNCTION;
    
    // Get the plugin instance.
    QObject *pluginInstance = mPluginFactory->pluginInstance(mailboxId);

    if (pluginInstance) {
        // Make sure the required signals are connected.
        connect(pluginInstance, SIGNAL(matchFound(const NmId &, const NmId &)),
                this, SIGNAL(matchFound(const NmId &, const NmId &)),
                Qt::UniqueConnection);

        connect(pluginInstance, SIGNAL(matchFound(const NmId &, const NmId &)),
                this, SLOT(handleMatchFound(const NmId &, const NmId &)),
                Qt::UniqueConnection);

        connect(pluginInstance, SIGNAL(searchComplete()),
                this, SIGNAL(searchComplete()),
                Qt::UniqueConnection);
    }

    int retVal(NmNoError);

    // Get the plugin interface.
    NmDataPluginInterface *pluginInterface =
        mPluginFactory->interfaceInstance(mailboxId);

    if (pluginInterface) {
        // Start the search.
        retVal = pluginInterface->search(mailboxId, searchStrings);
    }

    return retVal;
}


/*!
    Cancels the search operation if one is ongoing.

    \param mailboxId The ID of the mailbox running the search.

    \return A possible error code.
*/
int NmUiEngine::cancelSearch(const NmId &mailboxId)
{
    int retVal(NmNoError);

    // Get the plugin interface.
    NmDataPluginInterface *pluginInterface =
        mPluginFactory->interfaceInstance(mailboxId);

    if (pluginInterface) {
        // Cancel the search.
        retVal = pluginInterface->cancelSearch(mailboxId);
    }
    return retVal;
}

/*!
    Get mailbox folder type by folder id
    \param mailboxId id of the mailbox
    \param folderId id of the folder
    \return Folder type
*/
NmFolderType NmUiEngine::folderTypeById(NmId mailboxId, NmId folderId)
{
    NmFolderType ret(NmFolderInbox);
    if (mDataManager){
        ret = mDataManager->folderTypeById(mailboxId,folderId);
    }
    return ret;
}

/*!
    Indicates application state information to protocol plugin
    \param mailboxId Id of active mailbox, 0 if application is closed.
    \param folderId Id of active folder, 0 if application is closed.
*/
void NmUiEngine::updateActiveFolder(const NmId &mailboxId, const NmId &folderId)
{
    NmApplicationStateInterface *interface =
        mPluginFactory->applicationStateInterfaceInstance(mailboxId);
    if (interface) {
        interface->updateActiveFolder(mailboxId, folderId);
    }
}

/*!
    Handle completed send operation.
*/
void NmUiEngine::handleCompletedSendOperation()
{
    emit sendOperationCompleted();
}

/*!
    Handle completed remove draft operation.
*/
void NmUiEngine::handleCompletedRemoveDraftOperation()
{
    // draft message deletion observing not yet implemented...
}

/*!
    Handle completed save draft operation.
*/
void NmUiEngine::handleCompletedSaveDraftOperation()
{
    // delete message object since it's not needed anymore
    if(mDraftMessage) {
        delete mDraftMessage;
        mDraftMessage = NULL;
    }
}

/*!
    Handles synch operation related events
 */
void NmUiEngine::handleSyncStateEvent(NmSyncState syncState, const NmOperationCompletionEvent &event)
{
    if ( syncState == SyncComplete ) {
        // signal for reporting about (sync) operation completion status
        emit operationCompleted(event);

        enableSyncIndicator(false);
    }

    // signal for handling sync state icons
    emit syncStateEvent(syncState, event.mMailboxId);
}

/*!
    Enable or disable sync indicator

    \param enabled true if indicator is enabled, false if disabled
*/
void NmUiEngine::enableSyncIndicator(bool enabled)
{
    HbIndicator indicator;
    if (enabled) {
        indicator.activate(syncIndicatorName, QVariant());
	}
    else {
        indicator.deactivate(syncIndicatorName, QVariant());
	}
}

/*!
    Gets the signature for the given mailbox.

    \param mailboxId The mailbox id whose signature is asked.
	\param signature The reference to the signature string pointer. NULL if no signature.
*/
int NmUiEngine::getSignature(const NmId &mailboxId, QString *&signature)
{
    int retVal(NmNoError);
    
    // Get the plugin interface.
    NmDataPluginInterface *pluginInterface =
        mPluginFactory->interfaceInstance(mailboxId);

    if (pluginInterface) {
        retVal = pluginInterface->getSignature(mailboxId, signature);
    }
    
    return retVal;
}

/*!
    Emits signals based on message events coming from plugins.
    Currently only NmMessageDeleted is handled.
*/
void NmUiEngine::handleMessageEvent(NmMessageEvent event,
                                    const NmId &folderId,
                                    const QList<NmId> &messageIds,
                                    const NmId& mailboxId)
{
    switch (event) {
        case NmMessageDeleted:
        {
            for (int i(0); i < messageIds.count(); i++) {
                emit messageDeleted(mailboxId, folderId, messageIds[i]);
            }
           break;
        }
        default:
        break;
    }
}

/*!
    Emits signals based on mailbox events coming from plugins.
    Currently only NmMailboxDeleted is handled.
*/
void NmUiEngine::handleMailboxEvent(NmMailboxEvent event,
                                    const QList<NmId> &mailboxIds)
{
    switch (event) {
        case NmMailboxDeleted:
        {
            for (int i(0); i < mailboxIds.count(); i++) {
                emit mailboxDeleted(mailboxIds[i]);
            }
           break;
        }
        case NmMailboxCreated:
            {
            if( mailboxIds.count() ){
                NmDataPluginInterface *pluginInterface =
                        mPluginFactory->interfaceInstance(mailboxIds.at(0));
            
                if (pluginInterface) {
                    pluginInterface->subscribeMailboxEvents(mailboxIds.at(0));
                    }
                }
            }
            break;
        default:
        break;
    }
}

/*!
    Emits signals based on folder deletion or creation from plugins.
    \param event folder event type
    \param folderIds Ids of folder having the action       
    \param mailboxId Id of active mailbox, 0 if application is closed.
*/
void NmUiEngine::handleFolderEvent(NmFolderEvent event, 
        const QList<NmId> &folderIds, const NmId &mailboxId)
{
    switch (event) {
        case NmFolderIsDeleted:
            handleFolderDeletedEvent(folderIds, mailboxId);
            break;
        case NmFolderIsCreated:
        default:
            handleFolderCreatedEvent(folderIds, mailboxId);            
            break;            
    };
}

/*!
    Handles folder deletion from plugins.
*/
void NmUiEngine::handleFolderDeletedEvent(const QList<NmId> &folderIds,
                                          const NmId &mailboxId)
{
    NmId folderId(0); 

    for (int i(0); i < folderIds.count(); i++) {
        folderId = folderIds[i];
        if (mInboxListModel){
            mInboxListModel->handleFolderDeletedEvent(folderId, mailboxId);
        }
        if (mMessageListModel){
            mMessageListModel->handleFolderDeletedEvent(folderId, mailboxId);
        }
    }
}

/*!
    Emits signals based on folder creation from plugins.
*/
void NmUiEngine::handleFolderCreatedEvent(const QList<NmId> &folderIds,
                                          const NmId &mailboxId)
{
    Q_UNUSED(folderIds);
    Q_UNUSED(mailboxId);
}


/*!
    Adds the found message into the search model.

    \param messageId The ID of the found message.
    \param folderId The ID of the folder where the message is located.
*/
void NmUiEngine::handleMatchFound(const NmId &messageId, const NmId &folderId)
{
    if (!mMessageSearchListModel) {
        // No search list model!
        return;
    }

    // Resolve the folder type.
    NmId mailboxId = mMessageSearchListModel->currentMailboxId();
    NmFolderType folderType = folderTypeById(mailboxId, folderId);

    // Do not display matches from outbox or draft folders.
    if (folderType != NmFolderOutbox && folderType != NmFolderDrafts) {
        // Add the found message into the search model.
        QList<NmId> messageIdList;
        messageIdList.append(messageId);

        mMessageSearchListModel->handleMessageEvent(NmMessageFound, folderId,
                                                    messageIdList, mailboxId);
    }
}

/*!
    Function sens events from plugin to both models. Inbox model for
    active mailbox is always alove whereas mMessageListModel can represent
    other folder in the device (sent, outbox, drafts, etc.)
*/
void NmUiEngine::messageEventForListModel(NmMessageEvent event,
                        const NmId &folderId,
                        const QList<NmId> &messageIds,
                        const NmId& mailboxId)
{
    // Forward event to both list models. Models will take care of checking
    // whether event really belongs to current mailbox & folder
    if (mInboxListModel){
        mInboxListModel->handleMessageEvent(event, folderId,
                                            messageIds, mailboxId);
    }
    if (mMessageListModel){
        mMessageListModel->handleMessageEvent(event, folderId,
                                              messageIds, mailboxId);
    }
}

/*!
    receives events when going online, and offline.
*/
void NmUiEngine::handleConnectEvent(NmConnectState connectState, const NmId &mailboxId, const int errorCode)
{
    // signal for connection state icon handling
    emit connectionEvent(connectState, mailboxId);

    // in case going offline w/ error, emit signal to UI
    if ( connectState == Disconnected && errorCode!= NmNoError ) {
        NmOperationCompletionEvent event={NoOp, errorCode, mailboxId, 0, 0};
        emit operationCompleted(event);
    }
}

/*!
     Create message list mode
 */
void NmUiEngine::createMessageListModel(
        NmMessageListModel **model,        
        const NmId &mailboxId,
        const NmId &folderId)
{
    // Delete previous model and set to NULL. Deleting model will also
    // delete all items in model.
    if (*model){
        delete *model;
        *model=NULL;
    }
    // Create new list model
    *model = new NmMessageListModel(*mDataManager);
    // Initial fill up of the list model, otherwise updated in the background
    QList<NmMessageEnvelope*> messageEnvelopeList;
    mDataManager->listMessages(mailboxId, folderId, messageEnvelopeList);
    (*model)->refresh(mailboxId, folderId, messageEnvelopeList);

    while (!messageEnvelopeList.isEmpty()) {
        delete messageEnvelopeList.takeFirst();
    }    
}
