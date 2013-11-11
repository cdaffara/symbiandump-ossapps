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




#ifndef CREATORCALENDARELEMENT_H_
#define CREATORCALENDARELEMENT_H_

#include "creator_scriptelement.h"

class CCalenderInterimParameters;

// Calendar elements:
namespace creatorcalendar
{
    _LIT(KCalendar, "calendar");

    // Calendar entry types:
    _LIT(KType, "type");
    _LIT(KCalTypeAppointment, "appointment");
    _LIT(KCalTypeEvent, "event");
    _LIT(KCalTypeReminder, "reminder");
    _LIT(KCalTypeAnniversary, "anniversary");
    _LIT(KCalTypeTodo, "todo");

    // Calendar entry field elements:
    _LIT(KSummary, "summary");
    _LIT(KDescription, "description");
    _LIT(KLocation, "location");
    _LIT(KStarttime, "starttime");
    _LIT(KEndtime, "endtime");
    _LIT(KRecurrentFreq, "recurrentfrequency");
    _LIT(KRecurrentInterval, "recurrentinterval");
    _LIT(KRecurrentFrom, "recurrentfrom");
    _LIT(KRecurrentTo, "recurrentto");
    _LIT(KRepeatingCount, "repeatingcount");
    _LIT(KAlarmtime, "alarmtime");
    _LIT(KSynchronization, "synchronization");
    _LIT(KOrganizername, "organizername");
    _LIT(KOrganizeremail, "organizeremail");
    _LIT(KAttendees, "attendees");
    _LIT(KAttendee, "attendee");
    _LIT(KCommonname, "commonname");
    _LIT(KEmail, "email");
    _LIT(KRole, "role");
    _LIT(KStatus, "status");
    _LIT(KPriority, "priority");

    // Recurrency frequency values:
    _LIT(KNotRepeated, "not-repeated");
    _LIT(KWeekly, "weekly");
    _LIT(KDaily, "daily");
    _LIT(KMonthly, "monthly");
    _LIT(KYearly, "yearly");

    // Attendee role values:
    _LIT(KRoleRequired, "required");
    _LIT(KRoleOptional, "optional");
    _LIT(KRoleNonPart, "non-participant");
    _LIT(KRoleChair, "chair");
    
    // Calendar entry status values:
    _LIT(KCalStatusTentative, "tentative");
    _LIT(KCalStatusConfirmed, "confirmed");
    _LIT(KCalStatusCancelled, "cancelled");
    _LIT(KCalStatusNeedsAction, "todoneedsaction");
    _LIT(KCalStatusCompleted, "todocompleted");
    _LIT(KCalStatusInProcess, "todoinprocess");

    // Attendee status values:
    _LIT(KStatusNeedsAction, "needsaction");
    _LIT(KStatusAccepted, "accepted");
    _LIT(KStatusTentative, "tentative");
    _LIT(KStatusConfirmed, "confirmed");
    _LIT(KStatusDeclined, "declined");
    _LIT(KStatusCompleted, "completed");
    _LIT(KStatusDelegated, "delegated");
    _LIT(KStatusInProcess, "inprocess");
    
    // Priority values:
    _LIT(KPriorityHigh, "high");
    _LIT(KPriorityMedium, "medium");
    _LIT(KPriorityLow, "low");
}

class CCreatorCalendarElement : public CCreatorScriptElement
{
public:
    static CCreatorCalendarElement* NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext = KNullDesC);
    void ExecuteCommandL();
protected:
    CCreatorCalendarElement(CCreatorEngine* aEngine);
    
    enum TEntryType {
        EAppointment,
        EEvent,
        EReminder,
        EAnniversary,
        ETodo
    };
    
    void FillEntryParamsL(TEntryType aType, const RPointerArray<CCreatorScriptElement>& fields, CCalenderInterimParameters* parameters); 
};

#endif /*CREATORCALENDARELEMENT_H_*/
