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



#ifndef __TEST_CALINTERIMAPI_MAIN_THREAD_H__
#define __TEST_CALINTERIMAPI_MAIN_THREAD_H__

#include "TestCalInterimApiReadData.h"
#include <e32std.h>


#include <calsession.h>
#include <calchangecallback.h>


 // Constant Literals
_LIT(KNotifierThreadName,          			"TestCalInterimNotifierThread");
_LIT(KNotifierThreadName1,         			"TestCalInterimNotifierThread1");
_LIT(KNotifierThreadName2,         			"TestCalInterimNotifierThread2");

_LIT(KModifierThreadName,               	"TestCalInterimModifierThread");
_LIT(KModifierThreadName1,               	"TestCalInterimModifierThread1");

_LIT(KTestCalInterimApiMainThread,			"TestCalInterimApiMainThread");
_LIT(KMainChunkName, 						"TestCalInterimModifierNotifierChunk");

_LIT(KNewSemaphoreName,            			"semaphorename");
_LIT(KNewCalendarFileName,         			"calenderfile");
_LIT(KDataFile1,							"datafile");
_LIT(KAdditionalModifier,					"additionalmodifier");

_LIT(KInfoNotifierThread1,         			"\n Notifier 1:");
_LIT(KInfoNotifierThread2,         			"\n Notifier 2:");
_LIT(KInfoNotifierThread3,         			"\n Notifier 3:");
_LIT(KInfoModifierThread1,         			"\n ModifierThread 1:");
_LIT(KInfoModifierThread2,         			"\n ModifierThread 2:");

GLDEF_C TInt RunNotifierThread(TAny* aTestStep); // Notifier thread entry point
GLDEF_C TInt RunModifierThread(TAny* aTestStep); // Modifier thread entry point

/**
 * Teststep that launches an observer thread & Modifier Thread which performs add, modify & delete of Calendar entries
 * It also validates the number of expected notifications with the notifications received
 */
class CTestCalInterimApiMainThread : public CTestCalInterimApiReadData
	{
public:
	CTestCalInterimApiMainThread();
	~CTestCalInterimApiMainThread();

	virtual TVerdict	doTestStepL();
	void	ProcessEntryL(CCalEntry* aEntry);

	void 	LaunchNotifierThreadsL();
	void	LaunchModifierThreadsL(const TBool& aAdditionalModifier);
	void	RetreiveIniDataL();
	void	ResumeNotifierThreads();
	void	ResumeModifierThreadsL(const TBool& aAdditionalModifier);
	void 	ResetEntryL();

public:
	RPointerArray<CCalEntry>			iEntryArray;
	RPointerArray<CCalEntry>			iOriginalEntryArray;
	RSemaphore							iSemaphore;
	CCalSession*						iSession;
	TPtrC								iSemName;
	TBool								iAdditionalModifierThread;
	RThread								iThread;
	RThread								iThread1;
	RThread								iThread2;
	RThread								iModifierThread;
	RThread								iModifierThread1;
	};

#endif
