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

#ifndef NMAPIFOLDERINFO_H
#define NMAPIFOLDERINFO_H

#include <QExplicitlySharedDataPointer>
#include <QList>
#include <nmapidef.h>
#include <nmapicommon.h>

/*!
   Mailbox metadata class
 */
namespace EmailClientApi
{

class NmApiFolderPrivate;

class NMAPI_EXPORT NmApiFolder
{
public:
    /*!
       constructor for NmApiFolder class
     */
    NmApiFolder();
    
    /*!
        Copy constructor for NmApiFolder 
     */
    NmApiFolder(const NmApiFolder &nmApiFolder);
    
    /*!
       destructor for NmApiFolder class
     */
    virtual ~NmApiFolder();

    /*!
       Assigns data from \a folder
     */
    NmApiFolder &operator=(const NmApiFolder &folder);
    
    /*!
       Compares data from \a folder
     */
    bool operator==(const NmApiFolder &folder);
    
    /*!
       getter for id
     */
    quint64 id() const;

    /*!
       getter for name 
     */
    QString name() const;

    /*!
       getter for foldertype
     */
    EmailClientApi::NmApiEmailFolderType folderType() const;

    /*!
       setter for id
     */
    void setId(quint64 id);

    /*!
       setter for name
     */
    void setName(const QString &name);

    /*!
       setter for foldertype
     */
    void setFolderType(EmailClientApi::NmApiEmailFolderType folderType);

    /*!
       setter for childfolder ids
     */
    void setChildFolderIds(QList<quint64> &childFolderIds);

    /*!
       setter for childfolder ids
     */
    void setParentFolderId(quint64 parentId);

    /*!
       returns child folder ids.
       to be implemented later when nmail functionality is available
     */
    void getChildFolderIds(QList<quint64> &childFolderIds);

    /*!
       returns parent folder id.
       
     */
    quint64 parentFolderId() const;

private:
    QExplicitlySharedDataPointer<NmApiFolderPrivate> d;
};

}
#endif
