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

#ifndef OUTBOXOBSERVER_H
#define OUTBOXOBSERVER_H

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
class COutboxObserver: public CBase, public MMsvEntryObserver, public MSignalStrengthObserver
{
public:
    // Constructors and destructor

    /**
     *   A Two-phased constructor.
     */
    static COutboxObserver* NewL();

    /**
     *   Destructor.
     */
    virtual ~COutboxObserver();

private:
    // From MSignalStrengthObserver

    void SignalStrengthAndBarUpdatedL(TInt aNewSignalValue, TInt aNewBarValue);

public:

    void HandleMsvSessionReadyL(CMsvSession& aMsvSession);
    void HandleMsvSessionClosedL();

public:

    /**
     *   The messaging server sends notifications of changes in the
     *   observed entry to COutboxObserver by calling this method.
     */
    void
        HandleEntryEventL(TMsvEntryEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/);

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
    COutboxObserver();

    /**
     *   By default Symbian OS constructor is private.
     */
    void ConstructL();

    /**
     *   By default, prohibit copy constructor
     */
    COutboxObserver(const COutboxObserver&);

    /**
     *   Prohibit assigment operator
     */
    COutboxObserver& operator=(const COutboxObserver&);

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

#endif      // OUTBOXOBSERVER_H
// End of File
