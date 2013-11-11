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
 * Description:  Audio stream controlling interface
 *
 */

#ifndef CSPAUDIOHANDLERBASE_H_
#define CSPAUDIOHANDLERBASE_H_

class MCSPAudioHandlerObserver;

/**
 * Provides interface for controlling audio streams.
 *
 */
class CSPAudioHandlerBase : public CBase
    {
public:    

    /*
     * Set observer to receive call audio stream events.
     */
    virtual void SetObserver(MCSPAudioHandlerObserver& aObserver) = 0;

    /**
     * Start audio streams.
     */
    virtual void Start() = 0;

    /**
     * Stop audio streams.
     */
    virtual void Stop() = 0;

    /*
     * Specify timeout after which stream start retry will be terminated.
     */
    virtual void ReportAudioFailureAfterTimeout(TInt aTimeout) = 0;

    };

#endif /* CSPAUDIOHANDLERBASE_H_ */
