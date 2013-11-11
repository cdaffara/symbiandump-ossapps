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
* Description:  Forwards remote party info changes to Mediator.
*
*/

#include "cperemotepartyinfomediator.h"

#include <MediatorDomainUIDs.h>
#include <MediatorCommandResponder.h>
#include <MediatorEventProvider.h>
#include <callremotepartyinformation.h>

#include "cperemotepartyinfoproxy.h"
#include "tperemotepartyinfomediatorupdater.h"
#include "mpephonemodelinternal.h"
#include "mpedatastore.h"
#include <talogger.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CPERemotePartyInfoMediator* CPERemotePartyInfoMediator::NewL( MPEPhoneModelInternal& aModel )
    {
    CPERemotePartyInfoMediator* self = CPERemotePartyInfoMediator::NewLC( aModel );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CPERemotePartyInfoMediator* CPERemotePartyInfoMediator::NewLC( MPEPhoneModelInternal& aModel )
    {
    CPERemotePartyInfoMediator* self = new( ELeave ) CPERemotePartyInfoMediator;
    CleanupStack::PushL( self );
    self->ConstructL( aModel );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPERemotePartyInfoMediator::~CPERemotePartyInfoMediator()
    {
    if ( iEventProvider )
        {
        TInt ignore = iEventProvider->UnregisterEvent( 
                        KMediatorTelephonyDomain, 
                        KCatCallRemotePartyInformation,
                        EChangesInRemotePartyInformation );
        }
    delete iEventProvider;
    if ( iResponder )
        {
        TInt ignore = iResponder->UnregisterCommand( 
                        KMediatorTelephonyDomain, 
                        KCatCallRemotePartyInformation,
                        EGetCallRemotePartyInformation );
        }
    delete iResponder;   
    delete iUpdater;
    delete iProxy;
    }

// ---------------------------------------------------------------------------
// Updates call infos
// ---------------------------------------------------------------------------
//
void CPERemotePartyInfoMediator::UpdateRemotePartyInfo()
    {
    TEFLOGSTRING( KTAINT, "CPERemotePartyInfoMediator::UpdateRemotePartyInfo(): IN" );
    
    iUpdater->Update(); 
    
    TEFLOGSTRING( KTAINT, "CPERemotePartyInfoMediator::UpdateRemotePartyInfo(): OUT" );
    }

// ---------------------------------------------------------------------------
// From base class MPEMediator
// Sends the response to mediator
// ---------------------------------------------------------------------------
//
void CPERemotePartyInfoMediator::SendResponse( TUid aCategory,
                                               TInt aCommandId,
                                               const TDesC8& aData )
    {
    TEFLOGSTRING( KTAINT, "CPERemotePartyInfoMediator::SendResponse(): IN" );
    const TInt ignore( iResponder->IssueResponse( 
                                    KMediatorTelephonyDomain,
                                    aCategory, 
                                    aCommandId, KErrNone, aData ) ); 
    TEFLOGSTRING2( KTAINT, "CPERemotePartyInfoMediator::SendResponse() err = %d", ignore );
    TEFLOGSTRING( KTAINT, "CPERemotePartyInfoMediator::SendResponse(): OUT" );
    }

// ---------------------------------------------------------------------------
// From base class MPEMediator
// Raises the mediator event.
// ---------------------------------------------------------------------------
//
void CPERemotePartyInfoMediator::RaiseEvent( TUid aCategory,
                                             TInt aEventId,
                                             const TDesC8& aData )
    {
    TEFLOGSTRING( KTAINT, "CPERemotePartyInfoMediator::RaiseEvent(): IN" );
    TVersion version( KCallRemotePartyInformationVersionMajor,
                      KCallRemotePartyInformationVersionMinor, 
                      KCallRemotePartyInformationVersionBuild );
    
    const TInt ignore( iEventProvider->RaiseEvent( 
                                        KMediatorTelephonyDomain,
                                        aCategory, 
                                        aEventId, version, aData ) );
    TEFLOGSTRING2( KTAINT, "CPERemotePartyInfoMediator::RaiseEvent() err = %d", ignore );
    TEFLOGSTRING( KTAINT, "CPERemotePartyInfoMediator::RaiseEvent(): OUT" );
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CPERemotePartyInfoMediator::CPERemotePartyInfoMediator()
    {
    }


// ---------------------------------------------------------------------------
// Second phase constructor.
// ---------------------------------------------------------------------------
//
void CPERemotePartyInfoMediator::ConstructL( MPEPhoneModelInternal& aModel )
    {
    MPEDataStore* dataStore = aModel.DataStore();
    
    iProxy = CPERemotePartyInfoProxy::NewL(*this );
    
    iUpdater = new( ELeave ) TPERemotePartyInfoMediatorUpdater( *iProxy, *dataStore );    
    
    iResponder = CMediatorCommandResponder::NewL( iUpdater );
    TCapabilitySet caps = TCapabilitySet( ECapabilityReadUserData );
    
    TVersion version( KCallRemotePartyInformationVersionMajor,
                      KCallRemotePartyInformationVersionMinor, 
                      KCallRemotePartyInformationVersionBuild );
    
    User::LeaveIfError( iResponder->RegisterCommand( 
                                        KMediatorTelephonyDomain,
                                        KCatCallRemotePartyInformation, 
                                        EGetCallRemotePartyInformation,
                                        version,
                                        caps,
                                        KMediatorTimeoutInfinite ) );
    
    iEventProvider = CMediatorEventProvider::NewL();
    User::LeaveIfError( 
        iEventProvider->RegisterEvent( 
            KMediatorTelephonyDomain, KCatCallRemotePartyInformation, 
            EChangesInRemotePartyInformation, version, caps ) );

    }
