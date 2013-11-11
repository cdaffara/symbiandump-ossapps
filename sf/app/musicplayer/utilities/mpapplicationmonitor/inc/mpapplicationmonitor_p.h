/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Music Player application monitor utility - private implementation.
*
*/

#ifndef MPAPPLICATIONMONITORPRIVATE_H
#define MPAPPLICATIONMONITORPRIVATE_H

#include <w32std.h>
#include <qglobal.h>

class MpApplicationMonitor;

class MpApplicationMonitorPrivate : public CActive
{
public:

    explicit MpApplicationMonitorPrivate( MpApplicationMonitor *qq );
    virtual ~MpApplicationMonitorPrivate();

    void init( quint32 clientSecureId );
    bool isApplicationRunning();

protected:

    void RunL();
    void DoCancel();

private:

    void DoInitL();
    bool DoIsApplicationRunningL();
    void CreateWindowGroupL();
    void StartMonitor();

private:

    MpApplicationMonitor        *q_ptr;

    TUid                        iHostUid;
    RWindowGroup                iWindowGroup;
    RWsSession                  iWsSession;

};

#endif  // MPAPPLICATIONMONITORPRIVATE_H
