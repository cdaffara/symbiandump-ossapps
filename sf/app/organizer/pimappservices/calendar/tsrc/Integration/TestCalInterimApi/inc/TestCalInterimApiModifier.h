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



#ifndef __TEST_CALINTERIMAPI_MODIFIER_H__
#define __TEST_CALINTERIMAPI_MODIFIER_H__

#include <coreappstest/testserver.h>
#include <test/testexecuteserverbase.h>
#include "TestCalInterimApiSuiteStepBase.h"
#include "TestCalInterimApiMainThread.h"
#include <e32std.h>


#include <calsession.h>
#include <calentryview.h>
#include <calrrule.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <caliterator.h>

// Constant Literals
_LIT(KTestCalInterimModifier,						"TestCalInterimModifier");

_LIT(KSemaphoreName,            					"semaphorename");
_LIT(KCalendarFileName,         					"calenderfile");

_LIT(KDeleteType, 									"deletetype");
_LIT(KDeleteByReference, 							"byreference");
_LIT(KDeleteByLocalUids,							"bylocaluids");
_LIT(KDeleteByTimeRange,							"bytimerange");
_LIT(KDeleteByInstances,							"byinstances");
_LIT(KAddEntriesFlag,								"add");
_LIT(KUpdateEntriesFlag,							"update");
_LIT(KDeleteEntriesFlag,							"delete");
_LIT(KDeleteFilterStartTime,						"delfilterstarttime");
_LIT(KDeleteFilterEndTime,							"delfilterendtime");
_LIT(KFilterTimeMode,								"TimeMode");
_LIT(KFilterTimeModeLocal,							"Local");
_LIT(KFilterTimeModeLocalFloating,					"LocalFloating");
_LIT(KUid,											"uid%d");
_LIT(KExpectedNameOfFile,							"expectedname%d");
_LIT(KDisableChangeBroadCast,             			"disablechangebroadcast");
_LIT(KEnableChangeBroadCast,             			"enablechangebroadcast");
_LIT(KAddOperation, 								"\n Addition of Entries in Progress..... \n");
_LIT(KModifyOperation, 								"\n Updation of Entries in Progress..... \n");
_LIT(KDeleteOperation, 								"\n Deletion of Entries in Progress..... \n");

_LIT(KInfoDatafile,									"\n datafile=%S \n");
_LIT(KInfoIndex,									"Index %d");
_LIT(KInfoEntryStored,								"Entry Stored successfully");

_LIT(KErrAddOperation,								"\n Add operation failed with err code %d \n");
_LIT(KErrUpdateOperation,							"\n Update operation failed with err code %d \n");
_LIT(KErrDeleteOperation,							"\n Delete operation failed with err code %d \n");
_LIT(KErrDatafileDataMissing,						"\n datafile data missing \n");
_LIT(KErrCalChangeNotificationsReceived,			" Error in CalChangeNotifications");

_LIT(KWarnInstancesNotFound,						"Instances are not present");


GLDEF_C TInt RunModifierThread(TAny* aTestStep); // Modifier thread entry point

/**
 * Teststep that launches an observer thread & performs add, modify & delete of Calendar entries
 * It also populates an array of expected notifications which are validated by the observer
 */
class CTestCalInterimApiModifier : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiModifier();
	~CTestCalInterimApiModifier();
	static	CTestCalInterimApiModifier* NewL(CTestStep* aTestStep);
	void	ConstructL(CTestStep* aTestStep);
	
	virtual TVerdict	doTestStepL();
	void	RunTestsL();
	TPtrC&	ModifierSemaphoreName();
	
private:
	void	DisableCalChangeBroadcast();
	void	EnableCalChangeBroadcast();
	void 	AddModifyDeleteEntriesL(const TPtrC& aDeleteFlag);
	void    AddEntriesL();
    void 	UpdateEntriesL();
	void    RetreiveIniDataL();
	void 	DeleteEntriesByUidListL();
	void 	DeleteEntriesByReferenceL();
	void 	DeleteEntriesByTimeRangeL();
	void 	DeleteInstancesL();
	void 	DeleteEntriesByArrayOfLocalUidsL();
	void	PopulateEntriesGuidListL();

private:
	RPointerArray<CCalEntry>	        iEntryArray;
	CTestStep* 							iTestStep;
	CCalEntryView*						iEntryView;
	RSemaphore							iSemaphore;
	CCalSession*						iSession;
	TPtrC*								iSemName;
	CTestCalInterimApiMainThread*		iModifier;
	CCalIter*							iEntryIterator;
	CDesC8ArrayFlat*					iGuidList;
	};

#endif
