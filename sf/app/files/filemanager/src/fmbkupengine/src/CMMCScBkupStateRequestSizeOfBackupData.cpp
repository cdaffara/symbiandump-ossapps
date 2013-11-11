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
* Description: CMMCScBkupStateRequestSizeOfBackupData implementation
*
*
*/

#include "CMMCScBkupStateRequestSizeOfBackupData.h"

// System includes
#include <e32std.h>

// User includes
#include "MMCScBkupLogger.h"
#include "MMCScBkupSBEUtils.h"
#include "TMMCScBkupOwnerDataType.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "MMMCScBkupProgressObserver.h"
#include "CMMCScBkupDataOwnerCollection.h"
#include "CMMCScBkupDriveAndOperationTypeManager.h"


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfBackupData::CMMCScBkupStateRequestSizeOfBackupData()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateRequestSizeOfBackupData::CMMCScBkupStateRequestSizeOfBackupData( MMMCScBkupDriver& aDriver )
:   CMMCScBkupState( aDriver )
    {
    __LOG1("CMMCScBkupStateRequestSizeOfBackupData::CMMCScBkupStateRequestSizeOfBackupData() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfBackupData::~CMMCScBkupStateRequestSizeOfBackupData()
// 
// C++ destructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateRequestSizeOfBackupData::~CMMCScBkupStateRequestSizeOfBackupData()
    {
    Cancel();
    //
    delete iRequestObject;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfBackupData::ConstructL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestSizeOfBackupData::ConstructL()
    {
    iRequestObject = CMMCScBkupStateRequestSizeOfDataOwner::NewL( Driver() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfBackupData::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateRequestSizeOfBackupData* CMMCScBkupStateRequestSizeOfBackupData::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateRequestSizeOfBackupData* self = new(ELeave) CMMCScBkupStateRequestSizeOfBackupData( aDriver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfBackupData::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateRequestSizeOfBackupData::StateId() const
    {
    return KMMCScBkupStateIdRequestSizeOfBackupData;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfBackupData::NextStateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateRequestSizeOfBackupData::NextStateId() const
    {
    return KMMCScBkupStateIdRequestListOfPublicFiles;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfBackupData::PerformStateInitL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestSizeOfBackupData::PerformStateInitL()
    {
    // Set to -1 so that the first time RunL is called, we start with
    // the data owner at index 0.
    iCurrentDataOwnerIndex = -1;
    CompleteSelf(KErrNone);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfBackupData::PerformAsynchronousStateStepL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestSizeOfBackupData::PerformAsynchronousStateStepL()
    {
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    const TInt count = dataOwners.Count();
    //
    if  ( ++iCurrentDataOwnerIndex < dataOwners.Count() )
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner(iCurrentDataOwnerIndex);

        // Request all the public files (on all supported drives) for this
        // particular data owner
        __ASSERT_ALWAYS(iRequestObject->IsActive() == EFalse, User::Invariant());
        //
        __LOG3("CMMCScBkupStateRequestSizeOfBackupData::PerformAsynchronousStateStepL() - START - owner[%3d/%3d], sid: 0x%08x", iCurrentDataOwnerIndex, count, owner.SecureId().iId);
        iRequestObject->RequestL(owner, iStatus);
        SetActive();
        }
    else
        {
        __LOG("CMMCScBkupStateRequestSizeOfBackupData::PerformAsynchronousStateStepL() - END");
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfBackupData::PerformAsynchronousErrorCleanup()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateRequestSizeOfBackupData::PerformAsynchronousErrorCleanup( TInt aError )
    {
    (void) aError;
    __LOG1("CMMCScBkupStateRequestSizeOfBackupData::PerformAsynchronousErrorCleanup() - **** - error: %d", aError );
    TBool continueProcessing = ETrue;

    // Cancel requester. Its probably already inactive, but just in case...
    PerformAsynchronousCancellation();

    // Reset state and move to next item. Treat the current (badly behaving) item
    // as completed/processed.
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    if  ( iCurrentDataOwnerIndex < dataOwners.Count() )
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner( iCurrentDataOwnerIndex );

        (void) owner;
        __LOGFILE2("CMMCScBkupStateRequestSizeOfBackupData::PerformAsynchronousErrorCleanup() - **** - error: %d, SID: 0x%08x - SIZE OF DATA skipped for DO", aError, owner.SecureId().iId );

        // Ensures that asynchronous state step is called again..
        CompleteSelf();
        }
    else
        {
        continueProcessing = EFalse;
        }
    //
    __LOG1("CMMCScBkupStateRequestSizeOfBackupData::PerformAsynchronousErrorCleanup() - END - continueProcessing: %d", continueProcessing );
    return continueProcessing;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfBackupData::PerformAsynchronousCancellation()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestSizeOfBackupData::PerformAsynchronousCancellation()
    {
    if  ( iRequestObject )
        {
        iRequestObject->Cancel();
        }
    }































// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfDataOwner::CMMCScBkupStateRequestSizeOfDataOwner()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateRequestSizeOfDataOwner::CMMCScBkupStateRequestSizeOfDataOwner( MMMCScBkupDriver& aDriver )
:   CMMCScBkupDriveSpecificRequest( aDriver.DrvParamsBase().DriveAndOperations(), EMMCScBkupOwnerDataTypeAny ), iDriver(aDriver)
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfDataOwner::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateRequestSizeOfDataOwner* CMMCScBkupStateRequestSizeOfDataOwner::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateRequestSizeOfDataOwner* self = new (ELeave) CMMCScBkupStateRequestSizeOfDataOwner(aDriver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfDataOwner::RequestL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestSizeOfDataOwner::RequestL( CMMCScBkupDataOwnerInfo& aOwner, TRequestStatus& aObserver )
    {
    iOwner = &aOwner;
    CMMCScBkupDriveSpecificRequest::RequestL( aObserver );

    // Start the asynchronous request. 
    CompleteSelf(KErrNone);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfDataOwner::RunL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestSizeOfDataOwner::RunL()
    {
    __LOG2("CMMCScBkupStateRequestSizeOfDataOwner::RunL() - START - iStatus: %d, sid: 0x%08x", iStatus.Int(), iOwner->SecureId().iId);

    User::LeaveIfError(iStatus.Int());
    //
    TDriveNumber drive = EDriveA;
    const TDriveList& ownerDrives = iOwner->Owner().DriveList();
    const TBool driveAvailable = NextValidDrive(drive, ownerDrives);
    //
    if  (driveAvailable)
        {
        CDataOwnerInfo& owner = iOwner->Owner();
        CSBGenericDataType& identifier = owner.Identifier();

        // Size any active and passive data
        GetActiveAndPassiveDataSizesL( owner, identifier, drive );

        // Size any system package data
        GetSystemDataSizesL( owner, identifier, drive );

        // Size any java data
        GetJavaDataSizesL( owner, identifier, drive );

        // Complete ourself to try the next drive
        CompleteSelf(KErrNone);
        __LOG1("CMMCScBkupStateRequestSizeOfDataOwner::RunL() - END - sid: 0x%08x, checking next drive...", iOwner->SecureId().iId);
        }
    else
        {
        __LOG1("CMMCScBkupStateRequestSizeOfDataOwner::RunL() - END - sid: 0x%08x, last drive scanned => notifying observer", iOwner->SecureId().iId);
        CompleteObserverRequest(KErrNone);
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfDataOwner::DoCancel()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateRequestSizeOfDataOwner::DoCancel()
    {
    CompleteObserverRequest( KErrCancel );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfDataOwner::GetActiveAndPassiveDataSizesL()
// 
// 
// ---------------------------------------------------------------------------
TUint CMMCScBkupStateRequestSizeOfDataOwner::GetActiveAndPassiveDataSizesL( const CDataOwnerInfo& aOwner, 
    CSBGenericDataType& /*aIdentifier*/, TDriveNumber aDrive )
    {
    // Passive Data
    const TBool passiveBURSupported = MMCScBkupSBEUtils::HasPassiveDataL( aOwner );
    const TBool passiveDataTransferAllowedForDrive = DriveAndOperations().IsDataTypeAllowedToAccessDrive( aDrive, EMMCScBkupOwnerDataTypePassiveData );
    __LOG2("CMMCScBkupStateRequestSizeOfDataOwner::GetSIDSizesL() - passiveBURSupported: %d, passiveDataTransferAllowedForDrive: %d", passiveBURSupported, passiveDataTransferAllowedForDrive );

    TUint ret = 0;
    TUint itemSize = 0;
    if  ( passiveBURSupported && passiveDataTransferAllowedForDrive )
        {
        const TSecureId secureId = iOwner->SecureId();
        const TTransferDataType type = Driver().DrvParamsBase().PassiveTransferType();
        __LOG2("CMMCScBkupStateRequestSizeOfDataOwner::GetSIDSizesL() - sid: 0x%08x claims PASSIVE Data... type: %d", iOwner->SecureId().iId, type);

        itemSize = GetSIDSpecificSizeL( secureId, aDrive, type );
        iOwner->AddToOperationalSizeL( EMMCScBkupOwnerDataTypePassiveData, aDrive, itemSize );
        ret += itemSize;
        __LOG2("CMMCScBkupStateRequestSizeOfDataOwner::GetSIDSizesL() - sid: 0x%08x, passive size: %8d", secureId.iId, itemSize);
        }

    // Active Data
    const TBool activeBURSupported = MMCScBkupSBEUtils::HasActiveDataL( aOwner );
    const TBool activeDataTransferAllowedForDrive = DriveAndOperations().IsDataTypeAllowedToAccessDrive( aDrive, EMMCScBkupOwnerDataTypeActiveData );
    __LOG2("CMMCScBkupStateRequestSizeOfDataOwner::GetSIDSizesL() - activeBURSupported: %d, activeDataTransferAllowedForDrive: %d", activeBURSupported, activeDataTransferAllowedForDrive );

    if  ( activeBURSupported && activeDataTransferAllowedForDrive )
        {
        const TSecureId secureId = iOwner->SecureId();
        const TTransferDataType type = Driver().DrvParamsBase().ActiveTransferType();
        __LOG2("CMMCScBkupStateRequestSizeOfDataOwner::GetSIDSizesL() - sid: 0x%08x claims ACTIVE Data... type: %d", iOwner->SecureId().iId, type);


        // Technically, we should wait for an active data owner to become ready before we ask
        // for its sizing information. If we don't wait, we might receive KErrNotReady back
        // from our SBE request. 
        //
        // Currently, the MMC engine doesn't actually make any use of the active data sizing
        // info, so we trap (and ignore) the error in this situation. In the future, we will
        // have to change the state machine in order to request the active data statuses
        // before the sizing can be requested.
        itemSize = 0;
        TRAPD(err, GetSIDSpecificSizeL( secureId, aDrive, type ));
        __LOG3("CMMCScBkupStateRequestSizeOfDataOwner::GetSIDSizesL() - sid: 0x%08x, active size: %8d, fetchError: %d", secureId.iId, itemSize, err);

        if  ( err == KErrNone || err == KErrNotReady )
            {
            err = KErrNone;
            }
        else
            {
            User::LeaveIfError( err );
            }

        iOwner->AddToOperationalSizeL( EMMCScBkupOwnerDataTypeActiveData, aDrive, itemSize );
        ret += itemSize;
        __LOG2("CMMCScBkupStateRequestSizeOfDataOwner::GetSIDSizesL() - sid: 0x%08x, active size: %8d", secureId.iId, itemSize);
        }

    return ret;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfDataOwner::GetSIDSpecificSizeL()
// 
// 
// ---------------------------------------------------------------------------
TUint CMMCScBkupStateRequestSizeOfDataOwner::GetSIDSpecificSizeL( TSecureId aSecureID, TDriveNumber aDrive, TTransferDataType aDataType )
    {
    CSBSIDTransferType* transferType = CSBSIDTransferType::NewL( aSecureID,
                                                                 aDrive,
                                                                 aDataType
                                                                );
    CleanupStack::PushL( transferType );
    const TUint size = Driver().DrvSecureBackupClient().ExpectedDataSizeL( *transferType );
    CleanupStack::PopAndDestroy( transferType );
    return size;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfDataOwner::GetSystemDataSizesL()
// 
// 
// ---------------------------------------------------------------------------
TUint CMMCScBkupStateRequestSizeOfDataOwner::GetSystemDataSizesL( const CDataOwnerInfo& aOwner, CSBGenericDataType& aIdentifier, TDriveNumber aDrive )
    {
    TUint size = 0;
    //
    const TBool systemBURSupported = MMCScBkupSBEUtils::HasSystemDataL( aOwner );
    const TBool systemDataTransferAllowedForDrive = DriveAndOperations().IsDataTypeAllowedToAccessDrive( aDrive, EMMCScBkupOwnerDataTypeSystemData );
    __LOG2("CMMCScBkupStateRequestSizeOfDataOwner::GetSystemDataSizesL() - systemBURSupported: %d, systemDataTransferAllowedForDrive: %d", systemBURSupported, systemDataTransferAllowedForDrive );

    if  ( systemBURSupported && systemDataTransferAllowedForDrive )
        {
        __LOG1("CMMCScBkupStateRequestSizeOfDataOwner::GetSystemDataSizesL() - sid: 0x%08x claims SYSTEM Data...", iOwner->SecureId().iId);

        // Again we have to create a temporary in order to actually retrieve the info...
        CSBPackageId* packageGeneric = static_cast<CSBPackageId*>( MMCScBkupSBEUtils::CopyLC( aIdentifier ) );
        const TUid packageId = packageGeneric->PackageIdL();
        //
        const TPackageDataType type = Driver().DrvParamsBase().PackageTransferType();
        CSBPackageTransferType* transferType = CSBPackageTransferType::NewL( packageId,
                                                                             aDrive,
                                                                             type
                                                                            );
        CleanupStack::PushL( transferType );
        size = Driver().DrvSecureBackupClient().ExpectedDataSizeL( *transferType );
        __LOG3("CMMCScBkupStateRequestSizeOfDataOwner::GetSystemDataSizesL() - packageId: 0x%08x, sid: 0x%08x, package size: %8d", packageId.iUid, iOwner->SecureId().iId, size);
        CleanupStack::PopAndDestroy( transferType );
        //
        iOwner->AddToOperationalSizeL( EMMCScBkupOwnerDataTypeSystemData, aDrive, size );
        CleanupStack::PopAndDestroy( packageGeneric );
        }

    return size;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfDataOwner::GetJavaDataSizesL()
// 
// 
// ---------------------------------------------------------------------------
TUint CMMCScBkupStateRequestSizeOfDataOwner::GetJavaDataSizesL( const CDataOwnerInfo& aOwner, CSBGenericDataType& aIdentifier, TDriveNumber aDrive )
    {
    TUint size = 0;
    //
    const TBool javaBURSupported = MMCScBkupSBEUtils::HasJavaDataL( aOwner );
    const TBool javaDataTransferAllowedForDrive = DriveAndOperations().IsDataTypeAllowedToAccessDrive( aDrive, EMMCScBkupOwnerDataTypeJavaData );
    __LOG2("CMMCScBkupStateRequestSizeOfDataOwner::GetJavaSizesL() - javaBURSupported: %d, javaDataTransferAllowedForDrive: %d", javaBURSupported, javaDataTransferAllowedForDrive );
    
    if  ( javaBURSupported && javaDataTransferAllowedForDrive )
        {
        // Again we have to create a temporary in order to actually retrieve the info...
        CSBJavaId* packageGeneric = static_cast<CSBJavaId*>(MMCScBkupSBEUtils::CopyLC( aIdentifier ));
        const TPtrC pHash(packageGeneric->SuiteHashL());
        __LOG1("CMMCScBkupStateRequestSizeOfDataOwner::GetJavaSizesL() - midlet hash: %S", &pHash );
 
        const TUint midletSize = GetJavaSpecificSizeL( pHash, aDrive, EJavaMIDlet);
        __LOG2("CMMCScBkupStateRequestSizeOfDataOwner::GetJavaSizesL() - EJavaMIDlet,     sid: 0x%08x, size: %8d", iOwner->SecureId().iId, midletSize);
    
        const TUint midletDataSize = GetJavaSpecificSizeL( pHash, aDrive, EJavaMIDletData);
        __LOG2("CMMCScBkupStateRequestSizeOfDataOwner::GetJavaSizesL() - EJavaMIDletData, sid: 0x%08x, size: %8d", iOwner->SecureId().iId, midletDataSize);

        // We put the midlet itself in the special 'java data' category (the 'system data' equivalent for
        // java midlets).
        iOwner->AddToOperationalSizeL( EMMCScBkupOwnerDataTypeJavaData, aDrive, midletSize );

        // We put the midlet's data (e.g. any data files that it created) inside
        // the 'passive data' bucket.
        iOwner->AddToOperationalSizeL( EMMCScBkupOwnerDataTypePassiveData, aDrive, midletDataSize );
        
        // Java abuses the passive data slot. We would've initially set passive data's completion status
        // to ETrue inside CMMCScBkupDataOwnerInfo::ConstructL() as a result of receiving a Java
        // transfer type... but we can reset it back to "not yet completed" if the midlet
        // does indeed have some 'passive data'.
        iOwner->SetCompletionStatus( EMMCScBkupOwnerDataTypePassiveData, ( midletDataSize == 0 ) );

        CleanupStack::PopAndDestroy( packageGeneric );
        //
        size = midletSize + midletDataSize;
        }

    return size;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateRequestSizeOfDataOwner::GetJavaSpecificSizeL()
// 
// 
// ---------------------------------------------------------------------------
TUint CMMCScBkupStateRequestSizeOfDataOwner::GetJavaSpecificSizeL( const TDesC& aHash, TDriveNumber aDrive, TJavaTransferType aDataType )
    {
    CSBJavaTransferType* transferType = CSBJavaTransferType::NewL( aHash,
                                                                   aDrive,
                                                                   aDataType
                                                                  );
    CleanupStack::PushL( transferType );
    const TUint size = Driver().DrvSecureBackupClient().ExpectedDataSizeL( *transferType );
    CleanupStack::PopAndDestroy( transferType );
    return size;
    }
