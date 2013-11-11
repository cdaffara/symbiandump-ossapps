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
* Description:  Utility class for voip-states.
*
*/


#ifndef C_CPHONESTATEUTILSVOIP_H
#define C_CPHONESTATEUTILSVOIP_H

#include <e32base.h>

class MPhoneStateMachine;
class MPhoneViewCommandHandle;
class CPhCltEmergencyCall;

/**
 *  Provides common utility functions for voip-states.
 *
 *  @lib PhoneUIVoIPExtension.dll
 *  @since S60 v5.1
 */
NONSHARABLE_CLASS( CPhoneStateUtilsVoip ) : public CBase
    {

public:

    /**
     * Two-phased constructor.
     * @param    aStateMachine       Handle to the state machine.
     * @param    aViewCommandHandle  Handle to the PhoneUIView.
     */
    static CPhoneStateUtilsVoip* NewL( MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle );
    
    /**
     * Two-phased constructor.
     * @param    aStateMachine       Handle to the state machine.
     * @param    aViewCommandHandle  Handle to the PhoneUIView.
     */
    static CPhoneStateUtilsVoip* NewLC( MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle );
    
    /**
     * Destructor.
     */
    virtual ~CPhoneStateUtilsVoip();

    /**
     * Dials a VoIP call.
     * 
     * @param   aNumber     Phone number.
     * @param   aServiceId  Service identifier.
     */
    void DialVoIPL( const TDesC& aNumber, TUint aServiceId = 0 );
    
    /**
     * Checks is VoIP preferred call type. If VoIP is preferred, service 
     * identifier is returned by the parameter aServiceId.
     * 
     * @param   aServiceId  Service identifier.
     * @return  ETrue if VoIP is preferred, EFalse otherwise.
     */
    TBool IsVoipPreferredCall( TUint& aServiceId );
    
    /**
     * Checks whether number entry contain VoIP number.
     * 
     * @return  ETrue if number entry contains VoIP number, EFalse otherwise.
     */
    TBool IsVoipNumber();

    /**
     * Selects VoIP service and makes dial.
     * 
     * @param  aNumber      Phone number.
     * @param  aServiceId   Service identifier for the call.
     */
    void SelectServiceAndDialL( const TDesC& aNumber = KNullDesC,
        TUint aServiceId = 0 );

    /**
     * Checks is the given number valid emergency number.
     * 
     * @param   aNumber     Number to be checked.
     * @return  ETrue if number is emergency number, EFalse otherwise.
     */
    TBool IsEmergencyNumber( const TDesC& aNumber ) const;
    
private:

    CPhoneStateUtilsVoip( MPhoneStateMachine& aStateMachine,
        MPhoneViewCommandHandle& aViewCommandHandle );

    void ConstructL();

private: // data

    /** 
     * Number entry string buffer.
     * Own.
     */
    HBufC* iBuffer;

    /** 
     * Phone client emergency call.
     * Own.
     */
    CPhCltEmergencyCall* iEmergencyCall;
    
    /**
     * Interface to the state machine.
     * Not own.
     */
    MPhoneStateMachine& iStateMachine;
    
    /**
     * Interface to the phone ui view.
     * Not own.
     */
    MPhoneViewCommandHandle& iViewCommandHandle;
    };


#endif // C_CPHONESTATEUTILSVOIP_H
