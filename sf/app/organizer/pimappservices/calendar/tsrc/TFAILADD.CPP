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

#include <s32file.h>
#include <conpics.h>
#include <e32test.h>

#include <calrrule.h> 
#include <calentryview.h>

#include "caltestlib.h"

RTest test(_L("TFAILADD"));

static const TInt KMyError(-12345);

_LIT(KTestCalendarFile, "tfailadd");

class CTestApp : public CBase
	{
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void FillEntryL(CCalEntry& aEntry);
	void TestFailL(CCalEntry::TType aEntryType);
	
	void StoreEntryL(const CCalEntry* aEntry);
	
private:
	void ConstructL();	

private:
	CCalTestLibrary* iCalTestLib;
	
	};
	
CTestApp* CTestApp::NewL()
	{
	CTestApp* self = new(ELeave) CTestApp;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CTestApp::ConstructL()
	{
	iCalTestLib = CCalTestLibrary::NewL();
	}
		
CTestApp::~CTestApp()
	{
	delete iCalTestLib;
	}
	
void CTestApp::StoreEntryL(const CCalEntry* aEntry)
	{
	iCalTestLib->ReplaceFileL(KTestCalendarFile);
	iCalTestLib->OpenFileL(KTestCalendarFile);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	entryArray.AppendL(aEntry);
	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	test(numSuc == entryArray.Count());
	CleanupStack::PopAndDestroy(&entryArray);
	}

void CTestApp::FillEntryL(CCalEntry& aEntry)
	{
	switch (aEntry.EntryTypeL())
		{
		case CCalEntry::EAppt:
			{
			aEntry.SetSummaryL(_L("appt"));

			// the appt spans midnight
			iCalTestLib->SetEntryStartAndEndTimeL(&aEntry, TTime(TDateTime(1995,EJanuary,0,20,15,0,0)), TTime(TDateTime(1995,EJanuary,1,5,15,0,0)) );

			// it repeats every 2 days and has and exception
			TCalRRule rRuleAppt(TCalRRule::EDaily);
			TCalTime dtStartAppt;
			dtStartAppt.SetTimeLocalL(TTime(TDateTime(1995,EJanuary,0,0,0,0,0)));
			TCalTime untilAppt;
			untilAppt.SetTimeLocalL(TTime(TDateTime(1995,EJanuary,9,0,0,0,0)));
			rRuleAppt.SetDtStart(dtStartAppt);
			rRuleAppt.SetUntil(untilAppt);
			rRuleAppt.SetInterval(2);
			aEntry.SetRRuleL(rRuleAppt);
			
			TCalTime exception;
			exception.SetTimeLocalL(TTime(TDateTime(1995,EJanuary,4,0,0,0,0)));
			RArray<TCalTime> exceptions;
			CleanupClosePushL(exceptions);
			exceptions.AppendL(exception);
			aEntry.SetExceptionDatesL(exceptions);
			CleanupStack::PopAndDestroy(&exceptions);
			break;
			}
	case CCalEntry::EEvent:
			{
			aEntry.SetSummaryL(_L("event"));
			iCalTestLib->SetEntryStartAndEndTimeL(&aEntry, TTime(TDateTime(1995,EJanuary,0,0,0,0,0)), TTime(TDateTime(1995,EJanuary,0,0,0,0,0)));
			break;
			}
	case CCalEntry::EAnniv:
			{
			aEntry.SetSummaryL(_L("anniv"));
			iCalTestLib->SetEntryStartAndEndTimeL(&aEntry, TTime(TDateTime(1995,EJanuary,0,0,0,0,0)), TTime(TDateTime(1995,EJanuary,0,0,0,0,0)));  // !!! WHAT'S THIS FOR ???
			
			TCalRRule rRuleAnniv(TCalRRule::EYearly);
			TCalTime dtStartAnniv;
			dtStartAnniv.SetTimeLocalL(TTime(TDateTime(1994,EJanuary,0,0,0,0,0)));
			TCalTime untilAnniv;
			untilAnniv.SetTimeLocalL(TCalTime::MaxTime());
			rRuleAnniv.SetDtStart(dtStartAnniv);
			rRuleAnniv.SetUntil(untilAnniv);
			rRuleAnniv.SetInterval(1);
			aEntry.SetRRuleL(rRuleAnniv);
			break;
			}
	case CCalEntry::ETodo:
			{
			aEntry.SetSummaryL(_L("todo"));
			aEntry.SetPriorityL(1);
			break;
			}
		}
	}

void CTestApp::TestFailL(CCalEntry::TType aEntryType)
	{
	TInt failAt(0);

	FOREVER
		{
		//When session is disconnected, it loses information present in CAgnTlsProxy
		//If not connected, It would cause panic when other clientside APIs try to make us of the CAgnTlsProxy object.
		//This call below makes sure that such a case does not affect this tests goal.
		iCalTestLib->ConnectAgendaServerL();

		HBufC8* guid = NULL;
		CCalEntry* entry = iCalTestLib->CreateCalEntryL(aEntryType, guid);
		CleanupStack::PushL(entry);
		FillEntryL(*entry);
		
		iCalTestLib->FileSession().SetErrorCondition(KMyError, ++failAt);
		TRAPD(error, StoreEntryL(entry));
		iCalTestLib->FileSession().SetErrorCondition(KErrNone);
		
		if (error == KErrNone)
			{
			// the store was successful
			CleanupStack::PopAndDestroy(entry);
			break;
			}
		else
			{
			// if the store failed it should fail with our error code
			test(error == KMyError);
			}	
		CleanupStack::PopAndDestroy(entry);
		}
	}



static void doMainL()
	{
#ifndef _DEBUG

	test.Next(_L("TFAILADD does not run on UREL builds. Test not run."));

#else
	CTestApp* testApp = CTestApp::NewL();
	CleanupStack::PushL(testApp);
	
	test.Next(_L("Test1 - Add Appointment"));

	testApp->TestFailL(CCalEntry::EAppt);

	test.Next(_L("Test2 - Add Event"));

	testApp->TestFailL(CCalEntry::EEvent);
		
	test.Next(_L("Test3 - Add Anniversary"));

	testApp->TestFailL(CCalEntry::EAnniv);

	test.Next(_L("Test4 - Add Todo"));

	testApp->TestFailL(CCalEntry::ETodo);
	
	CleanupStack::PopAndDestroy(testApp);
#endif
	}

/**

@SYMTestCaseID     PIM-TFAILADD-0001

*/

TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-TFAILADD-0001 TFAILADD.CPP"));

	test.Title();
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	TRAPD(ret,doMainL());	
	delete scheduler;
	test(ret==KErrNone);
	delete theCleanup;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

