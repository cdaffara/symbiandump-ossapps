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
* Description:  Sends UUI message.
*
*/


#ifndef CSPUUIMESSAGESENDER_H
#define CSPUUIMESSAGESENDER_H

#include <e32base.h>
#include <etelmm.h>

/**
 *  Sends user to user information messages.
 *
 *  @lib PhoneEngine.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CSPUUIMessageSender ) : public CActive
    {

public:

    static CSPUUIMessageSender* NewL(
        RMobileCall& aCall );

    virtual ~CSPUUIMessageSender();

    /**
     * Sends user to user information.
     *
     * @since S60 v3.2
     * @param aMessage UUI message to be sent.
     */
    void SendUUIMessage( const TDesC& aMessage );
     
// from base class CActive

protected:

    /**
     * From CActive.
     *
     * Cancels active request.
     * @since S60 v3.2
     */
    void DoCancel();
    
    /**
     * From CActive.
     *
     * Callback to indicate completion
     * of request.
     * @since S60 v3.2
     */
    void RunL();

private:

    CSPUUIMessageSender(
        RMobileCall& aCall );

    void ConstructL();

private: // data
    
    /**
     * Call, which the UUI is sent to.
     * Not own.
     */
    RMobileCall& iCall;
    
    /**
     * Request.
     */
    RMobileCall::TMobileCallUUSRequestV1 iUUSRequest;
    
    /**
     * Request package.
     */
    RMobileCall::TMobileCallUUSRequestV1Pckg iUUSRequestPckg;
    };

#endif // CSPUUIMESSAGESENDER_H
