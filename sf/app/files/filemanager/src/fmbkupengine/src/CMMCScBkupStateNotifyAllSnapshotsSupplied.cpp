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
* Description: CMMCScBkupStateNotifyAllSnapshotsSupplied implementation
*
*
*/

#include "CMMCScBkupStateNotifyAllSnapshotsSupplied.h"

// System includes
#include <e32std.h>

// User includes
#include "MMCScBkupLogger.h"




// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateNotifyAllSnapshotsSupplied::CMMCScBkupStateNotifyAllSnapshotsSupplied()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateNotifyAllSnapshotsSupplied::CMMCScBkupStateNotifyAllSnapshotsSupplied( MMMCScBkupDriver& aDriver )
:   CMMCScBkupState( aDriver )
    {
    __LOG1("CMMCScBkupStateNotifyAllSnapshotsSupplied::CMMCScBkupStateNotifyAllSnapshotsSupplied() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateNotifyAllSnapshotsSupplied::NewL()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateNotifyAllSnapshotsSupplied* CMMCScBkupStateNotifyAllSnapshotsSupplied::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateNotifyAllSnapshotsSupplied* self = new(ELeave) CMMCScBkupStateNotifyAllSnapshotsSupplied( aDriver);
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateNotifyAllSnapshotsSupplied::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateNotifyAllSnapshotsSupplied::StateId() const
    {
    return KMMCScBkupStateIdNotifyAllSnapshotsSupplied;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateNotifyAllSnapshotsSupplied::NextStateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateNotifyAllSnapshotsSupplied::NextStateId() const
    {
    TMMCScBkupStateId nextState = KMMCScBkupStateIdRequestSizeOfBackupData;
    //
    const TMMCScBkupOperationType type = Driver().DrvOperation();
    switch(type)
        {
    case EMMCScBkupOperationTypeFullBackup:
        nextState = KMMCScBkupStateIdRequestSizeOfBackupData;
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
// CMMCScBkupStateNotifyAllSnapshotsSupplied::PerformStateInitL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateNotifyAllSnapshotsSupplied::PerformStateInitL()
    {
    Driver().DrvSecureBackupClient().AllSnapshotsSuppliedL();
    }







