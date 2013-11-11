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
*                CPEParserSimControlHandler object
*
*/


#ifndef CPEPARSERSIMCONTROLHANDLER_H
#define CPEPARSERSIMCONTROLHANDLER_H

//INCLUDES
#include <cphonegsmsimcontrolhandler.h>
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
class MPEContactHandling;
class MPEPhoneModelInternal;
class CManualSecuritySettings;

// CLASS DECLARATION

/**
*  Provides processing for ICC/SIM related Phone Parser requests
*
*  @lib phoneenginegsm.lib
*  @since Series 60 4.0
*/
NONSHARABLE_CLASS( CPEParserSimControlHandler ) : public CPhoneGsmSimControlHandler
    {
    public:  //Constructors and descructor    

        /**
        * Two-phased constructor.
        */
        static CPEParserSimControlHandler* NewL
            (
            MPEPhoneModelInternal& aModel, 
            MPEContactHandling& aContactHandling );

        /**
        * Destructor.
        */
        virtual ~CPEParserSimControlHandler();

    public: // Functions from base classes

        /**
        * Change pin code
        *
        * @param aType It is type of pin.
        * @param aOldPin It is old pin.
        * @param aNewPin It is new pin.
        * @param aNewPin2 It is new pin.
        * @return None. 
        */
        void ProcessChangePinL
                ( 
                TPinCode aType, 
                const TDesC& aOldPin, 
                const TDesC& aNewPin, 
                const TDesC& aVerifyNew 
                );

        /**
        * Unblock pin code.
        *
        * @param aType It is type of pin to unblock.
        * @param aPuk It is unblocking code.
        * @param aNewPin It is new pin.
        * @param aNewPin2 It is new pin.
        * @return None. 
        */
        void ProcessUnblockPinL
                ( 
                TPinCode aType, 
                const TDesC& aPuk, 
                const TDesC& aNewPin, 
                const TDesC& aVerifyNew 
                );
    
    private:

        /**
        * C++ default constructor.
        */
        CPEParserSimControlHandler
                ( 
                MPEPhoneModelInternal& aModel, 
                MPEContactHandling& aContactHandling
                );

    private:
        //MPEPhoneModelInternal is used to send message to the phone application
        MPEPhoneModelInternal& iModel;
        // Handle to contact handling
        MPEContactHandling& iContactHandling;
        // Contains information of the Suplementary Services command.
        TPESSCommandInfo iSupplementaryServicesCommandInfo;
        // Security model
        CManualSecuritySettings* iSecurityModel;
    };

#endif // CPEPARSERSIMCONTROLHANDLER_H
            
// End of File
