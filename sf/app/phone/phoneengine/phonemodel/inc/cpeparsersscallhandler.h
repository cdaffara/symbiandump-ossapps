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
* Description:  This file contains the header file of the CPEParserSSCallHandler 
*                class
*
*/


#ifndef CPEPARSERSSCALLHANDLER_H
#define CPEPARSERSSCALLHANDLER_H

//INCLUDES
#include <cphonegsmsscallhandler.h>
#include <pevirtualengine.h>

// CONSTANTS
// None.

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class CPEMessageHandler;
class CPEGsmPhoneData;
class MPEPhoneModelInternal;
class CPEManualCallControlHandler;

// CLASS DECLARATION

/**
*  Provides processing for supplementary service related Phone Parser requests
*
*  @lib phoneenginegsm.lib
*  @since Series 60 4.0
*/
NONSHARABLE_CLASS( CPEParserSSCallHandler ) : public CPhoneGsmSsCallHandler
{
    public:  //Constructors and descructor    
        /**
        * C++ default constructor.
        */
        CPEParserSSCallHandler( 
            CPEMessageHandler& aOwner, 
            MPEPhoneModelInternal& aModel,
            CPEManualCallControlHandler& aManualCallControlHandler );

        /**
        * Destructor.
        */
        virtual ~CPEParserSSCallHandler();

    public: // Functions from base classes

        /**
        * Ends a call and accepts another.
        *
        * Command: 1 SEND.
        * @param None. 
        * @return None. 
        */
        void ProcessEndAndAcceptL();

        /**
        * End specific active call.
        *
        * Command: 1X SEND.
        * @param aCallNo, the number of the call
        * @return None.
        */
        void ProcessEndCallL( TInt aCallNo );

        /**
        * Reject a call.
        *
        * Command: 0 SEND.
        * @param None.
        * @return None.
        */
        void ProcessEndOrRejectL();

        /**
        * Perform explicit call transfer
        *
        * Command: 4 SEND.
        * @param None.
        * @return None.
        */ 
        void ProcessExplicitCallTransferL();

        /**
        * Hold all active calls except specific call.
        *
        * Command: 2X SEND.
        * @param None.
        * @return None.
        */
        void ProcessHoldAllCallsExceptL( TInt aCallNo );

        /**
        * Join two calls to conference.
        *
        * Command: 3 SEND.
        * @param aCallNo, the number of the call.
        * @return None.
        */
        void ProcessJoinL();

        /**
        * Swap
        *
        * Command: 2 SEND.
        * @param None.
        * @return None.
        */
        void ProcessSwapL();

    private:  
        // CPEMessageHandler object which owns this requester.
        CPEMessageHandler& iOwner;
        // MPEPhoneModelInternal is used to send message to the phone application
        MPEPhoneModelInternal& iModel;
        // Manual Call Control Handler
        CPEManualCallControlHandler& iManualCallControlHandler;
    };

#endif // CPEPARSERSSCALLHANDLER_H
            
// End of File
