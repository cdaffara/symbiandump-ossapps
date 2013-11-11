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

#ifndef NMAILFOLDER_H
#define NMAILFOLDER_H

#include <QString>
#include <QDateTime>
#include <QExplicitlySharedDataPointer>

#include "nmbasedef.h"
#include "nmcommon.h"

class NMBASE_EXPORT NmFolderPrivate : public QSharedData
{
public:
    NmFolderPrivate();
    virtual ~NmFolderPrivate();

public:
    NmId mFolderId;
    NmId mParentFolderId;
    NmId mMailboxId;
    QString mName;
    NmFolderType mFolderType;
    quint32 mMessageCount;
    quint32 mUnreadMessageCount;
    quint32 mUnseenCount;
    quint32 mSubFolderCount;
    QDateTime mUpdated;
};

class NMBASE_EXPORT NmFolder
{
public:

    NmFolder(NmId folderId);
    NmFolder(const NmFolder &foder);
    NmFolder(QExplicitlySharedDataPointer<NmFolderPrivate> folderPrivate);
    NmFolder &operator=(const NmFolder &folder);
    virtual ~NmFolder();

    NmId folderId() const;
    void setFolderId(NmId folderId);

    NmId parentId() const;
    void setParentId(NmId parentFolderId);

    NmId mailboxId() const;
    void setMailboxId(NmId mailboxId);

    QString name() const;
    void setName(QString name);

    NmFolderType folderType() const;
    void setFolderType(NmFolderType folderType);

    QDateTime lastUpdated() const;
    void setLastUpdated(QDateTime time);

    quint32 messageCount() const;
    void setMessageCount(quint32 messageCount);

    quint32 unreadMessageCount() const;
    void setUnreadMessageCount(quint32 unreadMessageCount);

    quint32 unseenCount() const;
    void setUnseenCount(quint32 unseenCount);

    quint32 subFolderCount() const;
    void setSubFolderCount(quint32 subFolderCount);

private:
  QExplicitlySharedDataPointer<NmFolderPrivate> d;

  friend class CFSMailFolderBase;
};


#endif /* NMAILFOLDER_H */
