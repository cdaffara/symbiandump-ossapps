/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the
*                CPEParserManufacturerHandler object
*
*/


#ifndef CPEPARSERMANUFACTURERHANDLER_H
#define CPEPARSERMANUFACTURERHANDLER_H

//INCLUDES
#include <cphonegsmmanufacturerhandler.h>
#include <pevirtualengine.h>

// CONSTANTS
enum
    {
    EPhoneCmdDebugPrint,
    EPhoneCmdHeapFailure
    };

// MACROS
// None.

// DATA TYPES
// None.

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS
class MPEPhoneModelInternal;
class MPECallHandling;

// CLASS DECLARATION

/**
*  Provides processing for manufacturer specific Phone Parser requests
*
*  @lib phoneengine.lib
*  @since Series60_4.0
*/
NONSHARABLE_CLASS( CPEParserManufacturerHandler ) : public CPhoneGsmManufacturerHandler
    {
    public:  //Constructors and descructor    
        /**
        * C++ default constructor.
        */
        CPEParserManufacturerHandler( MPEPhoneModelInternal& aModel, 
                                      MPECallHandling& aCallHandling,
                                      RFs& aFsSession );

        /**
        * Destructor.
        */
        virtual ~CPEParserManufacturerHandler();

    public: // Functions from base classes

        /**
        * Process manufacturer specific command.
        * @param aCommand command to be performed.
        */
        void ProcessCommandL( TUint aCommand );

        /**
        * Process debug code.
        * @param aCode code to be processed.
        */
        void ProcessDebugL( const TDesC& aCode );
    
    private:    
        //MPEPhoneModelInternal is used to send message to the phone application
        MPEPhoneModelInternal& iModel;
        // CPEPhoneData is used to create request to the CallHandling subsystem.
        MPECallHandling& iCallHandling;
        // Handle to a file server session
        RFs& iFsSession; 
        
    };

#endif // CPEPARSERMANUFACTURERHANDLER_H

// End of File
