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
* Description:  This file contains the header file of the CPESystemCallState object
*
*/


#ifndef CPESYSTEMCALLSTATE_H
#define CPESYSTEMCALLSTATE_H

//INCLUDES
#include <pevirtualengine.h>
#include <e32property.h>
#include "mpedatastore.h"

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None. 

// FORWARD DECLARATIONS

// CLASS DECLARATION
class CPECallArrayOwner;
class MPEDataStore;
class CPESingleCall;

/**
*  Create call handling requests to Call Command object
*  @lib CallHandling.lib
*  @since Series60_4.0
*/
class CPESystemCallState 
        : public CBase
    {
    public:
        /**
        * Destructor.
        */
        ~CPESystemCallState();
        
        /**
        * Two-phased constructor.
        */
        static CPESystemCallState* NewL( 
                CPECallArrayOwner& aCallArrayOwner,
                MPEDataStore& aDataStore );

    private: //Constructors and descructor
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
        /**
        * C++ default constructor.
        */
        CPESystemCallState(
                CPECallArrayOwner& aCallArrayOwner,
                MPEDataStore& aDataStore );

    public:    //New functions
    
        /**
        * NotifySystemCallStateChanged.
        */
        void NotifySystemCallStateChanged( 
                MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
                TInt aCallId );
    
    private:    //New functions
    
        /**
        * HandleIncoming.
        */
        void HandleIncoming( TInt aCallId );
        
        /**
        * HandleDialling.
        */
        void HandleDialling( TInt aCallId );
        
        /**
        * HandleConnected.
        */
        void HandleConnected( TInt aCallId );
        
        /**
        * HandleDisconnecting.
        */
        void HandleDisconnecting( TInt aCallId );
        
        /**
        * HandleIdle.
        */
        void HandleIdle( TInt aCallId );
        
        /**
        * HandleHeld.
        */
        void HandleHeld( TInt aCallId );
        
        /**
        * HandleAnswering.
        */
        void HandleAnswering( TInt aCallId );
        
        /**
        * HandleConnecting.
        */
        void HandleConnecting( TInt aCallId );
        
        /**
        * HandleCallControlCapsChanged.
        */
        void HandleCallControlCapsChanged();
        
        /**
        * SetCallInformatioAPICallType.
        */
        void SetCallInformatioAPICallType( CPESingleCall& aCallData );
        
        /**
        * HandleRemotePartyChanged.
        */
        void HandleRemotePartyChanged( TInt aCallId );

    private:

        // Call array owner
        CPECallArrayOwner& iCallArrayOwner;
        // MPEDataStore
        MPEDataStore& iDataStore;
        // TPSCTsyCallState
        TPSCTsyCallState iCallStateTsy;
        // TPSCTsyCallType
        TPSCTsyCallType iCallTypeTsy;
        // P&S property for call state
        RProperty iCallStateProperty;
        // Current call state value
        TPSCTsyCallState iCallStateInCenRep;
        // Current call type value
        TPSCTsyCallType iCallTypeInCenRep;
        // P&S property for call type
        RProperty iCallTypeProperty;

    };

#endif // CPESYSTEMCALLSTATE_H

//    End of File
