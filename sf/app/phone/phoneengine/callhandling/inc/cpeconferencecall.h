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
* Description:  This file contains the header file of the CPEConferenceCallData object
*
*/


#ifndef CPECONFERENCECALLDATA_H
#define CPECONFERENCECALLDATA_H

//INCLUDES
#include <pevirtualengine.h>
#include <ccce.h>
#include <mcceconferencecall.h>
#include <mcceconferencecallobserver.h>
#include "cpecall.h"

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class MPEMessageSender;

// CLASS DECLARATION

/**
* This class handles Conference Calls. It forwards events and messages to CPECallHandling class
* and uses Converged Call Engine interface to pass requests and receive event indications.
*
* @lib CallHandling.lib
* @since Series60_4.0
*/
class CPEConferenceCall 
        : public CPECall, 
          public MCCEConferenceCallObserver
    {
    public: // Constructors and destructors
        /**
        * Two-phased constructor.
        */
        static CPEConferenceCall* NewL( 
            MPEMessageSender& aOwner,
            CCCE& aConvergedCallEngine );
        
        /**
        * Two-phased constructor.
        * Conference call is made outof telephony control
        */
        static CPEConferenceCall* NewL( 
            MPEMessageSender& aOwner,
            MCCEConferenceCall& aCall );
            
        /**
        * Destructor.
        */
        virtual ~CPEConferenceCall();
      
    public:
    
        /**
        * Forwards message given as parameter to Phone Engine object
        *
        * @param  aMessage is the message to be forwarded
        */
        virtual void SendMessage( 
            const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage ); 
        
        /**
        * Forwards message given as parameter to Phone Engine object
        *
        * @param  aMessage is the message to be forwarded
        */
        virtual void SendMessage( 
            const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
            const TName& aName );

        
   public: // From MCCEConferenceCallObserver
   
        /**
        * A ConferenceCall event has occurred.
        *
        * @param aEvent Occurred event.
        * @param aReferredCall Pointer to call that the event refers to. 
        * Used e.g. for 
        * MCCEConferenceCall::AddCallL and 
        * MCCEConferenceCall::RemoveCallL
        * Default value is NULL.
        */
        void CallEventOccurred( 
            const MCCEConferenceCallObserver::TCCEConferenceCallEvent aEvent,
            MCCECall* aReferredCall = NULL  );
    
        /**
        * Conference state has changed.
        *
        * @param aStateContains the state of the conference call
        */
        void CallStateChanged( 
                     const MCCEConferenceCallObserver::TCCEConferenceCallState aState );
    
    
        /**
        * Conference capabilities have changed.
        * @param aCaps Updated call control capabilities
        */
        void CallCapsChanged( 
                     const MCCEConferenceCallObserver::TCCEConferenceCallCaps aCaps );

        /**
        * CallDurationChanged
        *
        * @param aDuration
        */
        void CallDurationChanged( const TTimeIntervalSeconds aDuration );

        /**
        * An error has occurred regarding the conference.
        *
        * @param aError Occurred error code. 
        */
        void ErrorOccurred( TCCPConferenceCallError aError );

            
    public:   // New Functions
    
        /**
        * Adds new member to Conference Call
        *
        * @param aCall the new call to be added
        */
        void AddCallL( MCCECall& aCall );

        /**
        * Returns Conference duration
        *
        * @param  TTimeIntervalSeconds& aDuration, current conference duration
        */
        void GetCallDuration( TTimeIntervalSeconds& aDuration );
        
        /**
        * Creates hangup request to call command object
        */    
        TInt HangUp();

        /**
        * Calls MMEtel's syncronic method to find out the count of Conference members
        * @return the number of members
        */
        TInt EnumerateCalls();

        /**
        * Returns conference call capabilities
        * @return the capabilities of this conference call
        */
        MCCEConferenceCallObserver::TCCEConferenceCallCaps CallCaps();

        /**
        * Swaps Conference Call between Active and Held states
        * @param  aSwap it tell that swapping
        * @return None
        */
        TInt Swap();
        
        /**
        * Returns that members name, which was added to or removed from Conference Call
        * @since Series60_4.0
        * @param   aCallName In this parameter is returned the TName information of member
        * @return None
        */
        void GetConferenceMemberName( TName& aCallName ) const;
        
        /**
        * Go one to one
        * @param  aCall
        * @return None
        */
        void GoOneToOneL( MCCECall& aCall );

    protected:
    
        /**
        * C++ default constructor.
        */
        CPEConferenceCall( MPEMessageSender& aOwner, MCCEConferenceCall& aCall );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CCCE& aConvergedCallEngine );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected:    // Data
        // Conference Call object. This class calls it's syncronic methods
        MCCEConferenceCall* iConferenceCall;
        // When added or removed event occurs, members name and callid is saved in these variables
        TName iCallName;
        // Conference calls capabilities
        MCCEConferenceCallObserver::TCCEConferenceCallCaps iCaps;
        //Current conference duration
        TTimeIntervalSeconds iDuration;
        //Last conference duration
        TTimeIntervalSeconds iLastConferenceDuration;
    };

#endif      // CPECONFERENCECALLDATA_H

// End of File
