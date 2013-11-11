/*
 * Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Observer interface for getting notifes from CSPTimer
 *
 */

#ifndef MCSPTIMEROBSERVER_H
#define MCSPTIMEROBSERVER_H

/**
 *  Observer for CSPTimer events.
 *
 */
class MCSPTimerObserver
    {
public:
    /**
     * Indicates timer timeout event.
     */
    virtual void TimerEvent() = 0;
    };

#endif // MCSPTIMEROBSERVER_H

