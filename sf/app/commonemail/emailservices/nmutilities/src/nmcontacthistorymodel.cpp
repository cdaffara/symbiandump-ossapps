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
* Description: Public implementation of Contact History Model API
*
*/

#include <QList>

#include "emailtrace.h"
#include "nmcontacthistorymodel.h"
#include "nmcontacthistorymodel_p.h"

// --------------------------------------------------------------------------
// Start of implementation (NmContactHistoryModel)
// --------------------------------------------------------------------------
/*!
    Constructor for NmContactHistoryModel

    \param modelType ModelType to be used, Email / Messaging
*/
NmContactHistoryModel::NmContactHistoryModel(
    const NmContactHistoryModelType modelType)
{
    NM_FUNCTION;
    d_ptr = new NmContactHistoryModelPrivate(modelType);

    QObject::connect(d_ptr, SIGNAL(queryCompleted(int)), this,
        SLOT(handleQueryCompleted(int)));
}

/*!
    Destructor of NmContactHistoryModel
 */
NmContactHistoryModel::~NmContactHistoryModel()
{
    NM_FUNCTION;
    delete d_ptr;
}

/*!
    Queries contact databases from private implementation

    \param query String to be queried.
*/
void NmContactHistoryModel::query(const QString &query)
{
    NM_FUNCTION;
    d_ptr->queryDatabases(query);
}

/*!
     Signaled once query is completed, emits modelCompleted.
     Private Slot.

     \param err, 0 if building model was success
*/
void NmContactHistoryModel::handleQueryCompleted(int err)
{
    NM_FUNCTION;

    int lastUpdateIndex = (d_ptr->privateDataCount())-1;

    if (lastUpdateIndex != -1)
    {
        // Notify views that we are about to change model data.
        beginInsertRows(QModelIndex(),0,lastUpdateIndex);
        d_ptr->refreshDataModel();
        endInsertRows();

        // Emit dataChanged();
        bool validIndex = hasIndex ( lastUpdateIndex, 0 );

        if (validIndex)
        {
            emit dataChanged(index(0,0), index(lastUpdateIndex,0));
        }
    }

    emit modelCompleted(err);
}

/*!
    Returns the number of rows under the given parent.

    From QAbstractItemModel
*/
int NmContactHistoryModel::rowCount(const QModelIndex &parent) const
{
    NM_FUNCTION;
    return d_ptr->modelRowCount(parent);
}

/*!
    Returns the data stored under the given role for the item referred
    to by the index.

    From QAbstractItemModel
*/
QVariant NmContactHistoryModel::data(const QModelIndex &index, int role) const
{
    NM_FUNCTION;
    return d_ptr->data(index, role);
}

// --------------------------------------------------------------------------
// End of implementation (NmContactHistoryModel)
// --------------------------------------------------------------------------

