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

#ifndef __AGSALARM_H__
#define __AGSALARM_H__

// System includes
#include <asclisession.h>
#include <asshdalarm.h>

// User includes
#include "agmids.h"
#include <calcommon.h>
#include "calcommonimpl.h"
#include <calalarm.h>

// Classes referenced
class CAgnEntryModel;
class CAgnAlarmActive;
class CAgnAlarmServerChangeObserver;
class TAgnSortInstance;
class CTzId;
class CAgnServFile;

class MAgnAlarmServerTerminationCallBack
/** The interface for an alarm server termination notifier.
If an object that implements this interface is supplied to 
CAgnAlarm::NewL(), its AlarmServerTerminated() function is 
called if the alarm server terminates.

@internalAll
@released
*/
	{
public:
	/** Callback function that is called if the alarm server terminates. */
	virtual void AlarmServerTerminated() = 0;
	};

struct TAgnAlarmEntry
/**
@internalAll
@released
*/
	{
	TTime				iDueDateTimeLocal;
	TAgnCalendarTime	iAlarmTime;
	TAlarmMessage		iMessage;
	TAlarmSoundName 	iSound;
	TAgnInstanceId		iInstanceId;
	TAlarmDayOrTimed	iSessionType;	// RASCliSession
	};

class CAgnAlarm : public CBase
/** The interface between the agenda model and the alarm server.

A session with the alarm server is created in the NewL() function. The agenda 
model's next due alarm is set as the session alarm using FindAndQueueNextAlarmL(). 
It is added to the alarm server's pending alarm queue and is handled by 
the alarm server when its due time is reached. 

If the session with the alarm server is disconnected, the session alarm is 
cancelled. If you want the alarm to run without reconnecting to the server, 
the alarm must be orphaned. Alternatively, you can reconnect to the server 
and re-queue the session alarm with the server using RestartL(). 

Optionally, a callback provider may be passed to the object on construction. This 
object defines a single function which is called if the alarm server terminates. 
For notification without a callback, call RequestServerTerminationNotification() instead.

@internalAll
@released
*/
	{
public:
	static CAgnAlarm* NewL(CAgnEntryModel* aModel,MAgnAlarmServerTerminationCallBack* aCallBack=NULL);
	virtual ~CAgnAlarm();
	void FindAndQueueNextAlarmL(TBool aSysTimeHasChanged);
	void FindAndQueueNextFewAlarmsL(const TInt aMaxNumberOfAlarms=10,const TInt aMaxNumberOfDays=31);
	void OrphanAlarm();
	void QueueNextAlarmL(CArrayFixFlat<TAgnSortInstance>*aAlarmedIds);
	void DeleteEntriesAlarmL(const TAgnEntryId& aEntryInstanceId);
	const TAgnAlarmEntry* AlarmEntry() const;
    void FindAndQueueNextFewAlarmsL(const TTime& startTime,const TTime& endTime,const TInt aMaxNumberOfAlarms=10);
	TBool QueueAlarmsInPast();
	const CAgnServFile& AgnServFile();
	void DeleteAllAlarms();
	CAgnAlarmServerChangeObserver& ChangeObserver();
	
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
    void DeleteAlarmsAndRequeueSessionAlarmL();
#endif
private:
    CAgnAlarm(RASCliSession& aRASCliSession);
	void ConstructL(CAgnEntryModel* aModel,MAgnAlarmServerTerminationCallBack* aCallBack);
	CAgnAlarmActive*					iAlarm;
	CAgnAlarmServerChangeObserver*	iChangeObserver;
	TInt 								iQueueAlarmsInPast;
	RASCliSession&                      iAlarmServer;
	};


/**
@internalTechnology
@released
*/
struct TAgnAlarmInfo
	{
   	TUid iAlarmCategory;
   	TAgnEntryId iEntryId;
   	TTime iInstanceTime;
   	MAgnCalendarTimeMode::TTimeMode iTimeMode;
   	TFileName iFileName;
	TBuf8<KAgnEntryMaxGuidLength> iGlobalIdentifier;
	TTime iRecurrenceId;
	TCalLocalUid iAgnUniqueId;	
	};

/**
Defines a package for a system-wide unique agenda entry reference.

/**
@internalTechnology
@released
*/
typedef TPckgC<TAgnAlarmInfo> TPkgCAgnAlarmInfo;

class CAgnAlarmActive : public CActive
/**
@internalAll
@released
*/
 	{
public:
	static CAgnAlarmActive* NewL(CAgnEntryModel* aModel,RASCliSession& aAlarmServer);
	virtual ~CAgnAlarmActive();
	virtual void RunL();
	void FindAndQueueNextAlarmL(TBool aSysTimeHasChanged);
	void FindAndQueueNextFewAlarmsL(const TInt aMaxNumberOfAlarms,const TInt aMaxNumberOfDays);
	void FindAndQueueNextFewAlarmsL(const TTime& startTime,const TTime& endTime,const TInt aMaxNumberOfAlarms);

	void OrphanAlarm();
	void QueueNextAlarmL(CArrayFixFlat<TAgnSortInstance>*aAlarmedIds);
	const TAgnAlarmEntry* CurrentAlarmEntry() const;
	void SetQueueAlarmsInPast(TBool aQueueAlarmsInPast);
	const CAgnServFile& AgnServFile();
private:
	CAgnAlarmActive(TInt aPriority,RASCliSession& aAlarmServer);
	void GetAlarmInfoL(const TAgnInstanceId& aInstanceId, TAgnAlarmInfo& aAlarmInfo) const;
	void InitialiseAlarmL(TASShdAlarm& aAlarm, const TAgnAlarmEntry& aEntryAlarm, TBool aIsSessionAlarm); 

	virtual void DoCancel();
	
	TBool AssignToL(TAgnAlarmEntry& aAlarmEntry, const TAgnSortInstance& aSortInstance);
	
private:
	enum {EPriority = EPriorityLow,EGranularity=2};
	RASCliSession&					iAlarmServer;
	CArrayFixFlat<TAgnAlarmEntry>*	iAlarmList;
	TInt							iAlarmListIndex; //index of the currently queued alarm
	CAgnEntryModel*                 iModel; // pointer to owning CAgnModel class
	TASShdAlarm						iAlarm;
	TBool 							iQueueAlarmsInPast;
	};

class CAgnAlarmServerChangeObserver : public CBase
/**
@internalAll
@released
*/
	{
public:
	static CAgnAlarmServerChangeObserver* NewL(MAgnAlarmServerTerminationCallBack* aCallBack, CAgnAlarm* aOwningObject);
	~CAgnAlarmServerChangeObserver();
	
	void HandleAlarmServerChangeL(TInt aChange);

private:
    CAgnAlarmServerChangeObserver(MAgnAlarmServerTerminationCallBack* iCallBack, CAgnAlarm* aOwningObject);	
	void ConstructL();
	
private:
	MAgnAlarmServerTerminationCallBack*	iCallBack;
	TRequestStatus*		iUserRequestStatus;
	CAgnAlarm*			iOwningObject;
	CTzId*				iSystemId;
	CAgnTlsProxy*		iTimeZoneConvertor;//not owned
	};

#endif
