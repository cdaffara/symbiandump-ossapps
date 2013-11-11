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
* Description:  Implementation of ECOM plug-in service interface. Provides
*                HtiMessagesServicePlugin service.
*
*/


#ifndef CHTIMESSAGESSERVICEPLUGIN_H
#define CHTIMESSAGESSERVICEPLUGIN_H

// INCLUDES
#include <HtiServicePluginInterface.h>
#include "../../../symbian_version.hrh"

// CONSTANTS
const TInt KHtiMessagesServiceUidValue = 0x10210CCF; // ECOM Implementation UID
const TUid KHtiMessagesServiceUid = {KHtiMessagesServiceUidValue};

// FORWARD DECLARATIONS
class CMessageMgmntHandler;
class CHtiIAPHandler;
class CHtiMailboxHandler;
class CHtiMsgSettingsHandler;
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
class CHtiNotificationHandler;
#endif

// CLASS DECLARATION

/**
* Implementation of ECOM plug-in service interface.
*/
class CHtiMessagesServicePlugin : public CHTIServicePluginInterface
    {
public:

    enum TCommand
        {
        // MessagesMgmntHandler
        EAddSms              = 0x01,
        EAddMms              = 0x02,
        EAddEmail            = 0x03,
        EAddIrMsg            = 0x04,
        EAddBtMsg            = 0x05,
        EAddSmartMsg         = 0x06,
        EAddAudioMsg         = 0x07,
        EDeleteMessage       = 0x10,
        EDeleteFolderContent = 0x11,

        // MailboxHandler
        ECreateMailBox       = 0x20,
        EDeleteMailBox       = 0x21,

        // IAPHandler
        ECreateIAP           = 0x30,
        EDeleteIAP           = 0x31,
        ECreateDestination   = 0x32,
        EDeleteDestination   = 0x33,
        EAddToDestination    = 0x34,
        ERemoveFromDestination = 0x35,
        ESetDefaultConnection = 0x36,

        // MgsSettingsHandler
        ESetDefaultSmsCenter = 0x40,
        EDeleteSmsCenter     = 0x41,
        ESetSmsSettings      = 0x42,
        ESetMmsSettings      = 0x45,

#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
        // NotificationHandler
        ECreateVoiceMessageNotification = 0x50,
        EClearAllNotifications          = 0x51,
#endif
        
        // only for response message
        EResultOk            = 0xFF,
        };

    static CHtiMessagesServicePlugin* NewL();

    // Interface implementation
    void ProcessMessageL( const TDesC8& aMessage,
        THtiMessagePriority aPriority );

protected:

    CHtiMessagesServicePlugin();
    void ConstructL();

    virtual ~CHtiMessagesServicePlugin();

private:

    CMessageMgmntHandler*   iMessageHandler;
    CHtiIAPHandler*         iIAPHandler;
    CHtiMailboxHandler*     iMailboxHandler;
    CHtiMsgSettingsHandler* iMsgSettingsHandler;
#if ( SYMBIAN_VERSION_SUPPORT < SYMBIAN_4 )
    CHtiNotificationHandler* iNotificationHandler;
#endif
    };

#endif // CHTIMESSAGESSERVICEPLUGIN_H

