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
* Description: CMMCScBkupStateArchiveOpDataOwners implementation
*
*
*/

#include "CMMCScBkupStateArchiveOpDataOwners.h"

// System includes
#include <e32std.h>
#include <s32strm.h>
#include <s32mem.h>

// User includes
#include "MMCScBkupLogger.h"
#include "CMMCScBkupArchive.h"
#include "RMMCScBkupProgressSizer.h"
#include "CMMCScBkupArchiveFooter.h"
#include "CMMCScBkupDataOwnerInfo.h"
#include "CMMCScBkupIndexDataOwners.h"
#include "MMMCScBkupProgressObserver.h"
#include "CMMCScBkupDataOwnerCollection.h"
#include "MMMCScBkupArchiveDataInterface.h"



// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpDataOwners::CMMCScBkupStateArchiveOpDataOwners()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpDataOwners::CMMCScBkupStateArchiveOpDataOwners( MMMCScBkupDriver& aDriver )
:   CMMCScBkupStateOpAware( aDriver ), iIndexValueCurrent( -1 )
    {
    __LOG1("CMMCScBkupStateArchiveOpDataOwners::CMMCScBkupStateArchiveOpDataOwners() - 0x%08x", StateId().iUid );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpDataOwners::~CMMCScBkupStateArchiveOpDataOwners()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpDataOwners::~CMMCScBkupStateArchiveOpDataOwners()
    {
    Cancel();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpDataOwners::NewL()
// 
// 
// ---------------------------------------------------------------------------
CMMCScBkupStateArchiveOpDataOwners* CMMCScBkupStateArchiveOpDataOwners::NewL( MMMCScBkupDriver& aDriver )
    {
    CMMCScBkupStateArchiveOpDataOwners* self = new(ELeave) CMMCScBkupStateArchiveOpDataOwners( aDriver );
    return self;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpDataOwners::StateId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpDataOwners::StateId() const
    {
    return KMMCScBkupStateIdArchiveOpDataOwners;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpDataOwners::NextStateBackupId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpDataOwners::NextStateBackupId( TBool /*aPartial*/ ) const
    {
    return KMMCScBkupStateIdArchiveOpArchiveFooter;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpDataOwners::NextStateRestoreId()
// 
// 
// ---------------------------------------------------------------------------
TMMCScBkupStateId CMMCScBkupStateArchiveOpDataOwners::NextStateRestoreId( TBool /*aPartial*/ ) const
    {
    return KMMCScBkupStateIdValidateDiskSpace;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpDataOwners::PerformStateInitBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpDataOwners::PerformStateInitBackupL( TBool /*aPartial*/ )
    {
    // Work is carried out inside asynchronous step
    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpDataOwners::PerformStateInitRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpDataOwners::PerformStateInitRestoreL( TBool /*aPartial*/ )
    {
    // Restore each data owner asynchronously
    CompleteSelf();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpDataOwners::PerformAsynchronousStateStepBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpDataOwners::PerformAsynchronousStateStepBackupL( TBool /*aPartial*/ )
    {
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    CMMCScBkupArchiveFooter& footer = Driver().DrvArchive().Footer();
    CMMCScBkupIndexBase& indexBase = footer.IndexByType(EMMCScBkupOwnerDataTypeDataOwner);
    CMMCScBkupIndexDataOwners& index = static_cast< CMMCScBkupIndexDataOwners& >( indexBase );
    const TInt count = dataOwners.Count();
    //
    if  ( ++iIndexValueCurrent < count )
        {
        CMMCScBkupDataOwnerInfo& owner = dataOwners.Owner( iIndexValueCurrent );

        // Externalise 
        MMMCScBkupArchiveDataInterface& archiveDataInterface = ADI();
        RWriteStream stream( archiveDataInterface.ADIWriteStreamUncompressedLC() );
        stream << owner;
        stream.CommitL();
        CleanupStack::PopAndDestroy(); // stream


        // Save the offset of the data owner
        const TMMCScBkupArchiveVector& info = archiveDataInterface.ADICurrentArchiveVectorInfo();
        index.AddIndexRecordL( info );

        // Now move on to the next data owner
        CompleteSelf();
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpDataOwners::PerformAsynchronousStateStepRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpDataOwners::PerformAsynchronousStateStepRestoreL( TBool /*aPartial*/ )
    {
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    CMMCScBkupArchiveFooter& footer = Driver().DrvArchive().Footer();
    CMMCScBkupIndexBase& indexBase = footer.IndexByType( EMMCScBkupOwnerDataTypeDataOwner );
    CMMCScBkupIndexDataOwners& index = static_cast< CMMCScBkupIndexDataOwners& >( indexBase );
    const TInt count = index.Count();
    __LOG1( "CMMCScBkupStateArchiveOpDataOwners::PerformAsynchronousStateStepRestoreL() - %d data owner index records...", count );
    //
    if  ( ++iIndexValueCurrent < count )
        {
        const TMMCScBkupArchiveVector& entry = index.At( iIndexValueCurrent );
        __LOG3( "CMMCScBkupStateArchiveOpDataOwners::PerformAsynchronousStateStepRestoreL() - entry[%03d] at offset: 0x%08x, length: %4d", iIndexValueCurrent, entry.Offset(), entry.Length() );

        MMMCScBkupArchiveDataInterface& archiveDataInterface = ADI();
        RReadStream stream( archiveDataInterface.ADIReadStreamUncompressedLC( entry.Offset() ) );
        
        // Internalize a new entry and add it to the data owner collection
        CMMCScBkupDataOwnerInfo* dataOwner = CMMCScBkupDataOwnerInfo::NewLC( stream );
        dataOwners.AppendL( dataOwner );
        CleanupStack::Pop( dataOwner );

        // Tidy up & check that we didn't read too much - oops
        const TMMCScBkupArchiveVector& readInfo = archiveDataInterface.ADICurrentArchiveVectorInfo();
        if  ( readInfo.Length() > entry.Length() )
            {
            __LOG( "CMMCScBkupStateArchiveOpDataOwners::PerformAsynchronousStateStepRestoreL() - ERROR - Read too much data! -> leave with KErrCorrupt" );
            User::Leave( KErrCorrupt );
            }
        CleanupStack::PopAndDestroy(); // stream

        // Continue to next entry
        CompleteSelf();
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpDataOwners::PerformLastRightsBackupL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpDataOwners::PerformLastRightsBackupL( TBool /*aPartial*/ )
    {
    CMMCScBkupIndexBase& index = Driver().DrvArchive().Footer().IndexByType( EMMCScBkupOwnerDataTypeDataOwner );
    index.StoreL( Driver() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpDataOwners::PerformLastRightsRestoreL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupStateArchiveOpDataOwners::PerformLastRightsRestoreL( TBool /*aPartial*/ )
    {
    // Calculate restore byte size (for all drives)
    CMMCScBkupDataOwnerCollection& dataOwners = Driver().DrvDataOwners();
    const TInt64 totalDataRestoreSize = dataOwners.TotalOperationalSizeL();

    // Inform progress manager
    if(Driver().DrvLastCategory())
        {
        __LOG2("CMMCScBkupStateArchiveOpDataOwners::PerformLastRightsRestoreL() - report progress understood (%Ld + %Ld)", 
            totalDataRestoreSize, Driver().DrvTotalProgress());
        Driver().DrvProgressHandler().MMCScBkupHandleProgressDomainUnderstood( totalDataRestoreSize + Driver().DrvTotalProgress());
        }
    else
        {
        __LOG2("CMMCScBkupStateArchiveOpDataOwners::PerformLastRightsRestoreL() - adding progress amount %Ld to existing %Ld)", 
            totalDataRestoreSize, Driver().DrvTotalProgress());
        Driver().DrvStoreTotalProgress( totalDataRestoreSize );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupStateArchiveOpDataOwners::PerformAsynchronousErrorCleanupRestore()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupStateArchiveOpDataOwners::PerformAsynchronousErrorCleanupRestore( TBool /*aPartial*/, TInt aError )
    {
    (void) aError;
    __LOGFILE1("CMMCScBkupStateArchiveOpDataOwners::PerformAsynchronousErrorCleanupRestore() - **** - error: %d", aError );

    // Just try to skip the data owner. 
    CompleteSelf();
    return ETrue;
    }










