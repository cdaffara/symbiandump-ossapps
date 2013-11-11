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

#include "emailtrace.h"

#include "nmfolder.h"



NmFolderPrivate::NmFolderPrivate()
:mFolderId(0),
mParentFolderId(0),
mMailboxId(0),
mName(""),
mFolderType(NmFolderOther),
mMessageCount(0),
mUnreadMessageCount(0),
mUnseenCount(0),
mSubFolderCount(0),
mUpdated(QDateTime())
{
    NM_FUNCTION;
}

NmFolderPrivate::~NmFolderPrivate()
{
    NM_FUNCTION;
}


/*!
    \class NmFolder
    \brief The NmFolder class represents data model for folder.
*/


/*!
    Constructor for folder identified by \a folderId.
*/
NmFolder::NmFolder(NmId folderId)
{
    NM_FUNCTION;
    
    d = new NmFolderPrivate();
    d->mFolderId = folderId;
}

/*!
    Copy constructor
*/
NmFolder::NmFolder(const NmFolder &folder)
{
    NM_FUNCTION;
    
    d = folder.d;
}

/*!
 * Creating NmFolder from privateData
 */
NmFolder::NmFolder(QExplicitlySharedDataPointer<NmFolderPrivate> folderPrivate)
{
    NM_FUNCTION;
    
  d = folderPrivate;
}

/*!
    operator =
*/
NmFolder &NmFolder::operator=(const NmFolder &folder)
{
    NM_FUNCTION;
    
  if (this != &folder) {
    d = folder.d;
  }
  return *this;
}

/*!
    Destructor
*/
NmFolder::~NmFolder()
{
    NM_FUNCTION;
}

/*!
    Folder id accessor
*/
NmId NmFolder::folderId() const
{
    NM_FUNCTION;
    
    return d->mFolderId;
}

/*!
    Set folder id
*/
void NmFolder::setFolderId(NmId folderId)
{
    NM_FUNCTION;
    
  d->mFolderId = folderId;
}

/*!
    Parent id accessor
*/
NmId NmFolder::parentId() const
{
    NM_FUNCTION;
    
    return d->mParentFolderId;
}

/*!
    Set parent id
*/
void NmFolder::setParentId(NmId parentFolderId)
{
    NM_FUNCTION;
    
    d->mParentFolderId = parentFolderId;
}

/*!
    Mailbox id accessor
*/
NmId NmFolder::mailboxId() const
{
    NM_FUNCTION;
    
    return d->mMailboxId;
}

/*!
    Set mailbox id
*/
void NmFolder::setMailboxId(NmId mailboxId)
{
    NM_FUNCTION;
    
    d->mMailboxId = mailboxId;
}

/*!
    Folder name accessor
*/
QString NmFolder::name() const
{
    NM_FUNCTION;
    
    return d->mName;
}

/*!
    Set folder name
*/
void NmFolder::setName(QString name)
{
    NM_FUNCTION;
    
    d->mName = name;
}

/*!
    Folder type accessor
*/
NmFolderType NmFolder::folderType() const
{
    NM_FUNCTION;
    
    return d->mFolderType;
}

/*!
    Set folder type
*/
void NmFolder::setFolderType(NmFolderType folderType)
{
    NM_FUNCTION;
    
    d->mFolderType = folderType;
}

/*!
    Last update time accessor
*/
QDateTime NmFolder::lastUpdated() const
{
    NM_FUNCTION;
    
    return d->mUpdated;
}

/*!
    Set last update time
*/
void NmFolder::setLastUpdated(QDateTime time)
{
    NM_FUNCTION;
    
    d->mUpdated = time;
}

/*!
    Get count of messages
 */
quint32 NmFolder::messageCount() const
{
    NM_FUNCTION;
    
  return d->mMessageCount;
}

/*!
    Set count of messages
 */
void NmFolder::setMessageCount(quint32 messageCount)
{
    NM_FUNCTION;
    
  d->mMessageCount = messageCount;
}

/*!
    Count of unread messages
 */
quint32 NmFolder::unreadMessageCount() const
{
    NM_FUNCTION;
    
  return d->mUnreadMessageCount;
}

/*!
    Set count of unread messages
 */
void NmFolder::setUnreadMessageCount(quint32 unreadMessageCount)
{
    NM_FUNCTION;
    
  d->mUnreadMessageCount = unreadMessageCount;
}

/*!
    Count of unseen messages
 */
quint32 NmFolder::unseenCount() const
{
    NM_FUNCTION;
    
  return d->mUnseenCount;
}

/*!
    Set count of unseen messages
 */
void NmFolder::setUnseenCount(quint32 unseenCount)
{
    NM_FUNCTION;
    
  d->mUnseenCount =  unseenCount;
}

/*!
    Count of subfolders of current folder
 */
quint32 NmFolder::subFolderCount() const
{
    NM_FUNCTION;
    
  return d->mSubFolderCount;
}

/*!
    Set count of subfolders of current folder
 */
void NmFolder::setSubFolderCount(quint32 subFolderCount)
{
    NM_FUNCTION;
    
  d->mSubFolderCount = subFolderCount;
}

