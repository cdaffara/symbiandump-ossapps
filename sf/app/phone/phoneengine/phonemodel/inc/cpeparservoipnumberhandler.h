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
* Description:  Header file of the CPEParserVoipNumberHandler class.
*
*/


#ifndef CPEPARSERVOIPNUMBERHANDLER_H
#define CPEPARSERVOIPNUMBERHANDLER_H

//  INCLUDES
#include <cphonevoipnumberhandler.h>
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
class MPECallHandling;
class CPEMessageHandler;
class MPEDataStore;
class MPEServiceHandling;

// CLASS DECLARATION

/**
*  Handles phone related messages from the CPEPhoneModel object.
*
*  @lib PhoneEngineGsm.lib
*  @since since S60 5.0
*/
NONSHARABLE_CLASS( CPEParserVoipNumberHandler ) : public CPhoneVoipNumberHandler
    {
    public:  //Constructors and descructor
    
        static CPEParserVoipNumberHandler* NewL( CPEMessageHandler& aOwner, 
                                             MPECallHandling& aCallHandling,
                                             MPEServiceHandling& aServiceHandling,
                                             MPEDataStore& aDataStore );

        static CPEParserVoipNumberHandler* NewLC( CPEMessageHandler& aOwner, 
                                              MPECallHandling& aCallHandling,
                                              MPEServiceHandling& aServiceHandling,
                                              MPEDataStore& aDataStore );

        ~CPEParserVoipNumberHandler();
        
    public: // Functions from base classes
        /**
        * Process dial to voip number
        * @param aNumber main number part.
        * @param aPostFix DTMF part of the number
        */
        void ProcessDialToVoipNumberL( 
            const TDesC& aNumber,
            const TDesC& aDtmfPostfix
            );

        /**
        * Continue VoIP call after service is enabled.
        * @return error code.
        */ 
        TInt ContinueVoipDial() const;
        

    private:
    
        /**
        * Removes non-GSM standard postfix characters before dialling attempt.
        * @since 5.0
        * @param aPostfix postfix to modify.
        * @return modified postfix.
        */
       TPtrC FilterPostfix( TPtrC aPostfix );
        
        /**
        * C++ default constructor.
        */
        CPEParserVoipNumberHandler( 
            CPEMessageHandler& aOwner, 
            MPECallHandling& aCallHandling,
            MPEServiceHandling& aServiceHandling,
            MPEDataStore& aDataStore );

        void ConstructL();
            
    private:    
        // CPEMessageHandler object which owns this requester.
        CPEMessageHandler& iOwner;
        // CPEPhoneData is used to create request to the CallHandling subsystem.
        MPECallHandling& iCallHandling;
        // Reference to common data store
        MPEDataStore& iDataStore;
        // Reference to Service handling object
        MPEServiceHandling& iServiceHandling;
        // If phone number contains dtmf string it is saved here 
        TPEDtmfString iDtmfString; 
    };

#endif      // CPEPARSERVOIPNUMBERHANDLER_H
            
// End of File
