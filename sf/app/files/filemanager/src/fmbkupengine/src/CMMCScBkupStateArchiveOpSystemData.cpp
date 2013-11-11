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
* Description: CMMCScBkupStateArchiveOpSystemData implementation
*
*
*/

#include "CMMCScBkupStateArchiveOpSystemData.h"

// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupDataOwnerCollection.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "MMCScBkupSBEUtils.h"
#include "CMMCScBkupArchive.h"
#include "CMMCScBkupArchiveFooter.h"
#include "CMMCScBkupIndexWithIdentifier.h"
#include "MMMCScBkupArchiveDataInterface.h"



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::CMMCScBkupStateArchiveOpSystemData()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpSystemData::CMMCScBkupStateArchiveOpSystemData( MMMCScBkupDriver& aDriver )
:   CMMCScBkupStateOpAware( aDriver ), iIndexValueCurrent( -1 )
    {
    __LOG1("CMMCScBkupStateArchiveOpSystemData::CMMCScBkupStateArchiveOpSystemData() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::~CMMCScBkupStateArchiveOpSystemData()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpSystemData::~CMMCScBkupStateArchiveOpSystemData()
    {
    Cancel();
    //
    delete iBackupTransferObject;
    delete iRestoreTransferObject;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpSystemData* CMMCScBkupStateArchiveOpSystemData::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateArchiveOpSystemData* self = new(ELeave) CMMCScBkupStateArchiveOpSystemData( aDriver );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpSystemData::StateId() const
    {
    return KMMCScBkupStateIdArchiveOpSystemData;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::NextStateBackupId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpSystemData::NextStateBackupId( TBool /*aPartial*/ ) const
    {
    return KMMCScBkupStateIdArchiveOpJavaData;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::NextStateRestoreId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpSystemData::NextStateRestoreId( TBool /*aPartial*/ ) const
    {
    return KMMCScBkupStateIdArchiveOpJavaData;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::PerformStateInitBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpSystemData::PerformStateInitBackupL( TBool /*aPartial*/ )
    {
    __DEBUG_TESTING_SKIP_BACKUP_SYS_DATA;

    iBackupTransferObject = CMMCScBkupWriteDataTransferRequest< TPackageDataType >::NewL(
                                    Driver(), 
                                    *this,
                                    EMMCScBkupOwnerDataTypeSystemData,
                                    EPackageTransferDerivedType,
                                    ESystemData
                                    );
    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::PerformStateInitRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpSystemData::PerformStateInitRestoreL( TBool /*aPartial*/ )
    {
    __LOG("CMMCScBkupStateArchiveOpSystemData::PerformStateInitRestoreL() - START" );
    __DEBUG_TESTING_SKIP_RESTORE_SYS_DATA;

    iRestoreTransferObject = CMMCScBkupReadDataTransferRequest< TPackageDataType >::NewL(
                                    Driver(),
                                    EMMCScBkupOwnerDataTypeSystemData,
                                    ESystemData
                                    );

    CompleteSelf();
    __LOG("CMMCScBkupStateArchiveOpSystemData::PerformStateInitRestoreL() - END" );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousStateStepBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousStateStepBackupL( TBool /*aPartial*/ )
    {
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    const TInt count = dataOwners.Count();
    //
    while ( ++iIndexValueCurrent < count )
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner( iIndexValueCurrent );
        //
        if  ( owner.CompletionStatus( EMMCScBkupOwnerDataTypeSystemData ) == EFalse )
            {
            // Request all the system data (on all supported drives) for this
            // particular data owner
            __ASSERT_ALWAYS( iBackupTransferObject->IsActive() == EFalse, User::Invariant() );
            iBackupTransferObject->RequestL( owner, 
                                             iStatus,
                                             Driver().DrvParamsBase().DriveAndOperations() );
            SetActive();
            break; // while loop
            }
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousStateStepRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousStateStepRestoreL( TBool /*aPartial*/ )
    {
    CMMCScBkupIndexBase& indexBase = Driver().DrvArchive().Footer().IndexByType( EMMCScBkupOwnerDataTypeSystemData );
    CMMCScBkupIndexWithIdentifier<  TInt32 >& index = static_cast< CMMCScBkupIndexWithIdentifier< TInt32 >& >( indexBase );
    const TInt count = index.Count();
    __LOG1("CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousStateStepRestoreL() - START - %d data owners...", count );
    //
    if  ( ++iIndexValueCurrent < count )
        {
        __LOG2("CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousStateStepRestoreL() - getting system data index record [%03d/%03d]", iIndexValueCurrent + 1, count);
        const CMMCScBkupIndexEntry< TInt32 >& entry = index.At( iIndexValueCurrent );
        iSecureId = entry.Identifier();

        __LOG("CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousStateStepRestoreL() - getting sub-entries for index record...");
        const RArray< TMMCScBkupArchiveDriveAndVector >& subEntries = entry.Entries();
        __LOG1("CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousStateStepRestoreL() - got %d sub-entries for index record", subEntries.Count());
#ifdef __MMCSCBKUPLOGGING_ENABLED__
        for(TInt i=0; i<subEntries.Count(); i++)
            {
            const TMMCScBkupArchiveDriveAndVector& item = subEntries[i];
            __LOG4("CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousStateStepRestoreL() - subentry[%2d] = %c:\\ [Offset: %8d, Length: %d] ", i, item.iDrive + 'A', item.iVector.Offset(), item.iVector.Length() );
            }
#endif
        
        // Get the associated data owner
        CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();

        __LOG1("CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousStateStepRestoreL() - trying to supply system data for SID: 0x%08x", entry.Identifier() );

        // The owner in question should be a package type object. 
        CMMCScBkupDataOwnerInfo& owner = dataOwners.OwnerL( entry.Identifier() );

        if  ( owner.CompletionStatus( EMMCScBkupOwnerDataTypeSystemData ) == EFalse )
            {
            // The owner in question should be a package type object. 
            const TSBDerivedType ownersDerivedType = owner.Owner().Identifier().DerivedTypeL();
            __ASSERT_ALWAYS( ownersDerivedType == EPackageDerivedType || ownersDerivedType == EPackageTransferDerivedType, User::Invariant() );

#ifdef __MMCSCBKUPLOGGING_ENABLED__
            const TUid packageUid = MMCScBkupSBEUtils::PackageIdFromGenericL( owner.Owner().Identifier() );
            __LOG2("CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousStateStepRestoreL() - trying to restore SYSTEM DATA for SID: 0x%08x, packageUID: 0x%08x", entry.Identifier(), packageUid.iUid );
#endif

            // Safety check
            __ASSERT_ALWAYS( iRestoreTransferObject->IsActive() == EFalse, User::Invariant() );

            // Request all the system data (on all supported drives) for this
            // particular data owner
            iRestoreTransferObject->RequestL( owner, 
                                              iStatus,
                                              subEntries );
            SetActive();
            }
        else
            {
            CompleteSelf();
            }
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousErrorCleanupBackup()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousErrorCleanupBackup( TBool /*aPartial*/, TInt aError )
    {
    (void) aError;
    __LOG1("CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousErrorCleanupBackup() - error: %d", aError );

    // Reset state and move to next item. Treat the current (badly behaving) item
    // as completed/processed.
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    if  ( iIndexValueCurrent < dataOwners.Count() )
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner( iIndexValueCurrent );
        owner.SetCompletionStatus( EMMCScBkupOwnerDataTypeSystemData, ETrue );

        __LOGFILE2("CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousErrorCleanupBackup() - **** - error: %d, SID: 0x%08x - system data skipped for DO", aError, owner.SecureId().iId );
        }

    CompleteSelf();
    return ETrue;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousErrorCleanupRestore()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousErrorCleanupRestore( TBool /*aPartial*/, TInt aError )
    {
    (void) aError;
    __LOGFILE1("CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousErrorCleanupRestore() - **** - error: %d", aError );

    // If system data restore phase returns "not supported", let's treat it as 
    // such an error, that other data types for this data owner are not
    // allowed to restore either.
    
    if( aError == KErrNotSupported || aError == KErrAlreadyExists )
        {
        CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
        CMMCScBkupDataOwnerInfo* owner = NULL;
        TRAPD( err, owner = &dataOwners.OwnerL( iSecureId ) );
        if( err == KErrNone && owner )
            {
            owner->SetCompletionStatus( EMMCScBkupOwnerDataTypeJavaData, ETrue );
            owner->SetCompletionStatus( EMMCScBkupOwnerDataTypePublicData, ETrue );
            owner->SetCompletionStatus( EMMCScBkupOwnerDataTypeSystemData, ETrue );
            owner->SetCompletionStatus( EMMCScBkupOwnerDataTypeActiveData, ETrue );
            owner->SetCompletionStatus( EMMCScBkupOwnerDataTypePassiveData, ETrue );

            __LOGFILE2("CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousErrorCleanupRestore() - **** - error: %d, SID: 0x%08x - all data types skipped for DO", aError, owner->SecureId().iId );
            }
        }
    
    CompleteSelf();
    return ETrue;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousCancellation()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpSystemData::PerformAsynchronousCancellation()
    {
    if  (iBackupTransferObject)
        {
        iBackupTransferObject->Cancel();
        }
    if  (iRestoreTransferObject)
        {
        iRestoreTransferObject->Cancel();
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::PerformLastRightsBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpSystemData::PerformLastRightsBackupL( TBool /*aPartial*/ )
    {
    CMMCScBkupIndexBase& index = Driver().DrvArchive().Footer().IndexByType( EMMCScBkupOwnerDataTypeSystemData );
    index.StoreL( Driver() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::PerformLastRightsRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpSystemData::PerformLastRightsRestoreL( TBool /*aPartial*/ )
    {
    Driver().DrvSecureBackupClient().AllSystemFilesRestored();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpSystemData::AddIndexRecordL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpSystemData::AddIndexRecordL( CMMCScBkupArchiveFooter& aFooter, CMMCScBkupDataOwnerInfo& aDataOwner, const TMMCScBkupArchiveVector& aInfo, TDriveNumber aDrive )
    {
    CMMCScBkupIndexBase& indexBase = aFooter.IndexByType( EMMCScBkupOwnerDataTypeSystemData );
    CMMCScBkupIndexWithIdentifier< TInt32 >& index = static_cast< CMMCScBkupIndexWithIdentifier< TInt32 >& >( indexBase );
    index.AddIndexRecordL( aInfo, aDrive, aDataOwner.SecureId().iId );
    }












