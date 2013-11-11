/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Works as a proxy together with RAknAlarmClient and CAknAlarmWrapper between EikAlert and CAlmAlarmControl.  
*
*/



#ifndef __AKNALARMSERVICE_H__
#define __AKNALARMSERVICE_H__

// INCLUDES
#include <AknServerApp.h>
#include "AlarmServiceDefs.h"

// FORWARD DECLARATIONS
class CAlmAlarmControl;


// entry point
IMPORT_C MAknAlarmServiceStartup* Entry();


/**
* An app service for passing commands from 
* CEikAlmControlSupervisor to CAlmAlarmControl.
**/
NONSHARABLE_CLASS(CAknAlarmService): public CAknAppServiceBase,
                                     public MAknAlarmServiceStartup
    {
public: // from MAknAlarmServiceStartup
    void StartupNewSessionL(TAny* aAny = 0);

public:
    CAknAlarmService();
    ~CAknAlarmService();

public:
    void CmdAcknowledgeAlarm();
    TBool CmdTaskAwayFromAlarmL();
    TBool CmdTaskAwayFromAlarmL(TInt aMinutesToSnooze);

public: // from CAknAppServiceBase
    void ServiceL(const RMessage2& aMessage);

private:
    void UpdateAlarmInfo(const RMessage2& aMessage);
    void SetAlertState(TInt aAlarmServerStateFlags);

private:
    RMessage2 iMessage;
    CAlmAlarmControl* iAlarmAlert;

    };

#endif // __AKNALARMSERVICE_H__


// End of File
