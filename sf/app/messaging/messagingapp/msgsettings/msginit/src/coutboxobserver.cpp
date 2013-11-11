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
 * Description:   Implements the class
 *
 */

// INCLUDE FILES
#include <e32def.h>
#include <msvids.h>              // Entry Ids
#include <e32property.h>
#include <PSVariables.h>
#include "coutboxsender.h"
#include "coutboxobserver.h"
#include "cmobilesignalstrengthhandler.h"
#include "debugtraces.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// COutboxObserver::COutboxObserver
// ---------------------------------------------------------
//
COutboxObserver::COutboxObserver(){}

// ---------------------------------------------------------
// COutboxObserver::ConstructL
// ---------------------------------------------------------
//
void COutboxObserver::ConstructL()
{
    QDEBUG_WRITE( "COutboxObserver::ConstructL Enter" );
    // instantiate the signal strength handler
    iSignalStrengthHandler = CMobileSignalStrengthHandler::NewL();

    if (iSignalStrengthHandler->ObservingSupported()) 
    {
        iSignalStrengthHandler->SetSignalStrengthObserverL(this);
        iSignalStrengthHandler->StartObservingL();
    }
    else 
    {
        QDEBUG_WRITE( "COutboxObserver::ConstructL else part" );
    }
    QDEBUG_WRITE( "COutboxObserver::ConstructL Exit" );
}

// ---------------------------------------------------------
// COutboxObserver::NewL
// ---------------------------------------------------------
//
COutboxObserver* COutboxObserver::NewL()
{
    // Create the instance of the outbox observer
    COutboxObserver* self = new (ELeave) COutboxObserver();
    // Push it to stack while executing the constructor
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// ---------------------------------------------------------
// COutboxObserver::~COutboxObserver
// ---------------------------------------------------------
//
COutboxObserver::~COutboxObserver()
{
    delete iSignalStrengthHandler;
    EndSessions();
}

// ---------------------------------------------------------
// COutboxObserver::SignalStrengthUpdatedL
// ---------------------------------------------------------
//
void COutboxObserver::SignalStrengthAndBarUpdatedL(TInt /*aNewSignalValue*/, TInt aNewBarValue)
{
    QDEBUG_WRITE_FORMAT( "COutboxObserver::SignalStrengthUpdatedL - Signal bars ", aNewBarValue );
    // pass new bar value to outbox sender
    InformOutboxSenderL(aNewBarValue);
}

// ---------------------------------------------------------
// COutboxObserver::InformOutboxSenderL
// ---------------------------------------------------------
//
void COutboxObserver::InformOutboxSenderL(const TInt& aNetworkBars)
{
    if (iOutboxSender) {
        QDEBUG_WRITE( "COutboxObserver::InformOutboxSenderL - Informing outbox sender." );

        iOutboxSender->CheckAndStartSendingL(aNetworkBars);
    }
    else {
        QDEBUG_WRITE( "COutboxObserver::InformOutboxSenderL - Outbox sender not initialized." );
    }
}

// ---------------------------------------------------------
// COutboxObserver::StartSessionsL
// ---------------------------------------------------------
//
void COutboxObserver::StartSessionsL(CMsvSession& aMsvSession)
{
    // Set outbox folder
    iOutboxFolder = aMsvSession.GetEntryL(KMsvGlobalOutBoxIndexEntryId);
    iOutboxFolder->AddObserverL(*this);

    // Remove the Sender, if it exists
    delete iOutboxSender;
    iOutboxSender = NULL;

    QDEBUG_WRITE("COutboxObserver::StartSessionsL - Initializing outbox sender.." );
    iOutboxSender = COutboxSender::NewL(aMsvSession);

    QDEBUG_WRITE( "COutboxObserver::StartSessionsL - Informing outbox sender of network status.." );

    // get current signal bar value
    TInt signalBars = 0;
    TRAPD( err, signalBars = iSignalStrengthHandler->BarValueL() );

    // check error
    if (err == KErrNone) 
    {
        QDEBUG_WRITE_FORMAT("COutboxObserver::StartSessionsL - Got signal bar value notifying..", signalBars );
        iOutboxSender->CheckAndStartSendingL(signalBars);
    }
    else 
    {
        QDEBUG_WRITE_FORMAT( "COutboxObserver::StartSessionsL - Failed to get signal bar value (err = )", err );
    }

}

// ---------------------------------------------------------
// COutboxObserver::EndSessions
// ---------------------------------------------------------
//
void COutboxObserver::EndSessions()
{
    // Delete Outbox sender
    delete iOutboxSender;
    iOutboxSender = NULL;

    if (iOutboxFolder) {
        // Delete outbox folder
        iOutboxFolder->RemoveObserver(*this);
        delete iOutboxFolder;
        iOutboxFolder = NULL;
    }
}

// ---------------------------------------------------------
// COutboxObserver::HandleMsvSessionReadyL
// ---------------------------------------------------------
//    
void COutboxObserver::HandleMsvSessionReadyL(CMsvSession& aMsvSession)
{
    StartSessionsL(aMsvSession);
}
// ---------------------------------------------------------
// COutboxObserver::HandleMsvSessionClosedL
// ---------------------------------------------------------
//    
void COutboxObserver::HandleMsvSessionClosedL()
{
    EndSessions();
}

// ---------------------------------------------------------
// COutboxObserver::HandleEntryEventL
// ---------------------------------------------------------
//
void COutboxObserver::HandleEntryEventL(TMsvEntryEvent /*aEvent*/, TAny* /*aArg1*/,
    TAny* /*aArg2*/, TAny* /*aArg3*/)
{

}

//  End of File
