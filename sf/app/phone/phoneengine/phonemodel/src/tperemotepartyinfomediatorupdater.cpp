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
* Description:  Updates remote party information via mediator.
*
*/

#include "tperemotepartyinfomediatorupdater.h"
#include "mpemediator.h"
#include "mpedatastore.h"

#include <talogger.h>
#include <MediatorDomainUIDs.h>

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
TPERemotePartyInfoMediatorUpdater::TPERemotePartyInfoMediatorUpdater(
    MPEMediator& aMediator,
    MPEDataStore& aDataStore ) 
    : iMediator( aMediator ),
    iDataStore( aDataStore )
    {
    }
   
// ---------------------------------------------------------------------------
// Sends mediator event with remote info.
// ---------------------------------------------------------------------------
//
void TPERemotePartyInfoMediatorUpdater::Update()
    {
    HBufC8* data = NULL;
    TRAPD( err, data = TakeSnapshotOfRemotePartyInfosL() );
    if ( !err )
        {
        iMediator.RaiseEvent( 
                KCatCallRemotePartyInformation,
                EChangesInRemotePartyInformation,
                *data );
        }
    delete data;
        
    }

// ---------------------------------------------------------------------------
// From base class MMediatorCommandObserver
// ---------------------------------------------------------------------------
//
void TPERemotePartyInfoMediatorUpdater::MediatorCommandL( 
    TUid aDomain,
    TUid aCategory, 
    TInt aCommandId,
    TVersion /*aVersion*/, 
    const TDesC8& /*aData*/ )
    {
    if ( aDomain == KMediatorTelephonyDomain &&
         aCategory == KCatCallRemotePartyInformation &&
         aCommandId == EGetCallRemotePartyInformation )
        {
        HBufC8* data = TakeSnapshotOfRemotePartyInfosL();
        
        iMediator.SendResponse( 
            KCatCallRemotePartyInformation,
            EGetCallRemotePartyInformation,
            *data );
    
        delete data;
        }
    }

// ---------------------------------------------------------------------------
// From base class MMediatorCommandObserver
// ---------------------------------------------------------------------------
//
void TPERemotePartyInfoMediatorUpdater::CancelMediatorCommand( 
    TUid /*aDomain*/,
    TUid /*aCategory*/, 
    TInt /*aCommandId*/ )
    {
    }

// ---------------------------------------------------------------------------
// Takes remote party information from all calls and packs them to 
// descriptor.
// ---------------------------------------------------------------------------
//
HBufC8* TPERemotePartyInfoMediatorUpdater::TakeSnapshotOfRemotePartyInfosL()
    {
    CCallRemotePartyInfos* infos = CCallRemotePartyInfos::NewLC();
    for ( TInt callId = 0; callId < KPEMaximumNumberOfCalls; callId++ )
        {
        const TInt callIndex = iDataStore.CallIndex( callId );
        if ( callIndex != KPEInvalidCallIndex )
            {
            CCallRemotePartyInfo* info = CCallRemotePartyInfo::NewLC();
            info->SetCallIndex( callIndex );
            
            if ( iDataStore.RemoteColpNumber( callId ).Length() )
                {
                info->SetAddressL( iDataStore.RemoteColpNumber( callId ) );
                }
            else
                {
                info->SetAddressL( iDataStore.RemotePhoneNumber( callId ) );    
                }            
            
            info->SetMatchedNameL( iDataStore.RemoteName( callId ) );
            info->SetCompanyNameL( iDataStore.RemoteCompanyName( callId ) );
            info->SetNameFromNetworkL( iDataStore.RemotePartyName( callId ) );
            
            if ( iDataStore.CallOrigin( callId ) == EPECallOriginSAT )
                {
                info->SetSATCall( ETrue );
                }  
            
            info->SetRemoteIdentity( ResolveRemoteIdentity( callId ) );
                        
            infos->AddL( info );
            CleanupStack::Pop( info );
            }
        }
    HBufC8* data = infos->ExternalizeL();
    CleanupStack::PopAndDestroy( infos );
    return data;
    }

// ---------------------------------------------------------------------------
// Sets remote party identity.
// ---------------------------------------------------------------------------
//
MCallRemotePartyInfo::TRemoteIdentityStatus
    TPERemotePartyInfoMediatorUpdater::ResolveRemoteIdentity( const TInt aCallId )
    {    
    MCallRemotePartyInfo::TRemoteIdentityStatus remoteIdentity;
    if ( iDataStore.CallDirection( aCallId ) == RMobileCall::EMobileOriginated  )
        {
        // In MO case Remote party address is always available 
        remoteIdentity = MCallRemotePartyInfo::ERemoteIdentityAvailable;
        }
    else
        {
        remoteIdentity = ConvertRemoteIdentity( aCallId );    
        }                 
    
    return remoteIdentity;
    }

// ---------------------------------------------------------------------------
// Converts remote party identity.
// ---------------------------------------------------------------------------
//
MCallRemotePartyInfo::TRemoteIdentityStatus 
    TPERemotePartyInfoMediatorUpdater::ConvertRemoteIdentity( const TInt aCallId )
    {    
    RMobileCall::TMobileCallRemoteIdentityStatus etelRemoteIdentity = iDataStore.RemoteIdentity( aCallId );    
    MCallRemotePartyInfo::TRemoteIdentityStatus remoteIdentity;
    
    switch ( etelRemoteIdentity )
            {
            case RMobileCall::ERemoteIdentityUnknown:
            case RMobileCall::ERemoteIdentityAvailableNoCliUnavailable:
                {
                remoteIdentity = MCallRemotePartyInfo::ERemoteIdentityUnknown;
                }
                break;
            case RMobileCall::ERemoteIdentityAvailable:
                {
                remoteIdentity = MCallRemotePartyInfo::ERemoteIdentityAvailable;
                }
                break;
            case RMobileCall::ERemoteIdentitySuppressed:
            case RMobileCall::ERemoteIdentityAvailableNoCliRejectedByUser:
                {
                remoteIdentity = MCallRemotePartyInfo::ERemoteIdentityRejectedByUser;
                }
                break;
            case RMobileCall::ERemoteIdentityAvailableNoCliInteractionWithOtherService:
            case RMobileCall::ERemoteIdentityUnavailableNoCliInteractionWithOtherService:
                {
                remoteIdentity = MCallRemotePartyInfo::ERemoteIdentityNoCliInteractionWithOtherService;
                }
                break;
            case RMobileCall::ERemoteIdentityAvailableNoCliCoinOrPayphone:
            case RMobileCall::ERemoteIdentityUnavailableNoCliCoinOrPayphone:
                {
                remoteIdentity = MCallRemotePartyInfo::ERemoteIdentityNoCliCoinOrPayphone;
                }
                break;
            default:
                {
                TEFLOGSTRING( KTAERROR, "TPERemotePartyInfoMediatorUpdater::ConvertRemoteIdentity, UNKNOWN REMOTE IDENTITY!" );
                remoteIdentity = MCallRemotePartyInfo::ERemoteIdentityUnknown;                
                break;
                }
            }    
    return remoteIdentity;
    }

