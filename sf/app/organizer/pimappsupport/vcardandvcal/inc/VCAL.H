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
 
#if !defined(__VCAL_H__)
#define __VCAL_H__

#if !defined(__VERSIT_H__)
#include <versit.h>
#endif

#if !defined(__VRECUR_H__)
#include <vrecur.h>
#endif


/** This constant is for internal use only.
@internalTechnology
*/
_LIT(KVersitTokenVCalVersionNo, "1.0");


//
// CParserVCal
//
class CParserVCal : public CVersitParser
/** A vCalendar parser.

Adds support for parsing vToDos and vEvents, and associated alarms 
(see CParserPropertyValueAlarm) to the functionality of CVersitParser.

Adds a constructor and overrides CVersitParser::InternalizeL() for streams, 
ExternalizeL() for streams, RecognizeToken(), RecognizeEntityName() and MakeEntityL().

The vCalendar data is read from or written to a stream or file, using the 
InternalizeL() and ExternalizeL() functions. Most users of this class will 
only need to use these functions.

Note: if you are sequentially creating and destroying multiple 
parsers, a major performance improvement may be achieved by using thread local 
storage to store an instance of CVersitUnicodeUtils which persists and can be 
used by all of the parsers.

See CVersitTlsData for more information. 

@publishedAll
@released
*/
	{
public:
	IMPORT_C static CParserVCal* NewL();
public: // from CVersitParser
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream);
protected: // from CVersitParser
	IMPORT_C CVersitParser* MakeEntityL(TInt aEntityUid,HBufC* aEntityName);
public: // from CVersitParser
	IMPORT_C TUid RecognizeToken(const TDesC8& aToken) const;
	IMPORT_C TInt RecognizeEntityName() const;
private:
	CParserVCal();
private: // from CVersitParser
	IMPORT_C virtual void Reserved1();
	IMPORT_C virtual void Reserved2();
	};

//
// CVersitAlarm
//
class CVersitAlarm : public CBase
/** An alarm for a vEvent or a vTodo. 

Specifies the date/time for the alarm, the amount of time the alarm will be 
snoozed (optional), the number of times that the alarm will repeat, the binary 
data of the sound to be played when the alarm runs (optional), and some text 
to associate with the alarm (optional).

An object of this class is owned by the CParserPropertyValueAlarm class.

This class supports the following types of alarm:

- Audible Alarm

- Display Alarm (displays message on the screen) 

- Procedure Alarm (calls a procedure)

- Mail Alarm (sends an email). 

@publishedAll
@released
*/
	{
public:
	IMPORT_C static CVersitAlarm* NewL(TVersitDateTime* aRunTime, TTime* aSnoozeTime, TInt aRepeatCount, const TDesC& aAudioContent, const TDesC& aNote);
	IMPORT_C ~CVersitAlarm();
private:
	CVersitAlarm(TInt aRepeatCount);
	void ConstructL(const TDesC& aAudioContent, const TDesC& aNote, TVersitDateTime* aRunTime, TTime* aSnoozeTime);
public:	
	/** The date/time for the alarm. */
	TVersitDateTime* iRunTime;
	/** The amount of time the alarm will be snoozed (optional). */
	TTime* iSnoozeTime;
	/** The number of times that the alarm will repeat. */
	TInt iRepeatCount;
	/** The binary data describing the sound to play for an AALARM (optional), or 
	the string which specifies the address to send the e-mail to, for an MALARM. */
	HBufC* iAudioContent;
	/** The mesage to display on the screen for a DALARM (optional), or 
	the body of the e-mail to be sent for an MALARM, or the name of the procedure 
	to call for a PALARM. */
	HBufC* iNote;//Empty except for MAlarm
	};
	
class CVersitExtendedAlarm : public CBase
/** An extended alarm for a vEvent or a vTodo. 

Specifies the date/time for the alarm, the amount of time the alarm will be 
snoozed (optional), the number of times that the alarm will repeat, and the 
action for the alarm.
An object of this class is owned by the CParserPropertyValueAlarm class.

This class supports the following types of alarm:
- X-EPOCALARM 

@publishedAll
@released
*/
	{
public:

	/** Extended Alarm's disposition types.
	*/
	enum TDisposition
		{
		/** URL disposition
		*/
		EDispositionUrl = 0,
		/** Inline disposition
		*/
		EDispositionInline = 1,
		/** Unknown disposition
		*/
		EDispositionUnknown = 15
		};
	
	IMPORT_C static CVersitExtendedAlarm* NewL(const TDesC8& aContent,const TDesC8& aContentMimeType, CVersitExtendedAlarm::TDisposition aDisposition);
	IMPORT_C ~CVersitExtendedAlarm();
private:
	CVersitExtendedAlarm();
	void ConstructL(const TDesC8& aContent, const TDesC8& aContentMimeType, CVersitExtendedAlarm::TDisposition aContentDisposition);
public:	
	/** The data describing the action for the alarm.*/
	HBufC8* iContent;
	/** The MIME type of the data describing the action for the alarm.*/
	HBufC8* iMimeType;
	/** The disposition of the data for the alarm action (i.e whether it is inline or an URI).*/
	CVersitExtendedAlarm::TDisposition iDisposition;
	};

//
// CParserVCalEntity
//
class CParserVCalEntity : public CRecurrenceParser
/** A parser for vCalendar sub-entities.

A vCalendar sub-entity is a vEvent or vToDo contained in a vCalendar. vEvents 
and vToDos are derived from CRecurrenceParser, which provides recurrence functionality. 

vEvents and vTodos may have alarm properties (see CParserPropertyValueAlarm). 

@publishedAll
@released
*/
	{
public:
	IMPORT_C static CParserVCalEntity* NewL();
	IMPORT_C ~CParserVCalEntity();
public: // from CVersitParser
	IMPORT_C void ExternalizeL(RWriteStream& aStream);
protected: // from CVersitParser
	IMPORT_C CParserPropertyValue* MakePropertyValueL(const TUid& aPropertyUid,HBufC16*& aValue);
	IMPORT_C CVersitAlarm* MakePropertyValueAlarmL(TPtr16 aAlarmValue);
	IMPORT_C CVersitExtendedAlarm* MakePropertyValueExtendedAlarmL(TPtr16 aAlarmValue);
	IMPORT_C CVersitExtendedAlarm* MakePropertyValueExtendedAlarmL(CBufSeg& aAlarmValue);
	IMPORT_C CVersitExtendedAlarm::TDisposition DecodeDisposition(const TDesC8& aContentDispositionToken) const;
	IMPORT_C TUid RecognizeToken(const TDesC8& aToken) const;
private:
	CParserVCalEntity();
private: // from CVersitParser
	IMPORT_C virtual void Reserved1();
	IMPORT_C virtual void Reserved2();
	};


//
// CParserPropertyValueAlarm
//
class CParserPropertyValueAlarm : public CParserTimePropertyValue
/** An alarm property value parser for a vCalendar entity.

An alarm can be associated with a vEvent or a vTodo. The value for the alarm 
is stored as a CVersitAlarm.

Note: The UID for an alarm property value is KVCalPropertyAlarmUid. 

@publishedAll
@released
*/
	{
public:
	IMPORT_C CParserPropertyValueAlarm(CVersitAlarm* aValue);
	IMPORT_C CParserPropertyValueAlarm();
	IMPORT_C ~CParserPropertyValueAlarm();
	inline CVersitAlarm* Value() const;
public: // from CParserTimePropertyValue
	IMPORT_C void ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight);
	IMPORT_C void ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement);
public: // from CParserPropertyValue
	IMPORT_C void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt aLengthOutput);
	IMPORT_C TBool IsAsciiCharacterSetSufficient();
private:
	CVersitAlarm* iValue;
	};


//
// CParserPropertyValueExtendedAlarm
//
class CParserPropertyValueExtendedAlarm : public CParserTimePropertyValue
/** An extended alarm property value parser for a vCalendar entity.

An alarm action can be attached with a vEvent or a vTodo. The value for the alarm 
action is stored as a CVersitExtendedAlarm object. 

Note: The UID for an extended alarm property value is KVCalPropertyExtendedAlarmUid. 

@publishedAll
@released
*/
	{
public:
	IMPORT_C CParserPropertyValueExtendedAlarm(CVersitExtendedAlarm* aValue);
	IMPORT_C ~CParserPropertyValueExtendedAlarm();
	inline CVersitExtendedAlarm* Value() const;
public: // from CParserTimePropertyValue
	IMPORT_C void ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight);
	IMPORT_C void ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement);
public: // from CParserPropertyValue
	IMPORT_C void ExternalizeL(RWriteStream& aStream, const Versit::TEncodingAndCharset& aEncodingCharset, TInt aLengthOutput);
	IMPORT_C TBool IsAsciiCharacterSetSufficient();
private:
	void ExternalizeUrlL(RWriteStream& aStream, const Versit::TEncodingAndCharset& aEncodingCharset, TInt aLengthOutput);
	void ExternalizeInlineL(RWriteStream& aStream, const Versit::TEncodingAndCharset& aEncodingCharset, TInt aLengthOutput);
	
private:
	CVersitExtendedAlarm* iValue;
	};

#include <vcal.inl>

#endif
