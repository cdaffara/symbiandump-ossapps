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

#include "caltestlib.h"
#include <e32test.h>
#include <caluser.h>
#include <calsession.h>
#include <calentry.h>

static RTest test(_L("tcal_attendee"));

_LIT8(KGUID1, "A_Global_UID1");
_LIT8(KGUID2, "A_Global_UID2");
_LIT8(KGUID3, "A_Global_UID3");
_LIT(KCalName, "TestCalendar");

_LIT(KAttendee1Address, "attendee.one@example.com");
_LIT(KAttendee2Address, "attendee.two@example.com");
_LIT(KAttendee3Address, "attendee.three@example.com");
_LIT(KAttendee4Address, "attendee.four@example.com");
_LIT(KAttendee2SentBy, "sent.by@example.com");
_LIT(KAttendee1CommonName, "attendee1");
_LIT(KAttendee3CommonNameA, "attendee3a");
_LIT(KAttendee3CommonNameB, "attendee3b");
_LIT(KOrganizerCommonName, "example_organizer");
_LIT(KOrganizerSentByA, "organizer.sentby@example.com");
_LIT(KOrganizerAddressA, "organizer.a@example.com");
_LIT(KOrganizerAddressB, "organizer.b@example.com");
_LIT(KAttendee11Address, "attendee.representative@example.com");
_LIT(KAttendee12Address, "attendee.twelve@example.com");
_LIT(KAttendee13Address, "attendee.thirteen@example.com");
_LIT(KAttendee14Address, "attendee.fourteen@example.com");
_LIT(KAttendee12SentBy,  "attendee.sentby@example.com");
_LIT(KOrganizerCommonName1, "another_organizer");


class CAttendeeTestManager : public CBase
	{
public:
    static CAttendeeTestManager* NewLC();
	~CAttendeeTestManager();	
	void CreateAndPopulateEntryL();
	void TestOrganizerL();
	void TestAttendeesL();
	void TestPhoneOwnerL();
	CCalEntry* Entry();
	void CompareEntriesL();
	void TestCopyFromL();
private:
	void ConstructL();
	void PrintAttendeeL(CCalAttendee* aAttendee);	
	void TestStatusValuesL(CCalAttendee* aAttendee);
	void TestRoleValuesL(CCalAttendee* aAttendee);
private:
	CCalTestLibrary* iTestLib;
	CCalEntry* iEntry;
	TCalTime iCalTime1;
	TCalTime iCalTime2;
	};


CAttendeeTestManager* CAttendeeTestManager::NewLC()
	{
	CAttendeeTestManager* self = new (ELeave) CAttendeeTestManager();
	
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return (self);
	}

	
void CAttendeeTestManager::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL();
	iTestLib->ReplaceFileL(KCalName);
	iTestLib->OpenFileL(KCalName);	
	}	

	
CAttendeeTestManager::~CAttendeeTestManager()	
	{
	delete iTestLib;
	delete iEntry;
	}


void CAttendeeTestManager::PrintAttendeeL(CCalAttendee* aAttendee)
	{
	TPtrC ptrAddress(aAttendee->Address());
	TPtrC ptrSentBy(aAttendee->SentBy());
	TPtrC ptrCommonName(aAttendee->CommonName());
	TInt status = aAttendee->StatusL();
	TInt role = aAttendee->RoleL();
	TBool rsvp = aAttendee->ResponseRequested();		
		
	test.Printf(_L("~~:ATTENDEE DETAILS:~~\n"));
	test.Printf(_L("Address:\t%S\n"), &ptrAddress);
	test.Printf(_L("Sent-By:\t%S\n"), &ptrSentBy);
	test.Printf(_L("Common Name:\t%S\n"), &ptrCommonName);
	test.Printf(_L("RSVP:\t%d\n"), rsvp);
	test.Printf(_L("CalStatus:\t%d\n"), status);
	test.Printf(_L("CalRole:\t%d\n"), role);
	}


void CAttendeeTestManager::TestRoleValuesL(CCalAttendee* aAttendee)
	{
	test.Next(_L("Testing Role Values"));

	aAttendee->SetRoleL(CCalAttendee::ENonParticipant); 
	test((aAttendee->RoleL() == CCalAttendee::ENonParticipant));		
	aAttendee->SetRoleL(CCalAttendee::EReqParticipant); 
	test((aAttendee->RoleL() == CCalAttendee::EReqParticipant));			
	aAttendee->SetRoleL(CCalAttendee::EOptParticipant); 
	test((aAttendee->RoleL() == CCalAttendee::EOptParticipant));			
	aAttendee->SetRoleL(CCalAttendee::EChair); 
	test((aAttendee->RoleL() == CCalAttendee::EChair));		
	}

		

void CAttendeeTestManager::TestStatusValuesL(CCalAttendee* aAttendee)
	{
	test.Next(_L("Testing Status Values"));

	aAttendee->SetStatusL(CCalAttendee::EInProcess);
	test((aAttendee->StatusL() == CCalAttendee::EInProcess));		
	aAttendee->SetStatusL(CCalAttendee::ENeedsAction);
	test((aAttendee->StatusL() == CCalAttendee::ENeedsAction));	
	aAttendee->SetStatusL(CCalAttendee::EAccepted);
	test((aAttendee->StatusL() == CCalAttendee::EAccepted));	
	aAttendee->SetStatusL(CCalAttendee::ETentative);
	test((aAttendee->StatusL() == CCalAttendee::ETentative));	
	aAttendee->SetStatusL(CCalAttendee::EConfirmed);
	test((aAttendee->StatusL() == CCalAttendee::EConfirmed));	
	aAttendee->SetStatusL(CCalAttendee::EDeclined);
	test((aAttendee->StatusL() == CCalAttendee::EDeclined));
	aAttendee->SetStatusL(CCalAttendee::ECompleted);
	test((aAttendee->StatusL() == CCalAttendee::ECompleted));	
	aAttendee->SetStatusL(CCalAttendee::EDelegated);
	test((aAttendee->StatusL() == CCalAttendee::EDelegated));	
	}


void CAttendeeTestManager::CreateAndPopulateEntryL()
	{
	//	set up entry
	TTime time1;
	time1.HomeTime();
	TTime time2(time1);
	time2 += TTimeIntervalMonths(30);	
	
	HBufC8* guid = KGUID1().AllocLC();

	iEntry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	
	
	iCalTime1.SetTimeUtcL(time1);
	iCalTime2.SetTimeUtcL(time2);
	iEntry->SetStartAndEndTimeL(iCalTime1, iCalTime2);
	
	test.Next(_L("Create and add a list of attendees to an entry"));


	CCalAttendee* attendee1 = CCalAttendee::NewL(KAttendee1Address);
	CleanupStack::PushL(attendee1);
	
	iEntry->AddAttendeeL(attendee1);	
	CleanupStack::Pop(attendee1);
	
	test.Printf(attendee1->SentBy());
	test.Printf(_L("\n"));
	test(attendee1->SentBy() == KNullDesC);

	CCalAttendee* attendee2 = CCalAttendee::NewL(KAttendee2Address, KAttendee2SentBy);
	iEntry->AddAttendeeL(attendee2);	
	test(attendee2->SentBy() != KNullDesC);

	CCalAttendee* attendee3 = CCalAttendee::NewL(KAttendee3Address);
	iEntry->AddAttendeeL(attendee3);	
	test.Printf(attendee3->SentBy());
	test.Printf(_L("\n"));

	CCalAttendee* attendee4 = CCalAttendee::NewL(KAttendee4Address);
	CleanupStack::PushL(attendee4);
	attendee4->SetResponseRequested(ETrue);
	test(attendee4->ResponseRequested());
	CleanupStack::Pop(attendee4);	
	iEntry->AddAttendeeL(attendee4);	
	
	test(iEntry->AttendeesL().Count() == 4);
	}


void CAttendeeTestManager::TestOrganizerL()
	{
	test.Next(_L("Testing that an organizer can be created and added to an entry."));

	
	CCalUser* organiserA = CCalUser::NewL(KOrganizerAddressA, KOrganizerSentByA);
	iEntry->SetOrganizerL(organiserA);
	
	CCalUser* organiserB = CCalUser::NewL(KOrganizerAddressB);
	CleanupStack::PushL(organiserB);
	
	test(organiserA->SentBy() != KNullDesC);
	test(iEntry->OrganizerL()->SentBy() == KOrganizerSentByA);
	test(organiserB->SentBy() == KNullDesC);
	organiserB->SetCommonNameL(KOrganizerCommonName);	
	CleanupStack::Pop(organiserB);
	iEntry->SetOrganizerL(organiserB);
	
	CCalUser* organiser = NULL;
	organiser = iEntry->OrganizerL();
	
	TPtrC ptrAddress(organiser->Address());
	TPtrC ptrSentBy(organiser->SentBy());
	TPtrC ptrCommonName(organiser->CommonName());

	test(organiser->Address() == KOrganizerAddressB);
	test(organiser->SentBy() == KNullDesC);
	test(organiser->CommonName() == KOrganizerCommonName);
	
	test.Printf(_L("ORGANISER details:"));
	test.Printf(_L("Address:\t%S\n"), &ptrAddress);
	test.Printf(_L("Sent-By:\t%S\n"), &ptrSentBy);
	test.Printf(_L("Common Name:\t%S\n"), &ptrCommonName);	
	}


void CAttendeeTestManager::TestAttendeesL()
	{
	RPointerArray<CCalAttendee> attendeeList = iEntry->AttendeesL();
	CleanupClosePushL(attendeeList);

	TInt count = attendeeList.Count();

	test.Printf(_L("Printing All Attendees before changes\n"));
	
	for(TInt i=0 ; i < count; ++i)
		{
		PrintAttendeeL(attendeeList[i]);
		}		
	
	test.Printf(_L("Setting common names\n"));
	attendeeList[0]->SetCommonNameL(KAttendee1CommonName);
	test(attendeeList[0]->CommonName() == KAttendee1CommonName);
	attendeeList[2]->SetCommonNameL(KAttendee3CommonNameA);
	test(attendeeList[2]->CommonName() == KAttendee3CommonNameA);
	attendeeList[2]->SetCommonNameL(KAttendee3CommonNameB); // checking that you can change common name once set
	test(attendeeList[2]->CommonName() == KAttendee3CommonNameB);	
		
	//	testing the setting of status/role values
	TestRoleValuesL(attendeeList[1]);
	TestStatusValuesL(attendeeList[3]);

	//	testing invalid role and status values
	TInt fakeEnumValue = 20;
	CCalAttendee::TCalRole fakeRole;
	CCalAttendee::TCalStatus fakeStatus;
	
	fakeRole = static_cast<CCalAttendee::TCalRole>(fakeEnumValue);
	fakeStatus = static_cast<CCalAttendee::TCalStatus>(fakeEnumValue);
	
	TRAPD(err, attendeeList[1]->SetRoleL(fakeRole));
	test(err == KErrNotFound);
	err = 0;
	TRAP(err, attendeeList[1]->SetStatusL(fakeStatus));		
	test(err == KErrNotFound);
	
	CleanupStack::Pop(&attendeeList);
	}


void CAttendeeTestManager::TestPhoneOwnerL()
	{
	RPointerArray<CCalAttendee> attendeeList = iEntry->AttendeesL();
	CleanupClosePushL(attendeeList);
	
	iEntry->SetPhoneOwnerL(iEntry->OrganizerL());
	test.Printf(_L("Testing phone owner is example organizer\n"));
	PrintAttendeeL(static_cast<CCalAttendee*>(iEntry->PhoneOwnerL()));
	test(iEntry->PhoneOwnerL()->CommonName() == KOrganizerCommonName);
	test(iEntry->PhoneOwnerL()->Address() == KOrganizerAddressB);
	
	
	iEntry->SetPhoneOwnerL(attendeeList[2]);
	test.Printf(_L("Testing phone owner is attendee three\n"));
	PrintAttendeeL(static_cast<CCalAttendee*>(iEntry->PhoneOwnerL()));
	test(iEntry->PhoneOwnerL()->Address() == KAttendee3Address);
	test(iEntry->PhoneOwnerL()->CommonName() == KAttendee3CommonNameB);

	attendeeList[2]->SetCommonNameL(_L("A random common name"));
	PrintAttendeeL(static_cast<CCalAttendee*>(iEntry->PhoneOwnerL()));
	test(iEntry->PhoneOwnerL()->CommonName() == _L("A random common name"));
	
	//	delete the attendee that is set as the phone owner
	iEntry->DeleteAttendeeL(2);
	
	if(iEntry->PhoneOwnerL())
		{
		test.Printf(_L("Phone owner deleted so it should not come in here!\n"));
		PrintAttendeeL(static_cast<CCalAttendee*>(iEntry->PhoneOwnerL()));
		test(0);	// panic
		}
	else
		{
		test.Printf(_L("Phone Owner No longer exists!\n"));
		}
	test.Printf(_L("ensuring organizer NICK HANLING is still there\n"));
	PrintAttendeeL(static_cast<CCalAttendee*>(iEntry->OrganizerL()));

	test.Printf(_L("Cleaning up the objects and finishing test!\n"));
	
	CleanupStack::Pop(&attendeeList);
	}


//Test code added for Defect DEF063673

void CAttendeeTestManager::CompareEntriesL()
	{
	test.Next(_L("Compare two entries"));

	
	//	set up entry
	HBufC8* guid = KGUID2().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	entry->SetStartAndEndTimeL(iCalTime1, iCalTime2);
	
	test.Printf(_L("Create a list of Attendees and an Organiser\n"));
	

	CCalAttendee* attendee1 = CCalAttendee::NewL(KAttendee11Address);
	CleanupStack::PushL(attendee1);
	entry->AddAttendeeL(attendee1);	
	CleanupStack::Pop(attendee1);
	
	CCalAttendee* attendee2 = CCalAttendee::NewL(KAttendee12Address, KAttendee12SentBy);
	CleanupStack::PushL(attendee2);
	entry->AddAttendeeL(attendee2);	
	CleanupStack::Pop(attendee2);

	CCalAttendee* attendee3 = CCalAttendee::NewL(KAttendee13Address);
	CleanupStack::PushL(attendee3);	
	entry->AddAttendeeL(attendee3);	
	CleanupStack::Pop(attendee3);

	CCalAttendee* attendee4 = CCalAttendee::NewL(KAttendee14Address);
	CleanupStack::PushL(attendee4);
	attendee4->SetResponseRequested(ETrue);
	test(attendee4->ResponseRequested());
	entry->AddAttendeeL(attendee4);	
	CleanupStack::Pop(attendee4);
	
	
	CCalUser* organiser1 = CCalUser::NewL(_L("organizer.one@example.com"), _L("dummy"));
	CleanupStack::PushL(organiser1);
	entry->SetOrganizerL(organiser1);
	CleanupStack::Pop(organiser1);
	
	
	CCalUser* organiser = CCalUser::NewL(_L("another.organizer@example.com"));
	CleanupStack::PushL(organiser);
	organiser->SetCommonNameL(KOrganizerCommonName1);	
	entry->SetOrganizerL(organiser);
	CleanupStack::Pop(organiser);

	organiser = NULL;
	organiser = entry->OrganizerL();
	
	TPtrC ptrAddress(organiser->Address());
	TPtrC ptrSentBy(organiser->SentBy());
	TPtrC ptrCommonName(organiser->CommonName());
	
	TRAPD(ret,entry->CompareL(*iEntry));
	test.Printf(_L("Return value from CCalEntry::CompareL function comparing two entries:%d\n"),ret);
	test(ret == KErrNone);
	
	CleanupStack::PopAndDestroy(entry);
	
	test.Printf(_L("End of comparing two entries\n"));
	}
	

// Test code added for Defect DEF077508

void CAttendeeTestManager::TestCopyFromL()
	{
	
	// Create a new entry, then use CopyFromL to copy data from this class's iEntry
	test.Next(_L("TestCopyFromL"));


	HBufC8* guid = KGUID3().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	entry->SetStartAndEndTimeL(iCalTime1, iCalTime2);
	
	CCalAttendee* attendee1 = CCalAttendee::NewL(KAttendee11Address);
	CleanupStack::PushL(attendee1);
	entry->AddAttendeeL(attendee1);	
	CleanupStack::Pop(attendee1);
	
	CCalAttendee* attendee2 = CCalAttendee::NewL(KAttendee12Address, KAttendee12SentBy);
	CleanupStack::PushL(attendee2);
	entry->AddAttendeeL(attendee2);	
	CleanupStack::Pop(attendee2);
	
	CCalUser* organiser1 = CCalUser::NewL(_L("some.dude@example.com"), _L("me"));
	CleanupStack::PushL(organiser1);
	entry->SetOrganizerL(organiser1);
	CleanupStack::Pop(organiser1);
	
	entry->CopyFromL(*iEntry);	
	test(entry->AttendeesL().Count() == iEntry->AttendeesL().Count());

	CleanupStack::PopAndDestroy(entry);
	test.Printf(_L("End of TestCopyFromL\n"));
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CAttendeeTestManager* testManager = CAttendeeTestManager::NewLC();
	

	TPerformanceTimer timer(test);
	timer.Start();	


	// Run the test suite

	testManager->CreateAndPopulateEntryL();
	testManager->TestOrganizerL();
	testManager->CompareEntriesL();
	testManager->TestAttendeesL();
	testManager->TestPhoneOwnerL();
	testManager->TestCopyFromL();
	
	
	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();


	CleanupStack::PopAndDestroy(testManager);
	}


/**

@SYMTestCaseID     PIM-TCAL-ATTENDEE-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-ATTENDEE-0001 Calendar Interim API Attendee test suite"));

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
