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

#include "nmuiengineheaders.h"

/*!
    \class NmMailboxListModel
    \brief The NmMailboxListModel class represents data model for mailbox list.
    @alpha

    The NmMailboxListModel class uses NmMailboxMetaData class to represent data returned in its'
    data method to get all information needed for one list row for a widget by calling the method
    once.
*/

/*!
    Constructor
 */
NmMailboxListModel::NmMailboxListModel(NmDataManager &dataManager, QObject *parent)
:QStandardItemModel(parent),
mDataManager(dataManager)
{
    NM_FUNCTION;
}

/*!
	Destructor
 */
NmMailboxListModel::~NmMailboxListModel()
{
    NM_FUNCTION;
    
    clear();
}

/*!
    Returns data specified by \a index. Only Qt::DisplayRole is supported in \a role.
    The refresh method must have been called before this method can return any real data.
 */
QVariant NmMailboxListModel::data(const QModelIndex &index, int role) const
{
    NM_FUNCTION;
    
    QVariant qVariant;
    if (index.isValid() && Qt::DisplayRole == role) {
        NmMailboxListModelItem *item = static_cast<NmMailboxListModelItem*>(itemFromIndex(index));
        NmMailboxMetaData *mailbox = item->itemMetaData();
        qVariant = QVariant::fromValue(mailbox);
    }
    return qVariant;
}

/*!
    This refreshes the data of the model.
 */
void NmMailboxListModel::refresh(
        QList<NmMailbox*> &mailboxList)
{
    NM_FUNCTION;
    
    clear();
    for (int i(0); i < mailboxList.count(); i++) {
        NmMailbox *mailbox = mailboxList[i];
        if (mailbox) {
            NmMailboxListModelItem *item = createMailboxItem(mailbox);
            appendRow(item);
        }
    }
}

/*!
    Updates specific model item.
    \param mailboxId Mailbox id.
 */
void NmMailboxListModel::refreshModelItem(const NmId &mailboxId)
{
    NM_FUNCTION;
    
    // Get correct mailbox data.
    NmMailbox* mailbox = mDataManager.mailbox(mailboxId);
    NmMailboxListModelItem *entryItem(NULL);

    // Get correct entry item.
    const int itemCount(rowCount());
    for (int itemIndex(0); itemIndex < itemCount; ++itemIndex) {
        entryItem = static_cast<NmMailboxListModelItem *>(itemFromIndex(index(itemIndex,0)));
        if (entryItem->itemMetaData()->id() == mailboxId) {
            break;
        }
        entryItem = NULL;
    }

    // Update entry item's data.
    if (mailbox && entryItem) {
        // Changes data and emits datachanged.
        NmMailboxMetaData *metaData = new NmMailboxMetaData;
        metaData->setId(mailbox->id());
        metaData->setName(mailbox->name());
        metaData->setAddress(mailbox->address().address());
        entryItem->setItemMetaData(metaData);
    }
}

/*!
    Function handles mailbox events such as created, renamed and deleted
 */
void NmMailboxListModel::handleMailboxEvent(NmMailboxEvent event, const QList<NmId> &mailboxIds)
{
    NM_FUNCTION;
    
    for (int a(0); a < mailboxIds.count(); a++) {
        NmId mailboxId = mailboxIds[a];
        switch (event) {
            case NmMailboxCreated: {
                NmMailbox* mailbox = mDataManager.mailbox(mailboxId);
                if (mailbox) {
                    NmMailboxListModelItem *item = createMailboxItem(mailbox);
                    appendRow(item);
                    item->callEmitDataChanged();
                }
                delete mailbox;
                mailbox = NULL;
                break;
            }
            case NmMailboxChanged: {
                refreshModelItem(mailboxId);
                break;
            }
            case NmMailboxDeleted: {
				int rowIndex(-1);
				NmMailboxListModelItem *entryItem(NULL);
				// Get correct entry item.
				const int itemCount(rowCount());
				for (int itemIndex(0); itemIndex < itemCount; ++itemIndex) {
					entryItem = static_cast<NmMailboxListModelItem *>(itemFromIndex(index(itemIndex,0)));
					if (entryItem && entryItem->itemMetaData()->id() == mailboxId) {
						rowIndex = itemIndex;
						break;
					}
				}
				if (rowIndex >= 0 && entryItem) {
					QStandardItem *parent = entryItem->parent();
					removeRow(rowIndex, indexFromItem(parent));
				}
                break;
            }
            default: {
                break;
            }
        }
    }
}

/*!
    Create mailbox item
    \param mailbox
 */
NmMailboxListModelItem *NmMailboxListModel::createMailboxItem(const NmMailbox *mailbox)
{
    NM_FUNCTION;
    
    NmMailboxMetaData *newMeta = new NmMailboxMetaData();
    newMeta->setId(mailbox->id());
    newMeta->setName(mailbox->name());
    newMeta->setAddress(mailbox->address().address());

    NmMailboxListModelItem *item = new NmMailboxListModelItem();
    item->setItemMetaData(newMeta);
    item->setData(Hb::StandardItem, Hb::ItemTypeRole);
    return item;
}
