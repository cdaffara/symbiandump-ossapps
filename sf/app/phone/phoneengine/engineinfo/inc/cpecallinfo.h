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
* Description:  Information container for a single call
*
*/


#ifndef C_CPECALLINFO_H
#define C_CPECALLINFO_H

#include <pevirtualengine.h>

class CPEClientInformation;

/**
 * Information container for a single call.
 */
NONSHARABLE_CLASS( CPECallInfo ) : public CBase
    {

public:
    
    /**
     * Two-phased constructor.
     * 
     * @return      The newly created object. Ownership is transferred to the caller.
     */
    static CPECallInfo* NewL();

    /**
     * Destructor.
     */
    ~CPECallInfo();

    /**
     * Resets all call information to their initial values.
     * 
     * @param   aIsConference   Flag indicating if the call is a conference call.
     */
    void Reset( TBool aIsConference );
    
    /**
     * Returns the client information of the call.
     * 
     * @return  Client information of the call.
     */ 
    CPEClientInformation& ClientInformation();
    
    /**
     * Sets the state of the call.
     * 
     * @param   aState          New state of the call.
     */
    void SetCallState( TPEState aState );
    
    /**
     * Returns the state of the call.
     * 
     * @return  State of the call.
     */
    TPEState CallState() const;
    
    /**
     * Sets the type of the call.
     * 
     * @param   aType           New type of the call.
     */
    void SetCallType( TPECallType aType );
    
    /**
     * Returns the type of the call.
     * 
     * @return  Type of the call.
     */
    TPECallType CallType() const;
    
    /**
     * Sets the call control capabilites.
     * 
     * @param   aCaps           Call control capabilites to set.
     */
    void SetCallControlCaps( TPECallControlCaps aCaps );
    
    /**
     * Returns the call control capabilities.
     * 
     * @return  Call control capabilities.
     */
    TPECallControlCaps CallControlCaps() const;

    /**
     * Sets the remote party name of the call as given by the network.
     * 
     * @param   aName           Remote party name of the call.
     */
    void SetRemotePartyName( const TPEContactName& aName );
    
    /**
     * Returns the remote party name of the call as given by the network.
     * 
     * @return  Remote party name of the call.
     */
    const TPEContactName& RemotePartyName() const;
    
    /**
     * Sets the whole outgoing phone number (invalid characters removed).
     * 
     * @param   aNumber         Phone number to set.
     */
    void SetWholeOutgoingPhoneNumber( const TPEPhoneNumber& aNumber );
    
    /**
     * Returns the whole outgoing phone number (invalid characters removed).
     * 
     * @return  Whole outgoing phone number.
     */
    const TPEPhoneNumber& WholeOutgoingPhoneNumber() const;

    /**
     * Sets the ALS line type of the call.
     * 
     * @param   aAlsLine        ALS line type to set.
     */
    void SetAlsLine( CCCECallParameters::TCCELineType aAlsLine );
    
    /**
     * Returns the ALS line type of the call.
     * 
     * @return  ALS line type of the call.
     */
    CCCECallParameters::TCCELineType AlsLine() const;
    
    /**
     * Sets the remote identity status of the call.
     * 
     * @param   aIdentity       Remote identity status to set.
     */
    void SetRemoteIdentity( RMobileCall::TMobileCallRemoteIdentityStatus aIdentity );
    
    /**
     * Returns the remote identity status of the call.
     * 
     * @return  Remote identity status of the call.
     */
    RMobileCall::TMobileCallRemoteIdentityStatus RemoteIdentity() const;
    
    /**
     * Sets the direction of the call.
     * 
     * @param   aDirection      Direction to set.
     */
    void SetCallDirection( RMobileCall::TMobileCallDirection aDirection );
    
    /**
     * Returns the direction of the call.
     * 
     * @return  Direction of the call.
     */
    RMobileCall::TMobileCallDirection CallDirection() const;

    /**
     * Sets the mobile call service type of the call.
     *  
     * @param   aService        Mobile call service type to set.
     */
    void SetService( RMobilePhone::TMobileService aService );
    
    /**
     * Returns the mobile call service type of the call.
     * 
     * @return  Mobile call service type of the call.
     */
    RMobilePhone::TMobileService Service() const;
    
    /**
     * Sets the conference member name of the call.
     * 
     * @param   aName           Conference member name of the call.
     */
    void SetConferenceMemberName( const TName& aName );
    
    /**
     * Returns the conference member name of the call.
     * 
     * @return  Conference member name of the call.
     */
    const TName& ConferenceMemberName() const;
    
    /**
     * Sets the call start time.
     * 
     * @param   aTime           Start time of the call.
     */
    void SetStartTime( const TTime& aTime );
    
    /**
     * Returns the start time of the call.
     * 
     * @return  Start time of the call.
     */
    const TTime& StartTime() const;
    
    /**
     * Sets the duration of the call.
     * 
     * @param   aDuration       Duration of the call in seconds.
     */
    void SetDuration( TTimeIntervalSeconds aDuration );
    
    /**
     * Returns the duration of the call.
     * 
     * @return  Duration of the call in seconds.
     */
    TTimeIntervalSeconds Duration() const;
    
    /**
     * Sets the service identifier of the call.
     * 
     * @param   aId             Service identifier to set.
     */
    void SetCallServiceId( TUint32 aId );
    
    /**
     * Returns the service identifier of the call.
     * 
     * @return  Service identifier of the call.
     */
    TUint32 CallServiceId() const;
    
    /**
     * Sets conference capabilities of the call.
     * 
     * @param   aCaps           Conference capabilities to set.
     */
    void SetConferenceCaps( TUint32 aCaps );
    
    /**
     * Returns the conference capabilities of the call.
     * 
     * @return  Conference capabilities of the call.
     */
    TUint32 ConferenceCaps() const;
    
    /**
     * Sets the identification number of the conference master.
     * 
     * @param   aConference     Identification number of the conference master to set.
     */
    void SetConference( TInt aConference );
    
    /**
     * Returns the identification number of the conference master.
     * 
     * @return  Identification number of the conference master.
     */
    TInt Conference() const;
    
    /**
     * Sets the identifier of the conference member call.
     * 
     * @param   aId             Identifier of the conference member call to set.
     */
    void SetConferenceMemberId( TInt aId );
    
    /**
     * Returns the identifier of the conference member call.
     * 
     * @return  Identifier of the conference member call.
     */
    TInt ConferenceMemberId();
    
    /**
     * Sets the number of conference members.
     * 
     * @param   aNumberOfConferenceMembers      Number of conference members.
     */
    void SetNumberOfConferenceMembers( TInt aNumberOfConferenceMembers );
    
    /**
     * Returns the number of conference members.
     * 
     * @return  Number of conference members.
     */
    TInt NumberOfConferenceMembers() const;
    
    /**
     * Sets the forced completion indicator for logging.
     * 
     * @param   aLog            Flag indicating if forced completion should be set.
     */
    void SetLogForcedCompletion( TBool aLog );
    
    /**
     * Returns the flag indicating if forced completion indicator for logging is used.
     * 
     * @return  Flag indicating if forced completion indicator for logging is used.
     */
    TBool LogForcedCompletion();
    
    /**
     * Enables or disables logging for the call.
     * 
     * @param   aEnabled        Flag enabling or disabling logging for the call.
     */
    void SetLoggingEnabled( TBool aEnabled );
    
    /**
     * Returns if logging is enabled for the call.
     * 
     * @return  Flag indicating if logging is enabled for the call.
     */
    TBool LoggingEnabled() const;
    
    /**
     * Sets the missed call status for the call.
     * 
     * @param   aMissedCall     Missed call status to set.
     */
    void SetMissedCall( TBool aMissedCall );
    
    /**
     * Returns the missed call status for the call.
     * 
     * @return  Missed call status for the call.
     */
    TBool MissedCall() const;
    
    /**
     * Sets the secure status for the call.
     * 
     * @param   aSecureStatus   Secure status to set.
     */
    void SetSecureStatus( TBool aSecureStatus );
    
    /**
     * Returns the secure status of the call.
     * 
     * @return  Secure status of the call.
     */
    TBool SecureStatus() const;
    
    /**
     * Sets the forwarded status for the call.
     * 
     * @param   aForwarded      Forwarded status to set.
     */
    void SetForwarded( TBool aForwarded );
    
    /**
     * Returns the forwarded status of the call.
     * 
     * @return  Forwarded status of the call.
     */
    TBool Forwarded() const;
    
    /**
     * Sets the resume held call status for the call.
     * 
     * @param   aResume         Resume held call status to set.
     */
    void SetResumeHeldCall( TBool aResume );
    
    /**
     * Returns the resume held call status of the call.
     * 
     * @return  Resume held call status of the call.
     */
    TBool ResumeHeldCall() const;

    /**
     * Returns the dtmf postfix of the call.
     * 
     * @return  dtmf postfix of the call.
     */
    const TPEDtmfString& DtmfPostFix() const;
    
    /**
     * Sets the dtmf postfix of the call.
     * 
     * @param aDtmfPostFix dtmf postfix to set.
     */
    void SetDtmfPostFix( const TPEDtmfString& aDtmfPostFix );

    /**
     * Sets the secure specified status for the call.
     * 
     * @param aSecureSpecified Secure specified status to set.
     */
    void SetSecureSpecified( TBool aSecureSpecified );

    /**
     * Returns the secure specified status of the call.
     * 
     * @return Secure specified status of the call.
     */
    TBool SecureSpecified() const;
    
    /**
     * Returns transfer target from the received unattended transfer 
     * request or KNullDesC if transfer target is not available.
     * 
     * @return  Transfer target.
     */
    const TPEPhoneNumber& UnattendedTransferTarget() const;
    
    /**
     * Sets transfer target from received unattended transfer request.
     *
     * @param   aTarget     Transfer target to set.
     */
    void SetUnattendedTransferTarget( const TPEPhoneNumber& aTarget );

    /**
     * Returns address choices from received call forward request 
     * or NULL if addresses are not available.
     * 
     * @return  New address.
     */
    const CDesC8Array* ForwardAddressChoices() const;
    
    /**
     * Sets address choices from received call forward request.
     *
     * @param   aArray     Address array.
     */
    void SetForwardAddressChoices( const CDesC8Array* aArray );
    
    /**
     * Sets array index for user selected address.
     *
     * @param   aIndex     Array index for selected address.
     */
    void SetForwardAddressIndex( TInt aIndex );
    
    /**
     * Sets the call index of a call. Note this is not a call id.
     */
    void SetCallIndex( TInt aCallIndex );
    
    /**
     * Gets the call index of a call. Note this is not the same as
     * call id.
     */
    TInt CallIndex() const;
    
    /**
     * Sets the origin of a call.
     */
    void SetCallOrigin( TPECallOrigin aOrigin );
    
    /**
     * Gets the origin of a call.
     */
    TPECallOrigin CallOrigin() const;
    
    /**
    * Sets the protocol spesific error code
    * @param aError is the error code from protocol.
    * @return None.
    */
    void SetProtocolError( TInt aError ); 
    
    /**
    * Returns the protocol spesific error code
    * @return Error code.
    */
    TInt ProtocolError() const; 
    
    /**
    * Sets incoming call forwarded flag.
    * @param aForwarded is call forwarded
    */
    void SetIncomingCallForwarded( TBool aForwarded );

    /**
     * Returns incoming call forwarded flag.
     */
    TBool IncomingCallForwarded() const;
    
private:
    
    /**
     * Constructor.
     */
    CPECallInfo();
    
    /**
     * Second-phase constructor.
     */
    void ConstructL();

private:
    
    /**
     * Client information of the call.
     * Owned.
     */
    CPEClientInformation* iClientInformation;
    
    /**
     * State of the call.
     */
    TPEState iCallState;
    
    /**
     * Type of the call.
     */
    TPECallType iCallType;
    
    /**
     * Call control capabilities.
     */
    TPECallControlCaps iCallControlCaps;

    /**
     * Name of the remote party as given by the network (SkypeID).
     */
    TPEContactName iRemotePartyName;
    
    /**
     * Whole outgoing phone number (invalid characters removed).
     */
    TPEPhoneNumber iWholeOutgoingPhoneNumber;

    /**
     * ALS line of the call.
     */
    CCCECallParameters::TCCELineType iALSLine;
    
    /**
     * Remote identity status.
     */
    RMobileCall::TMobileCallRemoteIdentityStatus iRemoteIdentity;
    
    /**
     * Direction of the call.
     */
    RMobileCall::TMobileCallDirection iCallDirection;

    /**
     * Mobile call service type.
     */
    RMobilePhone::TMobileService iService;

    /**
     * Conference member name.
     */
    TName iConferenceMemberName; 
    
    /**
     * Start time of the call.
     */
    TTime iStartTime;
    
    /**
     * Duration of the call in seconds.
     */
    TTimeIntervalSeconds iDuration;

    /**
     * Service identifier.
     */
    TUint32 iCallServiceId;
    
    /**
     * Conference capabilities.
     */
    TUint32 iConferenceCaps;
    
    /**
     * Identification number of the conference master.
     */
    TInt iConference;
    
    /**
     * Conference member call identifier.
     */
    TInt iConferenceMemberId;
    
    /**
     * Number of conference members.
     */
    TInt iNumberOfConferenceMembers;

    /**
     * Flag to indicate forced completion indicator for log handling.
     */
    TBool iLogForcedCompletion;
    
    /**
     * Flag to indicate logging enabled status.
     */
    TBool iLoggingEnabled;
    
    /**
     * Flag to indicate missed call status.
     */
    TBool iMissedCall;
    
    /**
     * Flag to indicate secure status.
     */
    TBool iSecureStatus;
    
    /**
     * Flag to indicate forwarded status of the incoming call.
     */
    TBool iForwarded;
    
    /**
     * Flag to indicate resume held call status.
     */
    TBool iResumeHeldCall;

    /**
     * Flag to indicate secure specified status.
     */    
    TBool iSecureSpecified;
    
    /**
     * Unattended transfer target.
     */
    TPEPhoneNumber iTransferTarget;
    
    /**
     * Dtmf postfix.
     */
    TPEDtmfString iDtmfPostFix;

    /**
     * Call forward address choices.
     */
    const CDesC8Array* iForwardAddressChoices;
    
    /**
     * User selected array index for call forward.
     */
    TInt iForwardAddressArrayIndex;
    
    /**
     * Call index of a call. Note this is not call id. 
     */
    TInt iCallIndex;
    
    /**
     * Origin of the call. 
     */
    TPECallOrigin iCallOrigin;
    
    /**
     * Protocol error code.
     */
    TInt iProtocolError;
    
    /**
     * Incoming call forwarded flag.
     */
    TInt iIncomingCallForwarded;
    };

#endif // C_CPECALLINFO_H
