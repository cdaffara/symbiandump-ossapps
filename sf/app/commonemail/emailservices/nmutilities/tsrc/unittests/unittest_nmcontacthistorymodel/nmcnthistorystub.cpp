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
* Description: Private implementation of Contact History Model API
*
*/

extern bool constructorCalled;
extern bool destructorCalled;
extern bool rowcountCalled;
extern bool dataCalled;
extern bool modelcompletedCalled;
extern bool queryCalled;
extern bool refreshdatamodelCalled;
extern bool handlequerycompletedCalled;

#include "nmcontacthistorymodel_p.h"

// --------------------------------------------------------------------------
// Start of implementation (NmContactHistoryModelPrivate)
// --------------------------------------------------------------------------
/*!
    Constructor of NmContactHistoryModelPrivate

    \param type The valid values are described by
    NmContactHistoryModelPrivate::HistoryModelType enum.
*/
NmContactHistoryModelPrivate::NmContactHistoryModelPrivate(
    const NmContactHistoryModelType modelType) :
    mType(modelType),
    mSignalConnected(false)
{
    constructorCalled = true;
    mPrivateItemList.append("DUMMYDATA");
}

/*!
    Destructor of ContactHistoryModel
*/
NmContactHistoryModelPrivate::~NmContactHistoryModelPrivate()
{
    destructorCalled = true;
}

/*!
    Searches contact & MRU  databases according to query parameters.
    Emits QueryCompleted() when ready.

    \param query Text to be searched
    \return none

    Note:: QContactManager supports max. ten characters long queries.
*/
void NmContactHistoryModelPrivate::queryDatabases(const QString& query)
{
    Q_UNUSED(query);
    queryCalled = true;
}

void NmContactHistoryModelPrivate::refreshDataModel()
{
    refreshdatamodelCalled = true;
}

/*!
    This is called by public class NmContactHistoryModel when rowcount() is requested
    Not meant to be used alone.

*/
int NmContactHistoryModelPrivate::modelRowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    rowcountCalled = true;
    return 0;
}

int NmContactHistoryModelPrivate::privateDataCount() const
{
    rowcountCalled = true;
    return mPrivateItemList.count();
}

/*!
    This is called by public class NmContactHistoryModel when data() is requested
    Not meant to be used alone.

*/
QVariant NmContactHistoryModelPrivate::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index);
    Q_UNUSED(role);

    dataCalled = true;
    return QVariant();
}

void NmContactHistoryModelPrivate::connectNotify ( const char * signal )
    {
    QString testSignal = signal;
    mSignalConnected = true;
    }

// --------------------------------------------------------------------------
// End of implementation (NmContactHistoryModelPrivate)
// --------------------------------------------------------------------------

NmContactHistoryModelItem::NmContactHistoryModelItem()
{
}

/*!
    Not meant to be used alone.
*/
NmContactHistoryModelItem::NmContactHistoryModelItem(
    const NmContactHistoryModelItem &other)
{
    Q_UNUSED(other);
}

/*!
    Not meant to be used alone.
*/
NmContactHistoryModelItem::~NmContactHistoryModelItem()
{
}

/*!
    Not meant to be used alone.
*/
unsigned int NmContactHistoryModelItem::contactId() const
{
    return 0;
}

/*!
    Not meant to be used alone.
*/
void NmContactHistoryModelItem::setContactId(unsigned int contactId)
{
    Q_UNUSED(contactId);
}

/*!
    Not meant to be used alone.
*/
int NmContactHistoryModelItem::subItemCount() const
{
    return 0;
}

/*!
    Not meant to be used alone.
*/
NmContactHistoryModelSubItem NmContactHistoryModelItem::subItemAt(
    int index) const
{
    Q_UNUSED(index);
    return NmContactHistoryModelSubItem();

}

/*!
    Returns SubEntries from private list
    First entry is contact name
    Second entry is email address / phone number

    \return List of items.
*/
QList<NmContactHistoryModelSubItem> NmContactHistoryModelItem::subEntries(
    ) const
{
    QList<NmContactHistoryModelSubItem> subItemList;
    return subItemList;
}

/*!
    Public - Appends subItem to private list

    \param entry subItem to be appended
*/
void NmContactHistoryModelItem::appendSubItem(
    NmContactHistoryModelSubItem entry)
{
    Q_UNUSED(entry);
}

