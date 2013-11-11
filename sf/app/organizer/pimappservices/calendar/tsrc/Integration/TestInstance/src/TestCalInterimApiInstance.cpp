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
// This file implements the test case for Process agenda entries/ Instance
// 
//

// User Include
#include "TestCalInterimApiInstance.h"
#include "TestCalInterimApiInstanceSuite.h"
#include "TestCalInterimApiInstanceData.h"

/**
Description:CTestCalInterimApiInstance() constructor
This function constructs the CTestCalInterimApiInstance() object and
initialises all its parameters along with its base class variable.
@internalComponent
@test
*/

CTestCalInterimApiInstance::CTestCalInterimApiInstance()
	{
	// Calls Base class method to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiInstance);
	}

/**
Description : CTestCalInterimApiInstance() destructor
This is a destructor function used to clear all the used memory.
@test
@internalTechnology
*/

CTestCalInterimApiInstance::~CTestCalInterimApiInstance()
	{
	if(iEntry != NULL)
		{
		delete iEntry;
		}
	delete iEntryView;
	delete iInstance;
	delete iInstanceView;
	delete iScheduler;
	delete iSession;
	}

/**
Decription:doTestStepPreambleL()
* Implementation of CTestStep base class virtual
* It is used for doing all initialisation common to derived classes in here.
@test
@internalTechnology
@return   EPass or EFail indicating the success or failure.
*/

TVerdict CTestCalInterimApiInstance::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("Start Instance Tests"));
	// Active Scheduler to start the alarmserver
	iScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	// Create a session for calendar file
	iSession = CCalSession::NewL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
Description:doTestStepPostambleL()
* Implementation of CTestStep base class virtual
* It is used for doing all after test treatment common to derived classes in here.
@test
@internalTechnology
@return EPass or EFail indicating the success or failure.
*/

TVerdict CTestCalInterimApiInstance::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("Completed the Instance Tests"));
	return TestStepResult();
	}

/**
Description:Tries to do the main functionality
@test
@internalTechnology
@return	EPass or EFail indicating the success or failure
*/

TVerdict CTestCalInterimApiInstance::doTestStepL()
	{
	// Function to create and open the calendar file
	TRAPD(err,CreateAndOpenCalFileL());
	TEST(err==KErrNone);

	if (!err)
		{
		// function to add the entries
		TRAP(err,AddEntryL());
		INFO_PRINTF2(_L("The value of the error code in AddEntryL is %d"),err);
		TEST(err == KErrNone);
		
		// function to fetch the entry and update the entry
		TRAP(err,FetchAndUpdateEntryL());
		INFO_PRINTF2(_L("The value of the error code in FetchAndUpdateEntryL is %d"),err);
		TEST(err == KErrNone);
		
		// function to find the instances
		TRAP(err,FindInstancesL());
		INFO_PRINTF2(_L("The value of the error code in FindInstanceL is %d"),err);
		TEST(err == KErrNone);

		// function to find the next and previous instance
		TRAP(err,NextPrevInstancesL());
		INFO_PRINTF2(_L("The value of the error code in NextPrevInstance is %d"),err);
		TEST(err == KErrNone);

		
		//function to delete the instances
		TRAP(err,DeleteInstancesL());
		INFO_PRINTF2(_L("The value of the error code in DeleteInstanceL is %d"),err);
		TEST(err == KErrNone);
		
		}
	else
		{
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

// Local function to destroy the RPointerArray
LOCAL_D void DestroyRPointerArray(TAny* aPtr)
	{
	RPointerArray<CCalEntry>* self = static_cast<RPointerArray<CCalEntry>*> (aPtr);
	self->ResetAndDestroy();
	self->Close();
	}
/**
@SYMTestCaseID PIM-APP-ENGINES-CALINTERIMAPI-CIT-0001
*/
/**
@test
@internalTechnology
@SYMTestCaseID PIM-APP-ENGINES-CALINTERIMAPI-CIT-0003
@SYMTestType CIT
@SYMTestPriority High
@SYMPREQ 1098
@SYMFssID 3.2.1 002, 003
@SYMTestCaseDesc Creates and opens a Calendar file on C:\
@SYMTestActions Create a new Calendar File,If the file already exists
it deletes the file and create it.Opens the created file.
@SYMTestExpectedResults CCalSession should have created and opened the Calendar file
*/

void CTestCalInterimApiInstance::CreateAndOpenCalFileL()
	{
	TPtrC calFileName;
	//Fetching the input from the Ini file
	if (!GetStringFromConfig(KIniCreateAndOpenCalFile, KIniCalFileName, calFileName))
		{
		ERR_PRINTF2(_L("Unable to fetch filename from ini file ; \n%s"),&KIniCalFileName);
		SetTestStepResult(EFail);
		}
	else
		{
		TRAPD(err, iSession->CreateCalFileL(calFileName));
		if (KErrAlreadyExists==err)
			{
			iSession->DeleteCalFileL(calFileName);
			iSession->CreateCalFileL(calFileName);
			}
		else
			{
			User::LeaveIfError(err);
			}
		}
	TInt err;
	TRAP(err,iSession->OpenL(calFileName));
	}

/**
@SYMTestCaseID PIM-APP-ENGINES-CALINTERIMAPI-CIT-0007
*/
/**
@SYMTestCaseID PIM-APP-ENGINES-CALINTERIMAPI-CIT-0008
*/
/**
@test
@internalTechnology
@SYMTestCaseID PIM-APP-ENGINES-CALINTERIMAPI-CIT-0010
@SYMTestType CIT
@SYMTestPriority High
@SYMPREQ 1098
@SYMFssID 3.2.4 001
@SYMTestCaseDesc Creates a new entry in the calendar file.
@SYMTestActions Adds the entry in the calendar file.Set the startdate time and enddate time
for the entry.Set the repeat rule of type daily for the entry.Stores the created entry.
@SYMTestExpectedResults CCalEntry should have created the entry and stored it.
*/
void CTestCalInterimApiInstance::AddEntryL()
	{
	// Local variables defined
	TInt entryType;
	TPtrC startDateTimePtr;
	TPtrC endDateTimePtr;
	TPtrC uidName;
	TPtrC summary;
	TInt repeatType=0;
	TInt interval=0;
	TBuf16<10> sectionName(_L("Entry"));
	TBuf16<10> sectionNameCopy(_L(""));
	TInt count=1;
	TBool sectionRead;
	
	// Active scheduler implemented for Entryview to store the index of entries
	TRequestStatus status(KRequestPending);
	CTestCalInterimApiInstanceData* instanceData = new (ELeave) CTestCalInterimApiInstanceData(&status);
	CleanupStack::PushL(instanceData);
	iEntryView = CCalEntryView::NewL(*iSession,*instanceData);
	CActiveScheduler::Add(instanceData);
	CActiveScheduler::Start();
	User::WaitForRequest(status);
	CleanupStack::PopAndDestroy(instanceData);
	
	sectionNameCopy=sectionName;
	sectionNameCopy.AppendNum(count);
	sectionRead=GetIntFromConfig(sectionNameCopy,KIniEntryType,  entryType);
	if (EFalse == sectionRead)
		{
		ERR_PRINTF2(_L("Entry Section Name or %s Not defined in .INI File"), &KIniEntryType);
		SetTestStepResult(EFail);
		}
		
	while (sectionRead)
		{
		if( !GetStringFromConfig(sectionNameCopy,  KIniStartDateTime,  startDateTimePtr) ||
			!GetStringFromConfig(sectionNameCopy,  KIniEndDateTime,  endDateTimePtr) ||
			!GetStringFromConfig(sectionNameCopy,  KIniuidName,  uidName)||
			!GetStringFromConfig(sectionNameCopy,  KIniSummary,  summary)||
			!GetIntFromConfig(sectionNameCopy,  	KIniRepeatType,  repeatType)||
			!GetIntFromConfig(sectionNameCopy,  	KIniInterval,  interval))
			{
			ERR_PRINTF7(_L("Problem in reading values from ini. \n Expected field is %d\n%s\n%s\n%s\n%s\n%d"
						),&KIniEntryType,&KIniStartDateTime,&KIniEndDateTime,&KIniuidName,&KIniSummary,&KIniRepeatType);
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF2(_L("The data to be added for the entry%d is" ),count);
			INFO_PRINTF2(_L("EntryType : %d"),entryType);
			INFO_PRINTF2(_L("Summary : %S"),&summary);
			INFO_PRINTF2(_L("RepeatType : %d"),repeatType);
			INFO_PRINTF2(_L("Interval : %d"),interval);
			INFO_PRINTF2(_L("Startdatetime : %S"),&startDateTimePtr);
			INFO_PRINTF2(_L("EndDateTime : %S"),&endDateTimePtr);
			INFO_PRINTF2(_L("uidName : %S"),&uidName);

			// To convert Tptrc to TTime
			TInt err1=iStartDateTime.Set(startDateTimePtr);
			err1=iEndDateTime.Set(endDateTimePtr);

			// To change TTime as TCalTime
			iCalTime1.SetTimeLocalL(iStartDateTime);
			iCalTime2.SetTimeLocalL(iEndDateTime);

			// To convert Tptrc16 to HBufc8
			HBufC8* uidData=HBufC8::NewL(uidName.Length());
			uidData->Des().Copy(uidName);

			// switch for entryType
			CCalEntry::TType entryTypeEval=(CCalEntry::TType)entryType;
 
			TUint seqNum=0;
			// if condition for invalid data
			if(entryType<CCalEntry::EAppt || entryType>CCalEntry::EAnniv)
				{
				INFO_PRINTF1(_L("The entry type values is invalid"));
				SetTestStepResult(EPass);
				}
			else
				{

				// create an entry with the specified details.
				TRAPD(err2,iEntry= CCalEntry::NewL(entryTypeEval, uidData, CCalEntry::EMethodAdd, seqNum));
				INFO_PRINTF2(_L("The error occured in CCalentry is %d"),err2);

				// set the startdate time and enddate time for the entry
				TRAPD(err3,iEntry->SetStartAndEndTimeL(iCalTime1,iCalTime2));	
				INFO_PRINTF2(_L("The error occured in startendtime is %d"),err3);

				// set the summary
				iEntry->SetSummaryL(summary);

				// Switch for repeat rule
				TCalRRule::TType repeatRuleEval=(TCalRRule::TType)repeatType;

				// if condition for invalid data
				if(repeatRuleEval<TCalRRule::EDaily || repeatRuleEval>TCalRRule::EYearly)
					{
					INFO_PRINTF1(_L("Repeat rule data is invalid"));
					SetTestStepResult(EPass);
					}
				else
					{
					TCalRRule* rpt = new (ELeave) TCalRRule;
					CleanupStack::PushL(rpt);
					// Set the Type of the repeat rule
					rpt->SetType(repeatRuleEval);
					INFO_PRINTF2(_L("The type which is set is %d"),rpt->Type());

					// To set the startdate and end datetime for repeat rule
					TCalTime calTime;
					calTime.SetTimeLocalL(iStartDateTime);

					// set the start date for the repeat rule
					rpt->SetDtStart(calTime);

					calTime.SetTimeLocalL(iEndDateTime);
					rpt->SetUntil(calTime);
					rpt->SetInterval(interval);

					// Repeat rule for weekly
					if(repeatRuleEval==TCalRRule::EWeekly)
						{
						RArray<TDay> array1;
						array1.Append(EMonday);
						rpt->SetByDay(array1);
						array1.Reset();
						}

					// repeat rule for monthly
					if(repeatRuleEval==TCalRRule::EMonthly)
						{
						RArray<TCalRRule::TDayOfMonth> dayArray;
						// Every third wednesday and last thursday of two months
						TCalRRule::TDayOfMonth day1(EWednesday, 3);
						dayArray.AppendL(day1);
						TCalRRule::TDayOfMonth day2(EThursday, -1);
						dayArray.AppendL(day2);
						rpt->SetByDay(dayArray);
						dayArray.Reset();
						}

					// To set the repeat rule
					TRAPD(err4,iEntry->SetRRuleL(*rpt));

					INFO_PRINTF2(_L("The error occured in setrrule is %d"),err4);

					// array to store the entry
					RPointerArray<CCalEntry> entryArray;
					CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &entryArray));
					entryArray.Append(iEntry);
					TInt success;

					// To Store the entries
					TRAPD(err5,iEntryView->StoreL(entryArray,success));
					INFO_PRINTF2(_L("The error occured in storeL is %d"),err5);
					INFO_PRINTF2(_L("Number of entries stored : %d"),success);
					if(success>0)
						{
						INFO_PRINTF1(_L("Entry is created successfully "));
						}
					else
						{
						INFO_PRINTF1(_L("The test step is failed"));
						SetTestStepResult(EFail);
						}
					CleanupStack::Pop(&entryArray);
					delete iEntry;
					iEntry = NULL;
					entryArray.Reset();
					CleanupStack::PopAndDestroy(rpt);					
					}//set the repeats
				}// if condition for invalid data
			}// End of else if we have the attributes from the ini file
		sectionNameCopy.Zero();
		++count;
		sectionNameCopy=sectionName;
		sectionNameCopy.AppendNum(count);	
		sectionRead=GetIntFromConfig(sectionNameCopy,KIniEntryType,  entryType);
		} // while
	}// End of function

/**
@SYMTestCaseID PIM-APP-ENGINES-CALINTERIMAPI-CIT-0011
*/
/**
@test
@internalTechnology
@SYMTestCaseID PIM-APP-ENGINES-CALINTERIMAPI-CIT-0019
@SYMTestType CIT
@SYMTestPriority High
@SYMPREQ 1098
@SYMFssID 3.2.4 002, 003
@SYMTestCaseDesc Get the entry or an instance, Update one or more instances of the entry
@SYMTestActions Fetch the entries with the specified uid's.Change the summary of the specified
entry.Store the updated entry.
@SYMTestExpectedResults CCalEntryView should have fetched the entry with the specified uid
and should have updated the entry.
*/

void CTestCalInterimApiInstance::FetchAndUpdateEntryL()
	{
	// Local variables declaration
	TPtrC uidName;
	TPtrC summary;
	TBuf16<20> sectionName(_L("FetchUpdateEntry"));
	TBuf16<20> sectionNameCopy(_L(""));
	TInt count=1;
	TBool sectionRead;

	sectionNameCopy=sectionName;
	sectionNameCopy.AppendNum(count);
	sectionRead=GetStringFromConfig(sectionNameCopy,  KIniuidName,  uidName);
	if (EFalse == sectionRead)
		{
		ERR_PRINTF2(_L("FetchUpdateEntry Section Name or %s Not defined in .INI File"), &KIniuidName);
		SetTestStepResult(EFail);
		}
		
	while (sectionRead)
		{
		if(!GetStringFromConfig(sectionNameCopy,  KIniSummary,  summary))
			{
			ERR_PRINTF2(_L("Problem in reading values from ini. \n Expected field is %s"), &KIniSummary);
			SetTestStepResult(EFail);
			}
		else
			{
			// To convert Tptrc16 to HBufc8
			HBufC8* uidData=HBufC8::NewLC(uidName.Length());
			uidData->Des().Copy(uidName);

			// To fetch the entries with the specified id's
			RPointerArray<CCalEntry> entryArray;
			CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &entryArray));
			TRAPD(err6,iEntryView->FetchL(*uidData, entryArray));


			INFO_PRINTF2(_L("The error occured in fetchL is %d"),err6);
			TInt entrieslist=entryArray.Count();
			INFO_PRINTF2(_L("The no. of entries fetched is %d"),entrieslist);

			if(entrieslist==0)
				{
				INFO_PRINTF1(_L("The entry with the specific uid is not existing"));
				}
			else
				{
				// fetch the first entry in the list
				iEntry=entryArray[0];

				// change the summary of the specified entry
				iEntry->SetSummaryL(summary);
				TInt success=0;

				// Update the entry
				TRAPD(err7,iEntryView->UpdateL(entryArray, success));
				INFO_PRINTF2(_L("The error occured in updateL is %d"),err7);
				INFO_PRINTF2(_L("Number of entries Updated : %d"),success);
				if(success>0)
					{
					INFO_PRINTF1(_L("Entry Updated successfully"));
					}
				else
					{
					INFO_PRINTF1(_L("The test step is failed"));
					SetTestStepResult(EFail);
					}

				}

			CleanupStack::Pop(&entryArray);
			CleanupStack::PopAndDestroy(uidData);
			entryArray.ResetAndDestroy();
			iEntry = NULL;

			} // End of else
		sectionNameCopy.Zero();
		++count;
		sectionNameCopy=sectionName;
		sectionNameCopy.AppendNum(count);	
		sectionRead=GetStringFromConfig(sectionNameCopy,  KIniuidName,  uidName);
		} // while
	} // End of main

/**
@test
@internalTechnology
@SYMTestCaseID PIM-APP-ENGINES-CALINTERIMAPI-CIT-0018
@SYMTestType CIT
@SYMTestPriority High
@SYMPREQ 1098
@SYMFssID 3.2.4  006, 007
@SYMTestCaseDesc Search the agenda entries and list the no.of instances.
@SYMTestActions Find the list of instances for the specified criteria.
@SYMTestExpectedResults CCalInstanceView should have found the specified instances
of the following criteria.
*/

void CTestCalInterimApiInstance::FindInstancesL()
	{

	// Local variables declaration
	TInt filter;
	TPtrC startDateTimePtr;
	TPtrC endDateTimePtr;
	TPtrC searchParam;
	TBuf16<20> sectionName(_L("FindInstance"));
	TBuf16<20> sectionNameCopy(_L(""));
	TInt count=1;
	TBool sectionRead;

	// Active Scheduler for InstanceView to store the no. of Instances
	TRequestStatus status(KRequestPending);
	CTestCalInterimApiInstanceData* instanceData = new (ELeave) CTestCalInterimApiInstanceData(&status);
	CleanupStack::PushL(instanceData);
	iInstanceView=CCalInstanceView::NewL(*iSession,*instanceData);
	CActiveScheduler::Add(instanceData);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(instanceData);

	sectionNameCopy=sectionName;
	sectionNameCopy.AppendNum(count);
	sectionRead=GetStringFromConfig(sectionNameCopy,  KIniStartDateTime,  startDateTimePtr);
	if (EFalse == sectionRead)
		{
		ERR_PRINTF2(_L("FindInstance Section Name or %s Not defined in .INI File"), &KIniStartDateTime);
		SetTestStepResult(EFail);
		}

	// for loop to fetch input from ini file
	while (sectionRead)
		{
		if(!GetStringFromConfig(sectionNameCopy,  KIniEndDateTime,  endDateTimePtr) ||
			!GetStringFromConfig(sectionNameCopy,  KIniSearchParam,  searchParam)||
			!GetIntFromConfig(sectionNameCopy,  	KIniInstanceFilter,  filter))
			{
			ERR_PRINTF4(_L("Problem in reading values from ini. \n Expected field is %s\n%s\n%d"),&KIniEndDateTime,&KIniSearchParam,&KIniInstanceFilter);
			SetTestStepResult(EFail);
			}
		else
			{

			INFO_PRINTF2(_L("Input datas for Findnstance%d"),count);
			INFO_PRINTF2(_L("Startdatetime : %S"),&startDateTimePtr);
			INFO_PRINTF2(_L("EndDateTime : %S"),&endDateTimePtr);
			INFO_PRINTF2(_L("SearchParam : %S"),&searchParam);
			INFO_PRINTF2(_L("Filter: %d"),filter);

			// To convert Tptrc to TTime
			iStartDateTime.Set(startDateTimePtr);
			iEndDateTime.Set(endDateTimePtr);

			// to change TTime as TCalTime
			iCalTime1.SetTimeLocalL(iStartDateTime);
			iCalTime2.SetTimeLocalL(iEndDateTime);


			// Time range for the instances ie, Startdatetime and enddatetime
			CalCommon::TCalTimeRange calTimeRange(iCalTime1,iCalTime2);

			// search parameter to find the Instances
			CCalInstanceView::TCalSearchParams calSearchParams(searchParam\
		   									  ,CalCommon::EFoldedTextSearch);
			// Filter to find the specific instances
			CalCommon::TCalViewFilter calFilter=(CalCommon::TCalViewFilter)filter;

			// Create CCalInstance Array - instanceViewList
			RPointerArray<CCalInstance> instanceViewList;
			CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &instanceViewList));

			// Find the anniversary instances for the specified parameters
			TRAPD(err,iInstanceView->FindInstanceL(instanceViewList,calFilter\
		    										, calTimeRange, calSearchParams));
			INFO_PRINTF2(_L("The error occured in findinstance is %d"),err);
			TInt viewList=instanceViewList.Count();
			INFO_PRINTF2(_L("Instance list is found and no of instances is %d"),viewList);
			if(viewList == 0)
				{
				INFO_PRINTF1(_L("The specified instances are not present"));
				}
			CleanupStack::Pop(&instanceViewList);
			instanceViewList.ResetAndDestroy();

			} // End of else
		sectionNameCopy.Zero();
		++count;
		sectionNameCopy=sectionName;
		sectionNameCopy.AppendNum(count);	
		sectionRead=GetStringFromConfig(sectionNameCopy,  KIniStartDateTime,  startDateTimePtr);
		} // while
	} // End of function

/**
@SYMTestCaseID PIM-APP-ENGINES-CALINTERIMAPI-CIT-0016
*/
/**
@test
@internalTechnology
@SYMTestCaseID PIM-APP-ENGINES-CALINTERIMAPI-CIT-0017
@SYMTestType CIT
@SYMTestPriority High
@SYMPREQ 1098
@SYMFssID 3.2.4 008, 009
@SYMTestCaseDesc Find the next day and previous day of the instance
@SYMTestActions Find the Previous day and next day of the specified instances.
@SYMTestExpectedResults CCalInstanceView should have found the next day and
previous day of instance.
*/

void CTestCalInterimApiInstance::NextPrevInstancesL()
	{
	// Local variables declaration
	TPtrC startDateTimePtr;
	TInt filter;
	TBuf16<20> sectionName(_L("NextPrevInstance"));
	TBuf16<20> sectionNameCopy(_L(""));
	TInt count=1;
	TBool sectionRead;

	sectionNameCopy=sectionName;
	sectionNameCopy.AppendNum(count);
	sectionRead=GetStringFromConfig(sectionNameCopy,  KIniStartDateTime,  startDateTimePtr);
	if (EFalse == sectionRead)
		{
		ERR_PRINTF2(_L("NextPrevInstance Section Name or %s Not defined in .INI File"), &KIniStartDateTime);
		SetTestStepResult(EFail);
		}

	// for loop to fetch the input from ini file
	while (sectionRead)
		{
		if(!GetIntFromConfig(sectionNameCopy,  	KIniInstanceFilter,  filter))
			{
			ERR_PRINTF2(_L("Problem in reading values from ini. \n Expected field is %d"),&KIniInstanceFilter);
			SetTestStepResult(EFail);
			}
		else
			{

			INFO_PRINTF2(_L("The datas for NetPrevInstaces%d is"),count);
			INFO_PRINTF2(_L("StartDateTime %S"),&startDateTimePtr);
			INFO_PRINTF2(_L("InstanceFilter %d"),filter);

			// To convert Tptrc to TTime
			TInt err1=iStartDateTime.Set(startDateTimePtr);

			// To change TTime as TCalTime
			iCalTime1.SetTimeLocalL(iStartDateTime);

			// Filter to find the specific instances
			CalCommon::TCalViewFilter calFilter=(CalCommon::TCalViewFilter)filter;

			// To find the next instance of the entry
			TCalTime NextDay=iInstanceView->NextInstanceL(calFilter,iCalTime1);
			INFO_PRINTF2(_L("The next day for this instance is %d"),NextDay.TimeUtcL().Int64());

			// To find the previous instance of the entry.
			TCalTime PrevDay=iInstanceView->PreviousInstanceL(calFilter,iCalTime1);
			INFO_PRINTF2(_L("The previous day for this instance is %d"),PrevDay.TimeUtcL().Int64());

			} // End of else
		sectionNameCopy.Zero();
		++count;
		sectionNameCopy=sectionName;
		sectionNameCopy.AppendNum(count);	
		sectionRead=GetStringFromConfig(sectionNameCopy,  KIniStartDateTime,  startDateTimePtr);
		} // while
	} // End of function

/**
@test
@internalTechnology
@SYMTestCaseID PIM-APP-ENGINES-CALINTERIMAPI-CIT-0014
@SYMTestType CIT
@SYMTestPriority High
@SYMPREQ 1098
@SYMFssID 3.2.4 004
@SYMTestCaseDesc Delete the instances of the entry.
@SYMTestActions Find the specified Instances.Delete the found instances.
@SYMTestExpectedResults CCalInstanceView should have deleted the instances from the entry.
*/

void CTestCalInterimApiInstance::DeleteInstancesL()
	{
	// Local variables declaration
	TPtrC startDateTimePtr;
	TPtrC endDateTimePtr;
	TPtrC searchParam;
	TInt filter;
	TInt recurrenceRange;
	TBuf16<20> sectionName(_L("FindDeleteInstance"));
	TBuf16<20> sectionNameCopy(_L(""));
	TInt count=1;
	TBool sectionRead;


	sectionNameCopy=sectionName;
	sectionNameCopy.AppendNum(count);
	sectionRead=GetStringFromConfig(sectionNameCopy,  KIniStartDateTime,  startDateTimePtr);
	if (EFalse == sectionRead)
		{
		ERR_PRINTF2(_L("FindDeleteInstance Section Name or %s Not defined in .INI File"), &KIniStartDateTime);
		SetTestStepResult(EFail);
		}

	// for loop to fetch the input from ini file
	while (sectionRead)
		{
		if(!GetStringFromConfig(sectionNameCopy,  KIniEndDateTime,  endDateTimePtr) ||
			!GetStringFromConfig(sectionNameCopy,  KIniSearchParam,  searchParam)||
			!GetIntFromConfig(sectionNameCopy,  	KIniInstanceFilter,  filter)||
			!GetIntFromConfig(sectionNameCopy,  KIniRecurrenceRange,  recurrenceRange))
			{
			ERR_PRINTF6(_L("Problem in reading values from ini.Expected field is %s\n%s\n%s\n%d\n%d"),\
			&KIniStartDateTime,&KIniEndDateTime,&KIniSearchParam,&KIniInstanceFilter,&KIniRecurrenceRange);
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF2(_L("Input datas for DeleteInstance%d"),count);
			INFO_PRINTF2(_L("Startdatetime : %S"),&startDateTimePtr);
			INFO_PRINTF2(_L("EndDateTime : %S"),&endDateTimePtr);
			INFO_PRINTF2(_L("SearchParam : %S"),&searchParam);
			INFO_PRINTF2(_L("InstanceFilter: %d"),filter);
			INFO_PRINTF2(_L("Recurrencerange %d"),recurrenceRange);


			// To convert Tptrc to TTime
			iStartDateTime.Set(startDateTimePtr);
			iEndDateTime.Set(endDateTimePtr);

			// to change TTime as TCalTime
			iCalTime1.SetTimeLocalL(iStartDateTime);
			iCalTime2.SetTimeLocalL(iEndDateTime);

			// Time range for the instances
			CalCommon::TCalTimeRange calTimeRange(iCalTime1,iCalTime2);

			// Filter to find the specific instances
			CalCommon::TCalViewFilter calFilter=(CalCommon::TCalViewFilter)filter;

			// Search data to find the instances
			CCalInstanceView::TCalSearchParams calSearchParams(searchParam\
		    									,CalCommon::EExactTextSearch);
			// Array to store the found instances
			RPointerArray<CCalInstance> instanceList;
			CleanupStack::PushL(TCleanupItem(DestroyRPointerArray, &instanceList));

			// To find the specified instances
			TRAPD(err,iInstanceView->FindInstanceL(instanceList, calFilter \
		    							, calTimeRange, calSearchParams));
			INFO_PRINTF2(_L("The error in FindInstanceL is %d"),err);
			TInt list=instanceList.Count();
			if(list==0)
				{
				INFO_PRINTF1(_L("The instances are not found for the given data "));
				INFO_PRINTF1(_L("The given data is invalid"));
				}
			else
				{
				INFO_PRINTF2(_L("The no of instances found are %d"),list);
				iInstance=instanceList[0];

				CalCommon::TRecurrenceRange recurrence=CalCommon::TRecurrenceRange\
			    													(recurrenceRange);
				// to delete the instances
				TRAPD(err2,iInstanceView->DeleteL(iInstance,recurrence));
				
				// instanceList[0] is already released in DeleteL
				instanceList[0] = NULL;
				
				instanceList.ResetAndDestroy();

				INFO_PRINTF2(_L("The error occured in deleteL is %d"),err2);

				// To find whether the instance is deleted or not
				TRAPD(err3,iInstanceView->FindInstanceL(instanceList, calFilter\
													, calTimeRange, calSearchParams));
				INFO_PRINTF2(_L("error in findInstanceL is %d"),err3);
				TInt list1=instanceList.Count();
				if(list1<list)
					{
					INFO_PRINTF2(_L("The instance is deleted and the no. of instances remaining is: %d")\
			    																			  ,list1);
					}
				else
					{
					INFO_PRINTF1(_L("The test step is failed"));
					SetTestStepResult(EFail);
					}
				}

			CleanupStack::Pop(&instanceList);
			instanceList.ResetAndDestroy();
			iInstance=NULL;
			} // End of else
		sectionNameCopy.Zero();
		++count;
		sectionNameCopy=sectionName;
		sectionNameCopy.AppendNum(count);	
		sectionRead=GetStringFromConfig(sectionNameCopy,  KIniStartDateTime,  startDateTimePtr);
		} // while
	} // End of function


