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
* Description: CMMCScBkupStateSetPhoneMode implementation
*
*
*/

#include "CMMCScBkupStateSetPhoneMode.h"

// System includes
#include <e32std.h>

// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupDriveAndOperationTypeManager.h"



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateSetPhoneMode::CMMCScBkupStateSetPhoneMode()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateSetPhoneMode::CMMCScBkupStateSetPhoneMode( MMMCScBkupDriver& aDriver )
:   CMMCScBkupState( aDriver )
    {
    __LOG1("CMMCScBkupStateSetPhoneMode::CMMCScBkupStateSetPhoneMode() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateSetPhoneMode::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateSetPhoneMode* CMMCScBkupStateSetPhoneMode::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateSetPhoneMode* self = new(ELeave) CMMCScBkupStateSetPhoneMode( aDriver);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateSetPhoneMode::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateSetPhoneMode::StateId() const
    {
    return KMMCScBkupStateIdSetPhoneMode;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateSetPhoneMode::NextStateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateSetPhoneMode::NextStateId() const
    {
    TMMCScBkupStateId nextState = KMMCScBkupStateIdNotifyAllSnapshotsSupplied;
    //
    const TMMCScBkupOperationType type = Driver().DrvOperation();
    switch(type)
        {
    case EMMCScBkupOperationTypeFullBackup:
        nextState = KMMCScBkupStateIdNotifyAllSnapshotsSupplied;
        break;
    case EMMCScBkupOperationTypeFullRestore:
        nextState = KMMCScBkupStateIdArchiveOpSystemData;    
        break;
    default:
        ASSERT( EFalse );
        break;
        }
    //
    return nextState;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateSetPhoneMode::PerformStateInitL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateSetPhoneMode::PerformStateInitL()
    {
    __LOG("CMMCScBkupStateSetPhoneMode::PerformStateInitL()" );
    //
    const CMMCScBkupOpParamsBase& params = Driver().DrvParamsBase();
    //
    Driver().DrvSecureBackupClient().SetBURModeL( params.DriveAndOperations().DriveList(), 
                                                  params.PartType(),
                                                  params.IncrementType(),
                                                  iStatus );
    SetActive();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateSetPhoneMode::PerformAsynchronousStateStepL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateSetPhoneMode::PerformAsynchronousStateStepL()
    {
    __LOG("CMMCScBkupStateSetPhoneMode::PerformAsynchronousStateStepL()" );
    }


