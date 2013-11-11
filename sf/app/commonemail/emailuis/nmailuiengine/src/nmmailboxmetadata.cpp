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
    \class NmMailboxMetaData
    \brief The NmMailboxMetaData class represents data for NmMailboxListModel.
    @alpha

    The NmMailboxMetaData class provides an item that is used by NmMailboxListModel and its' data
    method to get all information needed for one list row for a widget by calling the method once.
*/

class NmMailboxMetaDataPrivate
{
public:
    QString mName;
    NmId mId;
    NmId mIconId;
    QString mAddress;
};


/*!
    Constructor.
*/
NmMailboxMetaData::NmMailboxMetaData()
{
    NM_FUNCTION;
    
    d = new NmMailboxMetaDataPrivate;
}


/*!
    Destructor.
*/
NmMailboxMetaData::~NmMailboxMetaData()
{
    NM_FUNCTION;
    
    delete d;
}

/*!
    Mailbox name accessor.
*/
QString NmMailboxMetaData::name() const
{
    NM_FUNCTION;
    
    return d->mName;
}

/*!
    Set Mailbox name as \a name.
*/
void NmMailboxMetaData::setName(const QString &name)
{
    NM_FUNCTION;
    
    d->mName = name;
}

/*!
    Mailbox id.
*/
NmId NmMailboxMetaData::id() const
{
    NM_FUNCTION;
    
    return d->mId;
}

/*!
    Set Mailbox id as \a id.
*/
void NmMailboxMetaData::setId(const NmId& id)
{
    NM_FUNCTION;
    
    d->mId = id;
}

/*!
    Mailbox icon accessor. \note Temporary solution.
*/
NmId NmMailboxMetaData::IconId() const
{
    NM_FUNCTION;
    
    return d->mIconId;
}

/*!
    Set Mailbox icon as \a id. \note Temporary solution.
*/
void NmMailboxMetaData::setIconId(const NmId &id)
{
    NM_FUNCTION;
    
    d->mIconId = id;
}

/*!
    Get mailbox address
*/
QString NmMailboxMetaData::address() const
{
    NM_FUNCTION;
    
    return d->mAddress;
}

/*!
    Set Mailbox address as \a address
*/
void NmMailboxMetaData::setAddress(const QString &address)
{
    NM_FUNCTION;
    
    d->mAddress = address;
}
