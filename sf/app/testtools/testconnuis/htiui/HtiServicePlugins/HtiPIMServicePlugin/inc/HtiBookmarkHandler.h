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
* Description:  Class that handles browser bookmark creation and deletion.
*
*/


#ifndef HTIBOOKMARKHANDLER_H
#define HTIBOOKMARKHANDLER_H

// INCLUDES
#include <e32std.h>
#include <favouriteslimits.h>
#include <favouritessession.h>
#include <HtiServicePluginInterface.h>

// CONSTANTS
const TInt KApMaxConnNameLength = 30;

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Class that handles browser bookmark creation and deletion.
*/
class CHtiBookmarkHandler : public CBase
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CHtiBookmarkHandler* NewL();

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
        virtual ~CHtiBookmarkHandler();

        /**
        * Sets the dispatcher to send outgoing messages to.
        * @param aDispatcher pointer to dispatcher instance
        */
        void SetDispatcher( MHtiDispatcher* aDispatcher );

    private: // constructors

        /**
        * C++ default constructor.
        */
        CHtiBookmarkHandler();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // helpers

        void HandleCreateBookmarkL();
        void HandleDeleteBookmarkL();
        void ParseCreateMessageL( const TDesC8& aMessage );
        void ParseDeleteMessageL( const TDesC8& aMessage );
        TInt ExtractStringL( const TDesC8& aMessage, TInt aOffset,
                             TInt aLengthBytes, TInt aMinLength,
                             TInt aMaxLength, TDes& aResult );
        TInt GetFolderUidL( const TDesC& aFolderName );
        TInt GetApUidL( const TDesC& aApName );
        void SendOkMsgL( const TDesC8& aData );
        void SendErrorMessageL( TInt aError, const TDesC8& aDescription );

    private: // data

        // Pointer to the dispatcher (referenced)
        MHtiDispatcher* iDispatcher;

        // Session to the favourites engine
        RFavouritesSession iFavSession;

        // The parsed parameter values from the command messages
        TBuf<KFavouritesMaxName>     iFolderName;
        TBuf<KFavouritesMaxName>     iBookmarkName;
        TBuf<KApMaxConnNameLength>   iApName;
        TBuf<KFavouritesMaxUserName> iUserName;
        TBuf<KFavouritesMaxPassword> iPassword;
        HBufC*                       iUrl;
    };

#endif // HTIBOOKMARKHANDLER_H
