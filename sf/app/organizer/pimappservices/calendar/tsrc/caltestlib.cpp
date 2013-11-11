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

#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calsession.h>
#include <calcalendarinfo.h>
#include <calprogresscallback.h>
#include <e32test.h>
#include <consolealarmalertservermain.h>
#include <e32math.h>
#include <caliterator.h>
#include <s32file.h>
#include <ecom/ecom.h>
#include <caldataformat.h>
#include <calinstance.h>
#include <s32mem.h>
#include <e32std.h>

#ifdef __WINS__
const TUid KServerUid2 = {0x1000008D};
const TUid KServerUid3 = {0x101F502A};
_LIT(KConsoleAlarmAlertServerImg, "ConsoleAlarmAlertServer");	
#endif // __WINS__

_LIT(KCalTestLibRefCountChunkName, "CalTestLibRefCount");

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Constructor/Destructor
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

EXPORT_C CCalTestLibrary* CCalTestLibrary::NewLC(TBool aDelFileFlag)
	{
	CCalTestLibrary* self = new (ELeave) CCalTestLibrary(aDelFileFlag);
	CleanupStack::PushL(self);
	self->BaseConstructL();
	return (self);
	}


EXPORT_C CCalTestLibrary* CCalTestLibrary::NewL(TBool aDelFileFlag)
	{
	CCalTestLibrary* self = NewLC(aDelFileFlag);
	CleanupStack::Pop(self);
	return (self);
	}


// protected

EXPORT_C CCalTestLibrary::CCalTestLibrary(TBool aDelFileFlag) : iDelFileFlag(aDelFileFlag)
	{
	TTime now;
	now.UniversalTime();
	iSeed = now.Int64();
	}


// protected

EXPORT_C void CCalTestLibrary::BaseConstructL()
	{
	#if __WINS__
		StartAlarmServerL();
	#endif
	User::LeaveIfError(iFs.Connect());
	
	if (iChunkMutex.OpenGlobal(KCalTestLibMutex) != KErrNone)
		{
		User::LeaveIfError(iChunkMutex.CreateGlobal(KCalTestLibMutex)) ;
		}
	TInt createChunkError = iChunk.CreateGlobal(KCalTestLibRefCountChunkName, sizeof(TInt), sizeof(TInt));
	iChunk.OpenGlobal(KCalTestLibRefCountChunkName, EFalse);
	iRefCount = reinterpret_cast<TInt*>(iChunk.Base());
	TBool refExists = ETrue;
	if (createChunkError != KErrAlreadyExists)
		{
		refExists = EFalse;	
		}
	OpenCalendarSessionL(refExists);
	iFilesToDel = new(ELeave) CDesCArrayFlat(5);
	User::LeaveIfError(iPIMTestServer.Connect());
	SetTimeZoneL(KDefaultTimeZone);
	
	iTestRegister = CTestRegister::NewL();
	User::LeaveIfError(iAlarmServer.Connect());
	}

void CCalTestLibrary::OpenCalendarSessionL(TBool aRefCountExists)
	{
	iChunkMutex.Wait();
	iSession = CCalSession::NewL();
	if (aRefCountExists)
		{
		++*iRefCount;
		}
	else
		{
		*iRefCount = 1;
		}
	iChunkMutex.Signal();
	}

void CCalTestLibrary::CloseCalendarSession(TBool aWaitForAgendaToClose)
	{
	iChunkMutex.Wait();
	iSubCalSessions.ResetAndDestroy();
	delete iSession;
	iSession = NULL;	
	if (iRefCount && (--*iRefCount <= 0) && aWaitForAgendaToClose)
		{
		WaitForAgendaServerClose();
		}
	iChunkMutex.Signal();
	}
	
EXPORT_C void CCalTestLibrary::WaitForAgendaServerClose()
	{
	// This is the last time we are closing the cal test library	
	// So wait for the agenda server process to finish
	
	// Make sure any ecom sessions are destroyed before waiting for the agenda server to close
	REComSession::FinalClose();
	
	// the name of the agenda server process includes its uid like this [10003a5b]
	TFindProcess findProcess(_L("*[10003a5b]*"));
	TFullName fullName;
	TInt findProcessResult = findProcess.Next(fullName);
	
	if (findProcessResult == KErrNone)
		{
		// find the latest agenda server process
		while (findProcessResult == KErrNone)
			{
			findProcessResult = findProcess.Next(fullName);
			}
	
		// the agenda server process is running so wait 
		RProcess process;
		if (process.Open(fullName) == KErrNone)
			{
			RTimer timeoutTimer;
			timeoutTimer.CreateLocal();
			TRequestStatus timeoutTimerStatus;
			timeoutTimer.After(timeoutTimerStatus, 10000000); // 10 seconds
			TRequestStatus processStatus;
			process.Logon(processStatus); // ask for a callback when the process ends
					
			User::WaitForRequest(processStatus, timeoutTimerStatus);
					
			if (timeoutTimerStatus.Int() == KRequestPending)
				{
				// the process ended so cancel the timeout timer
				timeoutTimer.Cancel();
				User::WaitForRequest(timeoutTimerStatus);
				}
			else if (processStatus.Int() == KRequestPending)
				{
				process.LogonCancel(processStatus);
				User::WaitForRequest(processStatus);
				}
			
			process.Close();
			}
		}
	}
	
EXPORT_C void CCalTestLibrary::CloseAgendaServer()
	{
	delete iDataExchange;
	iDataExchange = NULL;

	delete iIter;
	iIter = NULL;;

	delete iInstanceView;
	iInstanceView = NULL;;

	delete iEntryView;
	iEntryView = NULL;;

	CloseCalendarSession();
	}
	
EXPORT_C void CCalTestLibrary::ConnectAgendaServerL()
	{
	if (!iSession)
		{
		OpenCalendarSessionL();		
		}
	}
	
EXPORT_C CCalTestLibrary::~CCalTestLibrary()
	{
	// Delete created files
	if(iDelFileFlag && iFilesToDel)
		{
		for(TInt i = 0; i < iFilesToDel->Count(); ++i)
			{
			TRAP_IGNORE(iSession->DeleteCalFileL((*iFilesToDel)[i]));
			}
		// Empty the file list:
		iFilesToDel->Reset();
		}
	delete iFilesToDel;
	delete iDataExchange;
	iPIMTestServer.Close();
	
	delete iIter;
	delete iInstanceView;
	delete iEntryView;
	delete iTestRegister;
	iFs.Close();

	if (iChunkMutex.IsHeld())
		{
		iChunkMutex.Signal();
		}
		
	CloseCalendarSession();

	iChunkMutex.Close();
	
	iChunk.Close();
	iAlarmServer.Close();
	}
	
EXPORT_C CTestRegister& CCalTestLibrary::TestRegister()
	{
	return *iTestRegister;
	}

#if __WINS__
EXPORT_C void CCalTestLibrary::StartAlarmServerL()
	{
	TInt findProcessResult(KErrNone);
	
	// the name of the console alarm alert server process includes its uid like this [101F502A]
	_LIT(KAlarmServerUIDMatch, "*[101F502A]*");
	TFindProcess findProcess(KAlarmServerUIDMatch);
	TFullName fullName;
	findProcessResult = findProcess.Next(fullName);
	if (findProcessResult != KErrNone)
		{
		// The console alarm alert server process is not running. Start it
		const TUidType serverUid(KNullUid, KServerUid2, KServerUid3);
		
		RProcess alarmAlertServer;

		User::LeaveIfError(alarmAlertServer.Create(KConsoleAlarmAlertServerImg, KNullDesC, serverUid));
		
		TRequestStatus stat;
		alarmAlertServer.Rendezvous(stat);
		if (stat != KRequestPending)
			{
			alarmAlertServer.Kill(0);
			}
		else
			{
			alarmAlertServer.Resume();	// logon OK - start the server
			}
		alarmAlertServer.Close();
		User::WaitForRequest(stat);
		TInt result = stat.Int();
		if(result != KErrNone && result != KErrAlreadyExists)
			{
			User::LeaveIfError(result);	
			}
		}
	}
#endif // __WINS__



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Implementation of MCalProgressCallBack interface
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CCalTestLibrary::Progress(TInt /*aPercentageCompleted*/)
	{
	}


void CCalTestLibrary::Completed(TInt aError)
	{
	CActiveScheduler::Stop();
	iCompletedError = aError;
	}


TBool CCalTestLibrary::NotifyProgress()
	{
	return (EFalse);
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * File handling
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

EXPORT_C RFs& CCalTestLibrary::FileSession()
	{
	return (iFs);
	}

EXPORT_C void CCalTestLibrary::RegisterCalFileL(const TDesC& aFileName)
	{
	TInt pos=-1;
	
	if(iDelFileFlag)
		{
		if(iFilesToDel->Find(aFileName,pos)!=0) 
			{
			iFilesToDel->AppendL(aFileName);
			}
		}
	}

EXPORT_C void CCalTestLibrary::CreateFileL(const TDesC& aFileName)
	{
	iSession->CreateCalFileL(aFileName);
	TInt pos=-1;
	if(iDelFileFlag)
		{
		if(iFilesToDel->Find(aFileName, pos)!=0)	
			{
			iFilesToDel->AppendL(aFileName);
			}
		}
	}


EXPORT_C void CCalTestLibrary::ReplaceFileL(const TDesC& aFileName)
	{
	TRAPD(err, CreateFileL(aFileName));
	
	if (err == KErrAlreadyExists)
		{
		DeleteFileL(aFileName);
		
		CreateFileL(aFileName);
		}
	else
		{
		User::LeaveIfError(err);
		}
	}

EXPORT_C void CCalTestLibrary::OpenFileL(const TDesC& aFileName, TBool aDelFileFlag) 
	{
	delete iDataExchange;
	iDataExchange = NULL;

	delete iIter;
	iIter = NULL;	
	
	delete iEntryView;
	iEntryView = NULL;
	
	delete iInstanceView;
	iInstanceView = NULL;
	
	CloseCalendarSession(EFalse);
	OpenCalendarSessionL();

	iSession->OpenL(aFileName);
	TInt pos=-1;
	if( iDelFileFlag && aDelFileFlag )
		{
		if( iFilesToDel->Find(aFileName, pos)!=0 )
			{
			iFilesToDel->AppendL(aFileName);
			}
		}
	}


EXPORT_C void CCalTestLibrary::DeleteFileL(const TDesC& aFileName, TBool aIsCalFile)
	{
	if (!iSession) // some tests call CloseAgendaServer
		{
		OpenCalendarSessionL();	
		}
	
	if (aIsCalFile)
		{
		
		for(TInt i = 0; i < iSubCalSessions.Count();i++)
		    {
		    CCalCalendarInfo* info = iSubCalSessions[i]->CalendarInfoL();
		    HBufC* fileName = info->FileNameL().AllocLC();
		    delete info;
		    iSubCalSessions[i]->DeleteCalFileL(*fileName);
		    CleanupStack::PopAndDestroy(fileName);
		    }
		
		iSession->DeleteCalFileL(aFileName);
		TInt indx=-1;
		if(iDelFileFlag)
			{
			if(iFilesToDel->Find(aFileName, indx)==0)
				{
				iFilesToDel->Delete(indx,1);
				}
			}
		}
		else
			{
			iPIMTestServer.DeleteFileL(aFileName);
			}
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Session and Views handling
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

EXPORT_C CCalSession& CCalTestLibrary::GetSession()
	{
	// the pointyer is never NULL because the object is created in NewL...
	return (*iSession);
	}


EXPORT_C CCalEntryView& CCalTestLibrary::AsynCGetEntryViewL()
	{
	//The caller should expect the completion in their RunL
	if (!iEntryView)
		{
		iEntryView = CCalEntryView::NewL(*iSession, *this);
		}

	return (*iEntryView);
	}


EXPORT_C CCalInstanceView& CCalTestLibrary::AsynCGetInstanceViewL()
	{
	if (!iInstanceView)
		{
		iInstanceView = CCalInstanceView::NewL(*iSession, *this);
		}

	return (*iInstanceView);
	}


EXPORT_C CCalInstanceView& CCalTestLibrary::SynCGetInstanceViewL()
	{
	if (!iInstanceView)
		{
		iInstanceView = CCalInstanceView::NewL(*iSession);
		}

	return (*iInstanceView);
	}


EXPORT_C CCalEntryView& CCalTestLibrary::SynCGetEntryViewL()
	{
	if (!iEntryView)
		{
		iEntryView = CCalEntryView::NewL(*iSession);
		}
		
	return (*iEntryView);
	}

EXPORT_C void CCalTestLibrary::DeleteViews()
	{
	delete iInstanceView;
	iInstanceView = NULL;
	delete iEntryView;
	iEntryView = NULL;
	delete iIter;
	iIter = NULL;
	delete iDataExchange;
	iDataExchange = NULL;
	}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Entry handling
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

EXPORT_C CCalEntry* CCalTestLibrary::CreateCalEntryL(CCalEntry::TType aType, HBufC8*& aUid)
	{
	HBufC8* uid = NULL;
	if (aUid)
		{
		uid = aUid;
		}
	else
		{
		uid = HBufC8::NewLC(255);
		TPtr8 uidP = uid->Des();
		RandomText8(uidP);
		}

	CCalEntry* entry = CCalEntry::NewL(aType, uid, CCalEntry::EMethodAdd, 0);
	if (!aUid)
		{
		CleanupStack::Pop(uid);
		}
	aUid = uid;
	
	return (entry);
	}

EXPORT_C void CCalTestLibrary::SetEntryStartAndEndTimeL(CCalEntry* aEntry, const TTime& aStartTime, const TTime& aEndTime, TInt aYear)
	{
	TTime today;
	today.HomeTime();
	TInt64 seed  = today.Int64();

	TInt date    = Math::Rand(seed);
	TInt year    = aYear;
	TInt month   = date % 12;
	TInt day     = date % 28;
	TInt hours   = date % 24;
	TInt minutes = date % 60;

	TTime startRanTime(TDateTime(year, (TMonth)month, day, hours, minutes, 0, 0));
	TTime startTime;
	TTime endTime;
	switch (aEntry->EntryTypeL())
		{
		case CCalEntry::EAppt:
		case CCalEntry::ETodo:
			if (aStartTime == Time::NullTTime() && aEndTime == Time::NullTTime())
				{
				startTime = startRanTime.Int64();
				endTime   = startRanTime + TTimeIntervalHours(2);
				}
			else
				{
				startTime = aStartTime.Int64();
				endTime   = aEndTime.Int64();
				}
			break;
			
		case CCalEntry::EReminder:
		case CCalEntry::EEvent:
		case CCalEntry::EAnniv:
			if (aStartTime == Time::NullTTime())
				{
				startTime = startRanTime.Int64();
				}
			else
				{
				startTime = aStartTime.Int64();
				}
			endTime = startTime;
			break;
			
		default:
			User::Invariant();
			break;
		}
				
	TCalTime startCalTime;
	startCalTime.SetTimeUtcL(startTime);
	TCalTime endCalTime;
	endCalTime.SetTimeUtcL(endTime);
	aEntry->SetStartAndEndTimeL(startCalTime, endCalTime);	
	}


EXPORT_C void CCalTestLibrary::RandomText(TDes& aText, TBool aPossibleNulDes)
	{
	aText.Zero();
	
	if (aText.MaxLength() <= 8)
		{
		User::Panic(_L("RandomText - descriptor too short"), 0);
		}
		
	if (aPossibleNulDes)
		{
		if (Math::Rand(iSeed) % 2)
			{
			aText=KNullDesC;
			}
		}
	else
		{
		TInt numChar = 0;
		do	{
			numChar = Math::Rand(iSeed) % (aText.MaxLength() - 1);
			}
		while (numChar < 8);
		
		for (TInt i = 0;i < numChar; ++i)
			{
			TInt c = Math::Rand(iSeed) % 6;
			c = (c == 0) ? ' ' : ( (Math::Rand(iSeed) % 26) + ((c == 2) ? 'A' : 'a') );
			aText.Append(c);
			}
		}
	}


EXPORT_C void CCalTestLibrary::RandomText8(TDes8& aText, TBool aPossibleNulDes)
	{
	aText.Zero();
		
	if (aPossibleNulDes)
		{
		if (Math::Rand(iSeed) % 2)
			{
			aText = KNullDesC8;
			}
		}
	else
		{
		// The number of characters in the guid should be
		// more than 8 (to ensure the guids are not too small)
		// less than 255 (this is the maximum length of a guid)
		// less than MaxLength of the descriptor passed in		
		TInt numChar = 0;
		do	{
			numChar = Math::Rand(iSeed) % (aText.MaxLength() - 1);
			}
		while( numChar > 255 || numChar < 8 );
		
		for (TInt i = 0; i < numChar; ++i)
			{
			TInt c = Math::Rand(iSeed) % 6;
			c = Math::Rand(iSeed) % 26 + ((c == 2)? 'A' : 'a');
			aText.Append(c);
			}
		}
	}


EXPORT_C void CCalTestLibrary::CleanDatabaseL()
	{
	CDesC8ArrayFlat* guidArray = new (ELeave) CDesC8ArrayFlat(1);
	CleanupStack::PushL(guidArray);

	CCalIter& iter = GetIterL();
	
	for (TPtrC8 Uid(iter.FirstL()); Uid != KNullDesC8(); Uid.Set(iter.NextL()))
		{
		guidArray->AppendL(Uid);
		}

	if (guidArray->Count() > 0)
		{
  		SynCGetEntryViewL().DeleteL(*guidArray);
		}
  	
	CleanupStack::PopAndDestroy(guidArray);
	}


EXPORT_C CCalIter& CCalTestLibrary::GetIterL()
	{
	if (!iIter)
		{
		iIter = CCalIter::NewL(*iSession);
		}
		
	return (*iIter);		
	}


EXPORT_C void CCalTestLibrary::StoreEntryL(const CCalEntry& aEntry)
	{
	RPointerArray<CCalEntry> entriesToAdd;
	CleanupClosePushL(entriesToAdd);
	entriesToAdd.AppendL(&aEntry);
	TInt success = 0;
	SynCGetEntryViewL().StoreL(entriesToAdd, success);
	CleanupStack::PopAndDestroy(&entriesToAdd); // entriesToAdd.Close()
	}

EXPORT_C void CCalTestLibrary::FetchAllInstancesL(RPointerArray<CCalInstance>& aInstances)
	{
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);
	
	SynCGetInstanceViewL().FindInstanceL(aInstances, filter, timeRange);
	}

EXPORT_C void CCalTestLibrary::OutputInstanceDataL(const RPointerArray<CCalInstance>& aInstances)
	{
	for (TInt i = 0; i < aInstances.Count(); i++)
		{
		TDateTime dt = aInstances[i]->Time().TimeLocalL().DateTime();
		TCalLocalUid luid = aInstances[i]->Entry().LocalUidL();
		RDebug::Print(_L("Instance %d has local uid %d and occurs at %2d:%2d:%2d on %2d/%2d/%4d"), 
			i, luid, dt.Hour(), dt.Minute(), dt.Second(), dt.Day()+1, dt.Month()+1, dt.Year());
		}
	}

EXPORT_C RPIMTestServer& CCalTestLibrary::PIMTestServer()
    {
    return iPIMTestServer;
    }

EXPORT_C void CCalTestLibrary::CreateTestFileL(const TDesC8& aVCalData, const TDesC& aFileName)
	{
	RFile file;
	
	iTestRegister->CreateTempFileLC(file, aFileName); 												     
	User::LeaveIfError(file.Write(aVCalData));
	CleanupStack::PopAndDestroy(&file); //file.Close()
	}
	
EXPORT_C CCalDataExchange& CCalTestLibrary::DataExchangeL()
	{
	if (iDataExchange == NULL)
		{
		iDataExchange = CCalDataExchange::NewL(*iSession);		
		}
		
	return *iDataExchange;
	}

EXPORT_C void CCalTestLibrary::ImportL(const TDesC& aImportVCalendarFile, RPointerArray<CCalEntry>& aEntriesImported)
	{	
	RFile infile;
    iTestRegister->OpenTempFileLC(infile, aImportVCalendarFile);
	RFileReadStream readStream(infile);
	CleanupClosePushL(readStream);
	
	DataExchangeL().ImportL(KUidVCalendar, readStream, aEntriesImported);

	CleanupStack::PopAndDestroy(&readStream); // readStream.Close()
	CleanupStack::PopAndDestroy(&infile); // infile.Close()
	}
	
EXPORT_C void CCalTestLibrary::ImportL(const TDesC8& aVCalendar, RPointerArray<CCalEntry>& aEntriesImported)
	{
	RDesReadStream readStream(aVCalendar);
	CleanupClosePushL(readStream);
	
	DataExchangeL().ImportL(KUidVCalendar, readStream, aEntriesImported);

	CleanupStack::PopAndDestroy(&readStream);
	}
	
EXPORT_C void CCalTestLibrary::ExportL(const TDesC& aExportVCalendarFile, RPointerArray<CCalEntry>& aEntriesExported)
	{
	RFile outFile;
    iTestRegister->CreateTempFileLC(outFile, aExportVCalendarFile);
	RFileWriteStream writeStream(outFile);
	CleanupClosePushL(writeStream);
	
	DataExchangeL().ExportL(KUidVCalendar, writeStream, aEntriesExported);

	writeStream.CommitL();

	CleanupStack::PopAndDestroy(&writeStream); // writeStream.Close()
	CleanupStack::PopAndDestroy(&outFile); // outFile.Close()
	}

EXPORT_C void CCalTestLibrary::SetTimeZoneL(const TDesC8& aTimeZoneName)
	{
	CSetTimezone* Timezone = CSetTimezone::NewL(aTimeZoneName, &iPIMTestServer);
	CleanupStack::PushL(Timezone);
	Timezone->Start();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(Timezone);
	}
	


EXPORT_C void CCalTestLibrary::__dbgClearTzClientCacheL(TBool aRestartCaching)
	{
	iSession->__dbgClearTzClientCacheL(aRestartCaching);	
	}

/*
  WaitForAlarmEvent
  Waits for specific alarm's event.
  @param aAlarmId - id of alarm we are waiting for notification. If KNullAlarmId then wait for notification from any alarm
  @param aExpectedEvent - event type we are waiting for
  @aExpectedState - expected state of given alarm. Used only if aExpectedEvent is EAlarmChangeEventState.
  @param aTimeout - timeout in microseconds (applied in parallel with every call to WaitForRequest)
  @return ETrue if alarm request was completed with expected event (and, in case of EAlarmChangeEventState, alarm is in correct state)
          EFalse if timeout was reached.
 */
EXPORT_C TBool CCalTestLibrary::WaitForAlarmEvent(TAlarmId aAlarmId, TAlarmChangeEvent aExpectedEvent, TAlarmState aExpectedState, TInt aTimeout)
	{
	TRequestStatus status;
	TRequestStatus statusTimeout;
	RTimer timer;
	CleanupClosePushL(timer);
	User::LeaveIfError(timer.CreateLocal());
	TAlarmId returnedAlarmId;
	FOREVER
		{
		iAlarmServer.NotifyChange(status,returnedAlarmId);
		timer.After(statusTimeout,TTimeIntervalMicroSeconds32(aTimeout));
		User::WaitForRequest(status,statusTimeout); // waits for change of status
		if(statusTimeout != KRequestPending)
			{
			if(statusTimeout == KErrNone)
				{
				RDebug::Print(_L("Timeout reached\n"));
				}
			else
				{
				RDebug::Print(_L("\tTimeout error %d \n"),statusTimeout.Int());
				}
			iAlarmServer.NotifyChangeCancel();
			User::WaitForRequest(status);
			CleanupStack::PopAndDestroy(&timer);
			return EFalse;
			}
		else
			{
			timer.Cancel();
			User::WaitForRequest(statusTimeout);
			if(statusTimeout == KErrCancel)
				{
				if((aAlarmId == KNullAlarmId || returnedAlarmId == aAlarmId) && status.Int() == aExpectedEvent) 
					{
					if(aExpectedEvent == EAlarmChangeEventState)
						{
						TASShdAlarm alarm;
						TInt err = iAlarmServer.GetAlarmDetails(returnedAlarmId,alarm);
						if(err == KErrNone || err == KErrNotFound)
							{
							if(err == KErrNotFound || alarm.State() != aExpectedState)
								{
								continue;
								}
							}
						else
							{
							User::Leave(err);
							}
						}
					CleanupStack::PopAndDestroy(&timer);
					return ETrue;
					}
				}
			}
		}
    }

// A utility class to set the timezone.
CSetTimezone::CSetTimezone() : CActive(EPriorityLow)
	{
	// For some as yet undetermined reason setting the priority of this object 
	// to a value other than EPriorityLow results in this class failing to set
	// the timezone correctly. Do not change this value.
	CActiveScheduler::Add(this);
	}
	
CSetTimezone::~CSetTimezone()
	{
	Cancel();
	if(iZoneId)	
		{
			delete iZoneId;
		}
	}

CSetTimezone* CSetTimezone::NewL(const TDesC8& aZoneId, RPIMTestServer* aPIMTestServer)
	{
	CSetTimezone* self = new (ELeave) CSetTimezone;
	CleanupStack::PushL(self);
	self->ConstructL(aZoneId, aPIMTestServer);
	CleanupStack::Pop(self);
	return self;
	}

void CSetTimezone::ConstructL(const TDesC8& aZoneId, RPIMTestServer* aPIMTestServer)
	{
	iPIMTestServer = aPIMTestServer;
	iZoneId = aZoneId.AllocL();
	iStep = 0;
	}

void CSetTimezone::Start()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, 0);
	SetActive();
	}
	
void CSetTimezone::RunL()
	{
	TBool complete = EFalse;
	switch (iStep)
		{
		case 0:
			iPIMTestServer->SetTimeZoneL(iZoneId->Des());
			break;
			
		default:
			complete = ETrue;
			break;
		}
		
	if (complete)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		iStep++;	
		Start();
		}
	}
	
void CSetTimezone::DoCancel()
	{
	}
	
EXPORT_C TPerformanceTimer::TPerformanceTimer(RTest& aTest)
	:iTest(aTest)
	{
	}

EXPORT_C void TPerformanceTimer::Start()
	{
	iStartTickCount = User::NTickCount();	
	}

EXPORT_C void TPerformanceTimer::Stop()
	{
	iEndTickCount = User::NTickCount();
	}
	
EXPORT_C void TPerformanceTimer::PrintOut() const
	{
	iTest.Printf(_L("Elapsed time: %d microseconds\n"), ElapsedTime().Int());		
	}

EXPORT_C TTimeIntervalMicroSeconds32 TPerformanceTimer::ElapsedTime() const
	{
	TInt tickPeriod;
	HAL::Get(HAL::ENanoTickPeriod, tickPeriod);
	TUint durationInTicks = iEndTickCount - iStartTickCount;
	return tickPeriod * durationInTicks;	
	}

EXPORT_C RPointerArray<CCalSession>& CCalTestLibrary::CreateCalSubSessionL(const TDesC& aFileName)
    {
    CCalSession* session = CCalSession::NewL(*iSession);
    CCalCalendarInfo* calendarInfo = CCalCalendarInfo::NewL();
    calendarInfo->SetColor(KRgbRed);
    calendarInfo->SetNameL(_L("test"));
    calendarInfo->SetEnabled(ETrue);
    
    TRAPD(err,session->CreateCalFileL(aFileName,*calendarInfo));
    if(err == KErrAlreadyExists)
        {
        session->DeleteCalFileL(aFileName);
        session->CreateCalFileL(aFileName,*calendarInfo);
        }
    session->OpenL(aFileName);
    iSubCalSessions.Append(session);
    
    delete calendarInfo;
    return iSubCalSessions;
    }

