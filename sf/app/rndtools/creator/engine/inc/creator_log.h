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
* Description: 
*
*/





#ifndef __CREATORLOGS_H__
#define __CREATORLOGS_H__

#include "engine.h"
#include "creator_modulebase.h"


#include <e32base.h>
//#include <logwrap.h>
#include <logwraplimits.h>
#include <logcli.h>
//#include <logeng.h>
#include <logengdurations.h>



class CCreatorEngine;
class CLogsParameters;


class CCreatorLogs : public CActive, public MCreatorModuleBase, public MUIObserver
    {
enum TCreatorLogsStatus{
    ECreatorLogsDelete = 0,
    ECreatorLogsStart
};

public: 
    static CCreatorLogs* NewL(CCreatorEngine* aEngine);
    static CCreatorLogs* NewLC(CCreatorEngine* aEngine);
    ~CCreatorLogs();

private:
    CCreatorLogs();
    void ConstructL(CCreatorEngine* aEngine); // from MCreatorModuleBase

    void RunL();  // from CActive
	void DoCancel();  // from CActive

public:
    TBool AskDataFromUserL( TInt aCommand ); // from MCreatorModuleBase
    void QueryDialogClosedL(TBool aPositiveAction, TInt aUserData);    
    
    TInt CreateMissedCallEntryL(CLogsParameters *aParameters);    
    TInt CreateReceivedCallEntryL(CLogsParameters *aParameters);
    TInt CreateDialledNumberEntryL(CLogsParameters *aParameters);
    void DeleteAllL();
    void DeleteAllCreatedByCreatorL();

private:
    CCreatorEngine* iEngine;
    TInt iCommand;
    TInt iEntriesToBeCreated;
    TInt iDummy;

    CLogClient* iLogClient;
    CLogEvent* iLogEvent;
    CActiveSchedulerWait iLogWriteWait;

    CLogsParameters* iParameters;
    RArray<TInt> iEntryIds; // TLogId ( == TInt32) CLogEvent::Id()

public:
    };


class CLogsParameters : public CCreatorModuleBaseParameters
    {
public: 
    /*HBufC*              iMissedCallPhoneNumber;
    TTime               iMissedCallEventTime;
	
    HBufC*              iReceivedCallPhoneNumber;
    TTime               iReceivedCallEventTime;
    TLogDuration        iReceivedCallDuration;

    HBufC*              iDialledNumberPhoneNumber;
    TTime               iDialledNumberEventTime;
    TLogDuration        iDialledNumberDuration;*/
    HBufC*              iPhoneNumber;
    TTime               iEventTime;
    TLogDuration        iDuration;

public:
    CLogsParameters();
    ~CLogsParameters();
    };



#endif // __CREATORLOGS_H__
