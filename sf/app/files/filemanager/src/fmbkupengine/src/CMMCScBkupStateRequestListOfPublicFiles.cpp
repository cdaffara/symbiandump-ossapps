/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: CMMCScBkupStateRequestListOfPublicFiles implementation
*
*
*/

#include "CMMCScBkupStateRequestListOfPublicFiles.h"

// System includes
#include <e32std.h>

// User includes
#include "MMCScBkupLogger.h"
#include "MMCScBkupSBEUtils.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "RMMCScBkupProgressSizer.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "MMMCScBkupProgressObserver.h"
#include "CMMCScBkupFileListCollection.h"
#include "CMMCScBkupDataOwnerCollection.h"




// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestListOfPublicFiles::CMMCScBkupStateRequestListOfPublicFiles()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateRequestListOfPublicFiles::CMMCScBkupStateRequestListOfPublicFiles( MMMCScBkupDriver& aDriver )
:   CMMCScBkupState( aDriver )
    {
    __LOG1("CMMCScBkupStateRequestListOfPublicFiles::CMMCScBkupStateRequestListOfPublicFiles() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestListOfPublicFiles::~CMMCScBkupStateRequestListOfPublicFiles()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateRequestListOfPublicFiles::~CMMCScBkupStateRequestListOfPublicFiles()
    {
    Cancel();
    //
    delete iRequestObject;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestListOfPublicFiles::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestListOfPublicFiles::ConstructL()
    {
    iRequestObject = CMMCScBkupStateRequestSpecificPublicFileInfo::NewL(Driver());
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestListOfPublicFiles::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateRequestListOfPublicFiles* CMMCScBkupStateRequestListOfPublicFiles::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateRequestListOfPublicFiles* self = new(ELeave) CMMCScBkupStateRequestListOfPublicFiles( aDriver );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestListOfPublicFiles::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateRequestListOfPublicFiles::StateId() const
    {
    return KMMCScBkupStateIdRequestListOfPublicFiles;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestListOfPublicFiles::NextStateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateRequestListOfPublicFiles::NextStateId() const
    {
    return KMMCScBkupStateIdValidateDiskSpace;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestListOfPublicFiles::PerformStateInitL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestListOfPublicFiles::PerformStateInitL()
    {
    // Set to -1 so that the first time RunL is called, we start with
    // the data owner at index 0.
    iCurrentDataOwnerIndex = -1;
    CompleteSelf(KErrNone);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestListOfPublicFiles::PerformAsynchronousStateStepL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestListOfPublicFiles::PerformAsynchronousStateStepL()
    {
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    if  (++iCurrentDataOwnerIndex < dataOwners.Count())
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner(iCurrentDataOwnerIndex);

        const TBool hasPublicFiles = (owner.Owner().PassiveSettings() & EHasPublicFiles);
        if  (hasPublicFiles)
            {
            // Request all the public files (on all supported drives) for this
            // particular data owner
            __ASSERT_ALWAYS(iRequestObject->IsActive() == EFalse, User::Invariant());
            //
            iRequestObject->RequestL(owner, iStatus);
            SetActive();
            }
        else
            {
            // Try next data owner
            CompleteSelf();
            }
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestListOfPublicFiles::PerformAsynchronousErrorCleanup()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateRequestListOfPublicFiles::PerformAsynchronousErrorCleanup(TInt aError)
    {
    (void) aError;
    __LOG1("CMMCScBkupStateRequestListOfPublicFiles::PerformAsynchronousErrorCleanup() - **** - error: %d", aError );

    // Cancel requester. Its probably already inactive, but just in case...
    PerformAsynchronousCancellation();

    // Reset state and move to next item. Treat the current (badly behaving) item
    // as completed/processed.
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    if  ( iCurrentDataOwnerIndex < dataOwners.Count() )
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner( iCurrentDataOwnerIndex );
        (void) owner;
        __LOGFILE2("CMMCScBkupStateRequestListOfPublicFiles::PerformAsynchronousErrorCleanup() - **** - error: %d, SID: 0x%08x - PUBLIC data skipped for DO", aError, owner.SecureId().iId );

        // Ensures that asynchronous state step is called again..
        CompleteSelf();
        }

    return ETrue;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestListOfPublicFiles::PerformAsynchronousCancellation()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestListOfPublicFiles::PerformAsynchronousCancellation()
    {
    if  ( iRequestObject )
        {
        iRequestObject->Cancel();
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestListOfPublicFiles::PerformLastRightsL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestListOfPublicFiles::PerformLastRightsL()
    {
    // Now we can calculate the progress        
    TInt64 totalProgressAmount = 0;

    // Calculate the total progress required for the entire backup operation
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    const TInt count = dataOwners.Count();
    //
    RMMCScBkupProgressSizer sizer( Driver().DrvParamsBase().DriveAndOperations() );
    for(TInt i=0; i<count; i++)
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner( i );
        //
        totalProgressAmount += sizer.BackupTotalProgressValueL( owner );
        }

    // Check whether all categories have been processed and either store current 
    // progress information in driver or inform progress handler about cumulative progress.
    // If size is getting beyond file system limitation (2GB), leave with KErrxxx.
    if(Driver().DrvLastCategory())
        {
        __LOG2("CMMCScBkupStateRequestListOfPublicFiles::PerformLastRightsL() - report progress understood (%Ld + %Ld)", 
            totalProgressAmount, Driver().DrvTotalProgress());
        
        Driver().DrvStoreTotalProgress( totalProgressAmount );

        if( Driver().DrvTotalProgress() > KMaxTInt )
            {
            __LOG1("CMMCScBkupStateRequestListOfPublicFiles::PerformLastRightsL() - leaving with KErrNotSupported due amount of data (%Ld)", 
                Driver().DrvTotalProgress());
            Driver().DrvProgressHandler().MMCScBkupStartBackuping( EFalse );
            User::Leave( KErrNotSupported );
            }
        else
            {
            Driver().DrvProgressHandler().MMCScBkupHandleProgressDomainUnderstood( Driver().DrvTotalProgress() );
            }
        }
    else
        {
        __LOG2("CMMCScBkupStateRequestListOfPublicFiles::PerformLastRightsL() - adding progress amount %Ld to existing %Ld)", 
            totalProgressAmount, Driver().DrvTotalProgress());
            
        if( (totalProgressAmount + Driver().DrvTotalProgress()) > KMaxTInt )
            {
            __LOG1("CMMCScBkupStateRequestListOfPublicFiles::PerformLastRightsL() - leaving due amount of data (%Ld)", 
                totalProgressAmount + Driver().DrvTotalProgress());
            User::Leave(KErrNotSupported);
            }
        else
            {
            Driver().DrvStoreTotalProgress( totalProgressAmount );
            }
        }
    }








































// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSpecificPublicFileInfo::CMMCScBkupStateRequestSpecificPublicFileInfo()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateRequestSpecificPublicFileInfo::CMMCScBkupStateRequestSpecificPublicFileInfo( MMMCScBkupDriver& aDriver )
:   CMMCScBkupDriveSpecificRequest( aDriver.DrvParamsBase().DriveAndOperations(), EMMCScBkupOwnerDataTypePublicData ), iDriver(aDriver)
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSpecificPublicFileInfo::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateRequestSpecificPublicFileInfo* CMMCScBkupStateRequestSpecificPublicFileInfo::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateRequestSpecificPublicFileInfo* self = new (ELeave) CMMCScBkupStateRequestSpecificPublicFileInfo(aDriver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSpecificPublicFileInfo::RequestL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestSpecificPublicFileInfo::RequestL( CMMCScBkupDataOwnerInfo& aOwner, TRequestStatus& aObserver )
    {
    iOwner = &aOwner;
    CMMCScBkupDriveSpecificRequest::RequestL( aObserver );

    // Start the asynchronous request. 
    CompleteSelf(KErrNone);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSpecificPublicFileInfo::RunL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestSpecificPublicFileInfo::RunL()
    {
    __LOG1("CMMCScBkupStateRequestSpecificPublicFileInfo::RunL() - START - iStatus: %d", iStatus.Int());
    User::LeaveIfError(iStatus.Int());
    //
    CDataOwnerInfo& owner = iOwner->Owner();
    const TDriveList& ownerDrives = owner.DriveList();
    TDriveNumber drive = EDriveA;
    const TBool driveAvailable = NextValidDrive(drive, ownerDrives);
    const TSecureId secureId = iOwner->SecureId();

    __LOG3("CMMCScBkupStateRequestSpecificPublicFileInfo::RunL() - secureId: 0x%08x, driveAvailable: %d, drive: %c", secureId.iId, driveAvailable, drive + 'A');

    if  (driveAvailable)
        {
        // This drive needs to be scanned...
        TInt err( KErrNone );
        RFileArray files;
        CleanupClosePushL(files);

        // Get a copy of the generic data type for which we are
        // requesting file listings...
        CSBGenericDataType* genericDataType = CSBSecureId::NewL( secureId );
        CleanupStack::PushL( genericDataType );
        TRAP( err, Driver().DrvSecureBackupClient().PublicFileListL( drive, *genericDataType, files ) );
        if( err != KErrNone && err != KErrNotFound )
            {
            User::Leave( err );
            }
        CleanupStack::PopAndDestroy( genericDataType );
        // Additionally we need to request list of public files registered for packages.
        if( MMCScBkupSBEUtils::HasSystemDataL( owner ) )
            {
            RFileArray pkgFiles;
            CleanupClosePushL( pkgFiles );
            __LOG2("CMMCScBkupStateRequestSpecificPublicFileInfo::RunL() - have %d files before package listing for DO, err %d", files.Count(), err);
            TRAP( err, Driver().DrvSecureBackupClient().PublicFileListL( drive, owner.Identifier(), pkgFiles ) );
            __LOG2("CMMCScBkupStateRequestSpecificPublicFileInfo::RunL() - have %d package files for DO, err %d", files.Count(), err);
            const TInt pkgCount = pkgFiles.Count();
            for(TInt i = 0; i < pkgCount; i++)
                {
                files.AppendL( pkgFiles[i] );
                }
            CleanupStack::PopAndDestroy( &pkgFiles );
            }

        // Add to the file list
        __LOG1("CMMCScBkupStateRequestSpecificPublicFileInfo::RunL() - have %d files for DO", files.Count());
        if  (files.Count())
            {

            // We want to be told by the public file collection exactly which files
            // it added... We need to do this in order to report progress as accurately 
            // as possible during the backup operation.
            RArray< const CMMCScBkupFileInfo* > addedItems;
            CleanupClosePushL(addedItems);
            
            // Add them to the public data file collection
            Driver().DrvFileList().AppendL( files, addedItems, secureId );
            
            // For each identified file, update the data owner with the size of public data
            const TInt count = addedItems.Count();
            for(TInt i=0; i<count; i++)
                {
                const CMMCScBkupFileInfo& file = *addedItems[i];
                const TInt size = file.Size();
                //
                __LOG3("CMMCScBkupStateRequestSpecificPublicFileInfo::RunL() - file[%04d] = %S, size: %d", i, &file.FileName(), size);
                iOwner->AddToOperationalSizeL( EMMCScBkupOwnerDataTypePublicData, drive, size );
                }
            //
            CleanupStack::PopAndDestroy( &addedItems );
            }
        //
        CleanupStack::PopAndDestroy( &files );

        // Complete ourself to try the next drive
        CompleteSelf( KErrNone );
        }
    else
        {
        CompleteObserverRequest( KErrNone );
        }

    __LOG("CMMCScBkupStateRequestSpecificPublicFileInfo::RunL() - END");
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSpecificPublicFileInfo::DoCancel()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestSpecificPublicFileInfo::DoCancel()
    {
    }




