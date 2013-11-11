/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CMMCScBkupEngine implementation
*
*
*/

#include "CMMCScBkupEngine.h"

// User includes
#include "CMMCScBkupEngineImpl.h"


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupEngine::CMMCScBkupEngine()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupEngine::CMMCScBkupEngine()
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngine::~CMMCScBkupEngine()
// 
// Destructor.
// ---------------------------------------------------------------------------
EXPORT_C CMMCScBkupEngine::~CMMCScBkupEngine()
    {
    delete iEngine;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngine::ConstructL()
// 
// Second phase constructor
// ---------------------------------------------------------------------------
void CMMCScBkupEngine::ConstructL( RFs& aFsSession )
    {
    iEngine = CMMCScBkupEngineImpl::NewL( aFsSession );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngine::NewL()
// 
// Static constructor
// ---------------------------------------------------------------------------
EXPORT_C CMMCScBkupEngine* CMMCScBkupEngine::NewL( RFs& aFsSession )
    {
    CMMCScBkupEngine* self = new(ELeave) CMMCScBkupEngine();
    CleanupStack::PushL( self );
    self->ConstructL( aFsSession );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngine::StartOperationL()
// 
// 
// ---------------------------------------------------------------------------
EXPORT_C void CMMCScBkupEngine::StartOperationL(TMMCScBkupOperationType aOperation, MMMCScBkupEngineObserver& aObserver, CMMCScBkupOpParamsBase* aParams)
    {
    TRAPD(err, iEngine->StartOperationL( aOperation, aObserver, aParams ));

    if(err != KErrNone)
        {
        TRAP_IGNORE( aObserver.HandleBkupEngineEventL( MMMCScBkupEngineObserver::ECommonOperationError, err) );
        TRAP_IGNORE( aObserver.HandleBkupEngineEventL( MMMCScBkupEngineObserver::ECommonOperationEnded, err) );
        // Call cleanup externally, because engine cannot be yet in active state and resources remain unfreed
        iEngine->CleanupL(KErrCancel);
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngine::CancelOperation()
// 
// 
// ---------------------------------------------------------------------------
EXPORT_C void CMMCScBkupEngine::CancelOperation()
    {
    iEngine->Cancel();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngine::ValidArchiveForRestore()
// 
// 
// ---------------------------------------------------------------------------
EXPORT_C TBool CMMCScBkupEngine::ValidArchiveForRestore( const TDesC& aFileName )
    {
    TBool archiveOkay = EFalse;
    //
    TRAPD(err, archiveOkay = iEngine->ValidArchiveForRestoreL( aFileName ) );
    if  ( err != KErrNone )
        {
        archiveOkay = EFalse;
        }
    //
    return archiveOkay;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngine::TotalOperationSizeL()
// 
// 
// ---------------------------------------------------------------------------
EXPORT_C TInt64 CMMCScBkupEngine::TotalOperationSizeL() const
    {
    return iEngine->TotalOperationSizeL();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupEngine::RebootRequired()
// 
// NOTE: This method is not used in S60 3.x - Sysap handles the reboot
// ---------------------------------------------------------------------------
EXPORT_C TBool CMMCScBkupEngine::RebootRequired() const
    {
    return iEngine->RebootRequired();
    }

EXPORT_C TBool CMMCScBkupEngine::DeleteArchivesL( RPointerArray< CMMCScBkupArchiveInfo >& aArchives ) const
    {
    return iEngine->DeleteArchivesL( aArchives );
    }
// ---------------------------------------------------------------------------
// CMMCScBkupEngine::ListArchivesL()
// 
// Provide a list of archives available on all drives
// ---------------------------------------------------------------------------
EXPORT_C void CMMCScBkupEngine::ListArchivesL(
        RPointerArray< CMMCScBkupArchiveInfo >& aArchives,
        CMMCScBkupOpParamsBase* aParams,
        const TUint32 aDriveAttMatch,
        const TInt aDriveMatch ) const
    {
    return iEngine->ListArchivesL(
        aArchives, aParams, aDriveAttMatch, aDriveMatch );
    }
