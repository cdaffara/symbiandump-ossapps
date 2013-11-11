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
 *       The purpose of this class is to observe if there are messages 
 *       in the outbox.
 *
 */

#ifndef TESTOUTBOXOBSERVER_STUB_H_
#define TESTOUTBOXOBSERVER_STUB_H_

//  INCLUDES
#include <e32base.h>
#include <msvapi.h>         // for MMsvSessionObserver
#include "msignalstrengthobserver.h"

// FORWARD DECLARATIONS

class CMsvSession;
class COutboxSender;
class MSignalStrengthHandler;

// CLASS DECLARATION

/**
 *  Observes if there are messages in the OutBox..
 */
class CTestOutboxObserver: public MSignalStrengthObserver
{
public:
    // Constructors and destructor

    /**
     *   A Two-phased constructor.
     */
    static CTestOutboxObserver* NewL();

    /**
     *   Destructor.
     */
    virtual ~CTestOutboxObserver();

private:
    // From MSignalStrengthObserver

    void SignalStrengthAndBarUpdatedL(TInt aNewSignalValue, TInt aNewBarValue);

public:

    void HandleMsvSessionReadyL(CMsvSession& aMsvSession);
    void HandleMsvSessionClosedL();

public:
    // New functions

    /**
     *  Informs the networkstatus to Outbox Sender
     */
    void InformOutboxSenderL(const TInt& aNetworkBars);

private:

    /**
     *   A C++ constructor.
     */
    CTestOutboxObserver();

    /**
     *   By default Symbian OS constructor is private.
     */
    void ConstructL();

    /**
     * Creates the Msv Session and sets the outbox 
     * observer and outbox sender.
     * @param aMsvSession The Msv session to use.
     */
    void StartSessionsL(CMsvSession& aMsvSession);

    /**
     *   Delete session, outbox observer and outbox sender.
     */
    void EndSessions();

private:
    // In-box folder entry. Note that the entry is not owned by this class.
    CMsvEntry* iOutboxFolder;
    // Pointer to Outbox Sender.
    COutboxSender* iOutboxSender;
    // Signal strength handler. Owned.
    MSignalStrengthHandler* iSignalStrengthHandler;
};

#endif /* TESTOUTBOXOBSERVER_STUB_H_ */
