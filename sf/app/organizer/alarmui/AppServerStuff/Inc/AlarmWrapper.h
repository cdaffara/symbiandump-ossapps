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
* Description:  This is the MAlarmObserver I/F implementation instantiated and used directly by EIkAlert. 
*
*/



#ifndef __AKNALARMWRAPPER_H__
#define __AKNALARMWRAPPER_H__

// INCLUDES
#include "pim_trace.h"
#include <AknCapServerClient.h>
#include <AlarmObserver.h>
#include "AlarmServiceDefs.h"


// DLL entry point
IMPORT_C MAlarmObserver* NewAlarm();


// FORWARD DECLARATIONS
class CAknDataFetcher;

/**
* This class connects only on demand, this is because eiksrv is started 
* before akncapserver and connecting would fail if done during construction 
**/
class RAknAlarmClient : public RAknUiServer
    {
public:
    RAknAlarmClient(CAknDataFetcher** aFetcher);
    TInt SendSynch(TInt aOpcode);
    TInt SendSynch(TInt aOpcode, TInt& aParam);
    void SendAsync(TInt aOpcode, TRequestStatus& aStatus, TIpcArgs& aArgs );
    TInt SetAlarm(const TASShdAlarm& aAlarm, const TFullName& aOwner, const TDesC8& aAlarmData);

private: // from  RAknCapServerClient
    TInt Connect(); // just to phohibit too early connection attemps

    TUid ServiceUid() const
        {
        TRACE_ENTRY_POINT;
        TRACE_EXIT_POINT;
        return KAknAlarmServiceUid;
        };

    TInt CheckConnection();

private:
    CAknDataFetcher** iFetcher; // not owned

    };


/**
* AO for fetching messages (snooze & stop)
**/
NONSHARABLE_CLASS( CAknDataFetcher ) : public CActive
    {
public:
    CAknDataFetcher( TInt aPriority, 
                     CEikAlmControlSupervisor* aSupervisor, 
                     RAknAlarmClient& aClient );
    void Start();
    void RunL();
    void DoCancel();
    CEikAlmControlSupervisor* iSupervisor; // not owned

private:
    RAknAlarmClient& iClient;
    TPckgBuf<TInt> iPckg;
    TIpcArgs iArgs;

    };


/**
* Main class
**/
NONSHARABLE_CLASS( CAknAlarmWrapper ) : public CBase, 
                                        public MAlarmObserver
    {
public: // from MAlarmObserver
    void ConstructL( CEikAlmControlSupervisor* aSupervisor,
                     CEikServAppUiBase* aAppUi );

public: // from MEikServAlarm
    void Release();
    void ShowAlarm();
    void HideAlarm();
    TInt CurrentServerState() const;
    void UpdateSoundPauseTimeInterval(TInt aMinutes);
    void UpdateForAlarmServerState(TInt aNewAlarmServerState);
    void UpdateAlarmInfo(const TASShdAlarm& aAlarm, const TFullName& aOwner);
    void StartPlayAlarmL(const TDesC& aAlarmName);
    void StopPlayAlarm();

public:
    CAknAlarmWrapper();

private:
    ~CAknAlarmWrapper();
    CEikServAppUiBase* iAppUi; // not owned
    __MUTABLE RAknAlarmClient iClient;
    CAknDataFetcher* iActive;

    };

#endif // __AKNALARMWRAPPER_H__


// End of File
