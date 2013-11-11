/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Header file of the CPhoneGsmPcnProcedureHandler class.
*
*/


#ifndef CPEPCNPARSERPROCEDUREHANDLER_H
#define CPEPCNPARSERPROCEDUREHANDLER_H

//  INCLUDES
#include    <cphonegsmpcnprocedurehandler.h>
#include    "pevirtualengine.h"

// CONSTANTS
//None

// MACROS
//None

// DATA TYPES
//None

// FUNCTION PROTOTYPES
//None

// FORWARD DECLARATIONS
//None

// CLASS DECLARATION
class CPEMessageHandler;
class CPESecurityCommand;
class CManualSecuritySettings;
class MPEPhoneModelInternal;

// DESCRIPTION
// It is handler for GsmPcn procedures.

NONSHARABLE_CLASS( CPEPcnParserProcedureHandler )
        :public CPhoneGsmPcnProcedureHandler
    {
    public:  //Constructors and descructor    

        /**
        * Two-phased constructor.
        */
        static CPEPcnParserProcedureHandler* NewL
            (
            CPEMessageHandler& aOwner,   // The owner of this object.  
            MPEPhoneModelInternal& aModel   // The reference parameter of the 
            );                              // phone model object which is used
                                            // to send message to the phone 
                                            // application.
        /**
        * Destructor.
        */
        virtual ~CPEPcnParserProcedureHandler();


    public: // New functions
        
        /**
        * Process SIM locking.
        *
        * Parameters are empty strings if SIM lock
        * code was just garbage.
        * 
        * @param aPassword sequence of digits.
        * @param aType sequence of digits.
        */
        void ProcessSimLockL( 
            const TDesC& aPassword, 
            const TDesC& aType );

        /**
        * Process SIM unlocking.
        *
        * Parameters are empty strings if SIM unlock
        * code was just garbage.
        * 
        * @param aPassword sequence of digits.
        * @param aType sequence of digits.
        */
        void ProcessSimUnlockL(
            const TDesC& aPassword,
            const TDesC& aType );

    private:

        /**
        * C++ default constructor.
        */
        void ConstructL();

        /**
        * C++ default constructor.
        */
        CPEPcnParserProcedureHandler
            ( 
            CPEMessageHandler& aOwner,  
            MPEPhoneModelInternal& aModel
            );


    private:    
        // CPEGsmMessageHandler object which owns this requester.
        CPEMessageHandler& iOwner;
        // Contains information of the Suplementary Services command.
        TPESSCommandInfo iSupplementaryServicesCommandInfo;
        //MPEPhoneModelInternal is used to send message to the phone application
        MPEPhoneModelInternal& iModel;
        // Security model
        CManualSecuritySettings* iSecurityModel;

    };

#endif      // CPEPCNPARSERPROCEDUREHANDLER_H
            
// End of File
