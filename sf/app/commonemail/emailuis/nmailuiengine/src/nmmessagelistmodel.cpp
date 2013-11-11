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

static const int NmFolderTypeRole = Qt::UserRole+1; 

/*!
    \class NmMessageListModel
    \brief The NmMessageListModel class represents data model for mailbox list.
    @alpha

    The NmMessageListModel class uses NmMessageEnvelope class to represent data returned in its'
    data method to get all information needed for one list row for a widget by calling the method
    once.
*/

/*!
	Class constructor.
*/
NmMessageListModel::NmMessageListModel(NmDataManager &dataManager,
                                       QObject *parent /* = 0 */)
: QStandardItemModel(parent),
  mDataManager(dataManager),
  mDividersActive(false),
  mParentPtr(NULL),
  mCurrentFolderType(NmFolderInbox),
  mIgnoreFolderIds(false)
{
    NM_FUNCTION;
    
    // Check for setting whether dividers are active
    // mDividersActive = ...
    // update also the test cases
}


/*!
	Class destructor.
*/
NmMessageListModel::~NmMessageListModel()
{
    NM_FUNCTION;
    
    clear();
}


/*!
    Returns data specified by \a index. Only Qt::DisplayRole is supported in \a role.
    The refresh method must have been called before this method can return any real data.
*/
QVariant NmMessageListModel::data(const QModelIndex &index, int role) const
{
    NM_FUNCTION;
    
    QVariant qVariant;

    if (index.isValid()){
        if (role == Qt::DisplayRole) {
            NmMessageListModelItem *item =
                static_cast<NmMessageListModelItem*>(itemFromIndex(index));
            qVariant = QVariant::fromValue(item);
        }
        else if (role == NmFolderTypeRole) {
            qVariant = QVariant(mCurrentFolderType);
        }    
    }

    return qVariant;
}


/*!
    This refreshes the data of the model.

    \param mailboxId The ID of the mailbox.
    \param folderId The ID of the folder.
    \param messageEnvelopeList A list containing the message meta data.
*/
void NmMessageListModel::refresh(
        const NmId mailboxId, 
        const NmId folderId,
        const QList<NmMessageEnvelope*> &messageEnvelopeList)
{
    NM_FUNCTION;

    // Store the current mailbox and folder IDs.
    mCurrentMailboxId = mailboxId;
    mCurrentFolderId = folderId;

    // Store the type of the currently displayed folder.
    mCurrentFolderType = mDataManager.folderTypeById(mailboxId, folderId);

    // Clear the model.
    clear();

    // Add the items from the given list.
    NmMessageEnvelope* insertedMessage(NULL);
    int parentCount(0);
    int childCount(0);

    for (int i(0); i < messageEnvelopeList.count(); i++) {
        NmMessageEnvelope *nextMessage = messageEnvelopeList[i];
        // imap and pop is using common sent, outbox or draft folder
        // for all mailboxes, here we want to filter out messages that
        // are not under this mailbox
        if (nextMessage) {
            bool insert(true);
            if (NmFolderSent == mCurrentFolderType
                || NmFolderOutbox == mCurrentFolderType
                || NmFolderDrafts == mCurrentFolderType) {
                insert = (mCurrentMailboxId == nextMessage->mailboxId());  
            }
            if (insert) {
                if (mDividersActive &&
                    !messagesBelongUnderSameDivider(insertedMessage, nextMessage)) {
                    insertDividerIntoModel(nextMessage, parentCount);
                    parentCount++;
                    childCount = 0;
                }
            insertMessageIntoModel(nextMessage, childCount, false);
            insertedMessage = nextMessage;
            childCount++;
            }
        }
    }
}


/*!
    insertDividerIntoModel. Function inserts divider into model.
*/
void NmMessageListModel::insertDividerIntoModel(
    NmMessageEnvelope *messageForDivider,
    int parentRow)
{
    NM_FUNCTION;
    
    mParentPtr = createTitleDividerItem(messageForDivider);
    insertRow(parentRow,mParentPtr);
    mParentPtr->callEmitDataChanged();
}

/*!
    Function inserts message into model. Message can be inserted
    either to root or to parent. If parent is zero, item is added into root.
*/
void NmMessageListModel::insertMessageIntoModel(
		NmMessageEnvelope *messageEnvelope, int childRow, bool emitSignal)
{
    NM_FUNCTION;
    
    NmMessageListModelItem *mailItem = createMessageItem(messageEnvelope);
    if (mParentPtr) {
        // Add under parent divider
        mParentPtr->insertRow(childRow, mailItem);
    }
    else {
        // No dividers, add under root item
        insertRow(childRow,mailItem);
    }
    if (emitSignal) {
        mailItem->callEmitDataChanged();
    }
}

/*!
    Function checks whether the messages can be drawn under same title divider
    Check is done depending of the current sorting criteria
*/
bool NmMessageListModel::messagesBelongUnderSameDivider(
    const NmMessageEnvelope *message1,
    const NmMessageEnvelope *message2) const
{
    NM_FUNCTION;
    
    bool retVal(false);
    // First check pointer validity
    if (message1 && message2) {
        // Switch here for other sort modes 
        // Date based comparison
        QDateTime sentTime1 = message1->sentTime().toLocalTime();
        QDateTime sentTime2 = message2->sentTime().toLocalTime();
        if ( sentTime1.date() == sentTime2.date()) {
            retVal = true;
        }
    }
    return retVal;
}


/*!
    Handles the message events.

    \param event The type of the message event.
    \param folderId The folder containing the message.
    \param messageIds A list of message IDs related to the event.
*/
void NmMessageListModel::handleMessageEvent(NmMessageEvent event,
                                            const NmId &folderId,
                                            const QList<NmId> &messageIds,
                                            const NmId &mailboxId)
{
    NM_FUNCTION;
    const int idCount = messageIds.count();
    NmId inFolderId = mDataManager.getStandardFolderId(mailboxId, NmFolderInbox);    

    // Folder ID does not concern us if this model instance is used for e.g.
    // searching messages.
    if (!mIgnoreFolderIds) {
        if (folderId == 0) {
            // Const cast is used here because also the input parameter has to
            // be changed.
            const_cast<NmId&>(folderId) = inFolderId;
            NmUiStartParam *startParam =
                new NmUiStartParam(NmUiViewMessageList, mailboxId, folderId);
            emit setNewParam(startParam);
        }

        if (mCurrentFolderId == 0 && folderId == inFolderId) {
            // Folder ID was not known at time when the mailbox opened and we
            // know that because of events the subscription is valid only for
            // the current mailbox.
            mCurrentFolderId = folderId; 
        }
        // MailboxId checking here is done because we want to filter out messages
        // that belongs to other mailboxes in case of imap/pop sent, outbox or draft folder
        if (mCurrentFolderId != folderId || mCurrentMailboxId != mailboxId) {
            // The event does not concern the folder currently being displayed.
            // Thus, ignore it.
            return;
        }
    }

    // Go through the given message IDs and do the appropriate operations
    // according to the type of the event.
    for (int i(0); i < idCount; ++i) {
        switch (event) {
            case NmMessageChanged: {
                updateMessageEnvelope(mailboxId, folderId, messageIds[i]);
                break;
            }
            case NmMessageCreated: {
                // mIgnoreFolderIds is true if (and only if) this model is used
                // for mail search purposes and thus, we do not want the model
                // to handle "message created" events. Issue to consider:
                // renaming mIgonreFolderIds => mModelUsedForSearch or something
                // similar.
                if (!mIgnoreFolderIds && !itemFromModel(messageIds[i])) {
                    insertNewMessageIntoModel(mailboxId, folderId, messageIds[i]);
                }
                
                break;
            }
            case NmMessageFound: {
                if (!itemFromModel(messageIds[i])) {
                    insertNewMessageIntoModel(mailboxId, folderId, messageIds[i]);
                }

                break;
            }
            case NmMessageDeleted: {
                removeMessageFromModel(messageIds[i]);
                break;
            }
        }
    }
}

/*!
    Handles the folder deleted events.

    \param folderId The folder deleted
    \param mailboxId A related mailbox
*/
void NmMessageListModel::handleFolderDeletedEvent(
                        const NmId &folderId,
                        const NmId &mailboxId)
{
    NM_FUNCTION;

    // Folder ID does not concern us if this model instance is used for e.g.
    // searching messages.
    if (!mIgnoreFolderIds 
    	  && mCurrentMailboxId == mailboxId 
    	  && mCurrentFolderId == folderId) {
        clear();
        mCurrentFolderId = 0;        
        // Go to inbox, but set folderId as 0 to indicate out-of-sync-state.
        NmUiStartParam *startParam =
            new NmUiStartParam(NmUiViewMessageList, mailboxId, mCurrentFolderId);
        emit setNewParam(startParam);
    }
}


/*!
    Function inserts new message into correct position to model.
    A new title divider is created if it is needed.
*/
void NmMessageListModel::insertNewMessageIntoModel(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &msgId)
{
    NM_FUNCTION;
    
    // envelope ownership is here
    NmMessageEnvelope *newMsgEnve = mDataManager.envelopeById(mailboxId, folderId, msgId);
    if (newMsgEnve) {
        int rowIndex(getInsertionIndex(*newMsgEnve));
        if (!mDividersActive) {
            if (rowIndex > -1){
                insertMessageIntoModel(newMsgEnve, rowIndex, true);
            } else {
                NmMessageListModelItem *mailItem = createMessageItem(newMsgEnve);
                appendRow(mailItem);
                mailItem->callEmitDataChanged();
            }
        } else {
            // model contain items, message goes to the middle of the list
            if (rowIndex > 0) {
                int dividerIndex(0);
                QList<QStandardItem*> items = findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
                QModelIndex index = items[rowIndex]->index();
                NmMessageListModelItem *item = static_cast<NmMessageListModelItem*>(itemFromIndex(index));
                NmMessageEnvelope *replacedEnve = item->envelopePtr();
                if (messagesBelongUnderSameDivider(newMsgEnve,replacedEnve)) {
                    // find the index of the divider and insert message as child object
                    // dividerIndex is not actually used here but function sets
                    // the correct parent pointer
                    dividerIndex = dividerInsertionIndex(rowIndex);
                    insertMessageIntoModel(newMsgEnve, 0, true);
                } else {
                    // create new divider and find the correct index for it
                    dividerIndex = dividerInsertionIndex(rowIndex);
                    insertDividerIntoModel(newMsgEnve, dividerIndex);
                    // Insert message as a first child under new divider
                    insertMessageIntoModel(newMsgEnve, 0, true);
                }
                items.clear();
            }
            // first mail item in the model,
            // insert new divider to index 0 and new message as a child.
            else if (0 == rowIndex) {
                insertDividerIntoModel(newMsgEnve, rowIndex);
                insertMessageIntoModel(newMsgEnve, 0, true);
            }
            // this will go to the last item of the list and create new title divider
            else {
                mParentPtr = createTitleDividerItem(newMsgEnve);
                appendRow(mParentPtr);
                mParentPtr->callEmitDataChanged();
                NmMessageListModelItem *mailItem = createMessageItem(newMsgEnve);
                // Add under parent divider
                mParentPtr->appendRow(mailItem);
                mailItem->callEmitDataChanged();
            }
        }
    }
    delete newMsgEnve;
}

/*!
    Function check model index in which the new message should be inserted
    with the currently active sort mode.
*/
int NmMessageListModel::getInsertionIndex(
    const NmMessageEnvelope &envelope) const
{
    NM_FUNCTION;
    
    int ret(NmNotFoundError);
    
    // Date+descending sort mode based comparison.
    // Go through model and compare sent times with QDateTime >= comparison operation.
    QList<QStandardItem*> items = findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
    int count(items.count());
    bool found(false);
    int i(0);
    while (i < count && !found) {
        QModelIndex index = items[i]->index();
        NmMessageListModelItem *item = static_cast<NmMessageListModelItem*>(itemFromIndex(index));
        if (NmMessageListModelItem::NmMessageItemMessage == item->itemType()) {
        	found = envelope.sentTime() >= item->envelope().sentTime();
        	if (found) {
                ret = i;
        	}
        }
        i++;
    }
    if (0 == count) {
        ret = NmNoError;
    }
    items.clear();
    return ret;
}

/*!
    Function finds preceding title divider index and sets the
    mParentPtr variable.
*/
int NmMessageListModel::dividerInsertionIndex(int messageIndex)
{
    NM_FUNCTION;
    
    bool found(false);
    int ret(NmNoError);
    QModelIndex index;
    QList<QStandardItem*> items = findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
    for (int i = messageIndex-1; i >= 0 && !found; i--) {
        index = items[i]->index();
        NmMessageListModelItem *item = static_cast<NmMessageListModelItem*>(itemFromIndex(index));
        if(item->itemType() == NmMessageListModelItem::NmMessageItemTitleDivider) {
            found = true;
            mParentPtr = static_cast<NmMessageListModelItem*>(itemFromIndex(index));
            ret = index.row();
        }
    }
    items.clear();
    return ret;
}

/*!
    Create title divider item
*/
NmMessageListModelItem *NmMessageListModel::createTitleDividerItem(
		NmMessageEnvelope *messageForDivider)
{
    NM_FUNCTION;
    
    NmMessageListModelItem *item = new NmMessageListModelItem();
    item->setItemType(NmMessageListModelItem::NmMessageItemTitleDivider);

    QDate sentLocalDate = messageForDivider->sentTime().toLocalTime().date();
    QDate currentdate = QDate::currentDate();

    if (sentLocalDate == currentdate) {
        // NOTE: when dividers are taken in to use hbTrId macro should used here,
        // to be clarified how to access ts-file (located under ui component)
        item->setTitleDivider(tr("Today", "txt_nmailuiengine_divider_today"));
        item->setText(tr("Today", "txt_nmailuiengine_divider_today"));
    }
    else {
        QLocale locale;
        QString divider = locale.toString(sentLocalDate, QLocale::LongFormat);
        item->setTitleDivider(divider);
        item->setText(divider);
        }
    item->setEnvelope(*messageForDivider);
    item->setData(Hb::ParentItem, Hb::ItemTypeRole);
    return item;
}

/*!
    Create message item
*/
NmMessageListModelItem *NmMessageListModel::createMessageItem(
		NmMessageEnvelope *envelope)
{
    NM_FUNCTION;
    
    NmMessageListModelItem *mailItem = new NmMessageListModelItem();
    mailItem->setEnvelope(*envelope);
    mailItem->setItemType(NmMessageListModelItem::NmMessageItemMessage);
    mailItem->setData(Hb::StandardItem, Hb::ItemTypeRole);
    return mailItem;
}

/*!
    Returns divider state
*/
bool NmMessageListModel::dividersActive()
{
    NM_FUNCTION;
    
    return mDividersActive;
}

/*!
    Set divider state
*/
void NmMessageListModel::setDividers(bool active)
{
    NM_FUNCTION;
    
    mDividersActive = active;
}

/*!
   Change item property if differs
*/
void NmMessageListModel::setEnvelopeProperties(
    NmEnvelopeProperties property,
    const QList<NmId> &messageIds)
{
    NM_FUNCTION;
    
    for (int i(0); i < messageIds.count(); i++) {
        updateEnvelope(property, messageIds[i]);
    }
}


/*!
    Returns the ID of the current mailbox.
*/
NmId NmMessageListModel::currentMailboxId()
{
    NM_FUNCTION;
    
    return mCurrentMailboxId;
}


/*!
    Sets whether the model should ignore the folder IDs or not. The folder IDs
    should be ignored e.g. when the model is used for searching messages
    (i.e. used by the search view).

    \param ignore If true, will ignore the folder IDs.
*/
void NmMessageListModel::setIgnoreFolderIds(bool ignore)
{
    NM_FUNCTION;
    
    mIgnoreFolderIds = ignore;
}


/*!
   Remove message from model if message exists in model
*/
void NmMessageListModel::removeMessageFromModel(const NmId &msgId)
{
    NM_FUNCTION;
    
    QList<QStandardItem*> items = findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
    int count(items.count());
    bool found(false);
    int i(0);
    while (!found && i < count) {
        QModelIndex index = items[i]->index();
        NmMessageListModelItem *item = static_cast<NmMessageListModelItem*>(itemFromIndex(index));
        if (NmMessageListModelItem::NmMessageItemMessage == item->itemType()
                && msgId == item->envelope().messageId()) {
            found = true;
            // dividers are not active, just remove message
            if (!mDividersActive) {
                removeItem(index.row(), *item);
            } else {
                // check the type from previous item
                QModelIndex prevIndex = items[i-1]->index();
                NmMessageListModelItem* previous = static_cast<NmMessageListModelItem*>(itemFromIndex(prevIndex));
                if (NmMessageListModelItem::NmMessageItemTitleDivider == previous->itemType()) {
                    // previous item is title divider
                    if (i < (count-1)) {
                        // if this is not last item in the list, check next item
                        QModelIndex nextIndex = items[i+1]->index();
                        NmMessageListModelItem* next = static_cast<NmMessageListModelItem*>(itemFromIndex(nextIndex));
                        if (NmMessageListModelItem::NmMessageItemMessage == next->itemType()) {
                            // next item is message, divider should not be removed
                            removeItem(index.row(), *item);
                        } else {
                            // next item is another divider, remove also divider
                            removeItem(index.row(), *item);
                            removeItem(prevIndex.row(), *previous);
                        }
                    } else {
                        // this is the last message in list, remove also divider
                        removeItem(index.row(), *item);
                        removeItem(prevIndex.row(), *previous);
                    }
                }  else {
                    // previous item is message, title divider should not be deleted
                    removeItem(index.row(), *item);
                }
            }
        }
        i++;
    }
    items.clear();
}

/*!
   Helper function to remove row
*/
void NmMessageListModel::removeItem(int row, NmMessageListModelItem &item)
{
    NM_FUNCTION;
    
    QStandardItem *parent = item.parent();
    removeRow(row, indexFromItem(parent));
}

/*!
   Search item from model
*/
NmMessageListModelItem *NmMessageListModel::itemFromModel(const NmId &messageId)
{
    NM_FUNCTION;
    
    QList<QStandardItem*> items = findItems("*", Qt::MatchWildcard | Qt::MatchRecursive);
    int count(items.count());
    bool found(false);
    int i(0);
    NmMessageListModelItem *ret(NULL);
    while (i < count && !found) {
        NmMessageListModelItem *item =
            static_cast<NmMessageListModelItem*>(itemFromIndex(items[i]->index()));
        if (NmMessageListModelItem::NmMessageItemMessage == item->itemType()
            && messageId == item->envelope().messageId()) {
            found = true;
            ret = item;
        }
        i++;
    }
    return ret;
}

/*!
   Helper function for envelope comparison
*/
bool NmMessageListModel::changed(const NmMessageEnvelope &first, const NmMessageEnvelope &second)
{
    NM_FUNCTION;
    
    return first != second;
}

/*!
   Updates envelope if something is changed
*/
void NmMessageListModel::updateMessageEnvelope(const NmId &mailboxId,
        const NmId &folderId,
        const NmId &msgId)
{
    NM_FUNCTION;
    
    NmMessageListModelItem *item = itemFromModel(msgId);
    // envelope ownership is here
    NmMessageEnvelope *newEnvelope = mDataManager.envelopeById(mailboxId, folderId, msgId);
    if (item && newEnvelope) {
        if (changed(*item->envelopePtr(), *newEnvelope)) {
            // function takes envelope ownership 
            item->setEnvelope(newEnvelope);
        } else {
            delete newEnvelope;
            newEnvelope = NULL;
        }
    }
}

/*!
   Update envelope property
*/
void NmMessageListModel::updateEnvelope(NmEnvelopeProperties property, const NmId &msgId)
{
    NM_FUNCTION;
    
    NmMessageListModelItem *item = itemFromModel(msgId);
    if (item) {
        bool changed(false);
        NmMessageEnvelope *envelope = item->envelopePtr();
        switch (property)
        {
        case MarkAsRead:
        {
            if (!envelope->isRead()) {
                envelope->setRead(true);
                changed = true;
            }
            break;
        }
        case MarkAsUnread:
        {
            if (envelope->isRead()) {
                envelope->setRead(false);
                changed = true;
            }
            break;
        }
        default:
            break;
        }
        if (changed) {
            item->callEmitDataChanged();
        }
    }
}
