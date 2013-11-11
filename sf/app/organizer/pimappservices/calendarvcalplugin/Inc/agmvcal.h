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

/**
 @file
 @internalComponent 
*/

#ifndef __AGMVCAL_H__
#define __AGMVCAL_H__


#include <vcal.h>
#include <apgdoor.h>
#include <concnf.h>
#include <confndr.h>
#include <conlist.h>

#include <tz.h>
#include <tzconverter.h>
#include <calcategory.h>
#include <calentry.h>

// Maximum characters in a GEO value should be 11 = -NNN.NNNNNN
const TUint KGEOMaxWidthOfGeoValue = 11;

const TUid KUidApaDoorStateStream={0x1000012a};
//const TUid KUidApaDoorDocStream={0x10000144};
const TUid KPlainTextToWordConverterUid={0x10001823};
const TUid KWordToPlainTextConverterUid={0x10001824};
const TUid KUidEtextToText={0x100040C8};
const TUid KUidTextToEtext={0x100040C7};
const TUid KUidTextToEtextNoTrim={0x10281B4C};
const TInt KConverterBufferSize = 1024;

// Extended VCal fields

_LIT(KVCalTokenENTRYTYPE,"AGENDAENTRYTYPE"); // for externalizing
_LIT(KVCalTokenXDASHENTRYTYPE,"X-EPOCAGENDAENTRYTYPE"); // for internalizing
_LIT8(KVCalToken8ENTRYTYPE,"AGENDAENTRYTYPE"); // for externalizing
_LIT8(KVCalToken8XDASHENTRYTYPE,"X-EPOCAGENDAENTRYTYPE"); // for internalizing

_LIT(KVCalTokenTypeAPPT,"APPOINTMENT");
_LIT(KVCalTokenTypeANNIV,"ANNIVERSARY");
_LIT(KVCalTokenTypeEVENT,"EVENT");
_LIT(KVCalTokenTypeTODO,"TODO");
_LIT(KVCalTokenTypeREMINDER,"REMINDER");
_LIT(KVCalTokenTypeNOTE,"NOTE");

_LIT(KVCalTokenPUBLIC,"PUBLIC");
_LIT(KVCalTokenPRIVATE,"PRIVATE");
_LIT(KVCalTokenCONFIDENTIAL,"CONFIDENTIAL");

_LIT(KVCalTokenTYPE,"TYPE");
_LIT8(KVCalToken8TYPE,"TYPE");
_LIT8(KVCalToken8RANGE,"RANGE");
_LIT(KVCalValueEPOCSOUND,"X-EPOCSOUND");
_LIT8(KVCalValue8EPOCSOUND,"X-EPOCSOUND");
_LIT8(KVCalAlarmActionType, "X-CONTENTTYPE");
_LIT8(KVCalAlarmActionTypeValue, "VALUE");
_LIT8(KVCalContentValueUrl, "URL");
_LIT8(KVCalContentValueInline, "INLINE");
_LIT8(KVCalContentValueContentId, "CONTENT-ID");
_LIT8(KVCalContentValueContentIdShort, "CID");
_LIT8(KVCalAttachFileDate, "X-FILEDATE");


//Recurence ID range
_LIT8(KVCalRecurrenceIDRange8THISONLY,"THISONLY");
_LIT8(KVCalRecurrenceIDRange8THISANDFUTURE,"THISANDFUTURE");
_LIT8(KVCalRecurrenceIDRange8THISANDPRIOR,"THISANDPRIOR");

// Attendee defines
_LIT8(KICalAttendee8XDASHPHONEOWNER, 			"X-PHONEOWNER");

_LIT(KVCalAttendeeROLE,"ROLE");
_LIT(KVCalAttendeeSTATUS,"STATUS");
_LIT(KVCalAttendeeRSVP,"RSVP");
_LIT(KVCalAttendeeEXPECT,"EXPECT");
_LIT8(KVCalAttendee8ROLE,"ROLE");
_LIT8(KVCalAttendee8STATUS,"STATUS");
_LIT8(KVCalAttendee8RSVP,"RSVP");
_LIT8(KVCalAttendee8EXPECT,"EXPECT");

_LIT(KVCalAttendeeRoleATTENDEE,"ATTENDEE");
_LIT(KVCalAttendeeRoleORGANIZER,"ORGANIZER");
_LIT(KVCalAttendeeRoleOWNER,"OWNER");
_LIT(KVCalAttendeeRoleDELEGATE,"DELEGATE");
_LIT8(KVCalAttendeeRole8ATTENDEE,"ATTENDEE");
_LIT8(KVCalAttendeeRole8ORGANIZER,"ORGANIZER");
_LIT8(KVCalAttendeeRole8OWNER,"OWNER");
_LIT8(KVCalAttendeeRole8DELEGATE,"DELEGATE");

_LIT(KVCalAttendeeStatusACCEPTED,"ACCEPTED");
_LIT(KVCalAttendeeStatusNEEDSACTION,"NEEDS ACTION");
_LIT(KVCalAttendeeStatusSENT,"SENT");
_LIT(KVCalAttendeeStatusTENTATIVE,"TENTATIVE");
_LIT(KVCalAttendeeStatusCONFIRMED,"CONFIRMED");
_LIT(KVCalAttendeeStatusDECLINED,"DECLINED");
_LIT(KVCalAttendeeStatusCOMPLETED,"COMPLETED");
_LIT(KVCalAttendeeStatusDELEGATED,"DELEGATED");
_LIT(KVCalAttendeeStatusXDASHRECEIVED,"X-RECEIVED"); // Epoc extension for received vCalendars
_LIT8(KVCalAttendeeStatus8ACCEPTED,"ACCEPTED");
_LIT8(KVCalAttendeeStatus8NEEDSACTION,"NEEDS ACTION");
_LIT8(KVCalAttendeeStatus8SENT,"SENT");
_LIT8(KVCalAttendeeStatus8TENTATIVE,"TENTATIVE");
_LIT8(KVCalAttendeeStatus8CONFIRMED,"CONFIRMED");
_LIT8(KVCalAttendeeStatus8DECLINED,"DECLINED");
_LIT8(KVCalAttendeeStatus8COMPLETED,"COMPLETED");
_LIT8(KVCalAttendeeStatus8DELEGATED,"DELEGATED");
_LIT8(KVCalAttendeeStatus8XDASHRECEIVED,"X-RECEIVED"); // Epoc extension for received vCalendars

//	iCal standard requires a hyphen for NEEDS ACTION value
_LIT(KICalAttendeeCalStatusNEEDSACTION,"NEEDS-ACTION");
_LIT8(KICalAttendeeCalStatus8NEEDSACTION,"NEEDS-ACTION");

// iCal flags that we want to preserve in serialization
_LIT(KICalAttendeeCalStatus,"X-STATUS");
_LIT(KICalAttendeeCalRole,"X-ROLE");
_LIT(KICalAttendeeCommonName,"X-CN");
_LIT(KICalAttendeeSentBy,"X-SENTBY");
_LIT8(KICalAttendeeCalStatus8,"X-STATUS");
_LIT8(KICalAttendeeCalRole8,"X-ROLE");
_LIT8(KICalAttendeeCommonName8,"X-CN");
_LIT8(KICalAttendeeSentBy8,"X-SENTBY");

// CalRole values
_LIT(KICalAttendeeCalRoleOPTIONAL, "OPT-PARTICIPANT");
_LIT(KICalAttendeeCalRoleNONPARTICIPANT, "NON-PARTICIPANT");
_LIT(KICalAttendeeCalRoleREQUIRED, "REQ-PARTICIPANT");
_LIT(KICalAttendeeCalRoleCHAIR, "CHAIR");
_LIT8(KICalAttendeeCalRole8OPTIONAL, "OPT-PARTICIPANT");
_LIT8(KICalAttendeeCalRole8NONPARTICIPANT, "NON-PARTICIPANT");
_LIT8(KICalAttendeeCalRole8REQUIRED, "REQ-PARTICIPANT");
_LIT8(KICalAttendeeCalRole8CHAIR, "CHAIR");
//	the new ones
_LIT(KICalAttendeeCalStatusINPROCESS, "IN-PROCESS");
_LIT8(KICalAttendeeCalStatus8INPROCESS, "IN-PROCESS");

_LIT(KVCalAttendeeRsvpYES,"YES");
_LIT(KVCalAttendeeRsvpNO,"NO");
_LIT8(KVCalAttendeeRsvp8YES,"YES");
_LIT8(KVCalAttendeeRsvp8NO,"NO");

_LIT(KVCalAttendeeExpectFYI,"FYI");
_LIT(KVCalAttendeeExpectREQUIRE,"REQUIRE");
_LIT(KVCalAttendeeExpectREQUEST,"REQUEST");
_LIT(KVCalAttendeeExpectIMMEDIATE,"IMMEDIATE");
_LIT8(KVCalAttendeeExpect8FYI,"FYI");
_LIT8(KVCalAttendeeExpect8REQUIRE,"REQUIRE");
_LIT8(KVCalAttendeeExpect8REQUEST,"REQUEST");
_LIT8(KVCalAttendeeExpect8IMMEDIATE,"IMMEDIATE");

// Categories property definitions
_LIT(KVCalCategoriesAPPOINTMENT,"APPOINTMENT");
_LIT(KVCalCategoriesBUSINESS,"BUSINESS");
_LIT(KVCalCategoriesEDUCATION,"EDUCATION");
_LIT(KVCalCategoriesHOLIDAY,"HOLIDAY");
_LIT(KVCalCategoriesMEETING,"MEETING");
_LIT(KVCalCategoriesMISCELLANEOUS,"MISCELLANEOUS");
_LIT(KVCalCategoriesPERSONAL,"PERSONAL");
_LIT(KVCalCategoriesPHONECALL,"PHONECALL");
_LIT(KVCalCategoriesSICKDAY,"SICKDAY");
_LIT(KVCalCategoriesSPECIALOCCASION,"SPECIALOCCASION");
_LIT(KVCalCategoriesTRAVEL,"TRAVEL");
_LIT(KVCalCategoriesVACATION,"VACATION");
_LIT(KVCalTokenXDash,"X-"); // same as KVersitTokenXDash but not explicitly narrow

// Status property
_LIT(KVCalTokenVarCOMPLETED, "COMPLETED");
_LIT(KVCalTokenVarSTATUS,	 "STATUS");
_LIT(KVCalStatusACCEPTED, "ACCEPTED");
_LIT(KVCalStatusSENT,		"SENT");
_LIT(KVCalStatusNEEDSACTION, "NEEDS ACTION");
_LIT(KVCalStatusTENTATIVE, "TENTATIVE");
_LIT(KVCalStatusCONFIRMED, "CONFIRMED");
_LIT(KVCalStatusDECLINED, "DECLINED");
_LIT(KVCalStatusCOMPLETED, "COMPLETED");
_LIT(KVCalStatusDELEGATED, "DELEGATED");

_LIT8(KVCalTokenCOMPLETED, "COMPLETED");
_LIT8(KVCalTokenSTATUS,	 "STATUS");

// X-Method property values
_LIT(KVCalTokenMethodStatusENone,  "NONE");
_LIT(KVCalTokenMethodStatusEPublish, "PUBLISH");
_LIT(KVCalTokenMethodStatusERequest, "REQUEST");
_LIT(KVCalTokenMethodStatusEReply, 	 "REPLY");
_LIT(KVCalTokenMethodStatusEAdd, "ADD");
_LIT(KVCalTokenMethodStatusECancel, "CANCEL");
_LIT(KVCalTokenMethodStatusERefresh, "REFRESH");
_LIT(KVCalTokenMethodStatusECounter, "COUNTER");
_LIT(KVCalTokenMethodStatusEDeclineCounter, "DECLINECOUNTER");

class CCalContent;
class CTzRules;
class CVTzActualisedRules;
class TCalRRule;

class AgnConvertUtil
/** 
@internalComponent
@released 
*/
	{
public:
	static HBufC* EncodeL(const TDesC& aText,TUid aConversion);
	};

// Agenda Entry to VCal conversion

NONSHARABLE_CLASS(CAgendaEntryToVCalConverter) : public CBase
/** 
@internalComponent
@released 
*/
	{
public:
	void ExportEntryL(CCalEntry* aEntry, CVersitParser& aParser);
	void SetTzConverter(RTz* aTzConverter) {iTzConverter = aTzConverter;}
	void AddTimeZonePropertiesL(CVersitParser& aParser, CCalEntry* aEntry);
	void NextVCalendarL(); // resets iAddedTzProperty

	~CAgendaEntryToVCalConverter();

private:
	void AddEntryPropertiesL(CCalEntry* aEntry, CVersitParser& aParser);
	void AddEventPropertiesL(CVersitParser& aParser, CCalEntry* aEntry);
	void AddTodoPropertiesL(CVersitParser& aParser, CCalEntry* aEntry);
	
 	void AddTzPropertyL(CVersitParser& aParser, TTimeIntervalSeconds aStdOffsetInSeconds);
 	void AddDaylightPropertiesL(CVersitParser& aParser, const CTzRules* aRules, TInt aStdOffset, TTime aRepeatStartTime, TTime aRepeatEndTime);
 	void AddDaylightPropertyL(CVersitParser& aParser, TTimeIntervalSeconds aOffsetInSeconds,
 									 	TTimeWithReference aStartTime, TTimeWithReference aEndTime);

	void AddIntegerPropertyL(CVersitParser& aParser, const TDesC8& aToken, TCalLocalUid aUid);
	void AddDateTimePropertyL(CVersitParser& aParser, const TDesC8& aToken, const TTime& aTime, const TVersitDateTime::TRelativeTime aRelativeTime, TVersitDateTime::TVersitDateTimeFlags aTimeFlag);
	void AddDesPropertyL(CVersitParser& aParser, const TDesC8& aToken, const TDesC& aDes);

	void AddAlarmPropertyL(CVersitParser& aParser, const TDesC8& aToken, const TTime& aAlarmTime, const TDesC& aAlarmName);
	void AddExtendedAlarmPropertyL(CVersitParser& aParser, const TDesC8& aToken, const CCalContent& aAlarmAction);

	void AddAttendeePropertyL(CVersitParser& aParser, CCalUser* aAttendee, TBool isPhoneOwner, TBool isOrganizer);
	void AddAttendeeParameterL(CArrayPtr<CParserParam>* aArrayOfParams, CCalAttendee* aAttendee);

	void AddRepeatPropertiesL(CVersitParser& aParser, TCalRRule& aRpt, CTzRules* aEntryTzRule);
	void AddRepeatExceptionPropertiesL(CVersitParser& aParser, RArray<TCalTime>& aExceptions);
	
	void AddRDatePropertyL(CVersitParser& aParser, RArray<TCalTime>& aRdates);
	CStreamDictionary* CreateDictionaryLC(CEmbeddedStore& aEmbeddedStore, TStreamId& aId);
	void AddCategoryPropertyL(CVersitParser& aParser, const RPointerArray<CCalCategory>& aCategories);
	void AddAttachmentPropertyL(CVersitParser& aParser, CCalAttachment& aAttachment);

	// Repeat properties
	CVersitRecurrence* CreateWeeklyRepeatL(TCalRRule& aRpt, TInt aDuration, TVersitDateTime* aEndTime);
	CVersitRecurrence* CreateMonthlyRepeatL(TCalRRule& aRpt, TInt aDuration, TVersitDateTime* aEndTime);
	CVersitRecurrence* CreateYearlyRepeatL(TCalRRule& aRpt, TInt aDuration, TVersitDateTime* aEndTime);
	static void ResetAndDestroyArrayOfParams(TAny* aObject);
	static void ResetAndDestroyArrayOfVersitDateTime(TAny* aObject);

	// Additional functions for compliance with vCal specs 1.0
	void AddStatusPropertyL(CVersitParser& aParser, CCalEntry* aEntry);
	
private:
	CParserVCal *iVCal;
	RTz* iTzConverter;			//RTz* is owned by TAgnVCalConverter
	TVersitDateTime::TRelativeTime iTimeType;
	TVersitDateTime::TVersitDateTimeFlags iTimeFlag;
	TDateTime iStartTime;
	TBool	iAddedTzProperty;
	};


// VCal to Agenda Entry conversion

NONSHARABLE_CLASS(CVCalToAgendaEntryConverter) : public CBase
/** 
@internalComponent
@released 
*/
	{
public:
	void ImportVCalL(CVersitParser& aParser, RPointerArray<CCalEntry>& aEntryArray);
	void SetTzConverter(RTz* aTzConverter);
	void SetTzRules(CTzRules* aTzRules);
	~CVCalToAgendaEntryConverter();

	void SetImportVCalendarValues(TBool aImportVCalendar);

	static TBool IsValidTime(const TTime& aTime);
private:
	CCalEntry::TType GetEntryTypeL(CVersitParser& aParser);
	TBool GetStartAndEndDatesL(CVersitParser& aParser, TTime& aStartDate, TTime& aEndDate, TVersitDateTime::TRelativeTime& aRelativeTime);

	TBool ImportEventPropertiesL(CVersitParser& aParser);
	TBool ImportTodoPropertiesL(CVersitParser& aParser);

	TBool ImportDateTimePropertyL(CVersitParser& aParser, const TDesC8& aToken, TTime& aValue, TVersitDateTime::TRelativeTime& aRelativeTime);

	TBool ImportIntegerPropertyL(CVersitParser& aParser, const TDesC8& aToken, TInt& aValue);
	HBufC* ImportDesPropertyL(CVersitParser& aParser, const TDesC8& aToken);

	TBool ImportAlarmPropertyL(CVersitParser& aParser, TTime& aEntryTime, TVersitDateTime::TRelativeTime& aRelativeTime);

	TBool ImportExtendedAlarmPropertyL(CCalAlarm* aAlarm, CVersitParser& aParser, const TDesC8& aToken);

	void ImportAttendeePropertiesL(CVersitParser& aParser, const TDesC8& aToken);
	void ImportCategoriesPropertyL(CVersitParser& aParser, const TDesC8& aToken);
	void ImportRDatePropertyL(CVersitParser& aParser, const TDesC8& aToken);

	void ImportRepeatRuleL(CVersitParser& aParser, const TDesC8& aToken, const TTime& aStartDate,TVersitDateTime::TRelativeTime aRelativeTime);
	void ImportRepeatPropertyL(CVersitParser& aParser, const TDesC8& aToken, const TTime& aRepeatTime, TVersitDateTime::TRelativeTime aRelativeTime);
	
	void ImportWeeklyRepeatPropertiesL(CVersitRecurrenceWeekly* aRepeat, TCalRRule& aRpt, TDay aDefaultDayOfWeek);
	void ImportMonthlyByDayRepeatPropertiesL(CVersitRecurrenceMonthlyByDay* aRepeat, TCalRRule& aRpt, TInt aDefaultDayOfMonth);
	void ImportMonthlyByPosRepeatPropertiesL(CVersitRecurrenceMonthlyByPos* aRepeat, TCalRRule& aRpt, const TTime& aStartDate);
	void ImportRepeatExceptionPropertiesL(CVersitParser& aParser, const TDesC8& aToken);
	void ImportAttachmentPropertyL(CVersitParser& aParser);
	TInt CalculateInstancesPerRepeatL(const TCalRRule& aRRule) const;

	TBool FindCategoryPropertyL(CArrayPtrFlat<CCalCategory>* aCategoryList, CCalCategory::TCalCategoryType aCategoryType);
	void UpdateCategoriesPropertyL(CVersitParser& aParser);
	void UpdateAttendeePropertiesL(CVersitParser& aParser);
	
	TCalTime::TTimeMode GetTimeModeL(CVersitParser& aParser);
	
	// Additional functions for compliance with vCal spec 1.0 and iCalendar spec
	void ImportStatusPropertyL(CVersitParser& aParser, CCalEntry::TStatus& aStatus);
	void ImportICalStatusPropertyL(CVersitParser& aParser, CCalEntry::TStatus& aStatus);
	void ImportVCalStatusPropertyL(CVersitParser& aParser, CCalEntry::TStatus& aStatus);
	
	/**
	 * @brief imports vcalendar for the types EAppt,
	 * EReminder,EAnniv,ETodo,EEvent
	 * 
	 * @param aParser reference to versit parser
	 * @param aRelativeTime reference to releative time
	 * @param aEntryType type of calendar entry
	 */
	void ImportVCalendarL(CVersitParser& aParser,
			TVersitDateTime::TRelativeTime& aRelativeTime,
			CCalEntry::TType& aEntryType);
	
	/**
	 * @brief import note entry from vcal
	 * 
	 * @param aParser reference to versit parser
	 * @param aRelativeTime reference to releative time
	 */
	void ImportNoteL(CVersitParser& aParser,
			TVersitDateTime::TRelativeTime& aRelativeTime);
	
private:
	CCalEntry*	iEntry; //for convenience
	RTz* iTzConverter; // It is used to convert time between UTC and Symstem local time. RTz* is owned by TAgnVCalConverter
	TVersitDateTime::TRelativeTime iTimeType;
	CTzRules* iTzZone; // It is imported from the vcs file used to convert time between UTC and event local time.
	TBool iVcalTokenTypeReminder;
	TBool iImportVCalendarValues;
	};

/** Template class for cleanup that needs ResetAndDestroy to be called 
@internalComponent
*/
template<class T> class CalCleanup
	{
public:
	static void PushResetAndDestroyL(T& aRef);
	static void PushResetAndDestroyPointerL(T& aRef);
	static void PushResetL(T& aRef);
	
private:
	static void ResetAndDestroy(TAny* aPtr);
	static void ResetAndDestroyPointer(TAny* aPtr);
	static void Reset(TAny* aPtr);
	};

const TInt KMaxTimeStringSize = 40;
#endif
