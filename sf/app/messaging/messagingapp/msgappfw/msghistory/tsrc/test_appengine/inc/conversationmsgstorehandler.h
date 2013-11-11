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
* Description:  Wrapper classes which encapsulates CCsClientConversation,
 * Avatar and refresh status.
 *
*/

#ifndef CONVERSATION_MSG_STORE_HANDLER_H
#define CONVERSATION_MSG_STORE_HANDLER_H

// INCLUDE FILES
#include <msvapi.h>
#include <ccsdefs.h>

// FORWARD DECLARATIONS
class CMsvSession;
class MMsvSessionObserver;

// CONSTANTS

class ConversationMsgStoreHandler: public MMsvSessionObserver
    {
public:

    /**
     * Default constructor.
     */
    explicit ConversationMsgStoreHandler();

    /**
     * Destructor
     */
    virtual ~ConversationMsgStoreHandler();

private:
    
    /**
     *      
     */
    void InitL();

public:
    /**
     * From MMsvSessionObserver. HandleSessionEventL.
     */
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
            TAny* aArg3);
public: 
    /**
     * Get MMS notification status.
     * @param aMsvId MMS notification status.
     */
    TCsMmsNotificationMsgState MmsNotificationStatus( TInt aMsvId );
    
    /**
     * Mark specified messages as unread.
     * @param aIdArray Array of message ids.
     */
    void MarkMessagesReadL(RArray<TInt>& aIdArray);

    /**
     * Delete specified messages from message store
     * @param aIdArray Array of message ids.
     */
    void DeleteMessages(RArray<TInt>& aIdArray);
    
    /**
     * Returns the msv session object
     * @return CMsvSession object
     */
    CMsvSession& GetMsvSession();      
    
private:

    /**
     * verifys the current operation status
     *@param aEntry, TMsvEntry
     */
    TBool OperationFinished( const TMsvEntry& aEntry ) const;

    /**
     * verifys the current operation status
     * @param aEntry, TMsvEntry
     */
    TBool OperationOngoing( const TMsvEntry& aEntry ) const;

private: 
    /**
     * Own. Msv Session
     */
    CMsvSession* iMsvSession;     
    };

#endif // CONVERSATION_MSG_STORE_HANDLER_H

// EOF
