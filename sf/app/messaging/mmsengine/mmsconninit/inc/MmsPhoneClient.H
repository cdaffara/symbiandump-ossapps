/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*     This class connects telephony server and requests information from it.
*
*/



#ifndef CMMSPHONECLIENT_H
#define CMMSPHONECLIENT_H

// INCLUDES
#include <etelmm.h> // RMobilePhone

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION
class RTelServer;
class RMobilePhone;

/**
* Class for asking information from telephony server.
* @lib mmsconninit.dll
* @since 3.0
*/
class CMmsPhoneClient : public CActive
    {        
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CMmsPhoneClient* NewL();

        /**
        * Destructor.
        */
        virtual ~CMmsPhoneClient();

    public: // New functions

        /**
        * Roaming determines whether phone is roaming currently or not
        * @param aStatus the iStatus of the caller. On completion, the caller
        * should interpret aStatus as follows:
        *   <  0 indicates an error
        *   == 0 indicates that phone is not roaming
        *   >  0 indicates that phone is roaming
        */
        IMPORT_C void Roaming( TRequestStatus& aStatus );
        
        /**
        * NetworkMode determines in what kind of network the phone currently is.
        * @return TMmsNetworkMode
        */
        IMPORT_C RMobilePhone::TMobilePhoneNetworkMode NetworkMode();
        
    public: // Functions from base classes

    protected:  // New functions

    protected:  // Functions from base classes

        /**
        * Active object cancellation
        */
        void DoCancel();

    private:

        /**
        * C++ default constructor.
        */
        CMmsPhoneClient();

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL();

        /**
        * Completion
        */
        void RunL();

    private:    // Data

        TRequestStatus*     iClientStatus;
        
        RTelServer          iTelServer;
        RMobilePhone        iMobilePhone;
        RMobilePhone::TMobilePhoneRegistrationStatus iRegistrationStatus;
        TInt                iRequestId;
    };

#endif      // CMMSPHONECLIENT_H

// End of File
