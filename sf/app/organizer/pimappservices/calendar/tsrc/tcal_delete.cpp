// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <calentry.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <calsession.h>
#include <e32test.h>
#include <caliterator.h>
#include <caluser.h>
#include <calrrule.h>
#include <calcategorymanager.h>
#include <calcategory.h>
#include <calinstance.h>

#include "caltestlib.h"

RTest test(_L("tcal_delete"));

_LIT(KCalendarFile,"tcal_delete");
_LIT(KCalendarFile2, "tcal_delete_calendar_copy");
_LIT(KOriginalDeleteCalendar, "z:\\private\\10003A5B\\tcal_delete_calendar");
_LIT8( KGUIDInc081869, "Guid081869" );

_LIT(KRecurrIdLocalTime, "20030500:000000.");   // June 1, 2003 00:00:00

// This number must be even and greater than the highest defined value in 
// KTidyCallbackRate, currently 24.  
// This would ensure the callback feature gets triggered for testing.
const TInt KNumberOfEntries = 26;

class CDummyCalendarApp;

// Progress call back utility class
class CProgressCallBack : public CBase, public MCalProgressCallBack
	{
public:
	void Progress(TInt aPercentageCompleted);	
	void Completed(TInt aError);	
	TBool NotifyProgress();
	};
	
void CProgressCallBack::Progress(TInt aPercentageCompleted)
	{
	test.Printf(_L("DeleteL is %d%% complete\n"), aPercentageCompleted);
	}
	
void CProgressCallBack::Completed(TInt aError)
	{
	test.Printf(_L("DeleteL has completed with error code %d\n"), aError);
	__ASSERT_ALWAYS(aError == KErrNone, User::Invariant());
	CActiveScheduler::Stop();
	}
	
TBool CProgressCallBack::NotifyProgress()
	{
	return ETrue;
	}
	
// a dummy test app that runs in a different thread
class CSecondThreadApp : public CBase
	{
public:
	static CSecondThreadApp* NewL();
	~CSecondThreadApp();
	
	void RunTestL();
	void TestFileLockL(TInt aError);
	
private:
	void ConstructL();
	CSecondThreadApp();

public:
	CCalTestLibrary* iCalTestLib;
	TInt iNumEntriesAdded;
	};
	
CSecondThreadApp* CSecondThreadApp::NewL()
	{
	CSecondThreadApp* self = new(ELeave) CSecondThreadApp;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CSecondThreadApp::ConstructL()
	{
	iCalTestLib = CCalTestLibrary::NewL();
	iCalTestLib->OpenFileL(KCalendarFile);
	}
	
CSecondThreadApp::CSecondThreadApp()
	{
	}
	
CSecondThreadApp::~CSecondThreadApp()
	{
	delete iCalTestLib;
	}
	
void CSecondThreadApp::RunTestL()
	{
	RThread thread;
	
	thread.Rendezvous(KErrNone);
	
	// wait for the other thread to call DeleteL
	thread.Suspend();
	
	// test that the file is locked
	TestFileLockL(KErrLocked);
	
	// wait for the DeleteL to finish
	thread.Suspend();
	
	// test that the file is now unlocked
	TestFileLockL(KErrNone);
	}
	
void CSecondThreadApp::TestFileLockL(TInt aError)
	{
	TCalTime from;
	from.SetTimeLocalL(TDateTime(2005, ESeptember, 11, 0, 0, 0, 0));
	TCalTime resultTime;
	
	// NextInstanceL
	RDebug::Printf("Test NextInstanceL");
	TInt error(KErrNone);
	TRAP(error, resultTime = iCalTestLib->SynCGetInstanceViewL().NextInstanceL(CalCommon::EIncludeAll, from));
	if (error != aError) 
		{
		User::Leave(KErrGeneral);
		}
	
	// PreviousInstanceL
	RDebug::Printf("Test PreviousInstanceL");
	error = KErrNone;
	TRAP(error, resultTime = iCalTestLib->SynCGetInstanceViewL().PreviousInstanceL(CalCommon::EIncludeAll, from));
	if (error != aError) 
		{
		User::Leave(KErrGeneral);
		}
	
	// GetIdsModifiedSinceDateL
	RDebug::Printf("Test GetIdsModifiedSinceDateL");
	RArray<TCalLocalUid> uids;
	CleanupClosePushL(uids);
	error = KErrNone;
	TRAP(error, iCalTestLib->SynCGetEntryViewL().GetIdsModifiedSinceDateL(from, uids));
	if (error != aError) 
		{
		User::Leave(KErrGeneral);
		}
	if (aError != KErrNone && uids.Count() != 0)
		{
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(&uids);
	
	// FindInstanceL
	RDebug::Printf("Test FindInstanceL");
	RPointerArray<CCalInstance> instanceList;
	CleanupResetAndDestroyPushL(instanceList);
	
	TCalTime start;
	start.SetTimeLocalL(TDateTime(2004, EMay, 12, 10, 30, 0, 0));
	TCalTime end;
	end.SetTimeLocalL(TDateTime(2007, EMay, 12, 10, 30, 0, 0));
	
	CalCommon::TCalTimeRange timeRange(start, end);
	
	error = KErrNone;
	TRAP(error, iCalTestLib->SynCGetInstanceViewL().FindInstanceL(instanceList, CalCommon::EIncludeAll, timeRange));
	if (error != aError)
		{
		User::Leave(KErrGeneral);
		}
	if (aError != KErrNone && instanceList.Count() != 0)
		{
		User::Leave(KErrGeneral);
		}
		
	CleanupStack::PopAndDestroy(&instanceList);
	
	// StoreL
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	HBufC8* guid = NULL;
	CCalEntry* entry = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guid);
	CleanupStack::PushL(entry);
	iCalTestLib->SetEntryStartAndEndTimeL(entry, TDateTime(2005, EMay, 23, 10, 30, 0, 0),
													TDateTime(2005, EMay, 23, 11, 0, 0, 0));
	entryArray.AppendL(entry);
	CleanupStack::Pop(entry);
	TInt numSuc;
	error = KErrNone;
	TRAP(error, iCalTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc));
	if (error != aError)
		{
		User::Leave(KErrGeneral);
		}
	
	// UpdateL
	error = KErrNone;
	TRAP(error, iCalTestLib->SynCGetEntryViewL().UpdateL(entryArray, numSuc));
	if (error != aError && error != KErrNotFound)
		{
		User::Leave(KErrGeneral);
		}
	// FetchL
	error = KErrNone;
	TRAP(error, iCalTestLib->SynCGetEntryViewL().FetchL(*guid, entryArray));
	if (error != aError)
		{
		User::Leave(KErrGeneral);
		}
		
	// DeleteL
	error = KErrNone;
	TRAP(error, iCalTestLib->SynCGetEntryViewL().DeleteL(*entry));
	if (error != aError)
		{
		User::Leave(KErrGeneral);
		}
	
	CleanupStack::PopAndDestroy(&entryArray);
	
	// CategoryManager
	CCalCategoryManager* categoryManager = CCalCategoryManager::NewL(iCalTestLib->GetSession());
	CleanupStack::PushL(categoryManager);
	
	CCalCategory* funCategory = CCalCategory::NewL(_L("Fun"));
	CleanupStack::PushL(funCategory);
	error = KErrNone;
	TRAP(error, categoryManager->AddCategoryL(*funCategory));
	if (error != aError)
		{
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(funCategory);
	
	error = KErrNone;
	TRAP(error, delete categoryManager->CategoryL(0));
	if (error != aError)
		{
		User::Leave(KErrGeneral);
		}
	
	CleanupStack::PopAndDestroy(categoryManager);
	
	// CCalIter
	CCalIter* calIter = CCalIter::NewL(iCalTestLib->GetSession());
	CleanupStack::PushL(calIter);
	
	error = KErrNone;
	TRAP(error, const TDesC8& guidRefFirst = calIter->FirstL());
	if (error != aError)
		{
		User::Leave(KErrGeneral);
		}
	
	error = KErrNone;
	TRAP(error, const TDesC8& guidRefNext = calIter->NextL());
	if (error != aError && error != KErrCorrupt)
		{
		User::Leave(KErrGeneral);
		}
		
	CleanupStack::PopAndDestroy(calIter);
	//	
	}
	
static void DoSecondThreadMainL()
	{
	CSecondThreadApp* secondThreadApp = CSecondThreadApp::NewL();
	CleanupStack::PushL(secondThreadApp);
	secondThreadApp->RunTestL();
	CleanupStack::PopAndDestroy(secondThreadApp);
	}
	
// The entry point for the other thread
static TInt SecondThreadMain(TAny* /*aRef*/)
	{
	__UHEAP_MARK;
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}

	CActiveScheduler* scheduler = new CActiveScheduler();
	if (!scheduler)
		{
		delete trapCleanup;
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);
	TRAPD(ret, DoSecondThreadMainL());
	delete scheduler;
	delete trapCleanup;		
	__UHEAP_MARKEND;
	return (ret);
	}
	

class CAppActive : public CActive
	{
public:
	enum TDummyAppStep
		{
		EStepOpenModel,
		ESimpleEntryDeleteAll,
		ESimpleEntryDeleteSome,
		EDeleteRestEntry,
		ERepeatEntryDelete,
		EDeleteAllTodos,
		EDeleteAndCheckProgress,
		ERepeatExceptedEntryCreate,
		ERepeatExceptedEntryFetchInstances,
		ERepeatExceptedEntryEditInstances,
		ERepeatExceptedEntryDelete,
		ELockedInOtherThreadDuringDelete,
		EDeleteRemiderAndMore,
		EDeleteEntryWithMinTime,
		EAddForDeleteBeforeDateWithException,
		EDeleteBeforeDateWithException,
		EDeleteAllBeforeDateWithException,
		EDeleteAllMemosBegin,
		EDeleteAllMemosMiddle,
		EDeleteAllMemosEnd,
		EEnd
		};

	CAppActive(CDummyCalendarApp& aApp);
	~CAppActive();
	void DoCancel();
	void RunL();
	void Start();
	TInt GetStep();
	void SetStep(TDummyAppStep aStep);


private:

	CDummyCalendarApp& iApp;
	TDummyAppStep iStep;
	RPointerArray<CCalInstance>	iInstances;
	};
	
class CDummyCalendarApp : public CCalTestLibrary
	{
public:
	
	enum TEntryType
		{
		EApp,
		ETodo,
		EReminder,
		EAll
		};
	
	static CDummyCalendarApp* NewLC();
	~CDummyCalendarApp();

	void Progress(TInt aPercentageCompleted);
	void Completed(TInt aError);
	TBool NotifyProgress();

	void Close();
	void OpenModelL();
	void AddAOAndStartL();

	void AddEntryL(TInt aNumToAdd, TBool isParent, TBool isRepeat);
	void StoreSimpleEntryL();
	void StoreRepeatEntryL();
	void DeleteEntryL(const TTime& aStart, const TTime& aEnd, CalCommon::TCalViewFilter aFilter);
	void DeleteAllTodosL();
	void DeleteAllMemosBeginL();
	void DeleteAllMemosEndL();
	TBool CheckEntryNumL(TEntryType aType, TInt aNumEntry);
	void DeleteWhileAnotherThreadAccessesCalendarL();
	void StoreManyEntriesL(TInt aCount);
	void RepeatExceptedEntryCreateL();
	void RepeatExceptedEntryFetchInstancesL(RPointerArray<CCalInstance>& aInstances);
	void RepeatExceptedEntryEditInstancesL(RPointerArray<CCalInstance>& aInstances);		

	TInt ProgressNotificationCount();
	void Add3ReminderAnd1ApptL();
	void AddFloatingEntryWithMinStartTimeL();
	
private:

	void ConstructL();


private: // member data

	CCalEntryView*           iLocalEntryView;// doesn't own
	RPointerArray<CCalEntry> iEntries;
	CAppActive*              iActive;
	TInt 					 iProgressNotifications;
	CCalTestLibrary* 			iTestLib;
	};


CDummyCalendarApp* CDummyCalendarApp::NewLC()
	{
	CDummyCalendarApp* self = new (ELeave) CDummyCalendarApp();
	
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return (self);
	}

	
CDummyCalendarApp::~CDummyCalendarApp()
	{
	delete iActive;
	delete iTestLib;
	iEntries.ResetAndDestroy();
	iEntries.Close();
	}


CAppActive::CAppActive(CDummyCalendarApp& aApp)
	: CActive(EPriorityHigh),iApp(aApp)
	{
	iStep = EStepOpenModel;
	}


CAppActive::~CAppActive()
	{
	iInstances.Close();
	Cancel();
	}


void CAppActive::Start()
	{
	if (!IsActive())
		{
		SetActive();
		}
				
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

	
void CAppActive::DoCancel()
	{
	iApp.Close();
	}
	
TInt CAppActive::GetStep()
	{
	return iStep;
	}

void CAppActive::SetStep(TDummyAppStep aStep)
	{
	iStep = aStep;
	}

void CDummyCalendarApp::ConstructL()
	{
	BaseConstructL();
	iActive = new (ELeave) CAppActive(*this);
	}

		
void CDummyCalendarApp::DeleteWhileAnotherThreadAccessesCalendarL()
	{
	test.Printf(_L("Adding 1000 entries"));
	
	const TInt KNumEntriesToAdd(1000);
	
	for (TInt i(0) ; i < KNumEntriesToAdd ; ++i)
		{
		HBufC8* guid = NULL;
		CCalEntry* entry = CreateCalEntryL(CCalEntry::EAppt, guid);
		CleanupStack::PushL(entry);
		
		SetEntryStartAndEndTimeL(entry,
									TDateTime(2005, ESeptember, 12, 0, 0, 0, 0),
									TDateTime(2005, ESeptember, 12, 1, 0, 0, 0));
		
		RPointerArray<CCalEntry> entryArray;
		CleanupClosePushL(entryArray);
		
		entryArray.AppendL(entry);
		TInt num;
		SynCGetEntryViewL().StoreL(entryArray, num);
		
		CleanupStack::PopAndDestroy(&entryArray);
		CleanupStack::PopAndDestroy(entry);
		
		test.Printf(_L("."));
		}
	
	// start the second thread and wait for it to signal
	// that it is ready to recieve notifications
	RThread thread;
	CleanupClosePushL(thread);
	User::LeaveIfError(thread.Create(_L("SecondThread"), SecondThreadMain,
															KDefaultStackSize,
															KDefaultStackSize * 20, // this is the minimum heap size
															KDefaultStackSize * 40, // this is the maximum heap size
															NULL));
	thread.SetPriority(EPriorityMore);
	thread.Resume();
	
	// wait untill the second thread is ready
	TRequestStatus status;
	thread.Rendezvous(status);
	User::WaitForRequest(status);
		
	TCalTime start;
	start.SetTimeLocalL(TDateTime(2005, ESeptember, 0, 0, 0, 0, 0));
	TCalTime end;
	end.SetTimeLocalL(TDateTime(2006, EJanuary, 0, 0, 0, 0, 0));
	
	CalCommon::TCalTimeRange timeRange(start, end);
	
	CProgressCallBack* progress = new(ELeave) CProgressCallBack;
	CleanupStack::PushL(progress);
	
	test.Printf(_L("Start DeleteL\n"));
	SynCGetEntryViewL().DeleteL(timeRange, CalCommon::EIncludeAll, *progress);
	thread.Resume();
	// signal to the other thread that we have called delete
	// and it should start testing that the file is locked
	CActiveScheduler::Start();
	thread.Resume();
	
	CleanupStack::PopAndDestroy(progress);
	
	// Wait for the other thread to exit
	// and test that the exit reason was KErrNone
	thread.Logon(status);
	User::WaitForRequest(status);
	
	// If the other thread's testing was unsuccessful
	// the exit reason will be KErrGeneral
	test(status.Int() == KErrNone);
	
	CleanupStack::PopAndDestroy(&thread);
	}

void CDummyCalendarApp::Progress(TInt aPercentageCompleted)
	{
	test.Printf(_L("MCalProgress callback - Percent complete=%d"), aPercentageCompleted);
	test.Printf( _L( "\n" ) );
	iProgressNotifications++;
	if(iActive->GetStep() == CAppActive::EDeleteAllMemosMiddle && aPercentageCompleted > 50)
		{
		CActiveScheduler::Stop();
		}
	}

	
void CDummyCalendarApp::Completed(TInt aError)
	{
	test.Printf(_L("MCalProgress callback - Completed, err=%d"), aError);	
	test.Printf( _L ( "\n") );
	iActive->Start();
	}


TBool CDummyCalendarApp::NotifyProgress()
	{
	return ETrue;
	}

	
void CDummyCalendarApp::OpenModelL()
	{
	ReplaceFileL(KCalendarFile);
	OpenFileL(KCalendarFile);
	iLocalEntryView = &AsynCGetEntryViewL();
	}


TBool CDummyCalendarApp::CheckEntryNumL(TEntryType aType, TInt aNumEntry)
	{
	CCalIter& iter = GetIterL();	
	
	TPtrC8 Uid(iter.FirstL());

	TInt ii=0;
	while (Uid!=KNullDesC8())
		{
		++ii;
		
		RPointerArray<CCalEntry> entryList;
		CleanupResetAndDestroyPushL(entryList);
		iLocalEntryView->FetchL(Uid, entryList);
		CCalEntry* entry = entryList[0];
		if(aType==EApp)
			{
			test(entry->EntryTypeL()==CCalEntry::EAppt);
			}
		else if (aType==ETodo)
			{
			test(entry->EntryTypeL()==CCalEntry::ETodo);
			}
		else if(aType == EReminder)
			{
			test(entry->EntryTypeL()==CCalEntry::EReminder);
			}
			
		CleanupStack::PopAndDestroy(&entryList);
	    
		Uid.Set(iter.NextL());
		}

	return (ii==aNumEntry);
	}


void CDummyCalendarApp::StoreSimpleEntryL()
	{
	AddEntryL(KNumberOfEntries, ETrue, EFalse);
	test(CheckEntryNumL(EAll,KNumberOfEntries));
	}


void CDummyCalendarApp::StoreRepeatEntryL()
	{
	AddEntryL(KNumberOfEntries, ETrue, ETrue);
	test(CheckEntryNumL(EAll,KNumberOfEntries));
	AddEntryL(KNumberOfEntries, EFalse, ETrue);
	test(CheckEntryNumL(EAll,KNumberOfEntries));
	}



void CDummyCalendarApp::AddEntryL(TInt aNumToAdd, TBool isParent, TBool isRepeat)
	{
	test.Next(_L("Adding entries"));

	TBuf<50> summary;
	TBuf<50> location;
	TBuf<50> description;

    

	iEntries.ResetAndDestroy();
	for (TInt index=0; index<aNumToAdd; index++)
		{
		TBuf8<255> buf;
		buf.Append(_L("GuidId"));
		buf.AppendNum(index);
		HBufC8* guid = buf.AllocLC();   // need to be pushed to ...

		CCalEntry::TType entryType=(index%2==0)?CCalEntry::ETodo:CCalEntry::EAppt;

		CCalEntry* entry = NULL;
		if(isParent)
			{
			entry = CreateCalEntryL(entryType, guid);
			}
		else
			{
			TTime localTime(KRecurrIdLocalTime);
			TCalTime recurrenceId;
			recurrenceId.SetTimeLocalL(localTime);
			entry = CCalEntry::NewL(entryType, guid, CCalEntry::EMethodAdd, 1, recurrenceId, CalCommon::EThisAndFuture);
			}
		CleanupStack::Pop(guid);
		iEntries.AppendL(entry);

		TInt year = -1;
		TInt month = -1;
		TInt day = -1;
		
		if (isParent)
		    {
    		year = index % 5 + 2001;    // Any year in the range: 2001 - 2005
    		month = index % 12;         
    		day = index % 28;
		    }
        else
            {
            // if this is a child entry, use the recurrence local time as the entry start time 
            // so it won't be out of range
            TCalTime recurrId = entry->RecurrenceIdL(); 
            TDateTime localTime = recurrId.TimeLocalL().DateTime();               
            year = localTime.Year();
            month = localTime.Month();
            day = localTime.Day();    
            }
		
		TTime start(TDateTime(year, (TMonth)month, day, 0, 0, 0, 0));
		TTime end(TDateTime(year, (TMonth)month, day, 0, 0, 0, 0));
		end += TTimeIntervalDays(1);

		SetEntryStartAndEndTimeL(entry,start,end);
				
		RandomText(summary);
		entry->SetSummaryL(summary);
		
		RandomText(location);
		entry->SetLocationL(location);
		
		RandomText(description);
		entry->SetDescriptionL(description);

		if(isRepeat)
			{
			//create a daily repeat rule and make sure its repeat start\end date is within the deleting time range
			TCalRRule rpt(TCalRRule::EDaily);
			rpt.SetInterval(1);
			TCalTime repeatStart;
			TCalTime repeatend;
			if(isParent)
				{
				//make sure the repeat time is within the delete time range
				repeatStart.SetTimeLocalL(TDateTime(2000, EJune, 0, 0, 0, 0, 0));
				repeatend.SetTimeLocalL(TDateTime(2006, EJune, 0, 0, 0, 0, 0));
				}
			else if (index<aNumToAdd/2)
				{
                // if this is a child entry, use the entry's recurrance time as repeating rule
                // start time 
                TCalTime recurrId = entry->RecurrenceIdL(); 
				
				// make sure the repeat time is within the delete time range for the first half entries
				repeatStart.SetTimeLocalL(recurrId.TimeLocalL());	// June 1, 2003 00:00:00
				repeatend.SetTimeLocalL(recurrId.TimeLocalL() + TTimeIntervalMonths(1));  // July 1, 2003 00:00:00
				}
			else
				{
				//make sure the repeat time is out of the delete time range for the second half entries
				repeatStart.SetTimeLocalL(TDateTime(2003, EJune, 0, 0, 0, 0, 0));
				repeatend.SetTimeLocalL(TDateTime(2007, EJune, 0, 0, 0, 0, 0));
				}

			rpt.SetDtStart(repeatStart); 
			rpt.SetUntil(repeatend);
			entry->SetRRuleL(rpt);
			}
		}
	
	TInt entriesStored(0);
	iLocalEntryView->StoreL(iEntries, entriesStored); //temp
	test(entriesStored == iEntries.Count());
	}


void CDummyCalendarApp::DeleteEntryL(const TTime& aStart,const TTime& aEnd, CalCommon::TCalViewFilter aFilter)
	{
	TCalTime startTime;
    TCalTime endTime;
    startTime.SetTimeLocalL(aStart);
    endTime.SetTimeLocalL(aEnd);
    CalCommon::TCalTimeRange range(startTime,endTime);
	iProgressNotifications = 0;
	iLocalEntryView->DeleteL(range, aFilter, *this); 
	}

void CDummyCalendarApp::DeleteAllTodosL()
	{
	delete iTestLib;
	iTestLib = NULL;
	
	TBuf<60> dtBuf;
	iTestLib = CCalTestLibrary::NewL();

	TRAPD(err, iTestLib->PIMTestServer().DeleteFileL(KCalendarFile2));
	if(err != KErrNotFound || err != KErrPathNotFound)
		{
		User::LeaveIfError(err);	
		}
	iTestLib->PIMTestServer().CopyFileL(KOriginalDeleteCalendar, _L("c:\\private\\10003A5B\\tcal_delete_calendar_copy"));
	iTestLib->RegisterCalFileL(KCalendarFile2);
	iTestLib->OpenFileL(KCalendarFile2);
	iTestLib->PIMTestServer().SetTimeZoneL(_L8("Europe/Helsinki"));
	
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
		
	//DeleteL has to be placed in the end of the function
	//because it is a long running operation and it needs
	//to wait for callback in order to go to the next step.
	iTestLib->SynCGetEntryViewL().DeleteL(timeRange, filter, *this);
	}

void CDummyCalendarApp::DeleteAllMemosBeginL()
	{
	test.Printf(_L("Test: DeleteAllMemosBeginL\r\n"));
	//Add 1000 entries
	test.Printf(_L("Adding 1000 Memos...\r\n"));
	
	const TInt KNumEntriesToAdd(1000);
	
	iTestLib->OpenFileL(KCalendarFile);
	
	for (TInt i(0) ; i < KNumEntriesToAdd ; ++i)
		{
		HBufC8* guid = NULL;
		CCalEntry* entry = iTestLib->CreateCalEntryL(CCalEntry::EReminder, guid);
		CleanupStack::PushL(entry);
		
		SetEntryStartAndEndTimeL(entry,
									TDateTime(2005, ESeptember, 12, 0, 0, 0, 0),
									TDateTime(2005, ESeptember, 12, 1, 0, 0, 0));
		
		RPointerArray<CCalEntry> entryArray;
		CleanupClosePushL(entryArray);
		
		entryArray.AppendL(entry);
		TInt num;
		iTestLib->SynCGetEntryViewL().StoreL(entryArray, num);
		
		CleanupStack::PopAndDestroy(&entryArray);
		CleanupStack::PopAndDestroy(entry);
		
		test.Printf(_L("."));
		}
		
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);
	
	iActive->SetStep(CAppActive::EDeleteAllMemosMiddle);
	
	//Attempt to delete all the entries
	test.Printf(_L("Deleting all the entries...\r\n"));
	iTestLib->SynCGetEntryViewL().DeleteL(timeRange, CCalEntry::EReminder, *this);
	CActiveScheduler::Start();
	delete iTestLib;
	iTestLib = NULL;
	}

void CDummyCalendarApp::DeleteAllMemosEndL()
	{
	test.Printf(_L("Test: DeleteAllMemosEndL\r\n"));
	test.Printf(_L("Interrupting the delete...\r\n"));
	
	//Get a time range
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);
	
	test.Printf(_L("Done!\r\n"));
	
	iTestLib = CCalTestLibrary::NewL(EFalse);
	iTestLib->OpenFileL(KCalendarFile);
	
	test.Printf(_L("Deleteing all the entiries again...\r\n"));
	iTestLib->SynCGetEntryViewL().DeleteL(timeRange, CCalEntry::EReminder, *this);
	}
void CDummyCalendarApp::StoreManyEntriesL(TInt aCount)
	{
	AddEntryL(aCount, ETrue, EFalse);
	test(CheckEntryNumL(EAll,aCount));
	}



void CDummyCalendarApp::RepeatExceptedEntryCreateL()
	{
	test.Next(_L("Add entry, repeat twice, set excepted and delete"));

	iEntries.ResetAndDestroy();	
	
	// Create new calendar entry.
	CCalEntry* entry = NULL;
	HBufC8* guid = KGUIDInc081869().AllocLC();	
	entry = CreateCalEntryL(CCalEntry::EAppt, guid);
	CleanupStack::Pop(guid);
	
	iEntries.AppendL(entry);	

    // Set start and end date.
	TTime start(TDateTime(2006, EMarch, 6, 10, 0, 0, 0));
	TTime end(TDateTime(2006, EMarch, 6, 14, 0, 0, 0));
	SetEntryStartAndEndTimeL(entry, start, end);
	
	TBuf<50> summary;			
	RandomText(summary);
	entry->SetSummaryL(summary);

	TBuf<50> location;
	RandomText(location);
	entry->SetLocationL(location);

	TBuf<50> description;		
	RandomText(description);
	entry->SetDescriptionL(description);	

	// Create a daily repeating rule that occurs for 2 days.
	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetInterval(1);
	rpt.SetCount(2);	
	
	// Make sure the repeat time is within the delete time range.
	TCalTime repeatStart;
	TCalTime repeatEnd;
	repeatStart.SetTimeLocalL(TDateTime(2006, EMarch, 6, 0, 0, 0, 0));
	repeatEnd.SetTimeLocalL(TDateTime(2006, EMarch, 8, 0, 0, 0, 0));
	rpt.SetDtStart(repeatStart); 
	rpt.SetUntil(repeatEnd);
	entry->SetRRuleL(rpt);

    // Store the entry. Because it repeats over 2 days, there will
    // be 2 entries.
	TInt entriesStored(0);
	SynCGetEntryViewL().StoreL(iEntries, entriesStored);
	test(entriesStored == iEntries.Count());
	}
	
void CDummyCalendarApp::RepeatExceptedEntryFetchInstancesL(
    RPointerArray<CCalInstance>& aInstances )
	{
	// Create a time range filter that returns all instances.
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	
	CleanupResetAndDestroyPushL(aInstances);	
	
	// Get all instances. 
	AsynCGetInstanceViewL().FindInstanceL(aInstances, filter, timeRange);
	test(aInstances.Count() >= 2);
	CleanupStack::Pop(&aInstances);
	}
 
void CDummyCalendarApp::RepeatExceptedEntryEditInstancesL(
    RPointerArray<CCalInstance>& aInstances )
	{
	CleanupResetAndDestroyPushL(aInstances);	
	
	// Delete all entries.
	iEntries.ResetAndDestroy();
	
	TTime start(TDateTime(2006, EMarch, 6, 10, 0, 0, 0));
	TTime end(TDateTime(2006, EMarch, 6, 14, 0, 0, 0));	
	
	// Re-create all the entries that previously existed.
	CCalEntry* entry = NULL;
	for (TUint i = 0; i < aInstances.Count(); ++i)
		{
		HBufC8* guid = KGUIDInc081869().AllocLC();	
		entry = CCalEntry::NewL( CCalEntry::EAppt, guid, CCalEntry::EMethodAdd,
		    i, aInstances[i]->Time(), CalCommon::EThisOnly );
		CleanupStack::Pop( guid );
		iEntries.AppendL( entry );
		
		SetEntryStartAndEndTimeL(entry, start, end);		
		}
	
	CleanupStack::PopAndDestroy(&aInstances);
	
	// Store the entries again, there should be 2.
	TInt entriesStored = 0;
	SynCGetEntryViewL().StoreL(iEntries, entriesStored);
	test(entriesStored == iEntries.Count());
	}

TInt CDummyCalendarApp::ProgressNotificationCount()
	{
	return iProgressNotifications;
	}
	


void CDummyCalendarApp::Add3ReminderAnd1ApptL()
	{
	test.Next(_L("Create some entries including day note and delete them"));


//Create entries	
	iEntries.ResetAndDestroy();
	//create reminder 1
	TBuf8<255> buf;
	buf.Append(_L("GuidId_Reminder_1"));
	HBufC8* guid = buf.AllocLC();
	CCalEntry* reminder1 = CCalEntry::NewL(CCalEntry::EReminder, guid, CCalEntry::EMethodAdd, 0);
	CleanupStack::Pop(guid);
	iEntries.AppendL(reminder1);

	TCalTime starttime;
	starttime.SetTimeLocalL(TDateTime(2006, EMarch, 5, 0, 0, 0, 0));
	reminder1->SetStartAndEndTimeL(starttime, starttime);

	TBuf<50> summary;
	RandomText(summary);
	reminder1->SetSummaryL(summary);

	//create reminder 2
	buf.Delete(buf.Length()-1,1);
	buf.Append(2);
	guid = buf.AllocLC();
	CCalEntry* reminder2 = CCalEntry::NewL(CCalEntry::EReminder, guid, CCalEntry::EMethodAdd, 0);
	CleanupStack::Pop(guid);
	iEntries.AppendL(reminder2);
	starttime.SetTimeLocalL(TDateTime(2006, EMarch, 6, 0, 0, 0, 0));
	reminder2->SetStartAndEndTimeL(starttime, starttime);
	RandomText(summary);
	reminder2->SetSummaryL(summary);

	//create reminder 3
	buf.Delete(buf.Length()-1,1);
	buf.Append(3);
	guid = buf.AllocLC();
	CCalEntry* reminder3 = CCalEntry::NewL(CCalEntry::EReminder, guid, CCalEntry::EMethodAdd, 0);
	CleanupStack::Pop(guid);
	iEntries.AppendL(reminder3);
	starttime.SetTimeLocalL(TDateTime(2006, EMarch, 7, 0, 0, 0, 0));
	reminder3->SetStartAndEndTimeL(starttime, starttime);
	RandomText(summary);
	reminder3->SetSummaryL(summary);
	
	//Create appt
	buf.Zero();
	buf.Append(_L("GuidId_Appt"));
	guid = buf.AllocLC();
	CCalEntry* appt = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodAdd, 0);
	CleanupStack::Pop(guid);
	iEntries.AppendL(appt);
	
	starttime.SetTimeLocalL(TDateTime(2006, EMarch, 7, 14, 0, 0, 0));
	TTime endtimetime(TDateTime(2006, EMarch, 7, 14, 0, 0, 0));
	endtimetime +=TTimeIntervalHours(1);
	TCalTime endtime;
	endtime.SetTimeLocalL(endtimetime);
	appt->SetStartAndEndTimeL(starttime, endtime);

	RandomText(summary);
	appt->SetSummaryL(summary);

//store 4 entries
	TInt entriesStored(0);
	SynCGetEntryViewL().StoreL(iEntries, entriesStored); 
	test(entriesStored == 4);
	iEntries.ResetAndDestroy();
	}
	
	
void CDummyCalendarApp::AddFloatingEntryWithMinStartTimeL()
	{//Defect fix for PDEF111562  - when a floating entry with miniman start time will never get deleted.
	iEntries.ResetAndDestroy();

	_LIT8(KUid, "UID_MinEntry");
	HBufC8* guid = KUid().AllocLC();
	CCalEntry* entry = iTestLib->CreateCalEntryL(CCalEntry::EReminder, guid);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	TCalTime caltimestart,caltimeend;
	TTime min(TDateTime(1900, EJanuary, 0, 0, 0, 0, 0));
	caltimestart.SetTimeLocalFloatingL(min);
	entry->SetStartAndEndTimeL(caltimestart,caltimestart);
	iEntries.AppendL(entry);
	CleanupStack::Pop(entry);
	TInt entriesStored(0);
	SynCGetEntryViewL().StoreL(iEntries, entriesStored); //temp
	iEntries.ResetAndDestroy();	
	}
	
void CAppActive::RunL()
	{
	TTime start(TDateTime(2000, EJanuary, 0, 0, 0, 0, 0));
	TTime end(TDateTime(2006, EDecember, 0, 0, 0, 0, 0));
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	switch (iStep)
		{
		case EStepOpenModel:
			iApp.OpenModelL();
			iStep = ESimpleEntryDeleteAll;
			break;
		
		case ESimpleEntryDeleteAll:
			// Create half todos and half appointments and store the entries 
			// to the file.
			iApp.StoreSimpleEntryL();
			iApp.DeleteEntryL(start, end, filter);
			iStep = ESimpleEntryDeleteSome;
			break;
			
		case ESimpleEntryDeleteSome:
			// Check whether all entries have been deleted.
			test(iApp.CheckEntryNumL(CDummyCalendarApp::EAll,0));
			
			// Store entry back to the file.
			iApp.StoreSimpleEntryL();
			
			// Delete only Appointments from the file.
			filter = CalCommon::EIncludeAppts;
			iApp.DeleteEntryL(start, end, filter);
			iStep = EDeleteRestEntry;
			break;
		
		case EDeleteRestEntry:
			//Check only Todos are left in file .
			test(iApp.CheckEntryNumL(CDummyCalendarApp::ETodo,KNumberOfEntries/2));
			filter = CalCommon::EIncludeAll;
			
			//Delete the rest of entries
			iApp.DeleteEntryL(start, end, filter);
			iStep = ERepeatEntryDelete;
			break;
		
		case ERepeatEntryDelete:
			// Check all entries are deleted.
			test(iApp.CheckEntryNumL(CDummyCalendarApp::EAll,0));
			
			// Store KNumberOfEntries mumber of parent entries 
			// and KNumberOfEntries mumber of children entries
			// half of children entries are within the delete time range
			// The other half are not
			iApp.StoreRepeatEntryL();
			
			// Delete entries
			iApp.DeleteEntryL(start, end, filter);
			iStep = EDeleteAndCheckProgress;
			break;
		
		case EDeleteAndCheckProgress:
			// Check parent and its children entries have been deleted only if 
			// all of them are within the time range are not deleted.
			test( iApp.CheckEntryNumL( CDummyCalendarApp::EAll, KNumberOfEntries/2 ) );
			
			// Add enough entries to force Progress Notifications during delete
			iApp.StoreManyEntriesL(50);
			
			// Delete all the entries
			iApp.DeleteEntryL(TCalTime::MinTime(), TCalTime::MaxTime(), filter);
			iStep = EDeleteAllTodos;
			break;

		case EDeleteAllTodos:
			// Check that we received some progress notifications.
			test( iApp.ProgressNotificationCount() > 0 );
			
			// Check all entries are deleted
			test(iApp.CheckEntryNumL(CDummyCalendarApp::EAll,0));
			//Delete a thousand entries contained in the calendar file.
			iApp.DeleteAllTodosL();
			iStep = ERepeatExceptedEntryCreate;
			break;

		case ERepeatExceptedEntryCreate:			
			// Check all entries are deleted.
			test( iApp.CheckEntryNumL( CDummyCalendarApp::EAll, 0 ) );
		
			// Make sure the databse is clean for the next test case.
			iApp.CleanDatabaseL();
			
			// Create the entry with repeating rule. There will be 2 entries
			// after this.
			iApp.RepeatExceptedEntryCreateL();
			
			// Being an active object, we need to kick it for the next loop.
			Start();
   			iStep = ERepeatExceptedEntryFetchInstances;
			break;
			
		case ERepeatExceptedEntryFetchInstances:
			// Fetch the 2 entries for modification. 
			iApp.RepeatExceptedEntryFetchInstancesL( iInstances );
			iStep = ERepeatExceptedEntryEditInstances;
			break;
				
		case ERepeatExceptedEntryEditInstances:
			// Modify the entries and commit.
			iApp.RepeatExceptedEntryEditInstancesL( iInstances );
			Start();
			iStep = ERepeatExceptedEntryDelete;
			break;
			
        case ERepeatExceptedEntryDelete:
            // Delete all the entries.
            iApp.DeleteEntryL( TCalTime::MinTime(), TCalTime::MaxTime(), CalCommon::EIncludeAll );
   			iStep = EDeleteRemiderAndMore;
			break;

		case EDeleteRemiderAndMore:
			//Check all entries are deleted.
			test(iApp.CheckEntryNumL(CDummyCalendarApp::EAll,0));
			
			iApp.Add3ReminderAnd1ApptL();
			end = TDateTime(2006, EMarch, 7, 0, 0, 0, 0);
			iApp.DeleteEntryL(TCalTime::MinTime(), end, filter);
			iStep = ELockedInOtherThreadDuringDelete;
			break;
		case ELockedInOtherThreadDuringDelete:
			iApp.DeleteWhileAnotherThreadAccessesCalendarL();
			iStep = EDeleteEntryWithMinTime;
			Start();
 			break;
 			
		case EDeleteEntryWithMinTime:
			test(iApp.CheckEntryNumL(CDummyCalendarApp::EApp, 1));
			_LIT8(KHong_Kong, "Asia/Hong_Kong");
			iApp.SetTimeZoneL(KHong_Kong);
			iApp.AddFloatingEntryWithMinStartTimeL();
			iApp.DeleteEntryL(TCalTime::MinTime(), TCalTime::MaxTime(), filter);
			iStep = EAddForDeleteBeforeDateWithException;
			break;

		case EAddForDeleteBeforeDateWithException:
			// Make sure the databse is clean for the next test case.
			iApp.CleanDatabaseL();
			// Add a repeat entry; startdate 6 March 2006 - enddate 8 March 2006
			iApp.RepeatExceptedEntryCreateL();
			Start();
			iStep = EDeleteBeforeDateWithException;
			break;

		case EDeleteBeforeDateWithException:
			{
			TTime startSingleDelete(TDateTime(2006, EMarch, 8, 0, 0, 0, 0));
			TTime endSingleDelete(TDateTime(2006, EMarch, 8, 23, 59, 0, 0));
			// Delete last instance
			iApp.DeleteEntryL(startSingleDelete, endSingleDelete, filter);
			iStep = EDeleteAllBeforeDateWithException;
			}
			break;

		case EDeleteAllBeforeDateWithException:
			{
			// Delete everything before 8th March 2006
			TTime startBeforeDelete(TCalTime::MinTime());
			TTime endBeforeDelete(TDateTime(TDateTime(2006, EMarch, 8, 23, 59, 0, 0)));//2006, EJune, 1, 1, 0, 0, 0));
			iApp.DeleteEntryL(startBeforeDelete, endBeforeDelete, filter);
			iStep = EDeleteAllMemosBegin;
			}
			break;
		case EDeleteAllMemosBegin:
			{
			iApp.CleanDatabaseL();
			iApp.DeleteAllMemosBeginL();
			iStep = EDeleteAllMemosEnd;
			}
			break;
		case EDeleteAllMemosEnd:
			{
			iApp.DeleteAllMemosEndL();
 			iStep = EEnd;
			}
 			break;
		
		case EEnd:
			test(iApp.CheckEntryNumL(CDummyCalendarApp::EAll, 0));
			iApp.SetTimeZoneL(KDefaultTimeZone);
			iApp.Close();
			break;
			
		default:
			break;
		}
	}


void CDummyCalendarApp::Close()
	{
	CActiveScheduler::Stop();
	}


void CDummyCalendarApp::AddAOAndStartL()
	{
	CActiveScheduler::Add(iActive);
	iActive->Start();
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CDummyCalendarApp* testManager = CDummyCalendarApp::NewLC();


	TPerformanceTimer timer(test);
	timer.Start();	


	// Run the test suite

	testManager->AddAOAndStartL();
	CActiveScheduler::Start();
	

	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();

	
	CleanupStack::PopAndDestroy(testManager);
	}


/**

@SYMTestCaseID     PIM-TCAL-DELETE-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-DELETE-0001 Calendar Interim API Delete test suite"));

	test.Title();

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}

	CActiveScheduler* scheduler = new CActiveScheduler();
	if (!scheduler)
		{
		delete trapCleanup;
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);	

	TRAPD(ret, DoTestL());
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return (KErrNone);
    }
