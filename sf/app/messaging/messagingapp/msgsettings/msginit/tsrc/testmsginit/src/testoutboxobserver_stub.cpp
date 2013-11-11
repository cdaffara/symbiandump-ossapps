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
#include "testoutboxobserver_stub.h"
#include "cmobilesignalstrengthhandler.h"
#include "debugtraces.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTestOutboxObserver::CTestOutboxObserver
// ---------------------------------------------------------
//
CTestOutboxObserver::CTestOutboxObserver(){}

// ---------------------------------------------------------
// CTestOutboxObserver::ConstructL
// ---------------------------------------------------------
//
void CTestOutboxObserver::ConstructL()
{
    QDEBUG_WRITE( "CTestOutboxObserver::ConstructL Enter" );
    // instantiate the signal strength handler
    iSignalStrengthHandler = CMobileSignalStrengthHandler::NewL();

    if (iSignalStrengthHandler->ObservingSupported()) 
    {
        iSignalStrengthHandler->SetSignalStrengthObserverL(this);
        iSignalStrengthHandler->StartObservingL();
    }
    else 
    {
        QDEBUG_WRITE( "CTestOutboxObserver::ConstructL else part" );
    }
    QDEBUG_WRITE( "CTestOutboxObserver::ConstructL Exit" );
}

// ---------------------------------------------------------
// CTestOutboxObserver::NewL
// ---------------------------------------------------------
//
CTestOutboxObserver* CTestOutboxObserver::NewL()
{
    // Create the instance of the outbox observer
    CTestOutboxObserver* self = new (ELeave) CTestOutboxObserver();
    // Push it to stack while executing the constructor
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// ---------------------------------------------------------
// CTestOutboxObserver::~CTestOutboxObserver
// ---------------------------------------------------------
//
CTestOutboxObserver::~CTestOutboxObserver()
{
    delete iSignalStrengthHandler;
    EndSessions();
}

// ---------------------------------------------------------
// CTestOutboxObserver::SignalStrengthUpdatedL
// ---------------------------------------------------------
//
void CTestOutboxObserver::SignalStrengthAndBarUpdatedL(TInt /*aNewSignalValue*/, TInt aNewBarValue)
{
    QDEBUG_WRITE_FORMAT( "CTestOutboxObserver::SignalStrengthUpdatedL - Signal bars ", aNewBarValue );
    // pass new bar value to outbox sender
    InformOutboxSenderL(aNewBarValue);
}

// ---------------------------------------------------------
// CTestOutboxObserver::InformOutboxSenderL
// ---------------------------------------------------------
//
void CTestOutboxObserver::InformOutboxSenderL(const TInt& aNetworkBars)
{
   QDEBUG_WRITE( "CTestOutboxObserver::InformOutboxSenderL" );
}

// ---------------------------------------------------------
// CTestOutboxObserver::StartSessionsL
// ---------------------------------------------------------
//
void CTestOutboxObserver::StartSessionsL(CMsvSession& aMsvSession)
{    
    QDEBUG_WRITE( "CTestOutboxObserver::StartSessionsL" );
}

// ---------------------------------------------------------
// CTestOutboxObserver::EndSessions
// ---------------------------------------------------------
//
void CTestOutboxObserver::EndSessions()
{
	 QDEBUG_WRITE( "CTestOutboxObserver::EndSessions" );
}

// ---------------------------------------------------------
// CTestOutboxObserver::HandleMsvSessionReadyL
// ---------------------------------------------------------
//    
void CTestOutboxObserver::HandleMsvSessionReadyL(CMsvSession& aMsvSession)
{
    StartSessionsL(aMsvSession);
}
// ---------------------------------------------------------
// CTestOutboxObserver::HandleMsvSessionClosedL
// ---------------------------------------------------------
//    
void CTestOutboxObserver::HandleMsvSessionClosedL()
{
    EndSessions();
}

//  End of File


