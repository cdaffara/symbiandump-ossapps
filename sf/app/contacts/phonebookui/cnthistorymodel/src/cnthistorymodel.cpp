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

#include "cnthistorymodel_p.h"
#include "cnthistorymodel.h"
#include "cntdebug.h"

/*!
 * Construct a new CntHistoryModel object to communicate 
 * with the conversations and logs databases.
 *
 * \param contactId History specific to this contact is cached. 
 * If no contact is specified all the call logs and conversation 
 * history from all contacts will be cached.
 */
CntHistoryModel::CntHistoryModel(QContactLocalId contactId,
                                 QContactManager* manager,
                                 QObject *parent)
    : QAbstractListModel(parent),
      d_ptr(new CntHistoryModelPrivate(contactId, manager))
{
    CNT_ENTRY
    
    Q_D(CntHistoryModel);
    d->q_ptr = this;
    
    CNT_EXIT
}

CntHistoryModel::~CntHistoryModel()
{
    CNT_ENTRY
    
    Q_D(CntHistoryModel);
    delete d;
    
    CNT_EXIT
}

/*!
 * Return the data to be used by the view or delegates for a particular
 * item and role.
 *
 * \param index The index of the item to return data about.
 * \param role The data should be relevant to this particular purpose.
 * \return QVariant The data for the specified index and role.
 */
QVariant CntHistoryModel::data(const QModelIndex& index, int role) const
{
    Q_D(const CntHistoryModel);
    return d->data(index, role);
}

/*!
 * Get the number of rows (conversations) in this model.
 *
 * \param parent Optional parent index value.
 * \return Number of rows in this model.
 */
int CntHistoryModel::rowCount(const QModelIndex& parent) const
{
    Q_D(const CntHistoryModel);
    return d->rowCount(parent);
}

/*!
 * Clear history from the database. If the history cached
 * is specific to one contact, only that history is cleared.
 * 
 */
void CntHistoryModel::clearHistory()
{
    CNT_ENTRY
    
    Q_D(CntHistoryModel);
    d->clearHistory();
    
    CNT_EXIT
}

/*!
 * Mark all the conversations in the view as seen.
 * 
 */
void CntHistoryModel::markAllAsSeen()
{
    CNT_ENTRY
    
    Q_D(CntHistoryModel);
    d->markAllAsSeen();
    
    CNT_EXIT
}

/*!
 * Sort items in the model and refresh the view
 *
 * \param order Order to sort the list items.
 */
void CntHistoryModel::sortAndRefresh(Qt::SortOrder order)
{
    CNT_ENTRY_ARGS(order)
    
    Q_D(CntHistoryModel);
    
    doBeginResetModel();
    d->sort(order);
    doEndResetModel();
    
    CNT_EXIT
}

void CntHistoryModel::doBeginInsertRows(const QModelIndex &parent, int first, int last)
{
    CNT_ENTRY_ARGS(parent << first << last)
    
    beginInsertRows(parent, first, last);
    
    CNT_EXIT
}

void CntHistoryModel::doEndInsertRows()
{
    CNT_ENTRY
    
    endInsertRows();
    
    CNT_EXIT
}

void CntHistoryModel::doBeginRemoveRows(const QModelIndex &parent, int first, int last)
{
    CNT_ENTRY_ARGS(parent << first << last)
    
    beginRemoveRows(parent, first, last);
    
    CNT_EXIT
}

void CntHistoryModel::doEndRemoveRows()
{
    CNT_ENTRY
    
    endRemoveRows();
    
    CNT_EXIT
}

void CntHistoryModel::doBeginResetModel()
{
    CNT_ENTRY
    
    beginResetModel();
    
    CNT_EXIT
}

void CntHistoryModel::doEndResetModel()
{
    CNT_ENTRY
    
    endResetModel();
    
    CNT_EXIT
}

void CntHistoryModel::doDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    CNT_ENTRY_ARGS(topLeft << bottomRight)
    
    emit dataChanged(topLeft, bottomRight);
    
    CNT_EXIT
}
