/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This is the client side server interface. This class handles
*               asynchronous notifications (e.g. new messages) from server
*               to client.
*/

#ifndef __C_CS_NOTIFICATION_HANDLER_H__
#define __C_CS_NOTIFICATION_HANDLER_H__

#include <e32base.h>
#include <rcssession.h>

// FORWARD DECLARATIONS
class CCSRequestHandler;
class CCsClientConversation;

// CLASS DECLARATION
/**
 * CCsNotificationHandler
 * This class handles asynchronous notifications (e.g. new messages) 
 * from server to client.
 */
class CCsNotificationHandler : public CActive
    {
public:    
    static CCsNotificationHandler* NewL (CCSRequestHandler* aRequestHandler);
    virtual ~CCsNotificationHandler ();
public: // CActive
    void RunL();
    void DoCancel();
public:
    void RequestConversationListChangeEventL();
    void RemoveConversationListChangeEventL();
    void RequestConversationChangeEventL(CCsClientConversation* aConversation);
    void RemoveConversationChangeEventL(CCsClientConversation* aConversation);
private:
    CCsNotificationHandler();
    void ConstructL(CCSRequestHandler* aRequestHandler);
    void RequestChangeEventL();
    void RemoveChangeEventL();
private:    
    CCSRequestHandler* iRequestHandler; // Not owned
    RCsSession iSession;
    TInt iLastReqID;
    HBufC8* iNextReqIDBuffer;
    HBufC8* iNotificationBuffer;
    HBufC8* iRequestBuffer;
    };

#endif // __C_CS_NOTIFICATION_HANDLER_H__
