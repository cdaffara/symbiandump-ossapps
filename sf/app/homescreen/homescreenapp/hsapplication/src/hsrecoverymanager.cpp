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

#include <e32property.h>

#include <QFile>

#include "hsrecoverymanager.h"

const TInt KPSCategoryUid(0x20022F36);
const TInt KPSCrashCountKey(1);
const int KCrashCountThreshold(3);
const int KResetCrashCountInterval(60000);

/*!
    \class HsRecoveryManager
    \ingroup group_hsapplication
    \brief Used during homescreen startup for recovering from possible crash situations.
*/

/*!
    Constructs a new recovery manager with the given \a parent item.
*/
HsRecoveryManager::HsRecoveryManager(QObject *parent)
  : QObject(parent)
{    
    mTimer.setInterval(KResetCrashCountInterval);
    mTimer.setSingleShot(true);
    connect(&mTimer, SIGNAL(timeout()), SLOT(resetCrashCount()));
}

/*!
    Destructor.
*/
HsRecoveryManager::~HsRecoveryManager()
{
}

/*!
    Executes this recovery manager. If crash count exceeds the
    threshold, the ROM configuration is selected. Otherwise,
    the existing one is used.
*/
void HsRecoveryManager::execute()
{
    int count = crashCount();    
    if (KCrashCountThreshold <= count) {
        restoreRomDatabase();
        resetCrashCount();
        return;
    }
    if (0 < count) {
        mTimer.start();
    }
}

/*!
    Restores the ROM database to c: drive.
*/
void HsRecoveryManager::restoreRomDatabase()
{
    // If exists, remove the database from c:
    QFile file("c:/private/20022f35/homescreen.db");
    if (file.exists()) {        
        file.remove();
    }          
    // Copy the rom database to c: and set permissions.
    file.setFileName("z:/private/20022f35/homescreen.db");     
    if (!file.copy("c:/private/20022f35/homescreen.db")) {
        qDebug() << "copying of homescreen.db to c: drive failed";
    } else {
        file.setFileName("c:/private/20022f35/homescreen.db");
        if (!file.setPermissions(QFile::ReadOwner | QFile::WriteOwner)) {
            qDebug() << "setPermissions of homescreen.db failed";
        }
    }
}

/*!
    Resets the crash count to zero.
*/
void HsRecoveryManager::resetCrashCount()
{
    RProperty::Set(TUid::Uid(KPSCategoryUid), KPSCrashCountKey, 0); 
}

/*!
    Returns the current crash count.
*/
int HsRecoveryManager::crashCount()
{
    TInt count = 0;
    RProperty::Get(TUid::Uid(KPSCategoryUid), KPSCrashCountKey, count);    
    return count;
}
