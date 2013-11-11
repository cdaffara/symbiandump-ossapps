/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains iCal exporter implementation.
*                Converts symbian agenda entries to iCal data.
*
*/



// Class include.
#include "AgnVersit2Exporter.h"

//debug
#include "calendarengines_debug.h"

// Agenda includes.
#include <calalarm.h>					// CCalAlarm
#include <calcategory.h>				// CCalCategory
#include <calentry.h>					// CCalEntry
#include <calrrule.h>					// TCalRRule
#include <caluser.h>					// CCalUser
#include <vtzrules.h>					// CVTzActualisedRules

// Versit includes.
#include "ICal.h"						// CICal
#include "ICalComponent.h"				// CICalComponent
#include "ICalKeyWords.h"				// Literals used during export
#include "ICalProperty.h"				// CICalProperty
#include "ICalValue.h"					// CICalValue
#include "ICalBase.h"					// TICalMethod

// AgnVersit2 includes.
#include "AgnVersit2StringProvider.h"	// CAgnVersit2StringProvider
#include "AgnExternalInterface.h"		// TAgnEntryExport

// Constants for timezone RRULE
_LIT(KRRuleFreqYearly, "FREQ=YEARLY;BYMONTH=");
_LIT(KRRuleByMonthDay, ";BYMONTHDAY=");
_LIT(KRRuleByDayPlus, ";BYDAY=+");
_LIT(KRRuleByDayPlusOne, ";BYDAY=+1");
_LIT(KRRuleByDayMinusOne, ";BYDAY=-1");

//iCalendar version number
_LIT(KICalVersionNumber,"2.0");


// Maximum characters in a GEO value should be 11 = -NNN.NNNNNN
const TUint KGEOMaxWidthOfGeoValue = 11;

/** A semi-colon character. */
const TUint KAgnVersitTokenSemiColonVal =';';

// Constant integer values
const TInt KMaxOffsetMinutes = 720; //12 hours * 60 minutes
const TInt KMinOffsetMinutes = -720; //-12 hours * 60 minutes

const TInt KSecondsInOneMinute = 60;
const TInt KInitialRRuleLength = 150; //RRule line length
const TInt KMaxBufLength = 80;	//Max time zone name length

/**
Constructs a new CAgnVersit2Exporter and returns it.
@param aObserver Application interface for error reporting.
@param aWriteStream Stream to write output to.
@param aStringProvider localisable string resources
@return The new CAgnVersit2Exporter
@internalTechnology
*/
CAgnVersit2Exporter* CAgnVersit2Exporter::NewL(CAgnVersit2StringProvider& aStringProvider)
	{
	TRACE_ENTRY_POINT;
		
	CAgnVersit2Exporter* self = CAgnVersit2Exporter::NewLC(aStringProvider);
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Constructs a new CAgnVersit2Exporter and returns it.
A pointer to the new object is left on the cleanup stack.
@param aObserver Application interface for error reporting.
@param aWriteStream Stream to write output to.
@param aStringProvider localisable string resources
@return The new CAgnVersit2Exporter
@internalTechnology
*/
CAgnVersit2Exporter* CAgnVersit2Exporter::NewLC(CAgnVersit2StringProvider& aStringProvider)
	{
	TRACE_ENTRY_POINT;
	
	CAgnVersit2Exporter* self = new (ELeave) CAgnVersit2Exporter(aStringProvider);
	CleanupStack::PushL(self);
	self->ConstructL();
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Destructor.
@internalTechnology
*/
CAgnVersit2Exporter::~CAgnVersit2Exporter()
	{
	TRACE_ENTRY_POINT;
	
	iTzNameArray.ResetAndDestroy();
	iTimeZoneArray.ResetAndDestroy();
	
	TRACE_EXIT_POINT;
	}
/**
Exports an array of calendar entries to a single iCalendar
@param aEntries array of entries to export.
@param aFlags not used.
@internalTechnology
*/
void CAgnVersit2Exporter::ExportL( RPointerArray<CCalEntry>& aEntries,
                                   RWriteStream& aWriteStream,
                                   TUint aExportFlags,
                                   MAgnExportObserver& aObserver )
	{
	TRACE_ENTRY_POINT;
	
	//We are exporting more than one entry to a single iCalendar. 
	//Clear the time zone arrays, otherwise if this function is called twice
	//components in this iCalendar may reference timezone components in a preceding
	//iCalendar.
	iTzNameArray.ResetAndDestroy();
	iTimeZoneArray.ResetAndDestroy();
		
	iExportProperties = aExportFlags;
	iObserver = &aObserver;
	ASSERT( aEntries.Count() > 0 );
	CICal* cal = AddICalLC(*aEntries[0]);
	
	TInt entryCount = aEntries.Count();
	
	TRAPD(err,
	
		for (TInt x = 0; x < entryCount; ++x)
			{
			if (iExportProperties & KAgnExportTzRules)
				{
				AddTimezoneL(*cal, *aEntries[x]);
				}
			AddComponentL(*cal, *aEntries[x]);
			}
		); //End TRAP
	
	User::LeaveIfError(err);

	cal->ExternalizeL(aWriteStream);
	CleanupStack::PopAndDestroy(cal);
	
	TRACE_EXIT_POINT;
	}
	
/**
Exports a given calendar entry.
@param aEntry entry to export.
@param aFlags not used.
@internalTechnology
*/
void CAgnVersit2Exporter::ExportL( const CCalEntry& aEntry,
                                   RWriteStream& aWriteStream,
                                   TUint aExportFlags,
                                   MAgnExportObserver& aObserver )
	{
	TRACE_ENTRY_POINT;
	
	//If we are exporting a single iCalendar per entry, we want to include the timezone
	//even if we have previously encountered it.  We do this by clearing the time zone 
	//name array
	iTzNameArray.ResetAndDestroy();
	iTimeZoneArray.ResetAndDestroy();
			
	iExportProperties = aExportFlags;
	iObserver = &aObserver;
	CICal* cal = AddICalLC(aEntry);

	TRAPD(err,
		if (iExportProperties & KAgnExportTzRules)
			{
			AddTimezoneL(*cal, aEntry);
			}
			AddComponentL(*cal, aEntry);
		);


	User::LeaveIfError(err);

	cal->ExternalizeL(aWriteStream);
	CleanupStack::PopAndDestroy(cal);
	
	TRACE_EXIT_POINT;
	}

/**
Constructor.
@param aObserver Application interface for error reporting.
@param aWriteStream Stream to write output to.
@param aStringProvider localisable string resources
@internalTechnology
*/
CAgnVersit2Exporter::CAgnVersit2Exporter(CAgnVersit2StringProvider& aStringProvider) :
	iStringProvider(aStringProvider)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

/**
Internal construction.
@internalTechnology
*/
void CAgnVersit2Exporter::ConstructL()
	{
	TRACE_ENTRY_POINT;
	iTimezoneIndex = KErrNotFound;
	
	TRACE_EXIT_POINT;
	}

/**
Creates a new CICal*, adds any properties and returns it. The new object is
left on the cleanup stack.
@code
ICalendar components can have the following properties:
	PRODID: This is fixed for all exports from this device.
	VERSION: Should	only ever be 2.0 for iCalendar v1 (vCalendar 2).
	METHOD: Member data from aEntry.
@endcode
@param aEntry entry to export.
@return newly created CICal*.
@internalTechnology
*/
CICal* CAgnVersit2Exporter::AddICalLC(const CCalEntry& aEntry) const
	{
	TRACE_ENTRY_POINT;
	
	CICal* cal = CICal::NewLC();
	cal->AddPropertyL(KICalProdId, iStringProvider.StringL(EICalProdIdValue));
	cal->AddPropertyL(KICalVersion,KICalVersionNumber);

	switch (aEntry.MethodL())
		{
		case CCalEntry::EMethodPublish :
			cal->AddPropertyL(KICalMethod, KICalPublish);
			cal->SetMethodL(CICal::EMethodPublish);
			break;
		case CCalEntry::EMethodRequest :
			cal->AddPropertyL(KICalMethod, KICalRequest);
			cal->SetMethodL(CICal::EMethodRequest);
			break;
		case CCalEntry::EMethodReply :
			cal->AddPropertyL(KICalMethod, KICalReply);
			cal->SetMethodL(CICal::EMethodReply);
			break;
		case CCalEntry::EMethodAdd :
			cal->AddPropertyL(KICalMethod, KICalAdd);
			cal->SetMethodL(CICal::EMethodAdd);
			break;
		case CCalEntry::EMethodCancel :
			cal->AddPropertyL(KICalMethod, KICalCancel);
			cal->SetMethodL(CICal::EMethodCancel);
			break;
		case CCalEntry::EMethodRefresh :
			cal->AddPropertyL(KICalMethod, KICalRefresh);
			cal->SetMethodL(CICal::EMethodRefresh);
			break;
		case CCalEntry::EMethodCounter :
			cal->AddPropertyL(KICalMethod, KICalCounter);
			cal->SetMethodL(CICal::EMethodCounter);
			break;
		case CCalEntry::EMethodDeclineCounter :
			cal->AddPropertyL(KICalMethod, KICalDeclineCounter);
			cal->SetMethodL(CICal::EMethodDeclineCounter);
			break;
		case CCalEntry::EMethodNone :
			// fall through...
		default :
			//Add MEHTOD:PUBLISH as default if no method exists
			//needed by outlook
			cal->AddPropertyL(KICalMethod, KICalPublish);
			cal->SetMethodL(CICal::EMethodPublish);
			break;
		}
	
	TRACE_EXIT_POINT;
	return cal;
	}


//
//     ADD COMPONENT METHODS
//     =====================
//

/**
Exports an agenda Anniversary to an iCalendar component.
@param aAnniv component to add to.
@param aEntry Entry containing information about the anniversary.
@internalTechnology
*/
void CAgnVersit2Exporter::AddAnnivL(CICalComponent& aAnniv, const CCalEntry& /*aEntry*/) const
	{
	TRACE_ENTRY_POINT;
	
	//Add 'X-PARAM' identifier
	if (iExportProperties & KAgnExportXProp)
		{
		aAnniv.AddPropertyL( iStringProvider.StringL(EICalXParamType),
		                     iStringProvider.StringL(EICalXParamAnniversary) );
		}
	
	TRACE_EXIT_POINT;
	}


/**
Adds an alarm to a component.
@code

RFC 2445 states that an alarm MUST have an ACTION and TRIGGER property. We can
retrieve the trigger property value from the alarm offset, but Agenda does not
support the ACTION property.

An alarm is defined as:

	"BEGIN" ":" "VALARM" CRLF
	(audioprop / dispprop / emailprop / procprop)
	"END" ":" "VALARM" CRLF

where audioprop / dispprop / emailprop and procprop are all values of the
ACTION property.

An ACTION property value of AUDIO only requires an additional TRIGGER property,
all other properties are optional.  To comply with RFC 2445, all alarms will be
exported with the following property:

	ACTION:AUDIO

as to set ACTION to any other value would require more properties, which we do
not have values for.

@endcode
@param aComponent Component to add the VALARM component to
@param aEntry Entry to extract ALARM information from
@internalTechnology
*/
void CAgnVersit2Exporter::AddAlarmL(CICalComponent& aComponent, const CCalEntry& aEntry) const
	{
	TRACE_ENTRY_POINT;
	
	CCalAlarm* alarm = aEntry.AlarmL();
	if (alarm)
		{
		CleanupStack::PushL(alarm);
		CICalComponent& component = aComponent.AddComponentL(CICalBase::EICalAlarm);
		CICalValue* triggerVal = CICalValue::NewLC();
		triggerVal->SetDurationL(TTimeIntervalSeconds(-alarm->TimeOffset().Int() * KSecondsInOneMinute));
		CleanupStack::Pop(triggerVal);
		component.AddPropertyL(KICalTrigger, triggerVal); //Pass ownership
		component.AddPropertyL(KICalAction, KICalAudio);
		CleanupStack::PopAndDestroy(alarm);
		}
	
	TRACE_EXIT_POINT;
	}

/**
Adds an appointment to a component.
@param aAppt Component to add the appointment to.
@param aEntry Entry to extract the appointment information from.
@internalTechnology
*/
void CAgnVersit2Exporter::AddApptL(CICalComponent& aAppt, const CCalEntry& aEntry) const
	{
	TRACE_ENTRY_POINT;
	
	//Add 'DTEND' property
	if (iExportProperties & KAgnExportDtEnd)
		{
		TCalTime time(aEntry.EndTimeL());
		if (time.TimeUtcL() != Time::NullTTime())
			{
			AddDateTimePropertyL(aAppt, time, KICalDtend);
			}
		}
		
	if (iExportProperties & KAgnExportStatus)
		{
		//Add 'STATUS' property
		switch (aEntry.StatusL())
			{
			//VEvent supports TENTATIVE, CONFIRMED, CANCELLED
			case CCalEntry::ETentative:
				aAppt.AddPropertyL(KICalStatus, KICalTentative);
				break;
			case CCalEntry::EConfirmed:
				aAppt.AddPropertyL(KICalStatus, KICalConfirmed);
				break;
			case CCalEntry::ECancelled:
				aAppt.AddPropertyL(KICalStatus, KICalCancelled);
				break;
			case CCalEntry::ETodoNeedsAction :	// not supported, fall through
			case CCalEntry::ETodoCompleted :	// not supported, fall through
			case CCalEntry::ETodoInProcess :	// not supported, fall through
			default:
				//Do not add a status property
				break;
			}
		}
			
	//Add 'X-PARAM' identifier
	if (iExportProperties & KAgnExportXProp)
		{
		aAppt.AddPropertyL( iStringProvider.StringL(EICalXParamType),
		                    iStringProvider.StringL(EICalXParamAppointment) );
		}
	
	TRACE_EXIT_POINT;
	}

/**
Adds a component to aICal. The type of component added is determined by the
value of aEntry.EntryTypeL(). Generic properties (valid in both VEVENT and
VTODO components) are added first, followed by type specific properties.
@param aICal CICal object to add the component to.
@param aEntry CCalEntry containing information about the component.
@internalTechnology
*/
void CAgnVersit2Exporter::AddComponentL(CICal& aICal, const CCalEntry& aEntry) const
	{
	TRACE_ENTRY_POINT;
	
	//Add the  component
	CICalBase::TICalComponentType type(CICalBase::EICalEvent);
	switch (aEntry.EntryTypeL())
		{
		//The agenda types all map to an iCalendar 'VEVENT' component
		case CCalEntry::EAppt :
			// fall through...
		case CCalEntry::EEvent :
			// fall through...
		case CCalEntry::EReminder :
			// fall through...
		case CCalEntry::EAnniv :
			{
			type = CICalBase::EICalEvent;
			break;
			}
		case CCalEntry::ETodo :
			{
			type = CICalBase::EICalTodo;
			break;
			}
		default :
			User::Leave(KErrNotSupported); //For now at least
			break;
		}
	CICalComponent& comp = aICal.AddComponentL(type);

	//Add 'ATTENDEE' properties
	if (iExportProperties & KAgnExportAttendee)
		{
		AddAttendeePropertiesL(comp, aEntry);
		}

	//Add 'ORGANIZER' property
	if (iExportProperties & KAgnExportOrganizer)
		{
		AddOrganizerPropertyL(comp, aEntry);
		}

	//Add 'LOCATION' property.
	if (iExportProperties & KAgnExportLocation)
		{
		AddTextPropertyL(comp, aEntry.LocationL(), KICalLocation);
		}

	//Outlook puts TRANSP here. The default value is 'OPAQUE'.

	//Add 'SEQUENCE' property.  The default value is '0', so
	//the property is only added if there is a value.
	if (iExportProperties & KAgnExportSequence)
		{
		TInt sequence = aEntry.SequenceNumberL();
		if (sequence != 0)
			{
			AddIntegerPropertyL(comp, aEntry.SequenceNumberL(), KICalSequence);
			}
		}

	//Add 'UID' property
	if (iExportProperties & KAgnExportUID)
		{
		CICalValue* uidVal = CICalValue::NewLC();
		uidVal->SetBinaryL(aEntry.UidL());
		CleanupStack::Pop(uidVal);
		comp.AddPropertyL(KICalUid, uidVal); //Pass ownership
		}

    //Time for DTSTAMP, CREATED and LAST-MODIFIED
	TCalTime time;
	
	//Check if DTSTAMP exist
	if( aEntry.DTStampL().TimeUtcL() != Time::NullTTime() )
	    {
	    time = aEntry.DTStampL();    
	    }
	else //If not use current time
	    {
	    TTime currentTime;
	    currentTime.UniversalTime();
	    time.SetTimeUtcL( currentTime );
	    }
	
	//Add 'DTSTAMP' property
	if (iExportProperties & KAgnExportDtStamp)
		{
		AddUtcDateTimePropertyL(comp, time.TimeUtcL(), KICalDtstamp);
		}

	//Add 'CATEGORIES' properties
	if (iExportProperties & KAgnExportCategory)
		{
		AddCategoriesPropertyL(comp, aEntry);
		}

	//Add 'DESCRIPTION' property.
	if (iExportProperties & KAgnExportDescription)
		{
		AddTextPropertyL(comp, aEntry.DescriptionL(), KICalDescription);
		}

	//Add 'SUMMARY' property
	if (iExportProperties & KAgnExportSummary)
		{
		AddTextPropertyL(comp, aEntry.SummaryL(), KICalSummary);
		}


	//Add 'PRIORITY' property
	if (iExportProperties & KAgnExportPriority)
		{
		TInt priority = aEntry.PriorityL();
		
		//Pirorites are saved always in vcal format
		//so they are mapped the following way
		//vCal 1 = iCal 1
		//vCal 2 = iCal 5
		//vCal 3 = iCal 9
		if (priority != 0)	// Zero priority is default
			{
			if( priority == 1 )
			    {
			    }
			
			else if( priority == 2 )
			    {
			    priority = 5;
			    }
			
			else
			    {
			    priority = 9;
			    }
			
			AddIntegerPropertyL(comp, priority, KICalPriority);
			}
		}

	//Add 'CLASS' property
	if (iExportProperties & KAgnExportClass)
		{
		AddClassPropertyL(comp, aEntry);
		}
    
    //Check if last modified date exist    
    if( aEntry.LastModifiedDateL().TimeUtcL() != Time::NullTTime() )
	    {
	    //update time with it otherwise set CREATED and LAST-MODIFIED to
	    //current time
	    time = aEntry.LastModifiedDateL();    
	    }

	//Add 'CREATED' property with current system time.
	//time should be set to 'now' in UTC if not already.
	if (iExportProperties & KAgnExportCreated)
		{
		AddUtcDateTimePropertyL(comp, time.TimeUtcL(), KICalCreated);
		}

	//Add 'LAST-MODIFIED' property
	if (iExportProperties & KAgnExportLastModified)
		{
		AddUtcDateTimePropertyL(comp, time.TimeUtcL(), KICalLastmodified);
		}

	//Add 'RECURRENCE-ID' property
	if (iExportProperties & KAgnExportRecurrenceId)
		{
		time = aEntry.RecurrenceIdL();
		if (time.TimeUtcL() != Time::NullTTime())
			{
			CICalProperty& recurProp = AddDateTimePropertyL(comp, time, KICalRecurrenceId);
			switch (aEntry.RecurrenceRangeL())
				{
				case CalCommon::EThisAndFuture:
					{
					recurProp.AddPropertyParamL(KICalRange,KICalThisAndFuture);
					}
				break;
				case CalCommon::EThisAndPrior:
					{
					recurProp.AddPropertyParamL(KICalRange,KICalThisAndPrior);
					}
				break;
				default:
				break;
				}

			}
		}

	//Add 'EXDATE' properties
	if (iExportProperties & KAgnExportExDate)
		{
		AddExceptionDatePropertyL(comp, aEntry);
		}

	//Add 'RDATE' property
	if (iExportProperties & KAgnExportRDate)
		{
		AddRDatePropertyL(comp, aEntry);
		}

	//Add 'RRULE' property
	if (iExportProperties & KAgnExportRRule)
		{
		AddRRulePropertyL(comp, aEntry);
		}

	//Add 'VALARM' component
	if (iExportProperties & KAgnExportAlarm)
		{
		AddAlarmL(comp, aEntry);
		}

	//Add 'DTSTART' property
	if (iExportProperties & KAgnExportDtStart)
		{
		time = aEntry.StartTimeL();
		if (time.TimeUtcL() != Time::NullTTime())
			{
			AddDateTimePropertyL(comp, time, KICalDtstart);
			}
		}

	//Add type specific properties
	switch (aEntry.EntryTypeL())
		{
		case CCalEntry::EAppt:
			AddApptL(comp, aEntry);
			break;
		case CCalEntry::EEvent:
			AddEventL(comp, aEntry);
			break;
		case CCalEntry::EReminder:
			AddReminderL(comp, aEntry);
			break;
		case CCalEntry::EAnniv:
			AddAnnivL(comp, aEntry);
			break;
		case CCalEntry::ETodo:
			AddTodoL(comp, aEntry);
			break;
		default:
			//Nothing more to add
			break;
		}
	
	// Add GEO
    CCalGeoValue* geoValue = aEntry.GeoValueL();
    
    // Check that the GEO values are not NULL
    if(geoValue)
        {
        CleanupStack::PushL(geoValue);
        
        TReal geoLatitude;
        TReal geoLongitude;
        
        // Extract latitude and longitude values
        // Check if it returns EFalse
        geoValue->GetLatLong(geoLatitude,geoLongitude);
        
        // Convert the geo values from numbers to string
        // Create GEO string buffer to be constructed from a latitude, delimiter and a longitude value
        TBuf<KGEOMaxWidthOfGeoValue*2+1> geoString;
        TBuf<KGEOMaxWidthOfGeoValue> geoLatString;
        TBuf<KGEOMaxWidthOfGeoValue> geoLongString;
        
        // Maximum width of a GEO value and max number of decimal places
        TRealFormat geoFormat(KGEOMaxWidthOfGeoValue,KCalGEOMaxDecimalPlaces);
        
        _LIT(KGeoStringFormat,"%S%c%S");
        
        // Ensure correct conversion from stored numeric values to descriptors
        if((geoLatString.Num(geoLatitude,geoFormat)>0) && (geoLongString.Num(geoLongitude,geoFormat)>0))
            {
            geoString.AppendFormat(KGeoStringFormat,&geoLatString,KAgnVersitTokenSemiColonVal,&geoLongString);
            
            // Add the GEO property
            AddTextPropertyL(comp,geoString,KICalGeo);
            }
        
        CleanupStack::PopAndDestroy(geoValue);
        }
	TRACE_EXIT_POINT;
	}

/**
Adds the information stored in aEntry to aEvent
@param aEvent CICalComponent& to add the agenda information to.
@param aEntry CCalEntry& containing the agenda information to export.
@internalTechnology
*/
void CAgnVersit2Exporter::AddEventL(CICalComponent& aEvent, const CCalEntry& aEntry) const
	{
	TRACE_ENTRY_POINT;
	
	//Add 'DTEND' property
	if (iExportProperties & KAgnExportDtEnd)
		{
		TCalTime time(aEntry.EndTimeL());
		if (time.TimeUtcL() != Time::NullTTime())
			{
			AddDateTimePropertyL(aEvent, time, KICalDtend);
			}
		}

	if (iExportProperties & KAgnExportStatus)
		{
		//Add 'STATUS' property
		switch (aEntry.StatusL())
			{
			//VEvent supports TENTATIVE, CONFIRMED, CANCELLED
			case CCalEntry::ETentative:
				aEvent.AddPropertyL(KICalStatus, KICalTentative);
				break;
			case CCalEntry::EConfirmed:
				aEvent.AddPropertyL(KICalStatus, KICalConfirmed);
				break;
			case CCalEntry::ECancelled:
				aEvent.AddPropertyL(KICalStatus, KICalCancelled);
				break;
			case CCalEntry::ETodoNeedsAction :	// not supported, fall through
			case CCalEntry::ETodoCompleted :	// not supported, fall through
			case CCalEntry::ETodoInProcess :	// not supported, fall through
			default:
				//Do not add a status property
				break;
			}
		}
	//Add 'X-PARAM' identifier
	if (iExportProperties & KAgnExportXProp)
		{
		aEvent.AddPropertyL( iStringProvider.StringL(EICalXParamType),
		                     iStringProvider.StringL(EICalXParamEvent) );
		}
	
	TRACE_EXIT_POINT;
	}

/**
Adds an agenda-style Reminder to the specified component.
@param aReminder Component to add the reminder to.
@param aEntry CCalEntry containing details of the reminder.
@internalTechnology
*/
void CAgnVersit2Exporter::AddReminderL(CICalComponent& aReminder, const CCalEntry& /*aEntry*/) const
	{
	TRACE_ENTRY_POINT;
	
	//Add 'X-PARAM' identifier
	if (iExportProperties & KAgnExportXProp)
		{
		aReminder.AddPropertyL( iStringProvider.StringL(EICalXParamType),
		                        iStringProvider.StringL(EICalXParamReminder) );
		}
	
	TRACE_EXIT_POINT;
	}

/**
Adds a Timezone component to the specified CCalEntry.
@param aICal CICal object to add the component to.
@param aEntry CCalEntry containing information about the Timezone.
@internalTechnology
*/
void CAgnVersit2Exporter::AddTimezoneL(CICal& aICal, const CCalEntry& aEntry)
	{
	TRACE_ENTRY_POINT;
	
	iTimezoneIndex = KErrNotFound;
	
	CTzRules* tzRules = aEntry.TzRulesL();
	CleanupStack::PushL(tzRules);

	if (!tzRules || tzRules->Count() < 1)
		{
		CleanupStack::PopAndDestroy(tzRules);
		return;
		}

	TInt minimumMinutes = KMaxOffsetMinutes;// TInts to store the maximum and minimum UTC offset
	TInt maximumMinutes = KMinOffsetMinutes;// to get a human readable name for TZID
	TInt count = tzRules->Count();
	for (TInt i = 0; i < count; ++i)
		{
		TTzRule& singleRule = (*tzRules)[i];
		if (singleRule.iNewLocalTimeOffset < minimumMinutes)
			{
			minimumMinutes = singleRule.iNewLocalTimeOffset;
			}
		if (singleRule.iNewLocalTimeOffset > maximumMinutes)
			{
			maximumMinutes = singleRule.iNewLocalTimeOffset;
			}
		if (singleRule.iOldLocalTimeOffset < minimumMinutes)
			{
			minimumMinutes = singleRule.iOldLocalTimeOffset;
			}
		if (singleRule.iOldLocalTimeOffset > maximumMinutes)
			{
			maximumMinutes = singleRule.iOldLocalTimeOffset;
			}
		}

	// Now we've got the max and minimum values, store these in the TzId text field
	// iTimeZoneName allocates a new HBufC, but either deletes it or transfers ownership
	// to iTznamesArray
	HBufC* timeZoneName = HBufC::NewLC(KMaxBufLength);
	timeZoneName->Des().Copy(iStringProvider.StringL(EICalTzidUtc));

	//Convert minutes to hours
	const TInt KConversionMul = 10;
	const TInt KConversionDiv = 6;
	
	// Number of digits to store time in
	const TInt KTimeWidth = 4;

	TInt minimumHours = minimumMinutes * KConversionMul / KConversionDiv; // convert to HHMM format
	if (minimumHours < 0)
		{
		minimumHours =- minimumHours;
		timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidMinus));
		}
	else
		{
		timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidPlus));
		}
	timeZoneName->Des().AppendNumFixedWidth(minimumHours, EDecimal, KTimeWidth);
	timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidStandard));

	//Add a slash seperator
	timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidSlash));
	timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidUtc));

	TInt maximumHours = maximumMinutes * KConversionMul / KConversionDiv; // convert to HHMM format
	if (maximumHours < 0)
		{
		maximumHours =- maximumHours;
		timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidMinus));
		}
	else
		{
		timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidPlus));
		}
	timeZoneName->Des().AppendNumFixedWidth(maximumHours, EDecimal, KTimeWidth);
	timeZoneName->Des().Append(iStringProvider.StringL(EICalTzidDaylight));

	// The time zone name should now read: "UTC +HHMM (Standard) / UTC +HHMM (Daylight)".
	
	// Check if we have already used this timezone in this iCal.  If we have,
	// we reuse this string but don't output the timezone again.
	TInt tzCount = iTzNameArray.Count();
	for (TInt x = 0; x < tzCount; ++x)
		{
		if ((iTzNameArray[x])->CompareC(*timeZoneName) == 0)
			{
			//Already used this timezone
			iTimezoneIndex = x;
			CleanupStack::PopAndDestroy(timeZoneName);
			CleanupStack::PopAndDestroy(tzRules);
			return;
			}
		}

	//Add the 'VTIMEZONE' component
	CICalComponent& timezone = aICal.AddComponentL(CICalBase::EICalTimeZone);

	timezone.AddPropertyL(KICalTzid, *timeZoneName);


	for (TInt ii = 0; ii < count; ++ii)
		{
		TTzRule& singleRule = (*tzRules)[ii];

		// Set the DAYLIGHT/STANDARD field based on whether the
		// time zone rule shifts time forwards or backwards
		CICalComponent& tzProp = singleRule.iNewLocalTimeOffset < singleRule.iOldLocalTimeOffset ?
			timezone.AddComponentL(CICalBase::EICalStandard) :
			timezone.AddComponentL(CICalBase::EICalDaylight);

		//  -DTSTART (Required)
		CICalValue* dtstart = CICalValue::NewLC();


		TTime startTime = singleRule.iFrom.iTime;
        
        //Check whether the year is 0, if so set it to 1970
		if( !startTime.DateTime().Year() )//Year == 0000
		    {
		    TDateTime dateTime = startTime.DateTime();
		    dateTime.SetYear( 1970 );
		    startTime = dateTime;
		    }

		// DTSTART in a VTIMEZONE must always be in local time (with no TZ identifier).
		dtstart->SetDateTimeL(startTime,CICalValue::ESpecifiedTimeZone);

		CleanupStack::Pop(dtstart);
		tzProp.AddPropertyL(KICalDtstart, dtstart); //Pass ownership

		//  -RRULE (optional)
		CICalValue* tzRrule = CICalValue::NewLC();
		HBufC* ruleText = HBufC::NewLC(KMaxBufLength);
		ruleText->Des().Copy(KRRuleFreqYearly);

		TInt monthNum = singleRule.iMonth + 1;

		switch (singleRule.iDayRule)
			{
			case ETzFixedDate :
				{
				// e.g. 11th October
				// Equivalent to FREQ=YEARLY;BYMONTH=?;BYMONTHDAY=?
				// This won't get imported by Outlook, but it's the only way of exporting it.
				// It can't be converted to a week of the month.
				ruleText->Des().AppendNum(monthNum);
				ruleText->Des().Append(KRRuleByMonthDay);
				ruleText->Des().AppendNum(singleRule.iDayOfMonth);
				break;
				}
			case ETzDayAfterDate :
				{
				// e.g. first Sunday after 8th October
				// To get this to a format understood by Outlook, we have to convert the day of
				// month into a week number. If the day of the month is greater than 21, the week
				// could cover multiple months (and possibly years), so we export using BYMONTHDAY.
				const TInt KLastPossibleDayOfMonth = 21;
				if (singleRule.iDayOfMonth > KLastPossibleDayOfMonth)
					{
					// Equivalent to FREQ=YEARLY;BYMONTH=?;BYMONTHDAY=?;BYDAY=+1?
					ruleText->Des().AppendNum(monthNum);
					ruleText->Des().Append(KRRuleByMonthDay);
					ruleText->Des().AppendNum(singleRule.iDayOfMonth);
					ruleText->Des().Append(KRRuleByDayPlusOne);
					ruleText->Des().Append(DayFromInt(singleRule.iDayOfWeek));
					}
				else
					{
					// Equivalent to FREQ=YEARLY;BYMONTH=?;BYDAY=+n?
					ruleText->Des().AppendNum(monthNum);
					ruleText->Des().Append(KRRuleByDayPlus);
					ruleText->Des().AppendNum(WeekNumFromDayOfMonth(singleRule.iDayOfMonth));
					ruleText->Des().Append(DayFromInt(singleRule.iDayOfWeek));
					}
				break;
				}
			case ETzDayBeforeDate :
				{
				// e.g. Sunday before 8th October
				// To get this to a format understood by Outlook, we have to convert the day of
				// month into a week number. If the day of the month is less than 8, the week
				// could cover multiple months (and possibly years), so we export using BYMONTHDAY.
				const TInt KFirstPossibleDayOfMonth = 8;
				if (singleRule.iDayOfMonth < KFirstPossibleDayOfMonth)
					{
					// Equivalent to FREQ=YEARLY;BYMONTH=?;BYMONTHDAY=?;BYDAY=-1?
					ruleText->Des().AppendNum(monthNum);
					ruleText->Des().Append(KRRuleByMonthDay);
					ruleText->Des().AppendNum(singleRule.iDayOfMonth);
					ruleText->Des().Append(KRRuleByDayMinusOne);
					ruleText->Des().Append(DayFromInt(singleRule.iDayOfWeek));
					}
				else
					{
					// Equivalent to FREQ=YEARLY;BYMONTH=?;BYDAY=+n?
					ruleText->Des().AppendNum(monthNum);
					ruleText->Des().Append(KRRuleByDayPlus);
					ruleText->Des().AppendNum(WeekNumFromDayOfMonth(singleRule.iDayOfMonth));
					ruleText->Des().Append(DayFromInt(singleRule.iDayOfWeek));
					}
				break;
				}
			case ETzDayInLastWeekOfMonth :
				{
				// e.g. last Sunday in October
				// Equivalent to FREQ=YEARLY;BYMONTH=?;BYDAY=-1?
				ruleText->Des().AppendNum(monthNum);
				ruleText->Des().Append(KRRuleByDayMinusOne);
				ruleText->Des().Append(DayFromInt(singleRule.iDayOfWeek));
				break;
				}
			default :
				User::Leave(KErrCorrupt);
				break;
			}
		tzRrule->SetRecurrenceRuleL(*ruleText);
		CleanupStack::PopAndDestroy(ruleText);
		CleanupStack::Pop(tzRrule);
		tzProp.AddPropertyL(KICalRRule, tzRrule); //Pass ownership

		//  -TZOFFSETFROM (Required)
		CICalValue* tzOffsetFrom = CICalValue::NewLC();
		tzOffsetFrom->SetUtcOffsetL(
		            TTimeIntervalSeconds(singleRule.iOldLocalTimeOffset * KSecondsInOneMinute) );
		CleanupStack::Pop(tzOffsetFrom);
		tzProp.AddPropertyL(KICalTzoffsetfrom, tzOffsetFrom); //Pass ownership

		//  -TZOFFSETTO (Required)
		CICalValue* tzOffsetTo = CICalValue::NewLC();
		tzOffsetTo->SetUtcOffsetL(
		            TTimeIntervalSeconds(singleRule.iNewLocalTimeOffset * KSecondsInOneMinute) );
		CleanupStack::Pop(tzOffsetTo);
		tzProp.AddPropertyL(KICalTzoffsetto, tzOffsetTo); //Pass ownership
		}
		
	iTzNameArray.AppendL(timeZoneName); //Transfer ownership
	iTimeZoneArray.AppendL(tzRules);	//Transfer ownership
	
	iTimezoneIndex = iTimeZoneArray.Count() -1;
	CleanupStack::Pop(timeZoneName);
	CleanupStack::Pop(tzRules);
	
	TRACE_EXIT_POINT;
	}

/**
Adds properties specific to VTODO to the specified component.
@param aTodo VTODO component to add properties to.
@param aEntry CCalEntry containing details of the TODO.
@internalTechnology
*/
void CAgnVersit2Exporter::AddTodoL(CICalComponent& aTodo,const CCalEntry& aEntry) const
	{
	TRACE_ENTRY_POINT;
	
	TCalTime time(aEntry.EndTimeL());

	if (iExportProperties & KAgnExportDtEnd)
		{
		if (time.TimeUtcL() != Time::NullTTime())
			{
			//Add 'DUE' property
			AddDateTimePropertyL(aTodo, time, KICalDue);
			}
		}

	//Add 'COMPLETED' property
	if (iExportProperties & KAgnExportCompleted)
		{
		time = aEntry.CompletedTimeL();
		if (time.TimeUtcL() != Time::NullTTime())
			{
			//Add 'COMPLETED' property
			AddDateTimePropertyL(aTodo, time, KICalCompleted);
			}
		}

	//Add 'STATUS' property
	if (iExportProperties & KAgnExportStatus)
		{
		switch (aEntry.StatusL())
			{
			//VTODO supports NEEDS_ACTION, COMPLETED, IN-PROCESS, CANCELLED
			case CCalEntry::ECancelled :
				aTodo.AddPropertyL(KICalStatus, KICalCancelled);
				break;
			case CCalEntry::ETodoNeedsAction :
				aTodo.AddPropertyL(KICalStatus, KICalNeedsAction);
				break;
			case CCalEntry::ETodoCompleted :
				aTodo.AddPropertyL(KICalStatus, KICalCompleted);
				break;
			case CCalEntry::ETodoInProcess :
				aTodo.AddPropertyL(KICalStatus, KICalInProcess);
				break;
			case CCalEntry::ETentative :	// not supported, fall through
			case CCalEntry::EConfirmed :	// not supported, fall through
			default :
				//Do not add a status property
				break;
			}
		}
	//Add 'X-PARAM' identifier
	if (iExportProperties & KAgnExportXProp)
		{
		aTodo.AddPropertyL( iStringProvider.StringL(EICalXParamType),
		                    iStringProvider.StringL(EICalXParamTodo) );
		}
	
	TRACE_EXIT_POINT;
	}


//
//     ADD PROPERTY FUNCTIONS
//     ======================
//

/**
Adds 'ATTENDEE' properties to aComponent from aEntry
@param aComponent component to add the ATTENDEE property to
@param aEntry entry to extract ATTENDEE information from
@internalTechnology
*/
void CAgnVersit2Exporter::AddAttendeePropertiesL( CICalComponent& aComponent,
                                                  const CCalEntry& aEntry ) const
	{
	TRACE_ENTRY_POINT;
	
	const RPointerArray<CCalAttendee>& attendees = aEntry.AttendeesL();
	const TInt count = attendees.Count();
	CCalAttendee* attendee = NULL;
	CICalValue* val = NULL;
	for (TInt x = 0; x < count; ++x)
		{
		/** Can't add the following property parameters due to no equivalence:
		*   (";" dirparam) / (";" languageparam) / (";" cutypeparam) /
		*   (";"memberparam) / (";" deltoparam) / (";" delfromparam)
		*/
		attendee = attendees[x]; //Not taking ownership
		val = CICalValue::NewLC();
		val->SetTextL(attendee->Address());

		//Add a property for each ATTENDEE
		CleanupStack::Pop(val);
		CICalProperty& attProp = aComponent.AddPropertyL(KICalAttendee, val); //Pass ownership

		//Add 'CN' property parameter
		TPtrC commonName(attendee->CommonName());
		if (commonName.Length() > 0)
			{
			attProp.AddPropertyParamL(KICalCn, commonName);
			}

		//Add 'ROLE' property parameter
		TPtrC roleName(RoleFromEnum(attendee->RoleL()));
		if (roleName.Length() > 0)
			{
			attProp.AddPropertyParamL(KICalRole, roleName);
			}

		//Add 'RSVP' property parameter
		// Default is FALSE so don't add a value if this is the case
		if (attendee->ResponseRequested())
			{
			CICalValue* rsvpVal = CICalValue::NewLC();
			rsvpVal->SetBooleanL(ETrue);
			CleanupStack::Pop(rsvpVal);
			attProp.AddPropertyParamL(KICalRsvp, rsvpVal);
			}

		//Add "SENT-BY" property parameter
		TPtrC sentBy(attendee->SentBy());
		if (sentBy.Length() > 0)
			{
			attProp.AddPropertyParamL(KICalSentBy, sentBy);
			}

		//Add PARTSTAT property parameter
		TPtrC participationStatus(StatusFromEnum(attendee->StatusL()));
		if (participationStatus.Length() > 0)
			{
			attProp.AddPropertyParamL(KICalPartStat, participationStatus);
			}
		}
	
	TRACE_EXIT_POINT;
	}

/**
Adds 'CATEGORY' properties to aComponent from aEntry
@param aComponent component to add the CATEGORY property to
@param aEntry entry to extract CATEGORY information from
@internalTechnology
*/
void CAgnVersit2Exporter::AddCategoriesPropertyL( CICalComponent& aComponent,
                                                  const CCalEntry& aEntry ) const
	{
	TRACE_ENTRY_POINT;
	
	//CategoryListL is not const so we need a non-const CalEntry to call it
	CCalEntry& nonConstEntry = const_cast<CCalEntry&>(aEntry);
	const RPointerArray<CCalCategory>& categories = nonConstEntry.CategoryListL();
	const TInt count = categories.Count();
	if (count)
		{
		CICalProperty& catProp =
                aComponent.AddPropertyL(KICalCategories, CategoryStringL(*categories[0]));
		for (TInt x = 1; x < count; ++x)
			{
			catProp.AddValueL(CategoryStringL(*categories[x]));
			}
		}
	
	TRACE_EXIT_POINT;
	}

/**
Adds a CLASS property to the specified component
@param aComponent component to add the CLASS property to
@param aEntry entry to extract CLASS information from
@internalTechnology
*/
void CAgnVersit2Exporter::AddClassPropertyL( CICalComponent& aComponent,
                                             const CCalEntry& aEntry ) const
	{
	TRACE_ENTRY_POINT;
	
	CCalEntry::TReplicationStatus repStatus = aEntry.ReplicationStatusL();
	TPtrC status(ClassStringL(repStatus));
	if (status.Length() > 0)
		{
		// Only add the CLASS property if it is not the default, 'PUBLIC'.
		if (status.CompareF(KICalPublic) != 0)
			{
			aComponent.AddPropertyL(KICalClass, status);
			}
		}
	
	TRACE_EXIT_POINT;
	}

/**
Adds 'EXDATE' property to aComponent from aEntry
@param aComponent component to add the EXDATE property to
@param aEntry entry to extract EXDATE information from
@internalTechnology
*/
void CAgnVersit2Exporter::AddExceptionDatePropertyL( CICalComponent& aComponent,
                                                     const CCalEntry& aEntry ) const
	{
	TRACE_ENTRY_POINT;
	
	RArray<TCalTime> exDates;
	CleanupClosePushL(exDates);
	aEntry.GetExceptionDatesL(exDates);
	const TInt count = exDates.Count();
	if (count)
		{
		CICalValue* val = CICalValue::NewLC();
		val->SetDateTimeL(exDates[0].TimeUtcL(), CICalValue::EUtcTime);
		CleanupStack::Pop(val);
		CICalProperty& prop = aComponent.AddPropertyL(KICalExdate, val); //Pass ownership

		for (TInt x = 1; x < count; ++x)
			{
			val = CICalValue::NewLC();
			val->SetDateTimeL(exDates[x].TimeUtcL(), CICalValue::EUtcTime);
			CleanupStack::Pop(val);
			prop.AddValueL(val); //Pass ownership
			}
		}
	CleanupStack::PopAndDestroy(&exDates);
	
	TRACE_EXIT_POINT;
	}

/**
Adds an integer value to a given component
@param aComponent component to add the integer value to
@param aInt integer to be added
@param aProperty current property
@internalTechnology
*/
void CAgnVersit2Exporter::AddIntegerPropertyL( CICalComponent& aComponent,
                                               TInt aInt,
                                               const TDesC& aProperty ) const
	{
	TRACE_ENTRY_POINT;
	
	CICalValue* val = CICalValue::NewLC();
	val->SetIntegerL(aInt);
	CleanupStack::Pop(val);
	aComponent.AddPropertyL(aProperty, val); //Pass ownership
	
	TRACE_EXIT_POINT;
	}

/**
Adds an 'ORGANIZER' property to aComponent from aEntry
@param aComponent component to add the ORGANIZER property to
@param aEntry entry to extract ORGANIZER information from
@internalTechnology
*/
void CAgnVersit2Exporter::AddOrganizerPropertyL( CICalComponent& aComponent,
                                                 const CCalEntry& aEntry ) const
	{
	TRACE_ENTRY_POINT;
	
	CCalUser* organizer = aEntry.OrganizerL();

	if (organizer)
		{
		/** Can't add the following property parameters due to no equivalence:
		*	(";" dirparam) /  (";" languageparam)
        */
		CICalValue* orgVal = CICalValue::NewLC();
		orgVal->SetTextL(organizer->Address());
		CleanupStack::Pop(orgVal);
		CICalProperty& orgProp = aComponent.AddPropertyL(KICalOrganizer, orgVal); //Pass ownership
		//Add 'CN' property parameter
		TPtrC commonName(organizer->CommonName());
		if (commonName.Length() > 0)
			{
			orgProp.AddPropertyParamL(KICalCn, commonName);
			}

		//Add "SENT-BY" property parameter
		TPtrC sentBy(organizer->SentBy());
		if (sentBy.Length() > 0)
			{
			orgProp.AddPropertyParamL(KICalSentBy, commonName);
			}
		}
	
	TRACE_EXIT_POINT;
	}

/**
Adds a text value to a given component
@param aComponent component to add the text value to
@param aText text to be added
@param aProperty current property
@internalTechnology
*/
void CAgnVersit2Exporter::AddTextPropertyL( CICalComponent& aComponent,
                                            const TDesC& aText,
                                            const TDesC& aProperty ) const
	{
	TRACE_ENTRY_POINT;
	
	if (aText.Length() > 0)
		{
		aComponent.AddPropertyL(aProperty, aText);
		}
	
	TRACE_EXIT_POINT;
	}

/**
Adds 'RDATE' property to aComponent from aEntry
@param aComponent component to add the RDATE property to
@param aEntry entry to extract RRULE information from
@internalTechnology
*/
void CAgnVersit2Exporter::AddRDatePropertyL( CICalComponent& aComponent,
                                             const CCalEntry& aEntry ) const
	{
	TRACE_ENTRY_POINT;
	
	RArray<TCalTime> rDates;
	CleanupClosePushL(rDates);
	aEntry.GetRDatesL(rDates);
	const TInt count = rDates.Count();
	if (count)
		{
		//Must create a property with a value
		CICalValue* val = CICalValue::NewLC();
		val->SetDateTimeL(rDates[0].TimeUtcL(), CICalValue::EUtcTime);
		CleanupStack::Pop(val);
		CICalProperty& prop = aComponent.AddPropertyL(KICalRdate, val); //Pass ownership

		for (TInt x = 1; x < count; ++x)
			{
			val = CICalValue::NewLC();
			val->SetDateTimeL(rDates[x].TimeUtcL(), CICalValue::EUtcTime);
			CleanupStack::Pop(val);
			prop.AddValueL(val);
			}
		}
	CleanupStack::PopAndDestroy(&rDates);
	
	TRACE_EXIT_POINT;
	}

/**
Adds 'EXDATE' property to aComponent from aEntry
@param aComponent component to add the EXDATE property to
@param aEntry entry to extract EXDATE information from
@internalTechnology
*/
void CAgnVersit2Exporter::AddRRulePropertyL( CICalComponent& aComponent,
                                             const CCalEntry& aEntry ) const
	{
	TRACE_ENTRY_POINT;
	
	TCalRRule rule;
	if (aEntry.GetRRuleL(rule))
		{
		HBufC* ruleVal = HBufC::NewLC(KInitialRRuleLength);
		TPtr ruleValPtr(ruleVal->Des());
		//Add 'FREQ'
		ruleValPtr.Append(KICalFreq);
		ruleValPtr.Append(KICalEquals);
		switch (rule.Type())
			{
			case TCalRRule::EDaily:
				ruleValPtr.Append(KICalDaily);
				break;
			case TCalRRule::EWeekly:
				ruleValPtr.Append(KICalWeekly);
				break;
			case TCalRRule::EMonthly:
				ruleValPtr.Append(KICalMonthly);
				break;
			case TCalRRule::EYearly:
				ruleValPtr.Append(KICalYearly);
				break;
			default:
				{
				User::Leave(KErrCorrupt);
				}
			}

		// Add 'UNTIL' or 'COUNT'.  Not both.
		// Try UNTIL first as the COUNT implementation has issues.
		TCalTime untilTime = rule.Until();

		if (untilTime.TimeUtcL() == TCalTime::MaxTime())
			{
			// "UNTIL" returns the maximum date that Agenda can handle, so let's assume that it
			// repeats "forever". In this case we do not export a value for either UNTIL or COUNT.
			// Do nothing...
			}
		else if (untilTime.TimeUtcL() != Time::NullTTime())
			{
			ruleValPtr.Append(KICalSemiColon);
			// Add the 'UNTIL'
			ruleValPtr.Append(KICalUntil);
			ruleValPtr.Append(KICalEquals);
			CICalValue* val = CICalValue::NewLC();
			// iCalendar spec states 'UNTIL' should be in UTC
			val->SetDateTimeL(untilTime.TimeUtcL(), CICalValue::EUtcTime);
			ruleValPtr.Append(val->TextL());
			CleanupStack::PopAndDestroy(val);
			}
		else
			{
			TInt countNum = rule.Count();

			if (countNum != 0)
				{
				ruleValPtr.Append(KICalSemiColon);
				ruleValPtr.Append(KICalCount);
				ruleValPtr.Append(KICalEquals);

				// APINOTE: COUNT seems to be divided by interval during aEntry.GetRRuleL call.
				// APINOTE: It also seems to be count left (from now) rather than count from DTSTART.
				// APINOTE: In either case UNTIL will always be valid with the current API, so COUNT will never be used.
				countNum *= rule.Interval();
				ruleValPtr.AppendNum(countNum);
				}

			// Else neither is present event repeats forever.
			}

		//Add 'INTERVAL' if present
		TInt interval = rule.Interval();

		if (interval > 1)	// "1" is the default, so don't bother exporting it.
			{
			ruleValPtr.Append(KICalSemiColon);
			ruleValPtr.Append(KICalInterval);
			ruleValPtr.Append(KICalEquals);
			ruleValPtr.AppendNum(rule.Interval());
			}

		//Add 'BYMONTHDAY' if present
		if(rule.Type() == TCalRRule::EMonthly)
			{
			RArray<TInt> monthDays;
			CleanupClosePushL(monthDays);
			rule.GetByMonthDayL(monthDays);
			TInt count = monthDays.Count();

			if (count > 0)
				{
				ruleValPtr.Append(KICalSemiColon);
				ruleValPtr.Append(KICalByMonthDay);
				ruleValPtr.Append(KICalEquals);

				for (TInt x = 0; x < count; ++x)
					{
					//Add 1 the month day ( 0 = 1).
					ruleValPtr.AppendNum( monthDays[x]  + 1 );
					if (x < count - 1)
						{
						ruleValPtr.Append(KICalComma);
						}
					}

				}
			CleanupStack::PopAndDestroy(&monthDays);
			}

		//Add 'WKST'.  Agenda only supports 'WKST' if 'FREQ=WEEKLY'
		if (rule.Type() == TCalRRule::EWeekly)
			{
			ruleValPtr.Append(KICalSemiColon);
			ruleValPtr.Append(KICalWkSt);
			ruleValPtr.Append(KICalEquals);
			ruleValPtr.Append(DayFromTDay(rule.WkSt()));
			}


		//Add 'BYDAY'
		if ( rule.Type() == TCalRRule::EYearly  ||
		     rule.Type() == TCalRRule::EMonthly ||
		     rule.Type() == TCalRRule::EWeekly )
			{
			RArray<TDay> days;
			CleanupClosePushL(days);
			RArray<TCalRRule::TDayOfMonth> daysOfMonth;
			CleanupClosePushL(daysOfMonth);
			rule.GetByDayL(days);
			rule.GetByDayL(daysOfMonth);
			TInt daysCount = days.Count();
			TInt daysOfMonthCount = daysOfMonth.Count();

			if (daysCount > 0 || daysOfMonthCount > 0)
				{
				ruleValPtr.Append(KICalSemiColon);
				ruleValPtr.Append(KICalByDay);
				ruleValPtr.Append(KICalEquals);
				}

			if (daysCount > 0)
				{
				for (TInt x = 0; x < daysCount; ++x)
					{
					ruleValPtr.Append(DayFromTDay(days[x]));
					if (x < daysCount - 1)
						{
						ruleValPtr.Append(KICalComma);
						}
					}
				}
			if (daysOfMonthCount > 0)
				{
				for (TInt x = 0; x < daysOfMonthCount; ++x)
					{
					ruleValPtr.AppendNum(daysOfMonth[x].WeekInMonth());
					ruleValPtr.Append(DayFromTDay(daysOfMonth[x].Day()));
					if (x < daysOfMonthCount - 1)
						{
						ruleValPtr.Append(KICalComma);
						}
					}
				}
			CleanupStack::PopAndDestroy(&daysOfMonth);
			CleanupStack::PopAndDestroy(&days);
			}


		//Add 'BYMONTH'. Agenda only supports 'BYMONTH' if 'FREQ=YEARLY'
		if (rule.Type() == TCalRRule::EYearly)
			{
			RArray<TMonth> theMonths;
			CleanupClosePushL(theMonths);
			rule.GetByMonthL(theMonths);

			TInt monthsCount = theMonths.Count();

			if (monthsCount > 0)
				{
				ruleValPtr.Append(KICalSemiColon);
				ruleValPtr.Append(KICalByMonth);
				ruleValPtr.Append(KICalEquals);

				for (int i = 0; i < monthsCount; i++)
					{
					TInt monthNum = (TInt)(theMonths[i] + 1);	// TMonth is offset from 0
					ruleValPtr.AppendNum(monthNum);
					if (i < monthsCount - 1)
						{
						ruleValPtr.Append(KICalComma);
						}
					}
				}
			CleanupStack::PopAndDestroy(&theMonths);
			}

		CICalValue* val = CICalValue::NewLC();
		val->SetTextL(ruleValPtr);
		CleanupStack::Pop(val);
		CICalProperty& prop = aComponent.AddPropertyL(KICalRRule,val); //Pass ownership

		CleanupStack::PopAndDestroy(ruleVal);
		
		TRACE_EXIT_POINT;
		}
	}

/**
Adds a date-time property to the passed component. Outputs in Local time if
possible, UTC otherwise.
@param aComponent component to add the property to.
@param aUtcTime time in UTC.
@param aProperty property to add.
@return property resulting from adding the date-time.
@internalTechnology
*/
CICalProperty& CAgnVersit2Exporter::AddDateTimePropertyL(CICalComponent& aComponent,
	const TCalTime& aTime, const TDesC& aProperty) const
	{
	TRACE_ENTRY_POINT;
	
	// Check if we have a local time zone name - output in this timezone if we do
	if ((iTimezoneIndex != KErrNotFound) && (iTimezoneIndex < iTzNameArray.Count()))
		{
		CICalValue* val = CICalValue::NewLC();
		TTime timeCopy(aTime.TimeUtcL());
		ASSERT(iTimeZoneArray.Count() >= iTimezoneIndex + 1);
		iTimeZoneArray[iTimezoneIndex]->ConvertToLocalL(timeCopy);
		val->SetDateTimeL(timeCopy, CICalValue::ESpecifiedTimeZone);
		CleanupStack::Pop(val);
		CICalProperty& prop = aComponent.AddPropertyL(aProperty, val); //pass ownership of val
		prop.AddPropertyParamL(KICalTzid,*iTzNameArray[iTimezoneIndex]);
		
		TRACE_EXIT_POINT;
		return prop;
		}
	else if(aTime.TimeMode() == TCalTime::EFloating)
		{
		TRACE_EXIT_POINT;
		// If the CalTime was a floating time when it was created, output in floating time.
		return AddFloatingDateTimePropertyL(aComponent, aTime.TimeLocalL(), aProperty);
		}
	else
		{
		TRACE_EXIT_POINT;
		// Otherwise output in UTC
		return AddUtcDateTimePropertyL(aComponent, aTime.TimeUtcL(), aProperty);
		}
	}

/**
Adds a date-time property to the passed component. Forces UTC.
@param aComponent component to add the property to.
@param aUtcTime time in UTC.
@param aProperty property to add.
@return property resulting from adding the date-time.
@internalTechnology
*/
CICalProperty& CAgnVersit2Exporter::AddUtcDateTimePropertyL(CICalComponent& aComponent,
	const TTime& aUtcTime, const TDesC& aProperty) const
	{
	TRACE_ENTRY_POINT;
	
	CICalValue* val = CICalValue::NewLC();
	val->SetDateTimeL(aUtcTime,CICalValue::EUtcTime);
	CleanupStack::Pop(val);
	CICalProperty& prop = aComponent.AddPropertyL(aProperty, val); //pass ownership of val
	
	TRACE_EXIT_POINT;
	return prop;
	}

/**
Adds a date-time property to the passed component. Forces floating.
@param aComponent component to add the property to.
@param aUtcTime time in floating time.
@param aProperty property to add.
@return property resulting from adding the date-time.
@internalTechnology
*/
CICalProperty& CAgnVersit2Exporter::AddFloatingDateTimePropertyL(CICalComponent& aComponent,
	const TTime& aFloatingTime, const TDesC& aProperty) const
	{
	TRACE_ENTRY_POINT;
	
	CICalValue* val = CICalValue::NewLC();
	val->SetDateTimeL(aFloatingTime,CICalValue::EFloatingTime);
	CleanupStack::Pop(val);
	CICalProperty& prop = aComponent.AddPropertyL(aProperty, val); //pass ownership of val
	
	TRACE_EXIT_POINT;
	return prop;
	}


//
//     HELPER FUNCTIONS
//     ================
//

/**
Helper function to convert between a CCalEntry::TReplicationStatus stored in
agenda and a spec-compliant descriptor
@param aStatus agenda representation of CLASS
@return Descriptor representation of CLASS
*/
const TDesC& CAgnVersit2Exporter::ClassStringL(const CCalEntry::TReplicationStatus aStatus)
	{
	TRACE_ENTRY_POINT;
	
	switch (aStatus)
		{
		case CCalEntry::EOpen :
		    TRACE_EXIT_POINT;
			return KICalPublic;
		case CCalEntry::EPrivate :
		    TRACE_EXIT_POINT;
			return KICalPrivate;
		case CCalEntry::ERestricted :
		    TRACE_EXIT_POINT;
			return KICalConfidential;
		default :
			User::Leave(KErrCorrupt);
			break;
		}

    TRACE_EXIT_POINT;	    
	return KNullDesC;
	}

/**
Helper function to convert between a category stored in agenda and a
spec-compliant descriptor
@param aCategory category to extract data from
@return Descriptor representation of category
@internalTechnology
*/
const TDesC& CAgnVersit2Exporter::CategoryStringL(const CCalCategory& aCategory) const
	{
	TRACE_ENTRY_POINT;
	
	switch (aCategory.Category())
		{
		case CCalCategory::ECalAppointment :
		    TRACE_EXIT_POINT;
			return iStringProvider.StringL(EICalAppointment);
		case CCalCategory::ECalBusiness :
		    TRACE_EXIT_POINT;
			return iStringProvider.StringL(EICalBusiness);
		case CCalCategory::ECalEducation :
		    TRACE_EXIT_POINT;
			return iStringProvider.StringL(EICalEducation);
		case CCalCategory::ECalHoliday :
		    TRACE_EXIT_POINT;   
			return iStringProvider.StringL(EICalHoliday);
		case CCalCategory::ECalMeeting :
		    TRACE_EXIT_POINT;
			return iStringProvider.StringL(EICalMeeting);
		case CCalCategory::ECalMiscellaneous :
		    TRACE_EXIT_POINT;
			return iStringProvider.StringL(EICalMisc);
		case CCalCategory::ECalPersonal :
		    TRACE_EXIT_POINT;
			return iStringProvider.StringL(EICalPersonal);
		case CCalCategory::ECalPhoneCall :
		    TRACE_EXIT_POINT;
			return iStringProvider.StringL(EICalPhoneCall);
		case CCalCategory::ECalSickDay :
		    TRACE_EXIT_POINT;
			return iStringProvider.StringL(EICalSick);
		case CCalCategory::ECalSpecialOccasion :
		    TRACE_EXIT_POINT;
			return iStringProvider.StringL(EICalSpecial);
		case CCalCategory::ECalTravel :
		    TRACE_EXIT_POINT;
			return iStringProvider.StringL(EICalTravel);
		case CCalCategory::ECalVacation :
		    TRACE_EXIT_POINT;
			return iStringProvider.StringL(EICalVacation);
		case CCalCategory::ECalExtended :
		    TRACE_EXIT_POINT;
			return aCategory.ExtendedCategoryName();
		default :
			User::Leave(KErrCorrupt);
			break;
		}
    
    TRACE_EXIT_POINT;
	return KNullDesC;
	}

/**
Converts from a TCalRole defined in CCalAttendee into a spec-compliant
descriptor.
@param aRole TCalRole enumeration to convert from.
@return Descriptor containing the equivalent text.
@internalTechnology
*/
const TDesC& CAgnVersit2Exporter::RoleFromEnum(CCalAttendee::TCalRole aRole)
	{
	TRACE_ENTRY_POINT;
	
	switch (aRole)
		{
		case CCalAttendee::EReqParticipant :
		    TRACE_EXIT_POINT;
			return KICalReqParticipant;
		case CCalAttendee::EOptParticipant :
		    TRACE_EXIT_POINT;
			return KICalOptParticipant;
		case CCalAttendee::ENonParticipant :
		    TRACE_EXIT_POINT;
			return KICalNonParticipant;
		case CCalAttendee::EChair :
		    TRACE_EXIT_POINT;
			return KICalChair;
		default :
			break;
		}
    
    TRACE_EXIT_POINT;
	return KNullDesC;
	}

/**
Converts from a TCalStatus defined in CCalAttendee into a spec-compliant descriptor.
@param aStatus TCalStatus enumeration to convert from.
@return Descriptor containing the equivalent text.
@internalTechnology
*/
const TDesC& CAgnVersit2Exporter::StatusFromEnum(CCalAttendee::TCalStatus aStatus)
	{
	TRACE_ENTRY_POINT;
	
	switch (aStatus)
		{
		case CCalAttendee::EAccepted :
		    TRACE_EXIT_POINT;
			return KICalAccepted;
		case CCalAttendee::ETentative :
		    TRACE_EXIT_POINT;
			return KICalTentative;
		case CCalAttendee::EConfirmed :
		    TRACE_EXIT_POINT;
			return KICalConfirmed;
		case CCalAttendee::EDeclined :
		    TRACE_EXIT_POINT;
			return KICalDeclined;
		case CCalAttendee::ECompleted :
		    TRACE_EXIT_POINT;
			return KICalCompleted;
		case CCalAttendee::EDelegated :
		    TRACE_EXIT_POINT;
			return KICalDelegated;
		case CCalAttendee::EInProcess :
		    TRACE_EXIT_POINT;
			return KICalInProcess;
		case CCalAttendee::ENeedsAction :
		// Needs action is the default so fall through and set a null descriptor
		default :
			break;
		}
    
    TRACE_EXIT_POINT;
	return KNullDesC;
	}

/**
Helper function to convert from a TUint8 as defined in vtzrules.h into a
spec-compliant descriptor, such as MO, TU etc.
@param aDayInt Day number to be converted.
@return Descriptor containing the converted day.
@internalTechnology
*/
const TDesC& CAgnVersit2Exporter::DayFromInt(TUint8 aDayInt)
	{
	TRACE_ENTRY_POINT;
	
	switch (aDayInt)
		{
		case 0 :
		    TRACE_EXIT_POINT;
			return KICalMonday;
		case 1 :
		    TRACE_EXIT_POINT;
			return KICalTuesday;
		case 2 :
		    TRACE_EXIT_POINT;
			return KICalWednesday;
		case 3 :
		    TRACE_EXIT_POINT;
			return KICalThursday;
		case 4 :
		    TRACE_EXIT_POINT;
			return KICalFriday;
		case 5 :
		    TRACE_EXIT_POINT;
			return KICalSaturday;
		case 6 :
		    TRACE_EXIT_POINT;
			return KICalSunday;
		default :
			break;
		}
    
    TRACE_EXIT_POINT;
	return KNullDesC;
	}

/**
Helper function to convert from a TDay into a spec-compliant descriptor, such
as MO, TU etc.
@param aDay TDay to be converted.
@return Descriptor containing the converted day.
@internalTechnology
*/
const TDesC& CAgnVersit2Exporter::DayFromTDay(TDay aDay)
	{
	TRACE_ENTRY_POINT;
	
	switch (aDay)
		{
		case EMonday :
		    TRACE_EXIT_POINT;
			return KICalMonday;
		case ETuesday :
		    TRACE_EXIT_POINT;
			return KICalTuesday;
		case EWednesday :
		    TRACE_EXIT_POINT;
			return KICalWednesday;
		case EThursday :
		    TRACE_EXIT_POINT;
			return KICalThursday;
		case EFriday :
		    TRACE_EXIT_POINT;
			return KICalFriday;
		case ESaturday :
		    TRACE_EXIT_POINT;
			return KICalSaturday;
		case ESunday :
		    TRACE_EXIT_POINT;
			return KICalSunday;
		default :
			break;
		}
	
	TRACE_EXIT_POINT;
	return KNullDesC;
	}

TInt CAgnVersit2Exporter::WeekNumFromDayOfMonth(TInt aDayOfMonth)
	{
	TRACE_ENTRY_POINT;
	
	TRACE_EXIT_POINT;
	return ((aDayOfMonth - 1) / 7) + 1;
	}

//End of file.
