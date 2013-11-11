/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file of the CPEParserPhoneNumberHandler class.
*
*/


#ifndef CPEPARSERPHONENUMBERHANDLER_H
#define CPEPARSERPHONENUMBERHANDLER_H

//  INCLUDES
#include <cphonegsmphonenumberhandler.h>
#include <pevirtualengine.h>

// CONSTANTS
const TUint KDtmfPlus = '+';
const TUint KDtmfWait = 'w';

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class MPECallHandling;
class CPEMessageHandler;
class MPEDataStore;

// CLASS DECLARATION

/**
*  Handles phone related messages from the CPEPhoneModel object.
*
*  @lib PhoneEngineGsm.lib
*  @since since S90 1.0
*/
NONSHARABLE_CLASS( CPEParserPhoneNumberHandler ) : public CPhoneGsmPhoneNumberHandler
    {
    public:  //Constructors and descructor

        /**
        * C++ default constructor.
        */
        CPEParserPhoneNumberHandler( 
            CPEMessageHandler& aOwner, 
            MPECallHandling& aCallHandling,
            MPEDataStore& aGsmDataStore );


    public: // Functions from base classes

        /**
        * Process dial to number(derived from CPEGsmPhoneNumberHandler)
        * @since since S90 1.0
        * @param aNumber main number part.
        * @param aDtmfPostfix dtmf postfix.
        * @param aClir clir part.
        */
        void ProcessDialToNumberL( 
            const TDesC& aNumber, 
            const TDesC& aDtmfPostfix,
            TClir aClir );

    private: 
    
        /**
        * Removes non-GSM standard postfix characters before dialling attempt.
        * Leaves with ECCPErrorInvalidPhoneNumber if recognizes + char.
        * @since 5.0
        * @param aPostfix postfix to modify.
        * @return modified postfix.
        */
        TPtrC FilterPostfixL( TPtrC aPostfix );

        /**
        * Checks if phone is offline
        * @since 5.0
        * @return offline status
        */
        TBool IsPhoneOffline();

    private:    
        // CPEMessageHandler object which owns this requester.
        CPEMessageHandler& iOwner;
        // CPEPhoneData is used to create request to the CallHandling subsystem.
        MPECallHandling& iCallHandling;
        // Reference to common data store
        MPEDataStore& iDataStore;
    };

#endif      // CPEPARSERPHONENUMBERHANDLER_H
            
// End of File

