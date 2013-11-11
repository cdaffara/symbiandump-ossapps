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
* Description:  Active callback for Backup/Restore observer.
*
*/


#include <bautils.h>

#include "hsbackuprestoreactivecallback.h"
#include "hsbackuprestoreobserver.h"

/*!
    \class CHsBURActiveCallback
    \ingroup group_hsdomainmodel
    \brief B&R active callback.
    Home screen active callback in backup/restore.
*/

/*!
    Constructor.
    \a observer Backup restore observer
*/
CHsBURActiveCallback* CHsBURActiveCallback::NewL(HsBackupRestoreObserver* observer)
{
    CHsBURActiveCallback* self =
        new( ELeave ) CHsBURActiveCallback(observer);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
}

/*!
    Destructor.
*/
CHsBURActiveCallback::~CHsBURActiveCallback()
{
}

/*!
    This method informs the active backup data client that all 
    snapshots have been supplied. If the client has not
    received a snapshot then it should perform a base backup.
    Inform that all data has been backed up or restored.
*/
void CHsBURActiveCallback::AllSnapshotsSuppliedL()
{
    // No implementation needed. Must not leave    
}

/*!
    This method receives all or part of a snapshot of data to allow 
    calculation of an incremental backup.  The snapshot is one that
    was previously supplied by the data owner.  The snapshot data 
    should be read from the location supplied. The snapshot data may
    be larger than the location supplied in which case the routine 
    will be called repeatedly until all data has been supplied.

    Snapshot data will also be supplied as part of a restore operation
          
    @param aDrive the drive being backed up
    @param aBuffer a pointer to the base of the location from whence 
           data can be copied.
    @param aLastSection ETrue if this is the last section of snapshot 
           data, else EFalse.
    
    Not supported.
*/
void CHsBURActiveCallback::ReceiveSnapshotDataL(
    TDriveNumber /*aDrive*/, TDesC8& /*aBuffer*/, TBool /*aLastSection*/)
{
	// No implementation needed
    User::Leave( KErrNotSupported );
}

/*!
    This method returns the expected size of backup data that will be
    supplied. If an incremental backup is underway then this method
    then this method will not be called until after 
    ReceiveSnapshotDataL(). The size data will be used for the purpose
    of tracking progess during a backup. If it is inaccurate then the
    user may see irregular progress but the actual backup data will 
    not be affected so it is acceptable to return an estimated value.
    
    @param aDrive the drive being backed up.
    @return the size of the data that will be returned
	
	Make a guess about data size.
*/
TUint CHsBURActiveCallback::GetExpectedDataSize(
    TDriveNumber /*aDrive*/)
{
    // No implementation needed
    return 0;
}

/*!
    This method returns a snapshot of data to accompany a backup. The 
    snapshot is expected to contain details on files / data being 
    backed up. The format of the snapshot is only meaningful to the
    data owner. The snapshot will be supplied if the data owner is 
    asked for an incremental backup and for a restore operation. The 
    snapshot data should be copied to the location supplied.
    
    The snapshot data may be larger than the location supplied in 
    which case the routine will be called repeatedly until all data
    has been retrieved.
    
    @param aDrive the drive being backed up
    @param aBuffer a pointer to the base of the location where data 
           can be copied.
    @param aFinished on return ETrue if all data has been returned 
           for this drive, else EFalse.
           
    Not supported.
*/
void CHsBURActiveCallback::GetSnapshotDataL(
    TDriveNumber /*aDrive*/, TPtr8& /*aBuffer*/, TBool& /*aFinished*/)
{
    // No implementation needed
    User::Leave( KErrNotSupported );
}

/*!
    This method prepares the implementor to return backup data. It 
    will be followed by a sequence of calls to request the actual 
    data.
          
    @param aDrive the drive being backed up.
	
    Initialize for backup.
*/
void CHsBURActiveCallback::InitialiseGetBackupDataL(
    TDriveNumber /*aDrive*/)
{
    iObserver->backupRestoreStarted();    
}

/*!
    This method requests a section of backup data.  
    InitialiseGetBackupDataL() will have been called previously to
    specify the drive concerned.  The data returned may be base or
    incremental depending on the type of backup and the capability of
    the data owner.
    
    @param aBuffer a pointer to the base of the location where data 
           can be copied.
    @param aFinished on return ETrue if all data has been returned 
          for this drive, else EFalse.
*/
void CHsBURActiveCallback::GetBackupDataSectionL(
    TPtr8& /*aBuffer*/, TBool& aFinished)
{
    // No data for active backup
    aFinished = ETrue;
}

/*!
    This method prepares the implementor to receive base restore data
    for a drive. It will be followed by a sequence of calls to supply
    the actual data.
    
    @param aDrive the drive being restored.
	
	Initialize restore.
*/
void CHsBURActiveCallback::InitialiseRestoreBaseDataL(
    TDriveNumber /*aDrive*/ )
{
    // No implementation needed
    User::Leave( KErrNotSupported );    
}

/*!
    This method receives a section of base restore data.
    InitialiseRestoreBaseDataL() will have been called previously to 
    specify the drive concerned.
    
    @param aBuffer a pointer to the base of the location whence data
           can be read.
    @param aFinished ETrue if all data has been returned for this 
           drive, else EFalse.
 
    Run state machine for restore. Receive stream from BUR engine and turn it
    to file(s).
*/
void CHsBURActiveCallback::RestoreBaseDataSectionL(
    TDesC8& /*aBuffer*/, TBool /*aFinished*/ )
{
    // No implementation needed
    User::Leave( KErrNotSupported );  
}

/*!
    This method prepares the implementor to receive incremental 
    restore data for a drive. It will be followed by a sequence 
    of calls to supply the actual data.  If multiple increments
    are supplied then this methid will be called before each increment
    
    @param aDrive the drive being restored.
    
    Not supported.
*/
void CHsBURActiveCallback::InitialiseRestoreIncrementDataL(
    TDriveNumber /*aDrive*/)
{
    // No implementation needed
    User::Leave( KErrNotSupported );
}

/*!
    This method receives a section of increment restore data.
    InitialiseRestoreIncrementDataL() will have been called 
    previously to specify the drive concerned.
    
    @param aBuffer a pointer to the base of the location whence data 
           can be read.
    @param aFinished ETrue if all data has been returned for this 
           increment, else EFalse.
 
    Not supported.
*/
void CHsBURActiveCallback::RestoreIncrementDataSectionL(
    TDesC8& /*aBuffer*/, TBool /*aFinished*/)
{
    // No implementation needed
    User::Leave( KErrNotSupported );
}

/*!
    This method is called when all data to be restored has been 
    supplied.
    
    @param aDrive the drive being restored.
    
    Called when restore is complete - sets data back to initial state.
*/
void CHsBURActiveCallback::RestoreComplete(TDriveNumber /*aDrive*/)
{
    // No implementation needed
}

/*!
    This method is called if copying of data is terminated prematurely 
    to allow the implementor to tidy up.  The same method applies to 
    all types of data and to backup and restore.
    Tidy up when operation is over.
*/
void CHsBURActiveCallback::TerminateMultiStageOperation()
{
    // No implementation needed
}

/*!
    Gets a 32-bit checksum for its private data.
    This routine is for test purposes.  It must be implemented but an
    invariant checksum value can be provided.  Some tests may cause 
    checksum values to be compared.
    
    @param aDrive the drive containing data being checksummed
    @return the 32-bit checksum
*/
TUint CHsBURActiveCallback::GetDataChecksum(TDriveNumber /*aDrive*/)
{
    // No implementation needed
    return 0;
}

/*!
    C++ constructor.
*/
CHsBURActiveCallback::CHsBURActiveCallback(HsBackupRestoreObserver* observer)
{
	iObserver = observer;
}

/*!
    2nd phase constructor.
*/
void CHsBURActiveCallback::ConstructL()
{
}
 
// End of file
