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

#include <calrrule.h> 
#include <calalarm.h>
#include <calinstance.h>
#include <tz.h>
#include "T_CrSuiteStepBase.h"

CCalProgress::CCalProgress()
:CActive(EPriorityStandard), iProgress(0),iCompleted(EFalse)
	{
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
		
		
//----------------------------------

_LIT(KRp, "Rp");
_LIT(KAgnInfo, "Create New Agenda File");
_LIT8(KUIDFmt, "UID-%d");
_LIT(KSumFmt, "summary - %d");
_LIT(KDescFmt, "description - %d");
_LIT(KLocFmt, "location - %d");
_LIT(KUnkEntryInfo, "Unknown entry type");
_LIT(KFx, "Fx");
_LIT(KFl, "Fl");
_LIT(KUnkTimeInfo, "Unknown time mode");

CCrTestStepBase::CCrTestStepBase()
	{
	}
	
CCrTestStepBase::~CCrTestStepBase()
	{
	delete iSession;
	delete iScheduler;
	delete iPrevTZ;
	}
	
TVerdict CCrTestStepBase::doTestStepPreambleL()
	{
	TESTL(TestStepResult() == EPass);
	
	iScheduler = new(ELeave)CActiveScheduler;
 	CActiveScheduler::Install(iScheduler);
	iSession  = CCalSession::NewL();
	OpenSessionFileL();
	
	return TestStepResult();
	}

CCalEntry* CCrTestStepBase::CreateEntryL(CCalEntry::TType aType, TTime aStartTime, TTime aEndTime, TCalTime::TTimeMode aMode)
	{
	static TUint8 seqNum=0;
	++seqNum;
	
	TBuf8<MAX_PROPERTY_STRLEN> uidStr;	
	uidStr.Format(KUIDFmt, seqNum);
	
	HBufC8* uId = HBufC8::NewLC(MAX_UID_BUF_SIZE);
	*uId = uidStr;
	
	CCalEntry* entry = CCalEntry::NewL(aType, uId, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(uId); // CCalEntry takes ownership
	CleanupStack::PushL(entry);
		
	TBuf<MAX_LONG_STRLEN> str;
	str.Format(KSumFmt, seqNum);
	entry->SetSummaryL(str);
	
	str.Format(KDescFmt, seqNum);
	
	entry->SetDescriptionL(str);
	
	str.Format(KLocFmt, seqNum);
	entry->SetLocationL(str);

	TCalTime start;
	TCalTime end;
	
	if (aMode == TCalTime::EFloating)
		{
		start.SetTimeLocalFloatingL(aStartTime);
		end.SetTimeLocalFloatingL(aEndTime);
		}
	else
		{
		start.SetTimeUtcL(aStartTime);
		end.SetTimeUtcL(aEndTime);
		}
	entry->SetStartAndEndTimeL(start, end);	
	CleanupStack::Pop(entry);
	
	return entry;
 	}



//Creates entry from text string
// for example string "VTODO;20050622:122500;20050623:154500;Fl;VAPPT;20050624:013300;20050624:013335;Fx;Rp"
// will create two entries:
// 1- Floating Todo, StartDate - 22 Jul 05, 12:25pm, EndDate 23 Jul 05, 3:45pm. non repeating
// 2- Fixed Appointment StartDate - 24 Jul 05, 1:33am, EndDate 24 Jul 05, 1:35pm repeating every day
//
void CCrTestStepBase::CreateEntriesL(TPtrC& aDataPtr, RPointerArray<CCalEntry>& aEntries)
	{

	TESTL(MAX_TEXT_MESSAGE >= aDataPtr.Length());
	TBuf<MAX_TEXT_MESSAGE>  data(aDataPtr);
	
	while (data.Length())
		{
		TBuf<MAX_LONG_STRLEN> item;
		
		// Get entry type
		TInt pos = data.Locate(';');
		item = data.Left(pos);
		data.Delete(0, pos+1);
		
		CCalEntry::TType entryType = CCalEntry::EAppt;
		
		if (0==item.Compare(KToDo))	
			{
			entryType = CCalEntry::ETodo;
			}
		else if (0==item.Compare(KEvent))	
			{
			entryType = CCalEntry::EEvent;
			}
		else if (0==item.Compare(KReminder))	
			{
			entryType = CCalEntry::EReminder;
			}
		else if (0==item.Compare(KAppt))	
			{
			entryType = CCalEntry::EAppt;
			}
		else if (0==item.Compare(KAnniv))	
			{
			entryType = CCalEntry::EAnniv;
			}
		else
			{
			INFO_PRINTF1(KUnkEntryInfo);
			TESTL(EFalse);
			}

		// Get start date
		pos = data.Locate(';');
		item = data.Left(pos);		
		data.Delete(0, pos+1);
		TTime startTime(item);

		// Get end date
		pos = data.Locate(';');
		item = data.Left(pos);		
		data.Delete(0, pos+1);
		TTime endTime(item);		

		// Get time mode
		pos = data.Locate(';');
		item = data.Left(pos);		
		data.Delete(0, pos+1);

		TCalTime::TTimeMode timemode = TCalTime::EFixedUtc; 
		if (0==item.Compare(KFx))	
			{
			timemode = TCalTime::EFixedUtc;
			}
		else if (0==item.Compare(KFl))	
			{
			timemode = TCalTime::EFloating;
			}
		else
			{
			INFO_PRINTF1(KUnkTimeInfo);
			TESTL(EFalse);
			}
			
		CCalEntry* entry = CreateEntryL(entryType, startTime, endTime, timemode);
		CleanupStack::PushL(entry);
		
		pos = data.Locate(';');
		if (pos != KErrNotFound)
			{
			item = data.Left(pos);
			if (0==item.Compare(KRp))  //Add repeating rule
				{							//For simpicity it will be daily rule for all days in a week
				data.Delete(0, pos+1);
				TCalRRule rule(TCalRRule::EDaily);
				TCalTime startCalTime;
				
				if(timemode == TCalTime::EFixedUtc)
					{
					startCalTime.SetTimeUtcL(startTime);
					}
				else
					{
					startCalTime.SetTimeLocalFloatingL(startTime);
					}
				
				rule.SetDtStart(startCalTime);
				rule.SetCount(10);
				entry->SetRRuleL(rule);
				}
			}

		aEntries.AppendL(entry);
		CleanupStack::Pop(entry);
		
		}
	}

void CCrTestStepBase::CloseAndDeleteRPtrArrayEntries(TAny* aObject)	
	{
	RPointerArray<CCalEntry>* array=reinterpret_cast<RPointerArray<CCalEntry>*>(aObject);

	TInt count = array->Count();
	for (TInt i=0; i < count; i++)
		{
		delete (*array)[i];
		}
	array->Close();
	}


void CCrTestStepBase::CloseAndDeleteRPtrArrayInstances(TAny* aObject)	
	{
	RPointerArray<CCalInstance>* array=reinterpret_cast<RPointerArray<CCalInstance>*>(aObject);

	TInt count = array->Count();
	for (TInt i=0; i < count; i++)
		{
		delete (*array)[i];
		}
	array->Close();
	}


void CCrTestStepBase::OpenSessionFileL()
	{
	INFO_PRINTF1(KAgnInfo);
	TRAPD(err, iSession->CreateCalFileL(KAgendaFile));
	if (KErrAlreadyExists==err)
		{
		iSession->DeleteCalFileL(KAgendaFile);
		iSession->CreateCalFileL(KAgendaFile);	
		err = KErrNone;
		}
	
	TESTL(err == KErrNone);
		
	iSession->OpenL(KAgendaFile());	
	}

void CCrTestStepBase::SetTZL(TPtrC aTZ)
	{
	TBuf8<64> aTZ8;
	aTZ8.Copy(aTZ);
	
	CTzId* id = CTzId::NewL(aTZ8);
	CleanupStack::PushL(id);
	
	RTz tz;
	CleanupClosePushL(tz);
	TESTL(tz.Connect() == KErrNone);
	
	if(iPrevTZ == NULL) 
		{
		iPrevTZ = tz.GetTimeZoneIdL();
		}
	
	tz.SetTimeZoneL(*id);
	CleanupStack::PopAndDestroy(2);
	}

void CCrTestStepBase::RestoreTZL()
	{
	RTz tz;
	CleanupClosePushL(tz);
	TESTL(tz.Connect() == KErrNone);
	
	if(iPrevTZ != NULL) 
		{
		tz.SetTimeZoneL(*iPrevTZ);
		}

	CleanupStack::PopAndDestroy();
	}
	

