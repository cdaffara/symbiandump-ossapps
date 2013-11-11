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

#ifndef TMSHANDLER_H
#define TMSHANDLER_H

#include <e32base.h>
#include <tms.h>
#include <tmsstreamobsrvr.h>

namespace TMS {
//FORWARD DECLARATIONS
class TMSFactory;
class TMSCall;
class TMSStream;
class TMSSource;
class TMSSink;
} //namespace TMS

using namespace TMS;

//FORWARD DECLARATIONS
class MTmsHandlerObserver;

/**
 *  Class responsible for creation and activation of TMS call audio control
 *  streams.
 */
NONSHARABLE_CLASS(TmsHandler) : public CBase,
                                public TMSStreamObserver
    {
public:

    /**
     * Two-phased constructing.
     */
    static TmsHandler* NewL(MTmsHandlerObserver& aObserver);

    /**
     * Two-phased constructing.
     */
    static TmsHandler* NewLC(MTmsHandlerObserver& aObserver);

    /**
     * C++ default destructor.
     */
    virtual ~TmsHandler();

    /**
     * Activates TMS call audio control streams.
     */
    TInt StartStreams();

    /**
     * Deactivates TMS call audio control streams.
     */
    void StopStreams();

    /*
     * Indicates whether both audio streams have been started.
     */
    TBool AreStreamsStarted();

    /*
     * From TMSStreamObserver
     * TMS call audio stream control event handler.
     */
    void TMSStreamEvent(const TMSStream& stream, TMSSignalEvent event);

protected:

    /**
     * C++ default constructor.
     */
    TmsHandler();

    /*
     * Symbian constructor
     */
    void ConstructL(MTmsHandlerObserver& aObserver);

private:

    /*
     * Method creating and activating TMS call control objects.
     */
    TInt CreateTMSCallControl();

    /*
     * Processes Uplink stream state change events.
     */
    void ProcessUplinkStreamChangeEvent(TInt aState);

    /*
     * Processes Downlink stream state change events.
     */
    void ProcessDownlinkStreamChangeEvent(TInt aState);

private:

    /*
     * Observer to TMS call audio stream events.
     */
    MTmsHandlerObserver* iObserver;

    /*
     * Pointers to TMS call audio control objects.
     */
    TMSFactory* iFactory;
    TMSCall* iTmsCall;
    TMSCallType iCallType;
    TMSStream* iTmsDnlink;
    TMSStream* iTmsUplink;
    TMSSource* iTmsMicSource;
    TMSSource* iTmsModemSource;
    TMSSink* iTmsSpeakerSink;
    TMSSink* iTmsModemSink;

    /*
     * Stream state control helpers
     */
    TBool iStartAfterInitComplete;
    TBool iUplInitializing;
    TBool iDnlInitializing;
    };

#endif // TMSHANDLER_H

