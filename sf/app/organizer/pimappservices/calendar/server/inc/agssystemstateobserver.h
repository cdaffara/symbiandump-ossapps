/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef CAGNSYSTEMSTATEOBSERVER_H
#define CAGNSYSTEMSTATEOBSERVER_H

#include <e32base.h>	
#include <ssm/ssmstateawaresession.h>
#include <e32property.h> //RProperty workaround
#include "agsfilemanager.h"

class CCAgnSystemStateObserver : public CActive
	{
public:
    static CCAgnSystemStateObserver* NewL (CAgnServFileMgr& aFileMgr);
    ~CCAgnSystemStateObserver ();
    TBool IsShutdownInProgress() const;
private:
    CCAgnSystemStateObserver (CAgnServFileMgr& aFileMgr);
    // Second-phase constructor
    void ConstructL ();
    //Some system state work around functions
    TBool SubscribeToShutdownNotifications();
    void ConvertToSystemState(TSsmState& aState);
    void SendSystemStateAck();
    void GetState(TSsmState& state);
    void AcknowledgeAndRequestStateNotification();
private:
    // From CActive
    void RunL ();
    void DoCancel ();
    TInt RunError (TInt aError);

private:
    RSsmStateAwareSession iStateAwareSession;
    RProperty iNotification;
    TBool iWorkAround;
    CAgnServFileMgr& iFileMgr;
    TBool iShutdown;
	};

#endif // CAGNSYSTEMSTATEOBSERVER_H
