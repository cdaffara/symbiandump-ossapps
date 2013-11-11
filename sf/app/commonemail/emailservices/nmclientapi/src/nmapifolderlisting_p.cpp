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
   Constructor form NmApiFolderListingPrivate
 */
NmApiFolderListingPrivate::NmApiFolderListingPrivate(quint64 mailboxId, QObject *parent) 
:QObject(parent), 
mMailboxId(mailboxId),
mEngine(NULL),
mIsRunning(false)
{
    NM_FUNCTION;
    mEngine = NmApiEngine::instance();
    Q_CHECK_PTR(mEngine);
}

/*!
   Destructor for NmApiFolderListingPrivate 
 */
NmApiFolderListingPrivate::~NmApiFolderListingPrivate()
{
    NM_FUNCTION;
    NmApiEngine::releaseInstance(mEngine);
}

/*!
   \brief Fetch folders from engine. 
   
   Because it uses NmFolder with sharedData we don't need care about release memory.
   
   \return Count of folders
 */
qint32 NmApiFolderListingPrivate::listFolders()
{
    NM_FUNCTION;
    mIsRunning = true;
    mFolders.clear();
    mEngine->listFolders(mMailboxId, mFolders);
    return mFolders.count();
}

/*! 
   \brief Returns results after listFolders is called.
   
    Caller gets ownership of messages. Returns true if results were available.
    It clears list of folders after be called.
    It also at start clear inputlist of NmFolder.
 */
bool NmApiFolderListingPrivate::folders(QList<EmailClientApi::NmApiFolder> &folders)
{
    NM_FUNCTION;
    bool ret(mIsRunning);
    folders.clear();
    while (!mFolders.isEmpty()) {
        folders << mFolders.takeFirst();
    }
    mIsRunning = false;
    return ret;
}

/*!
   \brief Return info if listing is running
 */
bool NmApiFolderListingPrivate::isRunning() const
{
    NM_FUNCTION;
    return mIsRunning;
}
/*!
   \brief Clears list of folders.
 */
void NmApiFolderListingPrivate::cancel() 
{
    mIsRunning = false;
    mFolders.clear();
}
}

