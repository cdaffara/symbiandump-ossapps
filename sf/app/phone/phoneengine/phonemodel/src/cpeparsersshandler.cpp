/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of 
*                CPEParserSSHandler class
*
*/



// INCLUDE FILES
#include "cpemessagehandler.h"
#include "cpeparsersshandler.h"
#include "mpephonemodelinternal.h"
#include "pepanic.pan"
#include <bldvariant.hrh>
#include <etelmm.h>
#include <featmgr.h>
#include <mpedatastore.h>
#include <nwdefs.h>
#include <psetcallbarring.h>
#include <psetcalldiverting.h>
#include <psetcallwaiting.h>
#include <psetcli.h>
#include <psetcontainer.h>
#include <talogger.h>

#include <psetwrapper.h>
#include <psuidivertnotehandler.h>
#include <psetcalldivertingwrapper.h>
#include <psuiwaitingnotehandler.h>
#include <psetcallwaitingwrapper.h>

// CONSTANTS
const TInt KPEDefinedBasicServiceGroup = 99;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEParserSSHandler::CPEParserSSHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEParserSSHandler::CPEParserSSHandler( 
        CPEMessageHandler& aOwner,       // The owner of this object.  
        MPEPhoneModelInternal& aModel       // The reference parameter of the 
        ) : iOwner( aOwner ),               // phone model object which is used
            iModel( aModel )                // to send message to the phone app.

    {
    iSupplementaryServicesCommandInfo.action = EPESSActionUnspecified;
    iSupplementaryServicesCommandInfo.type = EPESSTypeUnspecified;
    iSupplementaryServicesCommandInfo.group = RMobilePhone::EServiceUnspecified;
    TEFLOGSTRING( KTAOBJECT, "CPEParserSSHandler::CPEParserSSHandler" );
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEParserSSHandler* CPEParserSSHandler::NewL(
        CPEMessageHandler& aOwner,   // The owner of this object.  
        MPEPhoneModelInternal& aModel ) // The reference parameter of the phone 
                                        // model object which is used to send
                                        // message to the phone application
    {
    CPEParserSSHandler* self = new ( ELeave ) CPEParserSSHandler( aOwner, aModel );

    TEFLOGSTRING( KTAOBJECT, "CPEParserSSHandler::NewL" );

    return self;
    } //NewL

// Destructor
CPEParserSSHandler::~CPEParserSSHandler()
    {    
    delete iCli;
    delete iBarring;
    delete iWaiting;
    delete iSettings;
    
    delete iCallDivertingHandler;
    delete iPsetWrapper;
    

    TEFLOGSTRING( KTAOBJECT, "CPEParserSSHandler::~CPEParserSSHandler" );
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::GetBasicService.
// Returns RMobilePhone::TMobileService type of basic service value.
// -----------------------------------------------------------------------------
//
RMobilePhone::TMobileService CPEParserSSHandler::BasicService( 
        const TDesC& aBasicCode ) // It is basic service code.
    {
    RMobilePhone::TMobileService serviceGroup( RMobilePhone::EServiceUnspecified );

    if ( aBasicCode ==  KPENoServiceCode )
        {
        serviceGroup = RMobilePhone::EAllServices;
        }
    else if ( aBasicCode == KPEAllTeleservices )
        {                    
        serviceGroup = RMobilePhone::EAllTele;
        }
    else if ( aBasicCode == KPETelephony )
        {                    
        serviceGroup = RMobilePhone::ETelephony;
        }
    else if ( aBasicCode == KPEAllDataServices )
        {                    
        serviceGroup = RMobilePhone::EAllDataTele;
        }
    else if ( aBasicCode == KPEFacsimileServices )
        {                    
        serviceGroup = RMobilePhone::EFaxService;
        }
    else if ( aBasicCode == KPEShortMessageServices )
        {                    
        serviceGroup = RMobilePhone::EShortMessageService;
        }
    else if ( aBasicCode == KPEVoiceGroupCallService )
        {                    
        serviceGroup = RMobilePhone::EVoiceGroupCall;
        }
    else if ( aBasicCode == KPEVoiceBroadcastService )
        {                    
        serviceGroup = RMobilePhone::EVoiceBroadcast;
        }
    else if ( aBasicCode == KPEAllTeleservicesExceptSMS )
        {                    
        serviceGroup = RMobilePhone::EAllTeleExcSms;
        }
    else if ( aBasicCode == KPEAllBearerServices )
        {                    
        serviceGroup = RMobilePhone::EAllBearer;
        }
    else if ( aBasicCode == KPEAllAsyncServices )
        {                    
        serviceGroup = RMobilePhone::EAllAsync;
        }
    else if ( aBasicCode == KPEAllSyncServices )
        {                    
        serviceGroup = RMobilePhone::EAllSync;
        }
    else if ( aBasicCode == KPEAllDataCircuitSync )
        {                    
        serviceGroup = RMobilePhone::ESyncData;
        }
    else if ( aBasicCode == KPEAllDataCircuitAsync )
        {                    
        serviceGroup = RMobilePhone::EAsyncData;
        }
    // Not supported in MM ETel API, CR maded to Symbian.
    /*  
    else if ( aBasicCode == KPEAllDedicatedPacketAccess )
        {                    
        serviceGroup = EPE???;
        }
    */
    else if ( aBasicCode == KPEAllDedicatedPadAccess )
        {                    
        serviceGroup = RMobilePhone::EPadAccess;
        }
    else if ( aBasicCode == KPEAllGprsBearerServices )
        {                    
        serviceGroup = RMobilePhone::EAllGprsBearer;
        }
    else if ( aBasicCode == KPEAllPlmnSpecificTeleservices )
        {                    
        serviceGroup = RMobilePhone::EAllPlmnTele;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice1 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTele1;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice2 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTele2;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice3 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTele3;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice4 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTele4;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice5 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTele5;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice6 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTele6;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice7 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTele7;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice8 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTele8;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice9 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTele9;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice10 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTeleA;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice11 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTeleB;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice12 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTeleC;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice13 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTeleD;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice14 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTeleE;
        }
    else if ( aBasicCode == KPEPlmnSpecificTeleservice15 )
        {                    
        serviceGroup = RMobilePhone::EPlmnTeleF;
        }
    else if ( aBasicCode == KPEAllPlmnSpecificBearerServices )
        {                    
        serviceGroup = RMobilePhone::EAllPlmnBearer;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService1 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServ1;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService2 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServ2;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService3 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServ3;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService4 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServ4;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService5 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServ5;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService6 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServ6;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService7 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServ7;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService8 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServ8;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService9 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServ9;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService10 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServA;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService11 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServB;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService12 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServC;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService13 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServD;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService14 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServE;
        }
    else if ( aBasicCode == KPEPlmnSpecificBearerService15 )
        {                    
        serviceGroup = RMobilePhone::EPlmnBearerServF;
        }
    else if ( aBasicCode == KPEAuxVoiceService )
        {                    
        serviceGroup = RMobilePhone::EAuxVoiceService;
        }
    else
        {
        TEFLOGSTRING( KTAINT, "PE CPEParserSSHandler::BasicService: Unspecified Service code ");
        }
    iOwner.SetBasicServiceGroup( serviceGroup );
    return serviceGroup;
    }
 
// -----------------------------------------------------------------------------
// CPEParserSSHandler::Cancel
// Method cancel command from Settings utility subsystem or security module.
// Security module don't have return value from cancel command.
// -----------------------------------------------------------------------------
//    
TInt CPEParserSSHandler::Cancel()
    { 
    // request cancellation is taken care of mainly in
    // the actual ss-objects. They are also able to handle
    // situation, where there is no ongoing request.
    TInt returnValue( ECCPErrorNone );
    if ( iWaiting )
        {
        TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::Cancel() > iWaiting->CancelProcess()" );
        iWaiting->CancelProcess();
        }
    else if ( iBarring )
        {
        TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::Cancel() > iBarring->CancelCurrentRequest()" );
        iBarring->CancelCurrentRequest();
        }
    else if ( iDivert )
        {
        TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::Cancel() > iDivert->CancelCurrentRequest()" );
        iDivert->CancelCurrentRequest();
        }
    else if ( iCli )
        {
        TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::Cancel() > iCli->CancelAll()" );
        iCli->CancelAll();
        }
    else
        {
        TEFLOGSTRING( KTAERROR, "PE CPEParserSSHandler::Cancel() ! Not Found" );
        returnValue = ECCPErrorNotFound;
        }

    return returnValue;
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::ProcessBarringL
// Process barring procedures
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::ProcessBarringL( 
        TAction aAction,                // It is action
        TBarringProcedure aBarring,     // It contains barring procedure.
        const TDesC& aPassword,         // It should be barring password, if required.
        const TDesC& aBasicCode )       // It is basic service code.
    {
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSHandler::ProcessBarringL: Start" );
    // Notify Phone Application that an SS string was identified
    // and is about to be processed. This procedure is needed to quarantee
    // that an incoming call will not be blocked by the confirmation dialog
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );

    // convert into bsc
    TBasicServiceGroups bsc = ChangeToBsc( aBasicCode );
    // Basic service group for S90 UI...
    RMobilePhone::TMobileService serviceGroup = BasicService( aBasicCode );
    iSupplementaryServicesCommandInfo.group = serviceGroup;

    // connect to ss engine
    CreateCBObsL();
    TCallBarringSetting setBarring;
    // set basic service group to be used in line init.
    setBarring.iServiceGroup = EServiceGroupVoice;
    // copy the password
    setBarring.iPassword = aPassword;
    // chooses the right barring program
    switch( aBarring )
        {
        case EBarringAllOutgoingCalls:
            setBarring.iType = EBarringTypeAllOutgoing;
            iSupplementaryServicesCommandInfo.type = EPECallBarringAllOutgoing;
            break;
        case EBarringAllOutgoingInternationalCalls:
            setBarring.iType = EBarringTypeOutgoingInternational;
            iSupplementaryServicesCommandInfo.type = EPECallBarringAllOutgoingInternational;
            break;
        case EBarringAllOutgoingInternationalCallsExceptHome:
            setBarring.iType = 
                EBarringTypeOutgoingInternationalExceptToHomeCountry;
            iSupplementaryServicesCommandInfo.type = EPECallBarringAllOutgoingInternationalExcHome;
            break;
        case EBarringAllIncomingCalls:
            setBarring.iType = EBarringTypeAllIncoming;
            iSupplementaryServicesCommandInfo.type = EPECallBarringAllIncoming;
            break;
        case EBarringAllIncomingCallsWhenRoaming:
            setBarring.iType = EBarringTypeIncomingWhenRoaming;
            iSupplementaryServicesCommandInfo.type = EPECallBarringAllIncomingWhenRoaming;
            break;
        case EBarringAllServices:
            setBarring.iType = EBarringTypeAllServices;
            iSupplementaryServicesCommandInfo.type = EPECallBarringAllServices;
            break;
        case EBarringAllOutgoingServices:
            setBarring.iType = EBarringTypeAllOutgoingServices;
            iSupplementaryServicesCommandInfo.type = EPECallBarringAllOutgoingServices;
            break;
        case EBarringAllIncomingServices:
            setBarring.iType = EBarringTypeAllIncomingServices;
            iSupplementaryServicesCommandInfo.type = EPECallBarringAllIncomingServices;
            break;
        default:
            break;
        }
    // right function of the iBarring is called according to aAction
    switch( aAction )
        {
        case EActivate:
            setBarring.iSetting = EActivateBarring;
            iSupplementaryServicesCommandInfo.action = EPESSActivation;
            //iBarring->SetBarringL( setBarring, bsc );
            //iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
            //iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
            break;
        case EDeactivate: 
            setBarring.iSetting = ECancelBarring;
            iSupplementaryServicesCommandInfo.action = EPESSDeactivation;
            //iBarring->SetBarringL( setBarring, bsc );
            //iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
            //iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
            break;
        case EInterrogate:
            iSupplementaryServicesCommandInfo.action = EPESSInterrogation;
            //iBarring->GetBarringStatusL( EServiceGroupVoice, setBarring.iType );
            //iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
            //iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
            break;
        case ERegister:
        case EErase:
        default:
            iSupplementaryServicesCommandInfo.action = EPESSActionUnspecified;
            iModel.SendMessage( MEngineMonitor::EPEMessageSSRequestFailed );
            break;
        }
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSHandler::ProcessBarringL: End" );
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::ProcessClipStatusL.
// Process calling line identification presentation status.
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::ProcessClipStatusL()
    {
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSHandler::ProcessBarringL: Start" );
    // Notify Phone Application that an SS string was identified
    // and is about to be processed. This procedure is needed to quarantee
    // that an incoming call will not be blocked by the confirmation dialog
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );

    CreateCliObsL();
    iCli->GetClipModeL();

    iSupplementaryServicesCommandInfo.type = EPEClip;
    iSupplementaryServicesCommandInfo.action = EPESSInterrogation;
    iSupplementaryServicesCommandInfo.group = RMobilePhone::EServiceUnspecified;
    iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
    
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSHandler::ProcessBarringL: End" );
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::ProcessClirStatusL
// Process connected line presentation status.
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::ProcessClirStatusL()
    {
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSHandler::ProcessClirStatusL: Start" );
    // Notify Phone Application that an SS string was identified
    // and is about to be processed. This procedure is needed to quarantee
    // that an incoming call will not be blocked by the confirmation dialog
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );

    CreateCliObsL();
    iCli->GetClirModeL();

    iSupplementaryServicesCommandInfo.type = EPEClir;
    iSupplementaryServicesCommandInfo.action = EPESSInterrogation;
    iSupplementaryServicesCommandInfo.group = RMobilePhone::EServiceUnspecified;
    iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSHandler::ProcessClirStatusL: End" );
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::ProcessCnapStatusL
// Process Calling Name Presentation (CNAP) status check.
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::ProcessCnapStatusL()
    {
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSHandler::ProcessCnapStatusL: Start" );
    if ( FeatureManager::FeatureSupported( KFeatureIdPhoneCnap ) )
        {
        // Notify Phone Application that an SS string was identified
        // and is about to be processed. This procedure is needed to quarantee
        // that an incoming call will not be blocked by the confirmation dialog
        iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );

        CreateCliObsL();
        iCli->GetCnapL(); 

        iSupplementaryServicesCommandInfo.type = EPECnap;
        iSupplementaryServicesCommandInfo.action = EPESSInterrogation;
        iSupplementaryServicesCommandInfo.group = RMobilePhone::EServiceUnspecified;
        iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
        iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
        }
    else
        {
        iOwner.SetGsmParserErrorCode( ECCPErrorNotSupported );
        }
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSHandler::ProcessCnapStatusL: End" );
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::ProcessColpStatusL
// Process connected line presentation status.
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::ProcessColpStatusL()
    {
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSHandler::ProcessColpStatusL: Start" );
    // Notify Phone Application that an SS string was identified
    // and is about to be processed. This procedure is needed to quarantee
    // that an incoming call will not be blocked by the confirmation dialog
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );

    CreateCliObsL();
    iCli->GetColpModeL();

    iSupplementaryServicesCommandInfo.type = EPEColp;
    iSupplementaryServicesCommandInfo.action = EPESSInterrogation;
    iSupplementaryServicesCommandInfo.group = RMobilePhone::EServiceUnspecified;
    iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
    
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSHandler::ProcessColpStatusL: End" );
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::ProcessColrStatusL
// Process connected line restriction status.
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::ProcessColrStatusL()
    {
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSHandler::ProcessColrStatusL: Start" );
    // Notify Phone Application that an SS string was identified
    // and is about to be processed. This procedure is needed to quarantee
    // that an incoming call will not be blocked by the confirmation dialog
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );

    CreateCliObsL();
    iCli->GetColrModeL();

    iSupplementaryServicesCommandInfo.type = EPEColr;
    iSupplementaryServicesCommandInfo.action = EPESSInterrogation;
    iSupplementaryServicesCommandInfo.group = RMobilePhone::EServiceUnspecified;
    iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSHandler::ProcessColrStatusL: End" );
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::ProcessEmlppL
// Process enhanced Multi-Level Precedence and Pre-emption (eMLPP).
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::ProcessEmlppL( 
        TAction /*aAction*/,          // It as action.
        const TDesC& /*aNumber*/, // It is directory number.
        const TInt ) /*aServiceCode*/   // It is service code.
    {
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSHandler::ProcessEmlppL: " );
    //This feature is not supported.
    iOwner.SetGsmParserErrorCode( ECCPErrorNotSupported );
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::ProcessForwardingL
// If a parameter is not allowed for a procedure,then it is empty. If a 
// parameter is not explicitly given, then it is empty. If a parameter is 
// present, then it can be assumed to contain appropriate values.
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::ProcessForwardingL( 
        TAction aAction,                    // It is action
        TForwardingProcedure aForwarding,   // It contains forwarding procedure.
        const TDesC& aNumber,               // It is directory number.
        const TDesC& aBasicCode,            // It is basic service code.
        const TDesC& aTimer )               // It is timer.
    {
    TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessForwardingL: Start" );
     
    // Notify Phone Application that an SS string was identified
    // and is about to be processed. This procedure is needed to quarantee
    // that an incoming call will not be blocked by the confirmation dialog
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );

    // connect to ss engine
    CreateCFObsL();

    RMobilePhone::TMobileService serviceGroup = BasicService( aBasicCode );
    iSupplementaryServicesCommandInfo.group = serviceGroup;

    TCallDivertSetting setDivert;
    // forwarded-to number copied
    if ( aNumber.Length() > KPsetPhoneNumberLength )
        {   
        User::Leave( KErrArgument );
        }       
    setDivert.iNumber.Copy( aNumber );
    // default basic service group used for line initialization is set
    setDivert.iServiceGroup = EServiceGroupData;

    // convert into bsc
    TBasicServiceGroups bsc = ChangeToBsc( aBasicCode );
    // to get the divert indicator correctly 
    // (needed when voice is concerned)
    if ( bsc == EAllTeleAndBearer ||
        bsc == EAllTele ||
        bsc == ETelephony ||
        bsc == EAllTeleExcSms ||
        bsc == EAltTele )
        {
        setDivert.iServiceGroup = EServiceGroupVoice;
        }
    
    // status initialized
    setDivert.iStatus = EDivertingStatusUnknown;
    setDivert.iNoReplyTimer = 0;

    // chooses the correct forwarding program

    switch ( aForwarding )
        {
        case EForwardingNotReachable:
            setDivert.iCondition = EDivertConditionNotReachable;
            iSupplementaryServicesCommandInfo.type = EPECallForwardingNotReachable;
            break;
        case EForwardingNoReply:
            {
            // set the timer in case of timer concerned function call
            if ( aTimer.Length() != 0 )
                {
                TLex input( aTimer );
                User::LeaveIfError( input.Val( setDivert.iNoReplyTimer ) );
                }
            else 
                {
                setDivert.iNoReplyTimer = iDivert->GetTimerValueL();
                }
            setDivert.iCondition = EDivertConditionNoReply;
            iSupplementaryServicesCommandInfo.type = EPECallForwardingNoReply;
            break;
            }
        case EForwardingBusy:
            setDivert.iCondition = EDivertConditionBusy;
            iSupplementaryServicesCommandInfo.type = EPECallForwardingBusy;
            break;
        case EForwardingUnconditional:
            setDivert.iCondition = EDivertConditionUnconditional;
            iSupplementaryServicesCommandInfo.type = EPECallForwardingUnconditional;
            break;
        case EForwardingAll:
            setDivert.iCondition = EDivertConditionAllCalls;
            iSupplementaryServicesCommandInfo.type = EPECallForwardingAll;
            break;
        case EForwardingAllConditional:
            setDivert.iCondition = EDivertConditionAllConditionalCases;
            iSupplementaryServicesCommandInfo.type = EPECallForwardingAllConditional;
            break;
        default:
            break;
        }
    // right function chosen from iDivert according to aAction
    switch( aAction )
        {
        case EActivate:
            TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessForwardingL EActivate" );
            setDivert.iSetting = EActivateDiverting;
            iDivert->SetDivertingL( setDivert, bsc );
            iSupplementaryServicesCommandInfo.action = EPESSActivation;
            iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
            iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
            break;
        case EDeactivate: 
            TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessForwardingL EDeactivate" );
            setDivert.iSetting = ECancelDiverting;
            iDivert->SetDivertingL( setDivert, bsc );
            iSupplementaryServicesCommandInfo.action = EPESSDeactivation;
            iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
            iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
            break;
        case EInterrogate:
            TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessForwardingL EInterrogate" );
            iDivert->GetDivertingStatusL( EServiceGroupVoice, 
                setDivert.iCondition, bsc );
            iSupplementaryServicesCommandInfo.action = EPESSInterrogation;
            iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
            iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
            break;
        case ERegister:
            setDivert.iSetting = ERegisterDiverting;
            iDivert->SetDivertingL( setDivert, bsc );
            iSupplementaryServicesCommandInfo.action = EPESSRegistration;
            iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
            iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
            break;
        case EErase:
            setDivert.iSetting = EEraseDiverting;
            iDivert->SetDivertingL( setDivert, bsc );
            iSupplementaryServicesCommandInfo.action = EPESSErasure;
            iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
            iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
            break;
        default:
            TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessForwardingL default FAIL" );
            iSupplementaryServicesCommandInfo.action = EPESSActionUnspecified;
            iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
            iModel.SendMessage( MEngineMonitor::EPEMessageSSRequestFailed );
            break;
        }
    TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessForwardingL: End" ); 
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::ProcessMspL
// Process Multiple Subscriber Profile (MSP).
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::ProcessMspL( 
        TAction /*aAction*/,        // It is action.
        const TDesC& /*aNumber*/,   // It is directory number.
        const TInt ) /*aServiceCode*/ // It is service code.
    {
    TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessMspL" );
    //This feature is not supported.
    iOwner.SetGsmParserErrorCode( ECCPErrorNotSupported );
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::ProcessPasswordChange
// Process password change.
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::ProcessPasswordChangeL
        ( 
        TPasswordProcedure aPassword,   // It is password procedure.
        const TDesC& aOldPassword,      // It should be old password.
        const TDesC& aNewPassword,
        const TDesC& aVerifyNewPassword )
    {
    TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessPasswordChangeL: Start" ); 
    // Notify Phone Application that an SS string was identified
    // and is about to be processed. This procedure is needed to quarantee
    // that an incoming call will not be blocked by the confirmation dialog
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );

    CreateCBObsL(); 

    if ( aPassword == EPasswordBarring )
        {
        RMobilePhone::TMobilePhonePasswordChangeV2 password;
        CopyL( password.iOldPassword, aOldPassword );
        CopyL( password.iNewPassword, aNewPassword );
        CopyL( password.iVerifiedPassword, aVerifyNewPassword );
        iBarring->ChangePasswordL( password, ETrue );
        iSupplementaryServicesCommandInfo.type = EPECallBarringPassword;
        }
    else 
        {
        RMobilePhone::TMobilePhonePasswordChangeV2 password;
        CopyL( password.iOldPassword, aOldPassword );
        CopyL( password.iNewPassword, aNewPassword );
        CopyL( password.iVerifiedPassword, aVerifyNewPassword );
        iBarring->ChangePasswordL( password, EFalse );
        iSupplementaryServicesCommandInfo.type = EPESSTypeUnspecified;
        }

    iSupplementaryServicesCommandInfo.action = EPEPasswordOperation;
    iSupplementaryServicesCommandInfo.group = RMobilePhone::EServiceUnspecified;
    iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
    
    TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessPasswordChangeL: End" ); 
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::ProcessSpnpL
// Process Support for Private Numbering Plan (SPNP).
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::ProcessSpnpL( 
        TAction      /*aAction*/,        // It is action.
        const TInt ) /*aServiceCode*/ // It is service code.
    {
    TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessSpnpL" ); 
    //This feature is not supported.
    iOwner.SetGsmParserErrorCode( ECCPErrorNotSupported );
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::ProcessWaitingL
// Process waiting.
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::ProcessWaitingL( 
        TAction aAction,          // It is action.
        const TDesC& aBasicCode ) // It is basic service code.
    {
    TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessWaitingL: Start" );
    // Notify Phone Application that an SS string was identified
    // and is about to be processed. This procedure is needed to quarantee
    // that an incoming call will not be blocked by the confirmation dialog
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );

    // changes aBasicCode to basic service group
    TBasicServiceGroups bsc = ChangeToBsc( aBasicCode );
    // Basic service group for S90 UI...
    RMobilePhone::TMobileService serviceGroup = BasicService( aBasicCode );
    iSupplementaryServicesCommandInfo.group = serviceGroup;
    iSupplementaryServicesCommandInfo.type = EPECallWaiting;
    // connects to ss engine
    CreateCWObsL();
    switch( aAction )
        {
        case EActivate:
            TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessWaitingL EActivate" );
            iWaiting->SetCallWaitingL( 
                MPsetCallWaiting::EActivateCallWaiting, 
                bsc );
            iSupplementaryServicesCommandInfo.action = EPESSActivation;
            iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
            iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
            break;
        case EDeactivate: 
            TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessWaitingL EDeactivate" );
            iWaiting->SetCallWaitingL( 
                MPsetCallWaiting::EDeactivateCallWaiting, 
                bsc );
            iSupplementaryServicesCommandInfo.action = EPESSDeactivation;
            iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
            iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
            break;
        case EInterrogate:
            TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessWaitingL EInterrogate" );
            iWaiting->GetCallWaitingStatusL();
            iSupplementaryServicesCommandInfo.action = EPESSInterrogation;
            iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
            iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
            break;
        case ERegister:
        case EErase:
        default:
            TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessWaitingL default" );
            iSupplementaryServicesCommandInfo.action = EPESSActionUnspecified;
            iModel.DataStore()->SetSSCommandInfo( iSupplementaryServicesCommandInfo );
            iModel.SendMessage( MEngineMonitor::EPEMessageSSRequestFailed );
            break;
        } 
    TEFLOGSTRING( KTAMESINT, "PE CPEParserSSHandler::ProcessWaitingL: End" );
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::ChangeToBsc
// -----------------------------------------------------------------------------
//
TBasicServiceGroups CPEParserSSHandler::ChangeToBsc( 
    const TDesC& aBasicCode )
    {
    //null values must be handled as well, length == 0
    if ( aBasicCode.Length() == 0 )
        {
        return EAllTeleAndBearer;
        }
    TInt value( 0 ); 
    TLex input( aBasicCode );
    TInt error = input.Val( value );
    __ASSERT_ALWAYS( error > -1, 
        User::Panic( KPEPhoneEnginePanic, EPEPanicStupidUser ) );
    TBasicServiceGroups returnValue = EAllTeleAndBearer;
    if ( (EAllTele <= value && value <= EFax) ||
        (ESms <= value && value <= EAllSync) ||
        (ESyncData <= value && value <= EPadAccess) ||
        (EAllPlmnTele <= value && value <= EPlmnTeleF) ||
        (EAllPlmnBearer <= value && value <= EPlmnBearerServF) ||
        value == EAltTele ||
        value == KPEDefinedBasicServiceGroup )
        {
        returnValue = ( TBasicServiceGroups ) value;
        }
    else 
        {
        User::Panic( KPEPhoneEnginePanic, EPEPanicStupidUser );
        }
    return returnValue;      
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::ConnectToSsEngineL
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::ConnectToSsEngineL()
    {
    if ( iSettings )
        {
        return;
        }
    CleanupStack::PushL( TCleanupItem( DoClean, this ) );
 
    //creates the containers
    iSettings = CPsetContainer::NewL();

    CleanupStack::Pop( this );
    }
    
// -----------------------------------------------------------------------------
// CPEParserSSHandler::CreateCWObsL
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::CreateCWObsL()
    {
    ConnectToSsEngineL();
   
    if ( !iPsetWrapper )
        {
        iPsetWrapper = new PSetWrapper;    
        iCallWaitingWrapper = &iPsetWrapper->callWaitingWrapper();
        iCallWaitingHandler = new PsUiWaitingNoteHandler(*iCallWaitingWrapper);
        iWaiting = &iCallWaitingWrapper->getCPsetCallWaiting();
        iWaiting->SetRequestObserver(this);
        }
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::CreateCBObsL
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::CreateCBObsL()
    {
    ConnectToSsEngineL();
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::CreateCFObsL
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::CreateCFObsL()
    {
    if ( !iPsetWrapper )
        {
        iPsetWrapper = new PSetWrapper;    
        iCallDivertingWrapper = &iPsetWrapper->callDivertingWrapper();
        iCallDivertingHandler = new PsUiDivertNoteHandler(*iCallDivertingWrapper);
        iDivert = &iCallDivertingWrapper->getCPsetCallDiverting();
        iDivert->SetRequestObserver(this);
        }
    }
    
// -----------------------------------------------------------------------------
// CPEParserSSHandler::CreateCliObsL
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::CreateCliObsL()
    {
    ConnectToSsEngineL();
    }   
     
// -----------------------------------------------------------------------------
// CPEParserSSHandler::ProcessForwardingL
// -----------------------------------------------------------------------------
void CPEParserSSHandler::RequestComplete()
    {
    ResetVariables();
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::RequestStatusChanged
// -----------------------------------------------------------------------------
void CPEParserSSHandler::RequestStatusChanged( 
    TInt /*aNewStatus*/ )
    {
    // Phone app is not interested in Ss state changes.
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::DoClean
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::DoClean( 
    TAny* aAny )
    {
    CPEParserSSHandler* ssHandler = 
        static_cast< CPEParserSSHandler* >( aAny );
    
    ssHandler->ResetVariables();
    }       


// -----------------------------------------------------------------------------
// CPEParserSSHandler::ResetVariables
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::ResetVariables()
    {
    delete iBarring;
    iBarring = NULL;
    
    delete iCli;
    iCli = NULL;
    delete iSettings;
    iSettings = NULL;
    
    iDivert = NULL;
    delete iCallDivertingHandler;
    iCallDivertingHandler = NULL;
      
    iWaiting = NULL;
    delete iCallWaitingHandler;
    iCallWaitingHandler = NULL;
    
    delete iPsetWrapper;
    iPsetWrapper = NULL;
    }

// -----------------------------------------------------------------------------
// CPEParserSSHandler::CopyL
// -----------------------------------------------------------------------------
//
void CPEParserSSHandler::CopyL( 
        TDes& aDest, 
        const TDesC& aSource )
    {
    if ( aDest.MaxLength() < aSource.Length() )
        {
        User::Leave( KErrOverflow );
        }

    aDest = aSource;
    }

//  End of File  
