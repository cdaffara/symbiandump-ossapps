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
* Description:  Header file of the CPEGsmParserPhoneNumberHandler class.
*
*/


#ifndef CPEPARSEREMERGENCYNUMBERHANDLER_H
#define CPEPARSEREMERGENCYNUMBERHANDLER_H

//  INCLUDES
#include <cphonegsmemergencynumberhandler.h>
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

// CLASS DECLARATION

/**
*  Handles phone related messages from the CPEPhoneModel object.
*
*  @lib PhoneEngineGsm.lib
*  @since since S60 5.0
*/
NONSHARABLE_CLASS( CPEParserEmergencyNumberHandler ) : public CPhoneGsmEmergencyNumberHandler
    {
    public:  //Constructors and descructor

        /**
        * C++ default constructor.
        */
        CPEParserEmergencyNumberHandler( 
            CPEMessageHandler& aOwner, 
            MPECallHandling& aCallHandling,
            MPEDataStore& aGsmDataStore );

    public: // Functions from base classes
        
        /**
        * Process dial to emergency number
        * @param aNumber main number part.
        */
        void ProcessDialToEmergencyNumberL( 
            const TDesC& aNumber
            );

    //public: //New functions

    private:    
        // CPEMessageHandler object which owns this requester.
        CPEMessageHandler& iOwner;
        // CPEPhoneData is used to create request to the CallHandling subsystem.
        MPECallHandling& iCallHandling;
        // Reference to common data store
        MPEDataStore& iDataStore;
    };

#endif      // CPEPARSEREMERGENCYNUMBERHANDLER_H
            
// End of File
