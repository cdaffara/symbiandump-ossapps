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
 * Description: Activates TMS call audio control streams.
 *
 */

#include <tmsfactory.h>
#include <tmscall.h>
#include <tmsstream.h>
#include <tmsver.h>
#include "tmshandler.h"
#include "csplogger.h"
#include "csppanic.pan"
#include "mtmshandlerobserver.h"

const TInt KStreamInitRetry = 1000000; //1 sec
const TInt KRetryForever = -1;

// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
TmsHandler* TmsHandler::NewL(MTmsHandlerObserver& aObserver)
    {
    TmsHandler* self = TmsHandler::NewLC(aObserver);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
TmsHandler* TmsHandler::NewLC(MTmsHandlerObserver& aObserver)
    {
    TmsHandler* self = new (ELeave) TmsHandler();
    CleanupStack::PushL(self);
    self->ConstructL(aObserver);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
TmsHandler::~TmsHandler()
    {
    if (iTmsUplink && iTmsCall)
        {
        iTmsCall->DeleteStream(iTmsUplink);
        }
    if (iTmsDnlink && iTmsCall)
        {
        iTmsCall->DeleteStream(iTmsDnlink);
        }
    if (iFactory && iTmsCall)
        {
        iFactory->DeleteCall(iTmsCall);
        }
    if (iFactory && iTmsMicSource)
        {
        iFactory->DeleteSource(iTmsMicSource);
        }
    if (iFactory && iTmsModemSource)
        {
        iFactory->DeleteSource(iTmsModemSource);
        }
    if (iFactory && iTmsSpeakerSink)
        {
        iFactory->DeleteSink(iTmsSpeakerSink);
        }
    if (iFactory && iTmsModemSink)
        {
        iFactory->DeleteSink(iTmsModemSink);
        }
    delete iFactory;
    }

// ---------------------------------------------------------------------------
// TmsHandler::StartStreams
// Activates TMS call audio control streams. If called in the middle of an
// ongoing stream initialization, the request is cashed in until both streams
// transition to initialized state.
// ---------------------------------------------------------------------------
//
TInt TmsHandler::StartStreams()
    {
    CSPLOGSTRING(CSPINT, "TmsHandler::StartStreams");
    TInt status(TMS_RESULT_SUCCESS);

    if (iTmsUplink)
        {
        if (iTmsUplink->GetState() == TMS_STREAM_INITIALIZED)
            {
            status = iTmsUplink->Start(KRetryForever);
            }
        else if (iUplInitializing)
            {
            iStartAfterInitComplete = ETrue;
            }
        }

    if (iTmsDnlink && status == TMS_RESULT_SUCCESS)
        {
        if (iTmsDnlink->GetState() == TMS_STREAM_INITIALIZED)
            {
            status = iTmsDnlink->Start(KRetryForever);
            }
        else if (iDnlInitializing)
            {
            iStartAfterInitComplete = ETrue;
            }
        }

    CSPLOGSTRING2(CSPINT, "TmsHandler::StartStreams status %d", status);

    if (status != TMS_RESULT_SUCCESS)
        {
        // Cancel any pending retry and cancel.
        StopStreams();
        status = KErrCancel;
        }
    return status;
    }

// ---------------------------------------------------------------------------
// TmsHandler::StopStreams
// Deactivates TMS call audio control streams. If streams are not started yet,
// any pending request on TMS streams will be canceled.
// ---------------------------------------------------------------------------
//
void TmsHandler::StopStreams()
    {
    CSPLOGSTRING(CSPINT, "TmsHandler::StopStreams");

    if (iTmsUplink)
        {
        iTmsUplink->Stop();
        }
    if (iTmsDnlink)
        {
        iTmsDnlink->Stop();
        }
    iUplInitializing = EFalse;
    iDnlInitializing = EFalse;
    iStartAfterInitComplete = EFalse;
    }

// ---------------------------------------------------------------------------
// TmsHandler::AreStreamsStarted
// Indicates whether both audio control streams have been started.
// ---------------------------------------------------------------------------
//
TBool TmsHandler::AreStreamsStarted()
    {
    TBool status(EFalse);

    if (iTmsUplink && iTmsDnlink)
        {
        if (iTmsUplink->GetState() == TMS_STREAM_STARTED &&
                iTmsDnlink->GetState() == TMS_STREAM_STARTED)
            {
            status = ETrue;
            }
        }
    return status;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
TmsHandler::TmsHandler()
    {
    }

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void TmsHandler::ConstructL(MTmsHandlerObserver& aObserver)
    {
    CSPLOGSTRING(CSPINT, "TmsHandler::ConstructL");
    iObserver = &aObserver;
    User::LeaveIfError(CreateTMSCallControl());
    }

// ---------------------------------------------------------------------------
// TmsHandler::CreateTMSCallControl()
// Allocates and initializes all TMS resources needed for call audio control.
// ---------------------------------------------------------------------------
//
TInt TmsHandler::CreateTMSCallControl()
    {
    CSPLOGSTRING(CSPINT, "TmsHandler::CreateTMSCallControl");
    TInt status;
    TMSVer v(2,0,0);
    status = TMSFactory::CreateFactory(iFactory, v);

    __ASSERT_ALWAYS(iFactory, Panic(ECSPPanicBadHandle));

    status = iFactory->CreateCall(TMS_CALL_CS, iTmsCall, 0);

    if (iTmsCall && status == TMS_RESULT_SUCCESS)
        {
        status = iTmsCall->CreateStream(TMS_STREAM_UPLINK, iTmsUplink);
        status |= iTmsCall->CreateStream(TMS_STREAM_DOWNLINK, iTmsDnlink);
        status |= iFactory->CreateSource(TMS_SOURCE_MIC, iTmsMicSource);
        status |= iFactory->CreateSink(TMS_SINK_MODEM, iTmsModemSink);
        status |= iFactory->CreateSource(TMS_SOURCE_MODEM, iTmsModemSource);
        status |= iFactory->CreateSink(TMS_SINK_SPEAKER, iTmsSpeakerSink);
        }
    if (iTmsUplink && iTmsMicSource && iTmsModemSink &&
            status == TMS_RESULT_SUCCESS)
        {
        status = iTmsUplink->AddSource(iTmsMicSource);
        status |= iTmsUplink->AddSink(iTmsModemSink);
        status |= iTmsUplink->AddObserver(*this, NULL);
        status |= iTmsUplink->Init(KStreamInitRetry);
        }
    if (iTmsDnlink && iTmsModemSource && iTmsSpeakerSink &&
            status == TMS_RESULT_SUCCESS)
        {
        status = iTmsDnlink->AddSource(iTmsModemSource);
        status |= iTmsDnlink->AddSink(iTmsSpeakerSink);
        status |= iTmsDnlink->AddObserver(*this, NULL);
        status |= iTmsDnlink->Init(KStreamInitRetry);
        }
    if (status == TMS_RESULT_SUCCESS)
        {
        iUplInitializing = ETrue;
        iDnlInitializing = ETrue;
        }
    else
        {
        status = KErrCancel; // Convert to Symbian error and cancel
        }
    return status;
    }

// ----------------------------------------------------------------------------
// TmsHandler::ProcessUplinkStreamChangeEvent
// Processes Uplink stream state change events.
// ----------------------------------------------------------------------------
//
void TmsHandler::ProcessUplinkStreamChangeEvent(TInt aState)
    {
    CSPLOGSTRING(CSPINT, "TmsHandler::ProcessUplinkStreamChangeEvent");
    switch (aState)
        {
        case TMS_STREAM_INITIALIZED:
            iUplInitializing = EFalse;
            if (iStartAfterInitComplete)
                {
                iStartAfterInitComplete = EFalse;
                TInt status = StartStreams();
                if (status != TMS_RESULT_SUCCESS)
                    {
                    iObserver->AudioStreamsError(status);
                    }
                }
            break;
        case TMS_STREAM_UNINITIALIZED:
            // As result of Deinit() or error
            iUplInitializing = EFalse;
            iStartAfterInitComplete = EFalse;
            break;
        case TMS_STREAM_PAUSED:
            break;
        case TMS_STREAM_STARTED:
            if (AreStreamsStarted())
                {
                iObserver->AudioStreamsStarted();
                }
            break;
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// TmsHandler::ProcessDownlinkStreamChangeEvent
// Processes Downlink stream state change events.
// ----------------------------------------------------------------------------
//
void TmsHandler::ProcessDownlinkStreamChangeEvent(TInt aState)
    {
    CSPLOGSTRING(CSPINT, "TmsHandler::ProcessDownlinkStreamChangeEvent");
    switch (aState)
        {
        case TMS_STREAM_INITIALIZED:
            iDnlInitializing = EFalse;
            if (iStartAfterInitComplete)
                {
                iStartAfterInitComplete = EFalse;
                TInt status = StartStreams();
                if (status != TMS_RESULT_SUCCESS)
                    {
                    iObserver->AudioStreamsError(status);
                    }
                }
            break;
        case TMS_STREAM_UNINITIALIZED:
            // As result of Deinit() or error
            iDnlInitializing = EFalse;
            iStartAfterInitComplete = EFalse;
            break;
        case TMS_STREAM_PAUSED:
            break;
        case TMS_STREAM_STARTED:
            if (AreStreamsStarted())
                {
                iObserver->AudioStreamsStarted();
                }
            break;
        default:
            break;
        }
    }

// TMS CALLBACKS

// ----------------------------------------------------------------------------
// TmsHandler::TMSStreamEvent
// From TMSStreamObserver
// TMS call audio stream control event handler.
// ----------------------------------------------------------------------------
//
void TmsHandler::TMSStreamEvent(const TMSStream& stream, TMSSignalEvent event)
    {
    CSPLOGSTRING2(CSPINT, "TmsHandler::TMSStreamEvent status %d", event.reason);

    TMSStreamType strmType = const_cast<TMSStream&> (stream).GetStreamType();

    if (strmType == TMS_STREAM_UPLINK)
        {
        if (event.type == TMS_EVENT_STREAM_STATE_CHANGED)
            {
            ProcessUplinkStreamChangeEvent(event.curr_state);
            }
        else if (event.type == TMS_EVENT_STREAM_STATE_CHANGE_ERROR)
            {
            Panic( ECSPPanicAudioStreamInitFailure );
            }
        }
    else if (strmType == TMS_STREAM_DOWNLINK)
        {
        if (event.type == TMS_EVENT_STREAM_STATE_CHANGED)
            {
            ProcessDownlinkStreamChangeEvent(event.curr_state);
            }
        else if (event.type == TMS_EVENT_STREAM_STATE_CHANGE_ERROR)
            {
            Panic( ECSPPanicAudioStreamInitFailure );
            }
        }
    }

//  End of File
