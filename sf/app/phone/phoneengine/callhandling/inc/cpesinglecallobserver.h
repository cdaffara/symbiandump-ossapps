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
* Description:  This file contains the header file of the CPESingleCallObserver object
*
*/


#ifndef CPESINGLECALLOBSERVER_H
#define CPESINGLECALLOBSERVER_H

//INCLUDES
#include <mccecall.h>
#include <mccecallobserver.h>
#include <pevirtualengine.h>

#include "cpecall.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None. 

// FORWARD DECLARATIONS
class MPEMessageSender;

// CLASS DECLARATION

/**
*  Create call handling requests to Call Command object
*  @lib CallHandling.lib
*  @since Series60_4.0
*/
class CPESingleCallObserver
        : public CPECall, 
          public MCCECallObserver
    {
    protected: //Constructors and descructor
        /**
        * C++ default constructor.
        */
        CPESingleCallObserver( MPEMessageSender& aOwner );

        /**
        * Destructor.
        */
        virtual ~CPESingleCallObserver();

    public:    // from MCCECallObserver
    
        /**
        * Call errors are notified using this interface. 
        *
        * @since S60 v3.2
        * @param aError Occurred error as integer,
        * return none
        */
        IMPORT_C void ErrorOccurred( const TCCPError aError );
        
        /**
        * Call state notificfation. 
        * TODO: The observer need to verify the correctness of given state.
        * CCE will do at least basic call state change checking but also the client 
        * needs to verify the correctness.
        *
        * @since S60 v3.2
        * @param aState New state of the call
        * return none
        */
        IMPORT_C void CallStateChanged( const CCPCall::TCallState aState );

        /**
        * The state of the call has changed with inband tone, meaning network is not playing 
        * the tone relating to the state.
        * @since S60 3.2
        * @param aState Call state.
        * @return none
        */
        IMPORT_C void CallStateChangedWithInband( const CCPCall::TCallState aState );

        /**
        * An event for the call has occurred. 
        * TODO: the observer need to verify event correcness.
        *
        * @since S60 v3.2
        * @param aEvent Occurred call event
        * return none
        */
        IMPORT_C void CallEventOccurred( const MCCECallObserver::TCCECallEvent aEvent );
        
        /**
        * Notifies observer the change in call duration.
        *
        * @since S60 v3.2
        * @param aDuration Duration of the call in seconds
        * return none
        */
        IMPORT_C void CallDurationChanged( const TTimeIntervalSeconds aDuration );
        
        /**
        * Notifies observer about the changes in call control caps.
        *
        * @since S60 v3.2
        * @param aCaps Contains updated call control capability flags
        * return none
        */
        IMPORT_C void CallCapsChanged( const MCCECallObserver::TCCECallControlCaps aCaps );
        
        /**
        * Notifies observer about transfer made by remote end
        *
        * @since S60 v3.2
        * @param aAttended ETrue: if transfer type is attended, EFalse: type is unattended
        * @param aDestination Address where the call is to be transferred. Given as received in CCE 
        * observer needs to verify the address, e.g. it can be KNullDesC.
        * TODO anonymous transfer target - quess cannot be occurring? 
        * A-B -> B-C, C has requested anonymity in network???
        */
        IMPORT_C void HandleTransfer( const TBool aAttended, 
                                      const TDesC& aDestination );
       
    public:    //New functions 
                     
        /**
        * Returns call duration time
        * @param aDuration the duration value of the call
        */    
        void GetCallDuration( TTimeIntervalSeconds& aDuration );

        /**
        * Returns call control capabilities
        * @param aCallControlCaps call control capabilities
        */    
        void GetCallControlCaps( 
            MCCECallObserver::TCCECallControlCaps& aCallControlCaps ) const;
   
        /**
        * Returns the reference to the call object handle. Handle is owned by a derived object.
        * @return Reference to the call object handle
        */            
        MCCECall& Call();

        /**
        * Sets the reference to the call object handle. Handle is owned by a derived object.
        * @return Reference to the call object handle
        */            
        void SetCall( MCCECall& aCall );
                
        /**
        * Sets Missed call indicator.
        * @param aMissed, Missed call indicator. 
        * @return KErrNone.
        */
        virtual void SetMissedCall( TBool aMissed ) = 0;

        /**
         * Returns transfer target from last received unattended transfer
         * request.
         * @return   Transfer target.
         */
        const TPEPhoneNumber& UnattendedTransferTarget() const;
        
        /**
         * Returns array of possible addresses from the received call forward 
         * request or NULL if addresses are not available.
         * @return   Address array.
         */
        const CDesC8Array* ForwardAddressChoices() const;

    protected:

        // Includes dialled number
        TPEPhoneNumber iDialledNumber; // Mihin t‰t‰ on tarkoitus k‰ytt‰‰?
        // Call duration time.
        TTimeIntervalSeconds iDuration;
        // iControlCaps contains the new capabilities 
        MCCECallObserver::TCCECallControlCaps iControlCaps;
        // handle to mobile call object
        MCCECall* iCall;
        
        /**
         * Transfer target from previous last unattended transfer reqeust.
         */
        TPEPhoneNumber iTransferTarget;
    };

#endif // CPESINGLECALLOBSERVER_H

//    End of File
