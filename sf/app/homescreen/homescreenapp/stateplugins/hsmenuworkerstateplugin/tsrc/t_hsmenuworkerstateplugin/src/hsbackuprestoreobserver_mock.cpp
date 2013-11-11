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
* Description:  Home screen Backup/Restore observer.
*
*/

#include "hsdatabase.h"
#include "hsbackuprestoreobserver.h"


/*!
    Singleton. 
*/
HsBackupRestoreObserver *HsBackupRestoreObserver::instance()
{
    if (!mInstance) {
        mInstance = new HsBackupRestoreObserver();
    }
    return mInstance;
}

/*!
    Constructs a new observer with the given \a parent object.
*/
HsBackupRestoreObserver::HsBackupRestoreObserver(QObject *parent)
 : QObject(parent),
   mBUROngoing(false)
{
}

/*!
    Destructor.
*/
HsBackupRestoreObserver::~HsBackupRestoreObserver()
{
}

/*!
    Called when backup/restore is started.
*/
void HsBackupRestoreObserver::backupRestoreStarted()
{
    mBUROngoing = true;
    emit event_backupRestoreStarted();
}

/*!
    Called when backup/restore is completed.
*/
void HsBackupRestoreObserver::backupRestoreCompleted()
{
    mBUROngoing = false;
    emit event_backupRestoreCompleted();
}

/*!
    Called when Home screen is ready for backup/restore.
*/
void HsBackupRestoreObserver::readyForBUR()
{
}

/*!
    Returns true if bacup/restore is ongoing.
*/
bool HsBackupRestoreObserver::checkBUR()
{
    if (mBUROngoing) {
        return true;
    }
    return false;
}
/*!
    Points to the observer instance.
*/
HsBackupRestoreObserver *HsBackupRestoreObserver::mInstance(0);

// End of File
