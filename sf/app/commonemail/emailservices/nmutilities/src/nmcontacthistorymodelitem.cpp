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
* Description: Implementation of NmContactHistoryModelItem
*
*/

#include <QList>

#include "nmcontacthistorymodel.h"
#include "nmcontacthistorymodel_p.h"

// --------------------------------------------------------------------------
// Start of implementation (NmContactHistoryModelItem)
// --------------------------------------------------------------------------
/*!
     Not meant to be used alone.
*/
NmContactHistoryModelItem::NmContactHistoryModelItem()
{
    d = new NmContactHistoryModelItemData();
}

/*!
    Not meant to be used alone.
*/
NmContactHistoryModelItem::NmContactHistoryModelItem(
    const NmContactHistoryModelItem &other)
{
    d = other.d;
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
    return d->mContactId;
}

/*!
    Not meant to be used alone.
*/
void NmContactHistoryModelItem::setContactId(unsigned int contactId)
{
    d->mContactId = contactId;
}

/*!
    Not meant to be used alone.
*/
int NmContactHistoryModelItem::subItemCount() const
{
    return d->mItems.count();
}

/*!
    Not meant to be used alone.
*/
NmContactHistoryModelSubItem NmContactHistoryModelItem::subItemAt(
    int index) const
{
    if ( index <= d->mItems.count() )
    {
        return d->mItems[index];
    }
    else
    {
        return NmContactHistoryModelSubItem();
    }
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

    if (!d->mItems.isEmpty())
    {
        for ( int subIndex = 0; subIndex < d->mItems.count(); subIndex++)
        {
            subItemList.append(d->mItems[subIndex]);
        }
    }

    return subItemList;
}

/*!
    Public - Appends subItem to private list

    \param entry subItem to be appended
*/
void NmContactHistoryModelItem::appendSubItem(
    NmContactHistoryModelSubItem entry)
{
    d->mItems.append(entry);
}
// --------------------------------------------------------------------------
// End of implementation (NmContactHistoryModelItem)
// --------------------------------------------------------------------------/*!
