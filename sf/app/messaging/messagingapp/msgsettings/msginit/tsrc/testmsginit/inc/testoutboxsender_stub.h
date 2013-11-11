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


#ifndef TESTOUTBOXSENDER_STUB_H_
#define TESTOUTBOXSENDER_STUB_H_

//  INCLUDES
#include <msvapi.h>
#include <MuiuMsvSingleOpWatcher.h>
#include <cenrepnotifyhandler.h>

// FORWARD DECLARATIONS
class CRepository;

// CLASS DECLARATION

class CTestOutboxSender: public MMsvSingleOpWatcher
{
public:
    // Constructors and destructor

    /**
     *   A two-phased constructor.
     */
    static CTestOutboxSender* NewL(CMsvSession& aMsvSession);

    /**
     *   A destructor.
     */
    virtual ~CTestOutboxSender();

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

private:
    /**
     *   A C++ Constructor
     */
    CTestOutboxSender(CMsvSession& aMsvSession);

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
};

#endif /* TESTOUTBOXSENDER_STUB_H_ */
