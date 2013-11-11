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

#include <s32mem.h>
#include <vtzrules.h>
#include <utf.h>
#include <tz.h>

#include <calentry.h>
#include <calcontent.h>
#include <calrrule.h>
#include <caluser.h>
#include <calalarm.h>
#include <calattachment.h>

#include "agmvcal.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <vpropbinaryfile.h>
#endif


/** Specifies number of years backward from current year for which DAYLIGHT properties are exported.*/
const TInt KDaylightPropertyYearsBackwards(2);

/** Specifies number of years forward from current year, or the repeat start year, for which DAYLIGHT properties are exported.*/
const TInt KDaylightPropertyYearsForwards(5);


// Utility method to ensure proper cleanup in OOM 
//
void CAgendaEntryToVCalConverter::ResetAndDestroyArrayOfParams(TAny* aObject)
	{
	CArrayPtr<CParserParam>* array=reinterpret_cast<CArrayPtr<CParserParam>*>(aObject);
	if (array)
		{
		array->ResetAndDestroy();
		}
	delete array;
	}

void CAgendaEntryToVCalConverter::ResetAndDestroyArrayOfVersitDateTime(TAny* aObject)
	{
	CArrayPtr<TVersitDateTime>* array=reinterpret_cast<CArrayPtr<TVersitDateTime>*>(aObject);
	if (array)
		{
		array->ResetAndDestroy();
		}
	delete array;
	}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	Public
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// Export agenda entry aEntry to the given parser
//
void CAgendaEntryToVCalConverter::ExportEntryL(CCalEntry* aEntry, CVersitParser& aParser)
	{
	CParserVCalEntity* entity = CParserVCalEntity::NewL();
	CleanupStack::PushL(entity);

	if (aEntry->EntryTypeL() == CCalEntry::ETodo)
		{
		entity->SetEntityNameL(KVersitVarTokenVTODO);
		}
	else
		{
		entity->SetEntityNameL(KVersitVarTokenVEVENT);
		}

	if ((aEntry->StartTimeL()).TimeMode() == TCalTime::EFloating)
		{
		iTimeType = TVersitDateTime::EIsMachineLocal;
		iTimeFlag = TVersitDateTime::EExportLeaveAsLocalTime;
		}
	else 
		{
		iTimeType=TVersitDateTime::EIsUTC;
		iTimeFlag=TVersitDateTime::EExportTimeZoneDesignator;
		}

	AddEntryPropertiesL(aEntry, *entity);
	CleanupStack::Pop(); // entity
	aParser.AddEntityL(entity); // takes ownership
	}

CAgendaEntryToVCalConverter::~CAgendaEntryToVCalConverter()
	{
	}
	

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	Private
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
/** AddTzPropertyL()
 *
 *	Adds TZ property to aParser.
 *
 *	@param aParser		- a vCal entity or sub-entity (VEVENT or VTODO);
 *	@param aStdOffset	- standard time zone offset from UTC in seconds.
 */
void CAgendaEntryToVCalConverter::AddTzPropertyL(CVersitParser& aParser, 
												 TTimeIntervalSeconds aStdOffsetInSeconds)
	{
	if (!iAddedTzProperty)
		{
		CParserPropertyValue* vValue = new (ELeave) CParserPropertyValueTimeZone(aStdOffsetInSeconds);
		CleanupStack::PushL(vValue);

		CParserProperty* vProp = CParserProperty::NewL(*vValue, KVersitTokenTZ, NULL); 
		CleanupStack::Pop(vValue);

		aParser.AddPropertyL(vProp);
		}
	iAddedTzProperty = ETrue;
	}


/** MapToRelativeTime()
 *
 *	Provides mapping between iTimeReference of TVTzActualisedRule and iRelativeTime of TVersitDateTime. 
 *
 *	CTzRules::AddActualisedRulesL() guarantees that TVTzActualisedRule is always UTC or 
 *	local(ETzWallTimeReference), and is never STD. The func maps everything to 
 *	TVersitDateTime::EIsMachineLocal, unless iTimeReference is ETzUtcTimeReference, which is mapped
 *  to TVersitDateTime::EIsUTC.
 *
 *	This is local helper function.
 *	Used exclusively by CAgendaEntryToVCalConverter::AddTimeZonePropertiesL() to export DAYLIGHT properties.
 */
static inline TVersitDateTime::TRelativeTime MapToRelativeTime(TTzTimeReference& aTzTimeRef)
	{
	return (aTzTimeRef==ETzUtcTimeReference) ? (TVersitDateTime::EIsUTC) : (TVersitDateTime::EIsMachineLocal);
	}

/*	AddDaylightPropertyL()
 *
 *	Export a DAYLIGHT property to describe the daylight saving period and its offset.
 *	There can be more than one DAYLIGHT property for a year (depenging on number of DST rules for the year).  
 *	Time designators (i.e EST, EDT) are not exported.
 *
 *	@param aParser		- a vCal entity or sub-entity (VEVENT or VTODO);
 *	@param aOffsetInSeconds	 - offset in seconds from UTC (i.e. STD + DST) for the period when the 
 					 	  daylight saving rule is in effect, 
 						  for instance, for time zone "Vancouver/America (PST)" aOffsetInSeconds is
 						  -480*60=-28800 secs in winter time, and -420*60=-25200 secs when DST is observed; 
 *	@param aStartTime	- exact time when a rule starts (usually 02:00:00 local time);
 *	@param aEndTime		- time when rule ends (usually 03:00:00 local time).
 */
void CAgendaEntryToVCalConverter::AddDaylightPropertyL(CVersitParser& aParser, 
													   TTimeIntervalSeconds aOffsetInSeconds,
													   TTimeWithReference aStartTime,
													   TTimeWithReference aEndTime)
	{
	TVersitDateTime* vStartTime = new(ELeave) TVersitDateTime(aStartTime.iTime.DateTime(),
															  MapToRelativeTime(aStartTime.iTimeReference));
	CleanupStack::PushL(vStartTime);

	TVersitDateTime* vEndTime = new (ELeave) TVersitDateTime(aEndTime.iTime.DateTime(),
															  MapToRelativeTime(aEndTime.iTimeReference));
	CleanupStack::PushL(vEndTime);

	CVersitDaylight* vDaylight = CVersitDaylight::NewL(ETrue, 
													  aOffsetInSeconds, 
													  vStartTime, 
													  vEndTime, 
													  KNullDesC(), 
													  KNullDesC());
	
	CleanupStack::Pop(vEndTime);   // CVersitDaylight takes ownership.
	CleanupStack::Pop(vStartTime); // CVersitDaylight takes ownership.

	CleanupStack::PushL(vDaylight);
	CParserPropertyValue* vValue = new (ELeave) CParserPropertyValueDaylight(vDaylight);
	CleanupStack::Pop(vDaylight);

	CleanupStack::PushL(vValue);
	CParserProperty* vProp = CParserProperty::NewL(*vValue, KVersitTokenDAYLIGHT, NULL); 
	CleanupStack::Pop(vValue);

	aParser.AddPropertyL(vProp);
	}


/**	Adds DAYLIGHT properties for a range of years from aStartYear to aEndYear inclusive to 
 *	a vCalendar entity (aParser).
 *	
 *	@param aParser		- a vCalendar object where properties added
 *	@param aRules		- time zone rules used to retrieve DST information
 *	@param aRepeatStartTime		- start of entry range, including the repeat rule
 *	@param aRepeatEndTime		- end of entry range, including the repeat rule
 *
 */
void CAgendaEntryToVCalConverter::AddDaylightPropertiesL(CVersitParser& aParser, const CTzRules* aRules, TInt aStdOffset, TTime aRepeatStartTime, TTime aRepeatEndTime)
	{
	const TInt KRepeatStartYear = aRepeatStartTime.DateTime().Year();
	const TInt KRepeatEndYear = aRepeatEndTime.DateTime().Year();
	
 	// actualise the rules starting a year before our period of interest
	CVTzActualisedRules* actRules = CVTzActualisedRules::NewL(KRepeatStartYear - 1, KRepeatEndYear + 1);
	
	if (actRules)
		{
		CleanupStack::PushL(actRules);
		aRules->GetActualisedRulesL(*actRules);
		
		TTime now;
		now.HomeTime();
		const TInt KThisYear = now.DateTime().Year();
		
		// Set up the time range for exported DAYLIGHT properties
		const TInt KRangeBeginYear(Max(KThisYear - KDaylightPropertyYearsBackwards, KRepeatStartYear));
		const TInt KRangeEndYear(Min(Max(KThisYear + KDaylightPropertyYearsForwards, KRepeatStartYear + KDaylightPropertyYearsForwards), KRepeatEndYear + 1));
		const TTime KRangeBeginTime(TDateTime(KRangeBeginYear, EJanuary, 0, 0, 0, 0, 0));
		const TTime KRangeEndTime(TDateTime(KRangeEndYear, EJanuary, 0, 0, 0, 0, 0));
		
		const TInt KRulesCount(actRules->Count());
		for (TInt i(0) ; i < KRulesCount - 1 ; ++i)
			{
			TVTzActualisedRule& actRule = (*actRules)[i];
			
			if (aStdOffset < actRule.iNewOffset)		// work with DST rules only
				{
				// End time of a rule is start time of the next rule
				TVTzActualisedRule& actRuleNext = (*actRules)[i+1];
				
				TTimeWithReference ruleStart(TTimeWithReference(actRule.iTimeOfChange, actRule.iTimeReference));
				TTimeWithReference ruleEnd(TTimeWithReference(actRuleNext.iTimeOfChange, actRuleNext.iTimeReference));
				
				TTime ruleStartTime = ruleStart.iTime;
				TTime ruleEndTime = ruleEnd.iTime;
				
				if (actRule.iTimeReference == ETzStdTimeReference)
					{
					ruleStartTime -= TTimeIntervalMinutes(aRules->InitialStdTimeOffset());
					}
				else if (actRule.iTimeReference == ETzWallTimeReference)
					{
					ruleStartTime -= TTimeIntervalMinutes(actRule.iNewOffset);
					}						

				if (actRule.iTimeReference == ETzStdTimeReference)
					{
					ruleEndTime -= TTimeIntervalMinutes(aRules->InitialStdTimeOffset());
					}
				else if (actRule.iTimeReference == ETzWallTimeReference)
					{
					ruleEndTime -= TTimeIntervalMinutes(actRule.iNewOffset);
					}	
				
				// Export rule if the first instance occours when dst is on
				// or if the rule covers the range defined above
				if ( (aRepeatStartTime >= ruleStartTime) && (aRepeatStartTime <= ruleEndTime) || (ruleEndTime >= KRangeBeginTime) && (ruleStartTime <= KRangeEndTime) )
					{
					// Export DAYLIGHT property for concrete DST rule
					AddDaylightPropertyL(aParser, actRule.iNewOffset * 60, ruleStart, ruleEnd);
					}
				}
			}
		CleanupStack::PopAndDestroy(actRules);
		}
	}

void CAgendaEntryToVCalConverter::NextVCalendarL()
	{
	iAddedTzProperty = EFalse;
	}

/**	CAgendaEntryToVCalConverter::AddTimeZonePropertiesL()
 *	 	
 *	Adds TZ and DAYLIGHT properties to a vCalendar entity. 
 *
 *	@param aEntry 	- an agenda entry which supplies timezone and tz rules information 
 *						(has to have a repeat definition, otherwise function will do nothing);
 *	@param aParser	- a vCalendar object to which the properties are added. 
 *	
 *	aParser can be 1) a vCalendar object (also referrred to as "vCalendar entity"), 
 *		as well as 2) a vEvent or vTodo objects (also referred to as "vCalendar sub-entities").
 *	
 *	TZ and DAYLIGHT properties (as per vCalendar v1.0 spec) apply to the vCalendar object as a whole; UNLESS 
 *	overriden by a property within the scope of an event or todo entity.
 *	
 *	@internalComponent
 */
void CAgendaEntryToVCalConverter::AddTimeZonePropertiesL(CVersitParser& aParser, CCalEntry* aEntry)
	{
	TCalRRule rrule;
	
  	if (aEntry->GetRRuleL(rrule) && aEntry->StartTimeL().TimeMode() != TCalTime::EFloating)
		{
		// Only export the TZ property when the entry is repeating.
		// Do not export TZ rules for floating entry.
		CTzRules* rules = aEntry->TzRulesL();
		
		if (rules)
			{
    		CleanupStack::PushL( rules );

            // find the standard offset for this time zone
            // which is the lowest offset from the rules for the entry's repeat range
            CVTzActualisedRules* actRules = CVTzActualisedRules::NewL(rrule.DtStart().TimeUtcL().DateTime().Year() - 1, rrule.Until().TimeUtcL().DateTime().Year() + 1);
        	CleanupStack::PushL(actRules);
        	rules->GetActualisedRulesL(*actRules);
        	
        	TInt stdOffset(KMaxTInt);
        	
        	const TInt KRulesCount = actRules->Count();
        	for (TInt i(0) ; i < KRulesCount ; ++i)
        		{
        		if ((*actRules)[i].iNewOffset < stdOffset)
        			{
        			stdOffset = (*actRules)[i].iNewOffset;
        			}
        		}
        	
        	if (stdOffset == KMaxTInt)
        		{
        		// there were no offsets found so use the current system utc offset
        		stdOffset = (User::UTCOffset().Int() / 60);
        		}
        	
        	CleanupStack::PopAndDestroy(actRules);
			AddTzPropertyL(aParser, stdOffset * 60);
			AddDaylightPropertiesL(aParser, rules, stdOffset, rrule.DtStart().TimeUtcL(), rrule.Until().TimeUtcL());
            
			CleanupStack::PopAndDestroy(rules);	
			}
		}
	}

// This function specifies the ordering of the vCalendar properties as exported by Versit.
//
// The UID property should be the first property. 
//
// The following three properties of a vCalendar should be DESCRIPTION, DTSTART and DTEND for appointments, 
// anniversaries & events. To-do entries should have DESCRIPTION and DUE as the following two properties.
// This is to assist processing by Connectivity (as requested by Time Technology)
//
void CAgendaEntryToVCalConverter::AddEntryPropertiesL(CCalEntry* aEntry, CVersitParser& aParser)
	{
	// UID 

	HBufC* guid16 = HBufC::NewLC(aEntry->UidL().Length());
	TPtr ptr16 = guid16->Des();
	CnvUtfConverter::ConvertToUnicodeFromUtf8(ptr16, aEntry->UidL());
	AddDesPropertyL(aParser, KVersitTokenUID, guid16->Des());
	CleanupStack::PopAndDestroy(guid16);
		
	// CAgnEntry::RichTextL()	=> SUMMARY 
	// CAgnEntry::NotesTextL()	=> DESCRIPTION 

	if (aEntry->DescriptionL().Length() > 0)
		{
		AddDesPropertyL(aParser, KVersitTokenDESCRIPTION, aEntry->DescriptionL());					
		}
		
	if (aEntry->SummaryL().Length() > 0)
		{
		AddDesPropertyL(aParser, KVersitTokenSUMMARY, aEntry->SummaryL());
		}

	// Add DTSTART and DTEND (or DUE, PRIORITY and todo list information for to-do items), then add X-EPOCAGENDAENTRYTYPE
	switch (aEntry->EntryTypeL())
		{
		case CCalEntry::EAppt:
			{
			AddEventPropertiesL(aParser, aEntry);
			AddDesPropertyL(aParser, KVCalToken8ENTRYTYPE, KVCalTokenTypeAPPT);	
			break;
			}
		case CCalEntry::EReminder:
			{
			AddEventPropertiesL(aParser, aEntry);
			AddDesPropertyL(aParser, KVCalToken8ENTRYTYPE, KVCalTokenTypeREMINDER);	
			break;
			}
		case CCalEntry::EAnniv:
			{
			AddEventPropertiesL(aParser, aEntry);
			AddDesPropertyL(aParser, KVCalToken8ENTRYTYPE, KVCalTokenTypeANNIV);
			break;
			}
		case CCalEntry::EEvent:
			{
			AddEventPropertiesL(aParser, aEntry);
			AddDesPropertyL(aParser, KVCalToken8ENTRYTYPE, KVCalTokenTypeEVENT);
			break;
			}
		case CCalEntry::ETodo:
			{
			AddTodoPropertiesL(aParser, aEntry);
			AddDesPropertyL(aParser, KVCalToken8ENTRYTYPE, KVCalTokenTypeTODO);
			break;
			}
		case CCalEntry::ENote:
			{
			AddDesPropertyL(aParser,KVCalToken8ENTRYTYPE, KVCalTokenTypeNOTE);
			break;
			}
		default: break;
		}

	// Class
	CCalEntry::TReplicationStatus status = aEntry->ReplicationStatusL();
	switch(status)
		{
		case CCalEntry::EOpen:
			AddDesPropertyL(aParser, KVersitTokenCLASS, KVCalTokenPUBLIC);
			break;
		case CCalEntry::EPrivate:
			AddDesPropertyL(aParser, KVersitTokenCLASS, KVCalTokenPRIVATE);
			break;
		case CCalEntry::ERestricted:
			AddDesPropertyL(aParser, KVersitTokenCLASS, KVCalTokenCONFIDENTIAL);
			break;
		}

	// Location
	const TDesC& location = aEntry->LocationL();
	if (location.Length() != 0)
		{
		AddDesPropertyL(aParser, KVersitTokenLOCATION, location);
		}

	// DTSTAMP
	TCalTime dTStamp = aEntry->DTStampL();

	if ( dTStamp.TimeUtcL() != Time::NullTTime() &&
		CCalEntry::ENote != aEntry->EntryTypeL() )
		{
		AddDateTimePropertyL(aParser, KVersitTokenXDTSTAMP, dTStamp.TimeUtcL(), TVersitDateTime::EIsUTC, iTimeFlag);
      	}
	
	// Sequence Number
	AddIntegerPropertyL(aParser, KVersitTokenSEQUENCE, aEntry->SequenceNumberL());

	//X-Method property
	CCalEntry::TMethod methodStatus = aEntry->MethodL();
	TPtrC methodStr;
	switch(methodStatus)
		{
		case  CCalEntry::EMethodNone:
			methodStr.Set(KVCalTokenMethodStatusENone);
			break;
		case CCalEntry::EMethodPublish:
			methodStr.Set(KVCalTokenMethodStatusEPublish);
			break;
		case CCalEntry::EMethodRequest:
			methodStr.Set(KVCalTokenMethodStatusERequest);
			break;
		case CCalEntry::EMethodReply:
			methodStr.Set(KVCalTokenMethodStatusEReply);
			break;
		case CCalEntry::EMethodAdd:
			methodStr.Set(KVCalTokenMethodStatusEAdd);
			break;
		case CCalEntry::EMethodCancel:
			methodStr.Set(KVCalTokenMethodStatusECancel);
			break;
		case CCalEntry::EMethodRefresh:
			methodStr.Set(KVCalTokenMethodStatusERefresh);
			break;
		case CCalEntry::EMethodCounter:
			methodStr.Set(KVCalTokenMethodStatusECounter);
			break;
		case CCalEntry::EMethodDeclineCounter:
			methodStr.Set(KVCalTokenMethodStatusEDeclineCounter);
			break;	
		default:
			User::Leave(KErrArgument);	
		}

	AddDesPropertyL(aParser, KVersitTokenXMETHOD, methodStr);
	// X-Recurrence-ID
	TCalTime recurrenceDate = aEntry->RecurrenceIdL();
	
	if (recurrenceDate.TimeUtcL() != Time::NullTTime())
		{
		TTime recId;
		if(iTimeType==TVersitDateTime::EIsUTC)
			{
			recId = recurrenceDate.TimeUtcL();
			}
		else
			{
			recId = recurrenceDate.TimeLocalL();
			}
		AddDateTimePropertyL(aParser, KVersitTokenXRECURRENCEID, recId, iTimeType, iTimeFlag);	
		}
		
	// Attendees
	RPointerArray<CCalAttendee> attendees = aEntry->AttendeesL();

	TInt count=attendees.Count();
	CCalUser* phoneowner=aEntry->PhoneOwnerL();
	for (TInt ii=0; ii<count; ii++)
		{
		if(phoneowner==attendees[ii])
			{
			AddAttendeePropertyL(aParser, attendees[ii],ETrue, EFalse);
			}
		else
			{
			AddAttendeePropertyL(aParser, attendees[ii],EFalse, EFalse);
			}
		
		}

	CCalUser* organizer=aEntry->OrganizerL();
	if(organizer)
		{
		if(phoneowner==organizer)
			{
			AddAttendeePropertyL(aParser, organizer,ETrue, ETrue);
			}
		else
			{
			AddAttendeePropertyL(aParser, organizer,EFalse, ETrue);
			}
		}

	// Recurrence details
	TCalRRule rule;
	TBool isrepeat = aEntry->GetRRuleL(rule);
	if (isrepeat)
		{
		CTzRules* repeatRule = aEntry->TzRulesL();
		CleanupStack::PushL(repeatRule);
		AddRepeatPropertiesL(aParser, rule, repeatRule);
		CleanupStack::PopAndDestroy(repeatRule);
		}
		
	// RDates
	RArray<TCalTime>rdates;
	CleanupClosePushL(rdates);
	aEntry->GetRDatesL(rdates);

	if (rdates.Count()>0)
		{
		AddRDatePropertyL(aParser, rdates);
		isrepeat = ETrue;
		}
		
	CleanupStack::PopAndDestroy(&rdates);

	if (isrepeat)
		{
		// Recurrence Exception details
		RArray<TCalTime> exceptions;
		CleanupClosePushL(exceptions);
		aEntry->GetExceptionDatesL(exceptions);

		if (exceptions.Count()>0)
			{
			AddRepeatExceptionPropertiesL(aParser, exceptions);
			}
		CleanupStack::PopAndDestroy(&exceptions);
		}
		
	// Alarm
	CCalAlarm* alarm = aEntry->AlarmL();
	if (alarm)
		{
		TCalTime alarmOrigin;
		CleanupStack::PushL(alarm);
		if(aEntry->EntryTypeL()==CCalEntry::ETodo && (aEntry->EndTimeL()).TimeUtcL()!=Time::NullTTime())
			{
			alarmOrigin = aEntry->EndTimeL();
			}
		else
			{
			alarmOrigin = aEntry->StartTimeL();
			}
		
		TTime alarmtime;
		if (aEntry->StartTimeL().TimeMode() != TCalTime::EFloating)
			{
			alarmtime = alarmOrigin.TimeUtcL() - TTimeIntervalMinutes(alarm->TimeOffset());
			}
		else
			{
			alarmtime = alarmOrigin.TimeLocalL() - TTimeIntervalMinutes(alarm->TimeOffset());
			}
		AddAlarmPropertyL(aParser, KVersitTokenAALARM, alarmtime, alarm->AlarmSoundNameL());
		

		CCalContent* alarmAction = alarm->AlarmAction();
		if (alarmAction != NULL)
			{
			AddExtendedAlarmPropertyL(aParser, KVersitTokenXALARM, *alarmAction);
			}
		CleanupStack::PopAndDestroy(alarm);
		}

	// Add Last Changed Date
	AddDateTimePropertyL(aParser, KVersitTokenLASTMODIFIED, (aEntry->LastModifiedDateL()).TimeUtcL(), TVersitDateTime::EIsUTC, iTimeFlag);

	// Category Support
	const RPointerArray<CCalCategory>& categories = aEntry->CategoryListL();

	if (categories.Count()>0)
		{
		AddCategoryPropertyL(aParser, categories);
		}

	// ADDITION FOR VERSIT COMPLIANCE 
	// Merged priorities into one place - ToDo priority was previously in AddTodoPropertiesL()
	TInt priority = aEntry->PriorityL();
	AddIntegerPropertyL(aParser, KVersitTokenPRIORITY, priority);
	if (aEntry->EntryTypeL() != CCalEntry::ETodo)
		{
		AddStatusPropertyL(aParser, aEntry);
		}
	AddIntegerPropertyL(aParser, KVersitTokenXLOCALUID, aEntry->LocalUidL());
	AddIntegerPropertyL(aParser, KVersitTokenTRANSP, aEntry->TimeTransparencyL());
	
	// Add GEO
	CCalGeoValue* geoValue = aEntry->GeoValueL();
	
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
			geoString.AppendFormat(KGeoStringFormat,&geoLatString,KVersitTokenCommaVal,&geoLongString);
			
			// Add the GEO property
			AddDesPropertyL(aParser,KVersitTokenGEO,geoString);
			}
		
		CleanupStack::PopAndDestroy(geoValue);
		}
		
	TUint attachCount = aEntry->AttachmentCountL();
	for(TInt ii = 0; ii<attachCount; ++ii)
		{
		CCalAttachment* attach = aEntry->AttachmentL(ii);
		AddAttachmentPropertyL(aParser, *attach);
		}
		
	// Add the X-FAVOURITE property. Versit only exports signed ints but
	// can't handle the '-' sign when importing. Therefore it must be exported
	// as a string to ensure there is no '-' sign. When importing the value
	// would incorrectly become 0.
	//AddIntegerPropertyL( aParser, KVCalTokenXFavourite, aEntry->FavouriteL());
	TBuf<10> favouriteString;  
	TUint32 favourite = aEntry->FavouriteL();
	favouriteString.Num( favourite );
	AddDesPropertyL( aParser, KVCalTokenXFavourite, favouriteString );
	}


// Add appointment properties to parser
//
void CAgendaEntryToVCalConverter::AddEventPropertiesL(CVersitParser& aParser, CCalEntry* aEntry)
	{
	//If this is an untimed appt the start time will be the default start time
 	//If the alarm time is *after* this time we need to move the start time to the alarm time
	TTime startTime; 
	TTime endTime; 
	if((aEntry->StartTimeL()).TimeMode() == TCalTime::EFloating )
		{
		startTime = (aEntry->StartTimeL()).TimeLocalL();
		if(aEntry->EntryTypeL()==CCalEntry::EReminder)
			{
			endTime = startTime;
			}
		else
			{
			endTime =(aEntry->EndTimeL()).TimeLocalL();
			}

		}
	else 	
		{
		startTime = (aEntry->StartTimeL()).TimeUtcL();
		if(aEntry->EntryTypeL()==CCalEntry::EReminder)
			{
			endTime = startTime;
			}
		else
			{
			endTime = (aEntry->EndTimeL()).TimeUtcL();
			}

		}
	
	iStartTime=startTime.DateTime();


	AddDateTimePropertyL(aParser, KVersitTokenDTSTART, startTime, iTimeType, iTimeFlag);
	AddDateTimePropertyL(aParser, KVersitTokenDTEND, endTime, iTimeType, iTimeFlag);
	}

// Add todo properties
//
void CAgendaEntryToVCalConverter::AddTodoPropertiesL(CVersitParser& aParser, CCalEntry* aEntry)
	{
	TBool isUtc = ETrue;
	if ((aEntry->EndTimeL()).TimeUtcL() != Time::NullTTime())
		{
		if ((aEntry->StartTimeL()).TimeMode() == TCalTime::EFloating)
			{
			isUtc = EFalse;
			AddDateTimePropertyL(aParser, KVersitTokenDUE, (aEntry->EndTimeL()).TimeLocalL(), iTimeType, iTimeFlag);
			}
		else
			{
			AddDateTimePropertyL(aParser, KVersitTokenDUE, (aEntry->EndTimeL()).TimeUtcL(), iTimeType, iTimeFlag);
			}
		}

	// Add todo name list is now done with rest of category implementation	
	TTime startDate; 
	
	if (isUtc)
		{
		startDate = aEntry->StartTimeL().TimeUtcL();
		}
	else
		{
		startDate = aEntry->StartTimeL().TimeLocalL();
		}
	iStartTime=startDate.DateTime();
	
	// We have already checked the start and end times so we do not need 
	// further checks here. After discussion with MV.
	if (startDate != Time::NullTTime())
		{
		AddDateTimePropertyL(aParser, KVersitTokenDTSTART, startDate, iTimeType, iTimeFlag);
		}

	// Is the entry completed - if so export completed date/time and COMPLETED status
	if ((aEntry->CompletedTimeL()).TimeUtcL() != Time::NullTTime())
		{
		AddDateTimePropertyL(aParser,KVCalTokenCOMPLETED, (aEntry->CompletedTimeL()).TimeUtcL(), TVersitDateTime::EIsUTC, iTimeFlag);
		AddDesPropertyL(aParser, KVCalTokenSTATUS, KVCalStatusCOMPLETED);
		}
	else
		{
		if (aEntry->StatusL()==CCalEntry::ETodoCompleted)
			{
			// If entry status is COMPLETED, ensure COMPLETED date is exported too
			if ((aEntry->EndTimeL()).TimeUtcL()!= Time::NullTTime())
				{
				aEntry->SetCompletedL(ETrue,aEntry->EndTimeL());
				}
			else
				{
				//No valid due date, set the completed date to the today's date
				TTime today;
				today.UniversalTime();
				TCalTime caltime;
				caltime.SetTimeUtcL(today);
				aEntry->SetCompletedL(ETrue, caltime);
				}
			AddDateTimePropertyL(aParser,KVCalTokenCOMPLETED, (aEntry->CompletedTimeL()).TimeUtcL(), TVersitDateTime::EIsUTC, iTimeFlag);
			AddDesPropertyL(aParser, KVCalTokenSTATUS, KVCalStatusCOMPLETED);
			}
		else
			{
			// Entry is not completed and doesn't have status==Completed
			// So, add status property as normal
			AddStatusPropertyL(aParser,aEntry);
			}

		}
		
	}

// Construct an integer property
//
void CAgendaEntryToVCalConverter::AddIntegerPropertyL(CVersitParser& aParser, const TDesC8& aToken, TCalLocalUid aInt)
	{
	CParserPropertyValue* value = new(ELeave)CParserPropertyValueInt(aInt);
	CleanupStack::PushL(value);
	CParserProperty* prop = CParserProperty::NewL(*value, aToken, NULL); 
	CleanupStack::Pop(value);	
	// Add to the parser
	aParser.AddPropertyL(prop);
	}

void CAgendaEntryToVCalConverter::AddDateTimePropertyL(CVersitParser& aParser, const TDesC8& aToken, const TTime& aTime, const TVersitDateTime::TRelativeTime aRelativeTime, TVersitDateTime::TVersitDateTimeFlags aTimeFlag)
	{
	// Construct a date-time property
	TVersitDateTime* versitDateTime = new(ELeave)TVersitDateTime(aTime.DateTime(), aRelativeTime);
	CleanupStack::PushL(versitDateTime);
	CParserPropertyValue* value = new(ELeave)CParserPropertyValueDateTime(versitDateTime);
	CleanupStack::Pop(versitDateTime);
	versitDateTime->SetFlag(aTimeFlag);
	
	CleanupStack::PushL(value);
	CParserProperty* prop = CParserProperty::NewL(*value, aToken, NULL);
	CleanupStack::Pop(value);
	// Add to the parser
	aParser.AddPropertyL(prop);

	}

void CAgendaEntryToVCalConverter::AddDesPropertyL(CVersitParser& aParser, const TDesC8& aToken, const TDesC& aDes)
	{
	HBufC* buf=AgnConvertUtil::EncodeL(aDes,KUidEtextToText);
	CleanupStack::PushL(buf);
	CParserPropertyValue* value = CParserPropertyValueHBufC::NewL(buf->Des());
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PushL(value);
	CParserProperty* prop = CParserProperty::NewL(*value, aToken, NULL);
	CleanupStack::Pop(value);
	// Add to the parser
	aParser.AddPropertyL(prop);
	}

void CAgendaEntryToVCalConverter::AddAlarmPropertyL(CVersitParser& aParser, const TDesC8& aToken, const TTime& aAlarmTime, const TDesC& aAlarmName)
	{
	// Create an array of paramaters for this property
	CArrayPtr<CParserParam>* arrayOfParams = new(ELeave)CArrayPtrFlat<CParserParam>(4);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfParams,arrayOfParams));

	CParserParam* typeParam = CParserParam::NewL(KVCalToken8TYPE, KNullDesC8);
	CleanupStack::PushL(typeParam);
	typeParam->SetValueL(KVCalValue8EPOCSOUND);
	arrayOfParams->AppendL(typeParam); // takes ownership
	CleanupStack::Pop(typeParam);

	// Construct a date-time property
	
	TVersitDateTime* versitDateTime = new(ELeave)TVersitDateTime(aAlarmTime.DateTime(), iTimeType);
	CleanupStack::PushL(versitDateTime);

	CVersitAlarm* alarm = CVersitAlarm::NewL(versitDateTime, NULL, 0, aAlarmName, TPtrC());
	CleanupStack::Pop(versitDateTime);
	versitDateTime->SetFlag(iTimeFlag);
	CleanupStack::PushL(alarm);
	CParserPropertyValue* value = new(ELeave)CParserPropertyValueAlarm(alarm);
	CleanupStack::Pop(alarm);
	CleanupStack::PushL(value);

	CParserProperty* prop = CParserProperty::NewL(*value, aToken, arrayOfParams);
	CleanupStack::Pop(2,arrayOfParams); // value, arrayOfParams

	// Add to the parser
	aParser.AddPropertyL(prop);
	}

void CAgendaEntryToVCalConverter::AddExtendedAlarmPropertyL(CVersitParser& aParser, const TDesC8& aToken, const CCalContent& aAlarmAction)
	{
	// Create an array of parameters for this property
	CArrayPtr<CParserParam>* arrayOfParams = new(ELeave)CArrayPtrFlat<CParserParam>(4);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfParams,arrayOfParams));
	
	// Create content disposition parameter
	CParserParam* dispositionParam = CParserParam::NewL(KVersitTokenVALUE, KNullDesC8);
	CleanupStack::PushL(dispositionParam);
	CCalContent::TDisposition disposition = aAlarmAction.Disposition();
	switch (disposition)
		{
		case CCalContent::EDispositionUrl:
			{
			dispositionParam->SetValueL(KVCalContentValueUrl);
			break;
			}
		case CCalContent::EDispositionInline:
			{
			dispositionParam->SetValueL(KVCalContentValueInline);
			break;
			}
		default:
			{
			// In the default case, the property is unsupported. So we'll clean up
			// and then return
			CleanupStack::PopAndDestroy(2); //arrayOfParams, dispositionParam
			return;
			}
		}
	arrayOfParams->AppendL(dispositionParam); // takes ownership
	CleanupStack::Pop(dispositionParam);

	// Create content MIME type parameter
	CParserParam* mimeTypeParam = CParserParam::NewL(KVersitTokenCONTENTTYPE, KNullDesC8);
	CleanupStack::PushL(mimeTypeParam);
	TPtrC8 mimeType(aAlarmAction.MimeType());
	mimeTypeParam->SetValueL(mimeType);
  	arrayOfParams->AppendL(mimeTypeParam);
  	CleanupStack::Pop(mimeTypeParam);
  	
  	
	// Construct X-EPOC-ALARM property and initialise with parameters and content
	CVersitExtendedAlarm* alarm = CVersitExtendedAlarm::NewL(aAlarmAction.Content(), aAlarmAction.MimeType(), static_cast<CVersitExtendedAlarm::TDisposition>(aAlarmAction.Disposition()));
	CleanupStack::PushL(alarm);
	CParserPropertyValue* value = new(ELeave)CParserPropertyValueExtendedAlarm(alarm);
	CleanupStack::Pop(alarm);
	CleanupStack::PushL(value);
	CParserProperty* prop = CParserProperty::NewL(*value, aToken, arrayOfParams);
	CleanupStack::Pop(2,arrayOfParams); // value, arrayOfParams

	// Add to the parser
	aParser.AddPropertyL(prop); // takes ownership of prop
	}

void CAgendaEntryToVCalConverter::AddAttachmentPropertyL(CVersitParser& aParser, CCalAttachment& aAttachment)
	{
	// Create an array of parameters for this property
	CArrayPtr<CParserParam>* arrayOfParams = new(ELeave)CArrayPtrFlat<CParserParam>(4);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfParams,arrayOfParams));
	
	// Create value parameter
	CParserParam* valueType = CParserParam::NewL(KVersitTokenVALUE, KNullDesC8);
	CleanupStack::PushL(valueType);
	CCalAttachment::TType type = aAttachment.Type();
	TBool exportBinaryData = EFalse;
	switch (type)
		{
		case CCalAttachment::EUri:
			{
			valueType->SetValueL(KVCalContentValueUrl);
			break;
			}
		case CCalAttachment::EFile:
			{
			if (aAttachment.FileAttachment()->ContentId().Length() > 0)
				{
				valueType->SetValueL(KVCalContentValueContentId);
				}
			else if (aAttachment.IsAttributeSet(CCalAttachment::EExportInline))
				{
				valueType->SetValueL(KVCalContentValueInline);
				exportBinaryData = ETrue;
				}
			break;
			}
		default:
			// Do nothing. In this case the value will not be set
			break;
		}
	if (valueType->Value().Length() == 0)
		{
		// The property is unsupported. So we'll clean up and then return
		CleanupStack::PopAndDestroy(2, arrayOfParams); //arrayOfParams, valueType
		return;
		}
	arrayOfParams->AppendL(valueType); // takes ownership
	CleanupStack::Pop(valueType);

	// Create content MIME type parameter
	TPtrC8 mimeType(aAttachment.MimeType());
	if(mimeType != KNullDesC8)
		{
		CParserParam* mimeTypeParam = CParserParam::NewL(KVersitAttachMimeType, mimeType);
		CleanupStack::PushL(mimeTypeParam);
	  	arrayOfParams->AppendL(mimeTypeParam);
	  	CleanupStack::Pop(mimeTypeParam);
		}
  
  	const TPtrC fileName(aAttachment.Label());
  	if(fileName != KNullDesC)
  		{
		HBufC8* filenameUtf8 = HBufC8::NewLC(2 * fileName.Length()); // need room to convert from 16 bit to 8 bit (x2)
		TPtr8 pfileName(filenameUtf8->Des());
		User::LeaveIfError(CnvUtfConverter::ConvertFromUnicodeToUtf8(pfileName, fileName));					 
		CParserParam* fileNameParameter = CParserParam::NewL(KVersitAttachLabel, pfileName);
		CleanupStack::PopAndDestroy(filenameUtf8);  
		CleanupStack::PushL(fileNameParameter);
	  	arrayOfParams->AppendL(fileNameParameter);
	  	CleanupStack::Pop(fileNameParameter);
  		}
	 	// Create File Date parameter
	TTime lastModifiedFileTime = Time::NullTTime();
	if (aAttachment.FileAttachment())
		{
		lastModifiedFileTime = aAttachment.FileAttachment()->LastModifiedTimeUtc();
		}
	if(lastModifiedFileTime != Time::NullTTime())
		{
		_LIT(KStringFormat, "%F%Y%M%DT%H%T%SZ"); // locale-independent formatting YYMMDD"T"HHMMSS"Z" (day,month,year,hour,minute,second)
		TBuf<KMaxTimeStringSize> dateString;
		lastModifiedFileTime.FormatL(dateString,KStringFormat);
		TBuf8<KMaxTimeStringSize> dateString8;
		dateString8.Copy(dateString);
		CParserParam* fileDate = CParserParam::NewL(KVCalAttachFileDate, dateString8);
		CleanupStack::PushL(fileDate);
	  	arrayOfParams->AppendL(fileDate);
	  	CleanupStack::Pop(fileDate);
		}
	  	
  	CParserPropertyValue* propertyValue = NULL;

 	if (exportBinaryData)
 		{
 		RFile file;
	 	TRAPD(err,aAttachment.FileAttachment()->FetchFileHandleL(file));
		if(err == KErrNone)
			{
			CleanupClosePushL(file);
			propertyValue = CParserPropertyValueBinaryFile::NewL(file);
			CleanupStack::PopAndDestroy(&file);
			}
		else if(err == KErrArgument)
			{//FetchFileHandleL leaves with KErrArgument if there is no file handle has been set, eg. the entry hasn't been saved to Calendar file which isn't common user case
			TRAP(err, aAttachment.FileAttachment()->LoadBinaryDataL());

			if(err != KErrNone)
				{//The user can keep synching if, e.g. it is out of memory, or the attachment file is on a drive where the media has been removed, or the attachment has been deleted from the calendar store by other clients.
				CleanupStack::PopAndDestroy(arrayOfParams);	
				return;
				}

			propertyValue = CParserPropertyValueBinary::NewL(aAttachment.Value());
			}
		else
			{
			User::Leave(err);
			}
		}
 	else 
 		{
 		TPtrC8 attach8;
 		if (type == CCalAttachment::EFile)
 			{
 			attach8.Set(aAttachment.FileAttachment()->ContentId());	
 			}
 		else
 			{
 			attach8.Set(aAttachment.Value());	
 			}
 		
 		HBufC* attach16 = HBufC::NewLC(attach8.Length());
		attach16->Des().Copy(attach8);
 		propertyValue = CParserPropertyValueHBufC::NewL(attach16->Des());
		CleanupStack::PopAndDestroy(attach16);		
  		}
	CleanupStack::PushL(propertyValue);
	CParserProperty* property = CParserProperty::NewL(*propertyValue, KVersitTokenATTACH, arrayOfParams);
	aParser.AddPropertyL(property);
	CleanupStack::Pop(2,arrayOfParams);		// propertyValue, arrayOfParams
	}

void CAgendaEntryToVCalConverter::AddAttendeeParameterL(CArrayPtr<CParserParam>* aArrayOfParams, CCalAttendee* aAttendee)
	{
	// Create an array of paramaters for this property
	CParserParam* expectParam = CParserParam::NewL(KVCalAttendee8EXPECT, KNullDesC8);
	CleanupStack::PushL(expectParam);
	switch (aAttendee->VCalExpect())
		{
		case CCalAttendee::EVCalFyi:
			expectParam->SetValueL(KVCalAttendeeExpect8FYI);
			break;
		case CCalAttendee::EVCalRequire:
			expectParam->SetValueL(KVCalAttendeeExpect8REQUIRE);
			break;
		case CCalAttendee::EVCalRequest:
			expectParam->SetValueL(KVCalAttendeeExpect8REQUEST);
			break;
		case CCalAttendee::EVCalImmediate:
			expectParam->SetValueL(KVCalAttendeeExpect8IMMEDIATE);
			break;
		default:
			break;
		}
	aArrayOfParams->AppendL(expectParam); // takes ownership
	CleanupStack::Pop(expectParam); 
	
	CParserParam* rsvpParam = CParserParam::NewL(KVCalAttendee8RSVP, KNullDesC8);
	CleanupStack::PushL(rsvpParam);
	switch (aAttendee->ResponseRequested())
		{
		case ETrue:
			rsvpParam->SetValueL(KVCalAttendeeRsvp8YES);
			break;
		case EFalse:
			rsvpParam->SetValueL(KVCalAttendeeRsvp8NO);
			break;
		default:
			break;
		}
	aArrayOfParams->AppendL(rsvpParam); // takes ownership
	CleanupStack::Pop(rsvpParam); 
	
	CParserParam* calroleParam = CParserParam::NewL(KICalAttendeeCalRole8, KNullDesC8);
	CleanupStack::PushL(calroleParam);
	
	CParserParam* vcalroleParam = CParserParam::NewL(KICalAttendeeCalRole8, KNullDesC8);
	CleanupStack::PushL(vcalroleParam);
	
	switch (aAttendee->RoleL())
		{
		case CCalAttendee::EChair:
			calroleParam->SetValueL(KICalAttendeeCalRole8CHAIR);
			break;
		case CCalAttendee::EReqParticipant:
			calroleParam->SetValueL(KICalAttendeeCalRole8REQUIRED);
			break;
		case CCalAttendee::EOptParticipant: 
			calroleParam->SetValueL(KICalAttendeeCalRole8OPTIONAL);
			break;
		case CCalAttendee::ENonParticipant:
			calroleParam->SetValueL(KICalAttendeeCalRole8NONPARTICIPANT);
			break;
			
		case CCalAttendee::EVCalAttendee:
			vcalroleParam->SetValueL(KVCalAttendeeRole8ATTENDEE);
			break;
		case CCalAttendee::EVCalDelegate:
			vcalroleParam->SetValueL(KVCalAttendeeRole8DELEGATE);
			break;
		case CCalAttendee::EVCalOwner:
			vcalroleParam->SetValueL(KVCalAttendeeRole8OWNER);
			break;
		default:
			break;
		}
	if (calroleParam->Value().Length() > 0)
		{
		aArrayOfParams->AppendL(calroleParam); // takes ownership
		CleanupStack::PopAndDestroy(vcalroleParam); // delete the vCal role as it is not used
		CleanupStack::Pop(calroleParam);
		}
	else
		{
		aArrayOfParams->AppendL(vcalroleParam); // takes ownership
		CleanupStack::Pop(vcalroleParam);
		CleanupStack::PopAndDestroy(calroleParam); // delete the iCal role as it is not used
		}
	
	CParserParam* calStatusParam = CParserParam::NewL(KICalAttendeeCalStatus8, KNullDesC8);
	CleanupStack::PushL(calStatusParam);
	
	CParserParam* vcalStatusParam = CParserParam::NewL(KVCalAttendee8STATUS, KNullDesC8);
	CleanupStack::PushL(vcalStatusParam);
	
	switch (aAttendee->StatusL())
		{
		case CCalAttendee::EAccepted:
			calStatusParam->SetValueL(KVCalAttendeeStatus8ACCEPTED);
			vcalStatusParam->SetValueL(KVCalAttendeeStatus8ACCEPTED);
			break;
		case CCalAttendee::ENeedsAction:
			calStatusParam->SetValueL(KICalAttendeeCalStatus8NEEDSACTION);
			vcalStatusParam->SetValueL(KVCalAttendeeStatus8NEEDSACTION);
			break;
		case CCalAttendee::ETentative:
			calStatusParam->SetValueL(KVCalAttendeeStatus8TENTATIVE);
			vcalStatusParam->SetValueL(KVCalAttendeeStatus8TENTATIVE);
			break;
		case CCalAttendee::EConfirmed:
			calStatusParam->SetValueL(KVCalAttendeeStatus8CONFIRMED);
			vcalStatusParam->SetValueL(KVCalAttendeeStatus8CONFIRMED);
			break;
		case CCalAttendee::EDeclined:
			calStatusParam->SetValueL(KVCalAttendeeStatus8DECLINED);
			vcalStatusParam->SetValueL(KVCalAttendeeStatus8DECLINED);
			break;
		case CCalAttendee::ECompleted:
			calStatusParam->SetValueL(KVCalAttendeeStatus8COMPLETED);
			vcalStatusParam->SetValueL(KVCalAttendeeStatus8COMPLETED);
			break;
		case CCalAttendee::EDelegated:
			calStatusParam->SetValueL(KVCalAttendeeStatus8DELEGATED);
			vcalStatusParam->SetValueL(KVCalAttendeeStatus8DELEGATED);
			break;
		case CCalAttendee::EInProcess:
			calStatusParam->SetValueL(KICalAttendeeCalStatus8INPROCESS);
			break;
		case CCalAttendee::EVCalSent:
			vcalStatusParam->SetValueL(KVCalAttendeeStatus8SENT);
			break;
		case CCalAttendee::EVCalXReceived:
			vcalStatusParam->SetValueL(KVCalAttendeeStatus8XDASHRECEIVED);
			break;
		default:
			break;
		}
	
	if (vcalStatusParam->Value().Length() > 0)
		{
		aArrayOfParams->AppendL(vcalStatusParam); // takes ownership
		CleanupStack::Pop(vcalStatusParam);
		}
	else
		{
		CleanupStack::PopAndDestroy(vcalStatusParam); 
		}
		
	if (calStatusParam->Value().Length() > 0)
		{
		aArrayOfParams->AppendL(calStatusParam); // takes ownership
		CleanupStack::Pop(calStatusParam); 
		}
	else
		{
		CleanupStack::PopAndDestroy(calStatusParam); 
		}
	}

void CAgendaEntryToVCalConverter::AddAttendeePropertyL(CVersitParser& aParser, CCalUser* aAttendee, TBool isPhoneOwner, TBool isOrganizer)
	{
	CArrayPtr<CParserParam>* arrayOfParams = new(ELeave)CArrayPtrFlat<CParserParam>(9);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfParams,arrayOfParams));

	if( aAttendee->SentBy() != KNullDesC)
		{
		CParserParam* sentByParam = CParserParam::NewL(KICalAttendeeSentBy8, aAttendee->SentBy());
		CleanupStack::PushL(sentByParam);
		arrayOfParams->AppendL(sentByParam);
		CleanupStack::Pop(sentByParam);	
		}	

	if( aAttendee->CommonName() != KNullDesC)
		{
		CParserParam* commonNameParam = CParserParam::NewL(KICalAttendeeCommonName8, aAttendee->CommonName());
		CleanupStack::PushL(commonNameParam);
		arrayOfParams->AppendL(commonNameParam);	// takes ownership
		CleanupStack::Pop(commonNameParam);	
		}
	
	if(isPhoneOwner)
		{
		CParserParam* phoneOwnerParam = CParserParam::NewL(KICalAttendee8XDASHPHONEOWNER, KNullDesC8);
		CleanupStack::PushL(phoneOwnerParam);
		arrayOfParams->AppendL(phoneOwnerParam);	// takes ownership
		CleanupStack::Pop(phoneOwnerParam);			
		}
		
	if (isOrganizer)
		{
		CParserParam* organizerParam = CParserParam::NewL(KVCalAttendee8ROLE, KVCalAttendeeRole8ORGANIZER);
		CleanupStack::PushL(organizerParam);
		arrayOfParams->AppendL(organizerParam);	// takes ownership
		CleanupStack::Pop(organizerParam);
		}
	else
		{
		// these properties are not supported in CCalUser so don't need to be exported for an organizer
		AddAttendeeParameterL(arrayOfParams,static_cast<CCalAttendee*>(aAttendee));
		}
				
	CParserPropertyValue* value = CParserPropertyValueHBufC::NewL(aAttendee->Address());
	CleanupStack::PushL(value);
	CParserProperty* prop = CParserProperty::NewL(*value, KVersitTokenATTENDEE, arrayOfParams);
	// Add to the parser
	aParser.AddPropertyL(prop);
	CleanupStack::Pop(2,arrayOfParams);		// value, arrayOfParams
	}



void CAgendaEntryToVCalConverter::AddRepeatPropertiesL(CVersitParser& aParser, TCalRRule& aRpt, CTzRules* aEntryTzRule)
	{
	CVersitRecurrence* recurrence = NULL;
	TCalRRule::TType rType = aRpt.Type();

	TVersitDateTime* endTime = NULL;
	TUint duration = 0;
	if (aRpt.Count() != 0)
		{
		TTime untilTime;
		if(aEntryTzRule)
			{
			untilTime = aRpt.Until().TimeUtcL();
			//convert utc to repeat local
			aEntryTzRule->ConvertToLocalL(untilTime);
			}
		else
			{//use system local
			untilTime = aRpt.Until().TimeLocalL();
			}
		TDateTime end=untilTime.DateTime();
		endTime = new(ELeave) TVersitDateTime(end, TVersitDateTime::EIsVCardLocal);
		endTime->SetFlag(TVersitDateTime::EExportLeaveAsLocalTime);
 		duration = aRpt.Count();
		}
	CleanupStack::PushL(endTime);

	switch (aRpt.Type())
		{
		// Daily Repeat
		case TCalRRule::EDaily:
			recurrence = new(ELeave)CVersitRecurrenceDaily(aRpt.Interval(), duration, endTime);
			break;
		// Weekly Repeat
		case TCalRRule::EWeekly:
			recurrence = CreateWeeklyRepeatL(aRpt, duration, endTime);
			break;
		// Monthly By Days Repeat
		case TCalRRule::EMonthly:
			recurrence = CreateMonthlyRepeatL(aRpt, duration, endTime);
			break;
		// Yearly By Day Repeat
		case TCalRRule::EYearly:
			recurrence = CreateYearlyRepeatL(aRpt, duration, endTime);
			break;
		default:
			delete endTime;
			break;
		}
	CleanupStack::Pop();	// endTime (owned by recurrence)

	if (recurrence)
		{
		CleanupStack::PushL(recurrence);
		CParserPropertyValue* value = new(ELeave)CParserPropertyValueRecurrence(recurrence);	// takes ownership
		CleanupStack::Pop();		// recurrence
		CleanupStack::PushL(value);
		CParserProperty* prop = CParserProperty::NewL(*value, KVersitTokenRRULE, NULL);
		CleanupStack::Pop(value);

		// Add to the parser
		aParser.AddPropertyL(prop);
		}
	}

CVersitRecurrence* CAgendaEntryToVCalConverter::CreateWeeklyRepeatL(TCalRRule& aRpt,TInt aDuration, TVersitDateTime* aEndTime)
	{
	// Convert the agenda weekly repeat to a versit recurrence

	CWeekDayArray* dayArray = new(ELeave)CWeekDayArray();
	CleanupStack::PushL(dayArray);
	dayArray->iArray = new(ELeave)CArrayFixFlat<TDay>(1);

	// Iterate through the days to check which days are set
	RArray<TDay> daysinweek;
	CleanupClosePushL(daysinweek);
	aRpt.GetByDayL(daysinweek);
	TInt count=daysinweek.Count();
	for (TInt ii=0; ii<count; ++ii)
		{
		dayArray->iArray->AppendL(daysinweek[ii]);
		}
	CleanupStack::PopAndDestroy(&daysinweek);
	CVersitRecurrence* recurrence = new(ELeave)CVersitRecurrenceWeekly(aRpt.Interval(), aDuration, aEndTime, dayArray); // takes ownership
	CleanupStack::Pop(dayArray);

	return recurrence;
	}
	
CVersitRecurrence* CAgendaEntryToVCalConverter::CreateMonthlyRepeatL(TCalRRule& aRpt, TInt aDuration, TVersitDateTime* aEndTime)
	{
	// Convert Agenda Monthly By Days repeat to a versit recurrence
	RArray<TCalRRule::TDayOfMonth> days;
	aRpt.GetByDayL(days);
	TInt count=days.Count();
	if(count>0)	
		{//e.g. every Monday of first week and every Friday of the third week.
		CleanupClosePushL(days);
		CArrayPtrFlat<CVersitRecurrenceMonthlyByPos::CMonthPosition>* monthPositions = new(ELeave)CArrayPtrFlat<CVersitRecurrenceMonthlyByPos::CMonthPosition>(1);
		CleanupStack::PushL(monthPositions);

		// Iterate through the five weeks for the month
		// (First,Second,Third,Fourth,Last)
				
		for (TInt ii=0; ii<count; ++ii)
			{
			CVersitRecurrenceMonthlyByPos::CMonthPosition* monthPos = new(ELeave)CVersitRecurrenceMonthlyByPos::CMonthPosition();
			CleanupStack::PushL(monthPos);
			
			TInt8 weekNo = days[ii].WeekInMonth();
			if(weekNo>0)
				{
				monthPos->iSign = CVersitRecurrenceMonthlyByPos::CMonthPosition::EWeeksFromStartOfMonth;
				monthPos->iWeekNo = weekNo;	
				}
			else
				{
				monthPos->iSign = CVersitRecurrenceMonthlyByPos::CMonthPosition::EWeeksFromEndOfMonth;
				monthPos->iWeekNo = Abs(weekNo);
				}
			monthPos->iArrayOfWeekDays = new(ELeave)CWeekDayArray();
			monthPos->iArrayOfWeekDays->iArray = new(ELeave)CArrayFixFlat<TDay>(1);
			monthPos->iArrayOfWeekDays->iArray->AppendL(days[ii].Day());
			monthPositions->AppendL(monthPos);
			CleanupStack::Pop();		// monthPos;
			}

		CVersitRecurrence* recurrence = new(ELeave)CVersitRecurrenceMonthlyByPos(aRpt.Interval(), aDuration, aEndTime, monthPositions);
		CleanupStack::Pop();	// monthPositions (owned by recurrence)
		CleanupStack::PopAndDestroy(&days);
		return recurrence;
		}
	else
		{
		RArray<TInt> dayofmonth;
		aRpt.GetByMonthDayL(dayofmonth);
		TInt count=dayofmonth.Count();
		if(count>0)
			{
			CleanupClosePushL(dayofmonth);
			CArrayFixFlat<TInt>* dayList = new(ELeave)CArrayFixFlat<TInt>(1);
			CleanupStack::PushL(dayList);

			// Iterate through each day in the month

			for (TInt ii=0; ii<count; ++ii)
				{
				dayList->AppendL(dayofmonth[ii]+1);  // add 1, since 0 means 1st
				}

			CVersitRecurrence* recurrence = new(ELeave)CVersitRecurrenceMonthlyByDay(aRpt.Interval(), aDuration, aEndTime, dayList, NULL, EFalse);

			CleanupStack::Pop();		// dayList;
			
			CleanupStack::PopAndDestroy(&dayofmonth);
			return recurrence;
			}
		}
	return NULL;
	}

CVersitRecurrence* CAgendaEntryToVCalConverter::CreateYearlyRepeatL(TCalRRule& aRpt, TInt aDuration, TVersitDateTime* aEndTime)
	{
	// Convert Agenda Yearly By Days repeat to a versit recurrence

	// There is no vCal equivalent of a yearly 'by day' repeat,
	// e.g. repeat on the first monday of october,
	// so a monthly repeat with an interval of 12* the yearly repeat
	// is used instead
	RArray<TCalRRule::TDayOfMonth> days;
	aRpt.GetByDayL(days);
	TInt count=days.Count();
	if(count>0)
		{
		CleanupClosePushL(days);
		CArrayPtrFlat<CVersitRecurrenceMonthlyByPos::CMonthPosition>* monthPositions = new(ELeave)CArrayPtrFlat<CVersitRecurrenceMonthlyByPos::CMonthPosition>(1);
		CleanupStack::PushL(monthPositions);

		// Iterate through the five weeks for the month
		// (First,Second,Third,Fourth,Last)
				
		for (TInt ii=0; ii<count; ++ii)
			{
			CVersitRecurrenceMonthlyByPos::CMonthPosition* monthPos = new(ELeave)CVersitRecurrenceMonthlyByPos::CMonthPosition();
			CleanupStack::PushL(monthPos);
			monthPos->iSign = CVersitRecurrenceMonthlyByPos::CMonthPosition::EWeeksFromStartOfMonth;
			monthPos->iWeekNo = days[ii].WeekInMonth(); // iWeekNo goes from 1-5
		
			monthPos->iArrayOfWeekDays = new(ELeave)CWeekDayArray();
			monthPos->iArrayOfWeekDays->iArray = new(ELeave)CArrayFixFlat<TDay>(1);
			monthPos->iArrayOfWeekDays->iArray->AppendL(days[ii].Day());
			monthPositions->AppendL(monthPos);
			CleanupStack::Pop();		// monthPos;
			}
		// Interval * 12, since we really want is to repeat yearly
		CVersitRecurrence* recurrence = new(ELeave)CVersitRecurrenceMonthlyByPos(aRpt.Interval() * 12, aDuration, aEndTime, monthPositions);
		CleanupStack::Pop();	// monthPositions (owned by recurrence)
		CleanupStack::PopAndDestroy(&days);//close the array

		return recurrence;
		}
	else
		{
		CArrayFixFlat<TMonth>* monthList = new(ELeave)CArrayFixFlat<TMonth>(1);
		CleanupStack::PushL(monthList);
		monthList->AppendL(aRpt.DtStart().TimeLocalL().DateTime().Month());
		CVersitRecurrence* recurrence = new(ELeave)CVersitRecurrenceYearlyByMonth(aRpt.Interval(), aDuration, aEndTime, monthList);
		CleanupStack::Pop();	// monthList
		return recurrence;
		}
	}

void CAgendaEntryToVCalConverter::AddRDatePropertyL(CVersitParser& aParser, RArray<TCalTime>& aRdates)
	{
	CArrayPtrFlat<TVersitDateTime>* dateList = new (ELeave) CArrayPtrFlat<TVersitDateTime>(4);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfVersitDateTime,dateList));
	
	const TInt KCount(aRdates.Count());

	for (TInt i = 0; i < KCount; ++i)
		{
		TCalTime sporadicDate = aRdates[i];
		TDateTime rDate;
		if (iTimeType == TVersitDateTime::EIsUTC)
			{
			rDate=sporadicDate.TimeUtcL().DateTime();
			}
		else
			{
			rDate=sporadicDate.TimeLocalL().DateTime();
			}
		
		// Construct a versit date-time property	
		TVersitDateTime* versitDateTime = new(ELeave) TVersitDateTime(rDate, iTimeType);
		CleanupStack::PushL(versitDateTime);
		versitDateTime->SetFlag(iTimeFlag);	
		dateList->AppendL(versitDateTime);
		CleanupStack::Pop(); // versitDateTime
		}
	
	CParserPropertyValue* value = new(ELeave)CParserPropertyValueMultiDateTime(dateList);
	CleanupStack::Pop(dateList);
	CleanupStack::PushL(value);
	
	CParserProperty* property = CParserProperty::NewL(*value, KVersitTokenRDATE, NULL);
	CleanupStack::Pop(value);
	
	// Add to the parser
	aParser.AddPropertyL(property);//aParser takes the ownership
	}

// Exception property
//
void CAgendaEntryToVCalConverter::AddRepeatExceptionPropertiesL(CVersitParser& aParser, RArray<TCalTime>& aExceptions)
	{
	CArrayPtrFlat<TVersitDateTime>* dateList = new(ELeave)CArrayPtrFlat<TVersitDateTime>(1);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfVersitDateTime, dateList));
	
	TInt size = aExceptions.Count();

	for (TInt count=0; count<size; count++)
		{
		TCalTime exception = aExceptions[count];
		
		TDateTime date;
		if (iTimeType == TVersitDateTime::EIsUTC)
			{
			date = exception.TimeUtcL().DateTime();
			}
		else
			{
			date = exception.TimeLocalL().DateTime();
			}

		// Construct a date-time property

		TVersitDateTime* versitDateTime = new(ELeave) TVersitDateTime(date, iTimeType);
 		versitDateTime->SetFlag(iTimeFlag);
 		CleanupStack::PushL(versitDateTime);
		dateList->AppendL(versitDateTime);
		CleanupStack::Pop(); // versitDateTime 
		}

	CParserPropertyValue* value = new(ELeave)CParserPropertyValueMultiDateTime(dateList);
	CleanupStack::Pop();		// dateList

	CleanupStack::PushL(value);
	CParserProperty* prop = CParserProperty::NewL(*value, KVersitTokenEXDATE, NULL);
	CleanupStack::Pop(value);
	// Add to the parser
	aParser.AddPropertyL(prop);
	}

// Creates a stream dictionary, from an embedded store and a stream ID 
//
CStreamDictionary* CAgendaEntryToVCalConverter::CreateDictionaryLC(CEmbeddedStore& aEmbeddedStore, TStreamId& aId)
	{
	CStreamDictionary* dictionary = CStreamDictionary::NewLC();
	RStoreReadStream dicStream;
	dicStream.OpenLC(aEmbeddedStore, aId);
	dicStream >> *dictionary;
	CleanupStack::PopAndDestroy(); //dicStream
	return dictionary;
	}


// The vCalendar CATEGORIES property has multiple property values so the property values
// are stored in an array.
//
// The categories of the entry are iterated through, and each category is added as a 
// property value to the array. 
//
void CAgendaEntryToVCalConverter::AddCategoryPropertyL(CVersitParser& aParser, const RPointerArray<CCalCategory>& aCategories)
	{
	CDesCArrayFlat* desArray = new(ELeave) CDesCArrayFlat(4);
	CleanupStack::PushL(desArray);

	TInt categories = aCategories.Count();
	for (TInt count=0; count<categories; count++)
		{
		CCalCategory* category = aCategories[count];
		switch (category->Category())
			{
		case CCalCategory::ECalAppointment:
				desArray->AppendL(KVCalCategoriesAPPOINTMENT);
				break;
		case CCalCategory::ECalBusiness:
				desArray->AppendL(KVCalCategoriesBUSINESS);
				break;
		case CCalCategory::ECalEducation:
				desArray->AppendL(KVCalCategoriesEDUCATION);
				break;
		case CCalCategory::ECalHoliday:
				desArray->AppendL(KVCalCategoriesHOLIDAY);
				break;
		case CCalCategory::ECalMeeting:
				desArray->AppendL(KVCalCategoriesMEETING);
				break;
		case CCalCategory::ECalMiscellaneous:
				desArray->AppendL(KVCalCategoriesMISCELLANEOUS);
				break;
		case CCalCategory::ECalPersonal:
				desArray->AppendL(KVCalCategoriesPERSONAL);
				break;
		case CCalCategory::ECalPhoneCall:
				desArray->AppendL(KVCalCategoriesPHONECALL);
				break;
		case CCalCategory::ECalSickDay:
				desArray->AppendL(KVCalCategoriesSICKDAY);
				break;
		case CCalCategory::ECalSpecialOccasion:
				desArray->AppendL(KVCalCategoriesSPECIALOCCASION);
				break;
		case CCalCategory::ECalTravel:
				desArray->AppendL(KVCalCategoriesTRAVEL);
				break;
		case CCalCategory::ECalVacation:
				desArray->AppendL(KVCalCategoriesVACATION);
				break;
		case CCalCategory::ECalExtended:
				desArray->AppendL(category->ExtendedCategoryName());
				break;
		default:
				break;
			}
		}

    CParserPropertyValue* value = new (ELeave) CParserPropertyValueCDesCArray(desArray);
	CleanupStack::Pop(); //desArray
	
	CleanupStack::PushL(value); 
	CParserProperty* prop = CParserProperty::NewL(*value, KVersitTokenCATEGORIES, NULL);
	CleanupStack::Pop(value); 
	// Add to the parser
	aParser.AddPropertyL(prop);
	}

/**
 * Checks the status value and adds the appropriate status value to the export list 
 * held by the versit parser. 
 */
void CAgendaEntryToVCalConverter::AddStatusPropertyL(CVersitParser& aParser, CCalEntry* aEntry)
	{
	switch(aEntry->StatusL())
		{
		case CCalEntry::ETodoInProcess:
		case CCalEntry::EVCalAccepted:
			AddDesPropertyL(aParser, KVCalTokenSTATUS, KVCalStatusACCEPTED);			
			break;
		case CCalEntry::ETodoNeedsAction:
		case CCalEntry::EVCalNeedsAction:
			AddDesPropertyL(aParser, KVCalTokenSTATUS, KVCalStatusNEEDSACTION);			
			break;
		case CCalEntry::ETodoCompleted:
			AddDesPropertyL(aParser, KVCalTokenSTATUS, KVCalStatusCOMPLETED);			
			break;
			
		case CCalEntry::EConfirmed:
			AddDesPropertyL(aParser, KVCalTokenSTATUS, KVCalStatusCONFIRMED);			
			break;
		case CCalEntry::ECancelled:
		case CCalEntry::EVCalDeclined:
			AddDesPropertyL(aParser, KVCalTokenSTATUS, KVCalStatusDECLINED);			
			break;
		case CCalEntry::ETentative:
			AddDesPropertyL(aParser, KVCalTokenSTATUS, KVCalStatusTENTATIVE);			
			break;
		case CCalEntry::EVCalSent:
			AddDesPropertyL(aParser, KVCalTokenSTATUS, KVCalStatusSENT);			
			break;
		case CCalEntry::EVCalDelegated:
			AddDesPropertyL(aParser, KVCalTokenSTATUS, KVCalStatusDELEGATED);			
			break;
		}

	}



