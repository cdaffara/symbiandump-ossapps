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
* Description: CMMCScBkupStateArchiveOpArchiveFooter implementation
*
*
*/

#include "CMMCScBkupStateArchiveOpArchiveFooter.h"

// System includes
#include <e32std.h>
#include <s32strm.h>
#include <s32mem.h>

// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupArchive.h"
#include "CMMCScBkupArchiveFooter.h"
#include "CMMCScBkupArchiveHeader.h"



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpArchiveFooter::CMMCScBkupStateArchiveOpArchiveFooter()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpArchiveFooter::CMMCScBkupStateArchiveOpArchiveFooter( MMMCScBkupDriver& aDriver )
:   CMMCScBkupState( aDriver )
    {
    __LOG1("CMMCScBkupStateArchiveOpArchiveFooter::CMMCScBkupStateArchiveOpArchiveFooter() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpArchiveFooter::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpArchiveFooter* CMMCScBkupStateArchiveOpArchiveFooter::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateArchiveOpArchiveFooter* self = new(ELeave) CMMCScBkupStateArchiveOpArchiveFooter( aDriver );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpArchiveFooter::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpArchiveFooter::StateId() const
    {
    return KMMCScBkupStateIdArchiveOpArchiveFooter;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpArchiveFooter::NextStateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpArchiveFooter::NextStateId() const
    {
    TMMCScBkupStateId nextState = KMMCScBkupStateIdOperationComplete;
    //
    const TMMCScBkupOperationType type = Driver().DrvOperation();
    switch(type)
        {
    case EMMCScBkupOperationTypeFullBackup:
        nextState = KMMCScBkupStateIdOperationComplete;
        break;
    case EMMCScBkupOperationTypeFullRestore:
        nextState = KMMCScBkupStateIdArchiveOpDataOwners;
        break;
    default:
        ASSERT( EFalse );
        break;
        }
    //
    return nextState;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpArchiveFooter::PerformStateInitL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpArchiveFooter::PerformStateInitL()
    {
    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpArchiveFooter::PerformAsynchronousStateStepL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpArchiveFooter::PerformAsynchronousStateStepL()
    {
    CMMCScBkupArchive& archive = Driver().DrvArchive();
    CMMCScBkupArchiveFooter& footer = archive.Footer();
    CMMCScBkupArchiveHeader& header = archive.Header();
    //
    const TMMCScBkupOperationType type = Driver().DrvOperation();
    switch(type)
        {
    case EMMCScBkupOperationTypeFullBackup:
        __LOG("CMMCScBkupStateArchiveOpArchiveFooter::PerformAsynchronousStateStepL() - EMMCScBkupOperationTypeFullBackup - storing footer..." );
        footer.StoreL();
        break;
    case EMMCScBkupOperationTypeFullRestore:
        {
        __LOG("CMMCScBkupStateArchiveOpArchiveFooter::PerformAsynchronousStateStepL() - EMMCScBkupOperationTypeFullRestore - restoring footer..." );

        const TMMCScBkupArchiveVector& archiveVector = Driver().DrvADI().ADIOverallArchiveVectorInfo();
        __LOG1("CMMCScBkupStateArchiveOpArchiveFooter::PerformAsynchronousStateStepL() - overall archive length: %d", archiveVector.Length() );

        const TInt footerLengthFromHeader = header.FooterLength();
        __LOG1("CMMCScBkupStateArchiveOpArchiveFooter::PerformAsynchronousStateStepL() - footer length (read from header): %d", footerLengthFromHeader );

        const TInt footerStartingOffset = archiveVector.Length() - footerLengthFromHeader;
        __LOG1("CMMCScBkupStateArchiveOpArchiveFooter::PerformAsynchronousStateStepL() - footer starting offset: %d", footerStartingOffset );

        footer.RestoreL( footerStartingOffset );

        __LOG("CMMCScBkupStateArchiveOpArchiveFooter::PerformAsynchronousStateStepL() - restoring all indicies");
        
        // Restore all indicies
        CMMCScBkupIndexBase& indexDataOwner = footer.IndexByType(EMMCScBkupOwnerDataTypeDataOwner);
        indexDataOwner.RestoreL( Driver() );

        CMMCScBkupIndexBase& indexSystem = footer.IndexByType( EMMCScBkupOwnerDataTypeSystemData );
        indexSystem.RestoreL( Driver() );

        CMMCScBkupIndexBase& indexJava = footer.IndexByType( EMMCScBkupOwnerDataTypeJavaData );
        indexJava.RestoreL( Driver() );

        CMMCScBkupIndexBase& indexPassive = footer.IndexByType( EMMCScBkupOwnerDataTypePassiveData );
        indexPassive.RestoreL( Driver() );

        CMMCScBkupIndexBase& indexActive = footer.IndexByType( EMMCScBkupOwnerDataTypeActiveData );
        indexActive.RestoreL( Driver() );

        CMMCScBkupIndexBase& indexPublic = footer.IndexByType( EMMCScBkupOwnerDataTypePublicData );
        indexPublic.RestoreL( Driver() );

        __LOG("CMMCScBkupStateArchiveOpArchiveFooter::PerformAsynchronousStateStepL() - all indicies restored");

        break;
        }
    default:
        User::Leave(KErrNotSupported);
        break;
        }
    }



