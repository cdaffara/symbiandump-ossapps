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
 * Description:  Observer interface
 *
 */

#ifndef TMSHANDLEROBSERVER_H
#define TMSHANDLEROBSERVER_H

/**
 *  Class observing callbacks from TmsHandler.
 *
 */
class MTmsHandlerObserver
    {
public:
    /**
     * Indicates successful start of call audio streams.
     */
    virtual void AudioStreamsStarted() = 0;

    /*
     * Indicates an error during audio streams startup.
     */
    virtual void AudioStreamsError(TInt aError) = 0;
    };

#endif // TMSHANDLEROBSERVER_H

