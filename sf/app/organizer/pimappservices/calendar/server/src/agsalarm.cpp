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

#include "agsalarm.h"
#include "agmutil.h"
#include "agssortinstance.h"
#include "agmentry.h"
#include "agsentrymodel.h"
#include "agmtlsproxy.h"
#include "agmdebug.h"
#include "agsfilemanager.h"
#include "agsmain.h"
#include <centralrepository.h>
#include <f32file.h>
#include <txtetext.h>
#include <tz.h>

// This is used when the system time is put forward. All alarms within KSystemTimeChangeMaxDaysOld of 
// the new/present time will be queued. If set to 1, and the time is put forward by 7 days then only
// alarms within 1 day of the new time will be queued.
// (Note: This may need to be changed to a TTime in the future if greater granularity is required.)
const TInt KSystemTimeChangeMaxDaysOld = 1;

// Maximum number of event alarms to be scheduled when the system time/date changes. Ideally there
// shouldn't be a maximum but the existing API FindAndQueueNextFewAlarmsL takes has a
// maximum parameter. This should be set to MAX_INT.
// (Note: This is the same default value used in the method CAgnAlarm::FindAndQueueNextFewAlarmsL)
const TInt KSystemTimeChangeMaxAlarmsQueued = 0xffff;


// Central Repository Uid
const TUid KUidCentralRepositoryAgenda = { 0x10003a5b };
 
//Used for configuration of whether or not to notify missing alarms
const TUint KKeyQueueMissingAlarm = 100;
CAgnAlarm::CAgnAlarm(RASCliSession& aRASCliSession)
    :iAlarmServer(aRASCliSession)
    {
    }
CAgnAlarm* CAgnAlarm::NewL(CAgnEntryModel* aModel,MAgnAlarmServerTerminationCallBack* aCallBack)
/** Allocates and constructs a CAgnAlarm object and makes a connection to 
the alarm server.

@internalAll
@param aModel Pointer to the agenda model which the alarm server connection 
is being set up for.
@param aCallBack An optional alarm server termination callback provider. May be NULL.
@return Pointer to the newly created alarm server interface object. */
	{
	CAgnAlarm* self = new (ELeave) CAgnAlarm(aModel->AgnServFile().Server().AlarmServer());
	
	CleanupStack::PushL(self);
	self->ConstructL(aModel,aCallBack);
	CleanupStack::Pop();
	
	return (self);
	}

void CAgnAlarm::DeleteAllAlarms()
    {
    // ignore any errors returned from this
    TParsePtrC parser(iAlarm->AgnServFile().FileName());
    TFileName fileName = parser.Drive();
    fileName.Append(parser.NameAndExt());
    iAlarmServer.AlarmDeleteByCalendarFile(fileName, EFuture|EExpired);
    }

void CAgnAlarm::ConstructL(CAgnEntryModel* aModel,MAgnAlarmServerTerminationCallBack* aCallBack)
//
// Construct CAgnAlarm
//
	{
	iAlarm = CAgnAlarmActive::NewL(aModel,iAlarmServer);
	iChangeObserver = CAgnAlarmServerChangeObserver::NewL(aCallBack,this);

	iQueueAlarmsInPast = 1;
	CRepository* repository = NULL;
	TRAPD(err,  repository = CRepository::NewL(KUidCentralRepositoryAgenda));
	if (err == KErrNone && repository)
		{
		CleanupStack::PushL(repository);
		User::LeaveIfError(repository->Get(KKeyQueueMissingAlarm, iQueueAlarmsInPast));
		CleanupStack::PopAndDestroy(repository);
		}

	iAlarm->SetQueueAlarmsInPast(iQueueAlarmsInPast);
	}

TBool CAgnAlarm::QueueAlarmsInPast()
	{
	return iQueueAlarmsInPast;
	}
	
 CAgnAlarm::~CAgnAlarm()
/** Frees all resources owned by the object, prior to its destruction, 
and ends the session with the alarm server. 
@internalAll
*/
	{
	delete iAlarm;
	delete iChangeObserver;
	}

void CAgnAlarm::OrphanAlarm()
/** Orphans the session alarm. 

This allows the alarm to be serviced after the session is closed. This 
function should be called by the user of the model before closing an 
agenda file. 

@internalAll
*/
	{
	iAlarm->OrphanAlarm();
	}

/** Finds the agenda model's next due alarm. 

If a due alarm is found, it is set as the session alarm and queued 
with the alarm server. 

This function should be called by the client immediately after the CAgnAlarm object is created. 

@internalAll
@capability WriteUserData
*/
void CAgnAlarm::FindAndQueueNextAlarmL(TBool aSysTimeHasChanged)
	{
	iAlarm->FindAndQueueNextAlarmL(aSysTimeHasChanged);
	}

void CAgnAlarm::QueueNextAlarmL(CArrayFixFlat<TAgnSortInstance>*aAlarmedIds)
	{
	iAlarm->QueueNextAlarmL(aAlarmedIds);
	}

void CAgnAlarm::FindAndQueueNextFewAlarmsL(const TInt aMaxNumberOfAlarms,const TInt aMaxNumberOfDays)
/** Finds the agenda model's next few alarms.

If one or more due alarms are found, they are set as session alarms
and queued with the alarm server.

This method is no longer required by the client because the alarms are now queued
by the agenda server once the last session is disconnected to it.

@internalAll
@param aMaxNumberOfAlarms The maximum number of alarms to be queued. The default 
value is 10.
@param aMaxNumberOfDays The maximum number of days to be scanned for alarms. 
The default value is 31. */
	{
	iAlarm->FindAndQueueNextFewAlarmsL(aMaxNumberOfAlarms, aMaxNumberOfDays);
	}


void CAgnAlarm::FindAndQueueNextFewAlarmsL(const TTime& startTime,const TTime& endTime,const TInt aMaxNumberOfAlarms)
/* Finds the agenda model's next few alarms.

If one or more due alarms are found, they are set as session alarms
and queued with the alarm server. 

(Note: This method was added to handle system date/time changes. It is not currently exported, however
 the less general version which takes aMaxNumberOfDays can be used for most purposes.  */
	{
	iAlarm->FindAndQueueNextFewAlarmsL(startTime,endTime,aMaxNumberOfAlarms);
	}

/* Gets the TAgnAlarmEntry struct 

*/
const TAgnAlarmEntry* CAgnAlarm::AlarmEntry() const
	{
	return ( iAlarm->CurrentAlarmEntry() );
	}

void CAgnAlarm::DeleteEntriesAlarmL(const TAgnEntryId& aEntryId)
   //Removes all of an entry's alarms from the alarm server.
   //First, obtain a list of alarms from the alarmserver.
   //Compare aEntryInstanceId to the TAgnEntryId of the alarm in the queue.
   //if they are the same then the alarm server is asked to remove the alarm from the queue.
   	{
   	RArray<TAlarmId> snoozedAlarmIds;
   	CleanupClosePushL(snoozedAlarmIds);

   	iAlarmServer.GetAlarmIdListForCategoryL(KUidAgendaModelAlarmCategory, snoozedAlarmIds);
  	
   	if( snoozedAlarmIds.Count() > 0 )
   		{
   		for( TInt i = 0; i < snoozedAlarmIds.Count(); ++i )
	   		{
	   		//get the info for this alarm id
	  		TBuf8<sizeof(TAgnAlarmInfo)> alarmInfoBuf;
	  		
	   		iAlarmServer.GetAlarmData(snoozedAlarmIds[i],alarmInfoBuf);
	   		
	  		const TAgnAlarmInfo* alarmInfo = reinterpret_cast<const TAgnAlarmInfo*>(alarmInfoBuf.Ptr());
	  		if ( alarmInfo->iEntryId == aEntryId )
	   			{
	   			iAlarmServer.AlarmNotificationCancelAndDequeue(snoozedAlarmIds[i]);
	   			}
	   		}
   		}
   		
   	CleanupStack::PopAndDestroy(&snoozedAlarmIds);
   	}
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
void CAgnAlarm::DeleteAlarmsAndRequeueSessionAlarmL()
	{
	iAlarmServer.AlarmDeleteByCategory(KUidAgendaModelAlarmCategory, EFuture|EExpired);
	FindAndQueueNextAlarmL(EFalse);
	}
#endif

const CAgnServFile& CAgnAlarm::AgnServFile()
    {
    return iAlarm->AgnServFile();
    }

CAgnAlarmServerChangeObserver& CAgnAlarm::ChangeObserver()
    {
    return *iChangeObserver;
    }

CAgnAlarmActive* CAgnAlarmActive::NewL(CAgnEntryModel* aModel,RASCliSession& aAlarmServer)
//
// Create a CAgnAlarmActive object and connect to the alarm server.
//
	{

	CAgnAlarmActive* self=new(ELeave)CAgnAlarmActive(EPriority,aAlarmServer);
	
	CleanupStack::PushL(self);
	self->iAlarmList=new(ELeave)CArrayFixFlat<TAgnAlarmEntry>(EGranularity);
	CleanupStack::Pop();
	
	self->iModel = aModel;
	CActiveScheduler::Add(self);
	
	return ( self );
	}

CAgnAlarmActive::CAgnAlarmActive(TInt aPriority,RASCliSession& aAlarmServer) 
//
// Constructor
//
	: CActive(aPriority), iAlarmServer(aAlarmServer)
	{
	}


void CAgnAlarmActive::OrphanAlarm()
//
// If there's currently an alarm queued with the alarm server then orphan it.
//
	{

	if ( IsActive() )
		{
		// Changing the characteristics of the alarm from SessionSpecific to
		// non-session specific means the alarm ownership is transferred from
		// this session to the alarm server.
		//
		// Calling this completes the pending request with KErrCancel
		TAlarmCharacteristicsFlags alarmCharacteristicsFlags;
		iAlarmServer.GetAlarmCharacteristics(iAlarm.Id(), alarmCharacteristicsFlags);
		alarmCharacteristicsFlags.Clear(EAlarmCharacteristicsSessionSpecific);
		iAlarmServer.SetAlarmCharacteristics(iAlarm.Id(), alarmCharacteristicsFlags);
		}
		
	Cancel();
	}


CAgnAlarmActive::~CAgnAlarmActive()
//
// If active then an alarm must be outstanding so create an orphan from it then cancel it.
//
	{
	OrphanAlarm();
	delete iAlarmList;
	}


void CAgnAlarmActive::RunL()
//
// The alarm server signals iRequestStatus in the following situations:
//	- KErrNone when the alarm has expired
//	- KErrCancel when the alarm has been canceled
//
	{

	if ( iStatus == KErrNone ) 	// an alarm has expired
		{
		__ASSERT_DEBUG(iAlarmList->Count()>0, Panic(EAgmNoAlarmsInList)); // the alarm which has just gone off should be in the alarm list

		if ( ++iAlarmListIndex < iAlarmList->Count() )
		//	if there is another entry with the same alarm time as the one which has just gone off then
		//  queue it with the alarm server (its time will now be in the past so it'll go off immediately)
			{
			const TAgnAlarmEntry& alarmEntry = (*iAlarmList)[iAlarmListIndex];
			
			// Create and queue an alarm with the Alarm Server
			InitialiseAlarmL(iAlarm,alarmEntry,ETrue);
			TAgnAlarmInfo alarmInfo;
			GetAlarmInfoL(alarmEntry.iInstanceId, alarmInfo);

			_DBGLOG_ALARM(AgmDebug::DebugLog("Alarm expired/cancelled. Queueing next alarm:");)
			_DBGLOG_ALARM(AgmDebug::DebugLogAlarmL(alarmEntry);)

			TPkgCAgnAlarmInfo pkgAgendaRef(alarmInfo);
			SetActive();
			iAlarmServer.AlarmAddWithNotification(iStatus, iAlarm, pkgAgendaRef);
			
			if ( iAlarm.Id() == KNullAlarmId )
				{
				User::Leave(KErrGeneral);
				}
			}
		else
			{
			FindAndQueueNextAlarmL(EFalse);
			}
		}
	else if( iStatus == KErrCancel )
		{
		DoCancel();
		}
	}


TBool CAgnAlarmActive::AssignToL(TAgnAlarmEntry& aAlarmEntry, const TAgnSortInstance& aSortInstance)
// assign data to the passed alarm entry
// Used by QueueNextAlarmL and FindAndQueueNextFewAlarmsL

	{
	aAlarmEntry.iDueDateTimeLocal = aSortInstance.iStartTimeLocal;
	
	//to get text in this entry
	CAgnEntry* entry = iModel->FetchEntryL(aSortInstance.SimpleEntry().EntryId());
	if (! entry)
		{
		return EFalse;
		}
		
	CleanupStack::PushL(entry);
	
	if ( entry->RptDef() && !entry->RptDef()->IsAnUnexceptedInstanceL(aSortInstance.InstanceDate()) )
		{
		// the entry should be not excepted
		_DBGLOG_ALARM(AgmDebug::DebugLog("Alarm: PANIC - Invalid Instance: The entry should be not excepted"));
		_DBGLOG_ALARM(AgmDebug::DebugLogEntryL(*entry, EDumpEntryAll);)
		DBG_PANIC(EAgmErrInvalidInstance);
		User::Leave(KErrNotFound);
		}

	TAgnCalendarTime instanceDate;
	if (entry->TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		aAlarmEntry.iAlarmTime.SetFloatingL(aSortInstance.InstanceAlarmDateTime());
		instanceDate.SetFloatingL(aSortInstance.InstanceDate());
		}
	else
		{
		aAlarmEntry.iAlarmTime.SetLocalL(aSortInstance.InstanceAlarmDateTime());
		instanceDate.SetLocalL(aSortInstance.InstanceDate());
		}
	aAlarmEntry.iInstanceId.SetEntryId(entry->EntryId());
	aAlarmEntry.iInstanceId.SetDate(instanceDate);
	
	if ( !entry->SummaryIsLoaded() )
		{
		entry->SetSummary(iModel->RestoreTextL(entry->SummaryStreamId()));
		}

	TInt len = entry->Summary().Length();
	
	if ( len > KMaxAlarmMessageLength )
		{
		len = KMaxAlarmMessageLength;
		}
		
	aAlarmEntry.iMessage = KNullDesC;
	
	for ( TInt ii = 0; ii < len; ++ii )
		{
		TChar ch(entry->Summary()[ii]);
		switch (ch)
			{
		case CEditableText::EParagraphDelimiter :
		case CEditableText::ELineBreak :
		case CEditableText::EPageBreak :
		case CEditableText::ETabCharacter :
		case CEditableText::ENonBreakingHyphen :
		case CEditableText::EPotentialHyphen :
		case CEditableText::ENonBreakingSpace :
		case CEditableText::EPictureCharacter :
			aAlarmEntry.iMessage.Append(CEditableText::ESpace);
			continue;
			}
		aAlarmEntry.iMessage.Append(ch);
		}

	aAlarmEntry.iSound = entry->AlarmSoundName();
	
	CleanupStack::PopAndDestroy(); //entry
	
	return ETrue;
	}

/** Find the next due alarm(s) with same time and if at least one exists then queue it with the alarm server
@capability WriteUserData
*/
void CAgnAlarmActive::FindAndQueueNextAlarmL(TBool aSysTimeHasChanged)
	{
	TTime now;
	now.HomeTime();

	// Certainly on WINS an alarm may expire a few seconds before it is due, the
	// following ensures that the same alarm isn't queued twice
	// If system time has been changed, we don't want to increase "now" to make sure
	// that the alarm whoes due time same as "now" is found.
	const TInt KAlarmCount = iAlarmList->Count();
	if ( KAlarmCount && !aSysTimeHasChanged)
		{
		TTime lastAlarm = (*iAlarmList)[KAlarmCount-1].iAlarmTime.LocalL();
		
		if ( now < lastAlarm )
			{
			now += TTimeIntervalSeconds(1);
			}
		}

	iAlarmList->Reset();
	
	CArrayFixFlat<TAgnSortInstance>* alarmedIds = new (ELeave) CArrayFixFlat<TAgnSortInstance>(2);
	CleanupStack::PushL(alarmedIds);

	iModel->NextAlarmForServerL(now, *alarmedIds);
	QueueNextAlarmL(alarmedIds);
	
	CleanupStack::PopAndDestroy(alarmedIds);
	}

void CAgnAlarmActive::QueueNextAlarmL(CArrayFixFlat<TAgnSortInstance>* aAlarmedIds)
// This function is only called by the server side since we want the alarm is queued in only server side
	{
	TTime now;
	now.HomeTime();

	// Certainly on WINS an alarm may expire a few seconds before it is due, the
	// following ensures that the same alarm isn't queued twice
	const TInt KAlarmCount = iAlarmList->Count();
	if ( KAlarmCount )
		{
		TTime lastAlarm = (*iAlarmList)[KAlarmCount-1].iAlarmTime.LocalL();
		
		if ( now < lastAlarm )
			{
			now += TTimeIntervalSeconds(1);
			}			
		}

	iAlarmList->Reset();
	
	for ( TInt ii = aAlarmedIds->Count() - 1; ii >= 0; --ii )
		{
		//create an alarmEntry
		TAgnAlarmEntry alarmEntry;
		
		if (AssignToL(alarmEntry, (*aAlarmedIds)[ii]))
			{
			TKeyArrayFix key(_FOFF(TAgnAlarmEntry, iInstanceId), ECmpNormal8, sizeof(TAgnInstanceId));
			iAlarmList->InsertIsqAllowDuplicatesL(alarmEntry, key);		
			}
		}

	Cancel();
	
	if ( iAlarmList->Count() )
	// Queue the first alarm in the alarm list
		{
		__ASSERT_DEBUG(IsAdded(), Panic(EAgmAgnAlarmActiveNotAdded));
		iAlarmListIndex = 0;
		const TAgnAlarmEntry& alarmEntry = (*iAlarmList)[0];

		InitialiseAlarmL(iAlarm, alarmEntry, ETrue);

		TAgnAlarmInfo alarmInfo;
		GetAlarmInfoL(alarmEntry.iInstanceId, alarmInfo);

		_DBGLOG_ALARM(AgmDebug::DebugLog("Queueing first alarm on list:");)
		_DBGLOG_ALARM(AgmDebug::DebugLogAlarmL(alarmEntry);)

		TPkgCAgnAlarmInfo pkgAgendaRef(alarmInfo);
		SetActive();
		iAlarmServer.AlarmAddWithNotification(iStatus, iAlarm, pkgAgendaRef);
		
		if ( iAlarm.Id() == KNullAlarmId )
			{
			User::Leave(KErrGeneral);
			}
		}
	}

void CAgnAlarmActive::InitialiseAlarmL(TASShdAlarm& aAlarm, const TAgnAlarmEntry& aEntryAlarm, TBool aIsSessionAlarm) 
	{
	aAlarm.Reset();
	aAlarm.Message() = aEntryAlarm.iMessage;
	aAlarm.Category() = KUidAgendaModelAlarmCategory;
	aAlarm.SoundName() = aEntryAlarm.iSound;
	aAlarm.DayOrTimed() = aEntryAlarm.iSessionType;

	if (aEntryAlarm.iAlarmTime.TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		aAlarm.NextDueTime() = aEntryAlarm.iAlarmTime.LocalL();
		}
	else
		{
		aAlarm.SetUtcNextDueTime(aEntryAlarm.iAlarmTime.UtcL());
		}

	if ( aIsSessionAlarm )
		{
		aAlarm.Characteristics().Set(EAlarmCharacteristicsSessionSpecific);
		}

	if (!iQueueAlarmsInPast)
		{
		aAlarm.Characteristics().Set(EAlarmCharacteristicsDeQueueIfDueTimeInPast);
		}
	}

void CAgnAlarmActive::FindAndQueueNextFewAlarmsL(const TInt aMaxNumberOfAlarms,const TInt aMaxNumberOfDays)
	{
	TTime now;
	now.HomeTime();
	
	FindAndQueueNextFewAlarmsL(now,now+TTimeIntervalDays(aMaxNumberOfDays),aMaxNumberOfAlarms); 
	}

void CAgnAlarmActive::FindAndQueueNextFewAlarmsL(const TTime& aStartTime, const TTime& aEndTime, const TInt aMaxNumberOfAlarms)
//
// Find the next due alarm(s) and if at least one exists then queue it with the alarm server
//
	{
	TTime currTime = aStartTime;

	CArrayFixFlat<TAgnAlarmEntry>* alarmList = new (ELeave) CArrayFixFlat<TAgnAlarmEntry>(10);
	CleanupStack::PushL(alarmList);
	
	// Certainly on WINS an alarm may expire a few seconds before it is due, the
	// following ensures that the same alarm isn't queued twice
	if ( iAlarmList->Count() )
		{
		TTime lastAlarm = ((*iAlarmList)[0]).iAlarmTime.LocalL();
		
		if ( currTime < lastAlarm )
			{
			currTime += TTimeIntervalSeconds(1);
			}			
		}

	CArrayFixFlat<TAgnSortInstance>* alarmedIds = new(ELeave) CArrayFixFlat<TAgnSortInstance>(10);
	CleanupStack::PushL(alarmedIds);

	iModel->NextFewAlarmsForServerL(currTime, aEndTime, *alarmedIds, aMaxNumberOfAlarms);
	
	const TInt KAlarmedIdsCount = alarmedIds->Count();
	
	for ( TInt ii = 0; ii < KAlarmedIdsCount; ++ii )
		{
		//create an alarmEntry
		TAgnAlarmEntry alarmEntry;
		
		if (AssignToL(alarmEntry, (*alarmedIds)[ii]))
			{
			TKeyArrayFix key(_FOFF(TAgnAlarmEntry, iAlarmTime), ECmpTInt64);//order by alarm time
			alarmList->InsertIsqAllowDuplicatesL(alarmEntry, key);
			}
		}
		
	CleanupStack::PopAndDestroy(alarmedIds);

	Cancel();

	if(iStatus != KErrLocked)
		{
		TASShdAlarm alarm;
		const TInt KCount = alarmList->Count();
		for( TInt ii = 0; ii < KCount; ++ii )
			{
			const TAgnAlarmEntry& alarmEntry = (*alarmList)[ii];

			InitialiseAlarmL(alarm, alarmEntry, EFalse);
			
			TAgnAlarmInfo alarmInfo;
			GetAlarmInfoL(alarmEntry.iInstanceId, alarmInfo);
			TPkgCAgnAlarmInfo pkgAgendaRef(alarmInfo);

			_DBGLOG_ALARM(	AgmDebug::DebugLog("Queueing first alarm on list:");)
			_DBGLOG_ALARM(AgmDebug::DebugLogAlarmL(alarmEntry);)

			iAlarmServer.AlarmAdd(alarm, pkgAgendaRef);
			}
		}
		
	CleanupStack::PopAndDestroy(alarmList);
	}

// Get the TAgnAlarmEntry struct for the current entry or return NULL if not available
const TAgnAlarmEntry* CAgnAlarmActive::CurrentAlarmEntry() const
	{
	if ( iAlarmList&&iAlarmList->Count() > 0 )
		{
		return ( &((*iAlarmList)[iAlarmListIndex]) );
		}
				
	return ( NULL );
	}
 
void CAgnAlarmActive::DoCancel()
//
// Cancel the alarm
//
// DoCancel only gets called from Cancel(). It will only be called if the object is active - which means that
// there should therefore be an alarm to cancel.
	{
	const TAlarmId KId = iAlarm.Id();
	if ( KId != KNullAlarmId )
		{
		iAlarmServer.AlarmNotificationCancelAndDequeue(KId); // resulting signal is picked up in Cancel()
		}
	}

void CAgnAlarmActive::GetAlarmInfoL(const TAgnInstanceId& aInstanceId, TAgnAlarmInfo& aAlarmInfo) const
	{
	aAlarmInfo.iEntryId = aInstanceId;
	
	aAlarmInfo.iTimeMode = aInstanceId.Date().TimeMode();
	
	if(aAlarmInfo.iTimeMode==MAgnCalendarTimeMode::EFloating)
		{
		aAlarmInfo.iInstanceTime = aInstanceId.Date().LocalL();
		}
	else
		{
		aAlarmInfo.iInstanceTime = aInstanceId.Date().UtcL();
		}

	aAlarmInfo.iAlarmCategory = KUidAgendaModelAlarmCategory;
 
 	//Remove path from the filename, leave only Drive:Filename.ext
	TParsePtrC parser(iModel->FileName());
	aAlarmInfo.iFileName = parser.Drive();
	aAlarmInfo.iFileName.Append(parser.NameAndExt());

	CAgnEntry* entry = iModel->FetchEntryL(aInstanceId);
	__ASSERT_ALWAYS(entry, User::Leave(KErrNotFound)); // alarm has expired for entry which cannot be found or has been deleted

	CleanupStack::PushL(entry);
	
	aAlarmInfo.iAgnUniqueId = entry->LocalUid();
	
	if(aInstanceId.Date().TimeMode()==MAgnCalendarTimeMode::EFloating)
		{
		aAlarmInfo.iRecurrenceId = entry->RecurrenceId().LocalL();
		}
	else
		{
		aAlarmInfo.iRecurrenceId = entry->RecurrenceId().UtcL();
		}	
	
	// Check the length of the Uid from the entry if it is less than the 
	// the supported length assign it to the aAlarmInfo.
	// as per the RFC we are supporting 256 chars most of  app UID is within this range
	// in future if anyone supports more than this length we need to handle this here .
	if (iModel->GetEntryGuidL(*entry).Length() <= KAgnEntryMaxGuidLength)	    
	    aAlarmInfo.iGlobalIdentifier = iModel->GetEntryGuidL(*entry);
	
	CleanupStack::PopAndDestroy(entry);
	}
	
void CAgnAlarmActive::SetQueueAlarmsInPast(TBool aQueueAlarmsInPast)
	{
	iQueueAlarmsInPast = aQueueAlarmsInPast;	
	}

const CAgnServFile& CAgnAlarmActive::AgnServFile()
    {
    return iModel->AgnServFile();
    }

CAgnAlarmServerChangeObserver* CAgnAlarmServerChangeObserver::NewL(MAgnAlarmServerTerminationCallBack* aCallBack, CAgnAlarm* aOwningObject)
//
// Create a CAgnAlarmServerChangeObserver object
//
	{
	CAgnAlarmServerChangeObserver* self = new(ELeave) CAgnAlarmServerChangeObserver(aCallBack, aOwningObject);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return (self);
	}

CAgnAlarmServerChangeObserver::CAgnAlarmServerChangeObserver(MAgnAlarmServerTerminationCallBack* aCallBack, CAgnAlarm* aOwningObject)
//
// C'tor
//
	:iCallBack(aCallBack), iOwningObject(aOwningObject)
	{
	}
	
void CAgnAlarmServerChangeObserver::ConstructL()
	{
	iTimeZoneConvertor = CAgnTlsProxy::CreateL(CAgnTlsProxy::TAgnTlsTzRulesType_Server);
	}


CAgnAlarmServerChangeObserver::~CAgnAlarmServerChangeObserver()
//
// D'tor
//
	{
	delete iSystemId;
	CAgnTlsProxy::Release(iTimeZoneConvertor);
	}

/**
Filter notifications from the alarm server, if KErrServerTerminated is received then
call the callback function
*/
void CAgnAlarmServerChangeObserver::HandleAlarmServerChangeL(TInt aChange)
	{

	if ( aChange == KErrServerTerminated )
		{
		if ( iCallBack )
			{
			iCallBack->AlarmServerTerminated();
			}			
		else if ( iUserRequestStatus )
			{
			User::RequestComplete(iUserRequestStatus,KErrServerTerminated);
			iUserRequestStatus = NULL;
			}
		}
	else // iStatus != KErrServerTerminated
		{
		if (aChange == EAlarmChangeEventSystemDateTimeChanged)
			{
			CTzId* newSysTime=iTimeZoneConvertor->TzServer().GetTimeZoneIdL();
			if( iSystemId && (*iSystemId==*newSysTime) )//when locale changes, we will receive notification twice but we only act once.
				{
				delete newSysTime;
				}
			else
				{
				delete iSystemId;
				iSystemId=newSysTime;
				}
				
			const TAgnAlarmEntry* alarmEntry = iOwningObject->AlarmEntry();
			TTime queuedEntryTime;

			// INC045770: Alarms less than X days old should fire when the system time is put forward.
			if ( alarmEntry && iOwningObject->QueueAlarmsInPast())
				{
				// Get the time of the currently scheduled alarm (serves as the old system time for our purposes.)
				queuedEntryTime=alarmEntry->iAlarmTime.LocalL();
				// sub 1st sec: FindAndQueue* adds 1 second to prevent scheduling an alarm twice.
				// sub 2nd sec: Our sess alarm that has been cancelled needs to be aged 1 sec to get queued.
				queuedEntryTime-=TTimeIntervalSeconds(2);  
		
				TTime now;
				now.HomeTime();
				TTimeIntervalDays delta = now.DaysFrom(queuedEntryTime);

				// clock set forward X days or more after the session alarm; schedule all alarms within X days of the curr time.
				if (delta >= TTimeIntervalDays(KSystemTimeChangeMaxDaysOld))
					iOwningObject->FindAndQueueNextFewAlarmsL(now-TTimeIntervalDays(KSystemTimeChangeMaxDaysOld),now,KSystemTimeChangeMaxAlarmsQueued);
				// clock set forward less than X days after sesion alarm, schedule all alarms between session alarm time and curr time
				else if (delta >= TTimeIntervalDays(0))
					iOwningObject->FindAndQueueNextFewAlarmsL(queuedEntryTime,now,KSystemTimeChangeMaxAlarmsQueued);				
				// else system time has been put backwards (normal code path can handle this case)
				}
			// *always* schedule a session alarm when the time changes
			iOwningObject->FindAndQueueNextAlarmL(ETrue);			
			}
		else if (aChange == EAlarmChangeEventRestoreCompleted ||
                    aChange == EAlarmChangeEventRestoreFailed)
		    {
		    // The restore has finished so try to find and queue the next few alarms
		    if (!iOwningObject->AgnServFile().IsLocked()
                    && !iOwningObject->AgnServFile().IsBackupRestoreLock())
                {
                iOwningObject->FindAndQueueNextAlarmL(EFalse);
                }		   
		    }
		}
	}

