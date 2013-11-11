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
 * Description:  Declarations for class CSPAudioHandler
 *
 */

#ifndef CSPAUDIOHANDLER_H
#define CSPAUDIOHANDLER_H

#include <e32base.h>
#include "cspaudiohandlerbase.h"
#include "mtmshandlerobserver.h"
#include "mcsptimerobserver.h"

//FORWARD DECLARATIONS
class TmsHandler;
class CSPTimer;

/**
 * Handles call adding from calls not done by the plugin.
 *
 */
class CSPAudioHandler : public CSPAudioHandlerBase,
                        public MTmsHandlerObserver,
                        public MCSPTimerObserver
    {
public:
    //Constructors and descructor

    /**
     * Two-phased constructing for the monitor.
     */
    static CSPAudioHandler* NewL();

    /**
     * C++ default destructor.
     */
    virtual ~CSPAudioHandler();
    
// from base class CSPAudioHandlerBase     

    /*
     * From CSPAudioHandlerBase
     * Set observer to receive call audio stream events.     
     */
    void SetObserver(MCSPAudioHandlerObserver& aObserver);

    /**
     * From CSPAudioHandlerBase
     * Start audio streams.     
     */
    void Start();

    /**
     * From CSPAudioHandlerBase
     * Stop audio streams.     
     */
    void Stop();

    /*
     * From CSPAudioHandlerBase
     * Specify timeout after which stream start retry will be terminated.
     */
    void ReportAudioFailureAfterTimeout(TInt aTimeout);

protected:
    /*
     * From MTmsHandlerObserver
     * Indicates call audio stream start success.
     */
    void AudioStreamsStarted();

    /*
     * From MTmsHandlerObserver
     * Indicates call audio stream start failure.
     */
    void AudioStreamsError(TInt aError);

    /**
     * From MCSPTimerObserver.
     */
    void TimerEvent();

private:
    /**
     * C++ default constructor
     */
    CSPAudioHandler();

    /**
     * Constructs the monitor in the second phase.
     */
    void ConstructL();

private:
    // data

    /**
     * Audio streams handler.
     */
    TmsHandler* iTmsHandler;

    /**
     * Call count
     */
    TInt iCallCount;

    /**
     * Timer waiting for the activation of streams.
     */
    CSPTimer* iTimer;

    /*
     * Observer receiving call audio stream start events.
     */
    MCSPAudioHandlerObserver* iObserver;
    };

#endif // CSPAUDIOHANDLER_H

