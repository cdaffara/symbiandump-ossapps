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
* Description: CMMCScBkupStateGetDataOwnerStatuses implementation
*
*
*/

#include "CMMCScBkupStateGetDataOwnerStatuses.h"

// System includes
#include <e32std.h>

// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "CMMCScBkupDataOwnerCollection.h"




// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwnerStatuses::CMMCScBkupStateGetDataOwnerStatuses()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateGetDataOwnerStatuses::CMMCScBkupStateGetDataOwnerStatuses( MMMCScBkupDriver& aDriver )
:   CMMCScBkupState( aDriver )
    {
    __LOG1("CMMCScBkupStateGetDataOwnerStatuses::CMMCScBkupStateGetDataOwnerStatuses() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwnerStatuses::~CMMCScBkupStateGetDataOwnerStatuses()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateGetDataOwnerStatuses::~CMMCScBkupStateGetDataOwnerStatuses()
    {
    Cancel();
    //
    iStatusArray.Close();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwnerStatuses::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateGetDataOwnerStatuses* CMMCScBkupStateGetDataOwnerStatuses::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateGetDataOwnerStatuses* self = new(ELeave) CMMCScBkupStateGetDataOwnerStatuses( aDriver );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwnerStatuses::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateGetDataOwnerStatuses::StateId() const
    {
    return KMMCScBkupStateIdGetDataOwnerStatuses;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwnerStatuses::NextStateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateGetDataOwnerStatuses::NextStateId() const
    {
    return KMMCScBkupStateIdArchiveOpActiveData;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwnerStatuses::PerformStateInitL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateGetDataOwnerStatuses::PerformStateInitL()
    {
    SetState( EPrearingQuery );
    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwnerStatuses::PerformAsynchronousStateStepL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateGetDataOwnerStatuses::PerformAsynchronousStateStepL()
    {
    switch( State() )
        {
    case EPrearingQuery:
        PrepareQueryL();
        break;
    case EGettingStatus:
        Driver().DrvSecureBackupClient().SIDStatusL( iStatusArray );
        SetState( EProcessingResults );
        CompleteSelf();
        break;
    case EProcessingResults:
        ProcessStatusResultsL();
        break;
    default:
        ASSERT( EFalse );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwnerStatuses::PrepareQueryL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateGetDataOwnerStatuses::PrepareQueryL()
    {
    const TDataOwnerStatus KMMCScBkupDefaultOwnerStatus = EUnset;
    const TInt KMMCScBkupDefaultOwnerStatusError = 0;

    // Build an array of all of the active data owners that we require
    // the statuses for.
    CMMCScBkupDataOwnerCollection& ownerCollection = Driver().DrvDataOwners();
    const TInt count = ownerCollection.Count();
    for(TInt i=0; i<count; i++)
        {
        CMMCScBkupDataOwnerInfo& owner = ownerCollection.Owner(i);
        //
        if  (owner.Owner().CommonSettings() & EActiveBUR)
            {
            const TDataOwnerAndStatus ownerStatus(
                                        owner.SecureId(),
                                        KMMCScBkupDefaultOwnerStatus,
                                        KMMCScBkupDefaultOwnerStatusError);
            iStatusArray.AppendL( ownerStatus );
            __LOG1("CMMCScBkupStateGetDataOwnerStatuses::PrepareQueryL() - DO 0x%08x *is* active", owner.SecureId().iId );
            }
        else
            {
            __LOG1("CMMCScBkupStateGetDataOwnerStatuses::PrepareQueryL() - DO 0x%08x is not active", owner.SecureId().iId );
            }
        }

    SetState( EGettingStatus );
    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateGetDataOwnerStatuses::ProcessStatusResultsL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateGetDataOwnerStatuses::ProcessStatusResultsL()
    {
    CMMCScBkupDataOwnerCollection& ownerCollection = Driver().DrvDataOwners();
    const TInt count = iStatusArray.Count();
    __LOG1("CMMCScBkupStateGetDataOwnerStatuses::ProcessStatusResultsL() - received %d entries from SBE...", count );
    for(TInt i=0; i<count; i++)
        {
        const TDataOwnerAndStatus& ownerStatus = iStatusArray[i];
        __LOG4("CMMCScBkupStateGetDataOwnerStatuses::ProcessStatusResultsL() - ownerStatus[%3d] sid: 0x%08x, status: %d, err: %d", 
            i, ownerStatus.iSID.iId, ownerStatus.iStatus, ownerStatus.iDataOwnerError );
        //
        CMMCScBkupDataOwnerInfo* owner = NULL;
        TRAPD(err, owner = &ownerCollection.OwnerL( ownerStatus.iSID ));
        if  (err == KErrNone && owner)
            {
            owner->SetStatus( ownerStatus.iStatus );
            }
        }
    }


