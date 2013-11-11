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
* Description: Active callback for Backup/Restore observer.
*
*/


#ifndef HSBACKUPRESTOREACTIVECALLBACK_H
#define HSBACKUPRESTOREACTIVECALLBACK_H

#include <e32base.h>
#include <connect/abclient.h>

class HsBackupRestoreObserver;


NONSHARABLE_CLASS(CHsBURActiveCallback) : public CBase, 
	                                     public conn::MActiveBackupDataClient
    {
public:

    static CHsBURActiveCallback* NewL(HsBackupRestoreObserver* observer);

    virtual ~CHsBURActiveCallback();    

    // new functions from base class MActiveBackupDataClient
    virtual void AllSnapshotsSuppliedL();
    virtual void ReceiveSnapshotDataL(
        TDriveNumber aDrive, TDesC8& aBuffer, TBool aLastSection);
    virtual TUint GetExpectedDataSize(TDriveNumber aDrive);
    virtual void GetSnapshotDataL( 
        TDriveNumber aDrive, TPtr8& aBuffer, TBool& aFinished);
    virtual void InitialiseGetBackupDataL(TDriveNumber aDrive);
    virtual void GetBackupDataSectionL(TPtr8& aBuffer, TBool& aFinished);
    virtual void InitialiseRestoreBaseDataL(TDriveNumber aDrive);
    virtual void RestoreBaseDataSectionL(TDesC8& aBuffer, TBool aFinished);
    virtual void InitialiseRestoreIncrementDataL(TDriveNumber aDrive);
    virtual void RestoreIncrementDataSectionL(TDesC8& aBuffer, TBool aFinished);
    virtual void RestoreComplete(TDriveNumber aDrive);   
    virtual void TerminateMultiStageOperation();
    virtual TUint GetDataChecksum(TDriveNumber aDrive);

private:
    CHsBURActiveCallback(HsBackupRestoreObserver* observer);

    void ConstructL();

private:
	// Bacup/Restore observer. Not own.
    HsBackupRestoreObserver *iObserver;
    };

#endif // HSBACKUPRESTOREACTIVECALLBACK_H

