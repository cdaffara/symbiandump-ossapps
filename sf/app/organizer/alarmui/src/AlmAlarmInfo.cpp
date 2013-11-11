/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This class provides the text for an alarm notification.
*
*/



// INCLUDE FILES
#include "pim_trace.h"
#include "AlmAlarmInfo.h"

#include <asshdalarm.h>
#include <calentry.h>
#include <StringLoader.h>
#include <AknUtils.h>
#include <akndialogcontroller.h>

#include <AlmAlert.rsg>

const TInt KAlarmNoteTextLength( 256 ); // KGlobalNoteTextLength
const TInt KDateTimeDescLen( 20 ); // enough space for // "16:54" or "1:56 am" or "10.10.2010"
// const TInt KClockAlarmType( -1 );
// _LIT( KWesternSubjectSeparator, ", " );
// _LIT( KArabicSubjectSeparator, " \x060c" );
// _LIT( KNewLine, "\n" );
// _LIT( KSpace, " " );


// ============================ MEMBER FUNCTIONS ===============================

// constructor
CAlmAlarmInfo::CAlmAlarmInfo(CAlarmUtils* aAlarmUtils)
: iAlarmUtils( aAlarmUtils )
{
    TRACE_ENTRY_POINT;
    
    iAlarmInfo = new SAlarmInfo;
    
    TRACE_EXIT_POINT;
}

// destructor
CAlmAlarmInfo::~CAlmAlarmInfo(void)
{
    TRACE_ENTRY_POINT;
    delete iSubject;
    delete iLocation;
    delete iDate;
    delete iTime;
    
    if( iAlarmInfo )
        {
        delete iAlarmInfo;
        iAlarmInfo = NULL;
        }
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Get the text for the alarm query.
// -----------------------------------------------------------------------------
//
void CAlmAlarmInfo::GetAlarmLabelL( const TASShdAlarm& /*aAlarm*/, 
                                    HBufC*& /*aText*/, 
                                    const CAlarmUtils::TAlarmType& /*aAlarmType*/ )
{
    /**
     *  Clock alarm:                  Other/unknown alarms:
     *  ---------------------------   --------------------------- 
     *  | Clock alarm:     :::::: |   | 01.01.2010       :::::: | 
     *  | 12:00            :icon: |   | 12:00            :icon: | 
     *  | DescriptionDesc  :::::: |   | DescriptionDesc  :::::: | 
     *  | riptionDescriptionDescr |   | riptionDescriptionDescr | 
     *  | iptionDescriptionDes... |   | iptionDescriptionDes... | 
     *  ---------------------------   --------------------------- 
     *
     *  Meeting or reminder alarm:   Event or anniversary alarm:   To-Do alarm:
     *  ---------------------------  ---------------------------   --------------------------- 
     *  | 01.01.2010       :::::: |  | 01.01.2010       :::::: |   | To-Do Task:      :::::: | 
     *  | 12:00            :icon: |  | SubjectSubjectS  :icon: |   | Due: 01.01.2010  :icon: | 
     *  | SubjectSubjectS  :::::: |  | ubjectSubjectSu  :::::: |   | SubjectSubjectS  :::::: | 
     *  | ubject, LocationLocatio |  | ubject, LocationLocatio |   | ubject, LocationLocatio | 
     *  | nLocationLocationLoc... |  | nLocationLocationLoc... |   | nLocationLocationLoc... | 
     *  ---------------------------  ---------------------------   --------------------------- 
     *
     *  Cover UI parameters:
     *  ====================
     *   Clock:       time: "12:00", date: "",           subject: "Description", location: ""
     *   Unknown:     time: "12:00", date: "01.01.2010", subject: "Description", location: ""
     *
     *   Meeting:     time: "12:00", date: "01.01.2010", subject: "Subject",     location: "Location"
     *   Reminder:    time: "12:00", date: "01.01.2010", subject: "Subject",     location: "Location"
     *
     *   Anniversary: time: "",      date: "01.01.2010", subject: "Subject",     location: "Location"
     *   Event:       time: "",      date: "01.01.2010", subject: "Subject",     location: "Location"
     *   To-Do:       time: "",      date: "01.01.2010", subject: "Subject",     location: "Location"
     */
    TRACE_ENTRY_POINT;

    // cover UI params...
    // TInt coverUIAlarmType( KClockAlarmType );
/*
    switch( aAlarmType )
    {
        case CAlarmUtils::EAlarmTypeClock:
        {
            GetCoverUIParamsForClockAndOtherL( aAlarm, CAlarmUtils::EAlarmTypeClock );
            SetupClockAlarmL( aText );
        }
        break;

        case CAlarmUtils::EAlarmTypeOther:
        {
            // For Cover UI - unknown alarms are handled as calendar meetings
            // coverUIAlarmType = CCalEntry::EAppt;

            GetCoverUIParamsForClockAndOtherL( aAlarm, CAlarmUtils::EAlarmTypeOther );
            SetupUnknownAlarmL( aText );
        }
        break;

        case CAlarmUtils::EAlarmTypeCalendar:
        {
            // check phone restore mode before creating agenda session
            if( iAlarmUtils->IsPhoneInRestoreMode() )
            {
                User::Leave( KErrCancel );  // trapped in CAlmAlarmControl
            }

            // fetch the calendar entry and instance time
            //
            CCalEntry* entry = NULL;
            iAlarmUtils->FetchEntryL( entry );
            CleanupStack::PushL( entry );
            CleanupStack::PushL( aText );

            // entry type
            CCalEntry::TType type = entry->EntryTypeL();
            // coverUIAlarmType = type;

            GetCoverUIParamsForCalendarL( entry );

            switch( type )
            {
                case CCalEntry::EAppt: // Appointment
                case CCalEntry::EReminder: // Reminder
                {
                    SetupAppointmentAlarmL( aText );
                }
                break;

                case CCalEntry::ETodo: // To-Do
                {
                    SetupToDoAlarmL( aText );
                }
                break;

                //case CCalEntry::EAnniv: // Anniversary
                //case CCalEntry::EEvent: // Day note / All day event
                default:
                {
                    SetupAnniversaryAlarmL( aText );
                }
                break;
            }

            CleanupStack::Pop(); // aText
            CleanupStack::PopAndDestroy( entry );
        }
        break;

        default:
            __ASSERT_DEBUG( EFalse, User::Invariant() );
    }

    // Set information for cover UI...
    //
    iAlarmUtils->NotifierDialogController()->AddAlarmAdditionalInfo( coverUIAlarmType, 
                                                                     *iCoverTime, 
                                                                     *iCoverDate, 
                                                                     *iSubject, 
                                                                     *iLocation );
    delete iSubject;   iSubject = NULL;
    delete iLocation;  iLocation = NULL;
    delete iDate;      iDate = NULL;
    delete iTime;      iTime = NULL;
    delete iCoverDate; iCoverDate = NULL;
    delete iCoverTime; iCoverTime = NULL;
*/
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Get the text for the wake-up query.
// -----------------------------------------------------------------------------
//
void CAlmAlarmInfo::GetWakeupLabelL(HBufC*& aLabel)
{
    TRACE_ENTRY_POINT;
    aLabel = StringLoader::LoadL( R_EIKALARM_CONFIRMATION );
    TRACE_EXIT_POINT;
}

SAlarmInfo* CAlmAlarmInfo::GetAlarmInfo(TASShdAlarm aAlarm, CAlarmUtils::TAlarmType aAlarmType)
{
    // Based on the alarm type, get the appropriate information
    switch( aAlarmType )
        {
        case CAlarmUtils::EAlarmTypeClock:
            {
            SetupClockAlarmL( aAlarm );
            // Set the alarm type
			iAlarmInfo->iAlarmAlertType = CAlarmUtils::EAlarmTypeClock;
            }
        break;

        case CAlarmUtils::EAlarmTypeOther:
            {
            // TODO: Handle this case 
            SetupUnknownAlarmL( aAlarm );
            iAlarmInfo->iAlarmAlertType = CAlarmUtils::EAlarmTypeOther;
            }
        break;

        case CAlarmUtils::EAlarmTypeCalendar:
            {
            // Check phone restore mode before creating agenda session
            if( iAlarmUtils->IsPhoneInRestoreMode() )
                {
                User::Leave( KErrCancel );
                }
            // Set the alarm type
            iAlarmInfo->iAlarmAlertType = CAlarmUtils::EAlarmTypeCalendar;
            
            // Fetch the calendar entry and instance time
            CCalEntry* entry = NULL;
            
            // TODO: Use agenda interface to fetch the entry details
            iAlarmUtils->FetchEntryL( entry );
            CleanupStack::PushL( entry );
            
            const TTime instanceTime( iAlarmUtils->AlarmData().iInstanceTime.TimeLocalL() );

            // Subject
            iAlarmInfo->iSubject = HBufC::NewL( KAlarmNoteTextLength - KDateTimeDescLen );
            iAlarmInfo->iSubject->Des().Append( entry->SummaryL().Left( iAlarmInfo->iSubject->Des().MaxLength() ) );
            iAlarmInfo->iSubject->Des().TrimAll();

            // Location
            iAlarmInfo->iLocation = HBufC::NewL( Max( 0, KAlarmNoteTextLength - KDateTimeDescLen - iAlarmInfo->iSubject->Length() ) );
            iAlarmInfo->iLocation->Des().Append( entry->LocationL().Left( iAlarmInfo->iLocation->Des().MaxLength() ) );
            iAlarmInfo->iLocation->Des().TrimAll();

            // Date
            iAlarmInfo->iDate = instanceTime;

            // Get the entry type
            CCalEntry::TType type = entry->EntryTypeL();

            // Based on the type of entry, get the information
            switch( type )
                {
                case CCalEntry::EAppt:
                case CCalEntry::EReminder:
                    {
                    // TODO : Format text based on type of entry
                    // SetupAppointmentAlarmL( aAlarm );
                    iAlarmInfo->iTime = instanceTime;
                    iAlarmInfo->iIsTimed = ETrue;
                    }
                break;

                case CCalEntry::ETodo:
                    {
                    // TODO : Format text based on type of entry
                    //SetupToDoAlarmL( aAlarm );
                    iAlarmInfo->iAlarmAlertType = CAlarmUtils::EAlarmTypeTodo;
                    iAlarmInfo->iTime = TTime( 0 );
                    iAlarmInfo->iIsTimed = EFalse;
                    // No Location is there for todo alarms
                    iAlarmInfo->iLocation = HBufC::NewL( 0 );
                    }
                break;
                
                case CCalEntry::EEvent:
                    {
                    // No time information for an all day event	
                    iAlarmInfo->iTime = TTime( 0 );
                    iAlarmInfo->iIsTimed = EFalse;
                    }
                break;
                
                default:
                    {
                    // TODO : Format text based on type of entry
                    // SetupAnniversaryAlarmL( aAlarm );
                    iAlarmInfo->iTime = TTime( 0 );
                    }
                break;
                }
            CleanupStack::PopAndDestroy( entry );
            }
        break;

        default:
            __ASSERT_DEBUG( EFalse, User::Invariant() );
    }
    return iAlarmInfo;
}

// =============================================================================
// ALARM SETUP METHODS
// =============================================================================

/*
// -----------------------------------------------------------------------------
// Create alarm notification string for Clock alarm.
// "[Clock alarm:][\n][time][\n][Description]"
// -----------------------------------------------------------------------------
//
void CAlmAlarmInfo::SetupClockAlarmL(HBufC*& aText)
{
    TRACE_ENTRY_POINT;
    // default text for clock alarm
    HBufC* clockText = StringLoader::LoadLC( R_EIKALARM_CLOCK_ALARM_TYPE );

    aText = HBufC::NewLC(  clockText->Length()
                         + KNewLine().Length()
                         + iTime->Length()
                         + KNewLine().Length()
                         + iSubject->Length() );

    // add default clock alarm text
    *aText = *clockText;
    TPtr ptr = aText->Des();
    ptr.Append( KNewLine );

    // add time
    ptr.Append( *iTime );

    // add description
    if( iSubject->Length() )
    {
        ptr.Append( KNewLine );
        ptr.Append( *iSubject );
    }

    CleanupStack::Pop( aText );
    CleanupStack::Pop( clockText );
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Create alarm notification string for an unknown alarm type.
// "[date][\n][time][\n][Description]"
// -----------------------------------------------------------------------------
//
void CAlmAlarmInfo::SetupUnknownAlarmL(HBufC*& aText)
{
    TRACE_ENTRY_POINT;
    aText = HBufC::NewLC(  iDate->Length()
                         + KNewLine().Length()
                         + iTime->Length()
                         + KNewLine().Length()
                         + iSubject->Length() );
    // add date
    *aText = *iDate;
    TPtr ptr = aText->Des();
    ptr.Append( KNewLine );

    // add time
    ptr.Append( *iTime );

    // add description
    if( iSubject->Length() )
    {
        ptr.Append( KNewLine );
        ptr.Append( *iSubject );
    }

    CleanupStack::Pop( aText );
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Create alarm notification string for a calendar appointment alarm.
// "[date][\n][time][\n][Subject][, ][Location]"
// -----------------------------------------------------------------------------
//
void CAlmAlarmInfo::SetupAppointmentAlarmL(HBufC*& aText)
{
    TRACE_ENTRY_POINT;
    const TDesC& separator( User::Language() == ELangArabic ? 
                            KArabicSubjectSeparator : 
                            KWesternSubjectSeparator );

    aText = HBufC::NewLC(  iDate->Length()
                         + KNewLine().Length()
                         + iTime->Length()
                         + KNewLine().Length()
                         + iSubject->Length()
                         + separator.Length()
                         + iLocation->Length() );

    // add date
    *aText = *iDate;
    TPtr ptr = aText->Des();
    ptr.Append( KNewLine );

    // add time
    ptr.Append( *iTime );
    ptr.Append( KNewLine );

    // subject and location
    AppendSubjectLocationText( ptr, separator );

    CleanupStack::Pop( aText );
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Create alarm notification string for a calendar to-do alarm.
// "[To-Do Task:][\n][Due:][ ][date][\n][Subject][, ][Location]"
// -----------------------------------------------------------------------------
//
void CAlmAlarmInfo::SetupToDoAlarmL(HBufC*& aText)
{
    TRACE_ENTRY_POINT;
    HBufC* todoStr1 = StringLoader::LoadLC( R_EIKALARM_TODO_TITLE ); // "To-Do Task:"
    HBufC* todoStr2 = StringLoader::LoadLC( R_EIKALARM_TODO_DUE ); // "Due:"
    const TDesC& separator( User::Language() == ELangArabic ? 
                            KArabicSubjectSeparator : 
                            KWesternSubjectSeparator );

    aText = HBufC::NewLC(  todoStr1->Length()
                         + KNewLine().Length()
                         + todoStr2->Length()
                         + KSpace().Length()
                         + iDate->Length()
                         + KNewLine().Length()
                         + iSubject->Length()
                         + separator.Length()
                         + iLocation->Length() );

    // add default to-do alarm text
    *aText = *todoStr1;
    TPtr ptr = aText->Des();
    ptr.Append( KNewLine );
    ptr.Append( *todoStr2 );
    ptr.Append( KSpace );

    // add date
    ptr.Append( *iDate );
    ptr.Append( KNewLine );

    // subject and location
    AppendSubjectLocationText( ptr, separator );

    CleanupStack::Pop( aText );
    CleanupStack::PopAndDestroy( todoStr2 );
    CleanupStack::PopAndDestroy( todoStr1 );
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Create alarm notification string for a calendar anniversary alarm.
// "[date][\n][Subject][, ][Location]"
// -----------------------------------------------------------------------------
//
void CAlmAlarmInfo::SetupAnniversaryAlarmL(HBufC*& aText)
{
    TRACE_ENTRY_POINT;
    const TDesC& separator( User::Language() == ELangArabic ? 
                            KArabicSubjectSeparator : 
                            KWesternSubjectSeparator );

    aText = HBufC::NewLC(  iDate->Length()
                         + KNewLine().Length()
                         + iSubject->Length()
                         + separator.Length()
                         + iLocation->Length() );
    // add date
    *aText = *iDate;
    TPtr ptr = aText->Des();
    ptr.Append( KNewLine );

    // subject and location
    AppendSubjectLocationText( ptr, separator );

    CleanupStack::Pop( aText );
    TRACE_EXIT_POINT;
}

*/

// =============================================================================
// STRING UTILITY FUNCTIONS
// =============================================================================

// -----------------------------------------------------------------------------
// Append subject and location strings into aText.
// -----------------------------------------------------------------------------
//
void CAlmAlarmInfo::AppendSubjectLocationText(TPtr& aDest, const TDesC& aSeparator)
{
    TRACE_ENTRY_POINT;
    aDest.Append( *iSubject );

    if( iLocation->Length() )
    {
        if( iSubject->Length() )
        {
            // add ", " if we have both subject and location
            aDest.Append( aSeparator );
        }
        // add locaton
        aDest.Append( *iLocation );
    }
    TRACE_EXIT_POINT;
}

/*
// -----------------------------------------------------------------------------
// Create strings needed for calendar alarms.
// AVKON global note can't (currently) show more than 256 characters.  So no 
// need to use more than KAlarmNoteTextLength -KDateTimeDescLen for subject 
// and location strings.
// -----------------------------------------------------------------------------
//
void CAlmAlarmInfo::GetCoverUIParamsForCalendarL(const CCalEntry* aEntry)
{
    TRACE_ENTRY_POINT;
    HBufC* dateFormat = StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO );
    HBufC* timeFormat = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO );
    const TTime instanceTime( iAlarmUtils->AlarmData().iInstanceTime.TimeLocalL() );

    // subject
    iSubject = HBufC::NewL( KAlarmNoteTextLength -KDateTimeDescLen );
    iSubject->Des().Append( aEntry->SummaryL().Left( iSubject->Des().MaxLength() ) );
    iSubject->Des().TrimAll();

    // location
    iLocation = HBufC::NewL( Max( 0, KAlarmNoteTextLength -KDateTimeDescLen -iSubject->Length() ) );
    iLocation->Des().Append( aEntry->LocationL().Left( iLocation->Des().MaxLength() ) );
    iLocation->Des().TrimAll();

    // date
    iDate = HBufC::NewL( KDateTimeDescLen );
    TPtr datePtr = iDate->Des();
    instanceTime.FormatL( datePtr, *dateFormat );
    iCoverDate = iDate->AllocL();
    AknTextUtils::LanguageSpecificNumberConversion( datePtr ); // digit conversion

    // time
    if( aEntry->EntryTypeL() == CCalEntry::EAppt || 
        aEntry->EntryTypeL() == CCalEntry::EReminder )
    {
        iTime = HBufC::NewL( KDateTimeDescLen );
        TPtr timePtr = iTime->Des();
        instanceTime.FormatL( timePtr, *timeFormat );
        iCoverTime = iTime->AllocL();
        AknTextUtils::LanguageSpecificNumberConversion( timePtr ); // digit conversion
    }
    else // no time for other calendar types
    {
        iTime = HBufC::NewL( 0 );
        iCoverTime = HBufC::NewL( 0 );
    }

    CleanupStack::PopAndDestroy( timeFormat );
    CleanupStack::PopAndDestroy( dateFormat );
    TRACE_EXIT_POINT;
}

// -----------------------------------------------------------------------------
// Create strings needed for clock alarm and unknown alarms.
// -----------------------------------------------------------------------------
//
void CAlmAlarmInfo::GetCoverUIParamsForClockAndOtherL( const TASShdAlarm& aAlarm, 
                                                       const CAlarmUtils::TAlarmType aType )
{
    TRACE_ENTRY_POINT;
    HBufC* dateFormat = StringLoader::LoadLC( R_QTN_DATE_USUAL_WITH_ZERO );
    HBufC* timeFormat = StringLoader::LoadLC( R_QTN_TIME_USUAL_WITH_ZERO );

    // alarm description
    iSubject = aAlarm.Message().AllocL();
    iSubject->Des().TrimAll();

    // no location for clock alarms
    iLocation = HBufC::NewL( 0 );

    TTime alarmTime( aType == CAlarmUtils::EAlarmTypeClock ?
                     aAlarm.NextDueTime() :
                     aAlarm.OriginalExpiryTime() );

    // alarm time
    iTime = HBufC::NewL( KDateTimeDescLen );
    TPtr timePtr = iTime->Des();
    alarmTime.FormatL( timePtr, *timeFormat );
    iCoverTime = iTime->AllocL();
    AknTextUtils::LanguageSpecificNumberConversion( timePtr ); // digit conversion

    if( aType == CAlarmUtils::EAlarmTypeClock )
    {
        // no date for clock alarms
        iDate = HBufC::NewL( 0 );
        iCoverDate = HBufC::NewL( 0 );
    }
    else // other alarm types
    {
        // date
        iDate = HBufC::NewL( KDateTimeDescLen );
        TPtr datePtr = iDate->Des();
        alarmTime.FormatL( datePtr, *dateFormat );
        iCoverDate = iDate->AllocL();
        AknTextUtils::LanguageSpecificNumberConversion( datePtr ); // digit conversion
    }

    CleanupStack::PopAndDestroy( timeFormat );
    CleanupStack::PopAndDestroy( dateFormat );
    TRACE_EXIT_POINT;
}
*/

void CAlmAlarmInfo::SetupUnknownAlarmL(TASShdAlarm /* aAlarm */)
{
    
}

void CAlmAlarmInfo::SetupClockAlarmL(TASShdAlarm aAlarm)
{
    // The alarm subject
    iAlarmInfo->iSubject = aAlarm.Message().AllocL();
    iAlarmInfo->iSubject->Des().TrimAll();

    // Location must not be displayed for clock alarms
    iAlarmInfo->iLocation = HBufC::NewL( 0 );

    // The alarm time
    iAlarmInfo->iTime = aAlarm.NextDueTime();

    // Date must not be shown for clock alarms
    iAlarmInfo->iDate = TTime( 0 );
}

void CAlmAlarmInfo::SetupAppointmentAlarmL(TASShdAlarm /* aAlarm */)
{

}

void CAlmAlarmInfo::SetupToDoAlarmL(TASShdAlarm /* aAlarm */)
{
    
}

void CAlmAlarmInfo::SetupAnniversaryAlarmL(TASShdAlarm /* aAlarm */)
{
    
}

// End of File
