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



#ifndef __TEST_CALINTERIMAPI_NOTIFIER_H__
#define __TEST_CALINTERIMAPI_NOTIFIER_H__


#include <coreappstest/testserver.h>
#include <test/testexecuteserverbase.h>
#include "TestCalInterimApiMainThread.h"
#include <calentryview.h>
#include <calprogresscallback.h>
#include <calsession.h>
#include <calrrule.h>


GLDEF_C TInt RunNotifierThread(TAny* aTestStep); // Notifier thread entry point
LOCAL_D const TTimeIntervalMicroSeconds32 KWait1second(1000000); // 1 second


_LIT(KNumberOfNotificationsExpected,         	"%d Notifications were Expected");
_LIT(KTypeToDo,									"todo");
_LIT(KTypeEvent,								"event");
_LIT(KTypeALL,									"all");
_LIT(KFilterTimeModeLocal,						"local");
_LIT(KFilterTimeModeLocalFloating,				"localfloating");
_LIT(KFilterEntryType,           				"filterentrytype");
_LIT(KFilterStartTime,         					"filterstarttime");
_LIT(KFilterEndTime,           					"filterendtime");
_LIT(KFilterTimeMode,							"timemode");
_LIT(KUndatedTodos,             				"undatedtodos");
_LIT(KNotifier,		             				"notifier");
_LIT(KNumOfExpectedNotifications,          		"numofexpectednotifications");
_LIT(KLocalUid,									"localuid%d");
_LIT(KChangeType,								"changetype%d");
_LIT(KEntryType,								"entrytype%d");
_LIT(KInfoIndex,								"Index %d");
_LIT(KVerification,								"verification");

_LIT(KInfoCalChangeNotification,				"Received Notifications ==> %d, Expected Notifications ==> %d");
_LIT(KInfoReceivedAndExpectedLocalUid,			"Received LocalUid ==> %d, Expected LocalUid ==> %d");
_LIT(KInfoReceivedAndExpectedEntryType,			"Received EntryType ==> %d, Expected EntryType ==> %d");
_LIT(KInfoReceivedAndExpectedChangeType,		"Received ChangeType ==> %d, Expected ChangeType ==> %d");

_LIT(KErrInvalidNotifications,					"Invalid LocalUId, ChangeType and EntryType");
_LIT(KErrCalChangeNotification,					"Received No.Of Notification ==> %d, Expected ==> %d");
_LIT(KErrCalChangeNotificationsReceived,		"Invalid Notifications received");


/**
 * A Notifier thread class & basically registers with the calendar server for notifications
 * It also validates the cal change notifications received  from the calendar server against expected notifications
 */
class CTestCalInterimApiNotifier : public CBase, public MCalChangeCallBack2
{
public:
	CTestCalInterimApiNotifier();
	~CTestCalInterimApiNotifier();
	
	static	CTestCalInterimApiNotifier* NewL(CTestStep* aTestStep);
	
	void	RunTestL();
	TPtrC&	SemaphoreName();


private:
	void	ConstructL(CTestStep* aTestStep);
	static	TInt StopWaitingForNotification(TAny* aPtr);
	void	DoStopWaitingForNotification();
	void	CreateNotificationFilterL();
	void	CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems);
	void	NextStep(const TBool& aNotifier);
	
	MCalChangeCallBack2::TChangeEntryType  GetFilterEntryType(const TPtrC& aFilter);

private:
	CTestStep* 						 iTestStep;
	CCalSession*                     iSession;
	CPeriodic*			             iTimer;
	CCalChangeNotificationFilter*    iFilter;
	TBool				             iNotified;
	RSemaphore			             iSemaphore;
	TPtrC*                           iSemName;
	TInt							 iNumOfExpectedNotifications;
	RArray<TInt>					 iChangeTypeArray;
	RArray<TInt>					 iEntryTypeArray;
	TBool							 iVerification;
	CTestCalInterimApiMainThread*	 iMainThread;
	
};

#endif

