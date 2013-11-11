/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 *       This class follows the offline status of the phone. When the mode is
 *       changed from online to offline, the sending operation is started. When
 *       the mode is changed from online to offline, the current sending
 *       operation is cancelled.
 *
 */

#ifndef __OUTBOXSENDER_H__
#define __OUTBOXSENDER_H__

//  INCLUDES
#include <msvapi.h>
#include <MuiuMsvSingleOpWatcher.h>
#include <cenrepnotifyhandler.h>

// FORWARD DECLARATIONS
class CRepository;

// CLASS DECLARATION
/**
 * COutboxSender
 * Sends sms messages from the outbox when offline is changed back to online.
 * It only tries to send message once. If sending fails due to other reasons
 * than offline mode, then it is up to the user to go to outbox and send it.
 * Note: At the moment only sms'es are sent. MMS messages are sent by mms mtm
 * and emails are saved to outbox as 'During next connection' and those must
 * not be sent.
 */
class COutboxSender: public CBase, public MMsvSingleOpWatcher, public MCenRepNotifyHandlerCallback
{
public:
    // Constructors and destructor

    /**
     *   A two-phased constructor.
     */
    static COutboxSender* NewL(CMsvSession& aMsvSession);

    /**
     *   A destructor.
     */
    virtual ~COutboxSender();

public:
    // New functions

    /**
     *   Starts the message sending operation for sms-messages.
     */
    void StartSendingL();

    /**
     *   Cancels current message sending operation.
     */
    void CancelSending();

    /**
     *   Checks if the sending operation is currently active.
     */
    TBool IsSending() const;

    /**
     *  Confirms the network status and starts sending messages
     */
    void CheckAndStartSendingL(const TInt& aNetworkBars);

public:
    // Functions from base classes

    /**
     *   From MMsvSingleOpWatcher. Deletes the sending operation.
     */
    virtual void OpCompleted(CMsvSingleOpWatcher& aOpWatcher, TInt aCompletionCode);

    /**
     *   From MCentRepNotifyHandlerCallback.
     */
    void HandleNotifyInt(TUint32 aId, TInt aNewValue);

    /**
     *   From MCentRepNotifyHandlerCallback.
     */
    void HandleNotifyError(TUint32 aId, TInt aError, CCenRepNotifyHandler* aHandler);

    /**
     *   From MCentRepNotifyHandlerCallback.
     */
    void HandleNotifyGeneric(TUint32 aId);

private:
    /**
     *   A C++ Constructor
     */
    COutboxSender(CMsvSession& aMsvSession);

    /**
     *   Creates the connection to shared data and
     *   begins listening the KGSNetworkConnectionAllowed-key.
     */
    void ConstructL();

    /**
     * Checks whether SMS sending is needed in boot-phase and
     * launches the sending operation
     */
    void CheckBootPhaseL();

private:
    enum TCleanupFlags
    {
        EUserSettingsConnected = 0x01,
        EUserSettingsNotifierSet = 0x02,
        EOffllineSendingNeeded = 0x10
    };

private:
    // Reference to Msv Session
    CMsvSession& iMsvSession;
    // Pointer to sending operation
    CMsvSingleOpWatcher* iRunningOperation;
    CCenRepNotifyHandler* iNotifyHandler;
    CRepository* iSession;
    // Flags to indicate status of Outbox Sender
    TInt8 iFlags;
    // Last known network coverage
    TInt iNetworkBars;

};

#endif // __OUTBOXSENDER_H__
// End of file
