/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the CPhoneGsmMiscHandler 
*                class
*
*/


#ifndef CPEPARSERMISCHANDLER_H
#define CPEPARSERMISCHANDLER_H

//INCLUDES
#include "cphonegsmmischandler.h"
#include <pevirtualengine.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class CPEMessageHandler;
class MPECallHandling;

// CLASS DECLARATION

/**
*  Provides processing for miscellaneous Phone Parser requests
*
*  @lib phoneenginegsm.lib
*  @since Series 60 4.0
*/
NONSHARABLE_CLASS( CPEParserMiscHandler )
    : public CPhoneGsmMiscHandler
{
    public:  //Constructors and descructor    
        /**
        * C++ default constructor.
        */
        CPEParserMiscHandler( CPEMessageHandler& aOwner, MPECallHandling& aCallHandling );

        /**
        * Destructor.
        */
        ~CPEParserMiscHandler();

    public: // Functions from base classes

        /**
        * Process USSD request
        * @param aString, USSD string to be sent
        */
        void ProcessUssdL( const TDesC& aString );

    private:
        // Handle to object which owns this requester
        CPEMessageHandler& iOwner;
        // Handle to the protocol specific Call Handling subsystem
        MPECallHandling& iCallHandling;
    };

#endif // CPEPARSERMISCHANDLER_H         
   
// End of File

