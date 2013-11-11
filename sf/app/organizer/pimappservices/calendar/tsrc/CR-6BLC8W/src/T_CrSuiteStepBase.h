// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#if (!defined __T_CR_STEP_BASE__)
#define __T_CR_STEP_BASE__

#include <calentry.h>
#include <calsession.h>
#include <calentryview.h>
#include <calprogresscallback.h>
#include <test/testexecutestepbase.h>

class CTzId;

#define MAX_TEXT_MESSAGE 256
#define MAX_UID_BUF_SIZE 20
#define MAX_PROPERTY_STRLEN 20
#define MAX_LONG_STRLEN 50

_LIT(KAgendaFile,"C:CITVCal.dat");

/** Entry Types */
_LIT(KToDo,"VTODO");
_LIT(KEvent,"VEVENT");
_LIT(KReminder,"VREMINDER");
_LIT(KAppt,"VAPPT");
_LIT(KAnniv,"VANNIV");
_LIT(KRule,"RRULE:");
_LIT(KYearly,"YEARLY");
_LIT(KMonthly,"MONTHLY");
_LIT(KWeekly,"WEEKLY");
_LIT(KDaily,"DAILY");

class CCalProgress: public CActive, public MCalProgressCallBack
	{
public:	
	CCalProgress();
	~CCalProgress();
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt aError);
	TBool NotifyProgress(); 

	TBool Completed();
	TInt  Progress();
	void  Reset();
	
	void Start();
private:

	void Close();
	void DoCancel();
	void RunL();

	TInt iProgress;
	TBool iCompleted;	
	};
	
class CCrTestStepBase : public CTestStep	
	{
public:
	CCrTestStepBase();
	~CCrTestStepBase();
	virtual TVerdict doTestStepPreambleL();
	void SetTZL(TPtrC aTZ);
	void RestoreTZL();
	
protected:	
	void OpenSessionFileL();
	CCalEntry* CreateEntryL(CCalEntry::TType aType, TTime aStartTime, TTime aEndTime, TCalTime::TTimeMode aMode);
	void CreateEntriesL(TPtrC& aDataPtr,RPointerArray<CCalEntry>& aEntries);
	static void CloseAndDeleteRPtrArrayEntries(TAny* aArray);
	static void CloseAndDeleteRPtrArrayInstances(TAny* aArray);

protected:
	CCalSession* iSession;	
	CActiveScheduler* iScheduler;

private:
	CTzId* iPrevTZ;
	};

template< typename T > class TCleanupRPtrArrayDeleteAndClose
	{
public:
	static void PushL(T& aRef)
		{
		CleanupStack::PushL(TCleanupItem(&RPtrArrayDeleteAndClose, &aRef));
		}
		
	static void RPtrArrayDeleteAndClose(TAny *aPtr)
		{
		T* array = reinterpret_cast< T* >(aPtr);
		array->ResetAndDestroy();
		array->Close();
		}
	};

template< typename T > void CleanupRPtrArrayDeleteAndCloseL(T& aRef)
	{
	TCleanupRPtrArrayDeleteAndClose< T >::PushL(aRef);
	}

#endif  //__T_CR_STEP_BASE__
