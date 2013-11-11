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

#ifndef __VPROP_H__
#define __VPROP_H__

#ifndef __E32BASE_H__
#include <e32base.h>
#endif

#ifndef __BADESCA_H__
#include <badesca.h>
#endif

#ifndef __S32STRM_H__
#include <s32strm.h>
#endif

#ifndef __E32MATH_H__
#include <e32math.h>
#endif


#ifndef __VUID_H__
#include <vuid.h>
#endif

#ifndef __VTOKEN_H__
#include <vtoken.h>
#endif

#ifndef __VUTIL_H__
#include <vutil.h>
#endif

#ifndef __S32FILE_H__
#include <s32file.h>
#endif
// 

class CParserProperty;
class CVersitParser;
class MVersitPlugIn;
class CParserPropertyValueCDesCArray;

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
const TInt KRandomnumberlen = 5;
const TInt KMaxGeneratedfilenamelen =16;
#endif 
//
// CParserParam
//

class CParserParam : public CBase
/** A Versit property parameter. 

A property parameter consists of a name and optionally a value, both in descriptor 
form. Parser properties (class CParserProperty) may own one or more property 
parameters.

Note:

The private parameter name and value members are stored as narrow (8-bit) 
rather than 16-bit Unicode descriptors.

However, certain member functions take or return a Unicode value, 
for the purpose of backwards compatibility. In this case a simple conversion 
takes place: this ignores the top 8 bits (for Unicode -> Narrow) or adds zero 
for the top 8-bits (for Narrow->Unicode). 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CParserParam* NewL(const TDesC8& aName, const TDesC8& aValue);
	IMPORT_C static CParserParam* NewL(const TDesC8& aName, const TDesC& aValue);
	IMPORT_C ~CParserParam();
	IMPORT_C void SetValueL(const TDesC8& aValue);
	IMPORT_C void SetValueL(HBufC8* aValue);
	IMPORT_C TInt ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C TPtrC8 Name() const;
	IMPORT_C TPtrC8 Value() const;
	IMPORT_C HBufC* ValueL() const;
	IMPORT_C TInt ExternalizeL(RWriteStream& aStream, TInt& aLengthOutput, CVersitParser* aVersitParser) const;
private:
	CParserParam(HBufC8* aName, HBufC8* aValue);
private:
	HBufC8* iParamName;
	HBufC8* iValue;
	};

//
// TVersitDateTime
//

class TVersitDateTime
/** Defines a Versit date and time.

This class is used throughout Versit to represent the date and time. It uses 
a TDateTime object to store the date/time value and records whether this value 
is local to the machine which originated the vCard, local to the machine on 
which the code is running, or universal time (UTC).

The Year(), Month() and Day() member functions of class TDateTime may be used 
to extract the date and time components from the TVersitDateTime::iDateTime. 
@publishedAll
@released */
	{
public:
	/** Relative time flags. */
	enum TRelativeTime
		{
		/** Time is stored in UTC. */
		EIsUTC,
		/** Time is local to the machine which originated the vCard. */
		EIsVCardLocal,
		/** Time is local to the machine on which the code is running. */
		EIsMachineLocal,
		/** Unused. */
		EIsCorrect
		};
	IMPORT_C TVersitDateTime(const TDateTime& aDateTime,TRelativeTime aRelativeTime);

	/** Date/time flags. 
	@publishedAll
	@deprecated since 9.1*/
	enum TVersitDateTimeFlags 
		{
		EExportNullFlag=0,
		/** Indicates whether the relative time is to be externalized along with the date/time 
		value.
	
		The flag is set to 'EExportNullFlag' by default.
	
		This is only implemented (by CParserTimePropertyValue::EncodeVersitDateTimeL()) 
		if the date/time is universal. When the flag is set, the externalised value 
		is followed by the token, KVersitTokenUniversalTime. */
		EExportTimeZoneDesignator=0x01,
		EExportLeaveAsLocalTime=0x02
		};
	/**
	@deprecated since 9.1
	*/
	inline void SetFlag(TVersitDateTimeFlags aFlag) { iFlags |= aFlag; }
	/**
	@deprecated since 9.1
	*/
	inline TBool IsFlagSet(TVersitDateTimeFlags aFlag) const { return iFlags & aFlag; }
	/**
	@deprecated since 9.1
	*/
	inline void ClearFlag(TVersitDateTimeFlags aFlag) { iFlags &= ~aFlag; }
public:
	/** The date/time value. */
	TDateTime iDateTime;
	/** Specifies whether the time value is local to the machine which originated the 
	vCard, local to the machine on which the code is running, or universal. */
	TRelativeTime iRelativeTime;
private:
	TInt iFlags;
	};

//
// CParserPropertyValue
//
class CParserPropertyValue : public CBase
/** Abstract base class for all property values. 

Defines a pure virtual ExternalizeL() function which should write out the 
property value to a stream.

The type of derived class is returned by the Uid() function; the UID value is 
specified on construction.

The other functions relate to the character set, encoding format and plug-in 
used during externalising. 
@publishedAll
@released
*/
	{
public:
	/** Externalises the property value to a write stream. 
	
	Implementations of this function are invoked by the parser's ExternalizeL() 
	function.
	
	@param aStream Stream to which the property value is externalised.
	@param aEncodingCharset The character set and encoding information.
	@param aLengthOutput The amount of text that has been outputted so far on the 
	line (for the property name), which may need to be taken into account when 
	calculating if and where any line break should occur. */
	virtual void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt aLengthOutput)=0;
	IMPORT_C virtual TBool IsAsciiCharacterSetSufficient();
	IMPORT_C virtual void EncodeL(CBufBase* aTarget,const TDesC8& aSource,const TUid& aEncoding) const;
	IMPORT_C virtual TBool SupportsInterface(const TUid& /*aInterfaceUid*/) const;
	inline TUid Uid() const;
	inline void SetPlugIn(MVersitPlugIn* aPlugIn);
protected:
	IMPORT_C CParserPropertyValue(const TUid& aPropertyValueUid);
	IMPORT_C void FoldEncodeAndWriteValueToStreamL(RWriteStream& aStream, const TDesC& aValue
														,const Versit::TEncodingAndCharset& aEncodingCharset,TInt& aLengthOutput) const;
	IMPORT_C void FoldEncodeAndWriteValueToStreamL(RWriteStream& aStream, const CDesCArray* aValueArray
														,const Versit::TEncodingAndCharset& aEncodingCharset,TInt& aLengthOutput) const;
	IMPORT_C void FoldAndWriteValueToStreamL(RWriteStream& aStream, const TDesC& aValue
														,const Versit::TEncodingAndCharset& aEncodingCharset,TInt& aLengthOutput) const;
	IMPORT_C static void Append(TDes16& aTarget,TDesC8& aSource);
protected:
    inline MVersitPlugIn* PlugIn();

private:
	TUid iPropertyValueTypeUid;
	MVersitPlugIn* iPlugIn;
	};

//
// CParserTimePropertyValue
//

class CVersitDaylight;

class CParserTimePropertyValue : public CParserPropertyValue
/** Abstract base class for all of the date/time property value classes.

Provides date/time conversion functions between machine-local and universal 
time. 

The date/time property value classes are CParserPropertyValueAlarm, 
CParserPropertyValueDate, CParserPropertyValueDateTime, 
CParserPropertyValueMultiDateTime, CParserPropertyValueDaylight and 
CParserPropertyValueRecurrence. 
@publishedAll
@released
*/
	{
public:
	/** This is implemented, where applicable, to convert date/time 
	values into universal time.
	
	The date/time is checked against the daylight saving information provided 
	in aDaylight. If it falls inside the daylight saving period then the daylight 
	saving offset is subtracted from the time to convert it to universal time. 
	Otherwise aIncrement is added to the date/time of the alarm to convert it 
	to universal time.
	
	Note that the daylight savings offset will adjust the time both for the daylight 
	saving and for the time zone.
	
	The function has no effect if it date/times are already stored in universal 
	time.
	
	If aDaylight is a NULL pointer then aIncrement is used.
	
	@param aIncrement A time interval in seconds which represents the time zone's 
	offset from universal time. 
	@param aDaylight Pointer to the specification for daylight saving. If the alarm's 
	time value is within the period for daylight saving, the value is modified 
	by the daylight saving offset (which accounts for both the time zone and daylight 
	saving rule). 
	@deprecated since 9.1
	*/
	IMPORT_C virtual void ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight)=0;
	/** Pure virtual function which is implemented, where applicable, to convert the 
	date/time property value into machine-local time. 
	
	This process involves adjusting the date/time value by the offset in aIncrement.
	
	The function has no effect if the value is already stored as machine-local 
	time.
	
	The universal date/times are assumed to have been corrected for any daylight saving 
	rule in effect.
	
	@param aIncrement A time interval which represents the number of seconds which 
	is to be added to the date/time value. This should normally be the universal 
	time offset for the machine's locale. 
	@deprecated since 9.1 */
	IMPORT_C virtual void ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement)=0;
	static void ConvertDateTime(TDateTime& aDateTime,const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight,TBool aTo);
public: //from CParserPropertyValue
	IMPORT_C virtual TBool SupportsInterface(const TUid& aInterfaceUid) const;
protected:
	IMPORT_C CParserTimePropertyValue(const TUid& aPropertyValueUid);
	IMPORT_C void ConvertDateTime(TDateTime* aDateTime,const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight);
	IMPORT_C void EncodeVersitDateTimeL(TDes8& aBuf,const TVersitDateTime& aDateTime,TBool aEncodeTime=ETrue) const;
	IMPORT_C void EncodeTimePeriodL(TDes8& aBuf,const TTime& aTimePeriod) const;
	};

//
//  CParserPropertyValueHBufC
//
class CParserPropertyValueHBufC : public CParserPropertyValue
/** A heap descriptor property value parser.

Many properties are stored using this class. Examples are vCard e-mails and 
vCalendar locations.

The UID for a heap descriptor property value is KVersitPropertyHBufCUid. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CParserPropertyValueHBufC* NewL(const TDesC& aValue);
	IMPORT_C CParserPropertyValueHBufC(HBufC16* aValue);
	IMPORT_C ~CParserPropertyValueHBufC();
	IMPORT_C TPtrC Value() const;
	IMPORT_C HBufC* TakeValueOwnership();
    IMPORT_C CParserPropertyValueCDesCArray* TreatAsArrayPropertyLC(const CParserProperty& aOwningProperty) const;
public: // from CParserPropertyValue
	IMPORT_C TBool IsAsciiCharacterSetSufficient();
	IMPORT_C void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt aLengthOutput);
private:
	CParserPropertyValueHBufC();
	void ConstructL(const TDesC& aValue);
protected:
	HBufC* iValue;
	};

//
//  CParserPropertyValueBinary
//

class CParserPropertyValueBinary : public CParserPropertyValue
/** A binary property value parser.

It is used to store logos, photos, alarm content and binary attachment as binary values.

The UID for a binary property value is KVersitPropertyBinaryUid. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CParserPropertyValueBinary* NewL(const TDesC8& aValue);
	IMPORT_C static CParserPropertyValueBinary* NewLC(const TDesC8& aValue);

	inline CParserPropertyValueBinary(CBufSeg& aValue);
	IMPORT_C ~CParserPropertyValueBinary();
	IMPORT_C const CBufSeg* Value() const;
public: // from CParserPropertyValue
	IMPORT_C void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt /*aLengthOutput*/);
protected:
	inline CParserPropertyValueBinary();
	void ConstructL(const TDesC8& aValue);
	void ExternalizeL(RWriteStream& aStream, const Versit::TEncodingAndCharset& aEncodingCharset, TInt aLengthOutput, RReadStream& aReadStream);

protected:
	CBufSeg* iValue;
	};//

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS	
NONSHARABLE_CLASS(CParserPropertyValueBinaryFile) : public CParserPropertyValueBinary
/** A file property value parser.

It is used to store a file handle so that the binary data can be read through the handle when exporting.
It is not used when importing.
The UID for a binary property value is KVersitPropertyBinaryUid. 
@publishedPartner
@released
*/
	{
public:
	IMPORT_C static CParserPropertyValueBinaryFile* NewL(const RFile& aFileHandle);
	IMPORT_C ~CParserPropertyValueBinaryFile();
public: // from CParserPropertyValue
	void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt aLengthOutput);

private:
	void ConstructL(const RFile& aFileHandle);
private:
	RFileReadStream iFileStream;
	};
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS
//  CParserPropertyValueCDesCArray
//
class CParserPropertyValueCDesCArray : public CParserPropertyValue
/** A property value parser which stores an array of descriptors.

Can be used by both vCards and vCalendars, for example to store a postal address 
or information about an organisation.

The UID for a descriptor array property value is KVersitPropertyCDesCArrayUid. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CParserPropertyValueCDesCArray(CDesCArray* aValue);
	IMPORT_C ~CParserPropertyValueCDesCArray();
	inline CDesCArray* Value() const;
	IMPORT_C TBool IsPresent(const TDesC& aValue) const;
public: // from CParserPropertyValue
	IMPORT_C TBool IsAsciiCharacterSetSufficient();
	IMPORT_C void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt aLengthOutput);
protected:
	CDesCArray* iValue;
	};

//
// CParserPropertyValueTimeZone
//

class CParserPropertyValueTimeZone: public CParserPropertyValue
/** A time zone property value parser.

Stores and externalises a property value which represents the time zone information 
for a vCard or vCalendar. 

The time zone is specified as the universal time offset. This is a time interval 
which is a positive or negative number of seconds from universal time. Time 
zones east of universal time have positive numbers. Time zones west of universal 
time have negative numbers.

The UID for a time zone property value is KVersitPropertyTimeZoneUid. 
@publishedAll
@released
*/
	{
public:
	static void EncodeTimeZone(TDes8& aBuf,TTimeIntervalSeconds iValue);
	IMPORT_C CParserPropertyValueTimeZone(TTimeIntervalSeconds aValue);
	inline TTimeIntervalSeconds Value() const;
public: // from CParserPropertyValue
	IMPORT_C void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt /*aLengthOutput*/);
protected:
	TTimeIntervalSeconds iValue;
	};

//
// CVersitDaylight
//
class CVersitDaylight : public CBase
/** Universal time offset information, including the daylight savings offset. 

This class deals with the daylight saving offset: a one hour offset that occurs 
in many countries during part of the year. This offset needs to be taken into 
account when converting Versit times (TVersitDateTimes) between universal 
and machine-local times.

This class stores information about the offset, including its length in seconds, 
when it takes effect and the standard time designations.

Note that the universal offset (iOffset) is the sum of the daylight saving 
offset and any time zone offset, as it is the absolute offset from GMT.

An instance of this class is owned by the CParserPropertyValueDaylight class. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static CVersitDaylight* NewL(TBool aSavings, TTimeIntervalSeconds aOffset, TVersitDateTime* aStartTime, TVersitDateTime* aEndTime, const TDesC& aStandardDesignation, const TDesC& aDaylightDesignation);
	IMPORT_C ~CVersitDaylight();
private:
	CVersitDaylight(TBool aSavings, TTimeIntervalSeconds aOffset, TVersitDateTime* aStartTime, TVersitDateTime* aEndTime);
	void ConstructL(const TDesC& aStandardDesignation, const TDesC& aDaylightDesignation);
public:
	/** A sort key than can be used when sorting an array of properties by start date/time. */
	TInt64 iStartTimeSortKey;
	/** The daylight savings flag: this is ETrue if daylight saving is in effect 
	in the locale and EFalse if not. */
	TBool iSavings;
	/** The absolute offset from GMT, which is the sum of the time zone and daylight 
	saving offsets (in seconds). */
	TTimeIntervalSeconds iOffset;
	/** The date/time at which the period for daylight saving begins. */
	TVersitDateTime* iStartTime;
	/** The date/time at which the period for daylight saving ends. */
	TVersitDateTime* iEndTime;
	/** The standard time designation, e.g. GMT, EST. */
	HBufC* iStandardDesignation;
	/** The daylight saving time designation, e.g. BST, EDT. */
	HBufC* iDaylightDesignation;
	};

//
// CParserPropertyValueDaylight
//
class CParserPropertyValueDaylight : public CParserTimePropertyValue
/** A property value parser which contains the daylight savings rule for a vCalendar.

The property value is stored using an instance of the CVersitDaylight class.

The UID for a daylight savings rule property value is KVersitPropertyDaylightUid. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CParserPropertyValueDaylight(CVersitDaylight* aValue);
	IMPORT_C ~CParserPropertyValueDaylight();
	inline CVersitDaylight* Value() const;
public: // from CParserTimePropertyValue
	IMPORT_C void ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight);
	IMPORT_C void ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement);
public: // from CParserPropertyValue
	IMPORT_C void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt aLengthOutput);
private:
	CVersitDaylight* iValue;
	};

//
//  CParserPropertyValueDateTime
//
class CParserPropertyValueDateTime : public CParserTimePropertyValue
/** A date/time property value parser. 

The date/time value is contained in a TVersitDateTime object.

The UID for a date/time property value is KVersitPropertyDateTimeUid. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CParserPropertyValueDateTime(TVersitDateTime* aValue);
	IMPORT_C ~CParserPropertyValueDateTime();
	inline TVersitDateTime* Value() const;
public: // from CParserTimePropertyValue
	IMPORT_C void ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight);
	IMPORT_C void ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement);
public: // from CParserPropertyValue
	IMPORT_C void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& /*aEncodingCharset*/,TInt /*aLengthOutput*/);
protected:
	TVersitDateTime* iValue;
	};

//
//  CParserPropertyValueDate
//
class CParserPropertyValueDate : public CParserTimePropertyValue
/** A date property value parser.

The date value is contained in a TVersitDateTime object. 

The UID for a date property value is KVersitPropertyDateUid. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CParserPropertyValueDate(TVersitDateTime* aValue);
	IMPORT_C ~CParserPropertyValueDate();
	inline TVersitDateTime* Value() const;
public: // from CParserTimePropertyValue
	IMPORT_C void ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& /*aIncrement*/,const CVersitDaylight* /*aDaylight*/);
	IMPORT_C void ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& /*aIncrement*/);
public: // from CParserPropertyValue
	IMPORT_C void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt /*aLengthOutput*/);
private:
	TVersitDateTime* iValue;
	};

//
//  CParserPropertyValueMultiDateTime
//
class CParserPropertyValueMultiDateTime : public CParserTimePropertyValue
/** A property value parser which stores a list of date/time values using an array 
of TVersitDateTime objects.

The UID for a multiple date/time property value is KVersitPropertyMultiDateTimeUid. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CParserPropertyValueMultiDateTime(CArrayPtr<TVersitDateTime>* aValue);
	IMPORT_C ~CParserPropertyValueMultiDateTime();
	inline CArrayPtr<TVersitDateTime>* Value() const;
public: // from CParserTimePropertyValue
	IMPORT_C void ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight);
	IMPORT_C void ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement);
public: // from CParserPropertyValue
	IMPORT_C void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& /*aEncodingCharset*/,TInt aLengthOutput);
protected:
	CArrayPtr<TVersitDateTime>* iValue;
	};

//
//  CParserPropertyValueInt
//
class CParserPropertyValueInt : public CParserPropertyValue
/** An integer property value parser.

This stores a property value as a signed integer. For example, an employee 
ID number might be stored using this class.

The UID for an integer property value is KVersitPropertyIntUid. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CParserPropertyValueInt(TInt aValue);
	inline TInt Value() const;
public: // from CParserPropertyValue
	IMPORT_C void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt /*aLengthOutput*/);
protected:
	TInt iValue;
	};

//
//	CParserProperty
//

class CParserProperty : public CBase
/** A vCard or vCalendar property.

A property consists of a name, an optional value and one or more optional 
parameters.

The name, value and parameters are initialised on construction.

Versit properties have the general form:

Property Name (; Property Parameter Name(=Property Parameter Value)* : Property 
Value)

where items in brackets are optional and * indicates that the item may be 
repeated. 

For instance, TEL; HOME; ENCODING=QUOTED-PRINTABLE; CHARSET=US-ASCII : 01234 567890

Here, TEL is the property name; HOME, ENCODING and CHARSET are property parameter 
names; QUOTED-PRINTABLE and US-ASCII are property parameter values. The component 
following the colon is the property value.

Properties also have a name UID, which set during internalisation of a property 
and used only during internalisation. It is set by SetNameUid() and is used 
to allow number comparisons during internalisation. This provides a faster 
alternative to string comparisons when checking a property to see if it is 
a significant one (e.g. a begin or binary property). This is the only use 
of the name UID: it is not used during externalisation of a property.

Note that grouped properties are supported by the derived class, CParserGroupedProperty. 
The vCalender parser does not have property groups, and so stores all properties 
using this base class, whereas the vCard parser stores all properties using 
CParserGroupedProperty. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static					CParserProperty* NewL(CParserPropertyValue& aPropertyValue, const TDesC8& aName, CArrayPtr<CParserParam>* aArrayOfParams);
	IMPORT_C CParserProperty(CArrayPtr<CParserParam>* aArrayOfParams);
	IMPORT_C ~CParserProperty();
	IMPORT_C virtual void			ExternalizeL(RWriteStream& aStream, CVersitParser* aVersitParser = NULL);
	IMPORT_C CParserParam*			Param(const TDesC8& aParamName) const; // note params can only appear once
	IMPORT_C void					AddParamL(CParserParam* aParam);
	IMPORT_C void					DeleteParam(TDesC8& aParamName);
	IMPORT_C void					SetNameL(const TDesC8& aName);
	IMPORT_C virtual TBool			SupportsInterface(const TUid& /*aInterfaceUid*/) const;
	IMPORT_C TPtrC8					Name() const;
	inline void						SetValue(CParserPropertyValue* aPropertyValue);
	inline CParserPropertyValue*	Value() const;
	inline TUid						Uid() const;
	inline void						SetNameUid(TUid aUid);
	inline TUid						NameUid() const;
	inline void						SetParamArray(CArrayPtr<CParserParam>* aArrayOfParams);

	
	IMPORT_C TBool LoadBinaryValuesFromFilesL(RFs& aFileSession);
	IMPORT_C TBool SaveBinaryValuesToFilesL(TInt aSizeThreshold,const TDesC& aPath,RFs& aFileSession);
protected:
	IMPORT_C CParserProperty(CParserPropertyValue& aPropertyValue, CArrayPtr<CParserParam>* aArrayOfParams);
	IMPORT_C static void			ConstructSelfL(CParserProperty& aSelf,const TDesC8& aName);

public:
	IMPORT_C CArrayPtr<CParserParam>* ParamArray()const;

protected:
	CParserPropertyValue*			iPropertyValue;
	HBufC8*							iPropertyName;
	TUid							iPropertyNameUid;
	CArrayPtr<CParserParam>*		iArrayOfParams;

private:
	friend class CVersitParser;
	inline HBufC8*&					NameBuf();
	IMPORT_C virtual void			Reserved();
	void ReadBinaryDataL(const CBufSeg* aBufseg_ptr,HBufC8** aBuffer);
	void GenerateNameAndCreateFileL(RFs& aFileSession,TPtr8 aPropertyName,RFile& aFile,TDes& aFileName);
	
	};

#include <vprop.inl>

#endif
