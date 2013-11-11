/*
 * Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Implements the class CSPAudioHandler
 *
 */

#include "cspaudiohandler.h"
#include "tmshandler.h"
#include "csptimer.h"
#include "csplogger.h"
#include "mcspaudiohandlerobserver.h"

// ---------------------------------------------------------------------------
// CSPAudioHandler::NewL.
// ---------------------------------------------------------------------------
//
CSPAudioHandler* CSPAudioHandler::NewL()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPAudioHandler::NewL()");
    CSPAudioHandler* self = new (ELeave) CSPAudioHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructs the object by canceling first ongoing monitoring.
// ---------------------------------------------------------------------------
//
CSPAudioHandler::~CSPAudioHandler()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPAudioHandler::~CSPAudioHandler()");
    if (iTimer)
        {
        iTimer->CancelNotify();
        delete iTimer;
        }
    delete iTmsHandler;
    }

// ---------------------------------------------------------------------------
// CSPAudioHandler::SetObserver
// ---------------------------------------------------------------------------
//
void CSPAudioHandler::SetObserver(MCSPAudioHandlerObserver& aObserver)
    {
    CSPLOGSTRING(CSPOBJECT, "CSPAudioHandler::SetObserver()");
    iObserver = &aObserver;
    }

// ---------------------------------------------------------------------------
// CSPAudioHandler::Start
// Note: Client must pass the observer through SetObserver() prior to calling
// Start(), otherwise MTmsHandlerObserver callbacks will be missed.
// ---------------------------------------------------------------------------
//
void CSPAudioHandler::Start()
    {
    iCallCount++;
    CSPLOGSTRING2(CSPINT, "CSPAudioHandler::Start callcount: %d", iCallCount);
    if (iTmsHandler && iCallCount == 1)
        {
        TInt err = iTmsHandler->StartStreams();
        if (err != KErrNone)
            {
            AudioStreamsError(err);
            }
        }
    }

// ---------------------------------------------------------------------------
// CSPAudioHandler::Stop
// ---------------------------------------------------------------------------
//
void CSPAudioHandler::Stop()
    {
    CSPLOGSTRING2(CSPINT, "CSPAudioHandler::Stop callcount: %d", iCallCount);
    if (iTimer)
        {
        iTimer->CancelNotify();
        }
    if (iTmsHandler && iCallCount == 1)
        {
        iTmsHandler->StopStreams();
        iCallCount--;
        }
    else if (iCallCount > 1)
        {
        iCallCount--;
        }
    }

// ---------------------------------------------------------------------------
// CSPAudioHandler::ReportAudioFailureAfterTimeout
// ---------------------------------------------------------------------------
//
void CSPAudioHandler::ReportAudioFailureAfterTimeout(TInt aTimeout)
    {
    CSPLOGSTRING(CSPOBJECT, "CSPAudioHandler::ReportAudioFailureAfterTimeout()");

    if (iTmsHandler)
        {
        // Make sure audio streams are not already active, so we don't
        // accidentaly hangup the call by setting the timer.
        if (iTimer && !iTmsHandler->AreStreamsStarted())
            {
            if (iTimer->IsNotifyOngoing())
                {
                iTimer->CancelNotify();
                }
            iTimer->NotifyAfter(aTimeout, *this);
            }
        }
    }

// ---------------------------------------------------------------------------
// CSPAudioHandler::AudioStreamsStarted
// From MTmsHandlerObserver
// ---------------------------------------------------------------------------
//
void CSPAudioHandler::AudioStreamsStarted()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPAudioHandler::AudioStreamsStarted()");
    if (iTimer)
        {
        iTimer->CancelNotify();
        }
    }

// ---------------------------------------------------------------------------
// CSPAudioHandler::AudioStreamsError
// From MTmsHandlerObserver
// ---------------------------------------------------------------------------
//
void CSPAudioHandler::AudioStreamsError(TInt /*aError*/)
    {
    CSPLOGSTRING(CSPOBJECT, "CSPAudioHandler::AudioStreamsError()");
    if (iTimer)
        {
        iTimer->CancelNotify();
        }

    // Note: The observer must be provided in SetObserver() prior to Start()
    // request, otherwise error conditions will not be propagated to the client.
    if (iObserver)
        {
        iObserver->AudioStartingFailed();
        }
    }

// ---------------------------------------------------------------------------
// CSPAudioHandler::TimerEvent
// From MCSPTimerObserver
// ---------------------------------------------------------------------------
//
void CSPAudioHandler::TimerEvent()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPAudioHandler::TimerEvent()");
    if (iObserver)
        {
        iObserver->AudioStartingFailed();
        }
    if (iTmsHandler)
        {
        iTmsHandler->StopStreams();
        }
    }

// ---------------------------------------------------------------------------
// Constructs the monitor.
// ---------------------------------------------------------------------------
//
CSPAudioHandler::CSPAudioHandler()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPAudioHandler::CSPAudioHandler()");
    iCallCount = 0; // Active calls count
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CSPAudioHandler::ConstructL()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPAudioHandler::ConstructL()");
    iTmsHandler = TmsHandler::NewL(*this);
    iTimer = CSPTimer::NewL();
    }

// End of file
