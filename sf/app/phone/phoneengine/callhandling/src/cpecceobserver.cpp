/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Observes CCE
*
*/


#include "cpecceobserver.h"

#include <mccecall.h>
#include <pepanic.pan>
#include <mpephonemodelinternal.h>
#include <mpedatastore.h>
#include "mpecallinitialiser.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Static construcotr
// ---------------------------------------------------------------------------
//
CPECCEObserver* CPECCEObserver::NewL( 
    MPECallInitialiser& aCallInitialiser,
    MPEPhoneModelInternal& aPhoneModel )
    {
    CPECCEObserver* self = 
        CPECCEObserver::NewLC( aCallInitialiser, aPhoneModel );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CPECCEObserver* CPECCEObserver::NewLC( 
    MPECallInitialiser& aCallInitialiser,
    MPEPhoneModelInternal& aPhoneModel )
    {
    CPECCEObserver* self = 
        new( ELeave ) CPECCEObserver( aCallInitialiser, aPhoneModel  );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPECCEObserver::~CPECCEObserver()
    {
    }

// ---------------------------------------------------------------------------
// From base class MCCEObserver
// ---------------------------------------------------------------------------
//
void CPECCEObserver::IncomingCall( MCCECall& aCall )
    {
    NotifyCallInitialiser( aCall );
    }

// ---------------------------------------------------------------------------
// From base class MCCEObserver
// ---------------------------------------------------------------------------
//
void CPECCEObserver::MOCallCreated( MCCECall& aCall )
    {
    NotifyCallInitialiser( aCall );
    }

// ---------------------------------------------------------------------------
// From base class MCCEObserver
// ---------------------------------------------------------------------------
//
void CPECCEObserver::ConferenceCallCreated( MCCEConferenceCall& aConferenceCall )
    {
    iCallInitialiser.InitConferenceCall( aConferenceCall );
    }

// ---------------------------------------------------------------------------
// From base class MCCEObserver
// ---------------------------------------------------------------------------
//
void CPECCEObserver::DataPortName( TName& aPortName )
    {
    iPhoneModel.DataStore()->SetDataPortName( aPortName );
    iPhoneModel.SendMessage( MEngineMonitor::EPEMessageDataPortLoaned );
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CPECCEObserver::CPECCEObserver( 
    MPECallInitialiser& aCallInitialiser,
    MPEPhoneModelInternal& aPhoneModel ) : 
iCallInitialiser( aCallInitialiser ),
iPhoneModel( aPhoneModel )
    {
    }

// ---------------------------------------------------------------------------
// Notifies the call initialiser to take start using the incoming/external 
// call
// ---------------------------------------------------------------------------
//
void CPECCEObserver::NotifyCallInitialiser( MCCECall& aCall )
    {
    const CCPCall::TCallType callType = 
        aCall.Parameters().CallType();
    
    switch( callType )
        {
        case CCPCall::ECallTypeVideo:
            iCallInitialiser.InitVideoCall( aCall );
            break;
        
        case CCPCall::ECallTypeCSVoice:
        case CCPCall::ECallTypePS:
            iCallInitialiser.InitVoiceCall( aCall );
            break;
            
        default:
            Panic( EPEPanicInvalidMode );
        }
    }


