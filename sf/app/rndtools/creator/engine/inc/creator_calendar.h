/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/





#ifndef __CREATORCALENDARINTERIM_H__
#define __CREATORCALENDARINTERIM_H__

#include "engine.h"
#include "creator_modulebase.h"
#include "creator_calendarbase.h"

#include <e32base.h>
#include <e32std.h>

//#include <calenlauncher.h> 
#include <calentry.h>
#include <calentryview.h>
#include <caltime.h>
#include <calsession.h>
#include <calprogresscallback.h>
#include <calcategory.h>
#include <caluser.h>
#include <calrrule.h>


class CCreatorEngine;
class CCalenderInterimParameters;
class CAsyncWaiter;

class CCreatorInterimCalendar : public CCreatorCalendarBase, public MCalProgressCallBack
    {
enum TCreatorCalendarStatus{
    ECreatorCalendarDelete = 0,
    ECreatorCalendarStart, 
    ECreatorCalendarGetWeeklyEntries,
    ECreatorCalendarGetAttendies,
    ECreatorCalendarGetStartDate,
    ECreatorCalendarGetEndDate, 
    ECreatorCalendarAddDefaultAlarm
};

public: 
    static CCreatorInterimCalendar* NewL(CCreatorEngine* aEngine);
    static CCreatorInterimCalendar* NewLC(CCreatorEngine* aEngine);
    ~CCreatorInterimCalendar();

private:
    CCreatorInterimCalendar();
    void ConstructL(CCreatorEngine* aEngine); // from MCreatorModuleBase
    void AddRepeatingRuleL(const TCalTime& aStartTime, CCalEntry& aCalEntry, CCalenderInterimParameters* parameters=0);
    void SetRandomAlarmL(CCalEntry& aCalEntry);
    void SetAlarmL(CCalEntry& aCalEntry, CCalenderInterimParameters* params);
    void AddAttendeesL(CCalEntry& aCalEntry, CCalenderInterimParameters* parameters=0);
    void SetPhoneOwnerL(CCalEntry& aCalEntry, const TDesC& aCN, const TDesC& aEmail, const TDesC& aSentBy );
    void SetOrganizerL(CCalEntry& aCalEntry, const TDesC& aCN, const TDesC& aEmail, const TDesC& aSentBy);
    void SetOrganizerL(CCalEntry& aCalEntry, CCalenderInterimParameters* parameters );
    
public:
    TBool AskDataFromUserL( TInt aCommand ); // from MCreatorModuleBase
    void QueryDialogClosedL(TBool aPositiveAction, TInt aUserData); //from MUIObserver
    
    TInt CreateAppointmentEntryL(CCreatorModuleBaseParameters *aParameters);    
    TInt CreateEventEntryL(CCreatorModuleBaseParameters *aParameters);
    TInt CreateAnniversaryEntryL(CCreatorModuleBaseParameters *aParameters); 
    TInt CreateTodoEntryL(CCreatorModuleBaseParameters *aParameters);
    TInt CreateReminderEntryL(CCreatorModuleBaseParameters *aParameters);
    
    void GenerateRandomId(TDes8& aText);
    void DeleteAllL();
    void DeleteAllCreatedByCreatorL();

private:

    CCalenderInterimParameters* iParameters;
    TTime   iRandomModeStartTime;
    TTime   iRandomModeEndTime;
    TInt    iNumberRepeatingEntries;
    TInt    iNumberOfAttendees;
    CCalEntryView* 		iCalEntryView;
    CCalSession* 		iCalSession;
    TBool iSetAlarm;
    CDesCArrayFlat* iSoundFileArray;
    CAsyncWaiter* iWaiter;
    HBufC* iTmpCN;
    HBufC* iTmpEmail;
    RArray<TUint32> iEntryIds; // TCalLocalUid ( == TUint32 ) CCalEntry::LocalUidL

public:

    //Observer implementation for MCalProgressCallBack
	void Progress(TInt aPercentageCompleted);
	
	void Completed(TInt aError);
	
	TBool NotifyProgress();

    };

class CCreatorCalenderAttendee : public CBase
{
public:
    virtual ~CCreatorCalenderAttendee();
    static CCreatorCalenderAttendee* NewL();
    void SetCommonNameL(const TDesC& aName);
    void SetEmailAddressL(const TDesC& aEmail);
    void SetRole(CCalAttendee::TCalRole aRole);
    void SetStatus(CCalAttendee::TCalStatus);
    
    const TDesC& CommonName() const;
    const TDesC& EmailAddress() const;
    CCalAttendee::TCalRole Role() const;
    CCalAttendee::TCalStatus Status() const;
    
private:
    CCreatorCalenderAttendee();
    TPtrC  iDummy;
    HBufC* iCommonName;
    HBufC* iEmailAddress;
    CCalAttendee::TCalRole iRole;
    CCalAttendee::TCalStatus iStatus;
};

class CCalenderInterimParameters : public CCreatorModuleBaseParameters
    {
public:

	void ParseL(CCommandParser* parser, TParseParams aCase);
	TBool IsAllowedEntryStatus(CCalEntry::TType aType, CCalEntry::TStatus aStatus);

	TCalRRule::TType iRecurrentFrequency;
	TInt iRecurrentInterval;
	TTime iRecurrentStartTime;
	TTime iRecurrentEndTime;
	TInt iRepeatingCount; // This is used when running a script...
	TTime iAlarmTime; // NullTTime means no alarm
	TBool iUseRandomAlarmTime; // ETrue if random alarm should be set. Overwrites the iAlarmTime.
	
	RPointerArray<CCreatorCalenderAttendee> iAttendees;
	RArray<TLinkIdParam> iAttendeeLinkIds; // For attendees. Stores the linked contact ids.
	HBufC* iOrganizerName;
	HBufC* iOrganizerEmail;
		 
	HBufC*  iDescription;
	HBufC*  iSummary;
	HBufC*  iLocation;
	TTime   iStartTime;
	TTime   iEndTime;
	TInt    iNumberRepeatingEntries; // This is used when user input is used...
	TInt    iNumberOfAttendees; // Number of randomly generated attendees
	TInt    iNumberOfExistingAttendees; // Number of attendees selected from existing contacts.
	
	//An appointment, which has a start time and end time.
    TTime   iAppointmentStartTime;
    TTime   iAppointmentEndTime;
    HBufC*  iAppointmentLocation;
    HBufC*  iAppointmentReason;

    //An event, which has a start time and end time.
    
    TTime   iEventStartTime;
    TTime   iEventEndTime;
    HBufC*  iEventReason;

    //A reminder, which has a start time only.
    TTime   iReminderStartTime;
    HBufC*  iReminderReason;
    
    //An anniversary, which has a start time and end time.
    TTime   iAnniversaryStartTime;
    TTime   iAnniversaryEndTime;
    HBufC*  iAnniversaryReason;

    //A to-do, which can have a start time and end time (the end time is the due date), or can be undated.
    TTime   iTodoStartTime;//optional
    TTime   iTodoDueTime;//also end time (optional)
    HBufC*  iTodoTask;
    TInt    iTodoPriority;

    TTime   iRandomModeStartTime;
    TTime   iRandomModeEndTime;
    
    CCalEntry::TStatus iStatus;
    
public:
	CCalenderInterimParameters();
    ~CCalenderInterimParameters();   
    
    };



#endif // __CREATORCALENDARINTERIM_H__
