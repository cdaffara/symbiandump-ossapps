/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* 
* Description:
*     The source file of drive event watcher
*/


// INCLUDE FILES
#include "fmdriveevent.h"
#include <e32std.h>
#include "fmcommon.h"

#include <aknnotewrappers.h> 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFileManagerFileSystemEvent::CFileManagerFileSystemEvent
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

CFmDriveEvent::CFmDriveEvent(
        RFs& aFs,
        MFmDriveEventObserver *aObserver ) :
    CActive( CActive::EPriorityLow ),
    iFs( aFs ),
    iObserver( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CFmDriveEvent::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFmDriveEvent::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CFmDriveEvent::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFmDriveEvent* CFmDriveEvent::NewL(
        RFs& aFs,
        MFmDriveEventObserver *aObserver )
    {
    CFmDriveEvent* self =
        new( ELeave ) CFmDriveEvent( 
            aFs,
            aObserver );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ------------------------------------------------------------------------------
// CFmDriveEvent::~CFmDriveEvent
//
// ------------------------------------------------------------------------------
CFmDriveEvent::~CFmDriveEvent()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CFmDriveEvent::RunError
// From CActive. Called when error occurred in asynchronous request
// Notifies the observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CFmDriveEvent::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CFmDriveEvent::DoCancel
// From CActive. Called when asynchronous request is canceled
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFmDriveEvent::DoCancel()
    {
    iFs.NotifyChangeCancel( iStatus );
    }

// ------------------------------------------------------------------------------
// CFmDriveEvent::Setup
//
// ------------------------------------------------------------------------------
//
void CFmDriveEvent::Setup()
    {
    FM_LOG( QString( "CFmDriveEvent::Setup start" ) );
    if( IsActive() )
        {
        return;
        }
    
    iFs.NotifyChange( ENotifyDisk, iStatus );
    SetActive();
    FM_LOG( QString( "CFmDriveEvent::Setup end" ) );
    }

// -----------------------------------------------------------------------------
// CFmDriveEvent::RunL
// From CActive. Called when asynchronous request is completed. 
// Notifies the observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFmDriveEvent::RunL()
    {
    if( iStatus == KErrNone ){
        if( iObserver ){
            iObserver->OnDriveAddedOrChangedL();
        }
        
        Setup();
    }

    /*
	CAknInformationNote* informationNote;
	informationNote = new ( ELeave ) CAknInformationNote;
	informationNote->ExecuteLD( _L( "driveChanged!"));
    */
    }

//  End of File
