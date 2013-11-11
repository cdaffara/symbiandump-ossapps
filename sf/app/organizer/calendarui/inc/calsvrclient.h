/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements client session part for calendar server.
*
*/



#ifndef CALSVRCLIENT_H
#define CALSVRCLIENT_H

#include <e32std.h>

class CCalSvrInitializer;


NONSHARABLE_CLASS( MCalSvrAgendaUpdateListener )
    {
public:
    virtual void CalendarServerInitialized() = 0;
    };

NONSHARABLE_CLASS( RCalSvrSession ) : public RSessionBase
    {
public:
    // construct
    IMPORT_C RCalSvrSession();

    // open/close
    IMPORT_C void ConnectL();
    IMPORT_C void Close();

    // protocol for Notifier 

    // protocol for Listeners

    IMPORT_C void Initialize(MCalSvrAgendaUpdateListener& aListener);
    IMPORT_C void StopInitialization();
    IMPORT_C void Uninitialize();

private:
    void Receive(TRequestStatus& aStatus);
    void CancelReceive();

    void Initialize(TRequestStatus& aStatus);
    void CancelInitialize();

private:
    MCalSvrAgendaUpdateListener* iListenerCallback;

    friend class CCalSvrInitializer;
    CCalSvrInitializer* iInitializer;

    };


#endif // CALSVRCLIENT_H


// End of File
