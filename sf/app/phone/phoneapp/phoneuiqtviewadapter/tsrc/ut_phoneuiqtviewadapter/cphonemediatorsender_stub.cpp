/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneMediatorSender class.
*
*/


// INCLUDE FILES
#include <MediatorDomainUIDs.h>
#include "PhoneUI.hrh"
#include "CPhoneMediatorSender.h"
#include "PhoneLogger.h"
#include "PhoneConstants.h"
#include "PhoneMediatorPackageTypes.h"
#include "phoneui.pan"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamboolean.h"
//#include "mphoneenginemessagesender.h"
#include "pevirtualengine.h"
#include "mphoneshutdowncommand.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneMediatorSender::Instance
// Initializes the singleton object
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C CPhoneMediatorSender* CPhoneMediatorSender::Instance()
    {
    CPhoneMediatorSender* instance = static_cast<CPhoneMediatorSender*> 
        ( CCoeEnv::Static ( KUidMediatorSenderSingleton ) );
    
    if ( !instance )
        {
        TRAPD( err, instance = CPhoneMediatorSender::NewL() );
        if ( err )
	        {
	        Panic( EPhoneMediatorCenterCouldNotCreateSingleton );	
	        }
        }
    return instance;
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorSender::CPhoneMediatorSender
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhoneMediatorSender::CPhoneMediatorSender() :
    CCoeStatic( KUidMediatorSenderSingleton )
    {
    }

// Destructor
CPhoneMediatorSender::~CPhoneMediatorSender()
    {
    delete iCommandInitiator;
    iEvents.Close();
    iGenericEvents.Close(); 
    delete iEventSender;
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorSender::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhoneMediatorSender::ConstructL()
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneMediatorSender::ConstructL( ) ");
   
    }

// -----------------------------------------------------------
// CPhoneMediatorSender::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneMediatorSender* CPhoneMediatorSender::NewL()
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneMediatorSender::NewL( ) ");
    
    CPhoneMediatorSender* self = new ( ELeave ) CPhoneMediatorSender;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorSender::RegisterGenericEvents
// -----------------------------------------------------------------------------
//
void CPhoneMediatorSender::RegisterGenericEvents()
	{
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneMediatorSender::RegisterGenericEvents( ) ");

	}

// -----------------------------------------------------------------------------
// CPhoneMediatorSender::SendEvent
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneMediatorSender::SendEvent( 
    const TPhoneViewCommandId /*aCommandId*/ ) const
    {
    // No one yet intereseted this event.
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorSender::SendEvent
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneMediatorSender::SendEvent( const TPhoneViewCommandId aCommandId, 
	const TInt aCallId ) const
	{

	}

// -----------------------------------------------------------------------------
// CPhoneMediatorSender::SendEvent
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneMediatorSender::SendEvent( const TPhoneViewCommandId aCommandId, 
	TPhoneCommandParam& aCommandParam )
	{

	}

// -----------------------------------------------------------------------------
// CPhoneMediatorSender::SendEvent
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneMediatorSender::SendEvent( const TPhoneViewCommandId aCommandId, 
 	const TInt aCallId, TPhoneCommandParam& aCommandParam ) const
	{
	
	}

// -----------------------------------------------------------------------------
// CPhoneMediatorSender::SendEvent
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneMediatorSender::SendEvent( 
    const TPhoneViewCommandId /*aCommandId*/, 
    const TInt /*aCallId*/, const TDesC* /*aMessage*/ ) const
    {
    // No one interested yet from these events.
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorSender::IssueCommand
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPhoneMediatorSender::IssueCommand( const TUid aDomain, const TUid aCategory, 
    const TInt aCommandId, const TVersion aVersion, const TDesC8& aData, 
    MPhoneShutdownCommand* aShutdownCommand  )
    {
	return KErrNone;	
	}

// -----------------------------------------------------------------------------
// CPhoneMediatorSender::CommandResponseL
// -----------------------------------------------------------------------------
//
void CPhoneMediatorSender::CommandResponseL( TUid aDomain, TUid aCategory, 
	TInt aCommandId, TInt /*aStatus*/, const TDesC8& /*aData*/ )
	{
    // First check for buffered command
	if( iCommandBuffer.iCommandId != KErrNotFound )
		{
		// We have a buffered command waiting
        __PHONELOG( EBasic, EPhoneMediatorCenter, 
            "CPhoneMediatorSender::VideoTelephonyCommandResponse - Buffered Command waiting" );
		if( iCommandBuffer.iCommandId != aCommandId )
			{
			// And it's not identical to the command which response we now received
			// so it's necessary to re-send it
	        __PHONELOG1( EBasic, EPhoneMediatorCenter, 
	            "CPhoneMediatorSender::VideoTelephonyCommandResponse - Resending command %d", iCommandBuffer.iCommandId );
			IssueCommand( iCommandBuffer.iDomainUid,
			              iCommandBuffer.iCategoryUid, 
			              iCommandBuffer.iCommandId,
                          iCommandBuffer.iVersion,
                          KNullDesC8 );
                          
			ResetCommandBuffer();
			}
		}

	}

// -----------------------------------------------------------------------------
// CPhoneMediatorSender::VideoTelephonyCommandResponse
// -----------------------------------------------------------------------------
//
void CPhoneMediatorSender::VideoTelephonyCommandResponse( TInt aCommandId )
    {
 
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorSender::MapCallState
// -----------------------------------------------------------------------------
//
TCallState CPhoneMediatorSender::MapCallState( const TInt aCallState ) const
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneMediatorSender::MapCallState( ) ");
    TCallState callState( ECallStateUnknown );
    
    switch( aCallState )
        {
        case EPEStateUnknown:
            callState = ECallStateUnknown;
            break;
            
        case EPEStateIdle:
            callState = ECallStateIdle;
            break;
            
        case EPEStateDialing:
            callState = ECallStateDialling;
            break;
            
        case EPEStateEmergencyDialing:
            callState = ECallStateEmergencyDialling;
            break;
            
        case EPEStateRinging:
            callState = ECallStateRinging;
            break;
            
        case EPEStateConnecting:
            callState = ECallStateConnecting;
            break;
            
        case EPEStateConnected:
            callState = ECallStateConnected;
            break;
            
        case EPEStateHangingUp:
            callState = ECallStateHangingUp;
            break;
            
        case EPEStateHeld:
            callState = ECallStateHeld;
            break;
            
        case EPEStateAnswering:
            callState = ECallStateAnswering;
            break;
            
        case EPEStateRejecting:
            callState = ECallStateRejecting;
            break;
            
        case EPEStateDisconnecting:
            callState = ECallStateDisconnecting;
            break;
            
        default:
            break;
        }
        
    return callState;
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorSender::MapCallType
// -----------------------------------------------------------------------------
//
TCallType CPhoneMediatorSender::MapCallType( const TInt aCallType ) const
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneMediatorSender::MapCallType( ) ");
    TCallType callType( ECallTypeUninitialized );
    
    switch( aCallType )
        {
        case EPECallTypeUninitialized:
            callType = ECallTypeUninitialized;
            break;
            
        case EPECallTypeCSVoice:
            callType = ECallTypeCSVoice;
            break;
            
        case EPECallTypeVideo:
            callType = ECallTypeVideo;
            break;
            
        case EPECallTypeVoIP:
            callType = ECallTypeVoIP;
            break;
            
        default:
            break;
        }
        
    return callType;
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorSender::ResetCommandBuffer
// -----------------------------------------------------------------------------
//
void CPhoneMediatorSender::ResetCommandBuffer()
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneMediatorSender::ResetCommandBuffer( ) ");
    TUid nullUid = TUid::Null();
    iCommandBuffer.iCommandId = KErrNotFound;
    iCommandBuffer.iDomainUid = nullUid;
    iCommandBuffer.iCategoryUid = nullUid;
    iCommandBuffer.iVersion = TVersion( KErrNotFound, KErrNotFound, KErrNotFound );
    }

// End of File
