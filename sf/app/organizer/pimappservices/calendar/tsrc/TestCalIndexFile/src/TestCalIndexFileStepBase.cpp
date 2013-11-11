// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestCalIndexFileStepBase.h"
#include "TestCalIndexFileDefs.h"
#include <f32file.h>
#include <bautils.h>

#include <calsession.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <calinstance.h>
#include "caltestlib.h"

// Constant Literals used.
_LIT(KInfoPercentageCompleted,		"Percentage Completed %d");
_LIT(KErrPercentageCompleted,		"Error progress not required");
_LIT(KErrCompleted,					"Error Completed %d");

_LIT(KDefaultDir, "private\\10003a5b");
_LIT(KDefaultFile, "calendar");
_LIT(KDefaultIdx, "calendaridx");
_LIT(KDrive, "drive");
_LIT(KCalendarName, "calendarname");
_LIT(KIndexName, "indexname");
_LIT(KIndexFileDataDir, "\\CalIndexFileTestData\\");
_LIT(KDefaultTestDrive, "C:");
_LIT(KDoubleBackslash, "\\");


TFileName GetFullFileName(const TDesC& aPath)
	{
	TFileName fileName;
#ifdef __WINS__
	fileName.Append(_L("c:"));
#else
	TFileName fullName = RProcess().FileName();
	TParsePtrC parse(fullName);
	fileName.Append(parse.Drive());
#endif
	fileName.Append(aPath);
	return fileName;
	}

/*@}*/

CTestCalIndexFileStepBase::CTestCalIndexFileStepBase()
:	CTestStep()
,	iNotifyProgress(ETrue)
,	iActiveScheduler(NULL)
,	iFsSessionConnected(EFalse)
,	iSession(NULL)
	{
	}

CTestCalIndexFileStepBase::~CTestCalIndexFileStepBase()
	{
	delete iSession;
	delete iActiveScheduler;
	iFsSession.Close();
	}

void CTestCalIndexFileStepBase::Progress(TInt aPercentageCompleted)
	{
	if ( iNotifyProgress )
		{
		INFO_PRINTF2(KInfoPercentageCompleted, aPercentageCompleted);
		}
	else
		{
		ERR_PRINTF1(KErrPercentageCompleted);
		SetTestStepResult(EFail);
		}
	}

void CTestCalIndexFileStepBase::Completed(TInt aError)
	{
	if ( aError!=KErrNone )
		{
		ERR_PRINTF2(KErrCompleted, aError);
		SetTestStepResult(EFail);
		SetTestStepError(aError);
		}
	CActiveScheduler::Stop();
	}

TBool CTestCalIndexFileStepBase::NotifyProgress()
	{
	return iNotifyProgress;
	}

TVerdict CTestCalIndexFileStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
*/
	{
	INFO_PRINTF1(_L("enter CTestCalIndexFileStepBase::doTestStepPreambleL()"));
	iActiveScheduler = new(ELeave)CActiveScheduler;
 	CActiveScheduler::Install(iActiveScheduler);
	INFO_PRINTF1(_L("exit CTestCalIndexFileStepBase::doTestStepPreambleL()"));
	return (EPass);
	}

TVerdict CTestCalIndexFileStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 */
	{
	return (EPass);
	}

	
void CTestCalIndexFileStepBase::SetUpCalDirL()
	{
	// determine calendar directory from ini
	// if not present use default
 
	TPtrC drive;
	TBool readRes = EFalse;
	TFileName driveName;
	readRes = GetStringFromConfig(ConfigSection(), KDrive, drive);
	if (readRes)
		{
		iDriveName = drive;
		}
	else
		{
		iDriveName = KDefaultTestDrive();
		}
		
	iDir = iDriveName;
	iDir.Append(KDoubleBackslash());
	iDir.Append(KDefaultDir());
	iDir.Append(KDoubleBackslash());
	
	// store the relevant file names for the calendar and
	// index and delete them from the directory if they already
	// exist
	if (!iFsSessionConnected)
		{
		iFsSession.Connect();
		iFsSessionConnected = ETrue;
		}
	
	// calendar file
	TPtrC calendarName;

	readRes = GetStringFromConfig(ConfigSection(), KCalendarName, calendarName);
	if (readRes)
		{
		iCalendarName = calendarName;
		}
	else
		{
		iCalendarName = KDefaultFile();
		}
		
	TFileName fName;
	fName = iDir;
	fName.Append(iCalendarName);
	if (BaflUtils::FileExists(iFsSession, fName))
		{
		iFsSession.Delete(fName);
		}

	// index file
	TPtrC indexName;
	readRes = EFalse;
	readRes = GetStringFromConfig(ConfigSection(), KIndexName, indexName);
	if (readRes)
		{
		iIndexName = indexName;
		}
	else
		{
		iIndexName = KDefaultIdx();
		}
	
	fName = iDir;
	fName.Append(iIndexName);
	if (BaflUtils::FileExists(iFsSession, fName))
		{
		iFsSession.Delete(fName);
		}

	iFsSession.Close();
	iFsSessionConnected = EFalse;
	INFO_PRINTF1(_L("exit CTestCalIndexFileStepBase::SetUpCalDirL()"));
	}
	
void CTestCalIndexFileStepBase::MakeCalendarFileName(TFileName& aCalFile)
	{
	aCalFile = iDriveName;
	aCalFile.Append(iCalendarName);
	}

void CTestCalIndexFileStepBase::AddCalendarFileL()
	{
	// copy file from stored data area to target directory
	TFileName trgtName;
	trgtName = iDir;
	trgtName.Append(iCalendarName);
	
	TFileName srcName = GetFullFileName(KIndexFileDataDir);
	srcName.Append(iCalendarName);
	
	if (!iFsSessionConnected)
		{
		iFsSession.Connect();
		iFsSessionConnected = ETrue;
		}
	CFileMan* fileman = CFileMan::NewL(iFsSession);
	CleanupStack::PushL(fileman);
	fileman->Copy(srcName, trgtName);
	CleanupStack::PopAndDestroy(fileman);
	iFsSession.Close();
	iFsSessionConnected = EFalse;
	}
	
void CTestCalIndexFileStepBase::AddIndexFileL()
	{
	// copy file from stored data area to target directory
	TFileName trgtName;
	trgtName = iDir;
	trgtName.Append(iIndexName);
	
	TFileName srcName = GetFullFileName(KIndexFileDataDir);
	srcName.Append(iIndexName);
	
	if (!iFsSessionConnected)
		{
		iFsSession.Connect();
		iFsSessionConnected = ETrue;
		}
	CFileMan* fileman = CFileMan::NewL(iFsSession);
	CleanupStack::PushL(fileman);
	fileman->Copy(srcName, trgtName);
	CleanupStack::PopAndDestroy(fileman);
	iFsSession.Close();
	iFsSessionConnected = EFalse;
	}

void CTestCalIndexFileStepBase::OpenAgendaL()
	{
	// create a calendar session using the filename  & directory 
	// retrieved earlier
	
	iSession = CCalSession::NewL();
	
	TFileName calName;
	MakeCalendarFileName(calName);
	iSession->OpenL(calName);
	}
	
void CTestCalIndexFileStepBase::CloseAgendaL()
	{
	// close the session to which we have a pointer
	delete iSession;
	iSession = NULL;

	// a little pause to let the index file get written
	TTimeIntervalMicroSeconds32 delay = 7000000; // 7 s - we need to wait more than 5s due to the delay introduced by CR0979.
	User::After(delay);	
	}
	
	
TBool CTestCalIndexFileStepBase::CheckIndexFilePresentL()
	{
	// check if indexfile is in directory
	TFileName idxName;
	idxName = iDir;
	idxName.Append(iIndexName);
	
	if (!iFsSessionConnected)
		{
		iFsSession.Connect();
		iFsSessionConnected = ETrue;
		}
	RFile idxFile;
	
	TBool result = BaflUtils::FileExists(iFsSession, idxName);
	iFsSession.Close();
	iFsSessionConnected = EFalse;
	return result;
	}

	
TBool CTestCalIndexFileStepBase::CheckCalendarFilePresentL()
	{
	// check if calendar file is in directory
	TFileName calName;
	calName = iDir;
	calName.Append(iCalendarName);
	
	if (!iFsSessionConnected)
		{
		iFsSession.Connect();
		iFsSessionConnected = ETrue;
		}
	RFile calFile;
	
	TBool result = BaflUtils::FileExists(iFsSession, calName);
	iFsSession.Close();
	iFsSessionConnected = EFalse;
	return result;
	}

void CTestCalIndexFileStepBase::GetEntryInfoFromConfigL(RPointerArray<CConfigTestEntryInfo>& aEntriesInfo, TInt& aNumEntries, TBool aPerformCrudOps)
	{
	// for the base class we don't care about CRUD operations
	aPerformCrudOps = aPerformCrudOps;
	
	for (TInt i = 0; i < aNumEntries; i++)
		{
		HBufC* entryBuf = HBufC::NewL(KTCIEntry().Length()+ 4); // add 4 to allow for 4 digits of entries
		CleanupStack::PushL(entryBuf);
		*entryBuf = KTCIEntry();
		TPtr ptr(entryBuf->Des());
		ptr.AppendNum(i+1);
		TPtrC entryString;
		TBool readRes = EFalse;
		readRes = GetStringFromConfig(ConfigSection(), ptr, entryString);
		if (!readRes)
			{
			INFO_PRINTF2(_L("Error in CTestCalIndexFileStepBase::GetEntryInfoFromConfigL - entry%d not found in config file"),(i+1));
			User::Leave(KErrNotFound);
			}
		CConfigTestEntryInfo* inf = new(ELeave)CConfigTestEntryInfo();
		CleanupStack::PushL(inf);
		ParseEntryStringL(*inf, entryString);
		aEntriesInfo.Append(inf);
		CleanupStack::Pop(inf); // now in array to be cleaned up
		CleanupStack::PopAndDestroy(entryBuf);
		}

	}
	
	
TBool CTestCalIndexFileStepBase::ValidateDbContentsL(TBool aPerformCrudOps)
	{
	// for each entry in the database validate ALL relevant
	// fields against info for the entry in the ini file
	TInt numEntries;
	TBool readRes = EFalse;
	readRes = GetIntFromConfig(ConfigSection(),KTCINumEntries, numEntries);
	if (!readRes)
		{
		INFO_PRINTF1(_L("Error in CTestCalIndexFileStepBase::ValidateDbContentsL - numentriespresent not found in config file"));
		User::Leave(KErrNotFound);
		}
	
	if (numEntries == 0)
		{
		return ETrue;
		}
	
	RPointerArray<CConfigTestEntryInfo> entriesInfo;	
	CleanupResetAndDestroyPushL(entriesInfo);	

	GetEntryInfoFromConfigL(entriesInfo, numEntries, aPerformCrudOps);
			
	// for each entry in the list of entries, see if we can find a 
	// match in the database using an instance view
	for (TInt j = 0; j < numEntries; j++)
		{
		INFO_PRINTF2(_L("Trying to validate entry info - %d"),(j+1));
		CConfigTestEntryInfo* infoToUse = entriesInfo[j];
		
		CCalProgress* progress = new(ELeave)CCalProgress;
		CleanupStack::PushL(progress);  //1
	
		CCalInstanceView* view = CCalInstanceView::NewL(*iSession, *progress);
		CleanupStack::PushL(view); //2
		
		CActiveScheduler::Start();
		
		CalCommon::TCalViewFilter filter = CalCommon::EIncludeAppts;
		switch (infoToUse->iType)
			{
			case (CCalEntry::EAppt):
				filter = CalCommon::EIncludeAppts;
				break;
			case (CCalEntry::ETodo):
				filter = CalCommon::EIncludeCompletedTodos | CalCommon::EIncludeIncompletedTodos;
				break;
			case (CCalEntry::EEvent):
				filter = CalCommon::EIncludeEvents;
				break;
			case (CCalEntry::EReminder):
				filter = CalCommon::EIncludeReminder;
				break;
			case (CCalEntry::EAnniv):
				filter = CalCommon::EIncludeAnnivs;
				break;
			default:
				User::Leave(KErrCorrupt);
				break;
			}
		RPointerArray<CCalInstance> instances;
		CleanupResetAndDestroyPushL(instances);

		// look for instances between (startdate -1 day) and
		// (enddate + 1 day)
		TTimeIntervalDays oneDay(1);
		TTime beginTTime = (infoToUse->iStartTime) - oneDay;
		TTime endTTime = (infoToUse->iEndTime) + oneDay;
		TCalTime begin;
		begin.SetTimeUtcL(beginTTime);
		TCalTime end;
		end.SetTimeUtcL(endTTime);
		CalCommon::TCalTimeRange timerange(begin, end);	
		
		view->FindInstanceL(instances, filter, timerange);
		
		TInt counter = 0;
		TInt max = instances.Count();
		INFO_PRINTF2(_L("%d instances to compare against"),max);
		TBool found = EFalse;
		while ((found == EFalse) && (counter < max))
			{
			INFO_PRINTF3(_L("compare info %d to entry %d"),(j+1), (counter+1));
			if (infoToUse->CompareToEntryL((instances[counter])->Entry()))
				{
				INFO_PRINTF3(_L("Entry info %d matched instance %d"),(j+1), (counter+1));
				found = ETrue;
				}
			else
				{
				counter++;
				}
			}
		CleanupStack::PopAndDestroy(&instances);
		CleanupStack::PopAndDestroy(view);
		CleanupStack::PopAndDestroy(progress);
		
		if (!found)
			{
			CleanupStack::PopAndDestroy(&entriesInfo);
			INFO_PRINTF2(_L("Could not validate entry info - %d"),(j+1));
			return EFalse;
			}
		}
	CleanupStack::PopAndDestroy(&entriesInfo);
	return ETrue;
	}

	
void CTestCalIndexFileStepBase::AddEntryL()
	{
	TPtrC entryString;
	TBool readRes = EFalse;
	readRes = GetStringFromConfig(ConfigSection(), KAddEntry, entryString);
	
	if (!readRes)
		{
		INFO_PRINTF1(_L("Error in CTestCalIndexFileStepBase::AddEntryL - section not found in config file"));
		User::Leave(KErrNotFound);
		}
		
	CConfigTestEntryInfo* entryInf = new(ELeave)CConfigTestEntryInfo();
	CleanupStack::PushL(entryInf);
	ParseEntryStringL(*entryInf, entryString);
	CCalEntry* newEntry = CreateEntryL(*entryInf);
	CleanupStack::PushL(newEntry);
	
	CCalProgress* progress = new(ELeave)CCalProgress;
	CleanupStack::PushL(progress);
	
	CCalEntryView* view = CCalEntryView::NewL(*iSession, *progress);
	CleanupStack::PushL(view);
	
	CActiveScheduler::Start();
			

	TInt cnt;
	RPointerArray<CCalEntry> entries;
	entries.Append(newEntry);
	view->StoreL(entries, cnt);

	CleanupStack::PopAndDestroy(4); //entryInf, newEntry, progress, view
	}
	
CCalEntry* CTestCalIndexFileStepBase::CreateEntryL(CConfigTestEntryInfo& aInfo)
	{
	// build the uid from the constant we always use here
	TInt len = KTCIUid().Length();
	
	HBufC8* uId = HBufC8::NewLC(len);
	*uId = KTCIUid;

	CCalEntry* retEntry = CCalEntry::NewL(aInfo.iType, uId, aInfo.iMethod, 0);
	CleanupStack::Pop(uId); // entry takes ownership
	CleanupStack::PushL(retEntry);
	
	
	TCalTime start;
	start.SetTimeUtcL(aInfo.iStartTime);
	TCalTime end;
	end.SetTimeUtcL(aInfo.iEndTime);
	retEntry->SetStartAndEndTimeL(start, end);
	
	retEntry->SetSummaryL(*(aInfo.iSummary));
	retEntry->SetDescriptionL(*(aInfo.iDescription));
	retEntry->SetLocationL(*(aInfo.iLocation));
	
	if (aInfo.iRepeats)
		{
		TCalRRule rRule(aInfo.iRptType);
		rRule.SetCount(aInfo.iRptCount);
		rRule.SetDtStart(start);
		rRule.SetInterval(1);
		retEntry->SetRRuleL(rRule);
		}
	CleanupStack::Pop(retEntry);
	return (retEntry);
	}
	
const TUint KMaxEntryStringLen = 250;	// arbitrary size, can be chanegd to match needs
									  	// of format described below

const TUint KMaxEntryPartStringLen = 50;	// arbitrary size, can be chanegd to match needs
									  		// of format described below
	// The string in the config file for the entry has one of the following 
	// formats:
	//    (1)  type;starttime;endtime;summary;description;location;method
	//    (2)  type;starttime;endtime;summary;description;location;method;repeating;rpttype;rptnum
	// where
	//	type = string that fits with one of the entry types
	//  starttime, endtime = TDateTime string
	//	summary, description, location = any string
	//	method = string that fits with one of the method types
	//  repeating = bool string
	//	rpttype = string that fits with one of the repeat types
	//  rptnum = integer string for number of repeats
	//
	// example1:  appt;20050622:122500;20050623:154500;a summary;a description;a location;none;true;weekly;5;
	// example2:  appt;20050622:122500;20050623:154500;a summary;a description;a location;none;false;
	// 
	// Refer to TestCalIndexFileDefs.h for the constants for type, method, and rpttype
									  
void CTestCalIndexFileStepBase::ParseEntryStringL(CConfigTestEntryInfo& aInfo,  TPtrC& aString)
	{
	TBuf<KMaxEntryStringLen> data(aString);
	
	TBuf<KMaxEntryPartStringLen> item;
	
	GoToNextEntryItem(data, item);
		
	// get the entry type
	if (0 == item.Compare(KTCIEntryTypeAppt))
		{
		aInfo.iType = CCalEntry::EAppt;
		}
	else if (0 == item.Compare(KTCIEntryTypeToDo))
		{
		aInfo.iType = CCalEntry::ETodo;
		}
	else if (0 == item.Compare(KTCIEntryTypeEvent))
		{
		aInfo.iType = CCalEntry::EEvent;
		}
	else if (0 == item.Compare(KTCIEntryTypeRemind))
		{
		aInfo.iType = CCalEntry::EReminder;
		}
	else if (0 == item.Compare(KTCIEntryTypeAnniv))
		{
		aInfo.iType = CCalEntry::EAnniv;
		}
	else
		{
		User::Leave(KErrArgument);
		}

	// get the start time
	GoToNextEntryItem(data, item);
	TTime start(item);
	aInfo.iStartTime = start;
	
	// get the end time
	GoToNextEntryItem(data, item);
	TTime end(item);
	aInfo.iEndTime = end;
	
	
	// use this integer length for all pure strings to be stored
	TInt len = 0;
	
	// get the summary
	GoToNextEntryItem(data, item);
	len = item.Length();
	if (aInfo.iSummary)
		{
		delete aInfo.iSummary;
		aInfo.iSummary = NULL;
		}
	aInfo.iSummary = HBufC::NewL(len);
	*(aInfo.iSummary) = item;

	// get the description
	GoToNextEntryItem(data, item);
	len = item.Length();
	if (aInfo.iDescription)
		{
		delete aInfo.iDescription;
		aInfo.iDescription = NULL;
		}
	aInfo.iDescription = HBufC::NewL(len);
	*(aInfo.iDescription) = item;
	
	// get the location
	GoToNextEntryItem(data, item);
	len = item.Length();
	if (aInfo.iLocation)
		{
		delete aInfo.iLocation;
		aInfo.iLocation = NULL;
		}
	aInfo.iLocation = HBufC::NewL(len);
	*(aInfo.iLocation) = item;
	
	// get the method
	GoToNextEntryItem(data, item);
	if (0 == item.Compare(KTCIEntryMethodNone))
		{
		aInfo.iMethod = CCalEntry::EMethodNone;
		}
	else if (0 == item.Compare(KTCIEntryMethodPub))
		{
		aInfo.iMethod = CCalEntry::EMethodPublish;
		}
	else if (0 == item.Compare(KTCIEntryMethodReq))
		{
		aInfo.iMethod = CCalEntry::EMethodRequest;
		}
	else if (0 == item.Compare(KTCIEntryMethodReply))
		{
		aInfo.iMethod = CCalEntry::EMethodReply;
		}
	else if (0 == item.Compare(KTCIEntryMethodAdd))
		{
		aInfo.iMethod = CCalEntry::EMethodAdd;
		}
	else if (0 == item.Compare(KTCIEntryMethodCancel))
		{
		aInfo.iMethod = CCalEntry::EMethodCancel;
		}
	else if (0 == item.Compare(KTCIEntryMethodRefresh))
		{
		aInfo.iMethod = CCalEntry::EMethodRefresh;
		}
	else if (0 == item.Compare(KTCIEntryMethodCounter))
		{
		aInfo.iMethod = CCalEntry::EMethodCounter;
		}
	else if (0 == item.Compare(KTCIEntryMethodDeclineCounter))
		{
		aInfo.iMethod = CCalEntry::EMethodDeclineCounter;
		}
	else
		{
		User::Leave(KErrArgument);
		}

	// check if repeating
	GoToNextEntryItem(data, item);
	if (0 == item.Compare(KTCIEntryTrue))
		{
		aInfo.iRepeats = ETrue;
		
		// get rpttype
		GoToNextEntryItem(data, item);
		if (0 == item.Compare(KTCIEntryRptDaily))
			{
			aInfo.iRptType = TCalRRule::EDaily;
			}
		else if (0 == item.Compare(KTCIEntryRptWeekly))
			{
			aInfo.iRptType = TCalRRule::EWeekly;
			}
		else if (0 == item.Compare(KTCIEntryRptMonthly))
			{
			aInfo.iRptType = TCalRRule::EMonthly;
			}
		else if (0 == item.Compare(KTCIEntryRptYearly))
			{
			aInfo.iRptType = TCalRRule::EYearly;
			}
		else
			{
			User::Leave(KErrArgument);
			}
	
		// get rptnum
		GoToNextEntryItem(data, item);
		TLex mylex(item);
		TInt lexErr = mylex.Val(aInfo.iRptCount);
		User::LeaveIfError(lexErr);
		}
	else
		{
		aInfo.iRepeats = EFalse;
		}
	}


void CTestCalIndexFileStepBase::GoToNextEntryItem(TDes& aData, TDes& aItem)
	{
	TInt pos;
	pos = aData.Locate(';');
	aItem = aData.Left(pos);
	aData.Delete(0, pos+1);
	}





// CConfigTestEntryInfo
//
CConfigTestEntryInfo::CConfigTestEntryInfo()
	{
	iSummary = NULL;
	iDescription = NULL;
	iLocation = NULL;
	}
	
CConfigTestEntryInfo::~CConfigTestEntryInfo()
	{
	if (iSummary)
		{
		delete iSummary;
		iSummary = NULL;
		}
	if (iDescription)
		{
		delete iDescription;
		iDescription = NULL;
		}
	if (iLocation)
		{
		delete iLocation;
		iLocation = NULL;
		}
	}

TBool CConfigTestEntryInfo::CompareToEntryL(CCalEntry& aEntry)
	{
	// This method assumes a pass.
	// If any check fails, it immediately returns EFalse since 
	// there is no sense checking any further.
	
	// NB: Implementers of any future test checks should be sure
	//     to include appropriate debug print information about
	//     the failure within the check method itself.
	if (!CheckTypeL(aEntry))
		{
		RDebug::Print(_L("Failed checking Entry type"));
		RDebug::Print(_L("Expected %d, Actual %d"),iType, aEntry.EntryTypeL());
		return EFalse;
		}
		
	if (!CheckTimesL(aEntry))
		{
		RDebug::Print(_L("Failed checking Entry times"));
		return EFalse;
		}
		
	if (!CheckSummaryL(aEntry))
		{
		RDebug::Print(_L("Failed checking Entry summary"));
		return EFalse;
		}
		
	if (!CheckLocationL(aEntry))
		{
		RDebug::Print(_L("Failed checking Entry location"));
		return EFalse;
		}
		
	if (!CheckMethodL(aEntry))
		{
		RDebug::Print(_L("Failed checking Entry method"));
		return EFalse;
		}
		
	if (!CheckRRuleL(aEntry))
		{
		RDebug::Print(_L("Failed checking Entry repeat rule"));
		return EFalse;
		}
		
	// all checks passed, return true
	return ETrue;
	}

TBool CConfigTestEntryInfo::CheckTypeL(CCalEntry& aEntry)
	{
	return (iType == aEntry.EntryTypeL());
	}
	
TBool CConfigTestEntryInfo::CheckTimesL(CCalEntry& aEntry)
	{
	if (iType == CCalEntry::EReminder)
		{
		return (iStartTime == aEntry.StartTimeL().TimeUtcL());
		}
	else
		{
		return ((iStartTime == aEntry.StartTimeL().TimeUtcL()) &&
					(iEndTime == aEntry.EndTimeL().TimeUtcL()));
		}
	}
	
TBool CConfigTestEntryInfo::CheckSummaryL(CCalEntry& aEntry)
	{
	return (*iSummary == aEntry.SummaryL());
	}
	
TBool CConfigTestEntryInfo::CheckDescriptionL(CCalEntry& aEntry)
	{
	return (*iDescription == aEntry.DescriptionL());
	}
	
TBool CConfigTestEntryInfo::CheckLocationL(CCalEntry& aEntry)
	{
	return (*iLocation == aEntry.LocationL());
	}
	
TBool CConfigTestEntryInfo::CheckMethodL(CCalEntry& aEntry)
	{
	return (iMethod == aEntry.MethodL());
	}
	
TBool CConfigTestEntryInfo::CheckRRuleL(CCalEntry& aEntry)
	{
	if (!iRepeats)
		{
		// no repeats, so check is superfluous
		// we keep the logic in here, therefore we don't
		// fail the check.
		return ETrue;  
		}
	
	TCalRRule theRule;
	if (!aEntry.GetRRuleL(theRule))
		{
		return EFalse; // no rule present, should not have got here
		}
		
	return ((iRptType == theRule.Type()) && (iRptCount == theRule.Count()));
	}



CCalProgress::CCalProgress()
:CActive(EPriorityStandard), iProgress(0),iCompleted(EFalse)
	{
	CActiveScheduler::Add(this);
	}
	
CCalProgress::~CCalProgress()
	{
	Cancel();
	}


void CCalProgress::Progress(TInt aPercentageCompleted)
	{
	iProgress = aPercentageCompleted;
	if (aPercentageCompleted<100)
		iCompleted = EFalse;
	}
	
void CCalProgress::Completed(TInt /* aError*/)
	{
	iCompleted = ETrue;
	Start();
	}
	
TBool CCalProgress::NotifyProgress() 	
	{
	return ETrue;
	}

TBool CCalProgress::Completed()
	{
	return iCompleted;
	}
	
TInt  CCalProgress::Progress()
	{
	return iProgress;
	}
	
void  CCalProgress::Reset()
	{
	iCompleted=EFalse;
	}
	
void CCalProgress::Start()
	{
	if (!IsActive())
		{
		SetActive();
		}
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}
	
void CCalProgress::DoCancel()
	{
	Close();
	}
	
void CCalProgress::RunL()
	{
	Close();
	}
	
void CCalProgress::Close()
	{
	CActiveScheduler::Stop();
	}

