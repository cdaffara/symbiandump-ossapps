/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TEST_CALINTERIMAPI_OBSERVER_THREAD_H__
#define __TEST_CALINTERIMAPI_OBSERVER_THREAD_H__

#include <coreappstest/testserver.h>
#include <test/testexecuteserverbase.h>
#include "TestCalInterimApiModifierThread.h"
#include <calentryview.h>
#include <calprogresscallback.h>
#include <calsession.h>
#include <calrrule.h>


GLDEF_C TInt RunObserverThread(TAny* aTestStep); // modifying thread entry point
LOCAL_D const TTimeIntervalMicroSeconds32 KWait1second(1000000); // 1 second


_LIT(KInfoCalChangeNotification, 			 	 "%d Notifications are Recieved");
_LIT(KNumberOfNotificationsExpected,         	 "%d Notifications were Expected");
_LIT(KInfoCalChangeNotificationCallbackData,	 "Change From CallBack ==> Entry LocalUid = %d; Entry type = %d; Change type = %d");
_LIT(KInfoDesiredCalChangeNotificationData, 	 "Expected Changes     ==> Entry LocalUid = %d; Entry type = %d; Change type = %d");
_LIT(KErrEntryIdNotFound,						 "Invalid Entry Id found \n");
_LIT(KErrInvalidChangeTypeFound,				 "Invalid Change Type found \n");
_LIT(KErrExcessCalChangeNotificationsReceived,	 "Excess notification received \n");


/**
 * An observer thread class & basically registers with the calendar server for notifications
 * It also validates the cal change notifications received  from the calendar server against expected notifications
 */
class CTestCalInterimApiObserverThread : public CBase, public MCalChangeCallBack2
{
public:
	CTestCalInterimApiObserverThread();
	~CTestCalInterimApiObserverThread();
	static CTestCalInterimApiObserverThread* NewL(CTestStep* aTestStep);
	void ConstructL(CTestStep* aTestStep);
	void RunTestL();
	static	TInt StopWaitingForNotification(TAny* aPtr);
	void DoStopWaitingForNotification();
	void CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems);
	void NextStepL();
	TPtrC&  SemaphoreName();
	void RefreshData();

public:
	CTestCalInterimApiModifierThread* 			iTestStep;
	CCalSession*                     			iSession;
	CPeriodic*			             			iTimer;
	CCalChangeNotificationFilter*    			iFilter;
	TBool				             			iNotified;
	RSemaphore			             			iSemaphore;
	RChunk				             			iChunk;
	TInt				            			iNumExpectedChanges;
	TCalChangeEntry*	             			iExpectedChanges;
	TFileName                        			iFileName;
	TPtrC                          	 			iSemName;
	TBool										iStopChangeNotification;
	TBool                           			iExecutionCompleted;
	TBool							 			iNotifierOff;
};

#endif
