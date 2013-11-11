/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPELogInfo class.
*
*/

#ifndef CPELOGINFO_H
#define CPELOGINFO_H


#include <e32base.h>
#include <LogsApiConsts.h>

#include "mpedatastore.h"

_LIT( KPELogUnknownText, "Unknown" );


/**
 *  Log info data.
 *
 *  @lib loghandling.dll
 *  @since S60 v5.0
 */
class CPELogInfo : public CBase
    {
public:
    //Log Event Type
    enum TPELogEventType
        {
        EPEUnknownEvent = 0,
        EPEVoiceEvent,
        EPEVideoCallEvent,
        EPEEmergecyEvent,
        EPEVoIPEvent
        };

    static CPELogInfo* NewL();
    
    /**
    * Destructor.
    */
    virtual ~CPELogInfo();

    /**
     * Save event data. Basic log data existence guaranteed for class.
     * 
     * @since S60 v5.0
     * @param aCallId Identification for call
     * @param aDataStore Logs datastore
     */
    void SetEventData( TInt aCallId, const MPEDataStore& aDataStore );

    /**
     * Copy event data.
     * 
     * @since S60 v5.0
     * @param aLogInfo Log info to be copied
     */
    void CopyL( const CPELogInfo& aLogInfo );

    /**
     * Restore default values.
     * 
     * @since S60 v5.0
     */
    void Reset();

    /**
     * Set the logged event type data,voice.
     * 
     * @since S60 v5.0
     * @param aEventType LogEventType
     */
    void SetEventType( TPELogEventType aEventType );
    
    /**
     * The logged event type data,voice.
     * 
     * @since S60 v5.0
     */
    CPELogInfo::TPELogEventType EventType() const;
    
    /**
     * Set call direction (0=MO/1=MT).
     * 
     * @since S60 v5.0
     * @param aCallDirection Call direction
     */
    void SetCallDirection( RMobileCall::TMobileCallDirection aCallDirection );
    
    /**
     * Call direction (0=MO/1=MT).
     * 
     * @since S60 v5.0
     */
    RMobileCall::TMobileCallDirection CallDirection() const;
    
    /**
     * Set call id.
     * 
     * @since S60 v5.0
     * @param aCallId Call Id
     */
    void SetCallId( TInt aCallId );
    
    /**
     * Call id.
     * 
     * @since S60 v5.0
     */
    TInt CallId() const;
    
    /**
     * Set phone number.
     * 
     * @since S60 v5.0
     * @param aPhoneNumber Phone number
     */
    void SetPhoneNumber( const TPEPhoneNumber& aPhoneNumber );
    
    /**
     * Phone number.
     * 
     * @since S60 v5.0
     */
    const TPEPhoneNumber& PhoneNumber() const;
    
    /**
     * Set firstname + lastname.
     * 
     * @since S60 v5.0
     * @param aName Name
     */
    void SetName( const TPEContactName& aName );
    
    /**
     * Firstname + lastname.
     * 
     * @since S60 v5.0
     */
    const TPEContactName& Name() const;
    
    /**
     * Set call state.
     * 
     * @since S60 v5.0
     * @param aCallState Call state
     */
    void SetCallState( TPEState aCallState );
    
    /**
     * Call state.
     * 
     * @since S60 v5.0
     */
    TPEState CallState() const;
    
    /**
     * Set call duration (seconds).
     * 
     * @since S60 v5.0
     * @param aDuration Call duration
     */
    void SetDuration( TTimeIntervalSeconds aDuration );
    
    /**
     * Call duration (seconds).
     * 
     * @since S60 v5.0
     */
    TTimeIntervalSeconds Duration() const;
    
    /**
     * Set call line.
     * 
     * @since S60 v5.0
     * @param aCurrentLine Current line type
     */
    void SetCurrentLine( CCCECallParameters::TCCELineType aCurrentLine );
    
    /**
     * Call line.
     * 
     * @since S60 v5.0
     */
    CCCECallParameters::TCCELineType CurrentLine() const;
    
    /**
     * Set phone number id.
     * 
     * @since S60 v5.0
     * @param aPhoneNumberId Phone number id type
     */
    void SetPhoneNumberId( TPEPhoneNumberIdType aPhoneNumberId );
    
    /**
     * Phone number id.
     * 
     * @since S60 v5.0
     */
    TPEPhoneNumberIdType PhoneNumberId() const;
    
    /**
     * Set logging indicator.
     * 
     * @since S60 v5.0
     * @param aLoggingEnabled Logging state
     */
    void SetLoggingEnabled( TBool aLoggingEnabled );
    
    /**
     * Logging indicator.
     * 
     * @since S60 v5.0
     */
    TBool LoggingEnabled() const;
    
    /**
     * Set start time.
     * 
     * @since S60 v5.0
     * @param aCallStartTime Call start time
     */
    void SetCallStartTime( TTime aCallStartTime );
    
    /**
     * Start time.
     * 
     * @since S60 v5.0
     */
    TTime CallStartTime() const;
    
    /**
     * Set missed call indicator.
     * 
     * @since S60 v5.0
     * @param aMissedCall Call missed status
     */
    void SetMissedCall( TBool aMissedCall );
    
    /**
     * Missed call indicator.
     * 
     * @since S60 v5.0
     */
    TBool MissedCall() const;
    
    /**
     * Set indication if entry should be completed.
     * 
     * @since S60 v5.0
     * @param aForcedCompletion Force completion state
     */
    void SetForcedCompletion( TBool aForcedCompletion );
    
    /**
     * Indication for entry if it should be completed.
     * 
     * @since S60 v5.0
     */
    TBool ForcedCompletion() const;
    
    /**
     * Set service id for the call.
     * 
     * @since S60 v5.0
     * @param aServiceId Service id
     */
    void SetServiceId( TUint32 aServiceId );
    
    /**
     * Service id of the call.
     * 
     * @since S60 v5.0
     */
    TUint32 ServiceId() const;
    
    /**
     * Set VoIP address.
     * 
     * @since S60 v5.0
     * @param aVoipAddress VoIP address
     */
    void SetVoipAddress( const TPEPhoneNumber& aVoipAddress );
    
    /**
     * VoIP address.
     * 
     * @since S60 v5.0
     */
    const TPEPhoneNumber& VoipAddress() const;
    
    /**
     * Set my address.
     * 
     * @since S60 v5.0
     * @param aMyAddress My address
     */
    void SetMyAddress( const TPEPhoneNumber& aMyAddress );
    
    /**
     * My address.
     * 
     * @since S60 v5.0
     */
    const TPEPhoneNumber& MyAddress() const;
    
    /**
     * Save contact link. Ownership is transferred.
     * 
     * @since S60 v5.0
     * @param aCallId Identification for call
     * @param aDataStore Logs datastore
     */
    void SetContactLink( const HBufC8* aContactLink );
    
    /**
     * Contact link to phonebook.
     * 
     * @since S60 v5.0
     */
    const HBufC8& ContactLink() const;
    
    
private:
    CPELogInfo();

    
private: // data, public because no processing done in this class
    TPELogEventType iEventType;     // The logged event type data,voice.
    RMobileCall::TMobileCallDirection iCallDirection;// Call direction (0=MO/1=MT)
    TInt iCallId;
    TPEPhoneNumber iPhoneNumber;
    TPEContactName iName;           // Firstname + Lastname
    TPEState iCallState;
    TTimeIntervalSeconds iDuration; // Call duration (seconds)
    CCCECallParameters::TCCELineType iCurrentLine;       // Call line.
    TPEPhoneNumberIdType iPhoneNumberId;
    TBool iLoggingEnabled;          // Logging indicator
    TTime iCallStartTime;           // Start time
    TBool iMissedCall;              // Missed call indicator
    TBool iForcedCompletion;        // indicates that entry should be completed
    TUint32 iServiceId;             // Service Id for the call  
    /** voip address of remote party */
    TPEPhoneNumber iVoipAddress;
    /** own address during the logged call */
    TPEPhoneNumber iMyAddress;
    
    /**
     * Contact id/link
     * Own.
     */
    const HBufC8* iContactLink;

    };

#endif // CPELOGINFO_H
