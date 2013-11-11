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


#include "engine.h"
#include "enginewrapper.h"

#include "creator_calendar.h" 
#include "creator_traces.h"
#include "creator_scriptentry.h"
#include <e32base.h>
#include <e32math.h>
#include <calrrule.h> 
#include <calalarm.h>
#include "creator_contactsetcache.h"
#include "creator_phonebook.h"

static const TInt KCalendarFieldLength = 128;


CCreatorCalenderAttendee::CCreatorCalenderAttendee() 
    {}

CCreatorCalenderAttendee::~CCreatorCalenderAttendee()
    {
    delete iCommonName;
    delete iEmailAddress;
    }

CCreatorCalenderAttendee* CCreatorCalenderAttendee::NewL()
    {
    CCreatorCalenderAttendee* self = new (ELeave) CCreatorCalenderAttendee;
    return self;
    }

void CCreatorCalenderAttendee::SetCommonNameL(const TDesC& aName)
    {
    delete iCommonName;
    iCommonName = 0;
    iCommonName = HBufC::NewL(aName.Length());
    iCommonName->Des().Copy(aName);
    }

void CCreatorCalenderAttendee::SetEmailAddressL(const TDesC& aEmail)
    {
    delete iEmailAddress;
    iEmailAddress = 0;
    iEmailAddress = HBufC::NewL(aEmail.Length());
    iEmailAddress->Des().Copy(aEmail);
    }

void CCreatorCalenderAttendee::SetRole(CCalAttendee::TCalRole aRole)
    {
    iRole = aRole;
    }

void CCreatorCalenderAttendee::SetStatus(CCalAttendee::TCalStatus aStatus)
    {
    iStatus = aStatus;
    }

const TDesC& CCreatorCalenderAttendee::CommonName() const
    {
    if( iCommonName == 0 )
    	return iDummy;
    return *iCommonName;
    }

const TDesC& CCreatorCalenderAttendee::EmailAddress() const
    {
    if( iEmailAddress == 0 )
    	return iDummy;
    
    return *iEmailAddress;
    }

CCalAttendee::TCalRole CCreatorCalenderAttendee::Role() const
    {
    return iRole;
    }

CCalAttendee::TCalStatus CCreatorCalenderAttendee::Status() const
    {
    return iStatus;
    }

TBool CCalenderInterimParameters::IsAllowedEntryStatus(CCalEntry::TType aType, CCalEntry::TStatus aStatus)
    {
    if( aType == CCalEntry::ETodo )
        {
        return  aStatus == CCalEntry::ECancelled ||
                aStatus == CCalEntry::ETodoNeedsAction ||
                aStatus == CCalEntry::ETodoCompleted ||
                aStatus == CCalEntry::ETodoInProcess;
        }
    
    return  aStatus == CCalEntry::ETentative ||
            aStatus == CCalEntry::EConfirmed ||
            aStatus == CCalEntry::ECancelled;
    }

//----------------------------------------------------------------------------

CCalenderInterimParameters::CCalenderInterimParameters ( )
    {
    LOGSTRING("Creator: CCalenderInterimParameters::CCalenderInterimParameters");

    iAppointmentLocation = HBufC::New (KCalendarFieldLength );
    iAppointmentReason = HBufC::New (KCalendarFieldLength );
    iEventReason = HBufC::New (KCalendarFieldLength );
    iAnniversaryReason = HBufC::New (KCalendarFieldLength );
    iTodoTask = HBufC::New (KCalendarFieldLength );
    iReminderReason = HBufC::New (KCalendarFieldLength );
    iDescription = HBufC::New (KCalendarFieldLength );
    iSummary = HBufC::New (KCalendarFieldLength );
    iLocation = HBufC::New (KCalendarFieldLength );
    iOrganizerName = HBufC::New (KCalendarFieldLength );
    iOrganizerEmail = HBufC::New (KCalendarFieldLength );

    // set defaults, start date today and end date = today + 1 month
    iRandomModeStartTime.HomeTime ( );
    iRandomModeEndTime = iRandomModeStartTime;
    iRandomModeEndTime += TTimeIntervalMonths (1 );
    iAlarmTime = Time::NullTTime();
    iUseRandomAlarmTime = EFalse;
    iStatus = CCalEntry::ENullStatus;
    }

CCalenderInterimParameters::~CCalenderInterimParameters ( )
    {
    LOGSTRING("Creator: CCalenderInterimParameters::~CCalenderInterimParameters");

    delete iTodoTask;
    delete iAnniversaryReason;
    delete iEventReason;
    delete iAppointmentReason;
    delete iAppointmentLocation;
    delete iReminderReason;
    delete iDescription;
    delete iSummary;
    delete iLocation;
    delete iOrganizerName;
    delete iOrganizerEmail;
    iAttendees.ResetAndDestroy();
    iAttendees.Close();
    iAttendeeLinkIds.Reset();
    iAttendeeLinkIds.Close();
    }

void CCalenderInterimParameters::ParseL (CCommandParser* parser,
        TParseParams aCase )
    {
    LOGSTRING("Creator: CCalenderInterimParameters::ParseL");

    HBufC* tempBuf= NULL;

    switch (aCase )
        {

        case MCreatorModuleBaseParameters::ECalendarTodo:
            {

            parser->StrParserL (tempBuf, '|' );

            CleanupStack::PushL (tempBuf );

            this->iTodoDueTime = TTime (tempBuf->Des ( ) );

            parser->StrParserL (this->iTodoTask, '|' );

            parser->StrParserL (tempBuf, ';' );
            this->iTodoPriority = 2;
            if (tempBuf->Compare (_L("LOW") ) == 0 )
                this->iTodoPriority = 3;
            else
                if (tempBuf->Compare (_L("HIGH") ) == 0 )
                    this->iTodoPriority = 1;

            CleanupStack::PopAndDestroy (tempBuf );

            }
            break;

        case MCreatorModuleBaseParameters::ECalendarMemo:
            {

            parser->StrParserL (tempBuf, '|' );

            this->iStartTime = TTime (tempBuf->Des ( ) );

            parser->StrParserL (tempBuf, '|' );
            this->iEndTime = TTime (tempBuf->Des ( ) );

            parser->StrParserL (this->iDescription, ';' );

            CleanupStack::PopAndDestroy (tempBuf );
            }
            break;

        case MCreatorModuleBaseParameters::ECalendarMeeting:
            {

            parser->StrParserL (tempBuf, '|' );

            CleanupStack::PushL (tempBuf );

            this->iStartTime = TTime (tempBuf->Des ( ) );

            parser->StrParserL (tempBuf, '|' );
            this->iEndTime = TTime (tempBuf->Des ( ) );

            parser->StrParserL (this->iLocation, '|' );
            parser->StrParserL (this->iDescription, ';' );

            CleanupStack::PopAndDestroy (tempBuf );
            }
            break;

        case MCreatorModuleBaseParameters::ECalendarAnniv:
            {

            parser->StrParserL (tempBuf, '|' );

            CleanupStack::PushL (tempBuf );

            this->iStartTime = TTime (tempBuf->Des ( ) );

            parser->StrParserL (this->iDescription, ';' );

            CleanupStack::PopAndDestroy (tempBuf );

            }
            break;
        default:
            {
            //implement 
            }
            break;
        }
    }
//----------------------------------------------------------------------------

CCreatorInterimCalendar* CCreatorInterimCalendar::NewL (CCreatorEngine* aEngine )
    {
    CCreatorInterimCalendar* self = CCreatorInterimCalendar::NewLC (aEngine );
    CleanupStack::Pop (self );
    return self;
    }

CCreatorInterimCalendar* CCreatorInterimCalendar::NewLC (CCreatorEngine* aEngine )
    {
    CCreatorInterimCalendar* self = new (ELeave) CCreatorInterimCalendar;
    CleanupStack::PushL (self );
    self->ConstructL (aEngine );
    return self;
    }

CCreatorInterimCalendar::CCreatorInterimCalendar ( )
    {
    }

void CCreatorInterimCalendar::ConstructL (CCreatorEngine* aEngine )
    {
    LOGSTRING("Creator: CCreatorInterimCalendar::ConstructL");

    iEngine = aEngine;

    // set defaults, start date today and end date = today + 1 month
    iRandomModeStartTime.HomeTime ( );
    iRandomModeEndTime = iRandomModeStartTime;
    iRandomModeEndTime += TTimeIntervalMonths (1 );

    iCalSession = CCalSession::NewL ( );
    // Create and open a calendar file
    const TDesC& fileName = iCalSession->DefaultFileNameL ( );//default filename is: "C:Calendar"
    TRAPD(err, iCalSession->CreateCalFileL(fileName))
    ;
    // ignore KErrAlreadyExists leave code
    if (err != KErrAlreadyExists )
        {
        User::LeaveIfError (err );
        }
    // Open the calendar file
    iCalSession->OpenL (fileName );
    iCalEntryView = CCalEntryView::NewL (*iCalSession, *this );
    iNumberRepeatingEntries = 0;
    iNumberOfAttendees = 0; //iEngine->RandomNumber(2, 15);
    iSetAlarm = EFalse;
    iWaiter = CAsyncWaiter::NewL();
    }

CCreatorInterimCalendar::~CCreatorInterimCalendar ( )
    {
    LOGSTRING("Creator: CCreatorInterimCalendar::~CCreatorInterimCalendar");

    if ( iEngine && iEntryIds.Count() )
        {
        TRAP_IGNORE( iEngine->WriteEntryIdsToStoreL( iEntryIds, KUidDictionaryUidCalendar ) );
        }
        
    iEntryIds.Reset();
    iEntryIds.Close();
        
    if (iParameters )
        {
        delete iParameters;
        }

    if (iCalEntryView )
        {
        delete iCalEntryView;
        }

    if (iCalSession )
        {
        delete iCalSession;
        }
    delete iSoundFileArray;
    delete iWaiter;
    delete iTmpCN;
    delete iTmpEmail;
    }

//----------------------------------------------------------------------------

void CCreatorInterimCalendar::QueryDialogClosedL(TBool aPositiveAction, TInt aUserData)
    {
    LOGSTRING("Creator: CCreatorInterimCalendar::QueryDialogClosedL");
    
    if( aPositiveAction == EFalse && aUserData != ECreatorCalendarAddDefaultAlarm )
        {
        iEngine->ShutDownEnginesL();
        return;
        }
    
    const TDesC* showText = &KSavingText;
    TBool finished(EFalse);
    TBool retval(ETrue);
    switch(aUserData)
        {
        case ECreatorCalendarDelete:
            iEntriesToBeCreated = 1;
            showText = &KDeletingText;
            finished = ETrue;
            break;
        case ECreatorCalendarStart:
            if( iCommand == ECmdCreateCalendarEntryEvents || iCommand == ECmdCreateCalendarEntryAppointments )
                {
                _LIT(KRepeatingNum, "How many repeating (weekly) entries");
                retval = iEngine->GetEngineWrapper()->EntriesQueryDialog(&iNumberRepeatingEntries, KRepeatingNum, ETrue, this, ECreatorCalendarGetWeeklyEntries);
                }
            else
                {
                _LIT(KStartPeriodDate, "Start date of creation period");// \n dd/mm/yyyy
                retval = iEngine->GetEngineWrapper()->TimeQueryDialog(&iRandomModeStartTime, KStartPeriodDate, this, ECreatorCalendarGetStartDate); // ask start date
                }
            break;
        case ECreatorCalendarGetWeeklyEntries:
            _LIT(KAttendeesNum, "How many attendees to create");
            retval = iEngine->GetEngineWrapper()->EntriesQueryDialog(&iNumberOfAttendees, KAttendeesNum, ETrue, this, ECreatorCalendarGetAttendies);
            break;
        case ECreatorCalendarGetAttendies:
            {
            // ask start date
            _LIT(KStartPeriodDate, "Start date of creation period");
            retval = iEngine->GetEngineWrapper()->TimeQueryDialog(&iRandomModeStartTime, KStartPeriodDate, this, ECreatorCalendarGetStartDate ); 
            }
            break;
        case ECreatorCalendarGetStartDate:
            {
            _LIT(KEndPeriodDate, "End date of creation period\n dd/mm/yyyy");
            iEngine->GetEngineWrapper()->TimeQueryDialog (&iRandomModeEndTime, KEndPeriodDate, this, ECreatorCalendarGetEndDate ); // ask end date
            }
            break;
        case ECreatorCalendarGetEndDate:
           if (iRandomModeStartTime > iRandomModeEndTime ) // check end date is ok
                {
                iEngine->GetEngineWrapper()->ShowNote(_L("End date must be after start date") );
                retval = EFalse;
                }
            else 
                {
                retval = iEngine->GetEngineWrapper()->YesNoQueryDialog(_L("Add default alarm?"), this, ECreatorCalendarAddDefaultAlarm);
                }
           break;
        case ECreatorCalendarAddDefaultAlarm:
            iSetAlarm = aPositiveAction;
            finished = ETrue;
            break;
        default:
            //some error
            retval = EFalse;
            break;
        }
    if( retval == EFalse )
        {
        iEngine->ShutDownEnginesL();
        }
    else if( finished )
        {
        // add this command to command array
        iEngine->AppendToCommandArrayL(iCommand, NULL, iEntriesToBeCreated);
        // started exucuting commands
        iEngine->ExecuteFirstCommandL( *showText );
        }
    }

//----------------------------------------------------------------------------

TBool CCreatorInterimCalendar::AskDataFromUserL( TInt aCommand )
    {
    LOGSTRING("Creator: CCreatorInterimCalendar::AskDataFromUserL");
    
    CCreatorModuleBase::AskDataFromUserL( aCommand );//ignore retval
    
    if( aCommand == ECmdDeleteCalendarEntries )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog(_L("Delete all calendar entries?"), this, ECreatorCalendarDelete );
        }
    if( aCommand == ECmdDeleteCreatorCalendarEntries )
        {
        return iEngine->GetEngineWrapper()->YesNoQueryDialog(_L("Delete all calendar entries created with Creator?"), this, ECreatorCalendarDelete ); 
        }

    // display queries
    return iEngine->GetEngineWrapper()->EntriesQueryDialog(&iEntriesToBeCreated, _L("How many entries to create?"), EFalse, this, ECreatorCalendarStart ); // ask number of entries to create
    }

//----------------------------------------------------------------------------

TInt CCreatorInterimCalendar::CreateAppointmentEntryL (
        CCreatorModuleBaseParameters *aParameters )
    {
    LOGSTRING("Creator: CCreatorInterimCalendar::CreateAppointmentEntryL");
        
    delete iParameters;
    iParameters = 0;
    
    CCalenderInterimParameters* parameters = (CCalenderInterimParameters*)aParameters;   
    
    if( !parameters )
        {
        iParameters = new(ELeave) CCalenderInterimParameters;
        parameters = iParameters;  
        parameters->iRandomModeStartTime = iRandomModeStartTime;
        parameters->iRandomModeEndTime = iRandomModeEndTime;    
    
        // difference between start and end times
        TTimeIntervalMinutes differenceInMinutes;
        parameters->iRandomModeEndTime.MinutesFrom (
                parameters->iRandomModeStartTime, differenceInMinutes );

        // start time (= something between the period start and end date)        
        parameters->iStartTime = parameters->iRandomModeStartTime;
        parameters->iStartTime
                += TTimeIntervalMinutes (iEngine->RandomNumber (differenceInMinutes.Int ( ) ) );   
        // set end time (= start time + some random time)    
        parameters->iEndTime = parameters->iStartTime;    
        parameters->iEndTime
                += TTimeIntervalMinutes (iEngine->RandomNumber (5, 500 ) );
        

        // strings        
        parameters->iLocation->Des ( )
                = iEngine->RandomString (CCreatorEngine::EMeetingPlace );
        parameters->iDescription->Des ( )
                = iEngine->RandomString (CCreatorEngine::EMeetingReason );
        
        parameters->iSummary->Des()
                        = parameters->iDescription->Des();
        
        parameters->iNumberRepeatingEntries = iNumberRepeatingEntries;
        parameters->iNumberOfAttendees = iNumberOfAttendees;
        }

    TInt err = KErrNone;
    HBufC8* guid = HBufC8::NewLC (255 );
    TPtr8 uidP=guid->Des ( );
    GenerateRandomId (uidP );
    CCalEntry* entry = CCalEntry::NewL (CCalEntry::EAppt, guid,
            CCalEntry::EMethodAdd, 0 );
    CleanupStack::PushL (entry );

    //Using same text as description and summary	
    entry->SetDescriptionL (parameters->iDescription->Des ( ) );
    entry->SetSummaryL (parameters->iSummary->Des ( ) );
    entry->SetLocationL (parameters->iLocation->Des ( ) );

    TCalTime startTime;
    TCalTime endTime;
    // Set the start and end times using time values local to the current system time zone
    TInt month = (TInt) parameters->iStartTime.DateTime().Month();
    TInt date = parameters->iStartTime.DateTime().Day();
    startTime.SetTimeLocalL (parameters->iStartTime );
    endTime.SetTimeLocalL (parameters->iEndTime );
    entry->SetStartAndEndTimeL (startTime, endTime );       
    
    if( parameters->IsAllowedEntryStatus(CCalEntry::EAppt, parameters->iStatus))
        {
        LOGSTRING2("Creator: CCreatorInterimCalendar::CreateAppointmentEntryL setting entry status: %d", parameters->iStatus);
        entry->SetStatusL(parameters->iStatus);
        }
    
    // Repeating times:
    AddRepeatingRuleL(startTime, *entry, parameters);
    SetOrganizerL(*entry, parameters);
    
    // Alarm:    
    if( iSetAlarm || parameters->iUseRandomAlarmTime )
        {        
        SetRandomAlarmL(*entry);
        }
    else
        {
        SetAlarmL(*entry, parameters);
        }
        
    // Add attendees:
    AddAttendeesL(*entry, parameters);
    
    RPointerArray<CCalEntry> array;
    CleanupClosePushL (array );
    array.AppendL (entry );
    
    TInt success(0);
    //If StoreL() leaves, 'success' contains the number of entries that were stored before it failed
    iCalEntryView->StoreL (array, success );

    // id has been generated, store it for being able to delete
    // entries created only with Creator
    iEntryIds.AppendL( entry->LocalUidL() );
    
    //One item should be added at time
    if (success != 1 )
        {
        err = 1;
        }

    CleanupStack::PopAndDestroy (&array );
    CleanupStack::PopAndDestroy (entry );
    CleanupStack::Pop (guid );//guid has been destroyed by entry, which has ownership to guid

    return err;
    }

void CCreatorInterimCalendar::SetOrganizerL(CCalEntry& aCalEntry, CCalenderInterimParameters* parameters )
    {    
    HBufC* orgName = 0;
    HBufC* orgEmail = 0;
    _LIT(KSpace, " ");
    
    //
    // Common name
    //
    if( parameters == 0 || 
        parameters->iOrganizerName == 0 || 
        parameters->iOrganizerName->Length() == 0 )
        {     
        // Random:
        TPtrC firstname  = iEngine->RandomString(CCreatorEngine::EFirstName);
        TPtrC lastname = iEngine->RandomString(CCreatorEngine::ESurname);
        orgName = HBufC::NewL(firstname.Length() + lastname.Length() + 1 );
        CleanupStack::PushL(orgName);
        orgName->Des().Copy(firstname);
        orgName->Des().Append(KSpace);
        orgName->Des().Append(lastname);        
        }
    else
        {
        orgName = HBufC::NewL(parameters->iOrganizerName->Length());
        CleanupStack::PushL(orgName);
        orgName->Des().Copy(parameters->iOrganizerName->Des());
        }
    
    //
    // Email
    //
    if( parameters == 0 ||
        parameters->iOrganizerEmail == 0 || 
        parameters->iOrganizerEmail->Length() == 0 )
        {        
        // Random:
        TDesC* tmp = iEngine->CreateEmailAddressLC();
        orgEmail = HBufC::NewL(tmp->Length());
        orgEmail->Des().Copy(*tmp);
        CleanupStack::PopAndDestroy(); // tmp
        CleanupStack::PushL(orgEmail);
        }
    else
        {
        orgEmail = HBufC::NewL(parameters->iOrganizerEmail->Length());
        CleanupStack::PushL(orgEmail);
        orgEmail->Des().Copy(parameters->iOrganizerEmail->Des());
        }
    
    SetOrganizerL(aCalEntry, orgName->Des(), orgEmail->Des(), orgEmail->Des());
    CleanupStack::PopAndDestroy(); // orgName
    CleanupStack::PopAndDestroy(); // orgEmail
    }

void CCreatorInterimCalendar::AddRepeatingRuleL(const TCalTime& aStartTime, CCalEntry& aCalEntry, CCalenderInterimParameters* parameters )
    {
    RArray<TDay> days;
    CleanupClosePushL(days);
    
    // Recurrence rule. Weekly event.
    if( parameters->iNumberRepeatingEntries > 0 )
        {
        TCalRRule calRepeat(TCalRRule::EWeekly);   
        calRepeat.SetDtStart(aStartTime);
        calRepeat.SetCount(parameters->iNumberRepeatingEntries);        
        calRepeat.SetInterval(1);
        //days.AppendL((TDay) iEngine->RandomNumber((TInt)EMonday, (TInt) ESunday));
        TDay tempDay(aStartTime.TimeLocalL().DayNoInWeek());
        days.AppendL(tempDay);
        calRepeat.SetByDay(days);        
        aCalEntry.SetRRuleL(calRepeat);
        }
    else if(parameters->iRecurrentFrequency > TCalRRule::EInvalid)
        {
        TCalTime calTime;        
        // Recurrent frequency
        TCalRRule calRepeat(parameters->iRecurrentFrequency);
        // Recurrent start        
        calRepeat.SetDtStart(aStartTime);
        if( parameters->iRecurrentFrequency == TCalRRule::EWeekly )
        	{
        	TDay tempDay(aStartTime.TimeLocalL().DayNoInWeek());
        	days.AppendL(tempDay);        
        	calRepeat.SetByDay(days);  
        	}
        else if( parameters->iRecurrentFrequency == TCalRRule::EMonthly )
        	{
        	RArray< TInt > daysInMonth;
        	CleanupClosePushL(daysInMonth);
        	daysInMonth.AppendL(aStartTime.TimeLocalL().DayNoInMonth());
        	calRepeat.SetByMonthDay(daysInMonth);
        	CleanupStack::PopAndDestroy(&daysInMonth);
        	}
        else if( parameters->iRecurrentFrequency == TCalRRule::EYearly)
        	{
        	RArray< TMonth > monthsInYear;
        	CleanupClosePushL(monthsInYear);
        	monthsInYear.AppendL(aStartTime.TimeLocalL().DateTime().Month());
        	calRepeat.SetByMonth(monthsInYear);
        	CleanupStack::PopAndDestroy(&monthsInYear);
        	}
        
        // Recurrent interval
        if( parameters->iRecurrentInterval > 0 )
        	calRepeat.SetInterval(parameters->iRecurrentInterval);
                
        // Recurrent end. End can be given as a number of repeating entries or as an end date.
        // End date must be later than the start date, otherwise CCalEntry::SetRRuleL() panics.
        if( parameters->iRepeatingCount > 0 )
            {
            calRepeat.SetCount(parameters->iRepeatingCount);
            aCalEntry.SetRRuleL(calRepeat);            
            }
        else
            {
            TCalTime endTime;
            endTime.SetTimeLocalL(parameters->iRecurrentEndTime);
            if( endTime.TimeLocalL() > aStartTime.TimeLocalL())
            	{
            	calRepeat.SetUntil(endTime);
            	aCalEntry.SetRRuleL(calRepeat);
            	}
            }           
        }
    CleanupStack::PopAndDestroy(&days);
    }

void CCreatorInterimCalendar::SetAlarmL(CCalEntry& aCalEntry, CCalenderInterimParameters* params)
    {
    if( params->iAlarmTime != Time::NullTTime() )
        {
        CCalAlarm* alarm = CCalAlarm::NewL();
        CleanupStack::PushL(alarm);
        //if( params->iAlarmTime <= params->iStartTime )
        //	{
        	TTimeIntervalMinutes diff;
        	TInt err = params->iStartTime.MinutesFrom(params->iAlarmTime, diff);
        	if( err == KErrNone)
        		{
        		alarm->SetTimeOffset(diff);
        		TRAP(err, aCalEntry.SetAlarmL(alarm));
        		if( err != KErrNone && err !=  KErrNotSupported )
        			{
        			User::Leave(err);
        			}        		
        		}
        //	}
        CleanupStack::PopAndDestroy(alarm);
        }
    }

void CCreatorInterimCalendar::SetRandomAlarmL(CCalEntry& aCalEntry)
    {
    CCalAlarm* alarm = CCalAlarm::NewL();
    CleanupStack::PushL(alarm);
    alarm->SetTimeOffset(TTimeIntervalMinutes(iEngine->RandomNumber(0, 60)));
    
    TRAPD(err, aCalEntry.SetAlarmL(alarm));
    if( err != KErrNone && err !=  KErrNotSupported )
    	{
    	User::Leave(err);
    	}
    CleanupStack::PopAndDestroy(alarm);
    }

void CCreatorInterimCalendar::SetPhoneOwnerL(CCalEntry& aCalEntry, const TDesC& aCN, const TDesC& aEmail, const TDesC& aSentBy )
    {
    CCalUser* phOwner = CCalUser::NewL(aEmail, aSentBy);
    CleanupStack::PushL(phOwner);
    TRAPD(err, aCalEntry.SetPhoneOwnerL(phOwner));            
    CleanupStack::Pop(phOwner);
    if( err == KErrNone )
        {
        phOwner->SetCommonNameL(aCN);
        }
    else
        {
        delete phOwner;
        }
    }

void CCreatorInterimCalendar::SetOrganizerL(CCalEntry& aCalEntry, const TDesC& aCN, const TDesC& aEmail, const TDesC& aSentBy)
    {
    CCalUser* organizer = CCalUser::NewL(aEmail, aSentBy);
    CleanupStack::PushL(organizer);
    TRAPD(err, aCalEntry.SetOrganizerL(organizer));
    CleanupStack::Pop(organizer);
    if( err == KErrNone )
        {
        organizer->SetCommonNameL(aCN);
        }
    else
        {
        delete organizer;
        }
    }

void CCreatorInterimCalendar::AddAttendeesL(CCalEntry& aCalEntry, CCalenderInterimParameters* parameters )
    {
    if( !parameters )
        return;
            
    // Random attendees:
    TInt numOfAttendees = parameters->iNumberOfAttendees;
    for( TInt i = 0; i < numOfAttendees; ++i )
        {
        TPtrC firstname  = iEngine->RandomString(CCreatorEngine::EFirstName);
        TPtrC lastname   = iEngine->RandomString(CCreatorEngine::ESurname);
        TPtrC company    = iEngine->RandomString(CCreatorEngine::ECompany);
        _LIT(KCountry, "com");        
        TDesC* emailAddr = iEngine->CreateEmailAddressLC(firstname, lastname, company, KCountry);        
        CCalAttendee* attendee = CCalAttendee::NewL(*emailAddr);        
        CleanupStack::PushL(attendee);
        aCalEntry.AddAttendeeL(attendee);
        CleanupStack::Pop(attendee);
        attendee->SetCommonNameL(firstname);
                
        // Set phone owner:
        if( i == 1 )
            {             
            SetPhoneOwnerL(aCalEntry, firstname, *emailAddr, *emailAddr); 
            attendee->SetRoleL(CCalAttendee::EReqParticipant); 
            }
        // Set organizer:
        else if ( i == 0 )
            {            
            // Organizer is the chairman of the meeting:
            SetOrganizerL(aCalEntry, firstname, *emailAddr, *emailAddr);
            attendee->SetRoleL(CCalAttendee::EChair);
            }
        else if( i % 3 == 0 )
            {
            // every third attendee is optional:
            attendee->SetRoleL(CCalAttendee::EOptParticipant);
            }
        else
            {
            // Others are required attendees:
            attendee->SetRoleL(CCalAttendee::EReqParticipant);            
            }
        
        TInt attStatus = iEngine->RandomNumber(0,5);
        attendee->SetStatusL((CCalAttendee::TCalStatus) attStatus);
        
        CleanupStack::PopAndDestroy(emailAddr);
        }
    
    // Explicit attendees:
    for( TInt i = 0; i < parameters->iAttendees.Count(); ++i )
        {
        CCreatorCalenderAttendee* attendeeInfo = parameters->iAttendees[i];
        if( attendeeInfo && attendeeInfo->EmailAddress().Length() > 0)
        	{
        	CCalAttendee* attendee = CCalAttendee::NewL(attendeeInfo->EmailAddress());
        	CleanupStack::PushL(attendee);
        	if( attendeeInfo->CommonName().Length() > 0 )
        		attendee->SetCommonNameL(attendeeInfo->CommonName());
        	attendee->SetRoleL(attendeeInfo->Role());
        	attendee->SetStatusL(attendeeInfo->Status());
        	attendee->SetResponseRequested(ETrue);
        	aCalEntry.AddAttendeeL(attendee);
        	CleanupStack::Pop(); // attendee
        	}
        }
    // linked attendees:
     for( TInt i = 0; i < parameters->iAttendeeLinkIds.Count(); ++i )
        {
        CCreatorPhonebook* phonebook = dynamic_cast<CCreatorPhonebook*>(iEngine->GetPhonebook());
        User::LeaveIfNull( phonebook );
        CCreatorPhonebookWrapper* phonebookWrapper = phonebook->GetPhonebookWrapper();
        
        TLinkIdParam attendeeLinkId = parameters->iAttendeeLinkIds[i];
        const CCreatorContactSet& set = ContactLinkCache::Instance()->ContactSet(attendeeLinkId.iLinkId);
        const RArray<TUint32> links = set.ContactLinks();
        TInt numberOfExplicitLinks = links.Count(); // Number of defined contacts in contact-set
        for( TInt j = 0; j < numberOfExplicitLinks; ++j )
            {
            TBuf<128> email;
            TBuf<128> name;
            TBuf<128> phoneNumber;
            phonebookWrapper->GetContactDetailsL( links[j], name, phoneNumber, email );
            CCalAttendee* attendee = CCalAttendee::NewL( email );
            CleanupStack::PushL(attendee);
            if( name.Length() > 0 )
                attendee->SetCommonNameL( name );
            attendee->SetResponseRequested(ETrue);
            aCalEntry.AddAttendeeL(attendee);
            CleanupStack::Pop(); // attendee
            }
        }
    }
//----------------------------------------------------------------------------

TInt CCreatorInterimCalendar::CreateEventEntryL (
        CCreatorModuleBaseParameters *aParameters )
    {
    LOGSTRING("Creator: CCreatorInterimCalendar::CreateEventEntryL");

    delete iParameters;
    iParameters = 0;
        
    CCalenderInterimParameters* parameters = (CCalenderInterimParameters*)aParameters;       
    
    // random data needed if no predefined data available
    if (!parameters )
        {
        iParameters = new(ELeave) CCalenderInterimParameters;

        iParameters->iRandomModeStartTime = iRandomModeStartTime;
        iParameters->iRandomModeEndTime = iRandomModeEndTime;
        iParameters->iNumberRepeatingEntries = iNumberRepeatingEntries;
        iParameters->iNumberOfAttendees = iNumberOfAttendees;
        
        // difference between start and end times
        TTimeIntervalMinutes differenceInMinutes;
        iParameters->iRandomModeEndTime.MinutesFrom (
                iParameters->iRandomModeStartTime, differenceInMinutes );

        // start time (= something between the period start and end date)
        iParameters->iStartTime = iParameters->iRandomModeStartTime;
        iParameters->iStartTime
                += TTimeIntervalMinutes (iEngine->RandomNumber (differenceInMinutes.Int ( ) ) );

        // set end time (= start time + some random time)
        iParameters->iEndTime = iParameters->iStartTime;
        iParameters->iEndTime
                += TTimeIntervalMinutes (iEngine->RandomNumber (5, 500 ) );        

        // strings
        iParameters->iDescription->Des()
                = iEngine->RandomString (CCreatorEngine::EMemoText );
        
        iParameters->iSummary->Des() = iParameters->iDescription->Des();
        
        // Random event status:
        iParameters->iStatus = 
            (CCalEntry::TStatus) iEngine->RandomNumber(0, (TInt) CCalEntry::ECancelled);
        
        parameters = iParameters;
        }

    TInt err = KErrNone;
    HBufC8* guid = HBufC8::NewLC (255 );
    TPtr8 uidP=guid->Des ( );
    GenerateRandomId (uidP );
    CCalEntry* entry = CCalEntry::NewL (CCalEntry::EEvent, guid,
            CCalEntry::EMethodAdd, 0 );
    CleanupStack::PushL (entry );
    // iEntryUids->AppendL( *guid->Alloc() );
    
    entry->SetDescriptionL (parameters->iDescription->Des ( ) );
    entry->SetSummaryL (parameters->iSummary->Des ( ) );

    TCalTime startTime;
    TCalTime endTime;
    // Set the start and end times using time values local to the current system time zone
    startTime.SetTimeLocalL (parameters->iStartTime );
    endTime.SetTimeLocalL (parameters->iEndTime );
    entry->SetStartAndEndTimeL (startTime, endTime );

    // Repeating times:
    AddRepeatingRuleL(startTime, *entry, parameters);
    
    if( iSetAlarm || parameters->iUseRandomAlarmTime )
        {        
        SetRandomAlarmL(*entry);
        }
    else
        {
        SetAlarmL(*entry, parameters);
        }
    
    // Add attendees:
    AddAttendeesL(*entry, parameters);    
    
    if( parameters->IsAllowedEntryStatus(CCalEntry::EEvent, parameters->iStatus))
        {
        LOGSTRING2("Creator: CCreatorInterimCalendar::CreateEventEntryL setting entry status: %d", parameters->iStatus);
        entry->SetStatusL(parameters->iStatus);
        }
        
    RPointerArray<CCalEntry> array;
    CleanupClosePushL (array );
    array.AppendL (entry );

    TInt success(0);
    //If StoreL() leaves, 'success' contains the number of entries that were stored before it failed
    iCalEntryView->StoreL (array, success );

    // id has been generated, store it for being able to delete
    // entries created only with Creator
    iEntryIds.AppendL( entry->LocalUidL() );
    
    //One item should be added at time
    if (success != 1 )
        {
        err = 1;
        }

    CleanupStack::PopAndDestroy (&array );
    CleanupStack::PopAndDestroy (entry );
    CleanupStack::Pop (guid );//guid has been destroyed by entry, which has ownership to guid
    return err;

    }

//----------------------------------------------------------------------------


TInt CCreatorInterimCalendar::CreateReminderEntryL (
        CCreatorModuleBaseParameters *aParameters )
    {
    LOGSTRING("Creator: CCreatorInterimCalendar::CreateReminderEntryL");

    delete iParameters;
    iParameters = NULL;
    
    CCalenderInterimParameters* parameters = (CCalenderInterimParameters*)aParameters;

    // random data needed if no predefined data available
    if (!parameters )
        {
        iParameters = new(ELeave) CCalenderInterimParameters;

        iParameters->iRandomModeStartTime = iRandomModeStartTime;
        iParameters->iRandomModeEndTime = iRandomModeEndTime;

        // difference between start and end times
        TTimeIntervalDays
                differenceInDays = iParameters->iRandomModeEndTime.DaysFrom (iParameters->iRandomModeStartTime );

        // due time (= something between the period start and end date)
        iParameters->iStartTime = iParameters->iRandomModeStartTime;
        iParameters->iStartTime
                += TTimeIntervalDays (iEngine->RandomNumber (differenceInDays.Int ( ) ) );

        // strings
        iParameters->iDescription->Des()
                = iEngine->RandomString (CCreatorEngine::EReminderText );
        
        iParameters->iSummary->Des() = iParameters->iDescription->Des();

        parameters = iParameters;
        }

    TInt err = KErrNone;

    HBufC8* guid = HBufC8::NewLC (255 );
    TPtr8 uidP=guid->Des ( );
    GenerateRandomId (uidP );

    CCalEntry* entry = CCalEntry::NewL (CCalEntry::EReminder, guid,
            CCalEntry::EMethodAdd, 0 );
    CleanupStack::PushL (entry );

    //Using same text as description and summary    
    entry->SetDescriptionL (parameters->iDescription->Des ( ) );
    entry->SetSummaryL (parameters->iSummary->Des ( ) );

    TCalTime startTime;
    TCalTime endTime;
    // Set the start and end times using time values local to the current system time zone
    startTime.SetTimeLocalL (parameters->iStartTime );
    TTime nullTime = Time::NullTTime ( );
    endTime.SetTimeLocalL (nullTime );
    entry->SetStartAndEndTimeL (startTime, endTime );
        
    if( parameters->IsAllowedEntryStatus(CCalEntry::EReminder, parameters->iStatus))
        {
        LOGSTRING2("Creator: CCreatorInterimCalendar::CreateReminderEntryL setting entry status: %d", parameters->iStatus);
        entry->SetStatusL(parameters->iStatus);
        }
    
    if( iSetAlarm || parameters->iUseRandomAlarmTime )
        {        
        SetRandomAlarmL(*entry);
        }
    else
        {
        SetAlarmL(*entry, parameters);
        }

    RPointerArray<CCalEntry> array;
    CleanupClosePushL (array );
    array.AppendL (entry );

    TInt success(0);
    //If StoreL() leaves, 'success' contains the number of entries that were stored before it failed
    iCalEntryView->StoreL (array, success );

    // id has been generated, store it for being able to delete
    // entries created only with Creator
    iEntryIds.AppendL( entry->LocalUidL() );
    
    //One item should be added at time
    if (success != 1 )
        {
        err = 1;
        }

    CleanupStack::PopAndDestroy (&array );
    CleanupStack::PopAndDestroy (entry );
    CleanupStack::Pop (guid );//guid has been destroyed by entry, which has ownership to guid

    return err;
    }

//----------------------------------------------------------------------------

TInt CCreatorInterimCalendar::CreateAnniversaryEntryL (
        CCreatorModuleBaseParameters *aParameters )
    {
    LOGSTRING("Creator: CCreatorInterimCalendar::CreateAnniversaryEntryL");

    delete iParameters;
    iParameters = NULL;
    
    CCalenderInterimParameters* parameters = (CCalenderInterimParameters*)aParameters;

    // random data needed if no predefined data available
    if (!parameters )
        {
        iParameters = new(ELeave) CCalenderInterimParameters;

        iParameters->iRandomModeStartTime = iRandomModeStartTime;
        iParameters->iRandomModeEndTime = iRandomModeEndTime;

        // difference between start and end times
        TTimeIntervalDays differenceInDays = iParameters->iRandomModeEndTime.DaysFrom(iParameters->iRandomModeStartTime);  

        // start time (= something between the period start and end date)
        iParameters->iStartTime = iParameters->iRandomModeStartTime;
        iParameters->iStartTime += TTimeIntervalDays( iEngine->RandomNumber(differenceInDays.Int()) );        
        
        // strings
        iParameters->iDescription->Des().Copy (iEngine->RandomString (CCreatorEngine::EFirstName ) );
        iParameters->iDescription->Des().Append ( _L("'s ") );
        iParameters->iDescription->Des().Append (iEngine->RandomString (CCreatorEngine::EAnniversaryReason ) );
        iParameters->iSummary->Des() = iParameters->iDescription->Des();
        parameters = iParameters;
        }
 
    TInt err = KErrNone;

    HBufC8* guid = HBufC8::NewLC (255 );
    TPtr8 uidP=guid->Des ( );
    GenerateRandomId (uidP );

    CCalEntry* entry = CCalEntry::NewL (CCalEntry::EAnniv, guid,
            CCalEntry::EMethodNone, 0 );
    CleanupStack::PushL (entry );

    //Using same text as description and summary	
    entry->SetDescriptionL (parameters->iDescription->Des ( ) );
    entry->SetSummaryL (parameters->iSummary->Des ( ) );

    TCalTime startTime;
    TCalTime endTime;
    TTime nullTime = Time::NullTTime ( );
    endTime.SetTimeLocalL (nullTime );
    
    // Set the start time using time value local to the current system time zone
    startTime.SetTimeLocalL (parameters->iStartTime );

    entry->SetStartAndEndTimeL (startTime, endTime );

    if( iSetAlarm || parameters->iUseRandomAlarmTime )
        {        
        SetRandomAlarmL(*entry);
        }
    else
        {
        SetAlarmL(*entry, parameters);
        }
    
    //set repeating rule
    TCalRRule yearlyRptRule(TCalRRule::EYearly);
    yearlyRptRule.SetDtStart (startTime );
    //Set repeating until forever, not setting it to stop
    //yearlyRptRule.SetUntil(endTime);
    entry->SetRRuleL (yearlyRptRule );

    RPointerArray<CCalEntry> array;
    CleanupClosePushL (array );
    array.AppendL (entry );

    TInt success(0);
    //If StoreL() leaves, 'success' contains the number of entries that were stored before it failed
    iCalEntryView->StoreL (array, success );

    // id has been generated, store it for being able to delete
    // entries created only with Creator
    iEntryIds.AppendL( entry->LocalUidL() );
    
    //One item should be added at time
    if (success != 1 )
        {
        err = 1;
        }

    CleanupStack::PopAndDestroy (&array );
    CleanupStack::PopAndDestroy (entry );
    CleanupStack::Pop (guid );//guid has been destroyed by entry, which has ownership to guid

    return err;
    }

//----------------------------------------------------------------------------

TInt CCreatorInterimCalendar::CreateTodoEntryL (
        CCreatorModuleBaseParameters *aParameters )
    {
    LOGSTRING("Creator: CCreatorInterimCalendar::CreateTodoEntryL");

    delete iParameters;
    iParameters = NULL;
    
    CCalenderInterimParameters* parameters = (CCalenderInterimParameters*)aParameters;

    // random data needed if no predefined data available
    if (!parameters )
        {
        iParameters = new(ELeave) CCalenderInterimParameters;

        iParameters->iRandomModeStartTime = iRandomModeStartTime;
        iParameters->iRandomModeEndTime = iRandomModeEndTime;

        // difference between start and end times
        TTimeIntervalDays
                differenceInDays = iParameters->iRandomModeEndTime.DaysFrom (iParameters->iRandomModeStartTime );

        // due time (= something between the period start and end date)
        iParameters->iTodoDueTime = iParameters->iRandomModeStartTime;
        iParameters->iTodoDueTime
                += TTimeIntervalDays (iEngine->RandomNumber (differenceInDays.Int ( ) ) );

        // strings
        iParameters->iDescription->Des ( )
                = iEngine->RandomString (CCreatorEngine::EToDoText );
        
        iParameters->iSummary->Des() = iParameters->iDescription->Des();

        //priority
        iParameters->iTodoPriority = iEngine->RandomNumber (1, 3 );
        iParameters->iStatus = (CCalEntry::TStatus) iEngine->RandomNumber((TInt) CCalEntry::ETodoNeedsAction, (TInt) CCalEntry::ETodoInProcess);
        parameters = iParameters;
        
        }

    TInt err = KErrNone;

    HBufC8* guid = HBufC8::NewLC (255 );
    TPtr8 uidP=guid->Des ( );
    GenerateRandomId (uidP );

    CCalEntry* entry = CCalEntry::NewL (CCalEntry::ETodo, guid,
            CCalEntry::EMethodAdd, 0 );
    CleanupStack::PushL (entry );

    //Using same text as description and summary	
    entry->SetDescriptionL (parameters->iDescription->Des ( ) );
    entry->SetSummaryL (parameters->iSummary->Des ( ) );

    entry->SetPriorityL (parameters->iTodoPriority );
    
    if( parameters->IsAllowedEntryStatus(CCalEntry::ETodo, parameters->iStatus))
        {
        LOGSTRING2("Creator: CCreatorInterimCalendar::CreateTodoEntryL setting entry status: %d", parameters->iStatus);
        entry->SetStatusL(parameters->iStatus);
        }

    TCalTime startTime;
    TCalTime endTime;
    // Set the start and end times using time values local to the current system time zone
    startTime.SetTimeLocalL (parameters->iTodoDueTime );
    endTime.SetTimeLocalL (parameters->iTodoDueTime );
    entry->SetStartAndEndTimeL (startTime, endTime );

    if( iSetAlarm || parameters->iUseRandomAlarmTime )
        {        
        SetRandomAlarmL(*entry);
        }
    else
        {
        SetAlarmL(*entry, parameters);
        }
    
    RPointerArray<CCalEntry> array;
    CleanupClosePushL (array );
    array.AppendL (entry );

    TInt success(0);
    //If StoreL() leaves, 'success' contains the number of entries that were stored before it failed
    iCalEntryView->StoreL (array, success );

    // id has been generated, store it for being able to delete
    // entries created only with Creator
    iEntryIds.AppendL( entry->LocalUidL() );
    
    //One item should be added at time
    if (success != 1 )
        {
        err = 1;
        }

    CleanupStack::PopAndDestroy (&array );
    CleanupStack::PopAndDestroy (entry );
    CleanupStack::Pop (guid );//guid has been destroyed by entry, which has ownership to guid

    return err;
    }

//----------------------------------------------------------------------------

void CCreatorInterimCalendar::Progress (TInt /*aPercentageCompleted*/)
    {
    //Not really needed but observer need to be implement
    }

void CCreatorInterimCalendar::Completed (TInt aError )
    {
    //Not really needed but observer need to be implement
    LOGSTRING2("Creator: CCreatorInterimCalendar::Completed err=%d", aError);
    }

TBool CCreatorInterimCalendar::NotifyProgress ( )
    {
    //Not really needed but observer need to be implement
    return NULL;
    }

//----------------------------------------------------------------------------


void CCreatorInterimCalendar::GenerateRandomId (TDes8& aText )
    {
    LOGSTRING("Creator: CCreatorInterimCalendar::GenerateRandomId");

    aText.Zero ( );

    TTime now;
    now.UniversalTime ( );
    TInt64 time = now.Int64 ( );

    // The number of characters in the guid should be
    // more than 8 (to ensure the guids are not too small)
    // less than 255 (this is the maximum length of a guid)
    // less than MaxLength of the descriptor passed in		
    TInt numChar(0);
    do
        {
        numChar = Math::Rand (time ) % (aText.MaxLength ( ) - 1);
        }
    while (numChar > 255|| numChar < 8 );

    for (TInt i(0); i < numChar ; ++i )
        {
        TInt character = Math::Rand (time )%6;
        character = Math::Rand (time ) % 26+ ((character == 2) ? 'A' : 'a');
        aText.Append (character );
        }

    }

//----------------------------------------------------------------------------
void CCreatorInterimCalendar::DeleteAllL()
    {
    LOGSTRING("Creator: CCreatorInterimCalendar::DeleteAllL");
    TCalTime zeroTime;
    zeroTime.SetTimeUtcL( TCalTime::MinTime() );
    RArray<TUint32> allIds;
    CleanupClosePushL( allIds );
    iCalEntryView->GetIdsModifiedSinceDateL( zeroTime, allIds );
    if ( allIds.Count() )
        {
        TInt successCount( KErrNotFound );
        iCalEntryView->DeleteL( allIds, successCount );
        if ( successCount != allIds.Count() )
            {
            User::Leave( KErrGeneral );
            }        
        }
    CleanupStack::PopAndDestroy( &allIds );

    // reset must be done here, because iEntryIds is stored in destructor
    iEntryIds.Reset();
    
    // all entries deleted, remove the Calendar related registry
    iEngine->RemoveStoreL( KUidDictionaryUidCalendar );
    }

//----------------------------------------------------------------------------
void CCreatorInterimCalendar::DeleteAllCreatedByCreatorL()
    {
    LOGSTRING("Creator: CCreatorInterimCalendar::DeleteAllCreatedByCreatorL");
    iEntryIds.Reset();
    
    // fetch ids of entries created by Creator
    iEngine->ReadEntryIdsFromStoreL( iEntryIds, KUidDictionaryUidCalendar );
    
    // delete entries
    TInt successCount(0); // ignore
    TRAP_IGNORE( iCalEntryView->DeleteL( iEntryIds, successCount ) );

    // reset must be done here, because iEntryIds is stored in destructor
    iEntryIds.Reset();
    
    // all entries deleted, remove the Calendar related registry
    iEngine->RemoveStoreL( KUidDictionaryUidCalendar );
    }

//----------------------------------------------------------------------------

