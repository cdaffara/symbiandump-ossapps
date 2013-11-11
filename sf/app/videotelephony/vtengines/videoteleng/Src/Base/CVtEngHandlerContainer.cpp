/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for functional handlers in the engine.
*
*/



// INCLUDE FILES
#include    "CVtEngHandlerContainer.h"
#include    <cvtlogger.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngHandlerContainer::Uninitialize
// Uninitialises handlers.
// -----------------------------------------------------------------------------
//
void CVtEngHandlerContainer::Uninitialize()
    {
    iSession->Uninitialize();
    }

// -----------------------------------------------------------------------------
// CVtEngHandlerContainer::CVtEngHandlerContainer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngHandlerContainer::CVtEngHandlerContainer()
    {
    }

// -----------------------------------------------------------------------------
// CVtEngHandlerContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngHandlerContainer::ConstructL( )
    {
    iMedia = CVtEngMediaHandler::NewL( );
    iSession = CVtEngSessionHandler::NewL( );
    iAudio = CVtEngAudioHandler::NewL( );
    
    }

void CVtEngHandlerContainer::CreateDtmfHandlerL( 
    MVtH324ConfigCommand* aH324Config )
    {
    __VTPRINTENTER( "HlrCnr.CreateDtmfHandlerL" )    
    iDtmf = CVtEngDtmfHandler::NewL( *aH324Config );
    __VTPRINTEXIT( "HlrCnr.CreateDtmfHandlerL" )
    }

// -----------------------------------------------------------------------------
// CVtEngHandlerContainer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngHandlerContainer* CVtEngHandlerContainer::NewL( )
    {
    __VTPRINTENTER( "HlrCnr.NewL" )
    
    CVtEngHandlerContainer* self = new( ELeave ) CVtEngHandlerContainer;
    
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();
    __VTPRINTEXIT( "HlrCnr.NewL" )
    return self;
    }


// Destructor
CVtEngHandlerContainer::~CVtEngHandlerContainer()
    {
    __VTPRINTENTER( "HlrCnr.~" )

    delete iMedia;
    delete iSession;
    delete iAudio;
    delete iDtmf;

    __VTPRINTEXIT( "HlrCnr.~" )
    }

//  End of File  
