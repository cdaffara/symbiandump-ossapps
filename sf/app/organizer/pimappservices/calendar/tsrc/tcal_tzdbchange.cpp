// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <calentry.h>
#include <calsession.h>
#include <calentryview.h>
#include <e32test.h>
#include <calchangecallback.h>
#include <tz.h>
#include <vtzrules.h>
#include <calrrule.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <tzusernames.h>
#include <tzuserdefineddata.h>
#endif

_LIT(KTestName,"tcal_tzdbchange");
RTest test(KTestName);
_LIT8(KEuropeLondon, "Europe/London");

const TInt KUserTzRuleOffSetOld = 60;
const TInt KUserTzRuleOffSetNew = 120;
const TInt KTzRuleStartYear = 0;
const TInt KTzRuleEndYear = 9999;
_LIT(KTzDbFrom,"z:\\tzdb.dbz");
_LIT(KTzDbTo,"c:\\private\\1020383e\\tzdb.dbz");

class CCalTzDbChangeTest : public CBase, MCalChangeCallBack2
	{
public:
	static CCalTzDbChangeTest* NewLC();
	~CCalTzDbChangeTest();

	// from MCalChangeCallBack2
	void CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems);
	//Test tasks
	void TestSystemDbNotificationL();
	void TestUserDbNotificationL();
	void TestEntryModifiedDateL();

private: 
	CCalTzDbChangeTest();
	void ConstructL();
	
	CTzId* CreateUserTimeZoneLC();
	void UpdateUserTimeZoneL(const CTzId& aTzId, TInt aNewOffset);
	void RegisterNotificationL();
	TCalLocalUid CreateAndStoreEntryL();
	TInt GetEntryTzOffSetL(TCalLocalUid aEntryId);
	TTime GetEntryLastModifiedTimeL(TCalLocalUid aEntryId);
	TBool CompareTime(const TTime& aTime1, const TTime& aTTime2);
	void ResetTzUserDataL();
	
private:
	TBool 				iNotificationCallBackOccurred;
	CCalTestLibrary* 	iTestLib;
	CTzUserData*		iTzUserdata;
	RTz					iTzSession;
	};

CCalTzDbChangeTest::CCalTzDbChangeTest()
	:iNotificationCallBackOccurred(EFalse)
	{
	}

void CCalTzDbChangeTest::CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems)
	{
	test.Printf(_L("MCalChangeCallBack2 notification happened\n") );
	iNotificationCallBackOccurred = ETrue;

//There must be only one change in the array and it is EChangeTzRules type.
	test(aChangeItems.Count() == 1);
	TCalChangeEntry change = aChangeItems[0];
	test(change.iChangeType== MCalChangeCallBack2::EChangeTzRules);
	CActiveScheduler::Stop();
	}

CCalTzDbChangeTest* CCalTzDbChangeTest::NewLC()
	{
	CCalTzDbChangeTest* self = new (ELeave) CCalTzDbChangeTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	return (self);
	}
	
CCalTzDbChangeTest::~CCalTzDbChangeTest()
	{
	TRAP_IGNORE(ResetTzUserDataL());
		
	delete iTzUserdata;
	iTzSession.Close();
	delete iTestLib;
	}

void CCalTzDbChangeTest::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL();
	iTestLib->ReplaceFileL(KTestName);
	iTestLib->OpenFileL(KTestName);	
	
	User::LeaveIfError(iTzSession.Connect());
	iTzUserdata = CTzUserData::NewL(iTzSession);
	}

void CCalTzDbChangeTest::ResetTzUserDataL()
	{
	RPointerArray<CTzId> tzids;	
	CleanupResetAndDestroyPushL(tzids);
	iTzUserdata->GetTzIdsL(tzids);
	TInt count = tzids.Count();
	for (TInt ii=0; ii<count; ++ii)
		{
		iTzUserdata->DeleteL(*tzids[ii]);	
		}
	CleanupStack::PopAndDestroy(&tzids);
	}

void CCalTzDbChangeTest::RegisterNotificationL()
	{
	RDebug::Print(_L("Register Notication" ));
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);
	CCalChangeNotificationFilter* filter = CCalChangeNotificationFilter::NewL(MCalChangeCallBack2::EChangeEntryAll, ETrue, timeRange);
	iTestLib->GetSession().StartChangeNotification(*this, *filter);
	delete filter;
	}

CTzId* CCalTzDbChangeTest::CreateUserTimeZoneLC()
	{	
	test.Printf(_L("Create User Time Zone\n" ));
	//Create a user defined rule - CTzRules.
	TTime dstOn (TDateTime(2005, EMarch, 0, 0, 0, 0, 0)); 
	TTime dstOff (TDateTime(2005, ESeptember, 0, 0, 0, 0, 0));
	TMonth monthOn = dstOn.DateTime().Month();
	TInt dayOn = dstOn.DateTime().Day();
	TMonth monthOff = dstOff.DateTime().Month();
	TInt dayOff = dstOff.DateTime().Day();
	
	TTzRule dstOffRule(KTzRuleStartYear, KTzRuleEndYear, KUserTzRuleOffSetOld, KUserTzRuleOffSetOld, monthOn,  ETzFixedDate, dayOn,  0, ETzWallTimeReference, 0);
	TTzRule dstOnRule (KTzRuleStartYear, KTzRuleEndYear, KUserTzRuleOffSetOld, KUserTzRuleOffSetOld, monthOff, ETzFixedDate, dayOff, 0, ETzWallTimeReference, 0);

	CTzRules* rules = CTzRules::NewL(KTzRuleStartYear, KTzRuleEndYear);
	CleanupStack::PushL(rules);
	rules->AddRuleL(dstOffRule);
	rules->AddRuleL(dstOnRule);
	
	//Create a user defined rule names- CTzUserNames.
	_LIT(KStandardName, "UserTzName");
	_LIT(KShortName, "UserTzSh");
	_LIT(KDaylightSaveName, "UserDLSaveName");
	_LIT(KShortDaylightSaveName, "UserShDLS");
	_LIT(KCityNameName, "UserCityName");
	_LIT(KRegionName, "UserRegionName");
	
	CTzUserNames* names = CTzUserNames::NewLC(KStandardName(), KShortName(), KDaylightSaveName(), KShortDaylightSaveName(), KCityNameName(), KRegionName());
	//Add the rule to the database
	CTzId* tzid = iTzUserdata->CreateL(*rules, *names);
	CleanupStack::PopAndDestroy(2, rules);
	CleanupStack::PushL(tzid);
	return tzid;
	}

void CCalTzDbChangeTest::UpdateUserTimeZoneL(const CTzId& aTzId, TInt aNewOffset)
	{	
	test.Printf(_L("Update User Time Zone\n") );
	//Create a user defined rule - CTzRules.
	TTime dstOn (TDateTime(2005, EMarch, 0, 0, 0, 0, 0)); 
	TTime dstOff (TDateTime(2005, ESeptember, 0, 0, 0, 0, 0));
	TMonth monthOn = dstOn.DateTime().Month();
	TInt dayOn = dstOn.DateTime().Day();
	TMonth monthOff = dstOff.DateTime().Month();
	TInt dayOff = dstOff.DateTime().Day();
	
	TTzRule dstOffRule(KTzRuleStartYear, KTzRuleEndYear, aNewOffset, aNewOffset, monthOn,  ETzFixedDate, dayOn,  0, ETzWallTimeReference, 0);
	TTzRule dstOnRule (KTzRuleStartYear, KTzRuleEndYear, aNewOffset, aNewOffset, monthOff, ETzFixedDate, dayOff, 0, ETzWallTimeReference, 0);

	CTzRules* rules = CTzRules::NewL(KTzRuleStartYear, KTzRuleEndYear);
	CleanupStack::PushL(rules);
	rules->AddRuleL(dstOffRule);
	rules->AddRuleL(dstOnRule);
	
	CTzUserNames* names = iTzUserdata->ReadNamesL(aTzId);
	CleanupStack::PushL(names);
	iTzUserdata->UpdateL(aTzId, *rules, *names);
	CleanupStack::PopAndDestroy(2, rules);
	}

TCalLocalUid CCalTzDbChangeTest::CreateAndStoreEntryL()
	{
	test.Printf(_L("Add entry\n" ));
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	_LIT8(KUid, "UID_TzDb");
	HBufC8* guid = KUid().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	
	//Set entry start and end time
	TTime startTime (TDateTime(2005, EJanuary, 7, 10, 0, 0, 0)); 
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime);
	TTime endTime (TDateTime(2005, EJanuary, 7, 11, 0, 0, 0)); 
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(endTime);
	entry->SetStartAndEndTimeL(calStartTime, calEndTime);
	
	//Set repeating 
	TCalRRule repeat(TCalRRule::EDaily);
	repeat.SetDtStart(calStartTime);
	repeat.SetCount(3);
	entry->SetRRuleL(repeat);

	//Store the Entry
	TInt numStoredEntries;
	iTestLib->SynCGetEntryViewL().StoreL(entries, numStoredEntries);
	test (numStoredEntries == entries.Count());
	TCalLocalUid entryId = entries[0]->LocalUidL();

	CleanupStack::PopAndDestroy(&entries);
	return entryId;	
	}

TInt CCalTzDbChangeTest::GetEntryTzOffSetL(TCalLocalUid aEntryId)
	{
	test.Printf(_L("Get Entry Tz OffSet\n" ));
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(aEntryId);
	CleanupStack::PushL(entry);
	CTzRules* tzrule = entry->TzRulesL();
	CleanupStack::PopAndDestroy(entry);
	CleanupStack::PushL(tzrule);
	TTime now;
	now.UniversalTime();
	TInt entryRuleOffset = tzrule->GetOffsetL(now, ETzUtcTimeReference);
	CleanupStack::PopAndDestroy(tzrule);
	return entryRuleOffset; 
	}

TTime CCalTzDbChangeTest::GetEntryLastModifiedTimeL(TCalLocalUid aEntryId)
	{
	test.Printf(_L("Get Entry Last Modified Time\n" ));
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(aEntryId);
	CleanupStack::PushL(entry);
	TTime lastModifiedTime = entry->LastModifiedDateL().TimeUtcL();
	CleanupStack::PopAndDestroy(entry);
	return lastModifiedTime; 
	}

TBool CCalTzDbChangeTest::CompareTime(const TTime& aTime1, const TTime& aTime2)
	{
	//We need to reset seconds to zero because delays caused in WDP builds
	//can be more than a second or may interleave between seconds in which case 
	//even though everthing else is same, test will fail intermittently.
	test.Printf(_L("aTime1 aTime2\n" ));
	TDateTime dateTime1 = aTime1.DateTime();
	dateTime1.SetMicroSecond(0);
	dateTime1.SetSecond(0);
	TTime time1(dateTime1);
	TDateTime dateTime2 = aTime2.DateTime();
	dateTime2.SetMicroSecond(0);
	dateTime2.SetSecond(0);
	TTime time2(dateTime2);
	return time1==time2;
	}

/** Time zone rules change notification for system time zone rules database

@SYMTestCaseID      PIM-CAL-TZC-0001
@SYMTestCaseDesc    The purpose of this test is to verify that a Calendar client is notified if and only if the time zone rules, from a time zone in the system time zone database, associated with a Calendar entry are modified.

@SYMTestActions 
1.	Register for Calendar entry change notification.
2.	Set the current time zone to a time zone in the system time zone rules database.
3.	Create a fixed time repeating Calendar entry in the current time zone.
4.	Copy a time zone rules database, which has a modified time zone rule for the current time zone, to the appropriate location on the C: drive. 2
5.	Delete the time zone database, which has reverted the time zone rule to the original one.
 
@SYMTestExpectedResults     The Calendar client is notified when the time zone database has been copied or deleted to\from c: drive
@SYMTestType                CT
@SYMTestPriority            1

*/

void CCalTzDbChangeTest::TestSystemDbNotificationL()
	{
	//Register for Calendar entry change notification.
	test.Next(_L("@SYMTESTCaseID:PIM-CAL-TZC-0001 Test System Db Notification"));

	RegisterNotificationL();

	//Set the current time zone to London
	iTestLib->SetTimeZoneL(KEuropeLondon);
	TTime now;
	now.HomeTime();
	
	//Create a repeating entry
	CreateAndStoreEntryL();
	//Copy time zone database to c:\....
	//Put the current time 2 second forward because the time resolution in h4 board is one second
	iTestLib->PIMTestServer().SetHomeTime(now + TTimeIntervalSeconds(2));

	iTestLib->PIMTestServer().CopyFileL(KTzDbFrom, KTzDbTo);
	iTzSession.SwiObsBeginL();
	iTzSession.SwiObsFileChangedL(RTz::EFilterTzPrivate);
	iTzSession.SwiObsEndL();

	CActiveScheduler::Start();
	//CActiveScheduler is stopped in Callback
	
	test(iNotificationCallBackOccurred);
	iNotificationCallBackOccurred = EFalse;
	now.HomeTime();
	//delete the tzdb in c:
	//Put the current time 2 second forward because the time resolution in h4 board is one second
	iTestLib->PIMTestServer().SetHomeTime(now + TTimeIntervalSeconds(2));
	iTestLib->PIMTestServer().DeleteFileL(KTzDbTo);
	iTzSession.SwiObsBeginL();
	iTzSession.SwiObsFileChangedL(RTz::EFilterTzPrivate);
	iTzSession.SwiObsEndL();

	CActiveScheduler::Start();
	//CActiveScheduler is stopped in Callback

	test(iNotificationCallBackOccurred);
		
	iTestLib->GetSession().StopChangeNotification();
	iNotificationCallBackOccurred = EFalse;
	}

/** Time zone rules change notification for user defined time zone

@SYMTestCaseID      PIM-CAL-TZC-0002
@SYMTestCaseDesc    The purpose of this test is to verify that:
1.	A Calendar client is notified and with correct time stamp.
2.	A Calendar entry has the correct time zone as the system time zone when the entry it is created.

@SYMTestActions 
 
1.	Create a user defined time zone.
2.	Set the current time zone to the user defined time zone created in action 1.
3.	Register for Calendar entry change notification.
4.	Create a fixed time repeating Calendar entry in the current time zone.
5.	Fetch the entry and check its time zone
6.	Update the time zone rules for the user defined time zone created in action 1.
7.	Fetch the entry and check its time zone 
8.	Disconnect to the Agenda server and reopen it
9.	Fetch the entry and check its time zone 
10.	Disconnect to the Agenda server 
11.	Update the time zone rules for the user defined time zone created in action 1 second time.
12.	Reconnect to the Agenda server and reopen the file
13.	Fetch the entry and check its time zone
14.	Delete the user defined time zone created in action 1.

@SYMTestExpectedResults     

1 The entry fetched in action 5 should have the same time zone object as the time zone created in action 1
2 The Calendar client is notified of a time zone rules change in action 6.
3 The entry fetched in action 7 and 9 should have the same time zone object as the time zone updated in action 6
4 The entry fetched in action 13 and 14 should have the same time zone object as the time zone updated in action 11   

@SYMTestType                CT
@SYMTestPriority            1  
*/

void CCalTzDbChangeTest::TestUserDbNotificationL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-CAL-TZC-0002 Test User Db Notification"));

	//Set time zone to the user defined time zone created 
	CTzId* tzid = CreateUserTimeZoneLC();
	iTestLib->SetTimeZoneL(tzid->TimeZoneNameID());
	
	RegisterNotificationL();
	//Add an entry to the Calendar file
	TCalLocalUid entryid = CreateAndStoreEntryL();
	
	//Get entry's rule and test offset
	test(KUserTzRuleOffSetOld == GetEntryTzOffSetL(entryid));
	
	//Update the user time zone
	
	TTime now;
	now.HomeTime();

	//Put the current time 2 second forward because the time resolution in h4 board is one second
	iTestLib->PIMTestServer().SetHomeTime(now + TTimeIntervalSeconds(2));
	UpdateUserTimeZoneL(*tzid, KUserTzRuleOffSetNew);
	
	//Test the notification
	CActiveScheduler::Start();
	//CActiveScheduler is stopped in Callback
	test(iNotificationCallBackOccurred);
	iNotificationCallBackOccurred = EFalse;
	
	//Get entry's rule and test offset
	test(KUserTzRuleOffSetNew == GetEntryTzOffSetL(entryid));
	
	//Close the agenda server, reconnect and reopen the file
	iTestLib->GetSession().StopChangeNotification();
	test.Printf(_L("Server is shuting down\n"));
	iTestLib->CloseAgendaServer();
	test.Printf(_L("Server is reconnected\n"));
	iTestLib->ConnectAgendaServerL();
	iTestLib->OpenFileL(KTestName);	
	
	//Get entry's rule and test offset
	test(KUserTzRuleOffSetNew == GetEntryTzOffSetL(entryid));
	
	test.Printf(_L("Server is shuting down"));
	iTestLib->CloseAgendaServer();
	
	//Update the user time zone
	now.HomeTime();
	iTestLib->PIMTestServer().SetHomeTime(now + TTimeIntervalSeconds(2));
	UpdateUserTimeZoneL(*tzid, KUserTzRuleOffSetOld);
	
	//Reconnect to the agenda server and reopen the file
	test.Printf(_L("Server is reconnected\n"));
	iTestLib->ConnectAgendaServerL();
	iTestLib->OpenFileL(KTestName);	
	//Check the entry's time zone
	test(KUserTzRuleOffSetOld == GetEntryTzOffSetL(entryid));
	
	//Delete the user time zone
	iTzUserdata->DeleteL(*tzid);
	//Get entry's rule which should be the same as it was (the time zone should still stored in Calendar although it has been deleted from the time zone server)
	test(KUserTzRuleOffSetOld == GetEntryTzOffSetL(entryid));

	CleanupStack::PopAndDestroy(tzid);
	
	iTestLib->GetSession().StopChangeNotification();
	iNotificationCallBackOccurred = EFalse;
	}
	
/** Entry last modified date after time zone rules change

@SYMTestCaseID      PIM-CAL-TZC-0003
@SYMTestCaseDesc    The purpose of this test is to verify that a change to time zone rules is reflected in the last modified date for a Calendar entry associated with those time zone rules.
@SYMTestActions 

1.  Set system time
2.  Create a user defined time zone
3.  Set the current time zone to the user defined time zone created in action1         
4. Create a fixed time repeating Calendar entry in the current time zone.
5. Get the last modified date for the Calendar entry created in action 4
6. Set the system time to later
7. Update the time zone rules for the user defined time zone created in action 2. 
8. Get the last modified date for the Calendar entry created in action 4. 
9. Get entry IDs since last modified time (after the entry creation time and before the time zone modified time).  
10. Disconnect to the agenda server and reconnect again and reopen the file.
11. Repeat action 9. 

@SYMTestExpectedResults     

1 The last modified date obtained in action 5 is same as the time when entry created.
2 The Calendar client is notified of a time zone rules change happened in action 7.
3 The last modified date obtained in action 8 is same as the time when the user defined zone is updated in action 7.
4 The entry fetched in action 9 and 11 is the entry created in action 4.

@SYMTestType                CT
@SYMTestPriority 			1
*/

void CCalTzDbChangeTest::TestEntryModifiedDateL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-CAL-TZC-0003 Test Entry Modified Date"));

	//Set system time so that it can be compared.
	TTime entryCreationTime (TDateTime(2005, EJanuary, 7, 10, 0, 0, 0)); 
	iTzSession.SetHomeTime(entryCreationTime);
	iTzSession.ConvertToUniversalTime(entryCreationTime);

	//Set time zone to the user defined time zone created
	CTzId* tzid = CreateUserTimeZoneLC();
	iTestLib->SetTimeZoneL(tzid->TimeZoneNameID());

	//Create a new Calendar file and add a repeating entry to it
	iTestLib->ReplaceFileL(KTestName);
	iTestLib->OpenFileL(KTestName);	
	RegisterNotificationL();
	TCalLocalUid entryid = CreateAndStoreEntryL();
	
	//Fetch the entry and Get lastmodified of date of the entry
	TTime time = GetEntryLastModifiedTimeL(entryid);
	test(CompareTime(entryCreationTime, time));

	//Change system time to the next day
	TTime tzDbChangeTime (TDateTime(2005, EJanuary, 8, 10, 0, 0, 0)); 
	iTzSession.SetHomeTime(tzDbChangeTime);
	iTzSession.ConvertToUniversalTime(tzDbChangeTime);
	
	//Update user rule
	UpdateUserTimeZoneL(*tzid, KUserTzRuleOffSetNew);
	
	//Calendar is notified for updating user time zone
	CActiveScheduler::Start();
	test(iNotificationCallBackOccurred);
	iNotificationCallBackOccurred = EFalse;
	CleanupStack::PopAndDestroy(tzid);
	
	//Fetch entry and get last modified date which is same as the time when the user time zone is updated
	time = GetEntryLastModifiedTimeL(entryid);
	test(CompareTime(tzDbChangeTime, time));
	
	//Get entry Ids since the time just before the time zone modified
	RArray<TCalLocalUid> ids;
	CleanupClosePushL(ids);
	TCalTime timeSince;
	timeSince.SetTimeUtcL(tzDbChangeTime - TTimeIntervalMinutes(5));
	iTestLib->SynCGetEntryViewL().GetIdsModifiedSinceDateL(timeSince, ids);
	test(ids.Count()==1);
	test(ids[0]==entryid);
	ids.Reset();
	
	//Disconnect to the agenda server and connect and reopen the file
	iTestLib->GetSession().StopChangeNotification();
	test.Printf(_L("Server is shuting down\n"));
	iTestLib->CloseAgendaServer();
	iTestLib->ConnectAgendaServerL();
	iTestLib->OpenFileL(KTestName);	
	
	//Get entry Ids since the time just before the time zone modified
	iTestLib->SynCGetEntryViewL().GetIdsModifiedSinceDateL(timeSince, ids);
	test(ids.Count()==1);
	test(ids[0]==entryid);
	
	CleanupStack::PopAndDestroy(&ids);
	}

void DeleteSysTzRulesDatabaseL()
	{
	//Make sure file c:\\private\\1020383e\\tzdb.dbz is deleted even the test fails.
	RPIMTestServer serv;
	User::LeaveIfError(serv.Connect());
	serv.CloseTzSession();
	_LIT(KAgendaServer,"agsvexe*");
	TRAP_IGNORE(serv.KillProcessL(KAgendaServer));
	_LIT(KTzServer,"Tzserver*");
	TRAP_IGNORE(serv.KillProcessL(KTzServer));
	serv.DeleteFileL(KTzDbTo);
	TInt err = serv.CheckForFile(KTzDbTo);
	test.Printf(_L("err = %d\n"), err );
	if(err!=KErrNotFound)
		{
		test.Printf(_L("c:\\private\\1020383e\\tzdb.dbz is not deleted!!!\n"));
		}
	
	serv.Close();
	}

void ClearTZDirectory()
	{
	RPIMTestServer serv;	
	User::LeaveIfError(serv.Connect());

	_LIT(KAgendaServer,"agsvexe*");
	TRAP_IGNORE(serv.KillProcessL(KAgendaServer));
	_LIT(KTzServer,"Tzserver*");
	TRAP_IGNORE(serv.KillProcessL(KTzServer));
	
	_LIT(KTzDir,"c:\\private\\1020383e\\");
	serv.DeleteFilesInDirL(KTzDir);

	serv.Close();
	}
	
/**

@SYMTestCaseID     PIM-TCAL-TZDBCHANGE-0001

*/
void DoTestsL()
	{
	test.Title();
	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-TZDBCHANGE-0001 Calendar TZ Rules Data Change Awareness Test Suite"));

	
	DeleteSysTzRulesDatabaseL();
	ClearTZDirectory();
	
	CCalTzDbChangeTest* testManager = CCalTzDbChangeTest::NewLC();
	
	// Run the test suite
	testManager->TestSystemDbNotificationL();
	testManager->TestUserDbNotificationL();
	testManager->TestEntryModifiedDateL();

	CleanupStack::PopAndDestroy(testManager);

	DeleteSysTzRulesDatabaseL();
	ClearTZDirectory();
	
	test.End();
	}

TInt E32Main()
    {
	__UHEAP_MARK;

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if(!trapCleanup)
		{
		return KErrNoMemory;
		}

	CActiveScheduler* scheduler = new CActiveScheduler;
	if(!scheduler)
		{
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);	
	
	TRAPD(ret, DoTestsL());
	test.Printf(_L("Trapped return value from DoTestsL(): %d\n"), ret);
	test(ret == KErrNone);

	test.Close();	

	delete scheduler;
	delete trapCleanup;	

	__UHEAP_MARKEND;

	return (KErrNone);
    }

