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
 
#include <vcal.h>
#include <vutil.h>
#include <utf.h>
#include <charconv.h>
#include "verror.h"
#include <vstaticutils.h>
#include <vobserv.h>

#include <s32mem.h>


//
// CParserVCal
//

EXPORT_C CParserVCal* CParserVCal::NewL()
/** Allocates and constructs a vCalendar parser.

@return Pointer to the newly created vCalendar parser. */
	{
	CParserVCal* self=new(ELeave) CParserVCal();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;	
	}

CParserVCal::CParserVCal() :CVersitParser(ESupportsVersion)
	{
	iDefaultVersion = KVersitTokenVCalVersionNo;
	}

EXPORT_C void CParserVCal::InternalizeL(RReadStream& aStream)
/** Internalises a vCalendar entity from a read stream.

The presence of this function means that the standard templated operator>>() 
(defined in s32strm.h) is available to internalise objects of this class.

@param aStream Stream from which the vCalendar should be internalised. 
@see CVersitParser::InternalizeL() */
	{
	CVersitParser::InternalizeL(aStream);
	}


EXPORT_C void CParserVCal::ExternalizeL(RWriteStream& aStream)
/** Externalises a vCalendar entity (and all sub-entities) to a write stream. 

Sets the entity name to KVersitVarTokenVCALENDAR if it hasn't already been 
set.

Adds a version property to the start of the current entity's array of properties 
if the entity supports this. (If there isn't an array of properties then one 
is made).

The presence of this function means that the standard templated operator<<() 
(defined in s32strm.h) is available to externalise objects of this class.

@param aStream Stream to which the vCalendar should be externalised. 
@see CVersitParser::ExternalizeL() */
	{
	if (!iEntityName)
		SetEntityNameL(KVersitVarTokenVCALENDAR);
	CVersitParser::ExternalizeL(aStream);
	}

EXPORT_C CVersitParser* CParserVCal::MakeEntityL(TInt aEntityUid,HBufC* aEntityName)
// From CVersitParser
	{
	CVersitParser* newEntity = NULL;

	switch (aEntityUid)
		{
		case KVCalEntityUidVEvent:
		case KVCalEntityUidVTodo:
			newEntity=CParserVCalEntity::NewL();
			CleanupStack::PushL(newEntity);
			break;
		default: //Allows it to read (and ignore) additional iCalendar entities
			newEntity = new(ELeave) CVersitParser(ENoVersionProperty);
			CleanupStack::PushL(newEntity);
			newEntity->ConstructL();
			break;
		}
	if (iObserver)
		iObserver->NewParser(newEntity);
	newEntity->SetAutoDetect(iFlags&EUseAutoDetection,iAutoDetectCharSets);
	if (iFlags&EImportSyncML)
		newEntity->SetFlags(EImportSyncML);

	// make child entries aware of default char set
    if (iFlags&EUseDefaultCharSetForAllProperties)
		newEntity->SetFlags(EUseDefaultCharSetForAllProperties);
	newEntity->SetDefaultCharSet(DefaultCharSet());
	
	if (iPlugIn)
		{
		newEntity->SetPlugIn(iPlugIn);
		}

	newEntity->InternalizeL(aEntityName, iLineReader);
	
	CleanupStack::Pop(newEntity);
	return newEntity;
	}

EXPORT_C TUid CParserVCal::RecognizeToken(const TDesC8& aToken) const
// From CVersitParser
/** Returns a UID that identifies a specified token's type.

For example, if aToken contains the property name DAYLIGHT the function returns 
KVersitPropertyDaylightUid. If the token is not recognized as vCalendar-specific, 
the function calls CVersitParser::RecognizeToken(), which recognizes generic Versit 
tokens.

@param aToken The token to be recognized.
@return A defined UID value if the token has been recognized, KVersitTokenUnknownUid 
otherwise. */
	{
	TUid uid = KNullUid;
	TChar firstChar(aToken.Ptr()[0]);
	firstChar=firstChar.GetUpperCase();
	switch (firstChar)
		{
	case 'D':
		if (!aToken.CompareF(KVersitTokenDAYLIGHT))
			uid.iUid = KVersitPropertyDaylightUid;
		break;
	case 'P':
		if (!aToken.CompareF(KVersitTokenPRODID))
			uid.iUid = KVersitPropertyHBufCUid;
		break;
	case 'T':
		if (!aToken.CompareF(KVersitTokenTZ))
			uid.iUid = KVersitPropertyTimeZoneUid;
		break;
	case 'V':
		if (!aToken.CompareF(KVersitTokenVERSION))
			uid.iUid = KVersitTokenVersionUid;
		break;
	default:
		break;
		}
	if (uid == KNullUid)
		return CVersitParser::RecognizeToken(aToken);
	return uid;
	}

EXPORT_C TInt CParserVCal::RecognizeEntityName() const
// From CVersitParser
/** Tests the current value to see if it a vEvent or vTodo entity.

@return KVCalEntityUidVEvent if it is a vEvent entity; KVersitVarTokenVTODO 
if it is a vTodo entity; zero if there is no current property or it has no value. */
	{
	if (iCurrentProperty && iCurrentProperty->Value())
		{
		TPtrC entityName=STATIC_CAST(CParserPropertyValueHBufC*,iCurrentProperty->Value())->Value();
		if (entityName==KVersitVarTokenVEVENT)
			return KVCalEntityUidVEvent;
		else if (entityName==KVersitVarTokenVTODO)
			return KVCalEntityUidVTodo;
		}
	return 0;
	}

EXPORT_C void CParserVCal::Reserved1()
	{}

EXPORT_C void CParserVCal::Reserved2()
	{}

//
// CParserVCalEntity
//

EXPORT_C CParserVCalEntity* CParserVCalEntity::NewL()
/** Allocates and constructs a vCalendar sub-entity parser.

@return A pointer to the new vCalendar sub-entity parser. */
	{
	CParserVCalEntity* self=new(ELeave) CParserVCalEntity();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

CParserVCalEntity::CParserVCalEntity()
	: CRecurrenceParser(EFalse)
/** Constructs a vCalendar sub-entity parser. */
	{}

EXPORT_C CParserVCalEntity::~CParserVCalEntity()
/** The destructor is empty. */
	{
	}

EXPORT_C void CParserVCalEntity::ExternalizeL(RWriteStream& aStream)
/** Externalises a vTodo or vEvent to a write stream.

Sets the entity's name to KVersitVarTokenVEVENT if it hasn't already been 
set.

Converts all date/time values from machine-local into universal time. 

The presence of this function means that the standard templated operator<<() 
(defined in s32strm.h) is available to externalise objects of this class. 

@param aStream Stream to which the vTodo or vEvent should be externalised. */
	{
	if (!iEntityName)
		SetEntityNameL(KVersitVarTokenVEVENT);
	CVersitParser::ExternalizeL(aStream);
	}

EXPORT_C CParserPropertyValue* CParserVCalEntity::MakePropertyValueL(const TUid& aPropertyUid,HBufC16*& aValue)
// From CVersitParser
	{
	if(!aValue && !iLargeDataBuf)
		return NULL;

	switch (aPropertyUid.iUid)
		{
		case KVCalPropertyAlarmUid: // won't be recognized except by subclass
			{
			CVersitAlarm* alarm = MakePropertyValueAlarmL(aValue->Des());
			CleanupStack::PushL(alarm);
			CParserPropertyValue* value = new(ELeave) CParserPropertyValueAlarm(alarm);
			CleanupStack::Pop(alarm);
			return value;
			}
		case KVCalPropertyExtendedAlarmUid: // won't be recognized except by subclass
			{
			CVersitExtendedAlarm* richAlarm = NULL;
			if (aValue)
				{
				richAlarm = MakePropertyValueExtendedAlarmL(aValue->Des());
				}
			else
				{
				richAlarm = MakePropertyValueExtendedAlarmL(*iLargeDataBuf);
				}
			CleanupStack::PushL(richAlarm);
			CParserPropertyValue* value = new(ELeave) CParserPropertyValueExtendedAlarm(richAlarm);
			CleanupStack::Pop(richAlarm);
			return value;
			}
		case KVCalPropertyRecurrenceUid:
			{
			TPtr val=aValue->Des();
			return MakePropertyValueRecurrenceL(val);
			}
		default:
			break;
		};
	return CVersitParser::MakePropertyValueL(aPropertyUid,aValue);
	}

EXPORT_C CVersitAlarm* CParserVCalEntity::MakePropertyValueAlarmL(TPtr16 aAlarmValue)
//Make an Alarm property from stream
	{
	TPtrC8 propName=iCurrentProperty->Name();
	TPtr16 field(NULL,0);

	FindFirstField(field,aAlarmValue);
	TVersitDateTime* runTime=NULL;
	if(field.Length()>0)
		runTime = DecodeDateTimeL(field);
	CleanupStack::PushL(runTime);
	
	FindFirstField(field,aAlarmValue);
	TTime* snoozeTime = NULL;
	if(field.Length()>0)
		snoozeTime = DecodeTimePeriodL(field);
	CleanupStack::PushL(snoozeTime);

	FindFirstField(field,aAlarmValue);
	TInt repeatCount=0;
	if(field.Length()>0)
		Val(field, repeatCount);

	if (propName==KVersitTokenAALARM || propName==KVersitTokenMALARM)
		FindFirstField(field,aAlarmValue);
	else
		field.Zero();
	
	TPtr16 note(NULL,0);
	if (propName!=KVersitTokenAALARM)
		FindFirstField(note,aAlarmValue);

	CVersitAlarm* alarm = CVersitAlarm::NewL(runTime, snoozeTime, repeatCount, field, note);

	CleanupStack::Pop(2,runTime);
	return alarm; //alarm takes ownership of runTime and snoozeTime
	}

/** Create a new extended alarm from a stream.
The MIME type is set to the creating object's MIME type.
The disposition is set to the creating object's disposition.  
@param aAlarmValue a pointer to a buffer containing the assocaited data for the alarm. Should not point to an empty 
descriptor.
@return The newly created extended alarm. */
EXPORT_C CVersitExtendedAlarm* CParserVCalEntity::MakePropertyValueExtendedAlarmL(TPtr16 aAlarmValue)
	{
	TPtrC8 propName=iCurrentProperty->Name();
	TPtr16 field(NULL,0);


	// Note: the presence of this property means that
	// the parameters VALUE and X-CONTENTTYPE should be present; if not
	// present, then INLINE rich data shall be assumed

	TPtrC8 mimeType(NULL,0);
	TPtrC8 dispositionField(NULL,0);

	// content-value-type assumes default is INLINE if parameter not present
	CVersitExtendedAlarm::TDisposition disposition = CVersitExtendedAlarm::EDispositionInline;

	// read in data content-type
	CParserParam* alarmParameter = iCurrentProperty->Param(KVersitTokenCONTENTTYPE);

	if (alarmParameter)
		{
		mimeType.Set(alarmParameter->Value());
		}

	// read in data content-value-type
	alarmParameter = iCurrentProperty->Param(KVersitTokenVALUE);
		
	if (alarmParameter)
		{
		dispositionField.Set(alarmParameter->Value());
		
		// if there is a value, convert from text to enum
		if (dispositionField.Length())
			{
			disposition = DecodeDisposition(dispositionField);
			}
		}
		
	FindRemainingField(field,aAlarmValue);
	CVersitExtendedAlarm* alarm = CVersitExtendedAlarm::NewL(field.Collapse(), mimeType, disposition);

	return alarm; 
	}
	
/** Create a new extended alarm from a buffer.
The MIME type is set to the creating object's MIME type.
The disposition is set to the creating object's disposition. 
@param aAlarmValue a reference to a buffer containing the assocaited data for the alarm. Should not be an empty 
buffer.
@return The newly created extended alarm. */	
EXPORT_C CVersitExtendedAlarm* CParserVCalEntity::MakePropertyValueExtendedAlarmL(CBufSeg& aAlarmValue)
	{
	TPtrC8 propName=iCurrentProperty->Name();
	TPtr16 field(NULL,0);


	// Note: the presence of this property means that
	// the parameters VALUE and CONTENTTYPE should be present; if not
	// present, then MIME rich data shall be assumed

	TPtrC8 mimeType(NULL,0);
	TPtrC8 dispositionField(NULL,0);

	// content-value-type assumes INLINE if parameter not present
	CVersitExtendedAlarm::TDisposition disposition = CVersitExtendedAlarm::EDispositionInline;

	// read in data content-type
	CParserParam* alarmParameter = iCurrentProperty->Param(KVersitTokenCONTENTTYPE);

	if (alarmParameter)
		{
		mimeType.Set(alarmParameter->Value());
		}

	// read in disposition
	alarmParameter = iCurrentProperty->Param(KVersitTokenVALUE);
		
	if (alarmParameter)
		{
		dispositionField.Set(alarmParameter->Value());
		
		// if there is a value, convert from text to enum
		if (dispositionField.Length())
			{
			disposition = DecodeDisposition(dispositionField);
			}
		}
		
	// read in rich data content
	// For this version of MakePropertyValueExtendedAlarmL, we have the 
	// data in a CBufSeg that should already have been decoded from BASE64
	// (assuming everything else works properly). So we need to get it into 
	// a TBufC8
	TInt segSize = aAlarmValue.Size();
	HBufC8* content = HBufC8::NewLC(segSize);
	TPtr8 ptrContent(content->Des());
	aAlarmValue.Read(0, ptrContent, segSize);
	
	CVersitExtendedAlarm* alarm = CVersitExtendedAlarm::NewL(ptrContent, mimeType, disposition);
	CleanupStack::PopAndDestroy(content);
	
	return alarm; 
	}

/** Used to find the type of the content disposition: inline, URL, or unknown.
Converts content-value-type token field to content-value-type enum.
@param aContentDisposition The disposition of the data for the alarm action.
@return decoded disposition type */
EXPORT_C CVersitExtendedAlarm::TDisposition CParserVCalEntity::DecodeDisposition(const TDesC8& aContentDisposition) const
	{
	if (!aContentDisposition.CompareF(KVersitTokenINLINE))
		{
		return CVersitExtendedAlarm::EDispositionInline;
		};
		
	if (!aContentDisposition.CompareF(KVersitTokenURL))
		{
		return CVersitExtendedAlarm::EDispositionUrl;
		};
	
	return CVersitExtendedAlarm::EDispositionUnknown;
	}


/*
 * Try to recognize aToken and return a UID for it. 
 * Call <code>CVersitParser::RecognizeToken()</code>
 * which recognizes generic Versit tokens if the token has not been 
 * recognized in this function 
 *
 * @param     " const TDesC8& aToken "
 *            The token to be recognized
 * @return    " TUid "
 *            a defined UID value if the token has been recognized, 
 *				<code>KVersitTokenUnknownUid</code>, otherwise
 */

EXPORT_C TUid CParserVCalEntity::RecognizeToken(const TDesC8& aToken) const
// From CVersitParser
	{
	TUid uid = KNullUid;
	TChar firstChar(aToken.Ptr()[0]);
	firstChar=firstChar.GetUpperCase();
	switch (firstChar)
		{
	case 'A':	
		if (!aToken.CompareF(KVersitTokenAALARM))
			uid.iUid = KVCalPropertyAlarmUid;
		else if (!aToken.CompareF(KVersitTokenATTENDEE))
			uid.iUid = KVersitPropertyHBufCUid;
		else if (!aToken.CompareF(KVersitTokenATTACH))
			{
			if(iCurrentProperty)
				{//If it is internalising, iCurrentProperty is set
				uid.iUid = KVersitPropertyBinaryUid;//In vCal standard, default value is binary
				CParserParam* valueParam = iCurrentProperty->Param(KVersitTokenVALUE);
				if (valueParam)
					{
					TPtrC8 pParameterValue(valueParam->Value());
					
					if (pParameterValue.CompareF(KVersitTokenINLINE) && pParameterValue.CompareF(KVersitTokenBINARY))
						{//If VALUE isn't specified as INLINE or BINARY, value is text type.
						uid.iUid = KVersitPropertyHBufCUid;			
						}
					}
				}
			}
		break;
	case 'C':
		if (!aToken.CompareF(KVersitTokenCATEGORIES))
			uid.iUid = KVersitPropertyCDesCArrayUid;
		else if (!aToken.CompareF(KVersitTokenCLASS))
			uid.iUid = KVersitPropertyHBufCUid;
		else if (!aToken.CompareF(KVersitTokenCOMPLETED))
			uid.iUid = KVersitPropertyDateTimeUid;
		break;
	case 'D':
		if (!aToken.CompareF(KVersitTokenDALARM))
			uid.iUid = KVCalPropertyAlarmUid;
		else if (!aToken.CompareF(KVersitTokenDCREATED) ||
				!aToken.CompareF(KVersitTokenDTEND) ||
				!aToken.CompareF(KVersitTokenDTSTART) ||
				!aToken.CompareF(KVersitTokenDUE))
			uid.iUid = KVersitPropertyDateTimeUid;
		else if(!aToken.CompareF(KVersitTokenDESCRIPTION))
			uid.iUid = KVersitPropertyHBufCUid;
		break;
	case 'E':
		if (!aToken.CompareF(KVersitTokenEXDATE))
			uid.iUid = KVersitPropertyMultiDateTimeUid;
		else if (!aToken.CompareF(KVersitTokenEXRULE))
			uid.iUid = KVCalPropertyRecurrenceUid;
		break;
	case 'G':
		if (!aToken.CompareF(KVersitTokenGEO))
			uid.iUid = KVersitPropertyHBufCUid;
		break;	
	case 'L':
		if (!aToken.CompareF(KVersitTokenLASTMODIFIED))
			uid.iUid = KVersitPropertyDateTimeUid;
		else if(!aToken.CompareF(KVersitTokenLOCATION))
			uid.iUid = KVersitPropertyHBufCUid;
		break;
	case 'M':
		if (!aToken.CompareF(KVersitTokenMALARM))
			uid.iUid = KVCalPropertyAlarmUid;
		break;
	case 'P':
		if (!aToken.CompareF(KVersitTokenPALARM))
			uid.iUid = KVCalPropertyAlarmUid;
		else if (!aToken.CompareF(KVersitTokenPRIORITY))
			uid.iUid = KVersitPropertyIntUid;
		break;
	case 'R':
		if(!aToken.CompareF(KVersitTokenRDATE))
			uid.iUid = KVersitPropertyMultiDateTimeUid;
		else if (!aToken.CompareF(KVersitTokenRELATEDTO))
			uid.iUid = KVersitPropertyHBufCUid;
		else if (!aToken.CompareF(KVersitTokenRESOURCES))
			uid.iUid = KVersitPropertyCDesCArrayUid;
		else if (!aToken.CompareF(KVersitTokenRNUM))
			uid.iUid = KVersitPropertyIntUid;
		else if (!aToken.CompareF(KVersitTokenRRULE))
			uid.iUid = KVCalPropertyRecurrenceUid;
		break;
	case 'S':
		if (!aToken.CompareF(KVersitTokenSEQUENCE))
			uid.iUid = KVersitPropertyIntUid;
		else if (!aToken.CompareF(KVersitTokenSTATUS) ||
				!aToken.CompareF(KVersitTokenSUMMARY))
			uid.iUid = KVersitPropertyHBufCUid;
		break;
	case 'T':
		if(!aToken.CompareF(KVersitTokenTRANSP))
			uid.iUid = KVersitPropertyIntUid;
		break;
	case 'U':
		if (!aToken.CompareF(KVersitTokenURL) ||
			!aToken.CompareF(KVersitTokenUID))
		uid.iUid = KVersitPropertyHBufCUid;
		break;
	case 'X':
		if (!aToken.CompareF(KVersitTokenXRECURRENCEID) ||
			!aToken.CompareF(KVersitTokenXDTSTAMP))
			{
			uid.iUid = KVersitPropertyDateTimeUid;
			}
		else if (!aToken.CompareF(KVersitTokenXALARM))
			{
			uid.iUid = KVCalPropertyExtendedAlarmUid;
			}
		else if (!aToken.CompareF(KVersitTokenXLOCALUID) ||
				!aToken.CompareF(KVCalTokenXFavourite))
			{
			uid.iUid = KVersitPropertyIntUid;
			}
		break;
		}
	if (uid == KNullUid)
		return CVersitParser::RecognizeToken(aToken);
	return uid;
	}

EXPORT_C void CParserVCalEntity::Reserved1()
	{}

EXPORT_C void CParserVCalEntity::Reserved2()
	{}


//
// CVersitAlarm
//

CVersitAlarm::CVersitAlarm(TInt aRepeatCount)
	: iRepeatCount(aRepeatCount)
	{
	}

void CVersitAlarm::ConstructL(const TDesC& aAudioContent, const TDesC& aNote, TVersitDateTime* aRunTime, TTime* aSnoozeTime)
	{
	if (aAudioContent.Size())
		{
		iAudioContent = aAudioContent.AllocL();
		}
	if (aNote.Size())
		{
		iNote = aNote.AllocL();
		}
	iRunTime = aRunTime;
	iSnoozeTime = aSnoozeTime;
	}

EXPORT_C CVersitAlarm* CVersitAlarm::NewL(TVersitDateTime* aRunTime, TTime* aSnoozeTime, TInt aRepeatCount, const TDesC& aAudioContent, const TDesC& aNote)
/** Allocates and constructs a new alarm.

Ownership of aRunTime and aSnoozeTime is taken in the end.

@param aRunTime Pointer to the alarm time. 
@param aSnoozeTime Pointer to the snooze time (may be NULL). 
@param aRepeatCount The repeat count. 
@param aAudioContent A binary buffer containing the sound data. May be an empty 
descriptor. 
@param aNote A descriptor containing text to display when the alarm is executing. 
May be an empty descriptor. 
@return Pointer to the newly created alarm. */
	{
	CVersitAlarm* self = new(ELeave) CVersitAlarm(aRepeatCount);
	CleanupStack::PushL(self);
	self->ConstructL(aAudioContent, aNote, aRunTime, aSnoozeTime);
	CleanupStack::Pop(self);
	return self;	
	}

EXPORT_C CVersitAlarm::~CVersitAlarm()
/** Frees all resources owned by the alarm, prior to its destruction. */
	{
	delete iRunTime;
	delete iSnoozeTime;
	delete iAudioContent;
	delete iNote;
	}

CVersitExtendedAlarm::CVersitExtendedAlarm()
	{
	}

void CVersitExtendedAlarm::ConstructL(const TDesC8& aContent, const TDesC8& aContentMimeType, CVersitExtendedAlarm::TDisposition aContentDisposition) 
	{
	if (aContent.Size())
		{
		iContent = aContent.AllocL();		
		}
	else
		{
		iContent = NULL;
		}
		
	if (aContentMimeType.Size())
		{
		iMimeType = aContentMimeType.AllocL();
		}
	else
		{
		iMimeType = NULL;
		}
	iDisposition = aContentDisposition;
	}

/** Allocates and constructs a new extended alarm (X-EPOCALARM).
@param aContent A binary buffer containing the assocaited data for the alarm. Should not be empty 
descriptor.
@param aContentMimeType The MIME type of the data describing the action for the alarm.  
@param aContentDisposition The disposition of the data for the alarm action.
@return Pointer to the newly created extended alarm. */
EXPORT_C CVersitExtendedAlarm* CVersitExtendedAlarm::NewL(const TDesC8& aContent, const TDesC8& aContentMimeType,
						   			CVersitExtendedAlarm::TDisposition aContentDisposition)
	{
	CVersitExtendedAlarm * self = new(ELeave) CVersitExtendedAlarm();
	CleanupStack::PushL(self);
	self->ConstructL(aContent, aContentMimeType, aContentDisposition);
	CleanupStack::Pop(self);
	return self;	
	}

EXPORT_C CVersitExtendedAlarm::~CVersitExtendedAlarm()
/** Frees all resources owned by the alarm, prior to its destruction. */
	{
	delete iContent;
	delete iMimeType;
	}

//
// CParserPropertyValueAlarm
//
EXPORT_C CParserPropertyValueAlarm::CParserPropertyValueAlarm(CVersitAlarm* aValue)
: CParserTimePropertyValue(TUid::Uid(KVCalPropertyAlarmUid))
	,iValue(aValue)
/** Constructs a new alarm property value with a pointer to a CVersitAlarm.

@param aValue Pointer to the alarm. The property value takes ownership of 
the pointer. */
	{}


EXPORT_C CParserPropertyValueAlarm::~CParserPropertyValueAlarm()
/** Frees all resources owned by the property value, prior to its destruction. */
	{
	delete iValue;
	}

EXPORT_C void CParserPropertyValueAlarm::ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& aIncrement, const CVersitDaylight* aDaylight)
/** Converts the alarm time into universal time.

The date/time of the alarm is checked against the daylight saving information 
provided in aDaylight. If it falls inside the daylight saving period then 
the daylight saving offset is subtracted from the time to convert it to universal 
time. Otherwise aIncrement is added to the date/time of the alarm to convert 
it to universal time.

Note that the daylight savings offset will adjust the time both for the daylight 
saving and for the time zone.

The function has no effect if the value is already stored as universal time.

If aDaylight is a NULL pointer then aIncrement is used.

@param aIncrement A time interval in seconds which represents the negative 
of the time zone of the originating machine. For instance, if the time zone 
is +04:30, aIncrement should be set to -04:30.
@param aDaylight Pointer to the specification for daylight saving. If the alarm's 
time value is within the period for daylight saving, the value is modified 
by the daylight saving offset (which accounts for both the time zone and daylight 
saving rule). 
@deprecated since 9.1
*/
	{
	if (iValue && iValue->iRunTime && (iValue->iRunTime->iRelativeTime != TVersitDateTime::EIsUTC) && !iValue->iRunTime->IsFlagSet(TVersitDateTime::EExportLeaveAsLocalTime))
		{
		ConvertDateTime(&iValue->iRunTime->iDateTime,aIncrement, aDaylight);
		iValue->iRunTime->iRelativeTime = TVersitDateTime::EIsUTC;		
		}
	}	

EXPORT_C void CParserPropertyValueAlarm::ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement)
/** Converts the alarm time to machine-local time. 

This involves adjusting the alarm's date/time by the offset in aIncrement.

The function has no effect if the value is already stored as machine-local 
time.

@param aIncrement A time interval which represents the number of seconds which 
is to be added to the date/time value. This should normally be the universal 
time offset for the machine's locale. 
@deprecated since 9.1
*/
	{
	if (iValue && iValue->iRunTime && (iValue->iRunTime->iRelativeTime == TVersitDateTime::EIsUTC))
		{
		ConvertDateTime(&iValue->iRunTime->iDateTime, aIncrement,NULL);
		iValue->iRunTime->iRelativeTime = TVersitDateTime::EIsMachineLocal;
		}
	}

EXPORT_C void CParserPropertyValueAlarm::ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt aLengthOutput)
	// From CParserProperty
/** Externalises the alarm property value into aStream.

@param aStream Stream to which the value should be externalised.
@param aEncodingCharset Contains the character set and encoding into which the 
property value should be converted.
@param aLengthOutput The amount of text that has been outputted on the line 
so far, which needs to be taken into account when calculating if and where 
any line break should occur. */
	{
	if (!iValue)
		return;

	TInt bufLen=64;		//This should cover the Date/Time, Duration, Repeat Count and semi colons
	if (iValue->iAudioContent)
		bufLen+=iValue->iAudioContent->Length();
	if (iValue->iNote)
		bufLen+=iValue->iNote->Length();
	HBufC* outputStringBuf=HBufC::NewLC(bufLen);
	TPtr outputString=outputStringBuf->Des();
	TBuf8<KVersitDefaultBufferSize> buf;
	if (iValue->iRunTime)
		{
		EncodeVersitDateTimeL(buf,*iValue->iRunTime);
		Append(outputString,buf);
		}
	outputString.Append(KVersitTokenSemiColonUnicode);
	if (iValue->iSnoozeTime)
		{
		EncodeTimePeriodL(buf,*iValue->iSnoozeTime);
		Append(outputString,buf);
		}
	outputString.Append(KVersitTokenSemiColonUnicode);
	__ASSERT_DEBUG(iValue->iRepeatCount < 100000, User::Invariant());
	if(iValue->iRepeatCount>0)
		{
		outputString.AppendNum(iValue->iRepeatCount);
		}
	outputString.Append(KVersitTokenSemiColonUnicode);

	if (iValue->iAudioContent)
		{
		outputString.Append(*iValue->iAudioContent);
		if (iValue->iNote)
			outputString.Append(KVersitTokenSemiColonUnicode);
		}
	if (iValue->iNote)
		outputString.Append(*iValue->iNote);
	FoldAndWriteValueToStreamL(aStream,outputString,aEncodingCharset,aLengthOutput);
	CleanupStack::PopAndDestroy(outputStringBuf);
	}

EXPORT_C TBool CParserPropertyValueAlarm::IsAsciiCharacterSetSufficient()
/** Tests whether the property value can be represented using the ASCII character 
set.

@return ETrue if the property value can be represented using the ASCII character 
set. If not, EFalse. */
	{
	if	(!iValue)
		return ETrue;
	if (iValue->iAudioContent && !VersitUtils::DescriptorContainsOnlySevenBitCharacters(*iValue->iAudioContent))
		return EFalse;
	if (iValue->iNote && !VersitUtils::DescriptorContainsOnlySevenBitCharacters(*iValue->iNote))
		return EFalse;
	return ETrue;
	}

//
// CParserPropertyValueExtendedAlarm. 
//
EXPORT_C CParserPropertyValueExtendedAlarm::CParserPropertyValueExtendedAlarm(CVersitExtendedAlarm* aValue)
: CParserTimePropertyValue(TUid::Uid(KVCalPropertyExtendedAlarmUid))
	,iValue(aValue)
/** Constructs a new extended alarm property value with a pointer to a CVersitExtendedAlarm.

@param aValue Pointer to the alarm. The property value takes ownership of 
the pointer. */
	{}

EXPORT_C CParserPropertyValueExtendedAlarm::~CParserPropertyValueExtendedAlarm()
/** Frees all resources owned by the property value, prior to its destruction. */
	{
	delete iValue;
	}

EXPORT_C void CParserPropertyValueExtendedAlarm::ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& /* aIncrement */, const CVersitDaylight* /* aDaylight */)
/** Converts the extended alarm time into universal time.

@param aIncrement A time interval in seconds which represents the negative 
of the time zone of the originating machine. For instance, if the time zone 
is +04:30, aIncrement should be set to -04:30.
@param aDaylight Pointer to the specification for daylight saving. If the alarm's 
time value is within the period for daylight saving, the value is modified 
by the daylight saving offset (which accounts for both the time zone and daylight 
saving rule). 
@deprecated since 9.1
*/
	{
	}	

EXPORT_C void CParserPropertyValueExtendedAlarm::ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& /*aIncrement */)
/** Converts the extended alarm time to machine-local time.

@param aIncrement A time interval which represents the number of seconds which 
is to be added to the date/time value. This should normally be the universal 
time offset for the machine's locale. 
@deprecated since 9.1
*/
	{
	}

EXPORT_C void CParserPropertyValueExtendedAlarm::ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt aLengthOutput)
// From CParserProperty
/** Externalises the extended alarm property value into aStream.

@param aStream Stream to which the value should be externalised.
@param aEncodingCharset Contains the character set and encoding into which the 
property value should be converted.
@param aLengthOutput The amount of text that has been outputted on the line 
so far, which needs to be taken into account when calculating if and where 
any line break should occur. */
	{
	if (!iValue)
		{
		return;
		}
	// There are two encoding possibilities for this value. If it is a URL, then we expand the 8 bit string,
	// which we know is US ASCII into Unicode and append it. If it is INLINE data, then we must encode it as 
	// 64 bit data
	switch (iValue->iDisposition)
		{
		case CVersitExtendedAlarm::EDispositionUrl:
			ExternalizeUrlL(aStream, aEncodingCharset, aLengthOutput);
			break;
		case CVersitExtendedAlarm::EDispositionInline:
		default:  //intentional fall through, we'll treat any unknown as INLINE data
			ExternalizeInlineL(aStream, aEncodingCharset, aLengthOutput);
			break;
		}
	
	}

void CParserPropertyValueExtendedAlarm::ExternalizeUrlL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt aLengthOutput)
	{
	TInt bufLen = 0;
	if (iValue->iContent)
		{
		bufLen+=iValue->iContent->Size();		
		}
	
	HBufC* outputStringBuf=HBufC::NewLC(bufLen);

	TPtr outputString=outputStringBuf->Des();
		
	HBufC* bufferUrl = NULL;

	CVersitUnicodeUtils* vUtils = new(ELeave)CVersitUnicodeUtils();
	vUtils->CreateConverterL();

	bufferUrl = vUtils->WidenL(*(iValue->iContent));

	TPtrC desUrl(bufferUrl->Des());
	CleanupStack::PushL(bufferUrl);

	outputString.Append(desUrl);

	FoldAndWriteValueToStreamL(aStream,outputString,aEncodingCharset,aLengthOutput);
	
	CleanupStack::PopAndDestroy(2,outputStringBuf);
	
	}

const TInt KBase64MaxLineLength = 64; // chars

void CParserPropertyValueExtendedAlarm::ExternalizeInlineL(RWriteStream& aStream,const Versit::TEncodingAndCharset& /*aEncodingCharset*/,TInt /*aLengthOutput*/)
	{
	if (iValue->iContent)
		{
		const TInt length = iValue->iContent->Size();
		if (length >0)
			{
			CBufSeg* target = CBufSeg::NewL(length);
			CleanupStack::PushL(target);

			//get the Data Content into a usable buffer
			CBufSeg* source = CBufSeg::NewL(length+1);
			source->Reset();
			source->InsertL(0, *(iValue->iContent));
			source->Compress();
			CleanupStack::PushL(source);

			
			//Do the BASE64 Encoding
			RBufReadStream readStream;
			readStream.Open(*source);
			VersitUtils::ConArcEncodeL(readStream, *target, 
						VersitUtils::ConArcEncodingUid(Versit::EBase64Encoding));
			readStream.Close();

			// make sure the  line lengths are appropriate
			VersitUtils::WrapLinesL(*target, KBase64MaxLineLength);

			// write to output stream
			TInt pos=0;
			TInt len=target->Size();
			while (pos < len)
				{
				TPtr8 ptr = target->Ptr(pos);
				aStream.WriteL(ptr);
				pos+=ptr.Length();
				}
			CleanupStack::PopAndDestroy(2, target);
			}
		}
	aStream.WriteL(KVersitTokenCRLF);
	}

	
EXPORT_C TBool CParserPropertyValueExtendedAlarm::IsAsciiCharacterSetSufficient()
/** Tests whether the property value can be represented using the ASCII character 
set.

@return ETrue if the property value can be represented using the ASCII character 
set. If not, EFalse. */
	{
	if (!iValue)
		{
		return ETrue;
		}
	
	if (!iValue->iContent)
		{
		return ETrue;
		}
	
	if (iValue->iDisposition == CVersitExtendedAlarm::EDispositionUrl)
		{
		return ETrue;
		}
		
	return EFalse;
	}

