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
* Description:  HTI Mailbox settings handler.
*
*/


#ifndef HTIMAILBOXHANDLER_H_
#define HTIMAILBOXHANDLER_H_

// INCLUDES
#include <cmmanager.h>
#include <msvapi.h>
#include <HtiServicePluginInterface.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class TSmtpAccount;

// CLASS DECLARATION
/**
*  HTI Mailbox settings handler.
*/
class CHtiMailboxHandler : public CBase,
                           public MMsvSessionObserver
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CHtiMailboxHandler* NewL();

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
        virtual ~CHtiMailboxHandler();

        /**
        * Sets the dispatcher to send outgoing messages to.
        * @param aDispatcher pointer to dispatcher instance
        */
        void SetDispatcher( MHtiDispatcher* aDispatcher );

    private:

        /**
        * C++ default constructor.
        */
        CHtiMailboxHandler();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // helpers

        void HandleCreateMailboxL( const TDesC8& aData );
        void HandleDeleteMailboxL( const TDesC8& aData );
        void ParseCreateMsgL( const TDesC8& aData );
        HBufC*  ParseStringL( const TDesC8& aData, TInt aStrLenOffset,
                                TInt aStrLenBytes = 1 );
        HBufC8* ParseString8L( const TDesC8& aData, TInt aStrLenOffset,
                                TInt aStrLenBytes = 1 );
        TInt AccessPointUIDL( const TDesC& aApName );
        TInt FindSmtpAccount( const TDesC& aAccountName,
                              TSmtpAccount& aAccount );
        void GetDeviceImeiL( TDes& aResult );
        void ResetVariables();
        void SendOkMsgL( const TDesC8& aData );
        void SendErrorMessageL( TInt aError, const TDesC8& aDescription );

    private: // from MMsvSessionObserver

        void HandleSessionEventL( TMsvSessionEvent aEvent,
                                  TAny* aArg1, TAny* aArg2, TAny* aArg3 );

    private:  // Data

        // Pointer to the dispatcher (referenced)
        MHtiDispatcher* iDispatcher;

        // Handle to connection manager
        RCmManager iCmManager;

        // Email account settings
        TInt    iMailboxType;
        HBufC*  iMailboxName;
        HBufC*  iIncomingServer;
        HBufC*  iIncomingApName;
        HBufC8* iIncomingUserName;
        HBufC8* iIncomingPassword;
        TInt    iIncomingSecurity;
        TInt    iIncomingPort;
        TBool   iAPopSecureLogin;
        HBufC*  iOutgoingServer;
        HBufC*  iOutgoingApName;
        HBufC8* iOutgoingUserName;
        HBufC8* iOutgoingPassword;
        TInt    iOutgoingSecurity;
        TInt    iOutgoingPort;
        HBufC*  iOwnMailAddress;
        HBufC*  iOwnName;
        TInt    iSendOption;           // (enum TImSMTPSendMessageOption)
        TInt    iSendCopyToOwnAddress; // (enum TImSMTPSendCopyToSelf)
        HBufC*  iSignatureText;
        TBool   iNewMailIndicators;
        TInt    iRetrievedParts;
        TInt    iRetrieveSizeLimitKb;
        TInt    iEmailsToRetrieve;
        HBufC8* iImapFolderPath;
        TInt    iAutomaticUpdate;
        TBool   iSetAsDefault;
    };

#endif // HTIMAILBOXHANDLER_H_

// End of file
