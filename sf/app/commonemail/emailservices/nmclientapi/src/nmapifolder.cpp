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
 
#include "nmapiheaders.h"


namespace EmailClientApi
{

/*!
   constructor for NmFolder class
 */
NmApiFolder::NmApiFolder()
{
    NM_FUNCTION;
    
    d = new NmApiFolderPrivate();
}

/*!
    Copy constructor for NmApiMailbox 
 */
NmApiFolder::NmApiFolder(const NmApiFolder &nmApiFolder)
: d(nmApiFolder.d)
{
    NM_FUNCTION;
}

/*!
   destructor for NmApiFolder class
 */
NmApiFolder::~NmApiFolder()
{
    NM_FUNCTION;
}

/*!
   Assign data from \a folder
 */
NmApiFolder &NmApiFolder::operator=(const NmApiFolder &folder)
{
    NM_FUNCTION;
    
    if (this != &folder) {
        d = folder.d;
    }
    return *this;
}

/*!
   Compare data from \a folder
 */
bool NmApiFolder::operator==(const NmApiFolder &folder)
{
    NM_FUNCTION;
    
    bool returnValue = false;
    if (d == folder.d) {
        returnValue = true;
    }
    return returnValue;
}

/*!
   getter for id
 */
quint64 NmApiFolder::id() const
{
    NM_FUNCTION;
    
    return d->id;
}

/*!
   getter for name 
 */
QString NmApiFolder::name() const
{
    NM_FUNCTION;
    
    return d->name;
}

/*!
   getter for foldertype
 */
EmailClientApi::NmApiEmailFolderType NmApiFolder::folderType() const
{
    NM_FUNCTION;
    
    return d->folderType;
}

/*!
   setter for name
 */
void NmApiFolder::setName(const QString& name)
{
    NM_FUNCTION;
    
    d->name = name;
}

/*!
   setter for id
 */
void NmApiFolder::setId(quint64 id)
{
    NM_FUNCTION;
    
    d->id = id;
}

/*!
   setter for foldertype
 */
void NmApiFolder::setFolderType(EmailClientApi::NmApiEmailFolderType folderType)
{
    NM_FUNCTION;
    
    d->folderType = folderType;
}

/*!
   setter for parent folder id
 */
void NmApiFolder::setParentFolderId(quint64 parentId)
{
    NM_FUNCTION;
    
    d->parentId = parentId;
}

/*!
   seter for childfolder ids
 */
void NmApiFolder::setChildFolderIds(QList<quint64> &childFolderIds)
{
    NM_FUNCTION;
    
    d->childFolderIds = childFolderIds;
}

/*!
   Returns count of child folder ids.
   to be implemented later when nmail functionality is available
 */
void NmApiFolder::getChildFolderIds(QList<quint64> &childFolderIds)
{
    NM_FUNCTION;
    
    childFolderIds = d->childFolderIds;
}


/*!
   Returns id of parent folder.
 */
quint64 NmApiFolder::parentFolderId() const
{
    NM_FUNCTION;
    
    return d->parentId;
}
}
