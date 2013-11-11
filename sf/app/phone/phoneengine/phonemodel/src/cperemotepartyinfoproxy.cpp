/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
*
*/

#include "cperemotepartyinfoproxy.h"

#include <talogger.h>



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CPERemotePartyInfoProxy::CPERemotePartyInfoProxy( MPEMediator& aMediator )
    :iMediator( aMediator )
    {    
    
    }


// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CPERemotePartyInfoProxy::ConstructL()
    {
    
    }


// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CPERemotePartyInfoProxy* CPERemotePartyInfoProxy::NewL( MPEMediator& aMediator )
    {
    CPERemotePartyInfoProxy* self = CPERemotePartyInfoProxy::NewLC( aMediator );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CPERemotePartyInfoProxy* CPERemotePartyInfoProxy::NewLC( MPEMediator& aMediator )
    {
    CPERemotePartyInfoProxy* self = new( ELeave ) CPERemotePartyInfoProxy( aMediator );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPERemotePartyInfoProxy::~CPERemotePartyInfoProxy()
    {
    delete iRemotePartyData;
    }


// ---------------------------------------------------------------------------
// From base class MPEMediator
// ---------------------------------------------------------------------------
//
void CPERemotePartyInfoProxy::SendResponse( TUid aCategory,
                                            TInt aCommandId,
                                            const TDesC8& aData )
    {
    TEFLOGSTRING( KTAINT, "CPERemotePartyInfoProxy::SendResponse(): IN" );
    
    iRemotePartyData = aData.Alloc();
    
    // No need to filter command responses
    iMediator.SendResponse( aCategory,
                             aCommandId,
                             aData );
    
    TEFLOGSTRING( KTAINT, "CPERemotePartyInfoProxy::SendResponse(): OUT" );    
    }

// ---------------------------------------------------------------------------
// From base class MPEMediator
// ---------------------------------------------------------------------------
//
void CPERemotePartyInfoProxy::RaiseEvent( TUid aCategory,
                                          TInt aEventId,
                                          const TDesC8& aData )
    {
    TEFLOGSTRING( KTAINT, "CPERemotePartyInfoProxy::RaiseEvent(): IN" );
    
    
    
    // if changes in data or no data --> update proxy's data and pass event forward.
    if ( !iRemotePartyData || !iRemotePartyData->Match( aData ) == KErrNone )
        {
        delete iRemotePartyData;
        iRemotePartyData = aData.Alloc();
        iMediator.RaiseEvent( aCategory,
                              aEventId,
                              aData );  
        }
    
    TEFLOGSTRING( KTAINT, "CPERemotePartyInfoProxy::RaiseEvent(): OUT" );
    }





