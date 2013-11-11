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
* Description: CMMCScBkupStateArchiveOpJavaData implementation
*
*
*/

#include "CMMCScBkupStateArchiveOpJavaData.h"

// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupDataOwnerCollection.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "MMCScBkupSBEUtils.h"
#include "CMMCScBkupArchive.h"
#include "CMMCScBkupArchiveFooter.h"
#include "CMMCScBkupIndexJavaData.h"
#include "MMMCScBkupArchiveDataInterface.h"



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::CMMCScBkupStateArchiveOpJavaData()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpJavaData::CMMCScBkupStateArchiveOpJavaData( MMMCScBkupDriver& aDriver )
:   CMMCScBkupStateOpAware( aDriver ), iIndexValueCurrent( -1 )
    {
    __LOG1("CMMCScBkupStateArchiveOpJavaData::CMMCScBkupStateArchiveOpJavaData() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::~CMMCScBkupStateArchiveOpJavaData()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpJavaData::~CMMCScBkupStateArchiveOpJavaData()
    {
    Cancel();
    //
    delete iBackupTransferObjectMidlet;
    delete iBackupTransferObjectMidletData;
    delete iRestoreTransferObjectMidlet;
    delete iRestoreTransferObjectMidletData;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpJavaData* CMMCScBkupStateArchiveOpJavaData::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateArchiveOpJavaData* self = new(ELeave) CMMCScBkupStateArchiveOpJavaData( aDriver );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpJavaData::StateId() const
    {
    return KMMCScBkupStateIdArchiveOpJavaData;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::NextStateBackupId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpJavaData::NextStateBackupId( TBool /*aPartial*/ ) const
    {
    return KMMCScBkupStateIdGetDataOwnerStatuses;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::NextStateRestoreId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpJavaData::NextStateRestoreId( TBool /*aPartial*/ ) const
    {
    return KMMCScBkupStateIdArchiveOpPassiveData;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::PerformStateInitBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpJavaData::PerformStateInitBackupL( TBool /*aPartial*/ )
    {
    __LOG("CMMCScBkupStateArchiveOpJavaData::PerformStateInitBackupL() - START" );
    __DEBUG_TESTING_SKIP_BACKUP_JAV_DATA;

    iBackupTransferObjectMidlet = CMMCScBkupWriteDataTransferRequest< TJavaTransferType >::NewL(
                                    Driver(), 
                                    *this,
                                    EMMCScBkupOwnerDataTypeJavaData,
                                    EJavaTransferDerivedType,
                                    EJavaMIDlet
                                    );

    iBackupTransferObjectMidletData = CMMCScBkupWriteDataTransferRequest< TJavaTransferType >::NewL(
                                    Driver(), 
                                    *this,
                                    EMMCScBkupOwnerDataTypePassiveData,
                                    EJavaTransferDerivedType,
                                    EJavaMIDletData
                                    );

    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::PerformStateInitRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpJavaData::PerformStateInitRestoreL( TBool /*aPartial*/ )
    {
    __LOG("CMMCScBkupStateArchiveOpJavaData::PerformStateInitRestoreL() - START" );
    __DEBUG_TESTING_SKIP_RESTORE_JAV_DATA;


    iRestoreTransferObjectMidlet = CMMCScBkupReadDataTransferRequest< TJavaTransferType >::NewL(
                                    Driver(), 
                                    EMMCScBkupOwnerDataTypeJavaData,
                                    EJavaMIDlet
                                    );

    iRestoreTransferObjectMidletData = CMMCScBkupReadDataTransferRequest< TJavaTransferType >::NewL(
                                    Driver(), 
                                    EMMCScBkupOwnerDataTypePassiveData,
                                    EJavaMIDletData
                                    );

    CompleteSelf();
    __LOG("CMMCScBkupStateArchiveOpJavaData::PerformStateInitRestoreL() - END" );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousStateStepBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousStateStepBackupL( TBool /*aPartial*/ )
    {
    __LOG1("CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousStateStepBackupL() - START - iCurrentJavaState: %d", iCurrentJavaState );

    switch( iCurrentJavaState )
        {
    case EJavaStateIdle:
        __LOG("CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousStateStepBackupL() - EJavaStateIdle");
        BackupMidletL();
        break;
    case EJavaStateMidlet:
        __LOG("CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousStateStepBackupL() - EJavaStateMidlet");
        BackupMidletDataL();
        break;
    case EJavaStateMidletData:
        __LOG("CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousStateStepBackupL() - EJavaStateMidletData");
        iCurrentJavaState = EJavaStateIdle;
        CompleteSelf();
        break;
    default:
        break;
        }

    __LOG2("CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousStateStepBackupL() - END - iCurrentJavaState: %d, IsActive: %d", iCurrentJavaState, IsActive() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousStateStepRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousStateStepRestoreL( TBool /*aPartial*/ )
    {
    __LOG1("CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousStateStepRestoreL() - START - iCurrentJavaState: %d", iCurrentJavaState );

    switch( iCurrentJavaState )
        {
    case EJavaStateIdle:
        __LOG("CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousStateStepRestoreL() - EJavaStateIdle");
        RestoreMidletL();
        break;
    case EJavaStateMidlet:
        __LOG("CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousStateStepRestoreL() - EJavaStateMidlet");
        RestoreMidletDataL();
        break;
    case EJavaStateMidletData:
        __LOG("CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousStateStepRestoreL() - EJavaStateMidletData");
        iCurrentJavaState = EJavaStateIdle;
        CompleteSelf();
        break;
    default:
        break;
        }

    __LOG2("CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousStateStepRestoreL() - END - iCurrentJavaState: %d, IsActive: %d", iCurrentJavaState, IsActive() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousErrorCleanupBackup()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousErrorCleanupBackup( TBool /*aPartial*/, TInt aError )
    {
    (void) aError;
    __LOG1("CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousErrorCleanupBackup() - error: %d", aError );

    // Reset state and move to next item. Treat the current (badly behaving) item
    // as completed/processed.
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    if  ( iIndexValueCurrent < dataOwners.Count() )
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner( iIndexValueCurrent );
        owner.SetCompletionStatus( EMMCScBkupOwnerDataTypeJavaData, ETrue );
        iCurrentJavaState = EJavaStateIdle;

        __LOGFILE2("CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousErrorCleanupBackup() - **** - error: %d, SID: 0x%08x - JAVA data skipped for DO", aError, owner.SecureId().iId );
        }

    CompleteSelf();
    return ETrue;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousErrorCleanupRestore()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousErrorCleanupRestore( TBool /*aPartial*/, TInt aError )
    {
    (void) aError;
    __LOGFILE1("CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousErrorCleanupRestore() - **** - error: %d", aError );

    iCurrentJavaState = EJavaStateIdle;
    CompleteSelf();
    return ETrue;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousCancellation()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpJavaData::PerformAsynchronousCancellation()
    {
    if  (iBackupTransferObjectMidlet)
        {
        iBackupTransferObjectMidlet->Cancel();
        }
    if  (iBackupTransferObjectMidletData)
        {
        iBackupTransferObjectMidletData->Cancel();
        }
    if  (iRestoreTransferObjectMidlet)
        {
        iRestoreTransferObjectMidlet->Cancel();
        }
    if  (iRestoreTransferObjectMidletData)
        {
        iRestoreTransferObjectMidletData->Cancel();
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::PerformLastRightsBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpJavaData::PerformLastRightsBackupL( TBool /*aPartial*/ )
    {
    CMMCScBkupIndexBase& index = Driver().DrvArchive().Footer().IndexByType( EMMCScBkupOwnerDataTypeJavaData );
    index.StoreL( Driver() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::PerformLastRightsRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpJavaData::PerformLastRightsRestoreL( TBool /*aPartial*/ )
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::AddIndexRecordL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpJavaData::AddIndexRecordL( CMMCScBkupArchiveFooter& aFooter, CMMCScBkupDataOwnerInfo& aDataOwner, const TMMCScBkupArchiveVector& aInfo, TDriveNumber aDrive )
    {
	CMMCScBkupIndexBase& indexBase = aFooter.IndexByType( EMMCScBkupOwnerDataTypeJavaData );
    CMMCScBkupIndexJavaData& index = static_cast< CMMCScBkupIndexJavaData& >( indexBase );
    //
    TJavaTransferType type = EJavaMIDlet;
    HBufC* hash = MMCScBkupSBEUtils::JavaHashFromGenericLC( aDataOwner.Owner().Identifier() );
    //
    if  ( iCurrentJavaState == EJavaStateMidlet )
        {
		__LOG4("CMMCScBkupStateArchiveOpJavaData::AddIndexRecordL() - hash: %S, aDrive: %c:, offset: %8d, length: %8d, type: EJavaMIDlet", hash, aDrive + 'A', aInfo.Offset(), aInfo.Length() );
        type = EJavaMIDlet;
        }
    else if ( iCurrentJavaState == EJavaStateMidletData )
        {
		__LOG4("CMMCScBkupStateArchiveOpJavaData::AddIndexRecordL() - hash: %S, aDrive: %c:, offset: %8d, length: %8d, type: EJavaMIDletData", hash, aDrive + 'A', aInfo.Offset(), aInfo.Length() );
        type = EJavaMIDletData;
        }
    //
    index.AddIndexRecordL( aInfo, *hash, aDrive, type );
    CleanupStack::PopAndDestroy( hash );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::BackupMidletL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpJavaData::BackupMidletL()
    {
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    const TInt count = dataOwners.Count();
    //
    while ( ++iIndexValueCurrent < count )
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner( iIndexValueCurrent );
        //
        if  ( owner.CompletionStatus( EMMCScBkupOwnerDataTypeJavaData ) == EFalse )
            {
#ifdef __MMCSCBKUPLOGGING_ENABLED__
			HBufC* hash = MMCScBkupSBEUtils::JavaHashFromGenericLC( owner.Owner().Identifier() );
			__LOG1("CMMCScBkupStateArchiveOpJavaData::BackupMidletL() - trying to get midlet for hash: %S", hash );
			CleanupStack::PopAndDestroy( hash );
#endif

			iBackupTransferObjectMidlet->RequestL( owner, 
                                                   iStatus,
                                                   Driver().DrvParamsBase().DriveAndOperations() );
            SetActive();
            iCurrentJavaState = EJavaStateMidlet;

            break; // while loop
            }
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::BackupMidletDataL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpJavaData::BackupMidletDataL()
    {
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner( iIndexValueCurrent );
 
    // We only need to request the midlet's passive data if it has a non-zero
    // size.
    if  ( owner.CompletionStatus( EMMCScBkupOwnerDataTypePassiveData ) == EFalse )
        {
        if  ( owner.OperationalSize( EMMCScBkupOwnerDataTypePassiveData ) > 0 )
            {
#ifdef __MMCSCBKUPLOGGING_ENABLED__
			HBufC* hash = MMCScBkupSBEUtils::JavaHashFromGenericLC( owner.Owner().Identifier() );
			__LOG1("CMMCScBkupStateArchiveOpJavaData::BackupMidletDataL() - trying to get midlet data for hash: %S", hash );
			CleanupStack::PopAndDestroy( hash );
#endif

			iBackupTransferObjectMidletData->RequestL( owner, 
                                                       iStatus,
                                                       Driver().DrvParamsBase().DriveAndOperations() );
            SetActive();
            }
        else
            {
            CompleteSelf();
            }
        }
    else
        {
        CompleteSelf();
        }

    iCurrentJavaState = EJavaStateMidletData;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::RestoreMidletL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpJavaData::RestoreMidletL()
    {
    CMMCScBkupIndexBase& indexBase = Driver().DrvArchive().Footer().IndexByType( EMMCScBkupOwnerDataTypeJavaData );
    CMMCScBkupIndexJavaData& index = static_cast< CMMCScBkupIndexJavaData& >( indexBase );

    const TInt count = index.Count();
    __LOG1("CMMCScBkupStateArchiveOpJavaData::RestoreMidletL() - START - %d data owners...", count );
    //
    if  ( ++iIndexValueCurrent < count )
        {
        __LOG2("CMMCScBkupStateArchiveOpJavaData::RestoreMidletL() - getting java data index record [%03d/%03d]", iIndexValueCurrent, count);
        const CMMCScBkupIndexJavaDataEntry& entry = index.At( iIndexValueCurrent );

        // Get the associated data owner
        CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();

        __LOG1("CMMCScBkupStateArchiveOpJavaData::RestoreMidletL() - trying to get data owner info for hash: %S", &entry.Hash() );
        CMMCScBkupDataOwnerInfo& owner = dataOwners.OwnerL( entry.Hash() );

        if  ( owner.CompletionStatus( EMMCScBkupOwnerDataTypeJavaData ) == EFalse )
            {
            // Request all the java data (on all supported drives) for this
            // particular data owner
    		__LOG3("CMMCScBkupStateArchiveOpJavaData::RestoreMidletL() - trying to restore JAVA MIDLET for hash: %S [offset: %8d, length: %8d]", &entry.Hash(), entry.MidletInfo().Offset(), entry.MidletInfo().Length() );

            // Build restore entry list (will only contain one entry - the 
            // reader API forces an array interface on us, so let's do ou best
            // to please it...)
            const TMMCScBkupArchiveDriveAndVector driveAndVector( entry.Drive(),  entry.MidletInfo() );
            iRestoreEntries.Reset();
            iRestoreEntries.AppendL( driveAndVector );
            
            // Start the asynchronous restore op...
            iRestoreTransferObjectMidlet->RequestL( owner, 
                                                    iStatus,
                                                    iRestoreEntries );
            SetActive();
            }
        else
            {
            CompleteSelf();
            }
            
        iCurrentJavaState = EJavaStateMidlet;
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpJavaData::RestoreMidletDataL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpJavaData::RestoreMidletDataL()
    {
    CMMCScBkupIndexBase& indexBase = Driver().DrvArchive().Footer().IndexByType( EMMCScBkupOwnerDataTypeJavaData );
    CMMCScBkupIndexJavaData& index = static_cast< CMMCScBkupIndexJavaData& >( indexBase );

    const TInt count = index.Count();
    __LOG1("CMMCScBkupStateArchiveOpJavaData::RestoreMidletDataL() - START - %d data owners...", count );
    //
    if  ( iIndexValueCurrent < count )
        {
        __LOG2("CMMCScBkupStateArchiveOpJavaData::RestoreMidletDataL() - getting java data index record [%03d/%03d]", iIndexValueCurrent, count);
        const CMMCScBkupIndexJavaDataEntry& entry = index.At( iIndexValueCurrent );

        if  ( entry.HasMidletDataInfo() )
            {
            // Get the associated data owner
            CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();

            __LOG1("CMMCScBkupStateArchiveOpJavaData::RestoreMidletDataL() - trying to get data owner info for hash: %S", &entry.Hash() );
            CMMCScBkupDataOwnerInfo& owner = dataOwners.OwnerL( entry.Hash() );

            if  ( owner.CompletionStatus( EMMCScBkupOwnerDataTypePassiveData ) == EFalse )
                {
                // Request all the java data (on all supported drives) for this
                // particular data owner
    			__LOG3("CMMCScBkupStateArchiveOpJavaData::RestoreMidletDataL() - trying to restore JAVA MIDLET DATA for hash: %S [offset: %8d, length: %8d]", &entry.Hash(), entry.MidletDataInfo().Offset(), entry.MidletDataInfo().Length() );

                // Build restore entry list (will only contain one entry - the 
                // reader API forces an array interface on us, so let's do ou best
                // to please it...)
                const TMMCScBkupArchiveDriveAndVector driveAndVector( entry.Drive(),  entry.MidletDataInfo() );
                iRestoreEntries.Reset();
                iRestoreEntries.AppendL( driveAndVector );
            
                // Start the asynchronous restore op...
                iRestoreTransferObjectMidletData->RequestL( owner, 
                                                            iStatus,
                                                            iRestoreEntries );
                SetActive();
                }
            else
                {
                CompleteSelf();
                }
            }
        else
            {
            CompleteSelf();
            }
        }
    else
        {
        CompleteSelf();
        }

    iCurrentJavaState = EJavaStateMidletData;
    }









