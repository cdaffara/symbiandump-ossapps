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
* Description: CMMCScBkupStateArchiveOpPassiveData implementation
*
*
*/

#include "CMMCScBkupStateArchiveOpPassiveData.h"

// User includes
#include "MMCScBkupLogger.h"
#include "MMCScBkupSBEUtils.h"
#include "CMMCScBkupArchive.h"
#include "CMMCScBkupArchiveFooter.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "CMMCScBkupDataOwnerCollection.h"
#include "CMMCScBkupIndexWithIdentifier.h"
#include "MMMCScBkupArchiveDataInterface.h"



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPassiveData::CMMCScBkupStateArchiveOpPassiveData()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpPassiveData::CMMCScBkupStateArchiveOpPassiveData( MMMCScBkupDriver& aDriver )
:   CMMCScBkupStateOpAware( aDriver ), iIndexValueCurrent( -1 )
    {
    __LOG1("CMMCScBkupStateArchiveOpPassiveData::CMMCScBkupStateArchiveOpPassiveData() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPassiveData::~CMMCScBkupStateArchiveOpPassiveData()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpPassiveData::~CMMCScBkupStateArchiveOpPassiveData()
    {
    Cancel();
    //
    delete iBackupTransferObject;
    delete iRestoreTransferObject;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPassiveData::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpPassiveData* CMMCScBkupStateArchiveOpPassiveData::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateArchiveOpPassiveData* self = new(ELeave) CMMCScBkupStateArchiveOpPassiveData( aDriver );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPassiveData::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpPassiveData::StateId() const
    {
    return KMMCScBkupStateIdArchiveOpPassiveData;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPassiveData::NextStateBackupId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpPassiveData::NextStateBackupId( TBool /*aPartial*/ ) const
    {
    return KMMCScBkupStateIdArchiveOpDataOwners;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPassiveData::NextStateRestoreId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpPassiveData::NextStateRestoreId( TBool /*aPartial*/ ) const
    {
    return KMMCScBkupStateIdGetDataOwnerStatuses;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPassiveData::PerformStateInitBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPassiveData::PerformStateInitBackupL( TBool /*aPartial*/ )
    {
    __DEBUG_TESTING_SKIP_BACKUP_PAS_DATA;

    iBackupTransferObject = CMMCScBkupWriteDataTransferRequest<TTransferDataType>::NewL(
                                    Driver(), 
                                    *this,
                                    EMMCScBkupOwnerDataTypePassiveData,
                                    ESIDTransferDerivedType,
                                    Driver().DrvParamsBase().PassiveTransferType()
                                    );
    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPassiveData::PerformStateInitRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPassiveData::PerformStateInitRestoreL( TBool /*aPartial*/ )
    {
    __DEBUG_TESTING_SKIP_RESTORE_PAS_DATA;

    iRestoreTransferObject = CMMCScBkupReadDataTransferRequest< TTransferDataType >::NewL(
                                    Driver(),
                                    EMMCScBkupOwnerDataTypePassiveData,
                                    Driver().DrvParamsBase().PassiveTransferType()
                                    );

    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousStateStepBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousStateStepBackupL( TBool /*aPartial*/ )
    {
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    const TInt count = dataOwners.Count();
    //
    while ( ++iIndexValueCurrent < count )
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner( iIndexValueCurrent );
        //
        if  ( owner.CompletionStatus( EMMCScBkupOwnerDataTypePassiveData ) == EFalse )
            {
            __LOG1("CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousStateStepBackupL() - trying to backup DO 0x%08x", owner.SecureId().iId );

            // Request all the passive data (on all supported drives) for this
            // particular data owner
            __ASSERT_ALWAYS( iBackupTransferObject->IsActive() == EFalse, User::Invariant() );
            //
            iBackupTransferObject->RequestL( owner, 
                                             iStatus,
                                             Driver().DrvParamsBase().DriveAndOperations() );
            SetActive();
            break; // while loop
            }
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousStateStepRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousStateStepRestoreL( TBool /*aPartial*/ )
    {
    CMMCScBkupIndexBase& indexBase = Driver().DrvArchive().Footer().IndexByType( EMMCScBkupOwnerDataTypePassiveData );
    CMMCScBkupIndexWithIdentifier< TSecureId>& index = static_cast< CMMCScBkupIndexWithIdentifier<TSecureId>& >( indexBase );
    const TInt count = index.Count();
    //
    if  ( ++iIndexValueCurrent < count )
        {
        const CMMCScBkupIndexEntry<TSecureId>& entry = index.At( iIndexValueCurrent );
        const RArray<TMMCScBkupArchiveDriveAndVector>& subEntries = entry.Entries();

        // Get the associated data owner
        CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
        CMMCScBkupDataOwnerInfo& owner = dataOwners.OwnerL( entry.Identifier() );

        if  ( owner.CompletionStatus( EMMCScBkupOwnerDataTypePassiveData ) == EFalse )
            {
            __LOG1("CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousStateStepRestoreL() - trying to restore DO 0x%08x", owner.SecureId().iId );


            // Request all the registration data (on all supported drives) for this
            // particular data owner
            __ASSERT_ALWAYS( iRestoreTransferObject->IsActive() == EFalse, User::Invariant() );
            //
            __LOG1("CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousStateStepRestoreL() - trying to restore PASSIVE DATA for SID: 0x%08x", owner.SecureId().iId );
            //
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
// CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousErrorCleanupBackup()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousErrorCleanupBackup( TBool /*aPartial*/, TInt aError )
    {
    (void) aError;
    __LOG1("CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousErrorCleanupBackup() - **** - error: %d", aError );

    // Reset state and move to next item. Treat the current (badly behaving) item
    // as completed/processed.
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    if  ( iIndexValueCurrent < dataOwners.Count() )
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner( iIndexValueCurrent );
        owner.SetCompletionStatus( EMMCScBkupOwnerDataTypePassiveData, ETrue );

        __LOGFILE1("CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousErrorCleanupBackup() - **** - PASSIVE data skipped for SID: 0x%08x", owner.SecureId().iId );
        }

    CompleteSelf();
    return ETrue;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousErrorCleanupRestore()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousErrorCleanupRestore( TBool /*aPartial*/, TInt aError )
    {
    (void) aError;
    __LOGFILE1("CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousErrorCleanupRestore() - **** - error: %d", aError );

    CompleteSelf();
    return ETrue;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousCancellation()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPassiveData::PerformAsynchronousCancellation()
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
// CMMCScBkupStateArchiveOpPassiveData::PerformLastRightsBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPassiveData::PerformLastRightsBackupL( TBool /*aPartial*/ )
    {
    CMMCScBkupIndexBase& index = Driver().DrvArchive().Footer().IndexByType(EMMCScBkupOwnerDataTypePassiveData);
    index.StoreL(Driver());
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpPassiveData::AddIndexRecordL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpPassiveData::AddIndexRecordL( CMMCScBkupArchiveFooter& aFooter, CMMCScBkupDataOwnerInfo& aDataOwner, const TMMCScBkupArchiveVector& aInfo, TDriveNumber aDrive )
    {
    CMMCScBkupIndexBase& indexBase = aFooter.IndexByType( EMMCScBkupOwnerDataTypePassiveData );
    CMMCScBkupIndexWithIdentifier< TSecureId>& index = static_cast< CMMCScBkupIndexWithIdentifier< TSecureId>& >( indexBase );
    //
    index.AddIndexRecordL( aInfo, aDrive, aDataOwner.SecureId() );
    __LOG3("CMMCScBkupStateArchiveOpPassiveData::AddIndexRecordL() - offset: %6d, length: %6d, secureId: 0x%08x", aInfo.Offset(), aInfo.Length(), aDataOwner.SecureId().iId );
    }





