/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Main class of CS Call Plug-in
*
*/


// EXTERNAL INCLUDES
#include <e32base.h>
#include <mccpconferencecallobserver.h>
#include <mmtsy_names.h>
#include <mccpssobserver.h>
#include <rmmcustomapi.h>
#include <startupdomainpskeys.h>
#include <mccecall.h>

// USER INCLUDES
#include "csprovider.h"
#include "cspvoicecall.h"
#include "cspvideocall.h"
#include "cspetelincomingcallmonitor.h"
#include "cspetelconferencestatusmonitor.h"
#include "cspdtmfprovider.h"
#include "cspconferencecall.h"
#include "cspcallarray.h"
#include "cspservicesettingshandler.h"
#include "cspaudiohandler.h"
#include "cspetelcallwaitingrequester.h"
#include "cspsupplementaryservicesmonitor.h"
#include "cspcipheringstatusmonitor.h"
#include "cspsssettingshandler.h"
#include "cspcalladdedhandler.h"
#include "csppubsublistener.h"
#include "cspcallcommandhandler.h"
#include "cspremotealertingtonelistener.h"
#include "csppanic.pan"
#include "csplogger.h"
#include "cspconsts.h"

const TInt KCSServiceId = 1; 

// ---------------------------------------------------------------------------
// CSProvider::CSProvider
// ---------------------------------------------------------------------------
//
CSProvider::CSProvider(): iImplementationUid( KCSPImplementationUid ),
        iCCPObserver( NULL ),
        iSsObserver( NULL ),
        iDTMFProvider( NULL ),
        iIncomingVoiceCallMonitor( NULL ),
        iIncomingDataCallMonitor( NULL ),
        iIncomingAuxCallMonitor( NULL ),
        iCallArray( NULL ),
        iServiceHandler( NULL ),
        iInitialized( EFalse ),
        iCallAddedHandler( NULL ),
        iServiceId( 0 ),
        iAudioHandler( NULL ),
        iCwRequester( NULL ),
        iSsMonitor( NULL ),
        iCipheringStatusMonitor( NULL ),
        iSsSettingsHandler( NULL ),
        iSimStatusListener( NULL ),
        iCallCommandHandler( NULL ),
        iRemoteAlertingToneListener( NULL ),
        iConferenceStatusMonitor( NULL ),
        iConferenceCall( NULL )
    {
    CSPLOGSTRING(CSPOBJECT, "CSProvider::CSProvider");
    }

// ---------------------------------------------------------------------------
// CSProvider::~CSProvider
// ---------------------------------------------------------------------------
//
CSProvider::~CSProvider()
    {
    CSPLOGSTRING(CSPOBJECT, "CSProvider::~CSProvider <");
    delete iCallArray;
    
    if ( iInitialized )
        {
        delete iConferenceStatusMonitor; 
        delete iCallCommandHandler;
        delete iRemoteAlertingToneListener;
        delete iSimStatusListener;
        delete iSsSettingsHandler;
        delete iAudioHandler;        
        delete iSsMonitor;
        delete iCwRequester;
        delete iServiceHandler;
        delete iIncomingVoiceCallMonitor;
        delete iIncomingDataCallMonitor;
        delete iIncomingAuxCallMonitor;
        delete iCallAddedHandler;
        delete iCipheringStatusMonitor;
        delete iDTMFProvider;

        CSPLOGSTRING(CSPOBJECT, "CSProvider::~CSProvider close lines");
        iLineContainer.Close();
        CSPLOGSTRING(CSPOBJECT, "CSProvider::~CSProvider close customapi");
        iMmCustom.Close();
        CSPLOGSTRING(CSPOBJECT, "CSProvider::~CSProvider close conference call");
        iMobileConferenceCall.Close();
        CSPLOGSTRING(CSPOBJECT, "CSProvider::~CSProvider close phone");
        iMobilePhone.Close();
        CSPLOGSTRING(CSPOBJECT, "CSProvider::~CSProvider unload phone");
        iServer.UnloadPhoneModule( KMmTsyModuleName );
        CSPLOGSTRING(CSPOBJECT, "CSProvider::~CSProvider close server session");
        iServer.Close();
        }

    CSPLOGSTRING(CSPOBJECT, "CSProvider::~CSProvider >");
    }

// ---------------------------------------------------------------------------
// CSProvider::ConstructL
// ---------------------------------------------------------------------------
//
void CSProvider::ConstructL()
    {
    CSPLOGSTRING(CSPOBJECT, "CSProvider::ConstructL <");
    iCallArray = CSPCallArray::NewL();
    CSPLOGSTRING(CSPOBJECT, "CSProvider::ConstructL >");
    }

// ---------------------------------------------------------------------------
// CSProvider::NewL
// ---------------------------------------------------------------------------
//
CSProvider* CSProvider::NewL()
    {
    CSPLOGSTRING(CSPOBJECT, "CSProvider::NewL() <");
    CSProvider* self = new ( ELeave ) CSProvider();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    CSPLOGSTRING(CSPOBJECT, "CSProvider::NewL() >");
    return self;
    }

// ---------------------------------------------------------------------------
// CSProvider::NotifySsEvent
// ---------------------------------------------------------------------------
//
void CSProvider::NotifySsEvent( 
                    RMmCustomAPI::TSsTypeAndMode& aSsTypeAndMode,
                    RMmCustomAPI::TSsInfo& aSsInfo )
    {
    CSPLOGSTRING(CSPINT, "CSProvider::NotifySsEvent <");
    const TDesC& addr = KNullDesC;
    
    switch( aSsTypeAndMode.iSsType )
        {
        case RMmCustomAPI::ESsAllSs:
            {
            CSPLOGSTRING(CSPINT, "CSProvider::NotifySsEvent ESsAllSs");
            if( iSsSettingsHandler )
                {
                TInt cugIndex( aSsInfo.iCugIndex );
                                
                if ( iSsSettingsHandler->IsValueValidCugIndex( cugIndex ) ) 
                    {
                    CSPLOGSTRING(CSPINT, "CSProvider::NotifySsEvent CallCugEventOccurred");
                    iSsObserver->CallCugEventOccurred(  
                            MCCPSsObserver::ECCPSsShowIncCallGroupIndex );
                    }  
                else 
                    {
                    CSPLOGSTRING2(CSPERROR, 
                            "CSProvider::NotifySsEvent err invalid cug index %d", cugIndex );
                    }
                }
            break; 
            }
        
        case RMmCustomAPI::ESsRegPassword:
        case RMmCustomAPI::ESsClip:
        case RMmCustomAPI::ESsClir:
        case RMmCustomAPI::ESsCnap:
        case RMmCustomAPI::ESsColp:
        case RMmCustomAPI::ESsColr:
            {
            CSPLOGSTRING(CSPINT, "CSProvider::NotifySsEvent Discarding");
            break; 
            }

        case RMmCustomAPI::ESsCallWaiting:
            {
            CSPLOGSTRING(CSPINT, "CSProvider::NotifySsEvent ESsCallWaiting");
            iSsObserver->CallForwardEventOccurred( 
                                MCCPSsObserver::ESsCallWaiting,
                                addr );
            break; 
            }        
            
        case RMmCustomAPI::ESsAllForwardings:
        case RMmCustomAPI::ESsForwUncond:
        case RMmCustomAPI::ESsForwBusy:
        case RMmCustomAPI::ESsForwNoReply:
        case RMmCustomAPI::ESsForwNoReach:
        case RMmCustomAPI::ESsAllCondForwardings:
            {
            switch( aSsInfo.iForwMode )
                {
                case RMmCustomAPI::ESsIncCallIsForw:
                    iSsObserver->CallForwardEventOccurred( 
                                        MCCPSsObserver::ECCPSsIncCallIsForw,
                                        addr );
                    break;
                case RMmCustomAPI::ESsIncCallForwToC:
                    iSsObserver->CallForwardEventOccurred( 
                                        MCCPSsObserver::ECCPSsIncCallForwToC,
                                        addr );
                    break;
                case RMmCustomAPI::ESsOutCallForwToC:
                    iSsObserver->CallForwardEventOccurred( 
                                        MCCPSsObserver::ECCPSsOutCallForwToC,
                                        addr );
                    break;
                default:
                    {
                    HandleDivertOrBarring( addr, aSsTypeAndMode );
                    break; 
                    }
                }
            break;
            }

        case RMmCustomAPI::ESsAllBarrings:  
        case RMmCustomAPI::ESsBarrAllOut:
        case RMmCustomAPI::ESsBarrOutInter:
        case RMmCustomAPI::ESsBarrOutInterExcHome:
        case RMmCustomAPI::ESsOutgoingBarrServ:
            {            
            iSsObserver->BarringEventOccurred( 
                                    MCCPSsObserver::ECCPSsOutgoingCallBarred );
            break;
            }

        case RMmCustomAPI::ESsBarrAllIn:
        case RMmCustomAPI::ESsBarrAllInRoam:
        case RMmCustomAPI::ESsIncomingBarrServ:
            {            
            iSsObserver->BarringEventOccurred( 
                                    MCCPSsObserver::ECCPSsIncomingCallBarred );
            break;
            }

        default:
            {
            CSPLOGSTRING(CSPINT, "CSProvider::NotifySsEvent Unknown SS");
            break; 
            }
        }
        
    CSPLOGSTRING(CSPINT, "CSProvider::NotifySsEvent >");    
    }

// ---------------------------------------------------------------------------
// CSProvider::InitializeL for testing purposes
// ---------------------------------------------------------------------------
//
void CSProvider::InitializeL( const MCCPObserver& aObserver,
                              const MCCPSsObserver& aSsObserver,
                              CSPAudioHandlerBase* aAudioHandler )
    {
    
    if (!iAudioHandler)
        {        
        iAudioHandler = aAudioHandler;
        }
    
    InitializeL( aObserver, aSsObserver );    
    }

// From CConvergedCallProvider
// ---------------------------------------------------------------------------
// CSProvider::InitializeL
// ---------------------------------------------------------------------------
//
void CSProvider::InitializeL( const MCCPObserver& aObserver,
                              const MCCPSsObserver& aSsObserver )
    {
    CSPLOGSTRING(CSPOBJECT, "CSProvider::InitializeL <");
    if ( iInitialized )
        {
        CSPLOGSTRING(CSPOBJECT, 
                "CSProvider::InitializeL() Error already initialized");
        User::Leave( KErrAlreadyExists );
        }
    iInitialized = ETrue;
    
    CreateEtelConnectionsL();
    
    iServiceHandler = CSPServiceSettingsHandler::NewL();

    TInt readErr = iServiceHandler->ReadCSServiceId( iServiceId );
    if ( readErr )
        {
        CSPLOGSTRING(CSPERROR, "CSProvider::InitializeL()\
                ERROR COULD NOT READ SERVICE ID FOR CS-PLUGIN ");
        CSPLOGSTRING(CSPERROR, "CSProvider::InitializeL() \
                Please ensure that SPSettings is initialized corretly! ");
        iServiceId = KCSServiceId; 
        }
    
    // Save CCP observer as pointer-type member 
    const MCCPCSObserver& obs = static_cast<const MCCPCSObserver&>(aObserver);
    iCCPObserver = const_cast<MCCPCSObserver*>(&obs);
    
    // Save CCP SS observer as pointer-type member 
    const MCCPSsObserver& ssObs = static_cast<const MCCPSsObserver&>(aSsObserver);
    iSsObserver = const_cast<MCCPSsObserver*>(&ssObs);
    
    RPhone::TLineInfo lineInfo;
    __ASSERT_ALWAYS( iMobilePhone.GetLineInfo( 0, lineInfo )
        == KErrNone, Panic( ECSPPanicNoEtel ) );
    
    // Open Lines. At least voice line must be opened.
    __ASSERT_ALWAYS( iLineContainer.Open( iMobilePhone, iServiceId ) == KErrNone,
        Panic( ECSPPanicNoEtel ) );

    RMobileLine& voiceLine = iLineContainer.LineByType( 
                                RCSPLineContainer::ECSPLineSpeech );
    
    // Create and start incoming voice call monitor for primary line
    iIncomingVoiceCallMonitor = CSPEtelIncomingCallMonitor::NewL( *this, 
                                      voiceLine,
                                      RCSPLineContainer::ECSPLineSpeech );
    iIncomingVoiceCallMonitor->StartMonitoring();

    // Create and start incoming data call monitor for data line    
    RMobileLine& dataLine = iLineContainer.LineByType( RCSPLineContainer::ECSPLineData );
    iIncomingDataCallMonitor = CSPEtelIncomingCallMonitor::NewL( *this, dataLine,
                                      RCSPLineContainer::ECSPLineData );
    iIncomingDataCallMonitor->StartMonitoring();
    
    // Create and start incoming call monitor for auxilary line    
    RMobileLine& auxLine = iLineContainer.LineByType( RCSPLineContainer::ECSPLineAuxSpeech );
    iIncomingAuxCallMonitor = CSPEtelIncomingCallMonitor::NewL( *this, auxLine,
                                      RCSPLineContainer::ECSPLineAuxSpeech );
    iIncomingAuxCallMonitor->StartMonitoring();

    iCwRequester = CSPEtelCallWaitingRequester::NewL( iMobilePhone );
    
    // Start conference call monitor for monitoring external conference creation  
    iConferenceStatusMonitor = CSPEtelConferenceStatusMonitor::NewL( *this, 
                                    iMobileConferenceCall );
    iConferenceStatusMonitor->StartMonitoring();

    CSPLOGSTRING(CSPOBJECT, "CSProvider::InitializeL create DTMF provider");
    iDTMFProvider = CSPDTMFProvider::NewL( iMobilePhone, iMmCustom );
    
    // Create and start ciphering status monitor    
    iCipheringStatusMonitor = CSPCipheringStatusMonitor::NewL( iMmCustom, *this );
    iCipheringStatusMonitor->StartMonitoring();

    // Create audio handler
    CSPLOGSTRING(CSPOBJECT, "CSProvider::InitializeL create audio handler");
    if (!iAudioHandler)
        {
        iAudioHandler = CSPAudioHandler::NewL();
        }
    
    // Create call added handler for listening ext call creation
    iCallAddedHandler = CSPCallAddedHandler::NewL( 
        obs, 
        iLineContainer, 
        *iCallArray,
        *this,
        *iAudioHandler );
    
    iSimStatusListener = CSPPubSubListener::NewL( 
        KPSUidStartup, 
        KPSSimStatus, 
        this );
    
    HandleSIMStatusL();
    
    iRemoteAlertingToneListener = 
        CSPRemoteAlertingToneListener::NewL( iMmCustom, *this );
    iRemoteAlertingToneListener->StartListening();

    iCallCommandHandler = CSPCallCommandHandler::NewL();
    
    CSPLOGSTRING(CSPOBJECT, "CSProvider::InitializeL >");
    }

// ---------------------------------------------------------------------------
// CSProvider::NewCallL creates MO call.
// ---------------------------------------------------------------------------
//
MCCPCall* CSProvider::NewCallL( const CCCPCallParameters& aCallParameters,
                                const TDesC& aRemoteParty, 
                                const MCCPCallObserver& aObserver )
    {
    CSPLOGSTRING(CSPREQIN, "CSProvider::NewCallL <");
    
    const CCCECallParameters& parameters = 
        reinterpret_cast<const CCCECallParameters&> (aCallParameters);
    
    TUint32 serviceId = aCallParameters.ServiceId();

    if ( serviceId != iServiceId )
        {
        CSPLOGSTRING(CSPERROR, "CSProvider::NewCallL ERROR, service id not acceptable!");
        User::Leave( ECCPErrorInvalidPhoneNumber );
        }
    
    RCSPLineContainer::TCSPLineId lineId = 
        iLineContainer.ResolveLineIdL( parameters );

    RMobileLine& line = iLineContainer.LineByType( lineId );
    
    CSPCall* call = NULL;
    
    if ( RCSPLineContainer::ECSPLineSpeech == lineId ||
         RCSPLineContainer::ECSPLineAuxSpeech == lineId )
        {
        call = CSPVoiceCall::NewL( const_cast<TDesC16&>(aRemoteParty), 
                            line, 
                            ETrue,
                            parameters,
                            *this,
                            EFalse );
        CleanupStack::PushL( call );
        }
        
    // Create CSPDataCall object for data/video call
    else if ( RCSPLineContainer::ECSPLineData == lineId )
        {
        call = CSPVideoCall::NewL( 
                            const_cast<TDesC16&>(aRemoteParty),
                            line, ETrue, parameters,
                            *this );
        CleanupStack::PushL( call );
        }
    else if ( RCSPLineContainer::ECSPLineFax == lineId )
        {
        CSPLOGSTRING(CSPERROR, 
            "CSProvider::NewCallL ERROR FAX is unsupported call type");
        User::Leave( KErrNotSupported );
        }
    
    if ( aRemoteParty.Length() == 0 )
        {
        CSPLOGSTRING(CSPERROR, "CSProvider::NewCallL ERROR: aRemoteParty.Length ==0");
         
        User::Leave( ECCPErrorInvalidPhoneNumber );
        }
    
    call->AddObserverL( aObserver );        
    TInt err = iCallArray->Add( call );
    User::LeaveIfError( err );

    call->SetAudioHandler( iAudioHandler );
    
    CleanupStack::Pop( call );
    
    CSPLOGSTRING(CSPOBJECT, "CSProvider::NewCallL >");
    return call;
    }

// ---------------------------------------------------------------------------
// CSProvider::ReleaseCall
// ---------------------------------------------------------------------------
//
TInt CSProvider::ReleaseCall( MCCPCall& aCall )
    {
    CSPLOGSTRING2(CSPREQIN, "CSProvider::ReleaseCall %d", &aCall);
    CSPCall* call = static_cast<CSPCall*>(&aCall);
    TInt err = iCallArray->Remove( call );
    if ( err == KErrNone )
        {
        delete call;
        }
    return err;
    }

// ---------------------------------------------------------------------------
// CSProvider::NewEmergencyCallL
// ---------------------------------------------------------------------------
//
MCCPEmergencyCall* CSProvider::NewEmergencyCallL( const TUint32 aServiceId,
                                                  const TDesC& aAddress,
                                           const MCCPCallObserver& aObserver )
    {
    CSPLOGSTRING(CSPOBJECT, "CSProvider::NewEmergencyCallL <");

    RMobileLine& voiceLine = iLineContainer.LineByType( 
                                    RCSPLineContainer::ECSPLineSpeech );
    TBuf8<KCCEBearerMaxSize> emptyBearer;
    TBuf<KCCESubAddressMaxSize> emptySubAddress;
    
    CCCECallParameters* tmpParams = CCCECallParameters::NewL();
    CleanupStack::PushL( tmpParams );
    tmpParams->SetServiceId(aServiceId);
    CSPVoiceCall* call = CSPVoiceCall::NewL( aAddress,
                                             voiceLine,
                                             ETrue,
                                             *tmpParams,
                                             *this,
                                             ETrue);
        
    CleanupStack::PopAndDestroy( tmpParams );
    CleanupStack::PushL( call );
    
    call->AddObserverL( aObserver );
    TInt err = iCallArray->Add( call );
    User::LeaveIfError( err );

    call->SetAudioHandler( iAudioHandler );
    
    CleanupStack::Pop( call );    
    
    CSPLOGSTRING(CSPOBJECT, "CSProvider::NewEmergencyCallL >");
    return call;
    }

// ---------------------------------------------------------------------------
// CSProvider::ReleaseEmergencyCall
// ---------------------------------------------------------------------------
//
TInt CSProvider::ReleaseEmergencyCall( MCCPEmergencyCall& aCall )
    {
    CSPLOGSTRING(CSPOBJECT, "CSProvider::ReleaseEmergencyCall");    
    CSPVoiceCall* call = static_cast<CSPVoiceCall*>(&aCall);
    iCallArray->Remove( call );
    delete call;
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CSProvider::NewConferenceL
// ---------------------------------------------------------------------------
//
MCCPConferenceCall* CSProvider::NewConferenceL( 
                        const TUint32 /*aServiceId*/,
                        const MCCPConferenceCallObserver& aObserver )
    {   
    CSPLOGSTRING(CSPREQIN, "CSProvider::NewConferenceL <");
    if ( !iConferenceCall )
        {
        iConferenceCall = CSPConferenceCall::NewL( 
                                    iMobilePhone, *iCallArray, iServiceId );    
        iConferenceCall->AddObserverL( aObserver );
        }
    else 
        {
        CSPLOGSTRING(CSPERROR, "CSProvider::NewConferenceL()\
                Error conference already exists");
        User::Leave( KErrAlreadyExists ); 
        }
    CSPLOGSTRING(CSPREQOUT, "CSProvider::NewConferenceL >");
    return iConferenceCall;
    }

// ---------------------------------------------------------------------------
// CSProvider::ReleaseConferenceCall
// ---------------------------------------------------------------------------
//
TInt CSProvider::ReleaseConferenceCall( MCCPConferenceCall& aCall )
    {
    CSPLOGSTRING(CSPREQIN, "CSProvider::ReleaseConferenceCall");
    TInt ret( KErrNone ); 
    if ( &aCall == iConferenceCall )
        {
        delete iConferenceCall;
        iConferenceCall = NULL;
        }
    else
        {
        CSPLOGSTRING(CSPERROR, 
                "CSProvider::ReleaseConferenceCall Error call not found");
        ret = KErrNotFound; 
        }
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CSProvider::Uid
// ---------------------------------------------------------------------------
//
const TUid& CSProvider::Uid() const
    {
    CSPLOGSTRING2(CSPREQIN, "CSProvider::Uid uid: %d", iImplementationUid);
    return iImplementationUid;
    }

// ---------------------------------------------------------------------------
// CSProvider::Caps
// ---------------------------------------------------------------------------
//
TUint32 CSProvider::Caps( ) const
    {
    CSPLOGSTRING(CSPREQIN, "CSProvider::Caps");
    return NULL;
    }

// ---------------------------------------------------------------------------
// CSProvider::DTMFProvider
// ---------------------------------------------------------------------------
//
MCCPDTMFProvider* CSProvider::DTMFProviderL( 
            const MCCPDTMFObserver& aObserver )
    {
    CSPLOGSTRING2(CSPREQIN, 
            "CSProvider::DTMFProvider observer: %x", &aObserver);
    iDTMFProvider->AddObserverL( aObserver );
    CSPLOGSTRING(CSPREQIN, "CSProvider::DTMFProvider observer added");
    return iDTMFProvider;
    }

// ---------------------------------------------------------------------------
// CSProvider::ExtensionProvider
// ---------------------------------------------------------------------------
//
MCCPExtensionProvider* CSProvider::ExtensionProviderL( 
                            const MCCPExtensionObserver& /*aObserver*/)
    {
    CSPLOGSTRING(CSPREQIN, "CSProvider::ExtensionProvider");
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// CSProvider::GetLifeTime
// ---------------------------------------------------------------------------
//
TBool CSProvider::GetLifeTime( TDes8& aLifeTimeInfo )
    {
    CSPLOGSTRING(CSPREQIN, "CSProvider::GetLifeTime");
    TInt err = iMmCustom.GetLifeTime( aLifeTimeInfo );    
    if ( err ) 
        {
        return EFalse;
        }
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CSProvider::GetCSInfo
// ---------------------------------------------------------------------------
//
TBool CSProvider::GetCSInfo( CSInfo& aCSInfo )
    {
    CSPLOGSTRING(CSPREQIN, "CSProvider::GetCSInfo");
    if ( iInitialized )
        {
        RMobilePhone::TMobilePhoneIdentityV1 imei;
        TRequestStatus reqStatus( KErrNone );
        
        CSPLOGSTRING(CSPREQOUT, 
                "CSProvider::GetCSInfo request get phone id");
        iMobilePhone.GetPhoneId( reqStatus, imei );
        User::WaitForRequest( reqStatus );
        CSPLOGSTRING(CSPREQOUT, 
                "CSProvider::GetCSInfo completed get phone id");
        
        if ( reqStatus.Int() != KErrNone )
             {
             imei.iSerialNumber.Zero();
             } 
             
        aCSInfo.iSerialNumber =  imei.iSerialNumber; 
        return reqStatus.Int() == KErrNone;
        }
        
    return iInitialized;
    }
        
// ---------------------------------------------------------------------------
// From MCSPIncomingCallObserver
// CSProvider::IncomingCallArrived
// ---------------------------------------------------------------------------
//
void CSProvider::IncomingCallArrived( RMobileLine& aLine, const TName& aCallName,
                                      RCSPLineContainer::TCSPLineId aLineId )
    {
    RMobileLine::TLineInfo lineInfo;
    aLine.GetInfo( lineInfo );
    CSPLOGSTRING2(CSPINT, "CSProvider::IncomingCallArrived < line id   %d", aLineId );
    CSPLOGSTRING2(CSPINT, "CSProvider::IncomingCallArrived call name %S", &aCallName );

    CSPLOGSTRING2(CSPINT, "CSProvider::IncomingCallArrived answ %S", &lineInfo.iNameOfCallForAnswering );
    CSPLOGSTRING2(CSPINT, "CSProvider::IncomingCallArrived last %S", &lineInfo.iNameOfLastCallAdded );
    CSPLOGSTRING2(CSPINT, "CSProvider::IncomingCallArrived status %d", lineInfo.iStatus );
    CSPLOGSTRING2(CSPINT, "CSProvider::IncomingCallArrived hook %d", lineInfo.iHookStatus );
    
    // Find call by name.
    // In case CallAddedHandler has opened it and added to array, 
    // it should not be re-opened.    
    if ( !iCallArray->FindCall( aCallName ) )
        {
        TInt err(KErrUnknown);
        TUint32 serviceId( 0 );
        CCPCall::TCallType callType;
        CCCECallParameters::TCCELineType lineType;
        err = iLineContainer.ResolveCallInfo( aLineId, serviceId, callType, lineType );
        
        if ( !err )
            {
            CSPCall* call = NULL;
            TBuf8<KCCEBearerMaxSize> emptyBearer;
            TBuf<KCCESubAddressMaxSize> emptySubAddress;
            CCCECallParameters* callParameters = NULL; 
            TRAP_IGNORE( callParameters = CCCECallParameters::NewL() );
            if ( callParameters )
                {
                callParameters->SetServiceId(serviceId);
                callParameters->SetCallType(callType);
                callParameters->SetLineType(lineType);
                callParameters->SetOrigin(CCCECallParameters::ECCECallOriginPhone);
            
                if ( callType == CCPCall::ECallTypeCSVoice )
                    {
                    TRAP( err, call = CSPVoiceCall::NewL( aCallName, 
                                                    aLine, 
                                                    EFalse, 
                                                    *callParameters,
                                                    *this, 
                                                    EFalse ) );
                    }
                else if ( callType == CCPCall::ECallTypeVideo )
                    {
                    TRAP( err, call = CSPVideoCall::NewL( aCallName,
                                                    aLine, 
                                                    EFalse, 
                                                    *callParameters,
                                                    *this ) );
                    }
                else
                     {
                    CSPLOGSTRING2(CSPERROR, 
                       "CSProvider::IncomingCallArrived() unknown call type %d", err );
                    err = KErrUnknown;
                    }
                
                delete callParameters;
                }
            
            if ( call && err == KErrNone )
                {                
                TInt appendError = iCallArray->Add( call );
                CSPLOGSTRING2(CSPERROR, 
                      "CSProvider::IncomingCallArrived Appending call res %d", 
                        appendError);
                
                // Set audio handler for DevSound.
                call->SetAudioHandler( iAudioHandler );

                // Indicate incoming call for observer. 
                TInt err = IndicateIncomingCall( call );
                
                if ( KErrNone == err && 
                     call->State() == MCCPCallObserver::ECCPStateAnswering )
                    {
                    // If call has proceeded already to Answering state (autoanswer) 
                    // a change notification needs to be sent. 
                    call->NotifyCallStateChangedETel( RMobileCall::EStatusAnswering );
                    }
                
                if ( err ) 
                    {
                    iCallArray->Remove( call );
                    delete call;
                    }

                CSPLOGSTRING( CSPINT, "CSProvider::IncomingCallArrived Inform CCE OK" );
                }
            else if ( call && err != KErrNone )
                {
                // Delete call, call array removal not needed.   
                delete call; 
                }
            else
                {
                CSPLOGSTRING2(CSPERROR, 
                  "CSProvider::IncomingCallArrived Call could not be created %d", err);                
                }            
            }
        else
            {
            CSPLOGSTRING2(CSPERROR, "CSProvider::IncomingCallArrived ERROR Resolve call info err=%d", 
                                   err);
            }
        }
    CSPLOGSTRING(CSPINT, "CSProvider::IncomingCallArrived >");
    }

// ---------------------------------------------------------------------------
// CSProvider::GetCallWaitingL
// ---------------------------------------------------------------------------
//
void CSProvider::GetCallWaitingL( const CCCECallParameters& aParams,
                                  TBool& aCallWaitingStatus )
    {
    CSPLOGSTRING(CSPINT, "CSProvider::GetCallWaitingL <");
    
    // If there are already connected or held call: call waiting must be 
    // already ON.    
    TInt callCount = iCallArray->GetCallCount();
    TInt activeCallCount = 0;
    for (TInt i = 0; i < callCount; i++)
        {
        switch (iCallArray->Get(i)->State())
            {
            case MCCPCallObserver::ECCPStateConnected:
                {
                activeCallCount++;
                break;
                }
            case MCCPCallObserver::ECCPStateHold:
                {
                activeCallCount++;
                break;
                }
            case MCCPCallObserver::ECCPStateAnswering:
                {
                activeCallCount++;
                break;
                }
            }
        }
    if ( activeCallCount > 0 )
        {
        // CW status must be ON, so it is not reasonable to ask it from network 
        aCallWaitingStatus = ETrue;
        }
    else 
        {
        iCwRequester->GetCallWaitingL( aParams, aCallWaitingStatus );
        }
    
    CSPLOGSTRING(CSPINT, "CSProvider::GetCallWaitingL >");
    }
    
// ---------------------------------------------------------------------------
// CSProvider::GetDiagnosticError
// ---------------------------------------------------------------------------
//
TInt CSProvider::GetDiagnosticError( TName& aCallName )
    {
    CSPLOGSTRING2(CSPINT, 
            "CSProvider::GetDiagnosticError call name %S", &aCallName );
    return iMmCustom.GetDiagnosticInfo( aCallName );
    }

// ---------------------------------------------------------------------------
// CSProvider::NetworkSecurityStatus
// ---------------------------------------------------------------------------
//
TBool CSProvider::NetworkSecurityStatus() const
    {
    CSPLOGSTRING(CSPINT, "CSProvider::NetworkSecurityStatus");
    return iCipheringStatusMonitor->NetworkSecurityStatus();
    }

// ---------------------------------------------------------------------------
// CSProvider::SecureSpecified
// ---------------------------------------------------------------------------
//
TBool CSProvider::SecureSpecified() const
    {
    CSPLOGSTRING(CSPINT, "CSProvider::SecureSpecified");
    return iCipheringStatusMonitor->SecureSpecified();
    }

// ---------------------------------------------------------------------------
// Remote alerting tone playing status. Error situation is handled as no tone.
// ---------------------------------------------------------------------------
//
RMmCustomAPI::TRemoteAlertingToneStatus CSProvider::GetRemoteAlertingToneStatus()
    {
    RMmCustomAPI::TRemoteAlertingToneStatus status;
    TInt err = iMmCustom.GetRemoteAlertingToneStatus( status );
    if ( err )
        {
        status = RMmCustomAPI::EUiNoTone;
        }
    CSPLOGSTRING2(CSPINT, 
        "CSProvider::GetRemoteAlertingToneStatus status: %d", status );
    return status;
    }

// ---------------------------------------------------------------------------
// CSProvider::SecuritySettingChanged
// ---------------------------------------------------------------------------
//
void CSProvider::SecuritySettingChanged( TInt aValue )
    {
    CSPLOGSTRING2(CSPINT, 
        "CSProvider::SecuritySettingChanged value: %d", aValue);
    
    TInt callCount = iCallArray->GetCallCount();
    for (TInt i = 0; i < callCount; i++)
        {
        iCallArray->Get(i)->SecuritySettingChanged( aValue );
        }
    }

// ---------------------------------------------------------------------------
// CSProvider::CreateEtelConnectionsL
// ---------------------------------------------------------------------------
//
void CSProvider::CreateEtelConnectionsL()
    {
    CSPLOGSTRING(CSPINT, "CSProvider::CreateEtelConnectionsL <");
    
    TInt errorCode( KErrNone );
    TInt phoneCount( 0 );
    RTelServer::TPhoneInfo phoneInfo;    

    //This method connects the client to the ETel Server. 
    //It must be used before any of other functions during a telephony session.
    __ASSERT_ALWAYS( iServer.Connect( KNbrOfMessageSlots ) 
        == KErrNone, Panic( ECSPPanicNoEtel ) );

    //This method loads an ETel TSY module. mmtsy
    errorCode = iServer.LoadPhoneModule( KMmTsyModuleName );
    __ASSERT_ALWAYS( 
        errorCode == KErrNone || errorCode == KErrAlreadyExists, 
        Panic( ECSPPanicNoEtel ) );
    
    errorCode = iServer.SetExtendedErrorGranularity( 
        RTelServer::EErrorExtended );
    __ASSERT_ALWAYS( errorCode == KErrNone, Panic( ECSPPanicNoEtel ) );
    
    errorCode = iServer.SetPriorityClientV2();
    CSPLOGSTRING2(CSPINT, "CSProvider:: Priority client: %d", errorCode );
    if( KErrAlreadyExists == errorCode )
        {
        CSPLOGSTRING(CSPOBJECT, "CSProvider::CreateEtelConnectionsL() Already initialized");
        User::Leave( KErrAlreadyExists );
        }
    //This method retrieves the total number of phones supported by all 
    //the currently loaded ETel (TSY) modules.
    errorCode = iServer.EnumeratePhones( phoneCount );
    __ASSERT_ALWAYS( errorCode == KErrNone, Panic( ECSPPanicNoEtel ) );

    //This method retrieves information associated with the specified phone
    while ( phoneCount-- ) 
        { 
        errorCode = iServer.GetPhoneInfo( phoneCount, phoneInfo ); 
        __ASSERT_ALWAYS( errorCode == KErrNone, Panic( ECSPPanicNoEtel ) );

        if ( phoneInfo.iName == KMmTsyPhoneName )
            {
            phoneCount = 0;
            }
        } 
 
    //This method opens a phone subsession by name, 
    //and starts the modem initialisation process.
    errorCode = iMobilePhone.Open( iServer, phoneInfo.iName );
    __ASSERT_ALWAYS( errorCode == KErrNone, Panic( ECSPPanicNoEtel ) );
    
    iMobilePhone.SetEmergencyClient(RPhone::EEmergencyCSVoiceCallRequest);
    
    errorCode = iMobileConferenceCall.Open( iMobilePhone ); 
    __ASSERT_ALWAYS( errorCode == KErrNone, Panic( ECSPPanicNoEtel ) );
    
    errorCode = iMmCustom.Open( iMobilePhone );
    __ASSERT_ALWAYS( errorCode == KErrNone, Panic( ECSPPanicNoEtel ) );
    
    iSsMonitor = new (ELeave) 
            CSPSupplementaryServicesMonitor( *this, iMmCustom );
    iSsMonitor->StartMonitoring();
    
    CSPLOGSTRING(CSPINT, "CSProvider::CreateEtelConnectionsL >");
    }

// ---------------------------------------------------------------------------
// CSProvider::HandleDivertOrBarring
// Helper method.
// ---------------------------------------------------------------------------
//
void CSProvider::HandleDivertOrBarring(
        const TDesC& addr,
        RMmCustomAPI::TSsTypeAndMode& aSsTypeAndMode )
    {
    CSPLOGSTRING(CSPINT, "CSProvider::HandleDivertOrBarring <");
    switch ( aSsTypeAndMode.iSsType )
        {
        // MO unconditional diverts
        case RMmCustomAPI::ESsAllForwardings:
        case RMmCustomAPI::ESsForwUncond:
            {
            if( aSsTypeAndMode.iSsMode == RMmCustomAPI::ESsModeActive )
                {                
                iSsObserver->CallForwardEventOccurred( 
                            MCCPSsObserver::ECCPSsForwardUnconditionalModeActive,
                            addr );
                 }
             else
                {
                iSsObserver->CallForwardEventOccurred( 
                            MCCPSsObserver::ECCPSsForwardUnconditionalModeNotActive,
                            addr );                                
                }
            break;
            }
        
        // MO conditional diverts
        case RMmCustomAPI::ESsForwBusy:
        case RMmCustomAPI::ESsForwNoReply:
        case RMmCustomAPI::ESsForwNoReach:
        case RMmCustomAPI::ESsAllCondForwardings:
            {
            if( aSsTypeAndMode.iSsMode == RMmCustomAPI::ESsModeActive )
                {
                iSsObserver->CallForwardEventOccurred( 
                            MCCPSsObserver::ECCPSsForwardConditionallyModeActive,
                            addr );
                }
             else
                {
                iSsObserver->CallForwardEventOccurred( 
                            MCCPSsObserver::ECCPSsForwardConditionallyModeNotActive,
                            addr );
                }
             break;
             }
        default:
            {
            // No handling needed
            }    
        }
    CSPLOGSTRING(CSPINT, "CSProvider::HandleDivertOrBarring >");
    }

// ---------------------------------------------------------------------------
// CSProvider::InitializeCallParameters
// Initialises call parameters from SSSettings
// ---------------------------------------------------------------------------
//
void CSProvider::InitializeCallParameters( RMobileCall::TMobileCallParamsV7& aParams )
    {
    CSPLOGSTRING(CSPINT, "CSProvider::InitializeCallParameters <");
    TInt defaultCug(0);
    TInt cug(0);
    
    if( iSsSettingsHandler )
        {
        TRAP_IGNORE(iSsSettingsHandler->GetClirL( aParams.iIdRestrict ));
        TRAP_IGNORE(iSsSettingsHandler->GetDefaultCugL( defaultCug ));
        TRAP_IGNORE(iSsSettingsHandler->GetCugL( cug ));
        }
    
    // This is always set to false thus allowing calls outside groups.
    aParams.iCug.iSuppressOA = EFalse;    
  
    if ( cug >= 0 && cug != defaultCug ) // set group
        {
        // Invoke cug.
        aParams.iCug.iCugIndex = cug; 
        aParams.iCug.iExplicitInvoke = ETrue;
        aParams.iCug.iSuppressPrefCug = ETrue;
        }
    else if ( cug == -1 ) // -1 supress
        {
        aParams.iCug.iCugIndex = defaultCug; 
        aParams.iCug.iExplicitInvoke = ETrue;
        aParams.iCug.iSuppressPrefCug = ETrue;
        }
    else    // default cug
        {
        aParams.iCug.iCugIndex = defaultCug; 
        aParams.iCug.iExplicitInvoke = EFalse;
        aParams.iCug.iSuppressPrefCug = EFalse;
        }
    CSPLOGSTRING(CSPINT, "CSProvider::InitializeCallParameters >");
    }

// ---------------------------------------------------------------------------
// CSProvider::InitializeDataCallParameters
// Initialises call parameters from SSSettings
// ---------------------------------------------------------------------------
//
void CSProvider::InitializeDataCallParameters( RMobileCall::TMobileHscsdCallParamsV1& aParams )
    {
    CSPLOGSTRING(CSPINT, "CSProvider::InitializeDataCallParameters <");
    TInt defaultCug(0);
    TInt cug(0);
    
    if( iSsSettingsHandler )
        {
        TRAP_IGNORE(iSsSettingsHandler->GetClirL( aParams.iIdRestrict ));
        TRAP_IGNORE(iSsSettingsHandler->GetDefaultCugL( defaultCug ));
        TRAP_IGNORE(iSsSettingsHandler->GetCugL( cug ));
        }
        
    // This is always set to false thus allowing calls outside groups.
    aParams.iCug.iSuppressOA = EFalse;
  
    if ( cug >= 0 && cug != defaultCug ) // set group
        {
        // Invoke cug.
        aParams.iCug.iCugIndex = cug; 
        aParams.iCug.iExplicitInvoke = ETrue;
        aParams.iCug.iSuppressPrefCug = ETrue;
        }
    else if ( cug == -1 ) // -1 supress
        {
        aParams.iCug.iCugIndex = defaultCug; 
        aParams.iCug.iExplicitInvoke = ETrue;
        aParams.iCug.iSuppressPrefCug = ETrue;
        }
    else    // default cug
        {
        aParams.iCug.iCugIndex = defaultCug; 
        aParams.iCug.iExplicitInvoke = EFalse;
        aParams.iCug.iSuppressPrefCug = EFalse;
        }
    
    CSPLOGSTRING(CSPINT, "CSProvider::InitializeDataCallParameters >");
    }

// ---------------------------------------------------------------------------
// CSProvider::IndicateClientCall
// This fucntion delivers call pointer to be managed by CCE.
// ---------------------------------------------------------------------------
//
void CSProvider::IndicateClientCall( MCCPCSCall* aCall )
    {
    CSPLOGSTRING2(CSPINT, "CSProvider::IndicateClientCall call: %x", aCall); 
    iCCPObserver->MoCallCreated( *aCall );
    CSPLOGSTRING(CSPINT, "CSProvider::IndicateClientCall Inform CCE OK"); 
    }

// ---------------------------------------------------------------------------
// CSProvider::IndicateIncomingCall
// This fucntion delivers call pointer to be managed by CCE.
// ---------------------------------------------------------------------------
//    
TInt CSProvider::IndicateIncomingCall( MCCPCSCall* aCall )
    {
    CSPLOGSTRING2(CSPINT, 
            "CSProvider::IncomingCallArrived call: %x, inform CCE", aCall);
    iCCPObserver->IncomingCall( aCall );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CSProvider::NotifyDataPortName
// ---------------------------------------------------------------------------
//      
void CSProvider::NotifyDataPortName( TName& aDataPortName )
    {
    CSPLOGSTRING2(CSPINT, 
            "CSProvider::NotifyDataPortName name: %S", &aDataPortName);
    iCCPObserver->DataPortName( aDataPortName );
    }

// ---------------------------------------------------------------------------
// CSProvider::HandleNotifyPSL
// From MCSPPubSubObserver.
// ---------------------------------------------------------------------------
//  
void CSProvider::HandleNotifyPSL( const TUid /*aUid*/, const TInt& /*aKey*/, 
            const TRequestStatus& /*aStatus*/ )
    {
    CSPLOGSTRING(CSPINT, "CSProvider::HandleNotifyPSL");
    HandleSIMStatusL();
    }

// ---------------------------------------------------------------------------
// CSProvider::RemoteAlertingToneStatusChanged
// From MCSPRemoteAlertingToneObserver.
// Notify all calls about the change. Call is responsible for 
// notifying observer if the status change was for the call.
// ---------------------------------------------------------------------------
// 
void CSProvider::RemoteAlertingToneStatusChanged(
    RMmCustomAPI::TRemoteAlertingToneStatus aNewStatus )
    {
    CSPLOGSTRING(CSPINT, "CSProvider::RemoteAlertingToneStatusChanged");
    for ( TInt i = 0; i < iCallArray->GetCallCount(); i++ )
        {
        CSPCall* call = iCallArray->Get( i );
        if ( call )
            {
            call->RemoteAlertingToneStatusChanged( aNewStatus );
            }
        }
    }

// ---------------------------------------------------------------------------
// CSProvider::HandleSIMStatusL
// ---------------------------------------------------------------------------
//  
void CSProvider::HandleSIMStatusL()
    {
    CSPLOGSTRING(CSPINT, "CSProvider::HandleSIMStatusL");
    if ( !iSsSettingsHandler && iSimStatusListener )
        {
        TInt simState( ESimNotReady );
        TInt err = iSimStatusListener->Get( simState );
        if ( err == KErrNone && simState == ESimUsable )
            {
            iSsSettingsHandler = CSPSsSettingsHandler::NewL( *iSsObserver );
            }
        else
            {
            CSPLOGSTRING2( CSPERROR, "CSProvider::HandleSIMStatusL, err: %d", err );
            }    
        }
    
    if ( iSsSettingsHandler && iSimStatusListener )
        {
        delete iSimStatusListener;
        iSimStatusListener = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CSProvider::IndicateActiveHangup
// ---------------------------------------------------------------------------
//  
TInt CSProvider::IndicateActiveHangup( MCCPCallCommandHandling& aCall )
    {
    CSPLOGSTRING(CSPINT, "CSProvider::IndicateActiveHangup");
    return iCallCommandHandler->IndicateActiveHangup( aCall );
    }

// ---------------------------------------------------------------------------
// CSProvider::IndicateHangupComplete
// ---------------------------------------------------------------------------
//  
TInt CSProvider::IndicateHangupComplete( MCCPCallCommandHandling& aCall )
    {
    CSPLOGSTRING(CSPINT, "CSProvider::IndicateHangupComplete");
    return iCallCommandHandler->IndicateHangupComplete( aCall );
    }

// ---------------------------------------------------------------------------
// CSProvider::IndicateDialRequest
// ---------------------------------------------------------------------------
//  
TInt CSProvider::IndicateDialRequest( MCCPCallCommandHandling& aCall )
    {
    CSPLOGSTRING(CSPINT, "CSProvider::IndicateDialRequest");
    return iCallCommandHandler->IndicateDialRequest( aCall );
    }

// ---------------------------------------------------------------------------
// CSProvider::IndicateAnswerRequest
// ---------------------------------------------------------------------------
//  
TInt CSProvider::IndicateAnswerRequest( MCCPCallCommandHandling& aCall )
    {
    CSPLOGSTRING(CSPINT, "CSProvider::IndicateAnswerRequest");
    return iCallCommandHandler->IndicateAnswerRequest( aCall );
    }
    
// ---------------------------------------------------------------------------
// CSProvider::DontReportTerminationError
// ---------------------------------------------------------------------------
//  
TInt CSProvider::DontReportTerminationError()
    {
    CSPLOGSTRING(CSPINT, "CSProvider::DontReportTerminationError");
   
    TInt callCount = iCallArray->GetCallCount();
    for ( TInt i = 0; i < callCount; i++ )
        {
        if ( iCallArray->Get(i)->State() == MCCPCallObserver::ECCPStateConnecting )
            {
            iCallArray->Get(i)->DontReportTerminationError();
            return KErrNone;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CSProvider::NotifyStateChange
// ---------------------------------------------------------------------------
//
void CSProvider::NotifyStateChange( 
            MCSPConferenceStatusObserver::TCSPConferenceState aStatus )
    {
    CSPLOGSTRING(CSPINT, "CSProvider::NotifyStateChange");
    
    if ( !iConferenceCall && 
         aStatus == MCSPConferenceStatusObserver::ECSPConferenceActive )
        {
        TRAPD( err, iConferenceCall = CSPConferenceCall::NewL( 
                                        iMobilePhone, *iCallArray, iServiceId ) );    
        if ( KErrNone == err )
            {
            CSPLOGSTRING( CSPINT, "CSProvider::NotifyStateChange ext conference created" );
            iCCPObserver->ConferenceCallCreated( *iConferenceCall );
            }
        else 
            {
            CSPLOGSTRING2(CSPERROR, "CSProvider::NotifyStateChange \
                    ext conference creation error %d", err);
            }
        }
   }
    
// End of File

