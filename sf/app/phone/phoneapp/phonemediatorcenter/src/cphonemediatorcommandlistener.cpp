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
* Description: Implementation of CPhoneMediatorCommandListener class.
*
*/


// INCLUDE FILES
#include <MediatorDomainUIDs.h>
#include "mphonemenuandcbaevents.h"
#include "mphonekeyeventhandler.h"
#include "mphoneviewcommandhandle.h"
#include "cphonemediatorcommandlistener.h"
#include "phonelogger.h"
#include "phonemediatorpackagetypes.h"
#include "cphonestatehandle.h"
#include "tphonecmdparamboolean.h"
#include "videoteltophonecommandsapi.h"
#include "mediatoraudiocommandstotelephonyapi.h"
#include "phoneui.pan"
#include "phoneui.hrh"
#include "pevirtualengine.h"
#include "mpeengineinfo.h"
#include "mediatorcommandstotelephonyapi.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneMediatorCommandListener::Instance
// Initializes the singleton object
// (other items were commented in a header).
// ---------------------------------------------------------
//
CPhoneMediatorCommandListener* CPhoneMediatorCommandListener::Instance()
    {
    CPhoneMediatorCommandListener* instance = static_cast<CPhoneMediatorCommandListener*> 
        ( CCoeEnv::Static ( KUidMediatorCommandListenerSingleton ) );
    
    if ( !instance )
        {
        TRAPD( err, instance = CPhoneMediatorCommandListener::NewL() );
        if ( err )
            {
            Panic( EPhoneUtilsCouldNotCreateSingleton );    
            }
        }
    return instance;
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorCommandListener::CPhoneMediatorCommandListener
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhoneMediatorCommandListener::CPhoneMediatorCommandListener() :
    CCoeStatic( KUidMediatorCommandListenerSingleton )
    {
    }

// Destructor
CPhoneMediatorCommandListener::~CPhoneMediatorCommandListener()
    {
    if ( iCommandResponder )
        {
        iCommandResponder->UnregisterCommand( KMediatorTelephonyDomain, 
                                              KCatVideoTelToPhoneCommands, 
                                              iVideoTelCommands );

        iCommandResponder->UnregisterCommand( KMediatorTelephonyDomain, 
                                              KCatAudioCommandsToTelephony, 
                                              iAudioCommands );

        iCommandResponder->UnregisterCommand( KMediatorTelephonyDomain, 
                                              KCatCommandsToTelephony, 
                                              iGenericCommands );

        delete iCommandResponder;
        iCommandResponder = NULL;
        }
    iVideoTelCommands.Close();
    iAudioCommands.Close();
    iGenericCommands.Close();
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorCommandListener::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhoneMediatorCommandListener::ConstructL()
    {
    __LOGMETHODSTARTEND(EPhoneMediatorCenter, "CPhoneMediatorCommandListener::ConstructL( ) ");

    RegisterMediatorCommandsL();
    }

// -----------------------------------------------------------
// CPhoneMediatorCommandListener::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneMediatorCommandListener* CPhoneMediatorCommandListener::NewL()
    {
    __LOGMETHODSTARTEND(EPhoneMediatorCenter, "CPhoneMediatorCommandListener::NewL( ) ");
    CPhoneMediatorCommandListener* self = new( ELeave ) CPhoneMediatorCommandListener();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorCommandListener::Initialize
// -----------------------------------------------------------------------------
//    
void CPhoneMediatorCommandListener::Initialize( 
    MPhoneMenuAndCbaEvents* aMenuAndCbaEventHandler,
    MPhoneEngineMessageSender* aMessageSender,
    MPEEngineInfo* aEngineInfo )
    {
    __LOGMETHODSTARTEND(EPhoneMediatorCenter, "CPhoneMediatorCommandListener::Initialize( ) ");
    iMenuAndCbaHandler = aMenuAndCbaEventHandler;
    iMessageSender = aMessageSender;
    iEngineInfo = aEngineInfo;
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorCommandListener::RegisterMediatorCommandsL
// -----------------------------------------------------------------------------
//
void CPhoneMediatorCommandListener::RegisterMediatorCommandsL()
    {
    __LOGMETHODSTARTEND(EPhoneMediatorCenter, 
        "CPhoneMediatorCommandListener::RegisterMediatorCommandsL( ) ");
    iCommandResponder = CMediatorCommandResponder::NewL( this );

    RegisterVideoTelephonyMediatorCommands();
    RegisterAudioMediatorCommands();
    RegisterGenericMediatorCommands();                                                
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorCommandListener::RegisterVideoTelephonyMediatorCommands
// -----------------------------------------------------------------------------
//
void CPhoneMediatorCommandListener::RegisterVideoTelephonyMediatorCommands()
    {
    __LOGMETHODSTARTEND(EPhoneMediatorCenter, 
        "CPhoneMediatorCommandListener::RegisterVideoTelephonyMediatorCommands( ) ");
    TCapabilitySet caps;
    caps.SetEmpty();

    // Video Telephony Commands
    MediatorService::TCommand newCommand;
    newCommand.iCommandId = EVtCmdLowMemory;
    newCommand.iVersion = TVersion( KVideoTelToPhoneCmdVersionMajor, 
        KVideoTelToPhoneCmdVersionMinor, KVideoTelToPhoneCmdVersionBuild );
    caps.Set( ECapabilityNetworkControl );
    newCommand.iCaps = caps;
    newCommand.iTimeout = KPhoneUiMediatorIfTimeout;    
    iVideoTelCommands.Append( newCommand );
    
    // Register Video Telephony Commands
    TInt error = iCommandResponder->RegisterCommand( KMediatorTelephonyDomain, 
                                                     KCatVideoTelToPhoneCommands, 
                                                     iVideoTelCommands );
    __ASSERT_DEBUG( error == KErrNone, Panic( EPhoneMediatorCenterRegistrationFailed ) );
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorCommandListener::RegisterAudioMediatorCommands
// -----------------------------------------------------------------------------
//
void CPhoneMediatorCommandListener::RegisterAudioMediatorCommands()
    {
    __LOGMETHODSTARTEND(EPhoneMediatorCenter, 
        "CPhoneMediatorCommandListener::RegisterAudioMediatorCommands( ) ");
    TCapabilitySet caps;
    caps.SetEmpty();

    // Audio Commands
    MediatorService::TCommand newCommand;
    newCommand.iCommandId = EAudioCmdUnmute;
    newCommand.iVersion = TVersion( KAudioCmdToTelephonyVersionMajor, 
        KAudioCmdToTelephonyVersionMinor, KAudioCmdToTelephonyVersionBuild );
    caps.Set( ECapabilityNetworkControl );
    newCommand.iCaps = caps;
    newCommand.iTimeout = KPhoneUiMediatorIfTimeout;    
    iAudioCommands.Append( newCommand );

    newCommand.iCommandId = EAudioCmdMute;
    iAudioCommands.Append( newCommand );

    // Register Audio Commands
    TInt error = iCommandResponder->RegisterCommand( KMediatorTelephonyDomain, 
                                                     KCatAudioCommandsToTelephony, 
                                                     iAudioCommands );
    __ASSERT_DEBUG( error == KErrNone, Panic( EPhoneMediatorCenterRegistrationFailed ) );
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorCommandListener::RegisterGenericMediatorCommands
// -----------------------------------------------------------------------------
//
void CPhoneMediatorCommandListener::RegisterGenericMediatorCommands()
    {
    __LOGMETHODSTARTEND(EPhoneMediatorCenter, 
        "CPhoneMediatorCommandListener::RegisterGenericMediatorCommands( ) ");
    TCapabilitySet caps;
    caps.SetEmpty();

    // Generic Commands
    MediatorService::TCommand newCommand;
    newCommand.iCommandId = EPhoneCmdEndActiveCall;
    newCommand.iVersion = TVersion( KTelephonyCommandsVersionMajor, 
        KTelephonyCommandsVersionMinor, KTelephonyCommandsVersionBuild );
    caps.Set( ECapabilityNetworkControl );
    newCommand.iCaps = caps;
    newCommand.iTimeout = KPhoneUiMediatorIfTimeout;    
    iGenericCommands.Append( newCommand );

    // Register Generic Commands
    TInt error = iCommandResponder->RegisterCommand( KMediatorTelephonyDomain, 
                                                     KCatCommandsToTelephony, 
                                                     iGenericCommands );
    __ASSERT_DEBUG( error == KErrNone, Panic( EPhoneMediatorCenterRegistrationFailed ) );
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorCommandListener::MediatorCommandL
// -----------------------------------------------------------------------------
//
void CPhoneMediatorCommandListener::MediatorCommandL( 
    TUid aDomain, 
    TUid aCategory, 
    TInt aCommandId, 
    TVersion aVersion, 
    const TDesC8& /*aData*/ )
    {
    __LOGMETHODSTARTEND(EPhoneMediatorCenter, "CPhoneMediatorCommandListener::MediatorCommandL( ) ");
    if( aDomain == KMediatorTelephonyDomain &&
        aCategory == KCatVideoTelToPhoneCommands )
        {
        VideoTelephonyCommandL( aCommandId, aVersion );
        SendResponse( aDomain, aCategory, aCommandId );
        return;         
        }
    else if( aDomain == KMediatorTelephonyDomain &&
             aCategory == KCatAudioCommandsToTelephony )
        {
        AudioCommandL( aCommandId, aVersion );
        SendResponse( aDomain, aCategory, aCommandId );
        return;
        }
    else if( aDomain == KMediatorTelephonyDomain &&
             aCategory == KCatCommandsToTelephony )
        {
        GenericCommandL( aCommandId, aVersion );
        SendResponse( aDomain, aCategory, aCommandId );
        return;         
        }
    
    TInt error = iCommandResponder->IssueResponse( aDomain, 
                                                 aCategory, 
                                                 aCommandId,
                                                 KErrNotFound, 
                                                 KNullDesC8 );                              
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorCommandListener::CancelMediatorCommand
// -----------------------------------------------------------------------------
//
void CPhoneMediatorCommandListener::CancelMediatorCommand( 
    TUid /*aDomain*/,
    TUid /*aCategory*/, 
    TInt /*aCommandId*/ )
    {
    __LOGMETHODSTARTEND(EPhoneMediatorCenter, 
        "CPhoneMediatorCommandListener::CancelMediatorCommand( ) ");
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorCommandListener::SendResponse
// -----------------------------------------------------------------------------
//
void CPhoneMediatorCommandListener::SendResponse( 
    TUid aDomain, 
    TUid aCategory, 
    TInt aCommandId )
    {
    __LOGMETHODSTARTEND(EPhoneMediatorCenter, 
        "CPhoneMediatorCommandListener::SendResponse( ) ");
    iCommandResponder->IssueResponse( aDomain, 
                                      aCategory, 
                                      aCommandId,
                                      KErrNone, 
                                      KNullDesC8 );     
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorCommandListener::VideoTelephonyCommandL
// -----------------------------------------------------------------------------
//
void CPhoneMediatorCommandListener::VideoTelephonyCommandL( TInt aCommandId, TVersion /*aVersion*/ )
    {
    __LOGMETHODSTARTEND(EPhoneMediatorCenter, 
        "CPhoneMediatorCommandListener::VideoTelephonyCommandL( ) ");
        
    __ASSERT_DEBUG( iMenuAndCbaHandler, Panic( EPhoneMediatorCenterSingletonNotInitialized ) );
    __ASSERT_DEBUG( iMessageSender, Panic( EPhoneMediatorCenterSingletonNotInitialized ) );
    
    switch( aCommandId )
        {
        case EVtCmdLowMemory:
            iMenuAndCbaHandler->HandleCommandL( EPhoneCmdVideoCallOutOfMemory );
            break;
            
        default:
            __ASSERT_DEBUG( false, Panic( EPhoneMediatorCenterInvalidCommand ) );
            break;
        }       
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorCommandListener::AudioCommandL
// -----------------------------------------------------------------------------
//
void CPhoneMediatorCommandListener::AudioCommandL( TInt aCommandId, TVersion /*aVersion*/ )
    {
    __LOGMETHODSTARTEND(EPhoneMediatorCenter, 
        "CPhoneMediatorCommandListener::AudioCommandL( ) ");

    __ASSERT_DEBUG( iMessageSender, Panic( EPhoneMediatorCenterSingletonNotInitialized ) );
    __ASSERT_DEBUG( iEngineInfo, Panic( EPhoneMediatorCenterSingletonNotInitialized ) );
        
    switch( aCommandId )
        {
        case EAudioCmdUnmute:
            iEngineInfo->SetAudioMuteCommand( false );
            iMessageSender->SendPhoneEngineMessage( MPEPhoneModel::EPEMessageSetAudioMute );
            break;
            
        case EAudioCmdMute:
            iEngineInfo->SetAudioMuteCommand( true );
            iMessageSender->SendPhoneEngineMessage( MPEPhoneModel::EPEMessageSetAudioMute );
            break;
            
        default:
            __ASSERT_DEBUG( false, Panic( EPhoneMediatorCenterInvalidCommand ) );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CPhoneMediatorCommandListener::GenericCommandL
// -----------------------------------------------------------------------------
//
void CPhoneMediatorCommandListener::GenericCommandL( TInt aCommandId, TVersion /*aVersion*/ )
    {
    __LOGMETHODSTARTEND(EPhoneMediatorCenter, 
        "CPhoneMediatorCommandListener::GenericCommandL( ) ");
    __ASSERT_DEBUG( iMenuAndCbaHandler, Panic( EPhoneMediatorCenterSingletonNotInitialized ) );

    switch( aCommandId )
        {
        case EPhoneCmdEndActiveCall:
            iMenuAndCbaHandler->HandleCommandL( EPhoneInCallCmdEndThisActiveCall );
            break;
            
        default:
            __ASSERT_DEBUG( false, Panic( EPhoneMediatorCenterInvalidCommand ) );
            break;
        }
    }

// End of File
