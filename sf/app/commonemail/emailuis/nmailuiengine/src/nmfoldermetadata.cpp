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
    \class NmFolderMetaData
    \brief The NmFolderMetaData class represents data for NmFolderListModel.
    @alpha

    The class NmFolderMetaData provides an item that is used by NmFolderListModel and its' data
    method to get all information needed for one list row for a widget by calling the method once.
*/

class NmFolderMetaDataPrivate
{
public:
    QString mName;
    NmId mId;
};


/*!
    Constructor.
*/
NmFolderMetaData::NmFolderMetaData()
{
    NM_FUNCTION;
    
    d = new NmFolderMetaDataPrivate;
}


/*!
    Destructor.
*/
NmFolderMetaData::~NmFolderMetaData()
{
    NM_FUNCTION;
    
    delete d;
}

/*!
    Mailbox name accessor.
*/
QString NmFolderMetaData::name() const
{
    NM_FUNCTION;
    
    return d->mName;
}

/*!
    Set Mailbox name as \a name.
*/
void NmFolderMetaData::setName(const QString &name)
{
    NM_FUNCTION;
    
    d->mName = name;
}

/*!
    Mailbox id.
*/
NmId NmFolderMetaData::id() const
{
    NM_FUNCTION;
    
    return d->mId;
}

/*!
    Set folder id as \a id.
*/
void NmFolderMetaData::setId(const NmId &id)
{
    NM_FUNCTION;
    
    d->mId = id;
}








