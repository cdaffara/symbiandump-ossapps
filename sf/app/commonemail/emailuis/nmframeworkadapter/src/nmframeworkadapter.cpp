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

#include "nmframeworkadapterheaders.h"

/*!
    \class NmFrameworkAdapter

    \brief The NmFrameworkAdapter works as an adapter between emailframework which is (legacy)
    Symbian code and emailuis which is done using QT.

    The NmFrameworkAdapter works as an adapter between emailframework which is (legacy)
    Symbian code and emailuis which is done using QT. Most functions in the adapter merely do d
    ata type conversions and forward function calls to emailframework implementation.
    Also events coming from emailframework are converted and emitted as signals.
 */

static const int NmListMessagesBlock = 100;



/*!
    Constructor
*/
NmFrameworkAdapter::NmFrameworkAdapter()
: mFSfw(NULL),
  mSearchObserver(NULL),
  mCurrentMailBox(NULL),
  mStateExtension(NULL)
{
    NM_FUNCTION;

    // get s60 email framework
    TRAP_IGNORE(mFSfw = CFSMailClient::NewL());

    if (mFSfw) {
        TRAP_IGNORE(mFSfw->AddObserverL(*this));
    }
}

/*!
    Destructor
*/
NmFrameworkAdapter::~NmFrameworkAdapter()
{
    NM_FUNCTION;

    delete mCurrentMailBox;
    mCurrentMailBox = NULL;

    mStateExtension = NULL;

    if (mSearchObserver) {
        delete mSearchObserver;
        mSearchObserver = NULL;
    }

    if (mFSfw) {
        mFSfw->RemoveObserver(*this);
        mFSfw->Close();
    }

    mFSfw = NULL;
}

/*!
    Add ids of all existing mailboxes to list given as reference.

    \param mailboxIdList The list to receive the mailbox ids.

    \return Error code.
 */
int NmFrameworkAdapter::listMailboxIds(QList<NmId>& mailboxIdList)
{
    NM_FUNCTION;

    QList<NmMailbox*> mailboxList;
    int ret = listMailboxes(mailboxList);

    if (ret == NmNoError) {
        QListIterator<NmMailbox*> iterator(mailboxList);
        while (iterator.hasNext()) {
            NmMailbox *box = iterator.next();
            mailboxIdList.append(box->id());
            delete box;
            box = NULL;
        }
    }
    return ret;
}

/*!
    Fill array given as reference with new mailbox objects, one for each existing mailbox.

    \param mailboxList The list to receive the mailbox object pointers, ownership of which is transferred to caller.

    \return Error code.
 */
int NmFrameworkAdapter::listMailboxes(QList<NmMailbox*>& mailboxList)
{
    NM_FUNCTION;

    // get list of mailboxes from all plugins
    TFSMailMsgId id;
    id.SetNullId();
    RPointerArray<CFSMailBox> mailBoxes;

    //if id.IsNullId(), mailboxes are listed from all plugins.
    //otherwise, only from the given one.
    TInt rcode = mFSfw->ListMailBoxes(id, mailBoxes);

    if (rcode == NmNoError) {
        // convert mailbox data to QT classes
        NmMailbox *box(NULL);
        for (TInt i(0) ; i < mailBoxes.Count(); i++) {
            box = NULL;
            if (mailBoxes[i]) {
                box = mailBoxes[i]->GetNmMailbox();
                }
            if (box) {
                mailboxList.append(box);
                }
            }
        }
    mailBoxes.ResetAndDestroy();
    return rcode;
}

/*!
    Get a pointer to a new mailbox object for the mailbox identified by id.

    \param id Id of the mailbox.
    \param mailbox Pointer reference to receive the mailbox object, ownership of which is transferred to caller.

    \return Error code.
 */
int NmFrameworkAdapter::getMailboxById(const NmId& id, NmMailbox*& mailbox)
{
    NM_FUNCTION;

    const TFSMailMsgId mailMsgId(id.pluginId32(), id.id32());
    CFSMailBox *box(NULL);
    TRAPD(err, box = mFSfw->GetMailBoxByUidL(mailMsgId));
    if (err == KErrNone && box) {
        mailbox = box->GetNmMailbox();
        delete box;
        box = NULL;
    }
    return err;
}

/*!
    Deletes the mailbox with the given id asynchronously.

    \param mailboxId Id of the mailbox to be deleted.

    \return Error code.
 */
QPointer<NmOperation> NmFrameworkAdapter::deleteMailboxById(const NmId& mailboxId)
{
    NM_FUNCTION;

    resetCache(mailboxId);

    QPointer<NmOperation> oper = new NmFwaDeleteMailboxOperation(mailboxId, *mFSfw);
    return oper;
}

/*!
    Returns folder

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the requested folder
    \param message Pointer reference to receive a folder object, ownership is transferred.

    \return Error code.
 */
int NmFrameworkAdapter::getFolderById(const NmId& mailboxId, const NmId& folderId, NmFolder*& folder)
{
    NM_FUNCTION;

    TRAPD(err, getFolderByIdL(mailboxId, folderId, folder));
    return err;
}

/*!
    Leaving version of getFolderById function
 */
void NmFrameworkAdapter::getFolderByIdL(const NmId& mailboxId, const NmId& folderId, NmFolder*& folder)
{
    NM_FUNCTION;

    CFSMailFolder* fsFolder(NULL);
    fsFolder = mFSfw->GetFolderByUidL(TFSMailMsgId(mailboxId), TFSMailMsgId(folderId));
    if (fsFolder) {
        folder = fsFolder->GetNmFolder();
        delete fsFolder;
        fsFolder = NULL;
    } else {
        User::Leave(KErrNotFound);
    }
}


/*!
    Returns message from the store together with whole message part structure

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the folder containing the message.
    \param messageId Id of the message.
    \param message Pointer reference to receive a message object containing requested message,
     ownership is transferred.

    \return Error code.
 */
int NmFrameworkAdapter::getMessageById(
    const NmId& mailboxId,
    const NmId& folderId,
    const NmId& messageId,
    NmMessage*& message)
{
    NM_FUNCTION;

    TRAPD(err, getMessageByIdL(mailboxId, folderId, messageId, message));
    return err;
}

/*!
    Leaving version of getMessageById function
 */
void NmFrameworkAdapter::getMessageByIdL(
    const NmId& mailboxId,
    const NmId& folderId,
    const NmId& messageId,
    NmMessage*& message)
{
    NM_FUNCTION;

    // select message details to be listed
    TFSMailDetails details(EFSMsgDataStructure);

    CFSMailMessage* newMessage(NULL);
    newMessage = mFSfw->GetMessageByUidL(TFSMailMsgId(mailboxId), TFSMailMsgId(folderId),
        TFSMailMsgId(messageId), details);

    // GetMessageByUidL can return NULL pointer
    if (newMessage) {
        message = newMessage->GetNmMessage();
        //assign all children found by mail plugin to NmMessage

        message->removeAllChildParts();
        childrenToNmMessagePartL(newMessage, message);
        delete newMessage;
        newMessage = NULL;
    }
    else {
        User::Leave(KErrNotFound);
    }
}

/*!
    Returns list of folders in a mailbox.

  \param mailboxId Id of the mailbox containing the folder.
  \param folderList Reference to a pointer list to receive pointers to the folders.

  \return Error code.
 */
int NmFrameworkAdapter::listFolders(
    const NmId& mailboxId,
    QList<NmFolder*>& folderList)
{
    NM_FUNCTION;
    int err(NmNoError);
    CFSMailBox* currentMailbox(NULL);
    TRAP(err, currentMailbox = mFSfw->GetMailBoxByUidL(mailboxId));
    if (KErrNone == err && currentMailbox) {
        RPointerArray<CFSMailFolder> folders = currentMailbox->ListFolders();
        for (int i = 0; i < folders.Count(); i++) {
            folderList.append(folders[i]->GetNmFolder());
        }
        delete currentMailbox;
        currentMailbox = NULL;
    } else {
        err = NmNotFoundError;
    }
    return err;
}


/*!
    Fetches all the messages from the given folder and appends their meta data
    into the given list.

    \param folder The folder instance.
    \param messageEnvelopeList The list where the data is stored to.
    \param maxEnvelopeCount The maximum number of messages to get.
*/
void NmFrameworkAdapter::getMessagesFromFolderL(
     CFSMailFolder *folder,
     QList<NmMessageEnvelope*> &messageEnvelopeList,
     const int maxEnvelopeCount)
{
    NM_FUNCTION;
    // validity of input parameters is checked in calling function
    int blockSize = NmListMessagesBlock;
    int maxItemCount = NmMaxItemsInMessageList;

    if (maxEnvelopeCount < NmMaxItemsInMessageList) {
        maxItemCount = maxEnvelopeCount;

        if(maxEnvelopeCount < NmListMessagesBlock) {
            blockSize = maxEnvelopeCount;
        }
    }

    // First prepare all the parameters and select message details to be listed.
    TFSMailDetails details(EFSMsgDataEnvelope);

    // Set the sorting criteria.
    TFSMailSortCriteria criteria;
    criteria.iField = EFSMailSortByDate;
    criteria.iOrder = EFSMailDescending;
    RArray<TFSMailSortCriteria> sorting;
    CleanupClosePushL(sorting);
    sorting.Append(criteria);
    
    NM_TIMESTAMP("ListMessagesL in getMessagesFromFolderL start");   

    // Get the message list from the backend.
    MFSMailIterator* iterator(NULL);
    iterator = folder->ListMessagesL(details, sorting);
    
    NM_TIMESTAMP("ListMessagesL in getMessagesFromFolderL end");

    if (iterator) {
        CleanupStack::PushL(iterator);
        RPointerArray<CFSMailMessage> messages;
        CleanupResetAndDestroy<CFSMailMessage>::PushL(messages);

        // The message list is fetched in blocks to prevent OOM in protocol
        // plugin side.
        bool moreMessagesToFollow(false);
        moreMessagesToFollow = iterator->NextL(TFSMailMsgId(), blockSize, messages);

        for (int i = blockSize;
             i < maxItemCount && moreMessagesToFollow;
             i += blockSize) {
            NM_TIMESTAMP("  NextL in getMessagesFromFolderL begins");            
            moreMessagesToFollow =
                iterator->NextL(messages[i-1]->GetMessageId(), blockSize, messages);
            NM_TIMESTAMP("  NextL in getMessagesFromFolderL end");              
        }

        // Add all the found emails into the result list.
        const TInt messageCount(messages.Count());

        for (TInt i = 0; i < messageCount; ++i) {
            NmMessageEnvelope *newEnvelope(NULL);
            newEnvelope = messages[i]->GetNmMessageEnvelope();

            if (newEnvelope) {
                messageEnvelopeList.append(newEnvelope);
            }
        }

        CleanupStack::PopAndDestroy(&messages);
        CleanupStack::Pop(iterator);
        delete iterator;
        iterator = NULL;
    }

    CleanupStack::PopAndDestroy(); // sorting
}


/*!
    Returns list of envelopes from the backend for specific mailbox and folder.

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Folder id.
    \param messageMetaDataList Reference to pointer list to receive the envelope objects,
     ownership is transferred.

    \return Error code.
 */
int NmFrameworkAdapter::listMessages(
	const NmId &mailboxId,
    const NmId &folderId,
    QList<NmMessageEnvelope*> &messageEnvelopeList)
{
    NM_FUNCTION;

    TRAPD(err, listMessagesL(mailboxId,folderId,messageEnvelopeList, NmMaxItemsInMessageList));
    return err;
}


/*!
    Fetches the meta data for each message in the given mailbox and given
    folder.

    \param mailboxId The ID of the mailbox of which messages to list.
    \param folderId The ID of the folder of which messages to list.
    \param messageEnvelopeList The list where the message data is stored to.
                               Note that the ownership is transferred!
    \param maxAmountOfEnvelopes The maximum number of messages to list.

    \return If success, KErrNone, an error code otherwise.
*/
int NmFrameworkAdapter::listMessages(
        const NmId& mailboxId,
        const NmId& folderId,
        QList<NmMessageEnvelope*> &messageEnvelopeList,
        const int maxAmountOfEnvelopes)
{
    NM_FUNCTION;

    TInt err(KErrNone);
    TRAP(err, listMessagesL(mailboxId,folderId, messageEnvelopeList,maxAmountOfEnvelopes) );
    return err;
}


/*!
    Fetches the meta data for each message in the given mailbox and given
    folder. Note that this private method can leave.
*/
void NmFrameworkAdapter::listMessagesL(
        const NmId &mailboxId,
        const NmId &folderId,
        QList<NmMessageEnvelope*> &messageEnvelopeList,
        const int maxAmountOfEnvelopes)
{
    NM_FUNCTION;

    // If we are requesting 0 or less mails, we can just return.
    if (maxAmountOfEnvelopes <= 0) {
        return;
    }

    CFSMailBox *mailbox(NULL);
    mailbox = mFSfw->GetMailBoxByUidL(mailboxId);

    if (!mailbox) {
        User::Leave(KErrNotFound);
    }

    CleanupStack::PushL(mailbox);

    CFSMailFolder* folder(NULL);
    folder = mFSfw->GetFolderByUidL(mailbox->GetId(), TFSMailMsgId(folderId));

    if (folder) {
        CleanupStack::PushL(folder);
        getMessagesFromFolderL(folder, messageEnvelopeList, maxAmountOfEnvelopes);
        CleanupStack::PopAndDestroy(folder);
    }

    CleanupStack::PopAndDestroy(mailbox);
}


/*!
    Returns list of messages from the backend for specific mailbox and folder.

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Folder id.
    \param messageMetaDataList Reference to pointer list to receive the envelope objects,
     ownership is transferred.

    \return Error code.
 */
int NmFrameworkAdapter::listMessages(
    const NmId &mailboxId,
    const NmId &folderId,
    QList<NmMessage*> &messageList,
    const int maxAmountOfMessages)
{
    NM_FUNCTION;

    TRAPD(err, listMessagesL(mailboxId,folderId,messageList, maxAmountOfMessages));
    return err;
}


/*!
    Leaving version of list messages with NmMessageList input
 */
void NmFrameworkAdapter::listMessagesL(
        const NmId &mailboxId,
        const NmId &folderId,
        QList<NmMessage*> &messageList,
        const int maxAmountOfEnvelopes)
{
    NM_FUNCTION;

    CFSMailBox* currentMailbox(NULL);
    CFSMailFolder* folder(NULL);

    //If we are requesting 0 or less mails so we can return
    if( maxAmountOfEnvelopes <= 0)
        {
        return;
        }

    int blockSize = NmListMessagesBlock;
    int maxLimit = NmMaxItemsInMessageList;

    if( maxAmountOfEnvelopes < NmMaxItemsInMessageList )
        {
        maxLimit = maxAmountOfEnvelopes;
        if(maxAmountOfEnvelopes < NmListMessagesBlock)
            {
            blockSize = maxAmountOfEnvelopes;
            }
        }

    currentMailbox = mFSfw->GetMailBoxByUidL(mailboxId);
    if (!currentMailbox) {
        User::Leave(KErrNotFound);
    }
    CleanupStack::PushL(currentMailbox);
    folder = mFSfw->GetFolderByUidL(currentMailbox->GetId(), TFSMailMsgId(folderId));

    if (folder) {
        CleanupStack::PushL(folder);
        // First prepare all the parameters
        // select message details to be listed
        TFSMailDetails details(EFSMsgDataEnvelope);

        // set sorting criteria
        TFSMailSortCriteria criteria;
        criteria.iField = EFSMailSortByDate;
        criteria.iOrder = EFSMailDescending;
        RArray<TFSMailSortCriteria> sorting;
        CleanupClosePushL(sorting);
        sorting.Append(criteria);

        TFSMailMsgId currentMessageId; // first call contains NULL id as begin id
        // get messages list from the backend
        MFSMailIterator* iterator(NULL);

        iterator = folder->ListMessagesL(details, sorting);
        if (iterator) {
            CleanupStack::PushL(iterator);
            RPointerArray<CFSMailMessage> messages;
            CleanupResetAndDestroy<CFSMailMessage>::PushL(messages);

            //Message list is fetched in blocks to prevent OOM in protocol plugin side
            bool moreMessagesToFollow(false);
            moreMessagesToFollow = iterator->NextL(
                TFSMailMsgId(), blockSize, messages);
            for ( int i = blockSize;
                  i < maxLimit && moreMessagesToFollow ;
                  i += blockSize ) {
                moreMessagesToFollow = iterator->NextL(
                    messages[i-1]->GetMessageId(), blockSize, messages);

            }

            //Add all found emails to the result list
            for(TInt i=0; i<messages.Count(); i++) {
                NmMessage* newMessage(NULL);
                newMessage = messages[i]->GetNmMessage();
                if (newMessage) {
					//Add content of message
                    NmMessagePart *plainTextPart = newMessage->plainTextBodyPart();
                    if (plainTextPart) {
                        contentToMessagePart(mailboxId, folderId, newMessage->envelope().messageId(),
                                             *plainTextPart);
                    }
                    messageList.append(newMessage);
                }
            }

            CleanupStack::PopAndDestroy( &messages );
            CleanupStack::Pop(iterator);
            delete iterator;
            iterator = NULL;
        }
        CleanupStack::PopAndDestroy(); // sorting
        CleanupStack::PopAndDestroy(folder);
    }
    CleanupStack::PopAndDestroy(currentMailbox);
}


/*!
    Starts an asynchronous search for messages with the given search strings.
    This is part of the public interface.

    \see NmFrameworkAdapter::searchL().

    \param mailboxId The mailbox to search from.
    \param searchStrings The strings to search.

    \return A possible error code.
*/
int NmFrameworkAdapter::search(const NmId &mailboxId,
                               const QStringList &searchStrings)
{
    NM_FUNCTION;

    if (!mSearchObserver) {
        mSearchObserver = new NmMailboxSearchObserver();
    }

    // Set connections for forwarding the signals emitted by the search
    // observer.
    connect(mSearchObserver, SIGNAL(matchFound(const NmId &, const NmId &)),
            this, SIGNAL(matchFound(const NmId &, const NmId &)), Qt::UniqueConnection);
    connect(mSearchObserver, SIGNAL(searchComplete()),
            this, SIGNAL(searchComplete()), Qt::UniqueConnection);

    TRAPD(err, searchL(mailboxId, QList<NmId>(), searchStrings, *mSearchObserver));
    return err;
}


/*!
    Cancels the search if one is ongoing.

    \param mailboxId The ID of the mailbox running the search.

    \return A possible error code.
*/
int NmFrameworkAdapter::cancelSearch(const NmId &mailboxId)
{
    NM_FUNCTION;

    // Get the mailbox with the given ID.
    CFSMailBox *mailbox(NULL);
    TRAPD(err, mailbox = mFSfw->GetMailBoxByUidL(mailboxId));

    if (err == KErrNone && mailbox) {
        mailbox->CancelSearch();
        delete mailbox;
        mailbox = NULL;
    }

    return err;
}

/*!
    Indicates application state information to protocol plugins
    \param mailboxId Id of active mailbox, 0 if application is closed.
    \param folderId Id of active folder, 0 if application is closed.
*/
void NmFrameworkAdapter::updateActiveFolder(
    const NmId &mailboxId, const NmId &folderId)
{
    TRAP_IGNORE(doUpdateActiveFolderL(mailboxId, folderId));
}

/*!
   Removes draft message asynchronously
 */
QPointer<NmOperation> NmFrameworkAdapter::removeDraftMessage(NmMessage *message)
{
    NM_FUNCTION;

    QPointer<NmOperation> oper = new NmFwaRemoveDraftMessageOperation(*this, message, *mFSfw);
    return oper;
}

/*!
   function to process updateActiveFolder. This method may leave.
 */
void NmFrameworkAdapter::doUpdateActiveFolderL(
    const NmId &mailboxId, const NmId &folderId)
{
    if ((mFSfw) && (!mCurrentMailBox || mCurrentMailBox->GetId()!=mailboxId)) {
        delete mCurrentMailBox;
        mCurrentMailBox = NULL;
        mCurrentMailBox = mFSfw->GetMailBoxByUidL(mailboxId);
        }
    CEmailExtension *extension = getEMailStateExtensionL();
    CMailboxStateExtension *stateExtension =
        static_cast<CMailboxStateExtension*>(extension);
    if (stateExtension) {
        stateExtension->NotifyActiveFolderChanged(mailboxId, folderId);
    }
}

/*!
   function to process updateActiveFolder. This method may leave.
 */
CEmailExtension* NmFrameworkAdapter::getEMailStateExtensionL()
{
    if (!mStateExtension && mCurrentMailBox) {
        // This extension is owned and deleted by the plugin, so no need to
        // use release unless the extension will be relocated into extensionbase.
        mStateExtension =  mCurrentMailBox->ExtensionL(KEmailMailboxStateExtensionUid);
    }
    return mStateExtension;
}

/*!
    Starts an asynchronous search for messages with the given search strings.

    \param mailboxId The mailbox to search from.
    \param folderIds (not used)
    \param searchStrings The strings to search.
    \param searchObserver The observer which gets informed about the progress
                          of the search (match found, search complete etc.)
*/
void NmFrameworkAdapter::searchL(const NmId &mailboxId,
                                 const QList<NmId> &folderIds,
                                 const QStringList &searchStrings,
                                 NmMailboxSearchObserver &searchObserver)
{
    NM_FUNCTION;

    // CFSMailBox has no support for search using folder IDs.
    Q_UNUSED(folderIds);

    // Get the mailbox with the given ID.
    CFSMailBox *mailbox(NULL);
    mailbox = mFSfw->GetMailBoxByUidL(mailboxId);

    if (mailbox) {
        CleanupStack::PushL(mailbox);
        RPointerArray<TDesC> strings;
        CleanupResetAndDestroy<TDesC>::PushL(strings);

        // Convert the search strings to HBufCs.
        foreach (QString string, searchStrings) {
            HBufC *buffer = NmConverter::qstringToHBufCLC(string);
            strings.AppendL(buffer);
            CleanupStack::Pop(buffer);
        }

        // Show results in ascending date/time order.
        TFSMailSortCriteria sortCriteria;
        sortCriteria.iField = EFSMailSortByDate;
        sortCriteria.iOrder = EFSMailAscending;

        // Start the search.
        mailbox->SearchL(strings, sortCriteria, searchObserver);

        // Clean up.
        strings.ResetAndDestroy();
        CleanupStack::Pop(&strings);
        CleanupStack::PopAndDestroy(mailbox);
    }
}


/*!
    Starts a message fetching operation.

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the folder containing the message.
    \param messageId Id of the message to fetch.

    \return An NmOperation object for the operation, ownership is transferred to caller
 */
QPointer<NmOperation> NmFrameworkAdapter::fetchMessage(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId )
{
    NM_FUNCTION;

    NmOperation *oper = new NmFwaMessageFetchingOperation(mailboxId, folderId, messageId, *mFSfw);
    return oper;
}

/*!
    Starts a message part fetching operation.

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the folder containing the message.
    \param messageId Id of message containing the message parts
    \param messagePartId id of message part

    \return An NmOperation object for the operation, ownership is transferred to caller
 */
QPointer<NmOperation> NmFrameworkAdapter::fetchMessagePart(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    const NmId &messagePartId)
{
    NM_FUNCTION;

    QPointer<NmOperation> oper = new NmFwaMessagePartFetchingOperation(
            mailboxId, folderId, messageId, messagePartId, *mFSfw);
    return oper;
}

/*!
    Starts a message parts fetching operation.

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the folder containing the message.
    \param messageId Id of message containing the message parts
    \param messagePartIds ids of message parts

    \return An NmOperation object for the operation, ownership is transferred to caller
 */
QPointer<NmOperation> NmFrameworkAdapter::fetchMessageParts(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    const QList<NmId> &messagePartIds)
{
    NM_FUNCTION;

    QPointer<NmOperation> oper = new NmFwaMessagePartsFetchingOperation(
            mailboxId, folderId, messageId, messagePartIds, *mFSfw);
    return oper;
}

/*!
    Returns sharable file handle to message part content

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the folder containing the message.
    \param messageId Id of message containing the message parts
    \param messagePartId id of message part

    \return XQSharableFile, sharable file object
 */
XQSharableFile NmFrameworkAdapter::messagePartFile(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        const NmId &messagePartId)
{
    NM_FUNCTION;

    XQSharableFile retFile;
    TFSMailDetails details(EFSMsgDataEnvelope);
    TFSMailMsgId fsMboxId(mailboxId);
    TFSMailMsgId fsFolderId(folderId);
    TFSMailMsgId fsMsgId(messageId);
    TFSMailMsgId fsMsgPartId(messagePartId);

    CFSMailMessage* fsMessage(NULL);
    int error(KErrNone);
    TRAP(error, fsMessage = mFSfw->GetMessageByUidL(fsMboxId, fsFolderId,
            fsMsgId, details) );

    CFSMailMessagePart* fsMessagePart(NULL);
    if (fsMessage && error == KErrNone) {
        TRAP(error, fsMessagePart = fsMessage->ChildPartL(fsMsgPartId) );

    }
    if (fsMessagePart && error == KErrNone) {
        RFile file = fsMessagePart->GetContentFileL();
        retFile.setHandle(file);
    }
    delete fsMessagePart;
    fsMessagePart = NULL;
    delete fsMessage;
    fsMessage = NULL;
    return retFile;
}

/*!
    Get the id of a standard folder.

    \param mailboxId Id of the mailbox containing the folder.
    \param folderType Type of standard folder to get the id of.

    \return Id of the standard folder, or NmId for which NmId.getId() == 0 if not found.
 */
NmId NmFrameworkAdapter::getStandardFolderId(
    const NmId& mailboxId,
    NmFolderType folderType )
{
    NM_FUNCTION;

	// Use cached inbox ID when available
    if (folderType==NmFolderInbox &&
        mailboxId==mCachedMailboxId &&
        mCachedInboxId!=0) {
        return mCachedInboxId;
    }

    TFSMailMsgId folderId;
    NmId resultId(0);
    CFSMailBox* currentMailbox(NULL);

    TRAPD(error, currentMailbox = mFSfw->GetMailBoxByUidL(mailboxId));

    if(!currentMailbox || error != KErrNone) {
      return resultId;
    }

    switch(folderType) {
      case NmFolderInbox:
        folderId = currentMailbox->GetStandardFolderId(EFSInbox);
        break;
      case NmFolderOutbox:
        folderId = currentMailbox->GetStandardFolderId(EFSOutbox);
        break;
      case NmFolderDrafts:
        folderId = currentMailbox->GetStandardFolderId(EFSDraftsFolder);
        break;
      case NmFolderSent:
        folderId = currentMailbox->GetStandardFolderId(EFSSentFolder);
        break;
      case NmFolderDeleted:
        folderId = currentMailbox->GetStandardFolderId(EFSDeleted);
        break;
      case NmFolderOther:
      default:
        folderId = currentMailbox->GetStandardFolderId(EFSOther);
        break;
    }

    delete currentMailbox;
    currentMailbox = NULL;

    resultId.setPluginId32(static_cast<quint32>(folderId.PluginId().iUid));
    resultId.setId32(folderId.Id());

    // Store cached inbox ID
    if (folderType==NmFolderInbox) {
        mCachedMailboxId = mailboxId;
        mCachedInboxId = resultId;
    }

    return resultId;
}

/*!
    Reset cache for the specific mailbox

    \param mailboxId id of the mailbox that cache will be reseted
*/
void NmFrameworkAdapter::resetCache(const NmId &mailboxId)
{
	if (mailboxId==mCachedMailboxId) {
        mCachedMailboxId=0;
        mCachedInboxId=0;
	}
}

/*!
    Connect to mailbox if not already connected and refresh.

    \param mailboxId Id of the mailbox.
    \return Async request id or error code.
 */
int NmFrameworkAdapter::refreshMailbox(NmId mailboxId)
{
    NM_FUNCTION;

    resetCache(mailboxId);

    TRAPD(err, RefreshMailboxL(mailboxId)); // return value not used
    return (err == KErrNone) ? NmNoError : NmGeneralError;
}

/*!
    Connect to mailbox if not already connected and refresh.

    \param mailboxId Id of the mailbox.
    \return Async request id or error code.
 */
int NmFrameworkAdapter::goOnline(const NmId& mailboxId)
{
    NM_FUNCTION;

    TRAPD(err, GoOnlineL(mailboxId)); // return value not used
    return (err == KErrNone) ? NmNoError : NmGeneralError;
}

/*!
    Disconnect to mailbox if not already disconnected.

    \param mailboxId Id of the mailbox.
    \return Async request id or error code.
 */
int NmFrameworkAdapter::goOffline(const NmId& mailboxId)
{
    NM_FUNCTION;

    TRAPD(err, GoOfflineL(mailboxId)); // return value not used
    return (err == KErrNone) ? NmNoError : NmGeneralError;
}

/*!
    Sets content for the given message part. Client shouldn't create
    message part on its own. Instead it should be requested by calling
    e.g. getMessageById.

    \param mailboxId Id of the mailbox containing the folder (Unused).
    \param folderId Id of the folder containing the message (Unused).
    \param messageId Id of the message (Unused).
    \param messagePart Part of a message for which content is going to be requested.

    \return Error code.
 */
int NmFrameworkAdapter::contentToMessagePart(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    NmMessagePart &messagePart)
{
    NM_FUNCTION;

    TRAPD(err, contentToMessagePartL(mailboxId,folderId,messageId,messagePart));
    return err;
}

/*!
    Leaving version of contentToMessagePart function
 */
void NmFrameworkAdapter::contentToMessagePartL(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    NmMessagePart &messagePart)
{
    NM_FUNCTION;

    CFSMailMessagePart* cfsPart = CFSMailMessagePart::NewLC(messageId,messagePart);
    cfsPart->SetMailBoxId(TFSMailMsgId(mailboxId));
    cfsPart->SetFolderId(TFSMailMsgId(folderId));
    QString contentType = messagePart.contentType();

    if (contentType.startsWith(NmContentTypeTextPlain)) {
        // add 1 for max size for zero termination and prevend 0 size hbufc
        HBufC* data = HBufC::NewLC(cfsPart->FetchedContentSize()+1);
        TPtr dataPtr = data->Des();
        TInt startOffset = 0;

        cfsPart->GetContentToBufferL(dataPtr, startOffset);
        messagePart.setTextContent(NmConverter::toQString(dataPtr), contentType);
        CleanupStack::PopAndDestroy(data);
    } else {
        RFile file = cfsPart->GetContentFileL();
        TInt fileSize = 0;
        file.Size(fileSize);
        if (fileSize != 0) {
            // Reserve buffer for content
            QByteArray byteArray;
            QT_TRYCATCH_LEAVING(byteArray = QByteArray(fileSize, 0));
            TUint8* ptr = reinterpret_cast<unsigned char*>(byteArray.data());
            TPtr8 dataPtr(ptr, byteArray.capacity());

            // Read data from the file
            TInt dummyPos = 0;
            file.Seek(ESeekStart, dummyPos);
            User::LeaveIfError(file.Read(dataPtr));

            if (contentType.startsWith(NmContentTypeTextHtml) || contentType.contains( NmContentDescrAttachmentHtml)) {
                QRegExp rxlen("(?:charset=)(?:\"?)([\\-\\_a-zA-Z0-9]+)", Qt::CaseInsensitive);
                QString charset;
                int pos = rxlen.indexIn(contentType);
                if (pos > -1) {
                    charset = rxlen.cap(1);
                }
                QTextCodec *codec = QTextCodec::codecForName(charset.toAscii());
                if (!codec) {
                    codec = QTextCodec::codecForName("UTF-8");
                }
                QString encodedStr = codec->toUnicode(byteArray);
                messagePart.setTextContent(encodedStr, contentType);
            } else {
                messagePart.setBinaryContent(byteArray, contentType);
            }
        }
        file.Close();
    }
    CleanupStack::PopAndDestroy(cfsPart);
}

/*!
    Deletes the listed messages from specific mailbox and folder.

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the folder containing the messages.
    \param messageIdList List of ids of the messages to be deleted.

    \return Error code.
 */
int NmFrameworkAdapter::deleteMessages(
    const NmId &mailboxId,
    const NmId &folderId,
    const QList<NmId> &messageIdList)
{
    NM_FUNCTION;

    TInt err(NmNoError);
    RArray<TFSMailMsgId> messageIds;
    for (TInt i(0); i < messageIdList.size(); i++) {
        err = messageIds.Append(TFSMailMsgId(messageIdList[i]));
        if (err != NmNoError) {
            break;
        }
    }
    if (NmNoError == err) {
        TRAP(err, mFSfw->DeleteMessagesByUidL(TFSMailMsgId(mailboxId), TFSMailMsgId(folderId), messageIds));
    }
    messageIds.Close();

    return (err == NmNoError) ? NmNoError : NmGeneralError;
}

/*!
    Starts an async operation to store listed message envelopes to the given mailbox

    \param mailboxId Id of the mailbox.
    \param folderId Unused.
    \param envelopeList List of the envelope objects to store.

    \return NmStoreEnvelopesOperation
 */
QPointer<NmStoreEnvelopesOperation> NmFrameworkAdapter::storeEnvelopes(
    const NmId &mailboxId,
    const NmId &folderId,
    const QList<const NmMessageEnvelope*> &envelopeList)
{
    NM_FUNCTION;

    Q_UNUSED(folderId);

    QPointer<NmStoreEnvelopesOperation> operation(NULL);
    RPointerArray<CFSMailMessage> envelopeMessages;

    int count(envelopeList.count());
    for(int i(0); i < count; i++) {
        TRAP_IGNORE(envelopeMessages.AppendL(mailMessageFromEnvelopeL(*envelopeList.at(i))));
    }

    if (envelopeMessages.Count()) {
        operation = new NmFwaStoreEnvelopesOperation(
                mailboxId,
                envelopeMessages,
                *mFSfw);
        }

    return operation;
}

/*!
    Creates a new message into the drafts folder a mailbox.

    \param mailboxId Id of the mailbox.

    \return NmMessageCreationOperation
 */
QPointer<NmMessageCreationOperation> NmFrameworkAdapter::createNewMessage(const NmId &mailboxId)
{
    NM_FUNCTION;

    QPointer<NmMessageCreationOperation> oper =
        new NmFwaMessageCreationOperation(mailboxId, *mFSfw);
    return oper;
}

/*!
    Creates a new forward message into the drafts folder of a mailbox.

    \param mailboxId Id of the mailbox.
    \param originalMessageId Id of the original message. Original message must be in the given mailbox.

    \return NmMessageCreationOperation
 */
QPointer<NmMessageCreationOperation> NmFrameworkAdapter::createForwardMessage(
    const NmId &mailboxId,
    const NmId &originalMessageId)
{
    NM_FUNCTION;

    QPointer<NmMessageCreationOperation> oper =
        new NmFwaForwardMessageCreationOperation(mailboxId, originalMessageId, *mFSfw);
    return oper;
}

/*!
    Creates a new reply message into the drafts folder of a mailbox.

    \param mailboxId Id of the mailbox.
    \param originalMessageId Id of the original message. Original message must be in the given mailbox.
    \param replyAll Is reply for all recipients?

    \return NmMessageCreationOperation
 */
QPointer<NmMessageCreationOperation> NmFrameworkAdapter::createReplyMessage(
    const NmId &mailboxId,
    const NmId &originalMessageId,
    const bool replyAll)
{
    NM_FUNCTION;

    QPointer<NmMessageCreationOperation> oper =
        new NmFwaReplyMessageCreationOperation(mailboxId, originalMessageId, replyAll, *mFSfw);
    return oper;
}

/*!
    Not implemented yet.
 */
int NmFrameworkAdapter::saveMessage(const NmMessage &message)
{
    NM_FUNCTION;

    Q_UNUSED(message);
    return NmNoError;
}

/*!
    Store asynchronously message with its parts.
 */
QPointer<NmOperation> NmFrameworkAdapter::saveMessageWithSubparts(const NmMessage &message)
{
    NM_FUNCTION;

    CFSMailMessage * cfsMessage = NULL;
    QPointer<NmOperation> oper(NULL);

    int err = KErrNone;
    TRAP(err, cfsMessage = CFSMailMessage::NewL(message));
    if (err == KErrNone) {
        //transfers ownership of cfsMessage
        oper = new NmFwaStoreMessageOperation(cfsMessage, *mFSfw);
    }

    return oper;
}

/*!
    From MFSMailEventObserver

    \sa MFSMailEventObserver
 */
void NmFrameworkAdapter::EventL(
    TFSMailEvent aEvent,
    TFSMailMsgId mailbox,
    TAny* param1,
    TAny* param2,
    TAny* param3)
{
    NM_FUNCTION;

    switch (aEvent) {
        case TFSEventMailboxSettingsChanged:
            break;

        // Folder related events:
        case TFSEventFoldersDeleted:
            handleFoldersEvent(param1, param2, NmFolderIsDeleted, mailbox);
            break;
        case TFSEventNewFolder:
            handleFoldersEvent(param1, param2, NmFolderIsCreated, mailbox);
            break;

        // Mailbox related events:
        case TFSEventNewMailbox:
            handleMailboxEvent(mailbox, NmMailboxCreated);
            break;
        case TFSEventMailboxDeleted:
            handleMailboxEvent(mailbox, NmMailboxDeleted);
            break;
        case TFSEventMailboxRenamed:
            handleMailboxEvent(mailbox, NmMailboxChanged);
            break;

        //
        // Mail related events:
        // signal:
        // void messageEvent(
        //      NmMessageEvent event,
        //      const NmId &folderId,
        //      QList<NmId> &messageIds
        //      const NmId& mailboxId);
        //
        // enum NmMessageEvent
        //   NmMessageCreated,
        //   NmMessageChanged,
        //   NmMessageDeleted
        //

        // New mails created
        // param1: RArray<TFSMailMsgId>* aNewEntries
        // param2: TFSMailMsgId* aParentFolder
        // param3: NULL
        case TFSEventNewMail:
            handleMessageEvent(param1, param2, NmMessageCreated, mailbox);
            break;
        // Mails changed
        // param1: RArray<TFSMailMsgId>* aEntries
        // param2: TFSMailMsgId* aParentFolder
        // param3: NULL
        case TFSEventMailChanged:
            handleMessageEvent(param1, param2, NmMessageChanged, mailbox);
            break;
        // Mails deleted
        // param1: RArray<TFSMailMsgId>* aEntries
        // param2: TFSMailMsgId* aParentFolder
        // param3: NULL
        case TFSEventMailDeleted:
            handleMessageEvent(param1, param2, NmMessageDeleted, mailbox);
            break;


        // Mails moved
        // param1: RArray<TFSMailMsgId>* aEntries
        // param2: TFSMailMsgId* aNewParentFolder
        // param3: TFSMailMsgId* aOldParentFolder
        case TFSEventMailMoved:
            if(param2 && param3)
                {
                handleMailMoved(param1, param2, param3, mailbox);
                }            
            break;

        // Mails copied
        // param1: RArray<TFSMailMsgId>* aNewEntries
        // param2: TFSMailMsgId* aNewParentFolder
        // param3: TFSMailMsgId* aOldParentFolder
        case TFSEventMailCopied:
            handleMailCopied(param1, param2, mailbox);
            break;


        //sync state related events
        case TFSEventMailboxSyncStateChanged:
            handleSyncstateEvent(param1, mailbox);
            break;

        case TFSEventMailboxOnline:{
            NmId id = NmConverter::mailMsgIdToNmId(mailbox);
            emit connectionEvent(Connected, id, NmNoError);
            }
            break;

        // param1: errorcode
        case TFSEventMailboxOffline: {
            NmId id = NmConverter::mailMsgIdToNmId(mailbox);
            TInt error(KErrNone);
            // if param1 is set, and it is != KErrNone, an unexpected offline event has occurred
            if(param1) {
                error=*(static_cast<TInt*>(param1));
            }
            if(error) {
                emit connectionEvent(Disconnected, id, NmConnectionError);
            }
            else {
                emit connectionEvent(Disconnected, id, NmNoError);
            }
      }
            break;

        default:
            break;
    }
}

/*!
    Delete the message from specific mailbox and folder.

    \param mailboxId Id of the mailbox containing the folder.
    \param folderId Id of the folder containing the message.
    \param messageId Id of the message.

    \return Error code.
 */
int NmFrameworkAdapter::removeMessage(
    const NmId& mailboxId,
    const NmId& folderId,
    const NmId& messageId)
{
    NM_FUNCTION;

    TRAPD(error, removeMessageL(mailboxId, folderId, messageId));
    return error;
}

/*!
    Copy messages between folders from specific mailbox.

    \param mailboxId Id of the mailbox containing messages.
    \param messageIds The list of source message Ids.
    \param newMessages The list of destination message Ids.
    \param sourceFolderId Id of source folder.
    \param destinationFolderId Id of destination folder.
 */
int NmFrameworkAdapter::copyMessages(
    const NmId &mailboxId,
    const QList<quint64>& messageIds,
    const NmId& sourceFolderId,
    const NmId& destinationFolderId)
{
    NM_FUNCTION;
    TRAPD(error, copyMessagesL(mailboxId, messageIds, sourceFolderId, destinationFolderId));
    return error;
}

/*!
    Gets the signature for the given mailbox.

    \param mailboxId Id of the mailbox.
*/
int NmFrameworkAdapter::getSignature(const NmId &mailboxId, QString *&signature)
{
    NM_FUNCTION;
    TRAPD(error, getSignatureL(mailboxId, signature));

    return error;
}

/*!
    Subscribe to events from a mailbox.

    \param mailboxId Id of the mailbox.
*/
void NmFrameworkAdapter::subscribeMailboxEvents(const NmId& mailboxId)
{
    NM_FUNCTION;

    TRAP_IGNORE(mFSfw->SubscribeMailboxEventsL(mailboxId, *this));
}

/*!
    Unsubscribe to events from a mailbox.

    \param mailboxId Id of the mailbox.
*/
void NmFrameworkAdapter::unsubscribeMailboxEvents(const NmId& mailboxId)
{
    NM_FUNCTION;

    mFSfw->UnsubscribeMailboxEvents(mailboxId, *this);
}


/*!
    Leaving version of removeMessage
*/
void NmFrameworkAdapter::removeMessageL(
    const NmId& mailboxId,
    const NmId& folderId,
    const NmId& messageId)
{
    NM_FUNCTION;

    CFSMailFolder* folder = mFSfw->GetFolderByUidL( TFSMailMsgId(mailboxId), TFSMailMsgId(folderId));
    CleanupStack::PushL(folder);
    if ( folder ) {
        folder->RemoveMessageL(TFSMailMsgId(messageId));
    }
    CleanupStack::PopAndDestroy(folder);
}

/*!
    Leaving version of copyMessages
*/
void NmFrameworkAdapter::copyMessagesL(
    const NmId &mailboxId,
    const QList<quint64>& messageIds,
    const NmId& sourceFolderId,
    const NmId& destinationFolderId)
{
    NM_FUNCTION;
    TInt count(messageIds.count());
    if (!count) {
        return;
    }
    RArray<TFSMailMsgId> messages;
    RArray<TFSMailMsgId> copiedMessages;

    CleanupClosePushL(messages);
    CleanupClosePushL(copiedMessages);

    for (TInt i = 0; i < count; i++) {
        NmId tmpId(messageIds[i]);
        messages.AppendL(TFSMailMsgId(tmpId));
    }

    CFSMailBox* mailBox(NULL);
    mailBox = mFSfw->GetMailBoxByUidL(mailboxId);
    if (mailBox) {
        mailBox->CopyMessagesL(messages, copiedMessages,
            TFSMailMsgId(sourceFolderId),
            TFSMailMsgId(destinationFolderId));
        delete mailBox;
        mailBox = NULL;
    } else {
        User::Leave(NmNotFoundError);
    }
    CleanupStack::PopAndDestroy(2,&messages);
}

/*!
    Leaving version of getSignature
*/
void NmFrameworkAdapter::getSignatureL(const NmId &mailboxId, QString *&signature)
{
    NM_FUNCTION;

    HBufC *sig = mFSfw->GetSignatureL(TFSMailMsgId(mailboxId));

    if (sig) {
        signature = new QString(NmConverter::toQString(*sig));
    }
}


/*!
   Sends the given message.
 */
QPointer<NmMessageSendingOperation> NmFrameworkAdapter::sendMessage(
    NmMessage *message)
{
    NM_FUNCTION;

    QPointer<NmMessageSendingOperation>oper = new NmFwaMessageSendingOperation(*this, message, *mFSfw);
    return oper;
}

/*!
   Add attachment into the given message.
 */
QPointer<NmAddAttachmentsOperation> NmFrameworkAdapter::addAttachments(
    const NmMessage &message,
    const QList<QString> &fileList)
{
    NM_FUNCTION;

    QPointer<NmAddAttachmentsOperation>oper = new NmFwaAddAttachmentsOperation(message, fileList, *mFSfw);
    return oper;
}

/*!
   Remove attachment from the given message.
 */
QPointer<NmOperation> NmFrameworkAdapter::removeAttachment(
    const NmMessage &message,
    const NmId &attachmentPartId)
{
    NM_FUNCTION;

    QPointer<NmOperation> oper = new NmFwaRemoveAttachmentOperation(message, attachmentPartId, *mFSfw);
    return oper;
}

/*!
    Returns the current sync state of the given mailbox
 */
NmSyncState NmFrameworkAdapter::syncState(const NmId& mailboxId) const
{
    NM_FUNCTION;

   CFSMailBox* mailBox = NULL;
   TRAPD(err, mailBox = mFSfw->GetMailBoxByUidL(TFSMailMsgId(mailboxId)) );
   if (KErrNone == err && mailBox) {
       TSSMailSyncState syncState = mailBox->CurrentSyncState();
       delete mailBox;
       mailBox = NULL;
       if (EmailSyncing == syncState) {
           return Synchronizing;
       }
       else {
           return SyncComplete;
       }
   }
   else {
       return SyncComplete;
   }
}

/*!
    Returns the current connection state of the given mailbox
 */
NmConnectState NmFrameworkAdapter::connectionState(const NmId& mailboxId) const
{
    NM_FUNCTION;

    CFSMailBox* mailBox(NULL);
    TRAPD(err, mailBox = mFSfw->GetMailBoxByUidL(TFSMailMsgId(mailboxId)) );
    if (KErrNone == err && mailBox) {
        TFSMailBoxStatus status = mailBox->GetMailBoxStatus();
        delete mailBox;
        mailBox = NULL;
        if (status == EFSMailBoxOnline) {
            return Connected;
        }
        else {
            return Disconnected;
        }
    }
    else {
       return Disconnected;
    }
}

/*!
   creates CFSMailMessage based on envelope
   Call to this must be trapped
 */
CFSMailMessage* NmFrameworkAdapter::mailMessageFromEnvelopeL(
    const NmMessageEnvelope& envelope)
{
    NM_FUNCTION;

    NmMessage* nmMessage = new(ELeave) NmMessage( envelope );
    CFSMailMessage* message = CFSMailMessage::NewL( *nmMessage );
    delete nmMessage;
    nmMessage = NULL;
    return message;
}


/*!
   Assigns recursively all children to NmMessagePart object.
 */
void NmFrameworkAdapter::childrenToNmMessagePartL(
        CFSMailMessagePart *cfsParent,
        NmMessagePart *nmParent)
{
    NM_FUNCTION;

    User::LeaveIfNull(cfsParent);
    User::LeaveIfNull(nmParent);

    RPointerArray<CFSMailMessagePart> parts;
    cfsParent->ChildPartsL(parts);
    CleanupResetAndDestroy<CFSMailMessagePart>::PushL(parts);

    NmMessagePart *nmPart = NULL;
    if(parts.Count()){
        for(int i=0; i<parts.Count(); i++){
            nmPart = parts[i]->GetNmMessagePart();
            nmParent->addChildPart(nmPart);
            childrenToNmMessagePartL(parts[i], nmPart);
        }
    }
    CleanupStack::PopAndDestroy(1, &parts);
}

/*!
   Leaving Refresh function
 */
int NmFrameworkAdapter::RefreshMailboxL(NmId mailboxId)
{
    NM_FUNCTION;

    int result(KErrNotFound);
    CFSMailBox *currentMailbox(NULL);
    currentMailbox = mFSfw->GetMailBoxByUidL(mailboxId);
    if(currentMailbox) {
        CleanupStack::PushL(currentMailbox);
        result = currentMailbox->RefreshNowL();
        CleanupStack::PopAndDestroy(currentMailbox);
    }
    return result;
}

/*!
   Leaving Go Online function
 */
int NmFrameworkAdapter::GoOnlineL(const NmId& mailboxId)
{
    NM_FUNCTION;

    int result(KErrNotFound);
    CFSMailBox *currentMailbox(NULL);
    currentMailbox = mFSfw->GetMailBoxByUidL(mailboxId);
    if(currentMailbox) {
        CleanupStack::PushL(currentMailbox);
        currentMailbox->GoOnlineL();
        CleanupStack::PopAndDestroy(currentMailbox);
    }
    return result;
}

/*!
   Leaving Go Offline function
 */
int NmFrameworkAdapter::GoOfflineL(const NmId& mailboxId)
{
    NM_FUNCTION;

    int result(KErrNotFound);
    CFSMailBox *currentMailbox(NULL);
    currentMailbox = mFSfw->GetMailBoxByUidL(mailboxId);
    if(currentMailbox) {
        CleanupStack::PushL(currentMailbox);
        currentMailbox->GoOfflineL();
        CleanupStack::PopAndDestroy(currentMailbox);
    }
    return result;
}

/*!
   handles mailbox related events
 */
void NmFrameworkAdapter::handleMailboxEvent(TFSMailMsgId mailbox, NmMailboxEvent event)
{
    NM_FUNCTION;

    QList<NmId> mailboxIds;
    NmId nmId = mailbox.GetNmId();
    mailboxIds.append(nmId);
    emit mailboxEvent(event, mailboxIds);
    }

/*!
   handles new, changed, deleted events
 */
void NmFrameworkAdapter::handleMessageEvent(
    TAny* param1,
    TAny* param2,
    NmMessageEvent event,
    TFSMailMsgId mailbox)
{
    NM_FUNCTION;

    NmId nmMsgId(0);
    QList<NmId> messageIds;
    RArray<TFSMailMsgId>* newFsEntries = reinterpret_cast<RArray<TFSMailMsgId>*> (param1);
    TFSMailMsgId* fsFolderId = reinterpret_cast<TFSMailMsgId*> (param2);
    NmId folderId = fsFolderId->GetNmId();

    TFSMailMsgId fsMsgId;
    for(TInt i = 0; i < newFsEntries->Count(); i++){
        fsMsgId = (*newFsEntries)[i];
        nmMsgId = fsMsgId.GetNmId();
        messageIds.append(nmMsgId);
    }
    emit messageEvent(event, folderId, messageIds, mailbox.GetNmId());
}

/*!
   function to handle mailmoved event
 */
void NmFrameworkAdapter::handleMailMoved(TAny* param1,TAny* param2,TAny* param3, TFSMailMsgId mailbox)
{
    NM_FUNCTION;

    NmId nmMsgId(0);
    QList<NmId> messageIds;
    RArray<TFSMailMsgId>* newFsEntries = reinterpret_cast<RArray<TFSMailMsgId>*> (param1);
    TFSMailMsgId* fsFromFolderId = reinterpret_cast<TFSMailMsgId*> (param3);
    NmId fromFolderId = fsFromFolderId->GetNmId();
    TFSMailMsgId* fsToFolderId = reinterpret_cast<TFSMailMsgId*> (param2);
    NmId toFolderId = fsToFolderId->GetNmId();

    TFSMailMsgId fsMsgId;
    for(TInt i = 0; i < newFsEntries->Count(); i++){
        fsMsgId = (*newFsEntries)[i];
        nmMsgId = fsMsgId.GetNmId();
        messageIds.append(nmMsgId);
    }
    // Yes, this is supposed to emit two signals from single incoming
    // event. Design decison was to create separate signals.
    emit messageEvent(NmMessageDeleted, fromFolderId, messageIds, mailbox.GetNmId());
    emit messageEvent(NmMessageCreated, toFolderId, messageIds, mailbox.GetNmId());
}

/*!
   function to handle mailcopied event
 */
void NmFrameworkAdapter::handleMailCopied(TAny* param1,TAny* param2, TFSMailMsgId mailbox)
{
    NM_FUNCTION;

    NmId nmMsgId(0);
    QList<NmId> messageIds;
    RArray<TFSMailMsgId>* newFsEntries = reinterpret_cast<RArray<TFSMailMsgId>*> (param1);
    TFSMailMsgId* fsToFolderId = reinterpret_cast<TFSMailMsgId*> (param2);
    NmId toFolderId = fsToFolderId->GetNmId();

    TFSMailMsgId fsMsgId;
    for(TInt i = 0; i < newFsEntries->Count(); i++){
        fsMsgId = (*newFsEntries)[i];
        nmMsgId = fsMsgId.GetNmId();
        messageIds.append(nmMsgId);
    }
    // Not interested in param3 (aOldParentFolder)
    emit messageEvent(NmMessageCreated, toFolderId, messageIds, mailbox.GetNmId());
}

void NmFrameworkAdapter::handleSyncstateEvent(TAny* param1, TFSMailMsgId mailbox)
{
    NM_FUNCTION;

    TSSMailSyncState* state = static_cast<TSSMailSyncState*>( param1 );
    NmOperationCompletionEvent event;
    event.mMailboxId = NmConverter::mailMsgIdToNmId(mailbox);
    event.mOperationType = Synch;

    switch(*state)
        {
        case StartingSync:
            {
            NM_TIMESTAMP("Sync started.");
            event.mCompletionCode = NmNoError;
            emit syncStateEvent(Synchronizing, event);
            break;
            }
        case FinishedSuccessfully:
            {
            NM_TIMESTAMP("Sync completed successfully.");
            event.mCompletionCode = NmNoError;
            emit syncStateEvent(SyncComplete, event);
            break;
            }
        case PasswordNotVerified:
            {
            event.mCompletionCode = NmAuthenticationError;
            emit syncStateEvent(SyncComplete, event);
            break;
            }
        case SyncCancelled:
            {
            event.mCompletionCode = NmCancelError;
            emit syncStateEvent(SyncComplete, event);
            break;
            }
        case ServerConnectionError:
            {
            event.mCompletionCode = NmServerConnectionError;
            emit syncStateEvent(SyncComplete, event);
            break;
            }
        case SyncError:
            {
            event.mCompletionCode = NmConnectionError;
            emit syncStateEvent(SyncComplete, event);
            break;
            }
        default:
            // ignore other statuses
            break;
        };
}

/*!
   function to handle folder event
 */
void NmFrameworkAdapter::handleFoldersEvent(
    TAny* param1, TAny* param2, NmFolderEvent event, TFSMailMsgId mailbox)
{
    NM_FUNCTION;
    
    Q_UNUSED(param2);

    // aParam1: RArray<TFSMailMsgId>* aEntries
    // aParam2: TFSMailMsgId* aParentFolder
    // aParam3: NULL

    NmId mailboxId(0);
    mailboxId = mailbox.GetNmId();

    NmId nmMsgId(0);
    QList<NmId> folderIds;

    resetCache(mailboxId);
    
    RArray<TFSMailMsgId>* fsEntries = reinterpret_cast<RArray<TFSMailMsgId>*> (param1);
    NmId folderId(0);

    TFSMailMsgId fsMsgId;
    for(TInt i = 0; i < fsEntries->Count(); i++){
        fsMsgId = (*fsEntries)[i];
        folderId = fsMsgId.GetNmId();
        folderIds.append(folderId);
    }

    emit folderEvent(event, folderIds, mailboxId);
}

Q_EXPORT_PLUGIN(NmFrameworkAdapter)

