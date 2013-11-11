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
* Description: CMMCScBkupStateArchiveOpActiveData implementation
*
*
*/

#include "CMMCScBkupStateArchiveOpActiveData.h"

// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupDataOwnerCollection.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "MMCScBkupSBEUtils.h"
#include "CMMCScBkupArchive.h"
#include "CMMCScBkupArchiveFooter.h"
#include "CMMCScBkupIndexWithIdentifier.h"
#include "MMMCScBkupArchiveDataInterface.h"

// Constants
const TInt KMMCScBkupActiveDataOwnerDelayBeforeRetry = 2 * 1000000; // 2 seconds
const TInt KMMCScBkupNumberOfAttemptsToAllowADOToBecomeReady = 4; // corresponds to 8 seconds of waiting



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::CMMCScBkupStateArchiveOpActiveData()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpActiveData::CMMCScBkupStateArchiveOpActiveData( MMMCScBkupDriver& aDriver )
:   CMMCScBkupStateOpAware( aDriver ), iIndexValueCurrent( -1 )
    {
    __LOG1("CMMCScBkupStateArchiveOpActiveData::CMMCScBkupStateArchiveOpActiveData() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::~CMMCScBkupStateArchiveOpActiveData()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpActiveData::~CMMCScBkupStateArchiveOpActiveData()
    {
    Cancel();
    //
    iTimer.Close();
    //
    delete iBackupTransferObject;
    delete iRestoreTransferObject;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpActiveData* CMMCScBkupStateArchiveOpActiveData::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateArchiveOpActiveData* self = new(ELeave) CMMCScBkupStateArchiveOpActiveData( aDriver );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpActiveData::StateId() const
    {
    return KMMCScBkupStateIdArchiveOpActiveData;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::NextStateBackupId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpActiveData::NextStateBackupId( TBool /*aPartial*/ ) const
    {
    // If we still have unprocessed active data owners remaining, then
    // we must
    //
    // a) check their status again - to see if they are ready, then
    // b) process them once again via this method
    //
    // The 'get data owner statuses' object will trigger us again
    // when it has performed an update of all statuses

    // By default, we want to move onto the passive data...
    TMMCScBkupStateId nextState = KMMCScBkupStateIdArchiveOpPassiveData;
    if  (AllDataOwnersHandled() == EFalse)
        {
        // ... but we've still got at least one data owner who's not yet
        // finished preparing their data... so we have to retry
        nextState = KMMCScBkupStateIdGetDataOwnerStatuses;
        }
    //
    return nextState;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::NextStateRestoreId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpActiveData::NextStateRestoreId( TBool /*aPartial*/ ) const
    {
    // If we still have unprocessed active data owners remaining, then
    // we must
    //
    // a) check their status again - to see if they are ready, then
    // b) process them once again via this method
    //
    // The 'get data owner statuses' object will trigger us again
    // when it has performed an update of all statuses

    // By default, we want to move onto the passive data...
    TMMCScBkupStateId nextState = KMMCScBkupStateIdArchiveOpPublicDataFiles;
    if  (AllDataOwnersHandled() == EFalse)
        {
        // ... but we've still got at least one data owner who's not yet
        // finished preparing their data... so we have to retry
        nextState = KMMCScBkupStateIdGetDataOwnerStatuses;
        }
    //
    return nextState;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::PerformStateInitBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpActiveData::PerformStateInitBackupL( TBool /*aPartial*/ )
    {
    __DEBUG_TESTING_SKIP_BACKUP_ACT_DATA;
    __LOG("CMMCScBkupStateArchiveOpActiveData::PerformStateInitBackupL() - START");

    iBackupTransferObject = CMMCScBkupWriteDataTransferRequest< TTransferDataType >::NewL(
                                    Driver(), 
                                    *this,
                                    EMMCScBkupOwnerDataTypeActiveData,
                                    ESIDTransferDerivedType,
                                    Driver().DrvParamsBase().ActiveTransferType()
                                    );
    CompleteSelf();
    __LOG("CMMCScBkupStateArchiveOpActiveData::PerformStateInitBackupL() - END");
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::PerformStateInitRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpActiveData::PerformStateInitRestoreL( TBool /*aPartial*/ )
    {
    __DEBUG_TESTING_SKIP_RESTORE_ACT_DATA;
    __LOG("CMMCScBkupStateArchiveOpActiveData::PerformStateInitRestoreL() - START");

    iRestoreTransferObject = CMMCScBkupReadDataTransferRequest< TTransferDataType >::NewL(
                                    Driver(),
                                    EMMCScBkupOwnerDataTypeActiveData,
                                    Driver().DrvParamsBase().ActiveTransferType()
                                    );

    CompleteSelf();
    __LOG("CMMCScBkupStateArchiveOpActiveData::PerformStateInitRestoreL() - END");
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousStateStepBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousStateStepBackupL( TBool /*aPartial*/ )
    {
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    const TInt count = dataOwners.Count();
    __LOG2("CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousStateStepBackupL() - START - iIndexValueCurrent: %d/%d", iIndexValueCurrent, count);
    //
    while ( ++iIndexValueCurrent < count )
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner(iIndexValueCurrent);

        // Has this data owner been processed?
        // Is the data owner even ready?
        const TBool activeDataAlreadyProcessed = owner.CompletionStatus( EMMCScBkupOwnerDataTypeActiveData );
        const TBool dataOwnerIsReady = ( owner.Status() == EDataOwnerReady || 
                                         owner.Status() == EDataOwnerReadyNoImpl );
        const TBool dataHasRealActiveData = ( owner.Status() == EDataOwnerReady );
        //
        __LOG5("CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousStateStepBackupL() - DO: 0x%08x has status: %d, alreadyProcessed: %d, isReady: %d, dataHasRealActiveData: %d", owner.SecureId().iId, owner.Status(), activeDataAlreadyProcessed, dataOwnerIsReady, dataHasRealActiveData);
        //
        if  (!activeDataAlreadyProcessed)
            {
            __LOG("CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousStateStepBackupL() - active data has not already been processed...");
            
            if  (dataOwnerIsReady)
                {
                __LOG("CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousStateStepBackupL() - data owner is ready...");
                
                if  ( dataHasRealActiveData )
                    {
                    __LOG("CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousStateStepBackupL() - data owner has real active data...");

                    // Try and obtain the active data for this client
                    __ASSERT_ALWAYS(iBackupTransferObject->IsActive() == EFalse, User::Invariant());
                    //
                    iBackupTransferObject->RequestL( owner, 
                                                    iStatus,
                                                    Driver().DrvParamsBase().DriveAndOperations() );
                    SetActive();
                    break; // while loop
                    }
                else
                    {
                    // The data owner is a hybrid - we shouldn't request data for a data owner that 
                    // doesn't provide an active data interface callback implementation.
                    __LOG("CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousStateStepBackupL() - DO is a hybrid - set completion status to ETrue");
                    owner.SetCompletionStatus( EMMCScBkupOwnerDataTypeActiveData, ETrue );
                    }
                }
            else
                {
                // Retry a number of times, but eventually we'll give up
                const TInt attemptsSoFar = owner.ActiveDataRetryCount();
                owner.SetActiveDataRetryCount( attemptsSoFar + 1 );

                if  ( attemptsSoFar >= KMMCScBkupNumberOfAttemptsToAllowADOToBecomeReady )
                    {
                    // Give up, this DO is taking too long
                    owner.SetCompletionStatus( EMMCScBkupOwnerDataTypeActiveData, ETrue );
                    __LOG1("CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousStateStepBackupL() - ADO with SID: 0x%08x has taken too long to prepare, skipping", owner.SecureId().iId);
                    }
                else
                    {
                    // DO is not yet ready, we're going to have to check its
                    // status again in a while...
                    __LOG1("CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousStateStepBackupL() - attemptsSoFar %d", attemptsSoFar);
                    iAtLeastOneDataOwnerIsNotYetReady = ETrue;
                    }
                }
            }
        }
    //
    CheckNeedToStartRetryTimerL();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousStateStepRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousStateStepRestoreL( TBool /*aPartial*/ )
    {
    CMMCScBkupIndexBase& indexBase = Driver().DrvArchive().Footer().IndexByType( EMMCScBkupOwnerDataTypeActiveData );
    CMMCScBkupIndexWithIdentifier< TSecureId >& index = static_cast< CMMCScBkupIndexWithIdentifier< TSecureId >& >( indexBase );
    const TInt count = index.Count();
    //
    //
    while ( ++iIndexValueCurrent < count )
        {
         // Get the associated data owner
        const CMMCScBkupIndexEntry<TSecureId>& entry = index.At( iIndexValueCurrent );
        CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
        CMMCScBkupDataOwnerInfo& owner = dataOwners.OwnerL( entry.Identifier() );

        // Has this data owner been processed?
        // Is the data owner even ready?
        const TBool activeDataAlreadyProcessed = owner.CompletionStatus(EMMCScBkupOwnerDataTypeActiveData);
        const TBool dataOwnerIsReady = ( owner.Status() == EDataOwnerReady || 
                                         owner.Status() == EDataOwnerReadyNoImpl );
        //
        if  (!activeDataAlreadyProcessed)
            {
            if  (dataOwnerIsReady)
                {
                __ASSERT_ALWAYS(iRestoreTransferObject->IsActive() == EFalse, User::Invariant());
                //
                const RArray<TMMCScBkupArchiveDriveAndVector>& subEntries = entry.Entries();
                iRestoreTransferObject->RequestL( owner, 
                                                iStatus,
                                                subEntries );
                __LOG1("CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousStateStepRestoreL() - trying to restore ACTIVE DATA for SID: 0x%08x", owner.SecureId().iId );
                //
                SetActive();
                break; // while loop
                }
            else
                {
                // Retry a number of times, but eventually we'll give up
                const TInt attemptsSoFar = owner.ActiveDataRetryCount();
                owner.SetActiveDataRetryCount( attemptsSoFar + 1 );

                if  ( attemptsSoFar >= KMMCScBkupNumberOfAttemptsToAllowADOToBecomeReady )
                    {
                    // Give up, this DO is taking too long
                    owner.SetCompletionStatus( EMMCScBkupOwnerDataTypeActiveData, ETrue );
                    __LOG1("CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousStateStepRestoreL() - ADO with SID: 0x%08x has taken too long to prepare, skipping", owner.SecureId().iId);
                    }
                else
                    {
                    // DO is not yet ready, we're going to have to check its
                    // status again in a while...
                    iAtLeastOneDataOwnerIsNotYetReady = ETrue;
                    }
                }
            }
        }
    //
    CheckNeedToStartRetryTimerL();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousErrorCleanupBackup()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousErrorCleanupBackup( TBool /*aPartial*/, TInt aError )
    {
    (void) aError;
    __LOG1("CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousErrorCleanupBackup() - **** - error: %d", aError );

    // Reset state and move to next item. Treat the current (badly behaving) item
    // as completed/processed.
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    if  ( iIndexValueCurrent < dataOwners.Count() )
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner( iIndexValueCurrent );
        owner.SetCompletionStatus( EMMCScBkupOwnerDataTypeActiveData, ETrue );

        __LOGFILE2("CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousErrorCleanupBackup() - **** - error: %d, SID: 0x%08x - ACTIVE data skipped for DO", aError, owner.SecureId().iId );
        }

    CompleteSelf();
    return ETrue;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousErrorCleanupRestore()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousErrorCleanupRestore( TBool /*aPartial*/, TInt aError )
    {
    (void) aError;
    __LOGFILE1("CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousErrorCleanupRestore() - **** - error: %d", aError );

    CompleteSelf();
    return ETrue;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousCancellation()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpActiveData::PerformAsynchronousCancellation()
    {
    if  (iTimer.Handle() != KNullHandle )
        {
        iTimer.Cancel();
        }
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
// CMMCScBkupStateArchiveOpActiveData::PerformLastRightsBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpActiveData::PerformLastRightsBackupL( TBool /*aPartial*/ )
    {
    __LOG1("CMMCScBkupStateArchiveOpActiveData::PerformLastRightsBackupL() - iAtLeastOneDataOwnerIsNotYetReady %d", iAtLeastOneDataOwnerIsNotYetReady );
    
    if  ( AllDataOwnersHandled() )
        {
        // Only write the index if we've handled all the active data owners
        CMMCScBkupIndexBase& index = Driver().DrvArchive().Footer().IndexByType( EMMCScBkupOwnerDataTypeActiveData );
        index.StoreL(Driver());
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::AddIndexRecordL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpActiveData::AddIndexRecordL( CMMCScBkupArchiveFooter& aFooter, CMMCScBkupDataOwnerInfo& aDataOwner, const TMMCScBkupArchiveVector& aInfo, TDriveNumber aDrive )
    {
    CMMCScBkupIndexBase& indexBase = aFooter.IndexByType( EMMCScBkupOwnerDataTypeActiveData );
    CMMCScBkupIndexWithIdentifier< TSecureId>& index = static_cast< CMMCScBkupIndexWithIdentifier< TSecureId>& >( indexBase );
    //
    index.AddIndexRecordL( aInfo, aDrive, aDataOwner.SecureId() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpActiveData::CheckNeedToStartRetryTimerL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpActiveData::CheckNeedToStartRetryTimerL()
    {
    // We need to delay until the remaining active data owners are ready
    const TBool processingDataHandler = IsActive();
    if  ( !processingDataHandler && !AllDataOwnersHandled() && iTimer.Handle() == KNullHandle )
        {
        // Implicitly, we have at least one DO that's not yet ready, and 
        // we have processed as many of the ready DO's as is possible on this
        // cycle. Let's wait a few seconds to give the DO's a time to finish
        // preparing and then try again. See NextStateId() for how this works.
        User::LeaveIfError( iTimer.CreateLocal() );

        __LOG("CMMCScBkupStateArchiveOpActiveData::CheckNeedToStartRetryTimerL() - SetActive");
        SetActive();
        const TTimeIntervalMicroSeconds32 delay(KMMCScBkupActiveDataOwnerDelayBeforeRetry);
        iTimer.After( iStatus, delay );
        }
    }













