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
* Description:
*
*/

#include <e32base.h>
#include <eikenv.h>
#include <mpeengineinfo.h>
#include "cphonestatemachine.h"
#include "mphonestorage.h"

class CPEEngineInfo: public CBase, public MPEEngineInfo
    {
public:
    CPEEngineInfo():
    iAudioOutput(EPENotActive)
        {
        }

    virtual ~CPEEngineInfo()
        {
    
        }

    TInt ContactId2 () const {}
    void SetProtocolError( TInt aError, TInt aCallId ) {}
    TInt ProtocolError( TInt aCallId ) {}
    TBool IsOutgoingCallBarringActivated() {}
    void SetOutgoingCallBarringActivated( 
            TBool aActivated ) {}            
		void SetIncomingCallForwarded( 
                TBool aForwarded, TInt aCallId ) {}
		TBool IncomingCallForwarded( TInt aCallId ) const {}                
    void SetContactId2( const TInt aContactId ) {}                   
    const CDesC8Array* ForwardAddressChoices( TInt aCallId ) const {}
    void SetForwardAddressChoices( const CDesC8Array* aArray, TInt aCallId ) {}
    void SetForwardAddressIndex( TInt aIndex ) {}
    TInt ForwardAddressIndex() {}
            
    const CCCECallParameters::TCCELineType& ALSLine() const{}
    const TBool& ALSLineSupport() const{}
    const TArray< TContactItemId > AlertForGroup() const {}
    const TBool& AudioMute() const{}
    const TPEAudioOutput& AudioOutput() const{  return iAudioOutput; }
    const TPEPhoneAudioRouteParameters& RouteParameters() const{}
    TBool AudioOutputAvailable( const TPEAudioOutput /*aOutput*/ ) const{}
    const TInt& AudioOutputPreference() const{}
    const TInt& AudioVolume() const{}
    RMobileCall::TMobileCallDirection CallDirection( const TInt /*aCallId*/ ) const{}
    const TInt& CallDuration() const{}
    TTimeIntervalSeconds CallDuration( const TInt /*aCallId*/ ) const{}
    const TPtrC CallerImage( const TInt /*aCallId*/ ) const {}
    const TPtrC CallerText( const TInt /*aCallId*/ ) const {}
    CFbsBitmap* CallerThumbnail( const TInt /*aCallId*/ ) const {}
    TBool HasCallerThumbnail( const TInt /*aCallId*/  ) const {}
    const TInt& CallId() const {}
    CCCECallParameters::TCCELineType CallALSLine( const TInt /*aCallId*/ ) const{}
    const MPEClientInformation& CallClientInformation( const TInt /*aCallId*/ ) const {}
    const TTime& CallStartTime( const TInt /*aCallId*/ ) const{}
    TPEState CallState( const TInt /*aCallId*/ ) const{}
    TPECallType CallType( const TInt /*aCallId*/ ) const{}
    const TPECallType& CallTypeCommand() const {}
    TBool IsSecureCall( const TInt /*aCallId*/ ) const{ return ETrue; }
    const TArray< TContactItemId > ContactGroups(
                const TInt /*aCallId*/ ) const {}
    RMobileCall::TMobileCallAiur DataCallAIURSpeed( 
               const TInt /*aCallId*/ ) const {}
    const TPEDtmfString& DtmfString() const{}
    const TPEErrorInfo& ErrorInfo() const{}
    const TChar& KeyCode() const {}
    const TInt& KeypadVolume() const {}
    const TCCPTone& InbandTone() const{}
    TBool LoggingIndicator( const TInt /*aCallId*/ ) const {}
    TBool MissedCall( const TInt /*aCallId*/ ) const {}
    const TNWNetworkRegistrationStatus& NetworkRegistrationStatus() const{}
    const TPEContactFileName& PersonalRingingTone(
                const TInt /*aCallId*/ ) const {}
    const TBool& PersonalToneStatus() const {}
    const TPEPhoneNumber& PhoneNumber() const{}
    const TBool& PhoneNumberIsServiceCode() const{}
    const TBool& ProactiveOrigin() const{}
    const TInt& ProfileId() const{}
    const TPEProfileName& ProfileName() const {}
    const TPEContactCompany& RemoteCompanyName(
                const TInt /*aCallId*/ ) const {}
    RMobileCall::TMobileCallRemoteIdentityStatus RemoteIdentity( 
                const TInt /*aCallId*/ ) const{}
    const TPEContactName& RemoteName( const TInt /*aCallId*/ ) const {}
    const TPEPhoneNumber& RemotePhoneNumber(
                const TInt /*aCallId*/ ) const {}
    const TPEPhoneNumberIdType& RemotePhoneNumberType(
                const TInt /*aCallId*/ ) const {}
    const TPtrC RemoteTextToSpeechText(
                const TInt /*aCallId*/ ) const {}
    const CDesCArray&  RemotePredefinedDtmfStrings(
                const TInt /*aCallId*/ ) const {}
    const TProfileRingingType& RingingType() const{}
    const TPEContactFileName& RingingTone( const TInt& /*aCallId*/ ) const {}
    const TInt& RingingVolume() const {}
    const TBool& TextToSpeech() const {}
    const TPEPhoneNumber& VoiceMailBoxNumberLine1() const  {}
    const TPEPhoneNumber& VoiceMailBoxNumberLine2() const {}
    void SetALSLine( const CCCECallParameters::TCCELineType& /*aALSLine*/ ){}
    void SetALSLineSupport( const TBool& /*aALSLineSupport*/ ){}
    void SetAudioMuteCommand( const TInt& /*aAudioMute*/ ){}
    void SetAudioOutputCommand( const TPEAudioOutput& /*aOutput*/,
                                                 const TBool /*aShowNote*/  ){}
    void SetAudioVolumeCommand( const TInt& /*aAudioVolume*/ ){}
    void SetCallId( const TInt& /*aCallId*/ ){}
    void SetCallALSLine( const CCCECallParameters::TCCELineType& /*aALSLine*/, const TInt /*aCallId*/ ){}
    void SetCallTypeCommand( const TPECallType& /*aCallType*/ ){}
    void SetDtmfStringCommand( const TPEDtmfString& /*aDtmfString*/ ){}
    void SetDynamicHscsdParams( const TInt /*aRxSlots*/ ) {}
    void SetInbandTone( const TCCPTone /*aTone*/ ) {}
    void SetKeyCode( const TChar& /*aKeyCode*/ ){}
    void SetPhoneNumber( const TPEPhoneNumber& /*aPhoneNumber*/ ){}
    const TBool& TwoDigitSupportStatus() const {}
    TUint32 ServiceIdCommand() const{}
    void SetServiceIdCommand( TUint32 /*aServiceId*/ ){}
    TUint32 ServiceId( const TInt /*aCallId*/ ) const {}
    const TPEActiveForwardInfo& ActiveForwardingInfo() const{}
    const TPEBarringInfo& BarringInfo() const{}
    TInt CallConference( const TInt /*aCallId*/ ) const{}
    TPECallControlCaps CallControlCaps( const TInt /*aCallId*/ ) const{}
    TBool CallForwarded( const TInt /*aCallId*/ ) const{}
    RMobilePhone::TMobileService CallService( const TInt /*aCallId*/ ) const{}
    TUint32 ConferenceCallCaps( const TInt /*aCallId*/ ) const{}
    const TName& ConferenceMemberName( const TInt /*aCallId*/ ) const{}
    TInt IsConferenceMemberId( const TInt /*aCallId*/ ) const{}
    const TPEPhoneNumber& EmergencyNumber() const{}
    TInt NumberOfConferenceMembers( const TInt /*aCallId*/ ) const{}
    const TPEPhoneNumber& RemoteColpNumber(
                const TInt /*aCallId*/ ) const {}
    const TPESimState& SimState() const
        {
        return EPESimStatusUninitialized;
        }
    TBool SecureSpecified() const{ return EFalse; }
    const TPESSCommandInfo& SSCommandInfo() const{}
    void SetEmergencyNumber( const TPEPhoneNumber& /*aEmergencyNumbers*/ ){}
    const TPEContactName& RemotePartyName( const TInt /*aCallId*/ ) const{}
    TInt ErrorCode() const{}
    const TDesC8& ContactLink( const TInt /*aCallId*/ ) const {}
    const TName& DataPortName() const {}
    const TPEPhoneNumber& UnattendedTransferTarget( 
                TInt /*aCallId*/ ) const {}
    void SetUnattendedTransferTarget( 
          const TPEPhoneNumber& /*aTarget*/, TInt /*aCallId*/ ) {}
    void SetTransferTargetCommand( const TPEPhoneNumber& /*aTransferTarget*/ ) {}
    
    const TPEPhoneNumber& TransferTargetCommand() const {}
    
    TBool CheckIfCallStateExists( const TPEState& aCallState ) {}
    
    TPEState ConferenceCallState() const {}
    
    TPECallOrigin CallOrigin( const TInt aCallId ) const {}
    
public:
    TPEAudioOutput iAudioOutput;
    
    };

class CPhoneEngine : public MPEPhoneModel
    {
public:
    CPhoneEngine()
        {
        iEngineInfo = new (ELeave) CPEEngineInfo();
        }
    
    ~CPhoneEngine()
        {
        delete iEngineInfo;
        }
    
    MPEEngineInfo* EngineInfo() 
         {
         return iEngineInfo;
         }
     
    void HandleMessage( const TInt /*aMessage*/ ) {}
     
    TPtrC NameByMessageToPhoneEngine( const TInt /*aMessage*/ ) const {}

    TPtrC NameByMessageFromPhoneEngine( const TInt /*aMessage*/ ) const {}
    
private:
    CPEEngineInfo* iEngineInfo;
    };


CPhoneStateMachine::CPhoneStateMachine(
    MPhoneViewCommandHandle* /*aViewCommandHandle*/ )
    {
    iPhoneEngine = new (ELeave) CPhoneEngine();
    }

CPhoneStateMachine::~CPhoneStateMachine()
    {
    delete iPhoneEngine;
    };

void CPhoneStateMachine::ChangeState( TInt aState ) {};

MPhoneState* CPhoneStateMachine::State() { return NULL; };

void CPhoneStateMachine::SetPhoneEngine( MPEPhoneModel* /*aPhoneEngine*/ ) {};

MPEPhoneModel* CPhoneStateMachine::PhoneEngine() { return iPhoneEngine; };

MPEEngineInfo* CPhoneStateMachine::PhoneEngineInfo() 
     {
     return iPhoneEngine->EngineInfo();
     }

MPEPhoneModel* CPhoneStateMachine::CreatePhoneEngineL( 
    MEngineMonitor& /*aEngineMonitor*/ ) { return iPhoneEngine; };
void CPhoneStateMachine::SetCallId( TInt /*aCallId*/ ) {};
void CPhoneStateMachine::SendPhoneEngineMessage( TInt /*aMessage*/ ) {};
MPhoneStorage* CPhoneStateMachine::PhoneStorage() { return NULL; }; 
CEikonEnv* CPhoneStateMachine::EikonEnv( ) const { return NULL; }

// end of file
