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


#include "t_hsdomainmodel.h"
#include "hsbackuprestoreobserver.h"
#ifdef Q_OS_SYMBIAN
#include "hsbackuprestoreactivecallback.h"
#endif
/*!

*/
void TestHsDomainModel::testBackupRestoreActiveCallback()
{
#ifdef Q_OS_SYMBIAN
    int error = 0;
    HsBackupRestoreObserver *observer = HsBackupRestoreObserver::instance();
    CHsBURActiveCallback *activeCallback = 0;
    TRAP(error, activeCallback = CHsBURActiveCallback::NewL(observer));
    QVERIFY(error == KErrNone);
    QVERIFY(activeCallback);
    activeCallback->AllSnapshotsSuppliedL();

    TDriveNumber driveNumber = EDriveC;
    TBuf8<1> buf;
    HBufC8 *heapBuf = HBufC8::NewL(1);
    TBool retValue = EFalse;
    
    TRAP(error, activeCallback->ReceiveSnapshotDataL(driveNumber, buf, retValue));
    QVERIFY(error == KErrNotSupported);
    
    TUint dataSize = activeCallback->GetExpectedDataSize(driveNumber);
    QVERIFY(dataSize == 0);
        
    TPtr8 bufPtr = heapBuf->Des();
    TRAP(error, activeCallback->GetSnapshotDataL(driveNumber, bufPtr, retValue));
    QVERIFY(error == KErrNotSupported);
    
    activeCallback->InitialiseGetBackupDataL(driveNumber);
    QVERIFY(observer->checkBUR());
    
    TRAP(error, activeCallback->GetBackupDataSectionL(bufPtr, retValue));
    QVERIFY(error == KErrNone);
    QVERIFY(retValue);
    
    TRAP(error, activeCallback->InitialiseRestoreBaseDataL(driveNumber));
    QVERIFY(error == KErrNotSupported);
    
    TRAP(error, activeCallback->RestoreBaseDataSectionL(buf, retValue));
    QVERIFY(error == KErrNotSupported);
    
    TRAP(error, activeCallback->InitialiseRestoreIncrementDataL(driveNumber));
    QVERIFY(error == KErrNotSupported);
    
    TRAP(error, activeCallback->RestoreIncrementDataSectionL(buf, retValue));
    QVERIFY(error == KErrNotSupported);

    activeCallback->RestoreComplete(driveNumber);
    activeCallback->TerminateMultiStageOperation();
    QVERIFY(activeCallback->GetDataChecksum(driveNumber)==0);
    
    delete activeCallback;
#endif
}

