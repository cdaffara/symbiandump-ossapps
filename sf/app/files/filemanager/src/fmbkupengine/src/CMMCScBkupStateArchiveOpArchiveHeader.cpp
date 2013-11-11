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
* Description: CMMCScBkupStateArchiveOpArchiveHeader implementation
*
*
*/

#include "CMMCScBkupStateArchiveOpArchiveHeader.h"

// System includes
#include <e32std.h>
#include <s32strm.h>
#include <s32mem.h>

// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupArchive.h"
#include "CMMCScBkupArchiveHeader.h"



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpArchiveHeader::CMMCScBkupStateArchiveOpArchiveHeader()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpArchiveHeader::CMMCScBkupStateArchiveOpArchiveHeader( MMMCScBkupDriver& aDriver )
:   CMMCScBkupState( aDriver )
    {
    __LOG1("CMMCScBkupStateArchiveOpArchiveHeader::CMMCScBkupStateArchiveOpArchiveHeader() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpArchiveHeader::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpArchiveHeader* CMMCScBkupStateArchiveOpArchiveHeader::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateArchiveOpArchiveHeader* self = new(ELeave) CMMCScBkupStateArchiveOpArchiveHeader( aDriver );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpArchiveHeader::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpArchiveHeader::StateId() const
    {
    return KMMCScBkupStateArchiveOpArchiveHeader;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpArchiveHeader::NextStateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpArchiveHeader::NextStateId() const
    {
    TMMCScBkupStateId nextState = KMMCScBkupStateIdGetDataOwners;
    //
    const TMMCScBkupOperationType type = Driver().DrvOperation();
    switch(type)
        {
    case EMMCScBkupOperationTypeFullBackup:
        nextState = KMMCScBkupStateIdGetDataOwners;
        break;
    case EMMCScBkupOperationTypeFullRestore:
        nextState = KMMCScBkupStateIdArchiveOpArchiveFooter;
        break;
    default:
        ASSERT( EFalse );
        break;
        }
    //
    return nextState;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpArchiveHeader::PerformStateInitL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpArchiveHeader::PerformStateInitL()
    {
    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpArchiveHeader::PerformAsynchronousStateStepL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpArchiveHeader::PerformAsynchronousStateStepL()
    {
    CMMCScBkupArchive& archive = Driver().DrvArchive();
    CMMCScBkupArchiveHeader& header = archive.Header();
    //
    const TMMCScBkupOperationType type = Driver().DrvOperation();
    switch(type)
        {
    case EMMCScBkupOperationTypeFullBackup:
        header.StoreL(archive.Category());
#ifdef RD_FILE_MANAGER_BACKUP
        // Activate crc-calculation for payload data. Header crc'd separately
        Driver().DrvADI().ADIActivateCrcCalculation(ETrue);
#endif
        break;
    case EMMCScBkupOperationTypeFullRestore:
        header.RestoreL();
        break;
    default:
        User::Leave(KErrNotSupported);
        break;
        }
    }



