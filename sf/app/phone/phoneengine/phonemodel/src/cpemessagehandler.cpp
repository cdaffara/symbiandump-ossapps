/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Message handler
*
*/

// INCLUDE FILES
#include "cpecenrepmonitor.h"
#include "cpeclientcallrequestmonitor.h"
#include "cpeclientcallrequestmonitor.h"
#include "cpeclientcommandhandlermonitor.h"
#include "cpeclientservices.h"
#include "cpeexternaldatahandler.h"
#include "cpemanualcallcontrolhandler.h"
#include "cpemessagehandler.h"
#include "cpeparseremergencynumberhandler.h"
#include "cpeparsermanufacturerhandler.h"
#include "cpeparsermischandler.h"
#include "cpeparserphonenumberhandler.h"
#include "cpeparsersimcontrolhandler.h"
#include "cpeparsersscallhandler.h"
#include "cpeparsersshandler.h"
#include "cpeparservoipnumberhandler.h"
#include "cpepcnparserprocedurehandler.h"
#include "cpesimstatemonitor.h"
#include "cpetimer.h"
#include "mpephonemodelinternal.h"
#include "mpeservicehandling.h"
#include "cperemotepartyinfomediator.h"

#include <apacmdln.h>
#include <apgcli.h>
#include <apgtask.h>
#include <AudioPreference.h>
#include <barsc.h> 
#include <barsread.h>
#include <bldvariant.hrh>
#include <ccpdefs.h>
#include <coreapplicationuisdomainpskeys.h>
#include <cpeclientinformation.h>
#include <cpegsmaudiodata.h>
#include <cphonegsmhandlercontainer.h>
#include <cphonegsmoptioncontainerbase.h>
#include <cphonegsmparserbase.h>
#include <cphonegsmparserresult.h>
#include <featmgr.h>
#include <mccecall.h>
#include <mpeaudiodata.h>
#include <mpecallhandling.h>
#include <mpecontacthandling.h>
#include <mpedatastore.h>
#include <mpeloghandling.h>
#include <pathinfo.h>
#include <pepanic.pan>
#include <phclttypes.h>
#include <phonegsmparser.h>
#include <ProfileEngineDomainConstants.h>
#include <talogger.h>
#include <w32std.h>

// EXTERNAL DATA STRUCTURES
// None.

// EXTERNAL FUNCTION PROTOTYPES  
// None.

// CONSTANTS
// Prefix change off. See SettingsInternalCRKeys.h
const TInt KPEPrefixChangeOff = 0;
// Prefix change on. See SettingsInternalCRKeys.h
const TInt KPEPrefixChangeOn = 1;
// International prefix
_LIT( KPEIntPrefix, "+" );
// Japan prefix
_LIT( KPEJapanPrefix, "+81" );
// Zero prefix
_LIT( KPEZeroPrefix, "0" );
// Valid dtmf sting
_LIT( KValidDtmfChars, "0123456789pw*+#" );

// Timeout for automatic answer. 
const TInt KPEAutomaticAnswerTimeOut = 5000000;

_LIT( KPEClirSuppress, "*31#" );
_LIT( KPEClirInvoke, "#31#" );

const TInt KPENumberPlus = '+';
const TInt KPENumberHash = '#'; 
const TInt KPENumberAsterisk = '*'; 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEMessageHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEMessageHandler* CPEMessageHandler::NewL
        ( 
        MPEPhoneModelInternal& aModel,
        MPECallHandling& aCallHandling,
        MEngineMonitor& aEngineMonitor,
        CPEGsmAudioData& aAudioData,
        MPELogHandling& aLogHandling,
        MPEContactHandling& aContactHandling,
        CPESimStateMonitor& aSimStateMonitor,
        CPEExternalDataHandler& aGsmExternalDataHandler,
        MPEServiceHandling& aServiceHandling,
        RFs& aFsSession )
    {
    CPEMessageHandler* self = new ( ELeave ) CPEMessageHandler( 
        aModel, 
        aCallHandling, 
        aEngineMonitor, 
        aAudioData, 
        aLogHandling, 
        aContactHandling, 
        aSimStateMonitor,
        aGsmExternalDataHandler,
        aServiceHandling,
        aFsSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::CPEMessageHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEMessageHandler::CPEMessageHandler( 
        MPEPhoneModelInternal& aModel,                 // The model of the phone object which owns this object
        MPECallHandling& aCallHandling,              // The reference parameter of the phone data object which is used to create call handling request
        MEngineMonitor& aEngineMonitor,        // The reference parameter of the engine monitor object which is used to communication with phone application
        CPEGsmAudioData& aAudioData,              // The reference parameter of the audio data object which is used to handle audio requests
        MPELogHandling& aLogHandling,                // The reference parameter of the logs data object which is used to handle log requests   
        MPEContactHandling& aContactHandling,          // The reference parameter of the contact data object which is used to handle contact requests
        CPESimStateMonitor& aSimStateMonitor,
        CPEExternalDataHandler& aExternalDataHandler, // The reference parameter of external data handler object
        MPEServiceHandling& aServiceHandling,
        RFs& aFsSession )                // The reference parameter of CustomAPI
        : iModel( aModel ), 
            iCallHandling( aCallHandling ),
            iEngineMonitor( aEngineMonitor ),
            iGsmAudioData( aAudioData ),
            iLogHandling( aLogHandling ),
            iContactHandling( aContactHandling ),
            iExternalDataHandler( aExternalDataHandler ),
            iSimStateMonitor( aSimStateMonitor ),
            iFsSession( aFsSession ),
            iServiceHandling( aServiceHandling ),
            iDataStore( *aModel.DataStore() )
    {
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::BaseConstructL
// Performs base construction of the object.
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::ConstructL()
    {
    iClientInformation = CPEClientInformation::NewL();

    iParser = PhoneGsmParser::CreateParserL();
    iOptions = PhoneGsmParser::CreateOptionContainerL();
    iManualCallControlHandler = CPEManualCallControlHandler::NewL( iCallHandling, *this, iDataStore );
    iClientServices = CPEClientServices::NewL( iModel, *this, iCallHandling, *iManualCallControlHandler );
    iParserHandlerContainer = CPhoneGsmHandlerContainer::NewL();

    // NOTE: The handler objects will break some CleanUpStack usage rules:
    // CPEMessageHandler class construct handler objects but ownership will be 
    // transfered to Handler Container. And therefore CleanUpStack will be 
    // used to member object because if iParserHandlerContainer->AddHandlerL 
    // leaves CPEMessageHandler don't delete handler object hence CleanUpStack
    // must delete handler object. 

    // Handles manufacturer specific codes from the parser.
    CPEParserManufacturerHandler* manufacturerHandler = 
        new ( ELeave ) CPEParserManufacturerHandler( iModel, iCallHandling, iFsSession );
    CleanupStack::PushL( manufacturerHandler );
    iParserHandlerContainer->AddHandlerL( *manufacturerHandler ); // ownership will be transfered to Handler Container.
    CleanupStack::Pop( manufacturerHandler );

    // Handles misc codes from the parser.
    CPEParserMiscHandler* miscHandler = new ( ELeave ) CPEParserMiscHandler( 
        *this, 
        iCallHandling );
    CleanupStack::PushL( miscHandler );
    iParserHandlerContainer->AddHandlerL( *miscHandler );// ownership will be transfered to Handler Container.
    CleanupStack::Pop( miscHandler );

    CPEParserPhoneNumberHandler* tempPhoneNumberHandler = 
        new ( ELeave ) CPEParserPhoneNumberHandler( *this, 
                                                       iCallHandling, 
                                                       iDataStore );
    CleanupStack::PushL( tempPhoneNumberHandler );
    iParserHandlerContainer->AddHandlerL( *tempPhoneNumberHandler ); // ownership will be transfered to Handler Container.
    CleanupStack::Pop( tempPhoneNumberHandler );
    iPhoneNumberHandler = tempPhoneNumberHandler;

    // Handles sim control procedures from the parser.
    CPEParserSimControlHandler* simControlHandler = CPEParserSimControlHandler::NewL( 
        iModel, 
        iContactHandling );
    CleanupStack::PushL( simControlHandler );
    iParserHandlerContainer->AddHandlerL( *simControlHandler );// ownership will be transfered to Handler Container.
    CleanupStack::Pop( simControlHandler );

    // Handles supplementary services during calls from the parser.
    CPEParserSSCallHandler* sSCallHandler = new ( ELeave ) CPEParserSSCallHandler( 
        *this, 
        iModel, 
        *iManualCallControlHandler );
    CleanupStack::PushL( sSCallHandler );
    iParserHandlerContainer->AddHandlerL( *sSCallHandler );// ownership will be transfered to Handler Container.
    CleanupStack::Pop( sSCallHandler );

    iSSHandler = CPEParserSSHandler::NewL( *this, iModel ); 
    iParserHandlerContainer->AddHandlerL( *iSSHandler );// ownership will be transfered to Handler Container.

    // Handles pcn service requests from the parser.
    CPEPcnParserProcedureHandler* pcnProcedureHandler = CPEPcnParserProcedureHandler::NewL( 
        *this, 
        iModel ); 
    CleanupStack::PushL( pcnProcedureHandler );
    iParserHandlerContainer->AddHandlerL( *pcnProcedureHandler );// ownership will be transfered to Handler Container.
    CleanupStack::Pop( pcnProcedureHandler );
    
    // Handles emergency phone number
    CPEParserEmergencyNumberHandler* tempEmergencyNumberHandler 
        = new ( ELeave ) CPEParserEmergencyNumberHandler( *this, 
                                                          iCallHandling, 
                                                          iDataStore );
    CleanupStack::PushL( tempEmergencyNumberHandler );
    iParserHandlerContainer->AddHandlerL( *tempEmergencyNumberHandler ); // ownership will be transfered to Handler Container.
    CleanupStack::Pop( tempEmergencyNumberHandler );
    iEmergencyNumberHandler = tempEmergencyNumberHandler;

    CPEParserVoipNumberHandler* tempVoipNumberHandler
        = CPEParserVoipNumberHandler::NewLC( *this, 
                                               iCallHandling, 
                                               iServiceHandling,
                                               iDataStore );

    iParserHandlerContainer->AddHandlerL( *tempVoipNumberHandler ); // ownership will be transfered to Handler Container.
    CleanupStack::Pop( tempVoipNumberHandler );
    iVoipNumberHandler = tempVoipNumberHandler;
    
    iAutomaticAnswerTimer = CPETimer::NewL( iModel );
 
    // Reserve needed callinfo and remoteinfo from heap.
    iCallInfo = new ( ELeave ) RMobileCall::TMobileCallInfoV3;
    
    iResult = PhoneGsmParser::CreateResultL();
    }
        
// Destructor
CPEMessageHandler::~CPEMessageHandler()
    {
    delete iClientInformation;
    delete iAsyncCallBack;
    delete iCallInfo;
    delete iAutomaticAnswerTimer;
    delete iParserHandlerContainer;
    delete iClientServices;
    delete iManualCallControlHandler;
    delete iOptions;
    delete iParser;
    delete iResult;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleAnswerCall
// Handles answer message from phone application. Method calls AnswerCall 
// method from the CallHandling subsystem.
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleAnswerCall(
        const TBool aAutomaticAnswer ) // Informs is this automatic answer or not
    {
    // Stop DTMF sending and empty buffers if answer was requested in the
    // middle of DTMF sequence.
    StopDtmfSending();
    
    //Check if this is automatic answer and play redialCompletedTone (confusing naming)
    if( !aAutomaticAnswer )
        {
        iAutomaticAnswerTimer->Cancel();
        }

    return iCallHandling.AnswerCall();
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleAudioRouting
// Handles accessory mode changed message from AudioHandling subsystem. Method 
// fecths accessory mode from AudioHandling subsystem and method sets new accessory
// mode value to the CPEEngineInfo class. Method finds previously used volume
// values for accessory and changes volume accordingly.
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleAudioRouting( TBool aVideoCall, TInt aCallId )
    {
    TInt error( ECCPErrorNone );
    
    if ( aCallId == KPEEmergencyCallId )     
         {
         error = iGsmAudioData.HandleEnergencyCallStarting();
         }
     else
         {
         error = iGsmAudioData.HandleCallStarting( aVideoCall );
         }
    
    return error;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleRoutePreferenceChanged
// Handles route preference changed message from AudioHandling subsystem. 
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleRoutePreferenceChanged()
    {
    TInt error( ECCPErrorNone );
    
    TEFLOGSTRING( KTAMESINT, "PE CPEMessageHandler::HandleRoutePreferenceChanged()" );
    
    error = iGsmAudioData.CallAudioRoutePreferenceChanged();
     
    return error;
    }    
// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleContinueDtmfSending
// Handles continue dtmf sending after 'w' character in the dtmf string
// Method calls ContinueDtmfSending or StopDtmfSending method from the 
// CallHandling subsystem.
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::HandleContinueDtmfSending( 
        const TBool aContinue )
    {
    if ( aContinue )
        {
        HandleSendDtmf();
        }
    else
        {
        StopDtmfSending();
        }
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleReleaseAll
// Handles release all message from phone application
// Method calls ReleaseAll method from the CallHandling subsystem object..
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleReleaseAll()
    {
    return iCallHandling.ReleaseAll();
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleReleaseCall
// Handles release message from phone application
// Method fecths call id number from the CPEEngineInfo structure and then
// Method calls HangUp method from the CallHandling subsystem.
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleReleaseCall( TPEHangUpOptions aAutoResumeOption )
    {
    TInt callId;
    TInt errorCode( ECCPErrorNone );
    callId = iDataStore.CallId();
    
    if ( CallIdCheck::IsVideo( callId ) )
        {
        //Video call
        iGsmAudioData.StopInbandTonePlay();
        errorCode = iCallHandling.HangUp( callId, ETPEHangUpNotResumeHeldCall );
        }
    else
        {
        errorCode = iCallHandling.HangUp( callId, aAutoResumeOption );
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleSendDtmf
// Handles send dtmf message from phone application
// Method fetches dtmf string from the CPEEngineInfo class and then
// Method parses the DTMF string and then take action properly: 
// hard pause: triggers sending MEngineMonitor::EPEMessageStoppedDTMF 
//      to phone model
// '+': triggers substitution speed dial digit with speed dial number. 
//
// Method calls SendDtmf method from the CallHandling subsystem.
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleSendDtmf()
    {
    TInt errorCode( ECCPErrorNone );
    TPEDtmfString dtmfString = iDataStore.DtmfStringCommand();

    RemoveInvalidChars( dtmfString, KPEClientValidChars, EFalse );

    if ( dtmfString.Length() == 0 )
        {
        iDataStore.SetDtmfString( KNullDesC() ); 
        errorCode = KErrArgument;
        }
    // Parse the first character of the string
    else if ( dtmfString[ 0 ] == KPEDtmfStopCharLowercase || 
              dtmfString[ 0 ] == KPEDtmfStopCharUppercase ) // hard pause
        {
        TEFLOGSTRING( KTAMESINT, "PE CPEMessageHandler::HandleSendDtmf(), Processing W" );
        // remove the hard pause char from the string
        iDataStore.SetDtmfString( dtmfString.Mid( 1 ) );
        iDataStore.SetDtmfStringCommand( dtmfString.Mid( 1 ) );
        
        // Send EPEMessageStoppedDTMF message asynchronic.

        TCallBack callBack( CallbackSendMessageStoppedDTMF, this );
        delete iAsyncCallBack;
        iAsyncCallBack = NULL;
        // Function does not allow to leave.
        iAsyncCallBack = new CAsyncCallBack( callBack, CActive::EPriorityHigh );
        if ( iAsyncCallBack )
            {
            iAsyncCallBack->CallBack();
            }
        else
            {
            iModel.SendMessage( MEngineMonitor::EPEMessageStoppedDTMF );
            }
        
        dtmfString = KNullDesC;
        }    
    else if ( dtmfString[ 0 ] == KPEDtmfPauseCharLowercase || 
              dtmfString[ 0 ] == KPEDtmfPauseCharUppercase ) // soft pause
        {
        TEFLOGSTRING( KTAMESINT, "PE CPEMessageHandler::HandleSendDtmf(), Processing P" );
        
        // store the full string for UI to display 
        iDataStore.SetDtmfString( dtmfString );

        if ( dtmfString.Length() > 1 )
            {
            // store the remainder of the string to be processed later
            iDataStore.SetDtmfStringCommand( dtmfString.Mid( 1 ) ); 
            }
        else
            {
            // nothing left to process
            iDataStore.SetDtmfStringCommand( KNullDesC() );
            }
        // set the dtmf string to send 
        dtmfString = dtmfString.Left( 1 );
        }
    else 
        {
        // store the full string for UI to display 
        iDataStore.SetDtmfString( dtmfString );
        
        // Find the next stop point
        TInt stopPoint = ECCPErrorNotFound;
        TPtrC validDtmfStringStopChars( KPEValidDTMFStringStopChars );
        for ( TInt index = 0 ; index < dtmfString.Length(); index++ )
            {
            if ( validDtmfStringStopChars.Locate( dtmfString[index] ) != ECCPErrorNotFound )
                {
                stopPoint = index;
                break; 
                }
            }

        if ( stopPoint != ECCPErrorNotFound ) 
            {
            // store the remainder of the string to be processed later
            iDataStore.SetDtmfStringCommand( dtmfString.Mid( stopPoint ) ); 
            // set the dtmf string to send 
            dtmfString = dtmfString.Left( stopPoint );            
            }
        else
            {
            // nothing left to process
            iDataStore.SetDtmfStringCommand( KNullDesC() );
            }
        }

    if ( dtmfString.Length() )  
        {
        TEFLOGSTRING2( KTAMESINT, "PE CPEMessageHandler::HandleSendDtmf(), Starting to play %S", &dtmfString );
        // send the dtmf string to call handling subsystem
        errorCode = iCallHandling.SendDtmf( dtmfString );
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::CallBackHandleSendDtmf
// Asyncronous callback for HandleSendDtmf() function.
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::CallBackHandleSendDtmf( TAny* aAny )
    {
    __ASSERT_ALWAYS( aAny , Panic( EPEPanicNullPointer ) );
    CPEMessageHandler* self = static_cast<CPEMessageHandler*>(aAny);

    delete self->iAsyncCallBack;
    self->iAsyncCallBack = NULL;
    return self->HandleSendDtmf();  
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::CheckPrefix
// Checks the status of phone number prefix change setting
// and calls CPEMessageHandler::ChangePrefix for execution.
// If error occurs the phone number is left untouched
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::CheckPrefix()
    {
    TInt err = ECCPErrorNone;
    TBool checkPrefix = 
        FeatureManager::FeatureSupported( KFeatureIdJapanPrefixChange );

    if ( checkPrefix )
        {
        TInt prefixMode = KPEPrefixChangeOff; // Default is "off"

        err = iExternalDataHandler.Get(
            EPEDialPrefixChangeSetting,
            prefixMode );

        if ( !err )
            {
            if ( prefixMode == KPEPrefixChangeOn )
                {
                TPEPrefixText prefixText;
                
                err = iExternalDataHandler.GetText(
                    EPEDialPrefixTextSetting,
                    prefixText );

                if ( !err )
                    {
                    err = ChangePrefix( prefixText );
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::ChangePrefix
// Executes phone number prefix change.
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::ChangePrefix(
    const TPEPrefixText& aPrefixText )
    {
    TInt err = ECCPErrorNone;

    const TInt beginning = 0; // The beginning for replacement.
    TInt replaceLength = 0; // The string length to be replaced.
    TInt location = ECCPErrorNotFound; // The location of searched string.
    TPEPhoneNumber phoneNumber = iDataStore.PhoneNumber();

    // Try to find Japan prefix.
    location = phoneNumber.Find( KPEJapanPrefix );
    if ( location == beginning )
        {
        // The string was found, so make replacement. 
        // Safe because KPEZeroPrefix < KPEJapanPrefix.
        replaceLength = KPEJapanPrefix().Length();
        phoneNumber.Replace( beginning, replaceLength, KPEZeroPrefix );
        }
    else
        {
        // Try to find international prefix.
        location = phoneNumber.Find( KPEIntPrefix );
        if ( location == beginning )
            {
            replaceLength = KPEIntPrefix().Length();
            // The string was found, so try to make replacement. 
            const TInt phoneNumLength = phoneNumber.Length();
            const TInt prefixLength = aPrefixText.Length();

            TInt stringLength = 
                ( phoneNumLength + prefixLength - replaceLength );

            if ( phoneNumber.MaxLength() >= stringLength )
                {
                // There is enough space to make this replace.
                phoneNumber.Replace( beginning, replaceLength, aPrefixText );
                }
            else
                {
                // There is no space to combine the strings, so inform it.
                err = KErrOverflow;
                }
            }
        }

    if ( err == ECCPErrorNone )
        {
        iDataStore.SetPhoneNumber( phoneNumber );
        }

    return err;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleDtmfSent
// Handles DTMF sent message from the CallHandling subsystem.
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleDtmfSent()
    {
    TInt errorCode( ECCPErrorNone );

    // check if there are still parts of the string left to process
    if ( iDataStore.DtmfStringCommand().Length() ) 
        {
        HandleSendDtmf();
        errorCode = KPEDontSendMessage;
        }
    else 
        {
        // dtmf sending complete
        iDataStore.SetDtmfString( KNullDesC() );
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::StopPlayingDtmf
// Stops/Cancels playing dtmf tones and string (audio feedback and network)
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::StopDtmfSending()
    {
    // cancel subtitution callback:
    if ( iAsyncCallBack )
        {
        iAsyncCallBack->Cancel();
        delete iAsyncCallBack;
        iAsyncCallBack = NULL;
        }

    iCallHandling.StopDtmfSending();
    
    iDataStore.SetDtmfString( KNullDesC() ); 
    iDataStore.SetDtmfStringCommand( KNullDesC() );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleSetAudioMute
// Handles audio mute message from the phone application.
// Method fecths mute value from the CPEEngineInfo and then
// Method sets mute value to the AudioHandling subsystem.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::HandleSetAudioMute()
    {
    TEFLOGSTRING( KTAGENERAL, "PE: CPEMessageHandler::HandleSetAudioMute" );
    TBool mute = iDataStore.AudioMuteCommand();
    iGsmAudioData.SetAudioMuteSync( mute );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleSetAudioOutput
// Handles audio output from the phone application.
// Method fecths output and note values from the CPEEngineInfo and then
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleSetAudioOutput()
    {
    TEFLOGSTRING( KTAGENERAL, "PE: CPEMessageHandler::HandleSetAudioOutput" );
    return iGsmAudioData.SetAudioOutput( iDataStore.AudioOutputCommand(),
                                      iDataStore.ShowNoteCommand() );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleSetAudioVolume
// Handles change volume message from the phone application.
// Method fecths volume value from the CPEEngineInfo and then
// Method sets volume value to the AudioHandling subsystem.
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::HandleSetAudioVolume()
    {    
    TInt volume = iDataStore.AudioVolumeCommand();
    TEFLOGSTRING2( KTAGENERAL, "PE: CPEMessageHandler::HandleSetAudioVolume %d", volume );
    iGsmAudioData.SetAudioVolumeSync( volume );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandlePlayDTMFL
// Handles key down event.
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::HandlePlayDTMFL()
    {
    ProcessPlayDTMFL();
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleEndDTMF
// Handles key up event.
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleEndDTMF()
    {
    return ProcessEndDTMF();
    }
    
// -----------------------------------------------------------------------------
// CPEMessageHandler::SetClientInformation
// Sets client information.
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::SetClientInformation(
        const TInt aCallId,  
        const TDesC& aMainPartOfPhoneNumber ) 
    {
    // Other ClientInformation it was already set before, in SetClientData
    // method. Methods is called from HandleClientCallDataL before CallId is known.
    iClientInformation->SetNumber( aMainPartOfPhoneNumber );
    
    iDataStore.SetCallClientInformation( *iClientInformation, aCallId );
    
    const TPECallOrigin& origin = iDataStore.CallOriginCommand();    
    iDataStore.SetCallOrigin( origin, aCallId );    
    }
 
// -----------------------------------------------------------------------------
// CPEMessageHandler::SetClientData
// Sets client dial data to member variable.
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::SetClientData(
        const CPhCltDialData& aClientDialData ) 
    {
    CCCECallParameters& params = iDataStore.CallParameters();
    
    iClientDialData = &const_cast<CPhCltDialData&>( aClientDialData );

    iClientInformation->SetName( iClientDialData->Name() );
    
    if ( iClientDialData->CallType() == EPhCltVideo || 
        iClientDialData->CallType() == EPhCltForcedVideo )
        {
        iClientInformation->SetCallType( EPECallTypeVideo );
        iDataStore.SetCallTypeCommand( EPECallTypeVideo );
        params.SetCallType( CCPCall::ECallTypeVideo );
        }
    else if ( iClientDialData->CallType() == EPhCltCallVoIP )
        {
        iClientInformation->SetCallType( EPECallTypeVoIP );
        iDataStore.SetCallTypeCommand( EPECallTypeVoIP );        
        params.SetCallType( CCPCall::ECallTypePS );
        }
    else
        {
        iClientInformation->SetCallType( EPECallTypeCSVoice );
        iDataStore.SetCallTypeCommand( EPECallTypeCSVoice );
        params.SetCallType( CCPCall::ECallTypeCSVoice );
        }
      
    iClientInformation->SetEndOtherCalls( iClientDialData->EndOtherCalls() );
    
    iClientInformation->SetAllowMatch( iClientDialData->AllowMatch() );
    
    iClientInformation->SetContactLink( iClientDialData->ContactLink() );
   
    iClientInformation->SetShowNumber( iClientDialData->ShowNumber() );

    // Not accept 0, TODO Requirement clarification needed, 
    // does service id 0 need special handling!
    if ( iClientDialData->ServiceId() != 0 )
        {
        iDataStore.SetServiceIdCommand( iClientDialData->ServiceId() );
        params.SetServiceId( iClientDialData->ServiceId() );
        }
        
    iDataStore.SetUserToUserInformation( iClientDialData->UUI() );
    
    params.SetBearer( iClientDialData->Bearer() );
    params.SetSubAddress( iClientDialData->SubAddress() );
    
    if ( iClientDialData->SATCall() )
        {
        params.SetOrigin( CCCECallParameters::ECCECallOriginSAT );
        }
    else
        {
        params.SetOrigin( CCCECallParameters::ECCECallOriginPhone );
        }
    }
     
// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleTerminateAllConnections
// Method resets redial timer if active and calls TerminateAllConnection 
// method from the CallHandling subsytem object.
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleTerminateAllConnections()
    {
    TInt errorCode( ECCPErrorNone );
    errorCode = iCallHandling.TerminateAllConnections();
    TEFLOGSTRING2( 
        KTAGENERAL, 
        "PE: CPEMessageHandler::HandleTerminateAllConnections: Callhandling::TerminateAllConnections() called, Error code: %d", 
        errorCode );

    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::IsItCharThatCanDelete
// Checks if character can be delete. If given charter can be located from 
// KPECharsThatCanBeDelete, method returns ETrue else EFalse.
// -----------------------------------------------------------------------------
//
TBool CPEMessageHandler::IsItCharThatCanDelete( 
        const TChar& aChar ) const
    {
    TPtrC Invalid( KPECharsThatCanBeDelete );

    return Invalid.Locate( aChar ) != ECCPErrorNotFound;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::IsValidChar
// Checks if character is acceptable. If given charter can be located from 
// KPEClientValidChars, method returns ETrue else EFalse.
// -----------------------------------------------------------------------------
//
TBool CPEMessageHandler::IsValidChar( 
        const TChar& aChar, //character to be checked
        const TDesC& aValidChars ) const
    {
    return aValidChars.Locate( aChar ) != ECCPErrorNotFound;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::RemoveInvalidChars
// Check all characters validity from descriptor one by one. 
//    (1) If chater is valid then OK and next charter... 
//    (2) If invalid check if that can be delete from string.
//    (3) If it can be delete then DELETE and next charter... 
//    (4) But if charter can NOT be delete then return value is EFalse.
// -----------------------------------------------------------------------------
//
TBool CPEMessageHandler::RemoveInvalidChars( 
        TDes& aString, //string to be processed
        const TDesC& aValidChars,
        const TBool aCheckForDelete ) const
    {
    TBool returnValue = ETrue;
    TInt index = aString.Length() - 1;

    TEFLOGSTRING2( KTAGENERAL, 
        "PE: CPEMessageHandler::RemoveInvalidChars, String to parse = %S", &aString );

    for ( ; index >= 0; index-- )
        {
        if ( !IsValidChar( aString[ index ], aValidChars ) )//(1)
            {
            if ( !aCheckForDelete 
                || ( aCheckForDelete && IsItCharThatCanDelete( aString[index] ) ) )//(2)
                {
                aString.Delete( index, 1 ); // one character  //(3)
                }
            else
                {
                returnValue = EFalse;//(4) //if String includes alphabets or other forbidden characters return TFalse
                }
            }
        }
    if ( aCheckForDelete )
        {
        TLex input( aString );

        TLexMark start;
        input.Mark( start );

        while ( input.Peek().IsDigit() ) 
            {
            input.Inc();
            }

        TPtrC mainpart( input.MarkedToken( start ) );
        if ( mainpart.Length() )
            {
            aString = mainpart;
            }
    
        if ( input.Remainder().Length() > 0 )
            {
            TPEDtmfString dtmfString = input.Remainder();
            iDataStore.SetDtmfStringCommand( dtmfString );
            }
        else
            {
            TPEDtmfString dtmfString( KNullDesC );
            iDataStore.SetDtmfStringCommand( dtmfString );
            } 
        
        }
    TEFLOGSTRING2( 
        KTAGENERAL, 
        "PE: CPEMessageHandler::RemoveInvalidChars, Parsed string = %S", 
        &aString);
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::CallbackSendMessageStoppedDTMF
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::CallbackSendMessageStoppedDTMF( TAny* aAny )
    {
    __ASSERT_ALWAYS( aAny , Panic( EPEPanicNullPointer ) );
    CPEMessageHandler* self = static_cast<CPEMessageHandler*>(aAny);

    delete self->iAsyncCallBack;
    self->iAsyncCallBack = NULL;
    self->iModel.SendMessage( MEngineMonitor::EPEMessageStoppedDTMF ); 

    return ECCPErrorNone;
    }
    
// -----------------------------------------------------------------------------
// CPEMessageHandler::RemovePlusPrefix
// Remove '+' from begin of the string
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::RemovePlusPrefix(
    TPEPhoneNumber& aPhoneNumber )
    {
    TLex input( aPhoneNumber);

    if ( input.Peek() == KPEDtmfPlusChar )
        {
        // remove plus char 
        input.Inc();
        aPhoneNumber = input.Remainder();
        TEFLOGSTRING2( KTAMESINT, 
            "PE CPEMessageHandler::RemovePlusPrefix(), aPhoneNumber: %S"
            , &aPhoneNumber ); 
        }
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleBuildConferenceCall
// Handles Create conference message from phone application.
// Method calls BuildConference method from MPECallHandling object.
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleBuildConferenceCall()
    {
    return iCallHandling.BuildConference();
    }


// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleGoOneToOne
// Handles GoOneToOne message from PhoneApplication. Calls GoOneToOne
// method from CallHandling object.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleGoOneToOne()
    {
    TInt callId = iDataStore.CallId();
    __ASSERT_DEBUG( CallIdCheck::IsVoice( callId ), 
        Panic( EPEPanicCallIndexOutOfRange ) );
    return iCallHandling.GoOneToOne( callId );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleAddConferenceMember
// Handles Add conference member message from phone application.
// Method creates AddMember request to the CallHandling object.
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleAddConferenceMember()
    {
    TEFLOGSTRING( KTAINT, 
       "PE CPEMessageHandler::HandleAddConferenceMember ");
    return iCallHandling.AddConferenceMember();
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleAddConferenceMember
// Handles dropped conference member message from callhandling subsystem.
// In case a long dtmf string is being sent, cancels the sending process.
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::HandleDroppedConferenceMember()
    {
    TEFLOGSTRING( KTAINT, 
       "PE CPEMessageHandler::HandleDroppedConferenceMember");
    
    StopDtmfSending();
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleCallHandlingError
// Handles error message from CallHandling subsystem
// Method fetches error code from CallHandling subsystem and
// Method sends error code to SAT
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::HandleCallHandlingError(
        const TInt aCallId,
        const TBool /*aBusyCall*/ )
    {
    // Call data not valid for call id -1, i.e. non-call error situations
    if ( aCallId != KPECallIdNotUsed )
        {
        //Update local call info
        const TInt errorCode = iCallHandling.GetCallInfo( *iCallInfo, aCallId );
        
        // Take the error from call handling.
        TInt errorForClient = iDataStore.ErrorCode(); 
        if (  errorCode == ECCPErrorNone )
            {
            const TInt diagnosticInfo = 
                iCallHandling.CallTerminatedError( aCallId );
            if ( diagnosticInfo != ECCPErrorNone )
                {
                errorForClient = diagnosticInfo;
                }
            }
        
        TEFLOGSTRING2( KTAINT,
                "PE CPEMessageHandler::HandleCallHandlingError, SendRespond errorForClient: %d", errorForClient );
        iClientServices->CallRequestMonitor()->SendRespond( errorForClient );            
        }
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleEngineInfo
// Handles call logging. Method add and update the call info.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleEngineInfo(
    const TInt aCallId )       
    {
    TEFLOGSTRING2( KTAINT,
        "PE CPEMessageHandler::HandleEngineInfo, call id: %d", aCallId );
    TInt errorCode(  ECCPErrorNone );
    TPEState callState;
    
    errorCode = FindCallInfo( aCallId );

    if ( !errorCode )
        {
        callState = iCallHandling.GetCallState( aCallId );
        TEFLOGSTRING2( KTAINT, 
            "PE CPEMessageHandler::HandleEngineInfo, call state: %d", 
            callState );

        SetPhoneNumberForCallLogging( aCallId );

        if ( callState == EPEStateDialing || callState == EPEStateRinging )
            {
            errorCode = UpdateClientInfo( aCallId );

            iDataStore.SetRemoteIdentity( 
                static_cast< RMobileCall::TMobileCallRemoteIdentityStatus > ( 
                    iCallInfo->iRemoteParty.iRemoteIdStatus ), 
                aCallId );
            iTime.UniversalTime();
            iDataStore.SetCallStartTime( iTime, aCallId );
            iDataStore.SetCallDuration( 0, aCallId );
            }

        // Calls have to log also without a contact (ECCPErrorNotFound).
        if ( errorCode == ECCPErrorNone || errorCode == ECCPErrorNotFound)
            {
            errorCode = ECCPErrorNone;
            
            // If call is emergency call, phonenumber must be fetched from engine info
            // Log type must be recognized from call id also, because in idle state
            // TSY has cleaned call info information
            CheckAndHideIdentity( aCallId );

            // Save the rest of information to EngineInfo.
            SetLoggingInfo( aCallId, callState );
            // Set missed call to EngineInfo.
            IsMissedCall( aCallId, callState );
            errorCode = iLogHandling.SaveCallEntry( aCallId );
            }
        } //if ( !errorCode )
        
    return errorCode;
    }   
 
// -----------------------------------------------------------------------------
// CPEMessageHandler::UpdateClientInfo
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::UpdateClientInfo( 
    const TInt aCallId )
    {
    TInt errorCode( ECCPErrorNone );
    const MPEClientInformation& clientInformation = iDataStore.CallClientInformation( aCallId );
    
    TEFLOGSTRING2( 
        KTAINT, 
        "PE CPEMessageHandler::UpdateClientInfo, remote name: '%S'", 
        &iDataStore.RemoteName( aCallId ) );

    TEFLOGSTRING2( 
        KTAINT, 
        "PE CPEMessageHandler::UpdateClientInfo, name: '%S'", 
        &clientInformation.Name() );

    TEFLOGSTRING2( 
        KTAINT, 
        "PE CPEMessageHandler::UpdateClientInfo, allowmatch: %d", 
        clientInformation.AllowMatch() );
    
    if ( EPECallOriginSAT != iDataStore.CallOrigin(aCallId) && 
       ( aCallId != KPEEmergencyCallId ) )
        {
        TEFLOGSTRING2( 
            KTAINT, 
            "PE CPEMessageHandler::UpdateClientInfo, match phone number: '%S'", 
            &iDataStore.RemotePhoneNumber( aCallId ) );
        if ( clientInformation.ContactLink().Length() > 0 )
            {
            errorCode = iContactHandling.FindContactInfoSync( 
            aCallId, 
            EPEFindWithContactId );
            }    
        else
            {
            errorCode = iContactHandling.FindContactInfoSync( 
            aCallId, 
            EPEFindWithPhoneNumber );            
            }
            
        TEFLOGSTRING2( 
            KTAINT, 
            "PE CPEMessageHandler::UpdateClientInfo > MPEContactHandling::FindContactInfoSync( EPEFindWithPhoneNumber ), error code: %d", 
            errorCode );
        }
    
    // Calls have to log also without a contact (ECCPErrorNotFound).
    if ( errorCode == ECCPErrorNone || errorCode == ECCPErrorNotFound)
        {
        // Set name to EngineInfo
        SetName( aCallId );
        }
     return errorCode;   
     }   
    
// -----------------------------------------------------------------------------
// CPEMessageHandler::CheckAndHideIdentity
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::CheckAndHideIdentity(
    const TInt aCallId )   
    {
    // If call is emergency call, phonenumber must be fetched from engine info
    // Log type must be recognized from call id also, because in idle state
    // TSY has cleaned call info information
    RMobileCall::TMobileCallRemoteIdentityStatus tempIdentity;
    tempIdentity = iDataStore.RemoteIdentity( aCallId );
    
    TEFLOGSTRING2( KTAINT,
        "PE CPEMessageHandler::CheckAndHideIdentity, Identity: %d", tempIdentity );
    if ( iCallInfo->iEmergency || aCallId == KPEEmergencyCallId ) 
        {
        iDataStore.SetRemotePhoneNumberType( EPEEmergencyNumber, aCallId );
        }
    else if( iDataStore.CallDirection( aCallId ) != RMobileCall::EMobileOriginated )
        {
        switch( tempIdentity )
            {
            case RMobileCall::ERemoteIdentitySuppressed:
                {
                // It is neccessary to perform an additional check for available
                // remote party phone number to comply with Italian operator
                // requirement: "If CLIR is active but network still provides
                // the device with a phone number, it should not be blocked
                // but passed to a client when requested."
                if( iDataStore.RemotePhoneNumber( aCallId ).Length() == 0 )
                    {
                    HideIdentification( EPEPrivateNumber, aCallId );
                    }
                else
                    {
                    // Align remote identity with remote phone number availability.
                    iDataStore.SetRemoteIdentity( RMobileCall::ERemoteIdentityAvailable, aCallId );
                    }
                break;
                }
            case RMobileCall::ERemoteIdentityAvailableNoCliRejectedByUser:
                {
                HideIdentification( EPEPrivateNumber, aCallId );
                break;
                }
            case RMobileCall::ERemoteIdentityUnknown:
            case RMobileCall::ERemoteIdentityAvailableNoCliInteractionWithOtherService:
            case RMobileCall::ERemoteIdentityUnavailableNoCliInteractionWithOtherService:
            case RMobileCall::ERemoteIdentityAvailableNoCliCoinOrPayphone:
            case RMobileCall::ERemoteIdentityUnavailableNoCliCoinOrPayphone:
            case RMobileCall::ERemoteIdentityAvailableNoCliUnavailable:
                {
                HideIdentification( EPEUnknownNumber, aCallId );
                break;
                }
            case RMobileCall::ERemoteIdentityAvailable:
            default:
                {
                TEFLOGSTRING( KTAINT,
                    "PE CPEMessageHandler::CheckAndHideIdentity, CLI available" );
                break;
                }
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CPEMessageHandler::FindCallInfo
// Return callInfo from Etel/TSY.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::FindCallInfo(
        const TInt aCallId )
    {
    TInt errorCode( ECCPErrorNone );
    if ( CallIdCheck::IsVoice( aCallId ) || CallIdCheck::IsVideo( aCallId ) )
        {
        // Single call
        errorCode = iCallHandling.GetCallInfo( *iCallInfo, aCallId );
        TEFLOGSTRING3( 
            KTAREQOUT, 
            "CNT CPEMessageHandler::FindCallInfo > MPECallHandling::GetCallInfo, call id: %d, error code: %d", 
            aCallId, 
            errorCode );
        TEFLOGSTRING2( 
            KTAINT, 
            "CNT CPEMessageHandler::FindCallInfo, remote number: '%S'", 
            &iCallInfo->iRemoteParty.iRemoteNumber );
        TEFLOGSTRING2( 
            KTAINT, 
            "CNT CPEMessageHandler::FindCallInfo, direction: %d", 
            iDataStore.CallDirection( aCallId ) );
        TEFLOGSTRING2( 
            KTAINT, 
            "CNT CPEMessageHandler::FindCallInfo, id status: %d", 
            iCallInfo->iRemoteParty.iRemoteIdStatus );
        }
    else
        {
        // We don't log the master conference call and other unknown calls.
        errorCode = KErrUnknown;
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::SetPhoneNumberForCallLogging
// Set phonenumber for call logging
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::SetPhoneNumberForCallLogging(
        const TInt aCallId )
    {
    // if call is MO then the phonenumber must be stored from dialled party.
    if ( iDataStore.CallDirection( aCallId ) == RMobileCall::EMobileOriginated )
        {

        TPEPhoneNumber number;        
        const TPECallOrigin origin = iDataStore.CallOrigin( aCallId );
        
        if ( origin == EPECallOriginSAT )
            {
            // Dont set for SAT calls >> number not logged
            }
        else if( iDataStore.PhoneNumber().Length() )
            {
            // Phonenumber given by user, includes pfe- and postfix.
            number = iDataStore.PhoneNumber();
            }   
        else
            {
            // If call is made using ETel api, then phonenumber must take from callinfo.
            number = iCallInfo->iDialledParty.iTelNumber;
            }
        
        
        // WholeOutgoingPhoneNumber should set only one time because in connected state 
        // PhoneNumber is cleaned from datastore.
        if( !iDataStore.WholeOutgoingPhoneNumber( aCallId ).Length() )
            {
        
            TEFLOGSTRING2( KTAINT,
                    "PE CPEMessageHandler::SetPhoneNumberAndDataCallLogging, number: '%S'"
                    ,&number );
            iDataStore.SetWholeOutgoingPhoneNumber( number, aCallId ); 
            }
        
        // RemotePhoneNumber should set only one time because user can edit DataStore::PhoneNumber after dialing was started.
        if( !iDataStore.RemotePhoneNumber( aCallId ).Length() )
            {
            TEFLOGSTRING3( 
                KTAMESINT, 
                "PE CPEMessageHandler::SetPhoneNumberForCallLogging, remote phone number: '%S', call id: %d", 
                &number, aCallId );
                                         
            RemovePreAndPostFix( number );
  
            iDataStore.SetRemotePhoneNumber( number, aCallId );
            }            
        }
    else if ( iDataStore.CallDirection( aCallId ) == RMobileCall::EMobileTerminated )
        {
        iDataStore.SetRemotePhoneNumber( iCallInfo->iRemoteParty.iRemoteNumber.iTelNumber, aCallId );
        TEFLOGSTRING2( 
            KTAMESINT, 
            "PE CPEMessageHandler::SetPhoneNumberForCallLogging: remote phone number: '%S'", 
            &iCallInfo->iRemoteParty.iRemoteNumber.iTelNumber );
        }
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::SetLoggingInfo
// Store call information for logging
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::SetLoggingInfo(
        const TInt aCallId,
        TPEState aCallState )
    {
    if ( aCallState == EPEStateConnected )
        {
        iDataStore.SetCallStartTime( iCallInfo->iStartTime, aCallId );   
        }
    iDataStore.SetCallForwarded( iCallInfo->iForwarded, aCallId );
    iDataStore.SetCallService( iCallInfo->iService, aCallId );
    iDataStore.SetCallState( aCallState, aCallId );
    iDataStore.SetCallDuration( iCallInfo->iDuration.Int() );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::SetName
// Set calling name or client name to RemoteInfo.
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::SetName( const TInt aCallId )
    {
    if ( iDataStore.RemoteName( aCallId ).Length() <= 0 &&
         iDataStore.RemoteCompanyName( aCallId ).Length() <= 0 &&
         iCallInfo->iRemoteParty.iCallingName.Length() >0 ) 
        { 
        iDataStore.SetRemoteName( iCallInfo->iRemoteParty.iCallingName, aCallId );
        }
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HideIdentification
// Hide identification to RemoteInfo.
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::HideIdentification(
        TPEPhoneNumberIdType aPhoneNumberId,
        const TInt aCallId )
    {
    TEFLOGSTRING2( KTAINT,
        "PE CPEMessageHandler::HideIdentification: aPhoneNumberId = %d", aPhoneNumberId );
    iDataStore.SetRemotePhoneNumberType( aPhoneNumberId, aCallId );
    iDataStore.SetRemotePhoneNumber( KNullDesC(), aCallId );
    iDataStore.SetRemoteName( KNullDesC(), aCallId );
    iDataStore.SetRemotePartyName( KNullDesC(), aCallId );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::IsMissedCall
// Check missed call in the idle state.
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::IsMissedCall(
        const TInt aCallId,
        TPEState aCallState )
    {
    if ( iDataStore.CallDirection( aCallId ) == RMobileCall::EMobileTerminated )
        {
        // If remote party has hanged up the current call on Ringing state, 
        // this call is the missed call in the idle state.
        if ( aCallState == EPEStateIdle )
            {
            TBool missedCall = EFalse;

            if ( CallIdCheck::IsVoice( aCallId ) || CallIdCheck::IsVideo( aCallId ) )
                {
                iCallHandling.GetMissedCall( missedCall, aCallId );
                }

            iDataStore.SetMissedCall( missedCall, aCallId );
            } 
        }
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleCancelSSstringCommand
// Handles cancel SS string command.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleCancelSSstringCommand()
    {
    TInt returnValue( ECCPErrorNone );

    returnValue = iSSHandler->Cancel();

    return returnValue;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleConferenceIdleState
// Handles EPEMessageConferenceIdle message.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleConferenceIdleState( 
        const TInt aCallId )
    {
    TInt counter;
    TInt conferenceValue;

    TTimeIntervalSeconds duration;
    TInt errorCode = iCallHandling.GetCallDuration( duration, aCallId );
    
    // Duration offset is stored previously to data store using conference members
    // duration. If we do not check for null here then the stored offset is ignored..
    if ( ECCPErrorNone == errorCode && duration.Int() > 0 ) 
        {
        // Set last conference duration
        iDataStore.SetCallDuration( duration.Int() );
        }
    else if ( ECCPErrorNone == errorCode && duration.Int() == 0 )
        {
        // copy conference call duration to last call duration
        // all call specific data is erased a few lines down
        iDataStore.SetCallDuration( iDataStore.CallDuration( aCallId ).Int() );
        }   

    //Reset values to the TPECallInfo structure
    iDataStore.ResetCallInfo( aCallId );

    // Reset Conference Master info from all ex-members
    for ( counter = 0; counter < KPEMaximumNumberOfVoiceCalls; counter++ )
        {
        conferenceValue = iDataStore.CallConference( counter );
        if ( conferenceValue == aCallId )
            {
            iDataStore.SetCallConference( KPENormalVoiceCall, counter );
            }
        }
    
    //Stops possible ringing tone playing
    //APS Stops possible remote alerting tone playing
    TEFLOGSTRING( KTAMESINT, 
        "PE  CPEMessageHandler::HandleConferenceIdleState > iGsmAudioData.StopInbandTonePlay()");
    iGsmAudioData.StopInbandTonePlay(); 
    
    ClearCallAudio();    
        
    return ECCPErrorNone;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleConnectedState
// Handles connected message from the CallHandling subsystem 
// 
//  Method fetches call state and ALS line values from the Callhandling subsystem and then
//  Method fetches remote information from the CPEEngineInfo class and then
//  Method sets new values to the logInfo parameters and then
//  Method updates call values to the LogHandling subsystem and then
//  Method sets new values to the CPEEngineInfo class and then
//  Method fecths possible dtmf string from the CPEParserPhoneNumberHandler class and then
//  Method calls HandleSendDtmf method if dtmf string lenght was more than zero.
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleConnectedState( 
        const TInt aCallId )
    {
    TInt errorCode( ECCPErrorNone );
    TPEState callState;
    TPEDtmfString dtmfString;

    RMobileCall::TMobileCallDirection callDirection;
    callDirection = iDataStore.CallDirection( aCallId );

    //Stops possible local playing of remote alerting tone
    TEFLOGSTRING( 
        KTAMESINT, 
        "PE  CPEMessageHandler::HandleConnectedState: Calling iGsmAudioData.StopInbandTonePlay()");
    iGsmAudioData.StopInbandTonePlay();

    //If call was held then no logging is done.
    callState = iDataStore.CallState( aCallId );
    if ( callState != EPEStateHeld )
        {
        errorCode = HandleEngineInfo( aCallId );

        // If phone number has contained dtmf string, phone engine sends dtmf string now.
        // Only in MO - call.

        if ( callDirection == RMobileCall::EMobileOriginated )
            {
            if ( aCallId == KPEEmergencyCallId )
                {
                dtmfString = iDataStore.DtmfStringCommand();
                if ( dtmfString.Length() > 0 )
                    {
                    errorCode = HandleSendDtmf();
                    }
                }
             else 
                {

                dtmfString = iDataStore.DtmfPostFix( aCallId );
                
                if ( dtmfString.Length() > 0 )
                    {
                    iDataStore.SetDtmfStringCommand( dtmfString );
                    errorCode = HandleSendDtmf();
                    }
                }

            iDataStore.SetErrorCode( errorCode );
            }
        }
    // For Sat call ( normal or emergency )    
    iClientServices->CallRequestMonitor()->SendRespond( ECCPErrorNone );
    
    // Reset Phonenumber from engine info, this is necessary so that call number
    // logging works OK (see CPEMessageHandler::SetPhoneNumberForCallLogging).  
    iDataStore.SetPhoneNumber( KNullDesC() );
    
    return ECCPErrorNone;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleDialCall
// Handles dial message from phone application
// 
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleDialCall(
    const TBool aClientCall )
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::HandleDialCall" );
    TInt errorCode( ECCPErrorNone );

    TRAPD( trapError, errorCode = HandleDialCallL( aClientCall ));

    if ( trapError )    
        {
        if ( aClientCall )    
            {
            iClientServices->CallRequestMonitor()->SendRespond( trapError );                    
            }
        TEFLOGSTRING2( KTAINT, "PE CPEMessageHandler::HandleDialCall: trapError = %d", trapError );
        return trapError;
        }
    return errorCode;
    }


// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleDialEmergencyCall
// Handles dial message from phone application
// 
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleDialEmergencyCall(
    const TBool aClientCall )
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::HandleDialEmergencyCall" );
    TInt errorCode( ECCPErrorNone );

    if( iEmergencyCallActive && aClientCall )
        {
        // Client tried to make an emergency call even though we already
        // have an active emergency call.
        iClientServices->CallRequestMonitor()->SendRespond( ECCPErrorAlreadyInUse );
        // Notify PhoneApp which is responsible for showing the error note
        return ECCPErrorAlreadyInUse;
        }
    
    iDataStore.SetCallTypeCommand( EPECallTypeCSVoice );
        
    // Check the phone number and change the prefix if needed
    CheckPrefix();
    
    if ( aClientCall )
        {
        TPEPhoneNumber phoneNumber = iDataStore.PhoneNumber();
        __ASSERT_DEBUG( !( phoneNumber == KNullDesC ), Panic( EPEPanicInvalidParameter));

        TBuf<KPEPhoneNumberMaxLength> tempPhoneNumber = phoneNumber;
        RemoveInvalidChars( tempPhoneNumber, KPEClientValidChars, ETrue );
        RemovePreAndPostFix( tempPhoneNumber );
        
        SetClientInformation( KPEEmergencyCallId, tempPhoneNumber );
        iClientServices->CallRequestMonitor()->SendRespond( ECCPErrorNone );
        }

    if ( IsActiveVideo() )
        {
        // PhoneApp needs to request releasing of data port from video telephony engine
        // Emergency call initialization will continue after receiving MPEPhoneModel::
        // EPEMessageContinueEmergencyCallInitialization
        iModel.SendMessage( MEngineMonitor::EPEMessageInitiatedEmergencyWhileActiveVideo );
        }
    else
        {
        ContinueDialEmergency();
        }

    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleDialCallL
// Handles dial message from phone application
// 
//  Method fecths phone number from the CPEEngineInfo class and then
//  Method sets process type to the CPEParserPhoneNumberHandler and then
//  Method fecths clir settings from the Settings utility subsystem and tehn
//  Method fecths call parameters from the CallHandling subsystem and then
//  Method sets updated call parameter to the CallHandling subsystem and then
//  Method parsers phonenumber and then
//  Method process parsing result with PhoneParser
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleDialCallL(
    const TBool aClientCall )
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::HandleDialCallL" );
    TBool parsingResult;
    TPEPhoneNumber phoneNumber;
    TInt errorCode( ECCPErrorNone );
    TInt numberOfCalls;
    
    if ( aClientCall )
        {
        HandleClientCallData();
        }
    else 
        {
        iDataStore.SetCallOriginCommand(EPECallOriginPhone);
		ResetClientCallData();
        }
        
    //Get number of calls
    numberOfCalls = iCallHandling.GetNumberOfCalls();

    // Check the phone number for prefix change and change the prefix if needed
    CheckPrefix();

    phoneNumber = iDataStore.PhoneNumber();
  
    __ASSERT_ALWAYS( !( phoneNumber == KNullDesC ), User::Leave( ECCPErrorInvalidPhoneNumber ));
    
    // Number parser operations
    iOptions->SetOptionStatus( KPhoneOptionSend, ETrue );
    iOptions->SetOptionStatus( KPhoneOptionInCall, numberOfCalls > 0 );
    // If voip call request
    if( iDataStore.CallTypeCommand() == EPECallTypeVoIP )
        {
        iOptions->SetOptionStatus( KPhoneOptionVoipCall, ETrue );
        }
    else
        {
        iOptions->SetOptionStatus( KPhoneOptionVoipCall, EFalse );
        }

    parsingResult = iParser->ParseL( phoneNumber, *iResult, *iOptions );
    if ( parsingResult )
        {
        iGsmParserErrorCode = ECCPErrorNone;
        iParserHandlerContainer->ProcessL( *iResult );
        errorCode = iGsmParserErrorCode;
        }
    else
        {
        // string was not recognised by any of the parser objects
        TEFLOGSTRING( KTAERROR, 
            "PE CPEMessageHandler::HANDLEDIALCALLL: PHONEPARSER DID NOT RECOGNIZE THE STRING!" );
        errorCode = KErrArgument;
        }

    if ( aClientCall )
        {
        // Do not sent the response to SAT, unless there is error.
        const TPECallOrigin& origin = iDataStore.CallOriginCommand();            
        if ( origin != EPECallOriginSAT || errorCode )    
            {
            iClientServices->CallRequestMonitor()->SendRespond( errorCode );                    
            }
        }

    TEFLOGSTRING2( KTAINT, "PE CPEMessageHandler::HandleDialCallL: errorCode = %d", errorCode );
        
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::ResetClientCallData
// Reset CCCECallParameters to prevent of use a previous call´s parameters
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::ResetClientCallData()
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::ResetClientCallData()" );

    CCCECallParameters& params = iDataStore.CallParameters();
    params.SetLineType( CCCECallParameters::ECCELineTypePrimary );
    params.SetUUSId( KNullDesC() );
    params.SetBearer( KNullDesC8() );
    params.SetSubAddress( KNullDesC() );
    params.SetOrigin( CCCECallParameters::ECCECallOriginPhone );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleSatCallRequestCompleted
// Sends respond to SAT after dial request completion
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::HandleSatCallRequestCompleted()
    {
    // Monitor with active request completes
    iClientServices->CallRequestMonitor()->
        SendRespond( iCallHandling.CallTerminatedError( iDataStore.CallId() ));                    
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleDialEmergency
// Handles emergency call message from phone application
//
//  Method calls DialEmergencyCall method from the CallHandling subsystem.
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::ContinueDialEmergency()
    {
    iDataStore.SetCallId( KPEEmergencyCallId );
    if ( !iEmergencyCallActive )
        {
        iExternalDataHandler.Set( EPEEmergencyCallInfo, ETrue );  
        iEmergencyCallActive = ETrue;        
        }
        // unmute mic
        iDataStore.SetAudioMuteCommand( EFalse );
        HandleSetAudioMute();

    iCallHandling.DialEmergencyCall( iDataStore.PhoneNumber() );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleDialingStateL
// Handles dialing state transition for voice and video calls
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::HandleDialingStateL( 
        const TInt aCallId )
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::HandleDialingStateL <");
    
    // Save call direction to engine info.
    iDataStore.SetCallDirection( RMobileCall::EMobileOriginated, aCallId );
    
    // Log the call information.
    User::LeaveIfError( HandleEngineInfo( aCallId ) );
    
    // publish remote party info to Mediator after contact matching is done
    // as remote party info contains information from contact matching. 
    UpdateRemotePartyInfo();
    
    TBool videoCall =
        ( iDataStore.CallType( aCallId ) == EPECallTypeVideo ) ? ETrue : EFalse;

    TEFLOGSTRING2(
            KTAINT,
            "CPEMessageHandler::HandleDialingStateL, callType: %d",
            iDataStore.CallType( aCallId ) );
        
    // Determine the preferred output for call audio and route accordingly.
    // Routing for incoming voice call is done in answering state.
    HandleAudioRouting( videoCall, aCallId );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleIncomingCallL
// Handles incoming voice and video call
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::HandleIncomingCallL( 
        const TInt aCallId )
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::HandleIncomingCallL <");
    
    // Save call direction to engine info.
    iDataStore.SetCallDirection( RMobileCall::EMobileTerminated, aCallId );

    // Log the call information.
    User::LeaveIfError( HandleEngineInfo( aCallId ) );
    
    // publish remote party info to Mediator after contact matching is done
    // as remote party info contains information from contact matching. 
    UpdateRemotePartyInfo();    
    
    TInt numberOfCalls = iCallHandling.GetNumberOfCalls();
    
    if( numberOfCalls > 1 )
        {
        TEFLOGSTRING( 
            KTAMESINT, 
            "PE  CPEMessageHandler::HandleIncomingCallL: Calling iGsmAudioData->PlayInbandTone() for call waiting tone");
        iDataStore.SetInbandTone( ECCPCallWaiting );
        iGsmAudioData.PlayInbandTone();
        iWaitingCallId = aCallId;
        }

    if( AutomaticAnswer( aCallId ) )
        {
        TEFLOGSTRING( KTAINT, 
        "PE  CPEMessageHandler::HandleIncomingCallL: iAutomaticAnswerTimer->StartTimer");
        iAutomaticAnswerTimer->StartTimer( KPEAutomaticAnswerTimeOut, MEngineMonitor::EPEMessageAnswer, aCallId );
        }

    // Cancel EnableService, if in progress
    iServiceHandling.CancelServiceEnabling();
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleDisconnecting
// Handles disconnecting. This means network doesn't play inband tones, but PE must play those.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleDisconnecting
        (
        const TInt /*aCallId*/  //The identification number of the call.
        )
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::HandleDisconnecting" );

    iAutomaticAnswerTimer->Cancel();

    TEFLOGSTRING( KTAMESOUT, 
        "PE CPEMessageHandler::HandleDisconnecting > iGsmAudioData.PlayInbandTone()");
    iGsmAudioData.PlayInbandTone();
    
    return ECCPErrorNone;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleDisconnectingWithInband
// Handles disconnecting with inband. This means network plays the inband tones
// and PhoneEngine does not.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleDisconnectingWithInband(
        const TInt /*aCallId*/ )
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::HandleDisconnectingWithInband" );
    return ECCPErrorNone;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleDropConferenceMember
// Handles drop message from phone application
// Method calls DropMember method from CallHandling object.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleDropConferenceMember()
    {
    TInt callId = iDataStore.CallId();
    __ASSERT_DEBUG( CallIdCheck::IsVoice( callId ), 
        Panic( EPEPanicCallIndexOutOfRange ) );
    return iCallHandling.DropMember( callId );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleHoldCall
// Handles hold message from phone application
// Method fecths call id number from the CPEEngineInfo class and then
// Method calls HoldCall method from the CallHandling subsystem.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleHoldCall()
    {
    return iCallHandling.HoldCall();
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleVoiceCallIdleState
// Handles idle message from the CallHandling subsystem.
//
//  Method stops playing possible ringing tone and the
//  Method gets call and ALS line information from the CallHandling subsystem and then
//  Method fecth remote party information from the CPEEngineInfo class and then
//  Method sets new values to the logInfo parameter and then
//  Method updates log values to the LogHandling subsystem and then
//  Method updates Engine Info's call terminated diagnostics value and then
//  Method resets call information.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleVoiceCallIdleState( 
    const TInt aCallId )
    {
    TInt errorCode( ECCPErrorGeneral );
    TInt returnValue( ECCPErrorNone );
    TInt numberOfCalls;

    //Stops possible ringing tone playing
    //APS Stops possible remote alerting tone playing
    TEFLOGSTRING( KTAMESINT, 
        "PE  CPEMessageHandler::HandleVoiceCallIdleState > iGsmAudioData.StopInbandTonePlay()");
    iGsmAudioData.StopInbandTonePlay(); 
    
    ClearCallAudio();
    
    errorCode = iCallHandling.GetCallInfo( *iCallInfo, aCallId );
    
    if (  errorCode == ECCPErrorNone )
        {
        TInt diagnosticInfo = iCallHandling.GetCallTerminatedDiagnostics( 
            iCallInfo->iCallName );
        // if diagnosticInfo available set diagnostic error code to DataStore    
        if ( diagnosticInfo != ECCPErrorNone )
            {
            iDataStore.SetErrorCode( diagnosticInfo );
            }
        }

    errorCode = HandleEngineInfo( aCallId );  
    iDataStore.SetErrorCode( ECCPErrorNone );

    numberOfCalls = iCallHandling.GetNumberOfCalls();

    if ( numberOfCalls == 0 )
        {
        // unmute mic
        iDataStore.SetAudioMuteCommand( EFalse );
        HandleSetAudioMute();
        
        if ( iEmergencyCallActive )
            {
            iExternalDataHandler.Set( EPEEmergencyCallInfo, EFalse );
            iEmergencyCallActive = EFalse;
            }
        }

    // If there is a waiting call (voice or data) on 
    // the line, and no other calls, play ringing tones for it.
    else if ( numberOfCalls == 1 )
        {
        TPEState callState;
        callState = iCallHandling.GetCallState( iWaitingCallId );
        // EPEStateRinging equals MT call
        if( callState == EPEStateRinging )
            {
            // unmute mic
            iDataStore.SetAudioMuteCommand( EFalse );
            HandleSetAudioMute();
            TEFLOGSTRING2( KTAMESINT, 
                "CPEMessageHandler::HandleVoiceCallIdleState: aCallId = %d", 
                aCallId );
            }
        else 
            {
            TEFLOGSTRING2( KTAMESINT, 
                "CPEMessageHandler::HandleVoiceCallIdleState: callState = %d", 
                callState );
            }
        }
    else
        {
        TEFLOGSTRING2( KTAMESINT, 
            "CPEMessageHandler::HandleVoiceCallIdleState: numberOfCalls = %d", 
            numberOfCalls );
        }
    
    iDataStore.ResetCallInfo( aCallId );    
    
    //publish remote party info to Mediator after call info has been cleared.
    UpdateRemotePartyInfo();    

    return returnValue;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::ProcessPlayDTMFL
// Handles key down message from phone application
//
//  Method fecths key code value from the CallHandling subsystem and then
//  Method calls StartDtmfTone method from the CallHandling subsystem.

// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::ProcessPlayDTMFL()
    {
    const TChar keyCode = iDataStore.KeyCode();
    
    TBuf<1> keyCodeBuf;
    keyCodeBuf.Append( keyCode );
    
    if ( KPEValidDTMFChars().Find( keyCodeBuf ) >= 0 )
        {
        if ( iCallHandling.GetNumberOfCalls() > 0 )
            {   //There is ongoing call(s)
            iCallHandling.StartDtmfTone( keyCode );
            }
        else
            {
            iGsmAudioData.PlayDtmfTone( keyCode );            
            }
        }
        
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::ProcessEndDTMF
// Handles end playing DTMF message from phone application
// Method calls StopDtmfTone method from the CallHandling subsystem.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::ProcessEndDTMF()
    {
    TInt returnValue( ECCPErrorNone ); 
    // Number parser operations
    if ( iCallHandling.GetNumberOfCalls() > 0 )
        {   //There is ongoing call(s)
        returnValue =  iCallHandling.StopDtmfTone();
        }
    iGsmAudioData.StopDtmfTonePlay();
    return returnValue;
    }


// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleRejectCall
// Handles reject message from phone application
//  Method stops playing possible ringing tone and then
//  Method calls RejectCall method from the CallHandling subsystem.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleRejectCall
        (
        // None.
        )
    {
    //Stops playing ringing tone
    //Data call
    iGsmAudioData.StopInbandTonePlay(); 
    iAutomaticAnswerTimer->Cancel();
    return iCallHandling.RejectCall( );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleResumeCall
// Handles resume message from phone application
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleResumeCall()
    {
    return iCallHandling.ResumeCall();
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleSendUssd
// Handles request from CPEParserMiscHandler
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleSendUssd(
        const TDesC& aString )   // USSD string to be sent.

    {
    return iClientServices->SendUssd( aString );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleSimStateChanged
// Handles EPEMessageSIMStateChanged message from DosServer.
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::HandleSimStateChanged()  
    {
    iDataStore.SetSimState( iSimStateMonitor.SimState() );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleStartUpL
// Handles startup message from the phone application.
// Method calls StartUp method from the CallHandling subsystem and then
// Method calls StartUp method from the AudioHandling subsystem and then
// Method starts monitoring client originated calls.
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::HandleStartUp()
    {
    iCallHandling.StartUp( );
    iGsmAudioData.StartUp( );
    
    iClientServices->StartMonitoring( );
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleVideoCallConnected
// Handles data call logging in connected and idle state.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleVideoCallConnected(
        const TInt aCallId )
    {
    //Stops possible remote alerting tone playing
    TEFLOGSTRING( 
        KTAINT, 
        "PE  CPEMessageHandler::HandleVideoCallConnected > CPEGsmAudioData::StopInbandTonePlay()");
    iGsmAudioData.StopInbandTonePlay(); 
    // EFalse updates log information.
    TInt errorCode = HandleEngineInfo( aCallId );
    iDataStore.SetErrorCode( errorCode );
    return ECCPErrorNone; 
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleVideoCallIdle
// Handles data call logging and a possible waiting voice call in Idle state.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleVideoCallIdle(
        const TInt aCallId )
    {
    TInt numberOfCalls;
    //Stops possible remote alerting tone playing
    TEFLOGSTRING( KTAINT,
        "PE CPEMessageHandler::HandleVideoCallIdle > CPEGsmAudioData::StopInbandTonePlay()");
    iGsmAudioData.StopInbandTonePlay(); 
    HandleEngineInfo( aCallId );
    iDataStore.SetErrorCode( ECCPErrorNone );
    TInt returnValue( ECCPErrorNone );

    numberOfCalls = iCallHandling.GetNumberOfCalls();
    if ( numberOfCalls == 0 )
        {
        iGsmAudioData.HandleCallEnding();

        // unmute mic
        iDataStore.SetAudioMuteCommand( EFalse );
        HandleSetAudioMute();
        }
    else if ( numberOfCalls == 1 )
        {
        TPEState callState;
        callState = iCallHandling.GetCallState( iWaitingCallId );
        // EPEStateRinging equals MT call
        if ( callState == EPEStateRinging )
            {
            TEFLOGSTRING2( KTAMESINT, 
                "CPEMessageHandler::HandleVideoCallIdle: aCallId = %d", 
                aCallId );
            iGsmAudioData.HandleCallEnding();
            
            // unmute mic
            iDataStore.SetAudioMuteCommand( EFalse );
            HandleSetAudioMute();
            }
        else 
            {
            TEFLOGSTRING2( KTAMESINT, 
                "CPEMessageHandler::HandleVideoCallIdle: callState = %d", 
                callState );
            }
        }
    else
        {
        TEFLOGSTRING2( KTAMESINT, 
            "CPEMessageHandler::HandleVideoCallIdle: numberOfCalls = %d", 
            numberOfCalls );
        }
    iDataStore.ResetCallInfo( aCallId );
    
    //publish remote party info to Mediator after call info has been cleared.
    UpdateRemotePartyInfo();    
    
    return returnValue;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleEmergencyCheck
// Check is given number emergency number.
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleEmergencyCheck( )
    {
    TInt errorCode( ECCPErrorNone );
    TPEPhoneNumber emergencyNumber;
    TBool isEmergency;
    emergencyNumber = iDataStore.PhoneNumber();

    errorCode = iClientServices->IsEmergencyPhoneNumber( emergencyNumber, isEmergency );
    if ( isEmergency  && errorCode == ECCPErrorNone )
        {
        iModel.SendMessage( MEngineMonitor::EPEMessageValidEmergencyNumber );
        }
    else
        {
        iModel.SendMessage( MEngineMonitor::EPEMessageInValidEmergencyNumber );
        }

    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandlePhoneNumberEditedL
// Handles Phone Number Edited message from phone application, 
// called when user is used paste or delete command.
// Method check if there is ongoing call(s) if there is return ECCPErrorNone else
// method fecths phone number from the CPEEngineInfo class and 
// then method parsers phonenumber and then method process parsing result
// with PhoneParser and Phoneengine.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
void CPEMessageHandler::HandlePhoneNumberEditedL()
    {
    TBool isServiceCode( EFalse );

    iOptions->SetOptionStatus( KPhoneOptionInCall, 
        ( iCallHandling.GetNumberOfCalls() > 0 ) );

    iOptions->SetOptionStatus( KPhoneOptionSend, EFalse );
    iOptions->SetOptionStatus( KPhoneOptionVoipCall, EFalse );

    if( iParser->ParseL( iDataStore.PhoneNumber(), *iResult, *iOptions ) )
        {
        isServiceCode = ETrue;
        iGsmParserErrorCode = ECCPErrorNone;
        iParserHandlerContainer->ProcessL( *iResult );
        }
    else // check for service codes that require SEND
        {
        iOptions->SetOptionStatus( KPhoneOptionSend, ETrue );
        if ( iParser->ParseL( iDataStore.PhoneNumber(), *iResult, *iOptions ) )
            {
            // if there is an incoming call only certain codes are allowed       
            if ( iCallHandling.IsCallInState( EPEStateRinging ) &&
                 !iCallHandling.IsCallInState( EPEStateConnected ) )
                {
                isServiceCode = PhoneGsmParser::IsAllowedForArriving( *iResult );
                }
            else
                {
                const PhoneGsmParser::TContentType type = 
                    PhoneGsmParser::DetermineContentType( *iResult );
                    
                if ( type == PhoneGsmParser::EContentSupplementaryService ||
                     type == PhoneGsmParser::EContentUnstructuredService )
                    {
                    isServiceCode = ETrue;
                    }
                }
            }
        }
        
    iDataStore.SetPhoneNumberIsServiceCode( isServiceCode );    
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::AutomaticAnswer
// Checks if automatic answer is defined for connected accessory.
// -----------------------------------------------------------------------------
// 
TBool CPEMessageHandler::AutomaticAnswer( const TInt aCallId ) const
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::AutomaticAnswer <" );
    
    TInt automaticAnswer( EFalse );
    TInt accessory( 0 );
         
    if ( iCallHandling.GetNumberOfCalls() > 1 ) // The new call need to be the first in the array
        {
        return EFalse;
        }
        
    iExternalDataHandler.Get( EPEAccessoryMode, accessory );
    
    switch ( accessory )
        {
        case EAccModeWiredHeadset:    //Wired headset
        case EAccModeWirelessHeadset: //Wireless headset
            {
            // Safe to ignore error code here, 
            // automatic answer setting just equals zero == EFalse if it fails
            iExternalDataHandler.Get( EPEAutomaticAnswerHeadsetSetting, automaticAnswer );
            break;
            }
        case EAccModeWiredCarKit:     //Wired carkit
            {
            iExternalDataHandler.Get( EPEAutomaticAnswerCarkitSetting, automaticAnswer );
            break;
            }           
        case EAccModeWirelessCarKit:  //Wireless carkit
            {
            iExternalDataHandler.Get( EPEAutomaticAnswerWirelessCarkitSetting, automaticAnswer );                
            break;
            }           
        case EAccModeLoopset:         //Loopset
            {
            iExternalDataHandler.Get( EPEAutomaticAnswerLoopsetSetting, automaticAnswer );                
            break;
            }           
        case EAccModeMusicStand:      //Musicstand
            {
            iExternalDataHandler.Get( EPEAutomaticAnswerMusicStandSetting, automaticAnswer );                
            break;
            }
         default:
            break;
         }

    if ( automaticAnswer )
        {
        TFileName noneFilepath;
        noneFilepath.Append( TParsePtrC( PathInfo::RomRootPath() ).Drive() );
        noneFilepath.Append( KProfileNoSoundPath() );
        
        TProfileRingingType ringingType = iDataStore.RingingType(); 
        TPEContactFileName ringingTone = iDataStore.RingingTone( aCallId );
         
        if ( ringingType == EProfileRingingTypeBeepOnce ||
             ringingType == EProfileRingingTypeSilent ||
             ringingTone == noneFilepath )
            {
            automaticAnswer = EFalse;
            }
        } 
    TEFLOGSTRING2( KTAINT, 
        "PE CPEMessageHandler::AutomaticAnswer > ret: %d", automaticAnswer );
    return automaticAnswer;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleStopInbandTonePlay
// Stop playing a InBand tone
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::HandleStopInbandTonePlay()
    {
    iGsmAudioData.StopInbandTonePlay();
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleAutomaticAnswerOff
// Gets EPEMessageAutomaticAnswerOff from UI and makes soft cancelation of the
// automatic answer when needed
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::HandleAutomaticAnswerOff() const
    {
    iAutomaticAnswerTimer->Cancel();
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleATDialingStarted()
// Handles AT dialing completed message from Phone UI
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::HandleATDialingStarted( const TBool aSucceed ) const
    {
    iClientServices->CommandHandlerMonitor()->DoCompleteCmdAtd( aSucceed ); 
    }    

// -----------------------------------------------------------------------------
// CPEMessageHandler::IsEmergencyAllowed()
// Checks if emergency call is allowed. 
// -----------------------------------------------------------------------------
//
TBool CPEMessageHandler::IsNetworkConnectionAllowed() const
    {
    TBool networkConnectionAllowed( EFalse );
    //It is safe to ignore error code here: a default value of EFalse is used if the get fails
    iExternalDataHandler.Get( EPENetworkConnectionAllowedSetting, networkConnectionAllowed );
    return networkConnectionAllowed;
    }
    
// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleClientCallData()
// Handle Client Call Data.
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::HandleClientCallData() 
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::HandleClientCallData" );
    
    CPhCltDialData* dialData = iClientServices->CallRequestMonitor()->ClientDialData();
    
    // Set already known Client information
    SetClientData( *dialData );
    
    if ( dialData->CallType() == EPhCltVideo || dialData->CallType() == EPhCltForcedVideo )
        {
        iDataStore.SetCallTypeCommand( EPECallTypeVideo );
        }
    else if( dialData->CallType() == EPhCltCallVoIP )
        {
        iDataStore.SetCallTypeCommand( EPECallTypeVoIP );
        }
    else
        {
        iDataStore.SetCallTypeCommand( EPECallTypeCSVoice );
        }
        
    // End other calls before dial SAT call
    if ( dialData->EndOtherCalls() )
        {
        HandleReleaseAll();
        TEFLOGSTRING( KTAINT,
            "PE CPEMessageHandler::HandleClientCallData > HandleReleaseAll()" );
         }       
    }    
    
// -----------------------------------------------------------------------------
// CPEMessageHandler::ClearCallAudio
// Notifies audio handling that there is no active call and audio should be routed
// accordingly.
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::ClearCallAudio()
    {
    TEFLOGSTRING( KTAMESINT, "PE CPEMGsmessageHandler::ClearCallAudio()" );
    TBool restrictingCalls( EFalse );

    if ( iCallHandling.GetNumberOfCalls() > 0 )
        {
        restrictingCalls = 
            // Single call states restricting the devsound notification
            iCallHandling.IsCallInState( EPEStateConnected ) ||
            iCallHandling.IsCallInState( EPEStateConnecting ) ||
            iCallHandling.IsCallInState( EPEStateDialing ) ||
            iCallHandling.IsCallInState( EPEStateHeld ) ||
            // Conference call states restricting the devsound notification
            ( iDataStore.CallState( KPEConferenceCallID )== EPEStateConnectedConference ) ||
            ( iDataStore.CallState( KPEConferenceCallID ) == EPEStateCreatingConference ) ||
            ( iDataStore.CallState( KPEConferenceCallID ) == EPEStateGoingOneToOne ) ||
            ( iDataStore.CallState( KPEConferenceCallID ) == EPEStateAddingConferenceMember ) ||
            ( iDataStore.CallState( KPEConferenceCallID ) == EPEStateAddedConferenceMember ) ||
            ( iDataStore.CallState( KPEConferenceCallID ) == EPEStateDroppingConferenceMember ) ||
            ( iDataStore.CallState( KPEConferenceCallID ) == EPEStateDroppedConferenceMember );
        }
    
    if ( restrictingCalls == EFalse )
        {
        // deactivate audio
        TEFLOGSTRING( KTAMESINT, "PE CPEMGsmessageHandler::ClearCallAudio() Deactivating" );
        iGsmAudioData.HandleCallEnding();
        }
    }


// -----------------------------------------------------------------------------
// CPEMessageHandler::RemovePreAndPostFix
// Remmove supplementary service prefix and dtmf postfix.
// Phone number can contain following parts: supplementary
// service prefix, main part and dtmf postfix.
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::RemovePreAndPostFix( 
        TDes& aString )
    {
    // Check that string contains only valid dtmf characters.
    if ( IsValidDtmfString( aString ))
        {
        TLex input( aString ); 
        
        RemovePrefix( input );
        // Take number part.
        HandleNumberPart( input, aString );        
        }
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::RemovePrefix
// Removes clir suppress/invoke prefix in the string.
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::RemovePrefix( 
        TLex& aLex )
    {
    TPtrC remainder( aLex.Remainder() );

    if ( EqualsLeft( remainder, KPEClirSuppress ) )
        {
        aLex.Inc( KPEClirSuppress().Length() );
        }
    else if ( EqualsLeft( remainder, KPEClirInvoke ) )
        {
        aLex.Inc( KPEClirSuppress().Length() );
        }
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleNumberPart
// Search the main part of the phone number.
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::HandleNumberPart(
        TLex& aLex,
        TDes& aNumberPart )
    {   
    TLexMark start;
    aLex.Mark( start );
  
    // Optional forced call prefixes. 
    if ( aLex.Peek() == KPENumberAsterisk || 
         aLex.Peek() == KPENumberHash ) 
         {
         aLex.Inc(); 
         }
         
    // Optional international prefix. 
    if ( aLex.Peek() == KPENumberPlus ) 
        { 
        aLex.Inc(); 
        } 

    // And the rest of number. 
    while ( ( aLex.Peek().IsDigit() ) ||  
            ( aLex.Peek() == KPENumberAsterisk ) || 
            ( aLex.Peek() == KPENumberHash ) ) 
        {
        aLex.Inc(); 
        }

    TPtrC mainpart( aLex.MarkedToken( start ) );
    const TInt length = mainpart.Length();
    if ( length <= KPEPhoneNumberMaxLength )
        {
        aNumberPart.Copy( mainpart );
        }
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::EqualsLeft
// -----------------------------------------------------------------------------
//
TBool CPEMessageHandler::EqualsLeft( 
        const TDesC& aString, 
        const TDesC& aPrefix )
    {
    TBool result = EFalse;

    if ( aPrefix.Length() <= aString.Length() )
        {
        TPtrC part( aString.Left( aPrefix.Length() ) );

        result = ( part == aPrefix );
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleReplaceActive
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleReplaceActive()
    {
    return iCallHandling.ReplaceActive();
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::CheckIfPhoneIsLockedL
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::CheckIfPhoneIsLockedL()
    {
    // Check if phone is locked
    TInt  keyLockStatus( EAutolockStatusUninitialized );
    TInt err = RProperty::Get( KPSUidCoreApplicationUIs, KCoreAppUIsAutolockStatus, keyLockStatus );
    const TBool phoneIsLocked = ( keyLockStatus > EAutolockOff );

    if ( phoneIsLocked && err == KErrNone )
        {
        // New call is not possible if device lock is on 
        TEFLOGSTRING2( KTAERROR,
            "PE CPEMessageHandler::CheckIfPhoneIsLockedL, keyLockStatus : %d", keyLockStatus );
        User::Leave( ECCPErrorAuthenticationFailed );
        }
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::IsActiveVideo
// Checks if there are any connected video calls
// -----------------------------------------------------------------------------
// 
TBool CPEMessageHandler::IsActiveVideo()
    {
    TBool activeVideoCall( EFalse );
    
    TInt callId = iCallHandling.GetCallIdByState( EPEStateConnected );
    if ( callId > ECCPErrorNotFound && iDataStore.CallType( callId ) == EPECallTypeVideo )
        {
        activeVideoCall = ETrue;
        }
    return activeVideoCall;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleServiceEnabled
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleServiceEnabled()
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::HandleServiceEnabled ");

    
    return iVoipNumberHandler->ContinueVoipDial();
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleRemotePartyInfoChanged
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::HandleRemotePartyInfoChanged( const TInt /*aCallId*/ )
    {        
    UpdateRemotePartyInfo(); 
        
    }


// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleUnattendedTransferRequestResponse
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleUnattendedTransferRequestResponse( 
        TBool aAcceptRequest )
    {
    TEFLOGSTRING( KTAINT, 
        "PE CPEMessageHandler::HandleUnattendedTransferRequestResponse" );
    
    if ( aAcceptRequest )
        {
        return iCallHandling.AcceptUnattendedTransfer();
        }
    else
        {
        return iCallHandling.RejectUnattendedTransfer();
        }
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleUnattendedTransfer
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::HandleUnattendedTransfer()
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::HandleUnattendedTransfer ");
    TInt errorCode = iCallHandling.DoUnattendedTransfer( 
            iDataStore.TransferTargetCommand() );
     
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::ForwardCallToAddress
// -----------------------------------------------------------------------------
//
TInt CPEMessageHandler::ForwardCallToAddress()
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::ForwardCallToAddress ");
    TInt errorCode = iCallHandling.ForwardCallToAddress( 
            iDataStore.ForwardAddressIndex() );
     
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleDisableService
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::HandleDisableService()
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::HandleDisableService ");
    iServiceHandling.DisableService();
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::IsValidDtmfString
// -----------------------------------------------------------------------------
//
TBool CPEMessageHandler::IsValidDtmfString( TDes& aString )
    { 
    TBool validDtmf = ETrue;

    for( TInt i = 0; i < aString.Length(); i++ )
        {
        if ( KErrNotFound == KValidDtmfChars().Locate( aString[i] ) )
            {
            validDtmf = EFalse;
            break;
            }      
        }
    return validDtmf;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::UpdateRemotePartyInfo
// -----------------------------------------------------------------------------
//
void CPEMessageHandler::UpdateRemotePartyInfo( )
    { 
    CPERemotePartyInfoMediator* mediatorUpdater = iModel.MediatorCommunicationHandler();
    if ( mediatorUpdater ) 
        {
        mediatorUpdater->UpdateRemotePartyInfo();
        }
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::AddSIMRejectedMoCsCallToLog
// -----------------------------------------------------------------------------
//	
TInt CPEMessageHandler::AddSIMRejectedMoCsCallToLog( const TInt aCallId )
    {
    TInt errorCode( ECCPErrorGeneral );
    
    errorCode = iCallHandling.GetCallInfo( *iCallInfo, aCallId );
    
    if ( errorCode == ECCPErrorNone )
        {
        TPEState callState;
        callState = iCallHandling.GetCallState( aCallId );
        TPECallType callType;
		callType = iDataStore.CallType( aCallId );

	    errorCode = ECCPErrorNotFound;
	            
        if ( EPEStateIdle == callState 
             && EPECallTypeCSVoice == callType )
            {
            SetPhoneNumberForCallLogging( aCallId );
            
            errorCode = UpdateClientInfo( aCallId );       
            
            // Calls have to log also without a contact (ECCPErrorNotFound).
            if ( errorCode == ECCPErrorNone || errorCode == ECCPErrorNotFound )
                {
                // Save the rest of information to EngineInfo.
                SetLoggingInfo( aCallId, callState );
                errorCode = iLogHandling.SaveCallEntry( aCallId );
                }
            }
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEMessageHandler::HandleDialCall
// Handles dial message from dial service
// 
// -----------------------------------------------------------------------------
// 
TInt CPEMessageHandler::HandleDialServiceCall(
    const TBool /*aClientCall*/ )
    {
    TEFLOGSTRING( KTAINT, "PE CPEMessageHandler::HandleDialCall" );
    TInt errorCode( ECCPErrorNone );
    //TODO 
    iModel.HandleInternalMessage( MPEPhoneModel::EPEMessageDialServiceCall );
    return errorCode;
    }

//  End of File
