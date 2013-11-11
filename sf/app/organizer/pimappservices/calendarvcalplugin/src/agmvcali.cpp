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


#include <s32file.h>
#include <s32mem.h>
#include <bassnd.h>
#include <calentry.h>
#include <vtzrules.h>
#include "agnversit.h"
#include <utf.h>
#include <tz.h> //Conversion between local and UTC times
#include <calalarm.h>
#include <calcontent.h>
#include <calrrule.h>
#include <caluser.h>
#include <calattachment.h>

#include "agmvcal.h"

const TInt KCountDigits = 8;
_LIT(KDateStringFormat, "%F%D%M%Y%H%T%S%C"); // locale-independent formatting DDMMYYHHMMSSUUUUUU (day,month,year,hour,minute,second,usec
_LIT(KVersitTokenMailTo,"MAILTO:");

HBufC* AgnConvertUtil::EncodeL(const TDesC& aText,TUid aConversion)
	{
	HBufC8* text=HBufC8::NewLC(aText.Length()*2);
	TPtr8 ptr = text->Des();
	TInt i;
	for (i=0; i < aText.Length(); i++)
		{
		ptr.Append(aText[i] & 0x00FF);
		ptr.Append((aText[i] >> 8) & 0x00FF);
		}
	CCnaConverterList* convList=CCnaConverterList::NewLC(); 
	CConverterBase* conv = convList->NewConverterL(aConversion); 
	if (!conv)
		{
		CleanupStack::PopAndDestroy();          // convList 
		User::Leave(KErrNotSupported);
		}
	CleanupStack::PushL(conv);
	CBufFlat* decodeBuffer = CBufFlat::NewL(256); 
	CleanupStack::PushL(decodeBuffer);
	CBufFlat* encodedBuffer = CBufFlat::NewL(256); 
	CleanupStack::PushL(encodedBuffer);
	decodeBuffer->InsertL(0,ptr);
	RBufReadStream readStream;
	RBufWriteStream writeStream;
	readStream.Open(*decodeBuffer);
	writeStream.Open(*encodedBuffer);
	conv->ConvertObjectL(readStream, writeStream); 
	readStream.Close();
	TInt size=encodedBuffer->Size();
	HBufC* writeBuf=HBufC::NewLC(size); 
	TPtr resulttext = writeBuf->Des();
	for(i = 0; i < (size - 1); i += 2)
		{
		resulttext.Append((encodedBuffer->Ptr(0)[i + 1] << 8) | 
			encodedBuffer->Ptr(0)[i]);
		}

	writeStream.CommitL();
	writeStream.Close();
	CleanupStack::Pop(writeBuf); // writebuf
	CleanupStack::PopAndDestroy(2,decodeBuffer); // buffers 
	CleanupStack::PopAndDestroy(2,convList); //conv+convList
	CleanupStack::PopAndDestroy(text);  //text
	return (writeBuf);
	}

// This method imports a vCalendar entity from aParser and returns a pointer to a CAgnEntry
// if successful or NULL if it wasn't possible to import the vCalendar. It reads the
// relevant properties from aParser entity and builds up a corresponding agenda entry.
// It allows to import vCal file without Summary and Description fields
void CVCalToAgendaEntryConverter::ImportVCalL(CVersitParser& aParser, RPointerArray<CCalEntry>& aEntryArray)
	{
	CCalEntry::TType entryType = GetEntryTypeL(aParser);
	
		// Group Scheduling Data
	
	//Import UID
	HBufC* guid16 = ImportDesPropertyL(aParser, KVersitTokenUID);
	
	if (!guid16) // non cal entry 
		{
		// create our a UID based on the current time (UTC, microsecond precision)
		TTime now;
		now.UniversalTime();  
		TBuf<KMaxTimeStringSize+KCountDigits> dateString;
		now.FormatL(dateString,KDateStringFormat);
		
		TInt threeDidgit=Math::Random() % 1000;
		dateString.Append(threeDidgit);
		
		guid16 = dateString.AllocL();	
		}
		
	CleanupStack::PushL(guid16);
	
	// the UID is stored as an 8-bit descriptor, so convert from 16-bit to 8-bit
	HBufC8* guid8= CnvUtfConverter::ConvertFromUnicodeToUtf8L(guid16->Des());
	CleanupStack::PopAndDestroy(guid16);  
	CleanupStack::PushL(guid8);		
		
	// Import Sequence Number
	TInt seqNumber;	
	
	// if no sequence number
	if (!ImportIntegerPropertyL(aParser, KVersitTokenSEQUENCE, seqNumber))
		{
		seqNumber = 0;
		}
	
	// Import Method
	CCalEntry::TMethod methodStatus(CCalEntry::EMethodNone);
	HBufC* method = ImportDesPropertyL(aParser, KVersitTokenXMETHOD);
	
	if (method)
		{
		CleanupStack::PushL(method);
		if (*method == KVCalTokenMethodStatusENone)	
			{
			methodStatus = CCalEntry::EMethodNone;
			}
		else if (*method == KVCalTokenMethodStatusEPublish)	
			{
			methodStatus = CCalEntry::EMethodPublish;
			}
		else if (*method == KVCalTokenMethodStatusERequest)	
			{
			methodStatus = CCalEntry::EMethodRequest;
			}
		else if (*method == KVCalTokenMethodStatusEReply)	
			{
			methodStatus = CCalEntry::EMethodReply;
			}
		else if (*method == KVCalTokenMethodStatusEAdd)	
			{
			methodStatus = CCalEntry::EMethodAdd;
			}
		else if (*method == KVCalTokenMethodStatusECancel)	
			{
			methodStatus = CCalEntry::EMethodCancel;
			}
		else if (*method == KVCalTokenMethodStatusERefresh)	
			{
			methodStatus = CCalEntry::EMethodRefresh;
			}
		else if (*method == KVCalTokenMethodStatusECounter)	
			{
			methodStatus = CCalEntry::EMethodCounter;
			}
		else if (*method == KVCalTokenMethodStatusEDeclineCounter)	
			{
			methodStatus = CCalEntry::EMethodDeclineCounter;
			}
		else
			{
			// do nothing if method is invalid - it will be None by default
			}	

		
		CleanupStack::PopAndDestroy(method);
		}		
	
	// Import X-Recurrence-Id
	TVersitDateTime::TRelativeTime relativeTime = TVersitDateTime::EIsUTC;
	TTime recurrenceId;
	TBool isRecurrenceIdPresent = ImportDateTimePropertyL(aParser,
			KVersitTokenXRECURRENCEID, recurrenceId, relativeTime);
	if (!isRecurrenceIdPresent)
		{
		iEntry = CCalEntry::NewL(entryType,guid8, methodStatus, seqNumber);
		}
	else
		{
		TCalTime caltime;
		if(GetTimeModeL(aParser) == TCalTime::EFloating)
			{
			caltime.SetTimeLocalFloatingL(recurrenceId);
			}
		else
			{
			caltime.SetTimeUtcL(recurrenceId);
			}
	
		iEntry = CCalEntry::NewL(entryType, guid8, methodStatus, seqNumber,
				caltime, CalCommon::EThisOnly);
		}
		
	CleanupStack::Pop(guid8); // ownership was passed to calGSData	
	// Fill in the entry description from the SUMMARY property
	// If this doesn't exist, use the DESCRIPTION property, and if this
	// doesn't exist, return a NULL entry
	
	if (entryType != CCalEntry::ENote)
		{
		ImportVCalendarL(aParser, relativeTime, entryType);
		}
	else
		{
		ImportNoteL(aParser,relativeTime);
		}
	
	// Import favourite property. The type will be converted to an usigned integer
	// implicitly. 
	TInt favourite = 0;
	TBool integerFound = ImportIntegerPropertyL( 
			aParser, KVCalTokenXFavourite, favourite );
	
	if(integerFound)
		{
		iEntry->SetFavouriteL(favourite);
		}
	
	aEntryArray.AppendL(iEntry); // takes ownership of entry
	iEntry = NULL;
	}

// -----------------------------------------------------------------------------
// CVCalToAgendaEntryConverter::ImportVCalendarL
// Imports vcalendar for calendar types EAppt/EReminder/EAnniv/ETodo/EEvent
// -----------------------------------------------------------------------------
// 
void CVCalToAgendaEntryConverter::ImportVCalendarL(CVersitParser& aParser,
		TVersitDateTime::TRelativeTime& aRelativeTime,
		CCalEntry::TType& aEntryType)
	{
	// Fill in the entry description from the SUMMARY property
	// If this doesn't exist, use the DESCRIPTION property, and if this
	// doesn't exist, return a NULL entry
	
	// get properties but don't take ownership of the elements of the array
	CArrayPtr<CParserProperty>* properties = aParser.PropertyL(
			KVersitTokenSUMMARY, TUid::Uid(KVersitPropertyHBufCUid), EFalse);
	
	if (!properties)
		{
		// get properties but don't take ownership of the elements of the array
		properties = aParser.PropertyL(KVersitTokenDESCRIPTION,
				TUid::Uid(KVersitPropertyHBufCUid), EFalse);
		}
	CleanupStack::PushL(properties);
	
	// Use the value found from the Summary/Description 
	// Both Summary and Description are not found in imported vCal file 
	//then insert empty enties for summary and description
	//  these empty entries will be descarded while exporting vCal file
	TPtrC summary;
	if (properties)
		{
		summary.Set(static_cast<CParserPropertyValueHBufC*>(
				(*properties)[0]->Value())->Value());
		}
		
	HBufC* summaryBuf = AgnConvertUtil::EncodeL(summary,KUidTextToEtextNoTrim);
	CleanupStack::PushL(summaryBuf);
	if (summaryBuf)
		{
		iEntry->SetSummaryL(*summaryBuf);
		}
	CleanupStack::PopAndDestroy(summaryBuf);
	
	HBufC* description =ImportDesPropertyL(aParser,KVersitTokenDESCRIPTION);
	CleanupStack::PushL(description);
	if (description != NULL)
		{
		iEntry->SetDescriptionL(*description); // Takes ownership
		}
	CleanupStack::PopAndDestroy(description);
	
	TBool validEntry = EFalse;
	if (aEntryType == CCalEntry::ETodo)
		{
		validEntry = ImportTodoPropertiesL(aParser);
		}
	else
		{
		validEntry = ImportEventPropertiesL(aParser);
		}
			
	if (!validEntry)
		{
		CleanupStack::PopAndDestroy(properties);
		delete iEntry;
		iEntry = NULL;
		return;
		}
	
	// Import CLASS property
	HBufC* property=ImportDesPropertyL(aParser, KVersitTokenCLASS);
	if (property)
		{
		CleanupStack::PushL(property);
		if (*property == KVCalTokenPUBLIC)
			iEntry->SetReplicationStatusL(CCalEntry::EOpen);
		else if (*property == KVCalTokenPRIVATE)
			iEntry->SetReplicationStatusL(CCalEntry::EPrivate);
		else if (*property == KVCalTokenCONFIDENTIAL)
			iEntry->SetReplicationStatusL(CCalEntry::ERestricted);
		CleanupStack::PopAndDestroy(property);
		}
	
	// Import LOCATION property
	HBufC* locationProperty=ImportDesPropertyL(aParser, KVersitTokenLOCATION);
	if (locationProperty)
		{
		CleanupStack::PushL(locationProperty);
		iEntry->SetLocationL(*locationProperty);
		CleanupStack::PopAndDestroy(locationProperty);
		}
	// Import DTSTAMP
	TTime timeProperty;
	if (ImportDateTimePropertyL(aParser, KVersitTokenXDTSTAMP, timeProperty,
			aRelativeTime))
		{
		TCalTime caltimeProperty;
		caltimeProperty.SetTimeUtcL(timeProperty);
		iEntry->SetDTStampL(caltimeProperty);
		}
	
	// Attendee property
	ImportAttendeePropertiesL(aParser, KVersitTokenATTENDEE);
	
	// Categories property
	ImportCategoriesPropertyL(aParser, KVersitTokenCATEGORIES);
	
	
	TInt localId;
	if (ImportIntegerPropertyL(aParser, KVersitTokenXLOCALUID, localId))
		{
		iEntry->SetLocalUidL(localId);
		}
	
	// Additional stuff to comply with Versit Specs 1.0  
	// All entries have a priority not just ToDo's
	// Todo Functionality merged into this code block
	TInt priority;
	if (ImportIntegerPropertyL(aParser, KVersitTokenPRIORITY, priority))
		{
		iEntry->SetPriorityL(priority);	
		}
	
	// All entries have a status - will default to Needs Action if cannot be read
	CCalEntry::TStatus entryStatus=CCalEntry::ENullStatus;
	ImportStatusPropertyL(aParser, entryStatus);
	//Now sync completed and ECompleted status to keep entry consistenty 
	if (aEntryType == CCalEntry::ETodo
			&& (iEntry->CompletedTimeL().TimeUtcL()) != Time::NullTTime())
		{
		iEntry->SetCompletedL(ETrue, iEntry->CompletedTimeL());
		}
	iEntry->SetStatusL(entryStatus);
	
	//Get last changed date from vcal
	if (ImportDateTimePropertyL(aParser, KVersitTokenLASTMODIFIED,
			timeProperty, aRelativeTime))
			{
			if (iTzZone && (aRelativeTime == TVersitDateTime::EIsVCardLocal))
				{
				iTzZone->ConvertToUtcL(timeProperty);
				}
			TCalTime lastModifiedDate;
			lastModifiedDate.SetTimeUtcL(timeProperty);
			iEntry->SetLastModifiedDateL(lastModifiedDate);
			}
		else
			{
			iEntry->SetLastModifiedDateL(); //set changed date to now
			}
		
	TInt transp;
	if (ImportIntegerPropertyL(aParser, KVersitTokenTRANSP, transp))
		{
		iEntry->SetTimeTransparencyL(static_cast<CCalEntry::TTransp>(transp));
		}
	
	ImportAttachmentPropertyL(aParser);
	
	// Import GEO
	HBufC* geoString=ImportDesPropertyL(aParser,KVersitTokenGEO);
	
	if (geoString != NULL)
		{
		CleanupStack::PushL(geoString);
		// Determine the position of the delimiter for extraction of the 
		// geo latitude and longitude values
		TInt delimiterPos = geoString->Locate(KVersitTokenCommaVal);
		
		if(delimiterPos!=KErrNotFound)
			{
			// Extract the latitude
			TLex geoLatitudeLex(geoString->Left(delimiterPos));
			// Extract the latitude by excluding the delimiter
			TLex geoLongitudeLex(geoString->Right(geoString->Length()-(delimiterPos+1)));
			
			TReal geoLatitude;
			TReal geoLongitude;
			
			if ((geoLatitudeLex.Val(geoLatitude) == KErrNone)
					&& (geoLongitudeLex.Val(geoLongitude) == KErrNone))
				{
				CCalGeoValue* importedGeoValue=CCalGeoValue::NewL();
				CleanupStack::PushL(importedGeoValue);
				TRAPD(err, importedGeoValue->SetLatLongL(geoLatitude,geoLongitude));
				if(err==KErrNone)
					{
					iEntry->SetGeoValueL(*importedGeoValue);
					}
				CleanupStack::PopAndDestroy(importedGeoValue);
				}
			}
		CleanupStack::PopAndDestroy(geoString);
		}
	
	CleanupStack::PopAndDestroy(properties);
	}

// -----------------------------------------------------------------------------
// CVCalToAgendaEntryConverter::ImportNoteL
// Imports note from vcal and maps it to CCalEntry::ENote
// -----------------------------------------------------------------------------
// 
void CVCalToAgendaEntryConverter::ImportNoteL(CVersitParser& aParser,
		TVersitDateTime::TRelativeTime& aRelativeTime)
	{
	
	// set description to entry from vcal
	// if description property is not present,look for summary property and
	// set the summary as description field for the note
	CArrayPtr<CParserProperty>* properties = aParser.PropertyL(
		KVersitTokenDESCRIPTION, TUid::Uid(KVersitPropertyHBufCUid),EFalse);
	if(!properties)
		{
		properties = aParser.PropertyL(KVersitTokenSUMMARY, TUid::Uid(
				KVersitPropertyHBufCUid), EFalse);
		CleanupStack::PushL(properties);
		TPtrC summary;
		if (properties)
			{
			summary.Set(static_cast<CParserPropertyValueHBufC*>(
					(*properties)[0]->Value())->Value());
			}
			
		HBufC* summaryBuf = AgnConvertUtil::EncodeL(summary,
				KUidTextToEtextNoTrim);
		CleanupStack::PushL(summaryBuf);
		if (summaryBuf)
			{
			iEntry->SetDescriptionL(*summaryBuf);
			}
		CleanupStack::PopAndDestroy(summaryBuf);
		CleanupStack::PopAndDestroy(properties);
		}
	else
		{
		HBufC* description = ImportDesPropertyL(aParser,KVersitTokenDESCRIPTION);
		CleanupStack::PushL(description);
		if (description != NULL)
			{
			iEntry->SetDescriptionL(*description); // Takes ownership
			}
		CleanupStack::PopAndDestroy(description);
		}
	
	// get local uid from vcal
	TInt localId;
	if (ImportIntegerPropertyL(aParser, KVersitTokenXLOCALUID, localId))
		{
		iEntry->SetLocalUidL(localId);
		}
	
	// get last changed date from vcal
	TTime timeProperty;
	if (ImportDateTimePropertyL(aParser, KVersitTokenLASTMODIFIED,
			timeProperty, aRelativeTime))
		{
		if (iTzZone && (aRelativeTime == TVersitDateTime::EIsVCardLocal))
			{
			iTzZone->ConvertToUtcL(timeProperty);
			}
		TCalTime lastModifiedDate;
		lastModifiedDate.SetTimeUtcL(timeProperty);
		iEntry->SetLastModifiedDateL(lastModifiedDate);
		}
	else
		{
		iEntry->SetLastModifiedDateL(); //set changed date to now
		}
	
	// get attachments from the vcal
	ImportAttachmentPropertyL(aParser);
	}

CVCalToAgendaEntryConverter::~CVCalToAgendaEntryConverter()
	{
	delete iEntry;
	delete iTzZone;
	}

void CVCalToAgendaEntryConverter::SetTzConverter(RTz* aTzConverter)
	{
	iTzConverter = aTzConverter;
	}
	
void CVCalToAgendaEntryConverter::SetTzRules(CTzRules* aTzZone)
	{
	// this function should only ever be called once so this shouldn't be needed
	if (iTzZone)
		{
		delete iTzZone;
		iTzZone = NULL;
		}
	iTzZone = aTzZone;
	}
	
TCalTime::TTimeMode CVCalToAgendaEntryConverter::GetTimeModeL(CVersitParser& aParser)
	{
	TTime dummyTime;
	TVersitDateTime::TRelativeTime relativeTime;
	
	if ( ! ImportDateTimePropertyL(aParser, KVersitTokenDTSTART, dummyTime, relativeTime))
		{
		if ( ! ImportDateTimePropertyL(aParser, KVersitTokenDTEND, dummyTime, relativeTime))
			{
			if ( ! ImportDateTimePropertyL(aParser, KVersitTokenDUE, dummyTime, relativeTime))
				{
				// no time
				relativeTime = TVersitDateTime::EIsUTC;
				}
			}
		}
	
	if (iTzZone || relativeTime == TVersitDateTime::EIsUTC)
		{
		return TCalTime::EFixedUtc;
		}
	return TCalTime::EFloating;
	}

// Try and find out which Epoc Agenda entry type this belongs to.
// At the moment, relies on having an 'X-EPOCAGENDAENTRYTYPE' field, and defaults to a appointment
// type if this isn't present.
// Maybe some heuristics should be used to try and ascertain which entry type is most appropriate
// e.g. if the entry has a yearly repeat, then maybe it should be an anniversary type.
//
// A call to this function resets iVcalTokenTypeReminder flag (unless the vCal entry is actually a Reminder).
//
CCalEntry::TType CVCalToAgendaEntryConverter::GetEntryTypeL(CVersitParser& aParser)
	{
	// If the entry type is a Todo, return immediately
	if (aParser.EntityName()==KVersitVarTokenVTODO)
		{
		return CCalEntry::ETodo;
		}
	
	// get properties but don't take ownership of the elements of the array
	CArrayPtr<CParserProperty>* properties = aParser.PropertyL(KVCalToken8XDASHENTRYTYPE, TUid::Uid(KVersitPropertyHBufCUid), EFalse);
	if (properties)
		{
		TPtrC type = static_cast<CParserPropertyValueHBufC*>((*properties)[0]->Value())->Value();
		delete properties;

		if (type == KVCalTokenTypeAPPT)
			{
			return CCalEntry::EAppt;
			}

		if (type == KVCalTokenTypeEVENT)
			{
			return CCalEntry::EEvent;
			}

		if (type == KVCalTokenTypeANNIV)
			{
			return CCalEntry::EAnniv;
			}

		if (type == KVCalTokenTypeTODO)
			{
			return CCalEntry::ETodo;
			}
		
		if (type == KVCalTokenTypeREMINDER)
			{
			return CCalEntry::EReminder;
			}
		
		if (type == KVCalTokenTypeNOTE)
			{
			return CCalEntry::ENote;
			}
		
		}

	// return an appointment as default type
	return (CCalEntry::EAppt);
	}

TBool CVCalToAgendaEntryConverter::IsValidTime(const TTime& aTime)
	{
	if (aTime < TCalTime::MinTime() || aTime > TCalTime::MaxTime())
		{
		return EFalse;
		}
	return ETrue;
	}

// Import Recurrence Dates property
//
void CVCalToAgendaEntryConverter::ImportRDatePropertyL(CVersitParser& aParser, const TDesC8& aToken)
	{
	// get properties but don't take ownership of the elements of the array
	CArrayPtr<CParserProperty>* properties = aParser.PropertyL(aToken, TUid::Uid(KVersitPropertyMultiDateTimeUid), EFalse);
	
	if (properties)
		{
		CleanupStack::PushL(properties);
		CArrayPtr<TVersitDateTime>* rDates = static_cast<CParserPropertyValueMultiDateTime*>((*properties)[0]->Value())->Value();
			
		if (rDates)	
			{
			RArray<TCalTime>calrdates;
			CleanupClosePushL(calrdates);
			const TInt KSize = rDates->Count();
			for (TInt count = 0; count < KSize; ++count)
				{
				TVersitDateTime* time = (*rDates)[count];
				TTime timeProperty(time->iDateTime);
				
				if (IsValidTime(timeProperty))
					{
					TVersitDateTime::TRelativeTime relativeTime = time->iRelativeTime;
					if (iTzZone && (relativeTime == TVersitDateTime::EIsVCardLocal))
						{
						//Convert to utc time using imported TZ rules
						iTzZone->ConvertToUtcL(timeProperty);   
						relativeTime = TVersitDateTime::EIsUTC;
						}
				
					TCalTime calrdate;
					if (relativeTime == TVersitDateTime::EIsVCardLocal)
						{
						calrdate.SetTimeLocalFloatingL(timeProperty);
						}
					else
						{
						__ASSERT_DEBUG(relativeTime == TVersitDateTime::EIsUTC, Panic(EAgnVersitPanicWrongTimeType));
						calrdate.SetTimeUtcL(timeProperty); //assume time as UTC for release build
						}
					calrdates.Append(calrdate);
					}
				}
			iEntry->SetRDatesL(calrdates);
			CleanupStack::PopAndDestroy();//close RArray
			}
		CleanupStack::PopAndDestroy(properties);
		}

	}
	
// Import the start and end dates of the vCalendar
// This method returns EFalse if there is no start date or end date, or if the start date 
// is after the end date. If the start date is missing but an end date is present, the 
// start date is set to the end date. If the end date is missing but there is a start
// date, the end date is set to the start date.
//
TBool CVCalToAgendaEntryConverter::GetStartAndEndDatesL(CVersitParser& aParser, TTime& aStartDate, TTime& aEndDate, TVersitDateTime::TRelativeTime& aRelativeTime)
	{
	TBool startValid = ETrue;
	TBool endValid = ETrue;
	
	if (!ImportDateTimePropertyL(aParser, KVersitTokenDTSTART, aStartDate,aRelativeTime))
		{
		startValid = EFalse;
		}

	TVersitDateTime::TRelativeTime relativeTime;
	
	if (!ImportDateTimePropertyL(aParser, KVersitTokenDTEND, aEndDate, relativeTime))
		{
		endValid = EFalse;
		}

	if (!startValid && !endValid)
		{
		// Neither times exist, so return Invalid Entry
		return EFalse;
		}

	if (startValid && !endValid)
		{
		// Start time but no end time
		aEndDate = aStartDate;
		}
	else if (!startValid && endValid)
		{
		// End time but no start time
		aStartDate = aEndDate;
		aRelativeTime = relativeTime;
		}

	if (aStartDate>aEndDate)
		return EFalse;

	return ETrue;
	}

// Import appointment properties
//
TBool CVCalToAgendaEntryConverter::ImportEventPropertiesL(CVersitParser& aParser)
	{
	TTime startDate;
	TTime endDate;
	TVersitDateTime::TRelativeTime relativeTime;
	if (!GetStartAndEndDatesL(aParser, startDate, endDate, relativeTime))
		{
		return EFalse;
		}

	if(iEntry->EntryTypeL()==CCalEntry::EReminder)
		{
		endDate = startDate;
		}

	TCalTime caltimeStart;
	TCalTime caltimeEnd;
	if (relativeTime == TVersitDateTime::EIsVCardLocal)
		{
		caltimeStart.SetTimeLocalFloatingL(startDate);
		caltimeEnd.SetTimeLocalFloatingL(endDate);
		}
	else
		{
		__ASSERT_DEBUG(relativeTime==TVersitDateTime::EIsUTC, Panic(EAgnVersitPanicWrongTimeType));
		caltimeStart.SetTimeUtcL(startDate); //assume time as UTC for release build
		caltimeEnd.SetTimeUtcL(endDate); //assume time as UTC for release build
		}
	
	if (startDate > endDate && endDate != Time::NullTTime())
		{
		return EFalse;
		}
	iEntry->SetStartAndEndTimeL(caltimeStart,caltimeEnd);

	// Attempt to import alarm/repeat properties
	ImportRepeatPropertyL(aParser, KVersitTokenRRULE, startDate, relativeTime);
	ImportAlarmPropertyL(aParser, startDate, relativeTime);
	return ETrue;
	}

//
TBool CVCalToAgendaEntryConverter::ImportTodoPropertiesL(CVersitParser& aParser)
	{
	TVersitDateTime::TRelativeTime relativeTime;//EIsUTC is the default
	TTime startDate = Time::NullTTime();
	TTime endDate = Time::NullTTime();
	
	if (!ImportDateTimePropertyL(aParser, KVersitTokenDUE, endDate, relativeTime))
		{
		if (!ImportDateTimePropertyL(aParser, KVersitTokenDTEND, endDate, relativeTime))
			{
			endDate = Time::NullTTime();
			}
		}
	
	if ( ! ImportDateTimePropertyL(aParser, KVersitTokenDTSTART, startDate, relativeTime))
		{
		startDate = endDate;
		}

	if (startDate > endDate && endDate != Time::NullTTime())
		{
		return EFalse;
		}



	if (endDate == Time::NullTTime())
		{
		endDate = startDate;
		}
	else if (startDate == Time::NullTTime() || startDate > endDate)
		{
		startDate = endDate;
		}

	TTime completedDate;
	if (ImportDateTimePropertyL(aParser, KVCalTokenCOMPLETED,completedDate, relativeTime))
		{
		TCalTime calcompletedDate;
		calcompletedDate.SetTimeUtcL(completedDate);
		iEntry->SetCompletedL(ETrue, calcompletedDate);
		}

	TCalTime caltimeStart;
	TCalTime caltimeEnd;
	
	if (relativeTime == TVersitDateTime::EIsUTC)
		{
		caltimeStart.SetTimeUtcL(startDate);
		caltimeEnd.SetTimeUtcL(endDate);
		}
	else
		{
		caltimeStart.SetTimeLocalFloatingL(startDate);
		caltimeEnd.SetTimeLocalFloatingL(endDate);
		}
	iEntry->SetStartAndEndTimeL(caltimeStart,caltimeEnd);

// Attempt to import alarm properties
	ImportAlarmPropertyL(aParser, endDate, relativeTime);
		
// Attempt to import repeat properties
	ImportRepeatPropertyL(aParser, KVersitTokenRRULE, endDate, relativeTime);
	
	return ETrue;
	}

// Import a date/time 
// This method returns true if a date/time exists and is within the standard agenda date range
// If it doesn't exist or the date isn't valid this method returns EFalse. 
//
TBool CVCalToAgendaEntryConverter::ImportDateTimePropertyL(CVersitParser& aParser, const TDesC8& aToken, TTime& aValue, TVersitDateTime::TRelativeTime& aRelativeTime)
	{
	// get properties but don't take ownership of the elements of the array
	CArrayPtr<CParserProperty>* properties = aParser.PropertyL(aToken, TUid::Uid(KVersitPropertyDateTimeUid), EFalse);

	if (properties)
		{
		CleanupStack::PushL(properties);
		TVersitDateTime* time = static_cast<CParserPropertyValueDateTime*>((*properties)[0]->Value())->Value();
		aValue =  TTime(time->iDateTime);
		TCalTime calTime;
		if (IsValidTime(aValue))
			{
			aRelativeTime = time->iRelativeTime;
			if  (iTzZone && ( aRelativeTime== TVersitDateTime::EIsVCardLocal))
				{
				//Convert to utc time using imported TZ rules
				iTzZone->ConvertToUtcL(aValue);
				aRelativeTime = TVersitDateTime::EIsUTC;
				}
			CleanupStack::PopAndDestroy(properties);
			return ETrue;
			}

		CleanupStack::PopAndDestroy(properties);
		}
	
	return EFalse;
	}

// Import an integer property
//
TBool CVCalToAgendaEntryConverter::ImportIntegerPropertyL(CVersitParser& aParser, const TDesC8& aToken, TInt& aValue)
	{
	// get properties but don't take ownership of the elements of the array
	CArrayPtr<CParserProperty>* properties = aParser.PropertyL(aToken, TUid::Uid(KVersitPropertyIntUid), EFalse);

	if (properties)
		{
		aValue = static_cast<CParserPropertyValueInt*>((*properties)[0]->Value())->Value();
		delete properties; 
		return ETrue;
		}
	else
		{
		// If there is no property available, return EFalse to signal that this property doesn't exist
		return EFalse;
		}
	}

// Import a descriptor property
//
HBufC* CVCalToAgendaEntryConverter::ImportDesPropertyL(CVersitParser& aParser, const TDesC8& aToken)
	{
	// get properties but don't take ownership of the elements of the array
	CArrayPtr<CParserProperty>* properties = aParser.PropertyL(aToken, TUid::Uid(KVersitPropertyHBufCUid), EFalse);
	// If there is no property available, return NULL to signal that this property doesn't exist
	HBufC* aHBufC = NULL;
	
	if (properties)
		{
		CleanupStack::PushL(properties);
		CParserPropertyValueHBufC* propertyValue = static_cast<CParserPropertyValueHBufC*>((*properties)[0]->Value());
		aHBufC=propertyValue->TakeValueOwnership();
		CleanupStack::PopAndDestroy(properties);
		if (aHBufC)
			{
			CleanupStack::PushL(aHBufC);
			HBufC* temp=AgnConvertUtil::EncodeL(aHBufC->Des(),KUidTextToEtextNoTrim);
			CleanupStack::PopAndDestroy(aHBufC);
			aHBufC=temp;
			}
		}
	return aHBufC;
	}

// Import an alarm property
//
TBool CVCalToAgendaEntryConverter::ImportAlarmPropertyL(CVersitParser& aParser, TTime& aEntryTime, TVersitDateTime::TRelativeTime& aRelativeTime)
	{
	// Get properties but don't take ownership of the elements of the array. 
	// Check if there is an AALARM first.
	CArrayPtr<CParserProperty>* properties = aParser.PropertyL(KVersitTokenAALARM, 
	    TUid::Uid(KVCalPropertyAlarmUid), EFalse);

	// If there is no AALARM then try with a DALARM. They are treated the same.
	if (!properties)
	    {
	    properties = aParser.PropertyL(KVersitTokenDALARM, 
	        TUid::Uid(KVCalPropertyAlarmUid), EFalse);
	    }

	if (properties)
		{
		CleanupStack::PushL(properties);
		CParserPropertyValueAlarm* alarmProperty = static_cast<CParserPropertyValueAlarm*>((*properties)[0]->Value());
		CVersitAlarm* alarm = alarmProperty->Value();
		if (!alarm->iRunTime)
			{
			CleanupStack::PopAndDestroy(properties);
			return EFalse;
			}
		TTime alarmTime	= TTime(alarm->iRunTime->iDateTime);
		TVersitDateTime::TRelativeTime relativeTime = alarm->iRunTime->iRelativeTime;
		if (iTzZone && (alarm->iRunTime->iRelativeTime == TVersitDateTime::EIsVCardLocal))
			{
			//Convert to utc time using imported TZ rules
			iTzZone->ConvertToUtcL(alarmTime);
			relativeTime = TVersitDateTime::EIsUTC;
			}

		if(aEntryTime==Time::NullTTime())
			{//It is only possible if it is a undated doto entry, in which, the due time should be set before the alarm time is set
			__ASSERT_DEBUG(iEntry->EntryTypeL()==CCalEntry::ETodo, Panic(EAgnVersitWrongAttribute));

			aEntryTime = alarmTime;	
			aRelativeTime = relativeTime;
			
			TCalTime endTime;
			if (relativeTime==TVersitDateTime::EIsVCardLocal || relativeTime==TVersitDateTime::EIsMachineLocal)
				{
				endTime.SetTimeLocalFloatingL(aEntryTime);
				}
			else
				{
				__ASSERT_DEBUG(relativeTime==TVersitDateTime::EIsUTC, Panic(EAgnVersitPanicWrongTimeType));
				endTime.SetTimeUtcL(aEntryTime);
				}
			iEntry->SetStartAndEndTimeL(iEntry->StartTimeL(), endTime);
			}
	
		
		TTimeIntervalMinutes minutes;
		aEntryTime.MinutesFrom(alarmTime, minutes);
		
		CCalAlarm* calalarm = CCalAlarm::NewL();
		CleanupStack::PushL(calalarm);

		const TInt KMinValidAlarmTime = -1440; // can have an alarm up to one day after an event
		if(minutes.Int() >= KMinValidAlarmTime)
			{
			calalarm->SetTimeOffset(minutes);
			}
		else
			{
			// Alarm run-time is not valid - return EFalse since there is no valid alarm property
			CleanupStack::PopAndDestroy(2,properties);
			return EFalse;
			}

		HBufC* soundName=NULL;
		// Check if the sound type is an Epoc Sound
		CParserParam* param = ((*properties)[0])->Param(KVCalToken8TYPE);
		if (param)
			{
			if (param->Value() == KVCalValue8EPOCSOUND)
				{
				soundName = alarm->iAudioContent;
				if (soundName)
					{
					calalarm->SetAlarmSoundNameL(*soundName);
					}
				}
			}
			
		TBool hasExtenedAlarm =	ImportExtendedAlarmPropertyL(calalarm, aParser, KVersitTokenXALARM);
	
		if (!soundName && !hasExtenedAlarm)
			{
			TBaSystemSoundType type(KSystemSoundAlarmUID);
			TBaSystemSoundInfo info;
			RFs s;
			User::LeaveIfError(s.Connect());
			BaSystemSound::GetSound(s,type,info); //don't check err as info will always be filled in
			s.Close();
			calalarm->SetAlarmSoundNameL(info.FileName());
			}

		iEntry->SetAlarmL(calalarm);
		
		CleanupStack::PopAndDestroy(2,properties);
		return ETrue;
		}
	else
		{
		// If there is no property available, return EFalse to signal that this property doesn't exist
		return EFalse;
		}
	}
	
// Import an extended alarm property (X-EPOCALARM) - containing the rich alarm data; it does not import the 
// timing control parameters because the AALARM property would have been imported and written to the agenda 
// model entry; no need to re-populate the timing controls since both audio sound and extended alarm share the 
// same timing controls. The audio alarm timing controls override the rich alarm's timing controls (not used).
TBool CVCalToAgendaEntryConverter::ImportExtendedAlarmPropertyL(CCalAlarm* aAlarm, CVersitParser& aParser, const TDesC8& aToken )
	{
	// get properties but don't take ownership of the elements of the array
	CArrayPtr<CParserProperty>* properties = aParser.PropertyL(aToken, TUid::Uid(KVCalPropertyExtendedAlarmUid), EFalse);

	if (properties)
		{
		CleanupStack::PushL(properties);
		CParserPropertyValueExtendedAlarm* extendedAlarmProperty = static_cast<CParserPropertyValueExtendedAlarm*>((*properties)[0]->Value());
		CVersitExtendedAlarm* extendedAlarm = extendedAlarmProperty->Value();
		// get properties but don't take ownership of the elements of the array
		CArrayPtr<CParserProperty>* audioAlarmProperties = aParser.PropertyL(KVersitTokenAALARM, TUid::Uid(KVCalPropertyAlarmUid), EFalse);

		if (extendedAlarm && audioAlarmProperties)
			{
			CleanupStack::PushL(audioAlarmProperties);

			// Convert X-EPOC-ALARM to alarm content and add to entry
			CCalContent* alarmAction = CCalContent::NewL();
			CleanupStack::PushL(alarmAction);
			HBufC8* content = NULL;
			if (extendedAlarm->iContent != NULL)
				{
				content = extendedAlarm->iContent->AllocL();
				CleanupStack::PushL(content);
				}
			HBufC8* mimeType = NULL;
			if (extendedAlarm->iMimeType != NULL)
				{
				mimeType = extendedAlarm->iMimeType->AllocL();
				CleanupStack::PushL(mimeType);
				}
			alarmAction->SetContentL(content, mimeType, static_cast<CCalContent::TDisposition>(extendedAlarm->iDisposition)); //Takes ownership of content & mimeType

			if (mimeType != NULL)
				{
				CleanupStack::Pop(mimeType);
				}
			if (content != NULL)
				{
				CleanupStack::Pop(content);
				}	
			aAlarm->SetAlarmAction(alarmAction); // takes ownership of alarmAction
			CleanupStack::Pop(alarmAction);
			
			CleanupStack::PopAndDestroy(audioAlarmProperties);
			CleanupStack::PopAndDestroy(properties);
			return ETrue;
			}
		else
			{
			// If there is no rich alarm available, return EFalse to signal that this property doesn't exist
			CleanupStack::PopAndDestroy(properties);
			return EFalse;
			}
		}
	else
		{
		// If there is no property available, return EFalse to signal that this property doesn't exist
		return EFalse;
		}
	}

void CVCalToAgendaEntryConverter::ImportAttachmentPropertyL(CVersitParser& aParser)
	{
	// get properties but don't take ownership of the elements of the array
	CArrayPtr<CParserProperty>* properties = aParser.PropertyL(KVersitTokenATTACH, KNullUid, EFalse);//We past KNullUid since we don't know whether it is a binary or URI - versit will have the informantion.

	if (!properties)
		{
		return;
		}

	CleanupStack::PushL(properties);
	const TInt KCount = properties->Count();	// no. of attachments
	for (TInt i = 0; i < KCount; ++i)
		{
		CParserParam* attachValueType = (*properties)[i]->Param(KVersitTokenVALUE);
		TPtrC8 valueType;
		if(attachValueType && attachValueType->Value().Length() > 0)
			{
			valueType.Set(attachValueType->Value());
			}
		
		CCalAttachment* attachment = NULL;
		if(!attachValueType || valueType == KVersitTokenINLINE || valueType == KVersitTokenBINARY)
			{
			CParserPropertyValueBinary* attachValueBinary = static_cast<CParserPropertyValueBinary*>((*properties)[i]->Value());
			if (attachValueBinary)
				{
				CBufSeg* binary=(CBufSeg*)(attachValueBinary->Value());
				const TInt KBinaryLength = binary->Size();
				if(KBinaryLength>0)
					{
					HBufC8* bufValue = HBufC8::NewLC(KBinaryLength);
					TPtr8 pBufValue = bufValue->Des();
					binary->Read(0, pBufValue, KBinaryLength);
					attachment = CCalAttachment::NewFileL(bufValue);
					CleanupStack::Pop(bufValue);
					}
				}
			}
		else 
			{
			CParserPropertyValueHBufC* attachValueDes = static_cast<CParserPropertyValueHBufC*>((*properties)[i]->Value());
			if (attachValueDes && attachValueDes->Value().Length() > 0)
				{
				TPtrC des16Value = attachValueDes->Value();
				HBufC8* buf8Value = HBufC8::NewLC(des16Value.Length());
				buf8Value->Des().Copy(des16Value);
				if ( ! valueType.Compare(KVCalContentValueContentId) || ! valueType.Compare(KVCalContentValueContentIdShort))
					{
					attachment = CCalAttachment::NewFileByContentIdL(*buf8Value);
					}
				else
					{
					attachment = CCalAttachment::NewUriL(*buf8Value);
					}
				CleanupStack::PopAndDestroy(buf8Value);
				}
			}
		if (attachment)
			{
			CleanupStack::PushL(attachment);
			//Set paramenters
			CParserParam* mimeTypeParam = (*properties)[i]->Param(KVersitAttachMimeType);	
			if(mimeTypeParam && mimeTypeParam->Value().Length() > 0)
				{
				attachment->SetMimeTypeL(mimeTypeParam->Value());
				}
				
			CParserParam* attachName = (*properties)[i]->Param(KVersitAttachLabel);
			if(attachName && attachName->Value().Length() > 0)
				{
				TPtrC8 attachnameNarror = attachName->Value();
				HBufC* attachname16 = HBufC::NewLC(attachnameNarror.Length());
				TPtr pattachname16 = attachname16->Des();
				User::LeaveIfError(CnvUtfConverter::ConvertToUnicodeFromUtf8(pattachname16, attachnameNarror));
				attachment->SetLabelL(*attachname16);
				CleanupStack::PopAndDestroy(attachname16);
				}

			CParserParam* attachDate = (*properties)[i]->Param(KVCalAttachFileDate);
			if(attachDate && attachment->FileAttachment() && attachDate->Value().Length() > 0)
				{
				TPtrC8 pAttachDate = attachDate->Value();
				TBuf<KMaxTimeStringSize>date16; 
				date16.Copy(pAttachDate);
				TVersitDateTime* datetime = aParser.DecodeDateTimeL(date16);
				CleanupStack::PushL(datetime);
				TTime fileTtime(datetime->iDateTime);
				if(iTzZone && datetime->iRelativeTime != TVersitDateTime::EIsUTC)
					{
					iTzZone->ConvertToUtcL(fileTtime);
					}
				attachment->FileAttachment()->SetLastModifiedTimeUtc(fileTtime);
				CleanupStack::PopAndDestroy(datetime);
				}
			iEntry->AddAttachmentL(*attachment);	
			CleanupStack::Pop(attachment);
			}
		}// for loop
		
	CleanupStack::PopAndDestroy(properties);
	}

// Import repeat property
//
void CVCalToAgendaEntryConverter::ImportRepeatPropertyL(CVersitParser& aParser, const TDesC8& aToken, const TTime& aRepeatTime, TVersitDateTime::TRelativeTime aRelativeTime)
	{
	if (aRepeatTime != Time::NullTTime())
		{
		ImportRepeatRuleL(aParser, aToken, aRepeatTime, aRelativeTime);
	
		//check for RDates - this operation must be carried out after any rule has been imported
		ImportRDatePropertyL(aParser, KVersitTokenRDATE);

		ImportRepeatExceptionPropertiesL(aParser, KVersitTokenEXDATE);
		}
	}

// Repeat end date behaviour
// - If the repeat end date is within the agenda range it is set
// - If the repeat end date is greater than the maximum agenda date then the entry is set to repeat forever
// - If the repeat end date is less than the minimum agenda date then the entry is made non-repeating
//
// Current implementation follows these policies:
//	1) time mode of repeat always matches time mode of the entry:
//		if entry's time mode is EFloating, then repeat's time mode is EFloating;
//		if entry's time mode is EFixedUTC, then repeat's time mode is EFixedTimeZone.
//	2) time format of DTSTART determines time mode of the entry. 
//		if DTSTART is not valid, then time format of DTEND/DUE is used.
//
void CVCalToAgendaEntryConverter::ImportRepeatRuleL(CVersitParser& aParser, const TDesC8& aToken, const TTime& aStartDate, TVersitDateTime::TRelativeTime aRelativeTime)
	{
	// get properties but don't take ownership of the elements of the array
	CArrayPtr<CParserProperty>* properties = aParser.PropertyL(aToken, TUid::Uid(KVCalPropertyRecurrenceUid), EFalse);
	if (properties)
		{
		CleanupStack::PushL(properties);
		CVersitRecurrence* recurrence = static_cast<CParserPropertyValueRecurrence*>((*properties)[0]->Value())->Value();
		if (!recurrence)
			{
			CleanupStack::PopAndDestroy(properties);
			return;
			}
			
	//Importing the start time of the repeating
		//startTimeLoc & startTimeUtc will be same if it is floating entry or when iTzZone is not specified.
		TTime startTimeLoc = aStartDate;
		TTime startTimeUtc = aStartDate;
		TBool isFloatingEntry = (!iTzZone && aRelativeTime != TVersitDateTime::EIsUTC); 
		
		if  (iTzZone)
			{
			if (aRelativeTime == TVersitDateTime::EIsUTC)
				{
				iTzZone->ConvertToLocalL(startTimeLoc);
				}
			else
				{
				iTzZone->ConvertToUtcL(startTimeUtc);
				}	
			}
		
		TCalRRule repeat;
		TCalTime calstart;

		if (isFloatingEntry)
			{
			calstart.SetTimeLocalFloatingL(startTimeLoc);
			}
		else
			{
			calstart.SetTimeUtcL(startTimeUtc);
			}
			
	repeat.SetDtStart(calstart);
	
	//Importing the recurrence of the repeating			
		switch(recurrence->iRepeatType)
			{
			case CVersitRecurrence::EDaily:
				repeat.SetType(TCalRRule::EDaily);
				break;
			case CVersitRecurrence::EYearlyByMonth:
				repeat.SetType(TCalRRule::EYearly);
				break;
			case CVersitRecurrence::EWeekly:
				ImportWeeklyRepeatPropertiesL(static_cast<CVersitRecurrenceWeekly*>(recurrence), repeat, startTimeLoc.DayNoInWeek());
				break;
			case CVersitRecurrence::EMonthlyByPos:
				ImportMonthlyByPosRepeatPropertiesL(static_cast<CVersitRecurrenceMonthlyByPos*>(recurrence), repeat, startTimeLoc);
				break;
			case CVersitRecurrence::EMonthlyByDay:
				ImportMonthlyByDayRepeatPropertiesL(static_cast<CVersitRecurrenceMonthlyByDay*>(recurrence), repeat, startTimeLoc.DayNoInMonth());
				break;
			case CVersitRecurrence::EYearlyByDay:
				// Not supported, since an event cannot repeat on the 100th day of a year, etc
				// -- intentional drop through to default case --
			default:
				// Don't understand the repeat type,
				// so cleanup and exit
				CleanupStack::PopAndDestroy(properties);
				return;
			}

		// We allow only TVersitDateTime::EIsUTC and EIsVCardLocal, because 
		// EIsMachineLocal is never returned by parser,and EIsCorrect is not used
		__ASSERT_DEBUG(aRelativeTime == TVersitDateTime::EIsUTC || 
					   aRelativeTime == TVersitDateTime::EIsVCardLocal, Panic(EAgnVersitPanicWrongTimeType));

	//Importing the until time of the repeating
		TVersitDateTime* rptEndDate = recurrence->iEndDate;
		TCalTime calUntil;
		TBool isUntilTimeValid = EFalse;
	
		if(!rptEndDate && recurrence->iDuration == 0)
			{//If no until time has been set and the count is 0, the repeating is forever
			calUntil.SetTimeUtcL(TCalTime::MaxTime());
			repeat.SetCount(0);
			isUntilTimeValid = ETrue;
			}
		else if(rptEndDate)
			{//Set the until time
			TDateTime untilDateTimeLoc = rptEndDate->iDateTime;
			//Adjust the local repeating until time according to the local start time 
			if (!isFloatingEntry && iTzZone && rptEndDate->iRelativeTime == TVersitDateTime::EIsUTC)
				{
				TTime time(untilDateTimeLoc);//create a temp object of TTime
				iTzZone->ConvertToLocalL(time);//convert the UTC until to local
				untilDateTimeLoc = time.DateTime();// set the local until
				}
			
			untilDateTimeLoc.SetHour(startTimeLoc.DateTime().Hour());
			untilDateTimeLoc.SetMinute(startTimeLoc.DateTime().Minute());
			untilDateTimeLoc.SetSecond(startTimeLoc.DateTime().Second());
			untilDateTimeLoc.SetMicroSecond(0);
			isUntilTimeValid = ETrue;
			
			if (isFloatingEntry)
				{
				if (TTime(untilDateTimeLoc) < TCalTime::MinTime())
					{
					isUntilTimeValid = EFalse;
					}
				else 
					{
					if (TTime(untilDateTimeLoc) > TCalTime::MaxTime())
						{
						untilDateTimeLoc = TCalTime::MaxTime().DateTime();
						}
					calUntil.SetTimeLocalFloatingL(untilDateTimeLoc);
					}
				}
			else 
				{
				TTime untilUtc(untilDateTimeLoc);
				if (iTzZone)
					{
					iTzZone->ConvertToUtcL(untilUtc);
					}
					
				if (untilUtc < TCalTime::MinTime())
					{
					isUntilTimeValid = EFalse;
					}
					
				else
					{
					if (untilUtc > TCalTime::MaxTime())
						{
						untilUtc = TCalTime::MaxTime().DateTime();
						}
					calUntil.SetTimeUtcL(untilUtc);
					}
				}
			
			}
			
		if (isUntilTimeValid)
			{
			repeat.SetUntil(calUntil);
			}
		
		if (recurrence->iDuration>0)
			{
			TInt instancesPerRepeat = CalculateInstancesPerRepeatL(repeat);
			
			TInt count = recurrence->iDuration * instancesPerRepeat;
			
			// it is possible to have both count and untiltime in an imported vcal
			//	we will import the shortest repeat of the two dates
			if(repeat.Until().TimeLocalL() != Time::NullTTime())
				{
				TCalTime countUntilTime = iEntry->FindRptUntilTimeL(count);
						
				if(countUntilTime.TimeLocalL() < repeat.Until().TimeLocalL())
					{
					//	if until time is already set, it will be replaced
					repeat.SetUntil(countUntilTime);
					}
				}
			else
				{
				repeat.SetCount(count);
				}
				
			isUntilTimeValid = ETrue;
			}
	
		if (isUntilTimeValid)
			{
			//Importing the interval		
			repeat.SetInterval(recurrence->iInterval);
			
			//Importing the repeating count
			
			TRAPD(err, iEntry->SetRRuleL(repeat));
			if (err != KErrArgument && err != KErrNotSupported)
				{
				User::LeaveIfError(err);
				}
				
			TCalRRule ruleGot;
			if(iEntry->GetRRuleL(ruleGot))
				{
				if(iTzZone)
					{
					// If there are DAYLIGHT and TZ properties, use these rules when converting
					// repeating dates to UTC for storage, otherwise the time zone device will be used
					// instead.
					iEntry->SetTzRulesL(*iTzZone);
					}
				}
			}

		CleanupStack::PopAndDestroy(properties);
		}
	}

TInt CVCalToAgendaEntryConverter::CalculateInstancesPerRepeatL(const TCalRRule& aRRule) const
	{
	TInt instancesPerRepeat = 1;
	switch (aRRule.Type())
		{
		case TCalRRule::EWeekly:
			{
			RArray<TDay> wkdays;
			CleanupClosePushL(wkdays);
			aRRule.GetByDayL(wkdays);
			instancesPerRepeat = wkdays.Count();
			CleanupStack::PopAndDestroy(&wkdays);
			}
			break;
		case TCalRRule::EMonthly:
			{
			RArray<TCalRRule::TDayOfMonth> monthdays;
			CleanupClosePushL(monthdays);
			aRRule.GetByDayL(monthdays);
			instancesPerRepeat = monthdays.Count();
			CleanupStack::PopAndDestroy(&monthdays);
			if (instancesPerRepeat == 0)
				{
				RArray<TInt> monthdates;
				CleanupClosePushL(monthdates);
				aRRule.GetByMonthDayL(monthdates);
				instancesPerRepeat = monthdates.Count();
				CleanupStack::PopAndDestroy(&monthdates);
				}
			}
			break;
		case TCalRRule::EYearly:
			{
			RArray<TCalRRule::TDayOfMonth> yrdays;
			CleanupClosePushL(yrdays);
			aRRule.GetByDayL(yrdays);
			instancesPerRepeat = yrdays.Count();
			CleanupStack::PopAndDestroy(&yrdays);
			if (instancesPerRepeat == 0)
				{
				RArray<TMonth> yrmonths;
				CleanupClosePushL(yrmonths);
				aRRule.GetByMonthL(yrmonths);
				instancesPerRepeat = yrmonths.Count();
				CleanupStack::PopAndDestroy(&yrmonths);
				}
			}
			break;
		case TCalRRule::EDaily:
		default:
			// do nothing
			break;
		}
	if (instancesPerRepeat == 0)
		{
		instancesPerRepeat = 1;
		}
	return instancesPerRepeat;
	}

void CVCalToAgendaEntryConverter::ImportWeeklyRepeatPropertiesL(CVersitRecurrenceWeekly* aRepeat, TCalRRule& aRpt, TDay aDefaultDayOfWeek)
	{
	aRpt.SetType(TCalRRule::EWeekly);
	CWeekDayArray* weekdayArray = aRepeat->iArrayOfWeekDayOccurrences;
	RArray<TDay> daysinweek;
	CleanupClosePushL(daysinweek);

	if (weekdayArray)
		{
		CArrayFix<TDay>* dayList = weekdayArray->iArray;

		TInt size = dayList->Count();
		for (TInt count=0; count<size; count++)
			{
			TDay day = (*dayList)[count];
			daysinweek.AppendL(day);
			}
		}
	else
		{
		daysinweek.AppendL(aDefaultDayOfWeek);
		}
		
	aRpt.SetByDay(daysinweek);
	CleanupStack::PopAndDestroy();//close rarray
	}


void CVCalToAgendaEntryConverter::ImportMonthlyByDayRepeatPropertiesL(CVersitRecurrenceMonthlyByDay* aRepeat, TCalRRule& aRpt, TInt aDefaultDayOfMonth)
	{
	aRpt.SetType(TCalRRule::EMonthly);

	// Specify which dates to repeat on
	// Only days from the start of the month are supported
	CArrayFix<TInt>* dayList = aRepeat->iArrayOfOccurrencesInDaysFromStartOfMonth;
	CArrayFix<TInt>* endDayList = aRepeat->iArrayOfOccurrencesInDaysFromEndOfMonth;
	RArray<TInt> monthdays;
	CleanupClosePushL(monthdays);
	if (dayList)
		{
		TInt size = dayList->Count();
		for (TInt count=0; count<size; count++)
			{
			TInt day = (*dayList)[count]-1;			
			// Set only valid dates
			// 0 = the first day of a month, 30 = the 31st day of the month
			if (day >= 0 && day <= 30)
				{
				monthdays.AppendL(day);
				}
			}
		}
	else if(endDayList) 
		{ 
		TInt size = endDayList->Count(); 
		for (TInt count=0;count<size;++count) 
        	{ 
        	TInt day = (*endDayList)[count]; 
       		 if(day==1) 
	       	 	{	//set 31st day in the monthlyByDates list as its the last day of the month
	       	 	monthdays.AppendL(30); 
	        	break; 
	       	 	} 
        	}
		}
	
	
	if(monthdays.Count()<1)	
		{ 
		monthdays.AppendL(aDefaultDayOfMonth); 
		} 
		
	aRpt.SetByMonthDay(monthdays);
	CleanupStack::PopAndDestroy();//close RArray
	}


void CVCalToAgendaEntryConverter::ImportMonthlyByPosRepeatPropertiesL(CVersitRecurrenceMonthlyByPos* aRepeat, TCalRRule& aRpt, const TTime& aDate)
	{
	aRpt.SetType(TCalRRule::EMonthly);

	TInt size = aRepeat->iMonthPositions->Count();
	RArray<TCalRRule::TDayOfMonth> dayofmonthArray;
	CleanupClosePushL(dayofmonthArray);
	
	if (size > 0)
		{
		for (TInt count=0; count<size; count++)
			{
			CArrayFix<TDay>* dayList = (*(aRepeat->iMonthPositions))[count]->iArrayOfWeekDays->iArray;
			TInt weekNo = (*(aRepeat->iMonthPositions))[count]->iWeekNo;
			TInt sign = (*(aRepeat->iMonthPositions))[count]->iSign;

			if (weekNo == 1 && sign == CVersitRecurrenceMonthlyByPos::CMonthPosition::EWeeksFromEndOfMonth)
				{
				// If the poisiton is in the last week of the month
				weekNo = -1;
				}
			else if (weekNo > 5 || sign == CVersitRecurrenceMonthlyByPos::CMonthPosition::EWeeksFromEndOfMonth)
				{
				// We can't model this setting, so ignore it
				continue;
				}

			// Now set the days for the week setting
			TInt dayCount = dayList->Count();
			for (TInt ii=0; ii<dayCount; ii++)
				{
				TDay day = (*dayList)[ii];
				TCalRRule::TDayOfMonth monthday(day, weekNo);
				dayofmonthArray.AppendL(monthday);
				}
			}
		}
	else
		{
		TDay day = aDate.DayNoInWeek();
		TInt week = aDate.DayNoInMonth() / 7 + 1;
		TCalRRule::TDayOfMonth monthday(day, week);
		dayofmonthArray.AppendL(monthday);
		}

	aRpt.SetByDay(dayofmonthArray);
	CleanupStack::PopAndDestroy();//close the RArray
	}

void CVCalToAgendaEntryConverter::ImportRepeatExceptionPropertiesL(CVersitParser& aParser, const TDesC8& aToken)
	{
	// get properties but don't take ownership of the elements of the array
	CArrayPtr<CParserProperty>* properties = aParser.PropertyL(aToken, TUid::Uid(KVersitPropertyMultiDateTimeUid), EFalse);

	if (properties)
		{
		CleanupStack::PushL(properties);
		CArrayPtr<TVersitDateTime>* exDates = static_cast<CParserPropertyValueMultiDateTime*>((*properties)[0]->Value())->Value();
		if (exDates)
			{
			TInt size = exDates->Count();
			RArray<TCalTime> exceptionlist;
			CleanupClosePushL(exceptionlist);
			
			for (TInt count=0; count<size; count++)
				{
				TCalTime exception;
				TVersitDateTime* time = (*exDates)[count];
				TTime timeProperty(time->iDateTime);
				if (IsValidTime(timeProperty))
					{
					TVersitDateTime::TRelativeTime relativeTime = time->iRelativeTime;
					
					if  (iTzZone && (relativeTime == TVersitDateTime::EIsVCardLocal))
						{
						//Convert to utc time
						iTzZone->ConvertToUtcL(timeProperty);
						relativeTime = TVersitDateTime::EIsUTC;
						}
				
					if (iEntry->StartTimeL().TimeMode() == TCalTime::EFloating)
						{
						exception.SetTimeLocalFloatingL(timeProperty);
						}
					else
						{
						// the EXDATE is floating but the entry is UTC
						exception.SetTimeUtcL(timeProperty);
						}

					exceptionlist.AppendL(exception);
					}
				}
			iEntry->SetExceptionDatesL(exceptionlist);
			CleanupStack::PopAndDestroy();//close the RArray
			}
		CleanupStack::PopAndDestroy(properties);
		}
	}

void CVCalToAgendaEntryConverter::ImportAttendeePropertiesL(CVersitParser& aParser, const TDesC8& aToken)
	{
	TBool mailto;
	TInt propLen;
	// get properties but don't take ownership of the elements of the array
	CArrayPtr<CParserProperty>* properties = aParser.PropertyL(aToken, TUid::Uid(KVersitPropertyHBufCUid), EFalse);
	CleanupStack::PushL(properties);

	if (properties)
		{
		TInt size = properties->Count();	// no. of attendees
		for (TInt count=0; count<size; count++)
			{
			CParserPropertyValueHBufC* attendeeProperty = static_cast<CParserPropertyValueHBufC*>((*properties)[count]->Value()); 

			mailto = EFalse;
			propLen = 0;
			if (attendeeProperty->Value().FindF(KVersitTokenMailTo) == 0)
				{
				mailto = ETrue;
				propLen = attendeeProperty->Value().Length() - KVersitTokenMailTo().Length();
				}
			
			CParserParam* sentBy = (*properties)[count]->Param(KICalAttendeeSentBy8);	
		
			CCalAttendee* attendee = NULL;
			CCalUser* user = NULL;
			CParserParam* role = (*properties)[count]->Param(KVCalAttendee8ROLE);

			const TDesC& KAttendeeProperty = mailto ? attendeeProperty->Value().Right(propLen) : attendeeProperty->Value();
			if (sentBy)
				{
				HBufC* sentByDesc = sentBy->ValueL();
				CleanupStack::PushL(sentByDesc);
				if (role && role->Value() == KVCalAttendeeRole8ORGANIZER)
					{
					user = CCalUser::NewL(KAttendeeProperty, *sentByDesc);
					iEntry->SetOrganizerL(user);
					}
				else
					{
					attendee = CCalAttendee::NewL(KAttendeeProperty, *sentByDesc);
					iEntry->AddAttendeeL(attendee);
					}
				CleanupStack::PopAndDestroy(sentByDesc);
				}
			else
				{
				if (role && role->Value() == KVCalAttendeeRole8ORGANIZER)
					{
					user = CCalUser::NewL(KAttendeeProperty);
					iEntry->SetOrganizerL(user);
					}
				else
					{
					attendee = CCalAttendee::NewL(KAttendeeProperty);
					iEntry->AddAttendeeL(attendee);
					}
				}
	
			if (attendee)
				{
				CParserParam* calrole = (*properties)[count]->Param(KICalAttendeeCalRole8);
				TBool roleSet = EFalse;
				//	Check for iCal roles values
				if (calrole)
					{
 					if(calrole->Value() == KICalAttendeeCalRole8CHAIR)
						{
						attendee->SetRoleL(CCalAttendee::EChair);
						roleSet = ETrue;
  						}
					else if(calrole->Value() == KICalAttendeeCalRole8REQUIRED)
						{
						attendee->SetRoleL(CCalAttendee::EReqParticipant);
						roleSet = ETrue;
						}
					else if(calrole->Value() == KICalAttendeeCalRole8OPTIONAL)
						{
						attendee->SetRoleL(CCalAttendee::EOptParticipant);				
						roleSet = ETrue;
						}
					else if(calrole->Value() == KICalAttendeeCalRole8NONPARTICIPANT)
						{
						attendee->SetRoleL(CCalAttendee::ENonParticipant);				
						roleSet = ETrue;
						}
					}
				if ( ! roleSet && role)
					{
					if (role->Value() == KVCalAttendeeRole8OWNER)
						{
						attendee->SetRoleL(CCalAttendee::EVCalOwner);
						roleSet = ETrue;
						}
					else if (role->Value() == KVCalAttendeeRole8DELEGATE)
						{
						attendee->SetRoleL(CCalAttendee::EVCalDelegate);
						roleSet = ETrue;
						}
					else if (role->Value() == KVCalAttendeeRole8ATTENDEE)
						{
						attendee->SetRoleL(CCalAttendee::EVCalAttendee);
						roleSet = ETrue;
						}
					}
				if ( ! roleSet)
					{
					attendee->SetRoleL(CCalAttendee::EReqParticipant);
					}
			
				// Check for vCal status values
				CParserParam* calStatus = (*properties)[count]->Param(KICalAttendeeCalStatus8);
				CParserParam* status = (*properties)[count]->Param(KVCalAttendee8STATUS);
				if (calStatus)
					{
					if (calStatus->Value() == KVCalAttendeeStatus8ACCEPTED)
						attendee->SetStatusL(CCalAttendee::EAccepted);
					else if (calStatus->Value() == KVCalAttendeeStatus8NEEDSACTION)
						attendee->SetStatusL(CCalAttendee::ENeedsAction);
					else if (calStatus->Value() == KVCalAttendeeStatus8TENTATIVE)
						attendee->SetStatusL(CCalAttendee::ETentative);
					else if (calStatus->Value() == KVCalAttendeeStatus8CONFIRMED)
						attendee->SetStatusL(CCalAttendee::EConfirmed);
					else if (calStatus->Value() == KVCalAttendeeStatus8DECLINED)
						attendee->SetStatusL(CCalAttendee::EDeclined);
					else if (calStatus->Value() == KVCalAttendeeStatus8COMPLETED)
						attendee->SetStatusL(CCalAttendee::ECompleted);
					else if (calStatus->Value() == KVCalAttendeeStatus8DELEGATED)
						attendee->SetStatusL(CCalAttendee::EDelegated);
					else if (calStatus->Value() == KICalAttendeeCalStatus8INPROCESS)
						attendee->SetStatusL(CCalAttendee::EInProcess);
					}
				else if (status)
					{
					if (status->Value() == KVCalAttendeeStatus8ACCEPTED)
						attendee->SetStatusL(CCalAttendee::EAccepted);
					else if (status->Value() == KVCalAttendeeStatus8NEEDSACTION)
						attendee->SetStatusL(CCalAttendee::ENeedsAction);
					else if (status->Value() == KVCalAttendeeStatus8SENT)
						attendee->SetStatusL(CCalAttendee::EVCalSent);
					else if (status->Value() == KVCalAttendeeStatus8XDASHRECEIVED)
						attendee->SetStatusL(CCalAttendee::EVCalXReceived);
					else if (status->Value() == KVCalAttendeeStatus8TENTATIVE)
						attendee->SetStatusL(CCalAttendee::ETentative);
					else if (status->Value() == KVCalAttendeeStatus8CONFIRMED)
						attendee->SetStatusL(CCalAttendee::EConfirmed);
					else if (status->Value() == KVCalAttendeeStatus8DECLINED)
						attendee->SetStatusL(CCalAttendee::EDeclined);
					else if (status->Value() == KVCalAttendeeStatus8COMPLETED)
						attendee->SetStatusL(CCalAttendee::ECompleted);
					else if (status->Value() == KVCalAttendeeStatus8DELEGATED)
						attendee->SetStatusL(CCalAttendee::EDelegated);
					}
	
				// Check for RSVP parameter
				CParserParam* rsvp = (*properties)[count]->Param(KVCalAttendee8RSVP);
				if (rsvp)
					{
					if (rsvp->Value() == KVCalAttendeeRsvp8YES)
						attendee->SetResponseRequested(ETrue);
					else
						attendee->SetResponseRequested(EFalse);
					}
		
				// Check for EXPECT parameter
				CParserParam* expect = (*properties)[count]->Param(KVCalAttendee8EXPECT);
				if (expect)
					{
					if (expect->Value() == KVCalAttendeeExpect8FYI)
						attendee->SetVCalExpect(CCalAttendee::EVCalFyi);
					else if (expect->Value() == KVCalAttendeeExpect8REQUIRE)
						attendee->SetVCalExpect(CCalAttendee::EVCalRequire);
					else if (expect->Value() == KVCalAttendeeExpect8REQUEST)
						attendee->SetVCalExpect(CCalAttendee::EVCalRequest);
					else if (expect->Value() == KVCalAttendeeExpect8IMMEDIATE)
						attendee->SetVCalExpect(CCalAttendee::EVCalImmediate);
					}
				}
				
		//	Check for a common name 
			CParserParam* commonname = (*properties)[count]->Param(KICalAttendeeCommonName8);
			if (commonname)
				{
				HBufC* commonNameDesC = commonname->ValueL();
				CleanupStack::PushL(commonNameDesC);
				if(user)
					{
					user->SetCommonNameL(*commonNameDesC);
					}
				else if (attendee)
					{
					attendee->SetCommonNameL(*commonNameDesC);
					}
				CleanupStack::PopAndDestroy(commonNameDesC);
				}
		//Set phone owner
			CParserParam* phoneowner = (*properties)[count]->Param(KICalAttendee8XDASHPHONEOWNER);
			if(phoneowner)
				{
				if(user)
					{
					iEntry->SetPhoneOwnerL(user);
					}
				else if (attendee)
					{
					iEntry->SetPhoneOwnerL(attendee);
					}
				}
			}//for
		}//if property
		
	CleanupStack::PopAndDestroy(properties);
	}

// Import the categories property
// For each category, try and match the name of the category with the standard category types.
// If this fails, the category is assumed to be an extended category type and the "X-" prefix 
// is removed
//
void CVCalToAgendaEntryConverter::ImportCategoriesPropertyL(CVersitParser& aParser, const TDesC8& aToken)
	{
	// get properties but don't take ownership of the elements of the array
	CArrayPtr<CParserProperty>* properties = aParser.PropertyL(aToken, TUid::Uid(KVersitPropertyCDesCArrayUid), EFalse);
	if (properties)
		{
		CleanupStack::PushL(properties);
		CDesCArray* categories = static_cast<CParserPropertyValueCDesCArray*>((*properties)[0]->Value())->Value();
		for (TInt count=0; count < categories->Count(); count++)
			{
			TPtrC categoryName=categories->MdcaPoint(count);
			CCalCategory* category = NULL;
			//match category name to standard ones
			CCalCategory::TCalCategoryType type = (CCalCategory::TCalCategoryType)(-1);
			if (categoryName.CompareF(KVCalCategoriesAPPOINTMENT)==0)
				{
				type = CCalCategory::ECalAppointment;
				}
			else if(categoryName.CompareF(KVCalCategoriesBUSINESS)==0)
				{
				type = CCalCategory::ECalBusiness;
				}
			else if(categoryName.CompareF(KVCalCategoriesEDUCATION)==0)
				{
				type = CCalCategory::ECalEducation;
				}
			else if(categoryName.CompareF(KVCalCategoriesHOLIDAY)==0)
				{
				type = CCalCategory::ECalHoliday;
				}
			else if(categoryName.CompareF(KVCalCategoriesMEETING)==0)
				{
				type = CCalCategory::ECalMeeting;
				}
			else if(categoryName.CompareF(KVCalCategoriesMISCELLANEOUS)==0)
				{
				type = CCalCategory::ECalMiscellaneous;
				}
			else if(categoryName.CompareF(KVCalCategoriesPERSONAL)==0)
				{
				type = CCalCategory::ECalPersonal;
				}
			else if(categoryName.CompareF(KVCalCategoriesPHONECALL)==0)
				{
				type = CCalCategory::ECalPhoneCall;
				}
			else if(categoryName.CompareF(KVCalCategoriesSICKDAY)==0)
				{
				type = CCalCategory::ECalSickDay;
				}
			else if(categoryName.CompareF(KVCalCategoriesSPECIALOCCASION)==0)
				{
				type = CCalCategory::ECalSpecialOccasion;
				}
			else if(categoryName.CompareF(KVCalCategoriesTRAVEL)==0)
				{
				type = CCalCategory::ECalTravel;
				}
			else if(categoryName.CompareF(KVCalCategoriesVACATION)==0)
				{
				type = CCalCategory::ECalVacation;
				}
			if(type>-1)
				{
				category = CCalCategory::NewL(type);
				}
			else if(categoryName.Length() > 0)
				{
				if (categoryName.Find(KVCalTokenXDash) == 0)
					{
					categoryName.Set(categoryName.Right(categoryName.Length() - 2));
					}
					
				if(categoryName.Length() > 0)
					{
					category = CCalCategory::NewL(categoryName);	
					}
				}
				
			if(category)
				{
				// AddCategoryL() takes ownership at the end of the function call (after calling leaving functions)
				iEntry->AddCategoryL(category);
				}
			}
		CleanupStack::PopAndDestroy(properties);
		}
	}	
		



// Utility method to ensure proper cleanup in OOM 
//
void ResetAndDestroyArrayOfEntries(TAny* aObject)
	{
	CArrayPtrFlat<CCalEntry>* array=reinterpret_cast<CArrayPtrFlat<CCalEntry>*>(aObject);
	if (array)
		array->ResetAndDestroy();
	delete array;
	}


void CVCalToAgendaEntryConverter::SetImportVCalendarValues(TBool aImportVCalendar)
	{
	iImportVCalendarValues = aImportVCalendar;
	}

void CVCalToAgendaEntryConverter::ImportStatusPropertyL(CVersitParser& aParser, CCalEntry::TStatus& aStatus)
	{
	if (iImportVCalendarValues)
		{
		ImportVCalStatusPropertyL(aParser, aStatus);
		}
	else
		{
		ImportICalStatusPropertyL(aParser, aStatus);
		}
	}

void CVCalToAgendaEntryConverter::ImportICalStatusPropertyL(CVersitParser& aParser, CCalEntry::TStatus& aStatus)
	{
	HBufC* property=ImportDesPropertyL(aParser, KVCalTokenSTATUS);
	if (!property)
		{
		aStatus = CCalEntry::ENullStatus;
		return;
		}	
// BB mapped according to CCalEntryImpl::StatusL() which should be reconsidered 
	if (property->Length() ==0)  // Property value not found - use default setting
		{
		aStatus = CCalEntry::ENullStatus;
		}
	else if (property->CompareF(KVCalStatusNEEDSACTION) == 0)
		{
		if (iEntry->EntryTypeL()==CCalEntry::ETodo)
			{
			aStatus = CCalEntry::ETodoNeedsAction;
			}
		else
			{
			aStatus = CCalEntry::EConfirmed;
			}
		}
	else if (property->CompareF(KVCalStatusSENT) ==0 || property->CompareF(KVCalStatusDELEGATED)==0 || property->CompareF(KVCalStatusCONFIRMED) ==0 || property->CompareF(KVCalStatusACCEPTED) == 0)
		{
		if (iEntry->EntryTypeL()==CCalEntry::ETodo)
			{
			aStatus = CCalEntry::ETodoInProcess;
			}
		else
			{
			aStatus = CCalEntry::EConfirmed;
			}
	
		}
	else if (property->CompareF(KVCalStatusTENTATIVE)==0)
		{
		if (iEntry->EntryTypeL()==CCalEntry::ETodo)
			{
			aStatus = CCalEntry::ETodoNeedsAction;
			}
		else
			{
			aStatus = CCalEntry::ETentative;
			}
	
		}
	else if (property->CompareF(KVCalStatusDECLINED)==0)
		{
		aStatus = CCalEntry::ECancelled;
		}
	else if (property->CompareF(KVCalStatusCOMPLETED)==0)
		{
		if(iEntry->EntryTypeL()==CCalEntry::ETodo)
			{
			aStatus = CCalEntry::ETodoCompleted;
			}
		else
			{
			aStatus = CCalEntry::EConfirmed;
			}
		
		}
	else // No Match found so use the default
		aStatus = CCalEntry::ENullStatus;
	delete property;
	}

void CVCalToAgendaEntryConverter::ImportVCalStatusPropertyL(CVersitParser& aParser, CCalEntry::TStatus& aStatus)
	{
	HBufC* property=ImportDesPropertyL(aParser, KVCalTokenSTATUS);
	if (!property || property->Length() == 0)
		{
		aStatus = CCalEntry::ENullStatus;
		}
	else if (property->CompareF(KVCalStatusNEEDSACTION) == 0)
		{
		aStatus = CCalEntry::EVCalNeedsAction;
		}
	else if (property->CompareF(KVCalStatusSENT) == 0)
		{
		aStatus = CCalEntry::EVCalSent;
		}
	else if (property->CompareF(KVCalStatusDELEGATED)== 0)
		{
		aStatus = CCalEntry::EVCalDelegated;
		}
	else if (property->CompareF(KVCalStatusCONFIRMED) == 0 && iEntry->EntryTypeL() != CCalEntry::ETodo)
		{
		aStatus = CCalEntry::EConfirmed;
		}
	else if (property->CompareF(KVCalStatusACCEPTED) == 0)
		{
		aStatus = CCalEntry::EVCalAccepted;
		}
	else if (property->CompareF(KVCalStatusTENTATIVE)==0 && iEntry->EntryTypeL() != CCalEntry::ETodo)
		{
		aStatus = CCalEntry::ETentative;
		}
	else if (property->CompareF(KVCalStatusDECLINED)==0)
		{
		aStatus = CCalEntry::EVCalDeclined;
		}
	else if (property->CompareF(KVCalStatusCOMPLETED)==0 && iEntry->EntryTypeL() == CCalEntry::ETodo)
		{
		aStatus = CCalEntry::ETodoCompleted;
		}
	delete property;
	}

