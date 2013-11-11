/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  handler for saving and deleting flash messages
 *
 */

#ifndef FLASH_MSG_NOTIFIER_H
#define FLASH_MSG_NOTIFIER_H

// INCLUDE FILES
#include <msvapi.h>

// FORWARD DECLARATIONS
class CMsvSession;
class MMsvSessionObserver;
class FlashMsgNotifier;
class CCSRequestHandler;

/**
 * @class MsgStoreHandler
 * handler for saving and deleting flash messages
 */
class FlashMsgNotifierPrivate: public CBase, public MMsvSessionObserver
{
public:

    /**
     * Constructor.
     */
    explicit FlashMsgNotifierPrivate(FlashMsgNotifier* notifier);

    /**
     * Destructor
     */
    virtual ~FlashMsgNotifierPrivate();

    /**
     * DeleteMessage
     * @param aMsgId TMsvId
     */
    void DeleteMessage(TMsvId aMsgId);

    /**
     * SaveMessageL
     * @param aMsgId TMsvId
     */
    void SaveMessageL(TMsvId aMsgId);
    
    /**
     * GetConversationIdFromAddressL
     * @param aContactAddress 
     * @return conversationId
     */
    TInt GetConversationIdFromAddressL(TDesC& aContactAddress);

public:
    
    /**
     * From MMsvSessionObserver. HandleSessionEventL.
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

private:
    /**
     * Initializes the MsgStoreHandler class.     
     */
    void InitL();

private:
    /**
     * Owned. Msv Session
     */
    CMsvSession* iMsvSession;

    /**
     * Conversation server client 
     * Owned
     */
    CCSRequestHandler* iRequestHandler;
    
    /**
     * Not Owned. Message notifier.
     */
    FlashMsgNotifier* iNotifier;

};

#endif // FLASH_MSG_NOTIFIER_H
// EOF
