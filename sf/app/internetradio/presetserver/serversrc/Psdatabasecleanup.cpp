/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Handles periodical database cleanup routines
*
*/


#include <ecom.h>

#include "pspresetinterface.h"
#include "psdatabase.h"
#include "psdatabasecleanup.h"
#include "psdebug.h"

const TInt KPSCleanupInterval = 480000000; // Interval for periodical compaction attempts
const TInt KPSRequestCleanupCheckDelay = 20000000; // Delay when compaction attempt is done after a request to check.
const TInt KPSCleanupAmountOfDirtAllowed = 300*1024; // Amount of dirty data allowed in database (bytes).
const TInt KPSCleanupCompactThresholdPercentage = 50; // Threshold percentage in database data efficiency when db compact process is triggered.

// ======== LOCAL FUNCTIONS ========

namespace
    {
    
    // ---------------------------------------------------------------------------
    // ?description_if_needed
    // ---------------------------------------------------------------------------
    //
    void CleanupImplArray( TAny* aSelf )
        {
        RImplInfoPtrArray& self = *static_cast<RImplInfoPtrArray*>( aSelf );
        self.ResetAndDestroy();
        self.Close();
        }
    
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CPSDatabaseCleanup::CPSDatabaseCleanup( RPSDatabase& aDatabase )
    : CTimer( CActive::EPriorityStandard ), iDatabase( aDatabase )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPSDatabaseCleanup::ConstructL()
    {
    CTimer::ConstructL();

    RunL();
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CPSDatabaseCleanup* CPSDatabaseCleanup::NewL( RPSDatabase& aDatabase )
    {
    CPSDatabaseCleanup* self = new ( ELeave ) CPSDatabaseCleanup( aDatabase );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CPSDatabaseCleanup::~CPSDatabaseCleanup()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// From class ?base_class.
// ?implementation_description
// ---------------------------------------------------------------------------
//
void CPSDatabaseCleanup::RunL()
    {
    After( KPSCleanupInterval );

    User::LeaveIfError( iDatabase.UpdateStats() ); // Update the statistics
    RDbDatabase::TSize size = iDatabase.Size();
    
    PSDEBUG( "PSDatabaseCleanup::RunL()" );
    PSDEBUG3( "\tCompacting the database: iSize = %d, iUsage = %d", size.iSize, size.iUsage );

    // Perform database compaction only if efficient data in database in percents is below defined percentage or the amount of dirty
    // data in database file is at least a defined constant value of bytes.
    TInt dirt = size.iSize * (100 - size.iUsage) / 100;
    if (dirt > KPSCleanupAmountOfDirtAllowed || size.iUsage < KPSCleanupCompactThresholdPercentage)
    	{
        User::LeaveIfError( iDatabase.Compact() );
        PSDEBUG3( "\tDatabase compacted succesfully: iSize = %d, iUsage = %d", iDatabase.Size().iSize, iDatabase.Size().iUsage );
    	}

    // Perform orphan cleanup.
    DeleteOrphanedPresetsL();
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPSDatabaseCleanup::DeleteOrphanedPresetsL()
    {
    RArray<TInt> dataHandlers;
    CleanupClosePushL( dataHandlers );
    RImplInfoPtrArray implementations;
    CleanupStack::PushL( TCleanupItem( CleanupImplArray, &implementations ) );
    
    REComSession::ListImplementationsL( KPSPresetInterface, implementations );
    
    for ( TInt i = 0; i < implementations.Count(); i++ )
        {
        dataHandlers.AppendL( implementations[i]->ImplementationUid().iUid );
        }

    CleanupStack::PopAndDestroy(); // implementations
    
    REComSession::FinalClose();
    
    iDatabase.DeleteOrphanedPresetsL( dataHandlers );
    
    CleanupStack::PopAndDestroy( &dataHandlers );
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CPSDatabaseCleanup::RequestCleanupCheck()
	{
	Cancel();
	After(KPSRequestCleanupCheckDelay);
	}
// ======== GLOBAL FUNCTIONS ========
