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

#include "t_hsapplication.h"
#include "hsrecoverymanager.h"

const TInt KPSCategoryUid(0x20022F36);
const TInt KPSCrashCountKey(1);
const int KResetCrashCountInterval(60000);

void t_hsapplication::testHsRecoveryManagerConstruction()
{
    HsRecoveryManager rm;
    QVERIFY(rm.mTimer.isSingleShot());
    QCOMPARE(rm.mTimer.interval(), KResetCrashCountInterval);
}

void t_hsapplication::testHsRecoveryManagerCrashCount()
{    
    TInt storedCount = 0;
    RProperty::Get(TUid::Uid(KPSCategoryUid), KPSCrashCountKey, storedCount);
 
    HsRecoveryManager rm;
    rm.resetCrashCount();
    QCOMPARE(rm.crashCount(), 0);
    
    RProperty::Set(TUid::Uid(KPSCategoryUid), KPSCrashCountKey, storedCount); 
}

void t_hsapplication::testHsRecoveryManagerRestoreRomDatabase()
{
    HsRecoveryManager rm;
    rm.restoreRomDatabase();
}

void t_hsapplication::testHsRecoveryManagerExecute()
{
    HsRecoveryManager rm;
    rm.execute();
}
