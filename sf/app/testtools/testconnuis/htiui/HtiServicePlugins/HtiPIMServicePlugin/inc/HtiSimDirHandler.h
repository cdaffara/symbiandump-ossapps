/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class that handles SIM card contacts creation and deletion
*                using the new Virtual Phonebook API
*
*/


#ifndef HTISIMDIRHANDLER_H
#define HTISIMDIRHANDLER_H


// INCLUDES

#include <HtiServicePluginInterface.h>
#include <etelmm.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Class that handles SIM card contacts creation and deletion using the
*  new Virtual Phonebook API.
*/
class CHtiSimDirHandler : public CBase
    {
    public:

    enum TContactFieldType
        {
        ENameField        = 0x01,
        ESecondNameField  = 0x02,
        EPhoneNumberField = 0x03,
        EEMailField       = 0x04,
        EAdditNumberField = 0x05
        };

    public:

        /**
        * Two-phased constructor.
        */
        static CHtiSimDirHandler* NewL();

        /**
        * Called when there is a message to be processed by this service.
        * @param aMessage message body destinated to the servive
        * @param aPriority message priority
        */
        void ProcessMessageL( const TDesC8& aMessage,
            THtiMessagePriority aPriority );

        /**
        * Indicates whether this handler is ready to receive
        * a new message or if it's busy processing previous message.
        * @return ETrue if processing, EFalse if ready for new request
        */
        TBool IsBusy();

        /**
        * Destructor.
        */
        virtual ~CHtiSimDirHandler();

        /**
        * Sets the dispatcher to send outgoing messages to.
        * @param aDispatcher pointer to dispatcher instance
        */
        void SetDispatcher( MHtiDispatcher* aDispatcher );

    private: // constructors

        /**
        * C++ default constructor.
        */
        CHtiSimDirHandler();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // helpers

        void HandleSimCardInfoL(const TDesC8& aData);
        void HandleSimContactImportL(const TDesC8& aData);
        void HandleSimContactDeleteL(const TDesC8& aData);

        TBool CheckImportMsg(const TDesC8& aData);

        void SendOkMsgL( const TDesC8& aData );
        void SendErrorMessageL( TInt aError, const TDesC8& aDescription );

    private: // data

        // Pointer to the dispatcher (referenced)
        MHtiDispatcher* iDispatcher;

        // Flag indicating if service is busy processing a request
        TBool iIsBusy;
        
        RTelServer iEtelServer;
        RMobilePhone iEtelPhone;
        RMobilePhoneBookStore iEtelStore;

        TBool iStoreIsOpen;
    };

#endif // HTISIMDIRHANDLER_H
