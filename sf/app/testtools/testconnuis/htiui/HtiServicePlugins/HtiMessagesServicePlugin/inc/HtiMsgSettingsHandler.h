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
* Description:  HTI message settings handler.
*
*/


#ifndef CHTIMSGSETTINGSHANDLER_H
#define CHTIMSGSETTINGSHANDLER_H


// INCLUDES
#include <HtiServicePluginInterface.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION
/**
*  HTI Internet Access Point handler.
*/
class CHtiMsgSettingsHandler : public CBase
    {
    public:

        /**
        * Character support settings values.
        */
        enum TCharSetSupport
            {
            ECharSetFull,
            ECharSetReduced,
            ECharSetFullLocked,
            ECharSetReducedLocked
            };

        /**
        * Two-phased constructor.
        */
        static CHtiMsgSettingsHandler* NewL();

        /**
        * Called when there is a message to be processed by this service.
        * @param aMessage message body destinated to the servive
        * @param aPriority message priority
        */
        void ProcessMessageL( const TDesC8& aMessage,
                              THtiMessagePriority aPriority );

        /**
        * Destructor.
        */
        virtual ~CHtiMsgSettingsHandler();

        /**
        * Sets the dispatcher to send outgoing messages to.
        * @param aDispatcher pointer to dispatcher instance
        */
        void SetDispatcher( MHtiDispatcher* aDispatcher );

    private:

        /**
        * C++ default constructor.
        */
        CHtiMsgSettingsHandler();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // helpers

        void HandleSetDefaultSmsCenterL( const TDesC8& aData );
        void HandleDeleteSmsCenterL( const TDesC8& aData );
        void HandleSetSmsSettingsL( const TDesC8& aData );

        void HandleSetMmsSettingsL( const TDesC8& aData );

        TInt GetAccessPointUIDL( const TDesC& aApName );
        void SendOkMsgL( const TDesC8& aData );
        void SendErrorMessageL( TInt aError, const TDesC8& aDescription );

    private:  // Data

        // Pointer to the dispatcher (referenced)
        MHtiDispatcher* iDispatcher; // referenced
    };


#endif // CHTIMSGSETTINGSHANDLER_H
