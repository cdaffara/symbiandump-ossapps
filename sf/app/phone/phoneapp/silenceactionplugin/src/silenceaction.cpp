/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A plug-in for silencing the rigning tones.
*
*/


#include "silenceaction.h"
#include "logger.h"

// CONSTANTS


// -----------------------------------------------------------------------------
// CSilenceAction::NewL
// -----------------------------------------------------------------------------
// 
CSilenceAction* CSilenceAction::NewL( MSilenceActionObserver& aObserver )
    {
    CSilenceAction* self = CSilenceAction::NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }
 
// -----------------------------------------------------------------------------
// CSilenceAction::NewLC
// -----------------------------------------------------------------------------
// 
CSilenceAction* CSilenceAction::NewLC( MSilenceActionObserver& aObserver )
    {
    CSilenceAction* self = new( ELeave ) CSilenceAction( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }
 
// -----------------------------------------------------------------------------
// CSilenceAction Destructor.
// -----------------------------------------------------------------------------
//
CSilenceAction::~CSilenceAction()
    {
    Cancel();
        
    if ( iPhoneClient )
        {
        delete iPhoneClient; 
        }
    }

// -----------------------------------------------------------------------------
// CSilenceAction constructor.
// -----------------------------------------------------------------------------
//
CSilenceAction::CSilenceAction( MSilenceActionObserver& aObserver ):
    CActive( EPriorityStandard ),
    iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CSilenceAction::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSilenceAction::ConstructL( )
    {
    LOG_1( _L("->CSilenceAction::ConstructL: START") );     

    iPhoneClient = CPhCltCommandHandler::NewL();

    LOG_1( _L("<-CSilenceAction::ConstructL: END") );
    }

//-----------------------------------------------------------------------------
// CSilenceAction::ActionMuteRingingToneL
//-----------------------------------------------------------------------------
// 
void CSilenceAction::ActionMuteRingingTone( )
    {
    LOG_1( _L("->CSilenceAction::ActionMuteRingingTone: START") );
    if ( !IsActive() ) 
        {
        iPhoneClient->MuteRingingTone(iStatus);
        SetActive();
        }
    LOG_1( _L("<-CSilenceAction::ActionMuteRingingTone: END") );
    }
//-----------------------------------------------------------------------------
// CSilenceAction::RunL
//-----------------------------------------------------------------------------
//
void CSilenceAction::RunL()
    {
    iObserver.MuteActionCompleted();
    LOG_2( _L("CSilenceAction::RunL iStatus: %d"), iStatus.Int() );
    }

//-----------------------------------------------------------------------------
// CSilenceAction::DoCancel
//-----------------------------------------------------------------------------
//
void CSilenceAction::DoCancel()
    {
    LOG_1( _L("->CSilenceAction::DoCancel: START") );
    iPhoneClient->CancelAsyncRequest( EPhCltCommandMuteRingingTone );
    LOG_1( _L("<-CSilenceAction::DoCancel: END") );
    }
        
//-----------------------------------------------------------------------------
// CSilenceAction::RunError
//-----------------------------------------------------------------------------
//
TInt CSilenceAction::RunError( TInt aError )
    {
    LOG_2( _L("CSilenceAction::RunError aError: %d"), aError );
    return aError;
    }

// End of File
