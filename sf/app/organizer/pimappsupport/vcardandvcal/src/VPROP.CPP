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

#include <versit.h>
#include <vprop.h>
#include <vutil.h>
#include <s32mem.h>
#include <concnf.h>
#include <confndr.h>
#include <conlist.h>
#include <charconv.h>
#include <utf.h>

#include <vcal.h>
#include <vcard.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <vpropbinaryfile.h>
#include "versit_internal.h"
#endif 

// User includes
#include <vstaticutils.h>
#include <vobserv.h>
#include "vpbapplugin.h"


// Constants
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
const TInt KRandomnumberlen = 5;
const TInt KMaxGeneratedfilenamelen =16;
#endif
const TInt KBase64MaxLineLength = 64; // chars
const TInt KFileProtocolStringLength = 7; //file://
const TInt KMinFileNameLen = 4; // eg: c:\<1 digit number> , assuming that the property name was zero len. 
// But always the property name will be at least 1 // char in length . 
// This constant used in LoadBinaryValuesFromFilesL() to check whether the URI is fine


_LIT8(KValue,"VALUE");
_LIT8(KUri,"URI");
_LIT(KFileProtocol,"file://");

#define UNUSED_VAR(a) a = a

//
// CParserParam
//


EXPORT_C CParserParam* CParserParam::NewL(const TDesC8& aName,const TDesC8& aValue)
/** Allocates and constructs a new property parameter with the name and value specified.

This object does does not take ownership of aName or aValue.

@param aName The parameter name.
@param aValue The parameter value. Use KNullDesC8 if not applicable.
@return Pointer to the newly created property parameter. */
	{
	HBufC8* paramName=aName.AllocLC();
	HBufC8* value=NULL;
	if (aValue.Length()>0)
		value=aValue.AllocLC();
	CParserParam* param=new(ELeave) CParserParam(paramName,value);
	if (value)
		CleanupStack::Pop(value);
	CleanupStack::Pop(paramName);
	return param;
	}

EXPORT_C CParserParam* CParserParam::NewL(const TDesC8& aName,const TDesC& aValue)
/** Allocates and constructs a new property parameter with the name and value specified.

This object does does not take ownership of aName or aValue.

A Unicode value string is converted to a narrow string for storage. 

@param aName The parameter name.
@param aValue The parameter value. (Use KNullDesC if applicable).
@return Pointer to the newly created property parameter. */
	{
	HBufC8* paramName=aName.AllocLC();
	HBufC8* value=NULL;
	TInt len=aValue.Length();
	if (len>0)
		{
		value=HBufC8::NewLC(len);
		value->Des().Copy(aValue);
		}
	CParserParam* param=new(ELeave) CParserParam(paramName,value);
	if (value)
		CleanupStack::Pop(value);
	CleanupStack::Pop(paramName);
	return param;
	}


EXPORT_C CParserParam::~CParserParam()
/** Frees all resources owned by the property parameter, prior to its destruction. */
	{
	delete iParamName;
	delete iValue;
	}

EXPORT_C void CParserParam::SetValueL(const TDesC8& aValue)
/** Sets the property parameter value.

@param aValue The new property parameter value. */
	{
	delete iValue;
	iValue=0;
	iValue=aValue.AllocL();
	}

EXPORT_C void CParserParam::SetValueL(HBufC8* aValue)
/** Sets the property parameter value.

The property parameter takes ownership of aValue.

@param aValue The new property parameter value. */
	{
	delete iValue;
	iValue=aValue;
	}

EXPORT_C TInt CParserParam::ExternalizeL(RWriteStream& aStream) const
/** Externalises a property parameter to the stream, in the form NAME=VALUE (or just 
NAME, depending on whether there is a value).

This function performs the esacaping of characters.

@param aStream Stream to which the property parameter is to be externalised.
@return The length of data written to the stream. */
	{
	//
	// Write a property parameter in the format of
	//    NAME=VALUE
	//
	aStream.WriteL(*iParamName);
	TInt outputLen=iParamName->Length();

	// If there isn't a VALUE part for this property parameter then just return here...
	if	(!iValue)
		return outputLen;

	// Write the equals...
	aStream.WriteL(KVersitTokenEquals);

	// Escape semi-colon characters
	TInt length = iValue->Length();
	TInt ii;
	TUint8 ch;
	outputLen+=length+1;		//1 for the "="
	for (ii=0;ii<length;++ii)
		{
		ch=(*iValue)[ii];
		if (ch==KVersitTokenSemiColonVal || ch==KVersitTokenBackslashVal)
			{
			aStream.WriteUint8L(KVersitTokenBackslashVal);
			++outputLen;
			}
		aStream.WriteUint8L(ch);
		}
	return outputLen;
	}


EXPORT_C TInt CParserParam::ExternalizeL(RWriteStream& aStream, TInt& aLengthOutput, CVersitParser* aVersitParser) const
/** Externalises vCard3.0 property parameter to the stream, in the form NAME=VALUE or just 
VALUE, if it is a nameless parameter.

This function performs the esacaping of characters. */
	{	
	TBool namelessParam = EFalse;
	
	/** Property parameter contains value only -> TEL;Home:984536577 in which case iParamName 
	contains the parameter value. */
	HBufC8* buf = NULL;
	TInt nameLen = 0;
	if(iValue == NULL)
		{
		buf = iParamName;
		namelessParam = ETrue;
		}
	else
		{
		buf = iValue;
		nameLen = iParamName->Length()+1; //1 for the "="
		}
	
	//allocate a buffer large enough to cope with all characters in the string being
	//escaped. If this allocation fails no escaping is attempted which will result in
	//the creation of an invalid vCard
	HBufC8* valueBuf = HBufC8::NewLC(2*buf->Length());	
	TPtr8 valuePtr(valueBuf->Des());
	
	/** Escape semi-colon,backslash,comma characters in parameter value. */
	const TUint8* pSource = buf->Ptr();
	TInt length = buf->Length();
	for(TInt i = 0; i<length; i++, pSource++) 
		{
		switch (*pSource) 
			{
			case KVersitTokenCommaVal:
				{
				//replace "," with "\,"
				valuePtr.Append(KVersitTokenBackslashVal);
				valuePtr.Append(*pSource);
				}
				break;
			case KVersitTokenSemiColonVal:
				{
				//replace ";" with "\;"
				valuePtr.Append(KVersitTokenBackslashVal);
				valuePtr.Append(*pSource);
				}
				break;
			case KVersitTokenBackslashVal:
				{
				//replace "\" with "\\"	
				valuePtr.Append(KVersitTokenBackslashVal);
				valuePtr.Append(*pSource);
				}
				break;
			default:
				valuePtr.Append(*pSource);
				break;	
			}
		}
		
	TInt nameValueLen = nameLen + valueBuf->Length();
	HBufC8* nameValue = HBufC8::NewLC(nameValueLen);
	TPtr8 pText(nameValue->Des());	
	
	if(!namelessParam)
		{
		//create text in the form NAME=VALUE.		
		pText.Append(*iParamName);
		pText.Append(KVersitTokenEqualsVal);
		pText.Append(*valueBuf);
		}
	else
		{
		//nameless parameter.
		pText.Append(*valueBuf);
		}
		
	//fold property parameter.
	if(aVersitParser->PlugIn())
		{
		aVersitParser->PlugIn()->WrapLine(aStream, aLengthOutput, pText);
		}
			
	CleanupStack::PopAndDestroy(nameValue);
	CleanupStack::PopAndDestroy(valueBuf);
		
	return aLengthOutput;
	}


EXPORT_C TPtrC8 CParserParam::Name() const
/** Gets the property parameter name. 

If no name has been set, the function returns an empty string.

@return The property parameter name. */
	{
	if (iParamName)
		return iParamName->Des();
	return KVersitTokenEmptyNarrow();
	}

EXPORT_C TPtrC8 CParserParam::Value() const
/** Gets the property parameter value. 

If no value has been set, the function returns an empty descriptor.

@return The property parameter value. */
	{
	if (iValue)
		return iValue->Des();
	return KVersitTokenEmptyNarrow();
	}

EXPORT_C HBufC* CParserParam::ValueL() const
/** Gets the property parameter value as a Unicode heap descriptor.

If no value has been set, the function returns an empty descriptor.

@return A Unicode version of the property parameter value. */
	{
	TInt len=0;
	if (iValue)
		len=iValue->Length();
	HBufC* value=HBufC::NewL(len);
	if (iValue)
		value->Des().Copy(*iValue);
	return value;
	}

CParserParam::CParserParam(HBufC8* aName,HBufC8* aValue)
	: iParamName(aName)
	, iValue(aValue)
	{}

//
// TVersitDateTime
//

EXPORT_C TVersitDateTime::TVersitDateTime(const TDateTime& aDateTime,TRelativeTime aRelativeTime)
	: iDateTime(aDateTime)
	,iRelativeTime(aRelativeTime)
/** Constructs the Versit date/time object with a date/time value and a specification 
of whether this value is local to the machine which originated the vCard, local 
to the machine on which the code is running, or universal time.

@param aDateTime The date/time value. 
@param aRelativeTime The time the date/time value represents. */
	{
	iFlags = EExportNullFlag;	
	}
//
// CParserPropertyValue
//
EXPORT_C TBool CParserPropertyValue::SupportsInterface(const TUid& /*aInterfaceUid*/) const
/** Tests whether the property value supports the specified interface.

This implementation returns EFalse.

It is implemented by the derived class CParserTimePropertyValue to return ETrue if 
aInterfaceUid is KVersitTimePropertyUid. 

@param aInterfaceUid Not used.
@return EFalse. */
	{
	return EFalse;
	}

GLDEF_C void DestroyHBufC(TAny* aHBufC)
	{
	delete *STATIC_CAST(HBufC**, aHBufC);
	}

EXPORT_C void CParserPropertyValue::FoldEncodeAndWriteValueToStreamL(RWriteStream& aStream, const CDesCArray* aValueArray
														 ,const Versit::TEncodingAndCharset& aEncodingCharset,TInt& aLengthOutput) const
	{
	if (!aValueArray)
		return;
	TInt count=aValueArray->Count();
	if (count==0)
		return;

	TInt bufferLen=64;
	HBufC* unicodeValue = HBufC::NewL(bufferLen);
	TPtr pUnicode(unicodeValue->Des());
	CleanupStack::PushL(TCleanupItem(DestroyHBufC, &unicodeValue));//unicodeValue's address
	TPtrC pValue;
	TInt extraLengthNeeded;
	TInt ii=0;
	FOREVER
		{
		pValue.Set(aValueArray->MdcaPoint(ii));
		extraLengthNeeded=2*pValue.Length()+1+pUnicode.Length()-bufferLen;
		if (extraLengthNeeded>0)
			{
			bufferLen+=32*(1+extraLengthNeeded/32);
			unicodeValue=unicodeValue->ReAllocL(bufferLen);
			pUnicode.Set(unicodeValue->Des());
			}
		VersitUtils::AddEscapedString(pUnicode,pValue,aEncodingCharset.iCharSetId);
		if (++ii==count)
			break;
		pUnicode.Append(KVersitTokenSemiColonUnicode);
		}
	if (iPlugIn)
		iPlugIn->AddEscaping(unicodeValue);
	FoldAndWriteValueToStreamL(aStream,*unicodeValue,aEncodingCharset,aLengthOutput);
	CleanupStack::PopAndDestroy(&unicodeValue);
	}

EXPORT_C void CParserPropertyValue::FoldEncodeAndWriteValueToStreamL(RWriteStream& aStream, const TDesC& aValue
														,const Versit::TEncodingAndCharset& aEncodingCharset,TInt& aLengthOutput) const
	{
	TInt length = aValue.Length();
	if	(!length)
		return;

	HBufC* unicodeValue = HBufC::NewL(2*length);
	TPtr pUnicode(unicodeValue->Des());
	CleanupStack::PushL(TCleanupItem(DestroyHBufC, &unicodeValue));
	VersitUtils::AddEscapedString(pUnicode,aValue,aEncodingCharset.iCharSetId);
	if (iPlugIn)
		iPlugIn->AddEscaping(unicodeValue);
	FoldAndWriteValueToStreamL(aStream,*unicodeValue,aEncodingCharset,aLengthOutput);
	CleanupStack::PopAndDestroy(&unicodeValue);
	}

EXPORT_C void CParserPropertyValue::FoldAndWriteValueToStreamL(RWriteStream& aStream, const TDesC& aValue
														,const Versit::TEncodingAndCharset& aEncodingCharset,TInt& aLengthOutput) const
	{
	__ASSERT_DEBUG(aEncodingCharset.iEncoding!=Versit::EEightBitEncoding, Panic(EVersitPanicUnexpectedEightBitEncoding));
	HBufC8* narrowBuffer = HBufC8::NewLC(3*aValue.Length());
	TPtr8 pText(narrowBuffer->Des());
	VersitUtils::UncodeToNarrowL(aValue,pText,aEncodingCharset);
	CBufFlat* buffer=NULL;
	if (aEncodingCharset.iEncoding!=Versit::ENoEncoding)
		{
		buffer=CBufFlat::NewL(3*pText.Length()/2);
		CleanupStack::PushL(buffer);
		TUid encodingUid = VersitUtils::ConArcEncodingUid(aEncodingCharset.iEncoding);
		EncodeL(buffer,*narrowBuffer,encodingUid);
		pText.Set(buffer->Ptr(0));
		if (aEncodingCharset.iEncoding==Versit::EBase64Encoding)
			{
			if (pText.Length()+aLengthOutput>KMaxExternalizedTokenLength)
				VersitUtils::WrapLinesL(*buffer, KBase64MaxLineLength);
			buffer->InsertL(buffer->Size(), KVersitTokenCRLF);
			pText.Set(buffer->Ptr(0));
			}
		else
			{
			TInt firstLineLine=pText.Find(KVersitTokenCRLF);
			if (firstLineLine+aLengthOutput>KMaxExternalizedTokenLength)
				{
				aStream.WriteL(KVersitTokenEquals);
				aStream.WriteL(KVersitTokenCRLF);
				}
			}
		aStream.WriteL(pText);
		aLengthOutput=0;		//aLengthOutput should not be used again when encoding is Quoted-Printable
		CleanupStack::PopAndDestroy(buffer);
		}
	else if (!iPlugIn || !iPlugIn->WrapLine(aStream,aLengthOutput,pText))
		{
		TPtr8 searchString(NULL,0,0);
		TInt lineLength=Max(KMaxExternalizedTokenLength-aLengthOutput,0);
		TInt lengthLeft=pText.Length();
		TInt searchPos;
		while (lengthLeft>lineLength)
			{
			searchPos=lineLength;
			while (searchPos>=0 && pText[searchPos]==CVersitParser::ESpace)		//Search for last non-space before wrapping length
				--searchPos;
			if (searchPos>0)
				{
				searchString.Set(&pText[0],searchPos,searchPos);
				searchPos=searchString.LocateReverse(CVersitParser::ESpace);	//If there is one search for a space before that
				}
			else
				searchPos=KErrNotFound;
			if (searchPos==KErrNotFound)		//If can't break before wrapping length find first suitable place after it
				{
				searchPos=lengthLeft-lineLength;
				searchString.Set(&pText[lineLength],searchPos,searchPos);
				searchPos=lineLength+searchString.Locate(CVersitParser::ESpace);
				if (searchPos<lineLength)
					break;		//No more spaces
				while (searchPos<lengthLeft && pText[searchPos]==CVersitParser::ESpace)
					++searchPos;
				--searchPos;
				}
			searchString.Set(&pText[0],searchPos,searchPos);
			aStream.WriteL(searchString);
			aStream.WriteL(KVersitLineBreak);
			aLengthOutput=2;
			lengthLeft-=++searchPos;
			if (lengthLeft==0)
				{
				pText.Set(NULL,0,0);
				break;
				}
			pText.Set(&pText[searchPos],lengthLeft,lengthLeft);
			lineLength=KMaxExternalizedTokenLength;
			}
		aLengthOutput+=pText.Length();
		aStream.WriteL(pText);
		}
	CleanupStack::PopAndDestroy(narrowBuffer);
	}

EXPORT_C CParserPropertyValue::CParserPropertyValue(const TUid& aPropertyValueUid)
	: iPropertyValueTypeUid(aPropertyValueUid)
	{}

EXPORT_C TBool CParserPropertyValue::IsAsciiCharacterSetSufficient()
/** Tests whether the Ascii character set is sufficient to represent a property 
value.

This implementation returns ETrue.

It is overridden by classes CParserPropertyValueAlarm, CParserPropertyValueHBufC 
and CParserPropertyValueCDesCArray.

This function is called by CParserProperty::ExternalizeL().

@return ETrue. */
	{
	return ETrue;
	}

EXPORT_C void CParserPropertyValue::EncodeL(CBufBase* aTarget,const TDesC8& aSource,const TUid& aEncoding) const
/** Encodes the text property value referred to in aSource, if encoding is required.

Uses the encoding format identified by aEncoding. This is only used for 
text property values (e.g. HBufC or DesCArray property types). 

Invoked by implementations of ExternalizeL().

@param aTarget A pointer to the buffer which will have the converted text 
written into it.
@param aSource The source text.
@param aEncoding An encoding UID. The possible encoding formats are defined 
in vuid.h. Specify NULL UID if no encoding is required. */
	{
	__ASSERT_DEBUG(aTarget, User::Invariant());
	RDesReadStream readStream(aSource);
	CleanupClosePushL(readStream);
	VersitUtils::ConArcEncodeL(readStream, *aTarget, aEncoding);
	CleanupStack::PopAndDestroy();	
	}

EXPORT_C void CParserPropertyValue::Append(TDes16& aTarget,TDesC8& aSource)
	{
	TInt lenT=aTarget.Length();
	TInt lenS=aSource.Length();
	aTarget.SetLength(lenT+lenS);
	TPtr16 target(&aTarget[lenT],lenS,lenS);
	target.Copy(aSource);
	}


//
//  CParserTimeProperty
//

EXPORT_C CParserTimePropertyValue::CParserTimePropertyValue(const TUid& aPropertyValueUid)
	: CParserPropertyValue(aPropertyValueUid)
	{}

EXPORT_C TBool CParserTimePropertyValue::SupportsInterface(const TUid& aInterfaceUid) const
/** Tests whether the property value supports the specified interface.

It overrides the base class function, which always returns EFalse.

This function is used to test whether or not a property value is 
time-related (i.e. derived from CParserTimePropertyValue). It returns 
ETrue if aInterfaceUid is KVersitTimePropertyUid.

@param aInterfaceUid An interface UID. 
@return ETrue if KVersitTimePropertyUid is specified as the interface UID, 
otherwise EFalse. */
	{
	if (aInterfaceUid==TUid::Uid(KVersitTimePropertyUid))
		return ETrue;
	return CParserPropertyValue::SupportsInterface(aInterfaceUid);
	}

EXPORT_C void CParserTimePropertyValue::ConvertDateTime(TDateTime* aDateTime,const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight)
	{
	ConvertDateTime(*aDateTime,aIncrement,aDaylight,ETrue);
	}

void CParserTimePropertyValue::ConvertDateTime(TDateTime& aDateTime,const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight,TBool aToUTC)
	{
	TTime runTime(aDateTime);
	TBool summerTime=EFalse;
	if ((aDaylight)&&(aDaylight->iSavings)&&(aDaylight->iStartTime)&&(aDaylight->iEndTime))
		{
		TTime savingsStart(aDaylight->iStartTime->iDateTime);
		TTime savingsEnd(aDaylight->iEndTime->iDateTime);
		if ((runTime>savingsStart)&&(runTime<savingsEnd))
			{
			if (aToUTC)
				runTime-=aDaylight->iOffset;
			else
				runTime+=aDaylight->iOffset;
			summerTime=ETrue;
			}
		}
	if (!summerTime)
		runTime+=aIncrement;
	aDateTime=runTime.DateTime();
	}

EXPORT_C void CParserTimePropertyValue::EncodeVersitDateTimeL(TDes8& aBuf,const TVersitDateTime& aDateTime,TBool aEncodeTime) const
// Convert aDateTime to descriptor format
	{
	_LIT8(formatString1, "%04d%02d%02d");
	aBuf.Format(formatString1, aDateTime.iDateTime.Year(), aDateTime.iDateTime.Month() + 1, aDateTime.iDateTime.Day() + 1);
	if (aEncodeTime)
		{
		_LIT8(formatString2, "%S%02d%02d%02d");
		aBuf.AppendFormat(formatString2, &KVersitTimePeriodTime, aDateTime.iDateTime.Hour(), aDateTime.iDateTime.Minute(), aDateTime.iDateTime.Second());

		if (aDateTime.iRelativeTime == TVersitDateTime::EIsUTC) 
			{
			aBuf.Append(KVersitTokenUniversalTime);
			}
		}
	}

EXPORT_C void CParserTimePropertyValue::EncodeTimePeriodL(TDes8& aBuf,const TTime& aTimePeriod) const
	{
	_LIT8(formatString, "%d%S");
	TDateTime dateTime = aTimePeriod.DateTime();

	aBuf=KVersitTimePeriodBegin;
	TInt time = dateTime.Year();
	if (time)
		aBuf.AppendFormat(formatString, time, &KVersitTimePeriodYear);
	time = dateTime.Month();
	if (time)
		aBuf.AppendFormat(formatString, time, &KVersitTimePeriodMonth);
	time = dateTime.Day();
	if (time)
		aBuf.AppendFormat(formatString, time, &KVersitTimePeriodDay);
	aBuf.Append(KVersitTimePeriodTime);
	time = dateTime.Hour();
	if (time)
		aBuf.AppendFormat(formatString, time, &KVersitTimePeriodHour);
	time = dateTime.Minute();
	if (time)
		aBuf.AppendFormat(formatString, time, &KVersitTimePeriodMinute);
	time = dateTime.Second();
	if (time)
		aBuf.AppendFormat(formatString, time, &KVersitTimePeriodSecond);
	}

//
//  CParserPropertyValueHBufC
//

EXPORT_C CParserPropertyValueHBufC::CParserPropertyValueHBufC(HBufC16* aValue)
	:CParserPropertyValue(TUid::Uid(KVersitPropertyHBufCUid)),iValue(aValue)
	{}

CParserPropertyValueHBufC::CParserPropertyValueHBufC()
	: CParserPropertyValue(TUid::Uid(KVersitPropertyHBufCUid))
	{
	}

void CParserPropertyValueHBufC::ConstructL(const TDesC& aValue)
	{
	iValue = aValue.AllocL();
	}

EXPORT_C CParserPropertyValueHBufC* CParserPropertyValueHBufC::NewL(const TDesC& aValue)
/** Allocates and constructs a new heap descriptor property value with a descriptor.

Sets the property value's UID to KVersitPropertyHBufCUid.

@param aValue The property value. 
@return Pointer to the newly created heap descriptor property value. */
	{
	CParserPropertyValueHBufC* self = new(ELeave) CParserPropertyValueHBufC();
	CleanupStack::PushL(self);
	self->ConstructL(aValue);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CParserPropertyValueHBufC::~CParserPropertyValueHBufC()
/** Frees all resources owned by the property value, prior to its destruction. */
	{
	delete iValue;
	}

EXPORT_C TBool CParserPropertyValueHBufC::IsAsciiCharacterSetSufficient()
// From CParserPropertyValue
/** Tests whether the property value can be represented using the ASCII character 
set.

@return ETrue if the property value can be represented using the ASCII character 
set. If not, EFalse. */
	{
	return VersitUtils::DescriptorContainsOnlySevenBitCharacters(*iValue);
	}

EXPORT_C void CParserPropertyValueHBufC::ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt aLengthOutput)
/** Externalizes the descriptor property value into aStream.

This function is invoked by the parser's ExternalizeL() function.

@param aStream Stream into which the value is to be externalised.
@param aEncodingCharset Specifies the character set and encoding information.
@param aLengthOutput The amount of text that has been output so far on the 
line, which needs to be taken into account when calculating if and where any 
line break should occur. */
	{
	FoldEncodeAndWriteValueToStreamL(aStream,Value(),aEncodingCharset,aLengthOutput);
	}

EXPORT_C TPtrC CParserPropertyValueHBufC::Value() const
/** Retrieves the property value. 

@return Pointer descriptor representing the property value. */
	{
	if (iValue)
		return iValue->Des();
	return KVersitTokenEmpty();
	}

EXPORT_C HBufC* CParserPropertyValueHBufC::TakeValueOwnership()
/** Take ownership of the heap descriptor property value.

The property value previously owned by the object is deleted.

@return A pointer to the property value. */
	{
	HBufC* value=iValue;
	iValue=NULL;
	return value;
	}

EXPORT_C CParserPropertyValueCDesCArray* CParserPropertyValueHBufC::TreatAsArrayPropertyLC(const CParserProperty& aOwningProperty) const
/** Treats this HBufC-based property value as a possible array-based property. 

This function was added for compatibility reasons to support array-based SOUND 
property values. This does not alter the representation of this parser property 
value.

If the underlying HBufC value cannot be parsed into any array elements, then this method
returns an array containing only a single item. Otherwise, the HBufC is split into its
constituent elements and returned as an array.

@param aOwningProperty The property that contains this property value.
@return An array-based representation of this object. */
	{
	CDesCArray* arrayOfValues = NULL;
	const CParserPropertyValue* valueFromStorage = VersitUtils::AdditionalPropertyValueFromStorageL(aOwningProperty);
	//
	if  (valueFromStorage && valueFromStorage->Uid().iUid == KVersitPropertyCDesCArrayUid)
		{
		const CParserPropertyValueCDesCArray* valueAsArrayProperty = static_cast<const CParserPropertyValueCDesCArray*>(valueFromStorage);
		CDesCArray* array = valueAsArrayProperty->Value();
		//
		if  (array && array->Count())
			{
			// Create a new array...
			const TInt count = array->Count();
			arrayOfValues = new(ELeave) CDesCArrayFlat(count);
			CleanupStack::PushL(arrayOfValues);

			// ... and copy the elements
			for(TInt i=0; i<count; i++)
				{
				arrayOfValues->AppendL(array->MdcaPoint(i));
				}
			}
		}

	if (!arrayOfValues)
		{
		// Just copy the single element in the HBufC structure to the
		// first element of an array-based structure.
		arrayOfValues = new(ELeave) CDesCArrayFlat(1);
		CleanupStack::PushL(arrayOfValues);
		arrayOfValues->AppendL(Value());
		}

	// Create array-property storage type...
	CParserPropertyValueCDesCArray* value = new(ELeave) CParserPropertyValueCDesCArray(arrayOfValues);
	CleanupStack::Pop(arrayOfValues);
	CleanupStack::PushL(value);
	//
	return value;
	}

//
//  CParserPropertyValueBinary
//

void CParserPropertyValueBinary::ConstructL(const TDesC8& aValue)
	{
	iValue=CBufSeg::NewL(128);
	iValue->InsertL(0,aValue);
	}

EXPORT_C CParserPropertyValueBinary* CParserPropertyValueBinary::NewL(const TDesC8& aValue)
/** Allocates and constructs a new binary property value with the value specified.

Sets the property value's UID to KVersitPropertyBinaryUid.

@param aValue The property value. 
@return Pointer to the newly created binary property value. */
	{
	CParserPropertyValueBinary* self = CParserPropertyValueBinary::NewLC(aValue);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CParserPropertyValueBinary* CParserPropertyValueBinary::NewLC(const TDesC8& aValue)
/** Allocates and constructs a new binary property value with the value specified.

Leaves the object on the cleanup stack. 

Sets the property value's UID to KVersitPropertyBinaryUid.

@param aValue The property value. 
@return Pointer to the newly created binary property value. */
	{
	CParserPropertyValueBinary* self = new(ELeave) CParserPropertyValueBinary();
	CleanupStack::PushL(self);
	self->ConstructL(aValue);
	return self;
	}

EXPORT_C CParserPropertyValueBinary::~CParserPropertyValueBinary()
/** Frees all resources owned by the property value, prior to its destruction. */
	{
	delete iValue;
	}
	
EXPORT_C void CParserPropertyValueBinary::ExternalizeL(RWriteStream& aStream, const Versit::TEncodingAndCharset& aEncodingCharset, TInt aLengthOutput)
/** Externalises the binary property value into aStream.

Uses the encoding format specified in aEncodingCharset. (Any character set 
specified in aEncodingCharset is not used).

@param aStream Stream into which the value is to be externalised.
@param aEncodingCharset Specifies the character set and encoding information. 
The encoding selected for a binary property value is Versit::EBase64Encoding.
@param aLengthOutput The amount of text that has been output so far on the 
line (for the property name). */
	{
	RBufReadStream readStream;
	readStream.Open(*iValue);
	CleanupClosePushL(readStream);
	ExternalizeL(aStream, aEncodingCharset, aLengthOutput, readStream);	
	CleanupStack::PopAndDestroy(&readStream);		
	}
	
void CParserPropertyValueBinary::ExternalizeL(RWriteStream& aStream, const Versit::TEncodingAndCharset& aEncodingCharset, TInt aLengthOutput, RReadStream& aReadStream)
/** Externalises the binary property value into aStream.

Uses the encoding format specified in aEncodingCharset. (Any character set 
specified in aEncodingCharset is not used).

@param aStream Stream into which the value is to be externalised.
@param aEncodingCharset Specifies the character set and encoding information. 
The encoding selected for a binary property value is Versit::EBase64Encoding.
@param aLengthOutput The amount of text that has been output so far on the 
line (for the property name). */
	{
	TAny* extPlugIn  = NULL;

	if (PlugIn())
		{
		PlugIn()->GetInterface(KUidVersitPlugInExtension, extPlugIn);
		}
	TInt dataLength = 0;	
	MStreamBuf * streamBuf = aReadStream.Source();
	if(streamBuf)
		{
		dataLength = streamBuf->SizeL();	
		}

	if (dataLength > 0)
		{
		//Make sure read from the beginning
		TStreamPos posStart(0);
		streamBuf->SeekL(MStreamBuf::ERead, posStart); 
		/*
		1). Create a segmented buffer with optimum granularity, this will have the converted data.
		2). Read chunk of data from aReadStream and store it in temporary segmented buffer for processing.
		3). Pass the buffer for line wrapping and write it to the output stream.
		4). Repeat the steps in the loop until data in iValue is exhausted.
		*/
		//Number of segments to process.				
		TInt splitSize = 30720; //optimum size of buffer which CONARC
		CBufSeg* target = CBufSeg::NewL(splitSize);
		CleanupStack::PushL(target);
		CBufSeg* tempSeg = CBufSeg::NewL(splitSize);
		CleanupStack::PushL(tempSeg);
		HBufC8* readdata = HBufC8::NewLC(splitSize);
		RBufReadStream readStream;

		TPtr8 ptr = readdata->Des();
		TInt numOfSplits = dataLength / splitSize;
		for(TInt loop = 0;loop <= numOfSplits; ++loop)
			{
			if(loop == numOfSplits)
				{//The length of data to read at the last loop
				splitSize = dataLength%splitSize;
				if(splitSize == 0)
					{
					break;	
					}
				}

			ptr.Zero();
			aReadStream.ReadL(ptr, splitSize);
		
			tempSeg->InsertL(0, ptr);
			readStream.Open(*tempSeg);
			CleanupClosePushL(readStream);
 			// Do the BASE64 encoding
			VersitUtils::ConArcEncodeL(readStream, *target, VersitUtils::ConArcEncodingUid(aEncodingCharset.iEncoding));
			CleanupStack::PopAndDestroy(&readStream);
 			
 			if (!extPlugIn || !static_cast<MVersitPlugInExtension*>(extPlugIn)->WrapBinaryLinesL(*target, aLengthOutput))
				{
				VersitUtils::WrapLinesL(*target, KBase64MaxLineLength);
				}

			// Write the buffer to the output stream
			TInt bufPos = 0;
			TInt len = target->Size();
			
			while (bufPos < len)
				{
				TPtr8 ptr = target->Ptr(bufPos);
				aStream.WriteL(ptr);
				bufPos += ptr.Length();
				}
			
			tempSeg->Reset();
			ptr.Zero();
			target->Reset();
			}
		
		CleanupStack::PopAndDestroy(3, target);//readdata tempSeg target
		}
		
	// vCard specification says that end of the text is marked with two CRLF sequences (2nd one added latter)
	if (!extPlugIn || !static_cast<MVersitPlugInExtension*>(extPlugIn)->DisableBlankLineAfterBinaryValue())
		{
		aStream.WriteL(KVersitTokenCRLF);
		}
	}

EXPORT_C const CBufSeg* CParserPropertyValueBinary::Value() const
/** Returns the binary property value. 

@return Pointer to the property value. */
	{
	return iValue;
	}
//
//  CParserPropertyValueBinaryFile
//	
void CParserPropertyValueBinaryFile::ConstructL(const RFile& aFileHandle)
	{
	RFile dupHandle;
	User::LeaveIfError(dupHandle.Duplicate(aFileHandle));
	iFileStream.Attach(dupHandle);
	}

	
EXPORT_C CParserPropertyValueBinaryFile* CParserPropertyValueBinaryFile::NewL(const RFile& aFileHandle)
/** Allocates and constructs a new file property value with the file handle to the data.

The property value's UID will be set to KVersitPropertyBinaryUid.

@param aFileHandle The file handle to the binary data. 
@return Pointer to the newly created file property value. */
	{
	CParserPropertyValueBinaryFile* self = new(ELeave) CParserPropertyValueBinaryFile();
	CleanupStack::PushL(self);
	self->ConstructL(aFileHandle);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CParserPropertyValueBinaryFile::~CParserPropertyValueBinaryFile()
/** Frees all resources owned by the property value, prior to its destruction. */
	{
	iFileStream.Close();
	}
	
void CParserPropertyValueBinaryFile::ExternalizeL(RWriteStream& aStream, const Versit::TEncodingAndCharset& aEncodingCharset, TInt aLengthOutput)
/** Externalises the binary data through the file handle into aStream.

Uses the encoding format specified in aEncodingCharset. (Any character set 
specified in aEncodingCharset is not used).

@param aStream Stream into which the value is to be externalized.
@param aEncodingCharset Specifies the character set and encoding information. 
The encoding selected for a binary property value is Versit::EBase64Encoding.
@param aLengthOutput The amount of text that has been output so far on the 
line (for the property name). */
	{
	CParserPropertyValueBinary::ExternalizeL(aStream, aEncodingCharset, aLengthOutput, iFileStream);
	}

//
//  CParserPropertyValueCDesCArray
//

EXPORT_C CParserPropertyValueCDesCArray::CParserPropertyValueCDesCArray(CDesCArray* aValue)
	: CParserPropertyValue(TUid::Uid(KVersitPropertyCDesCArrayUid))
	,iValue(aValue)
/** Constructs a new descriptor array property value with the array pointed to 
by aValue.

Sets the property value's UID to KVersitPropertyCDesCArrayUid.

The property value takes ownership of aValue.

Called by CVersitParser::MakePropertyValueL() when internalising from a 
stream.

@param aValue Pointer to the descriptor array. */
	{}

EXPORT_C CParserPropertyValueCDesCArray::~CParserPropertyValueCDesCArray()
/** Frees all resources owned by the property value array, prior to its destruction. */
	{
	delete iValue;
	}

EXPORT_C TBool CParserPropertyValueCDesCArray::IsAsciiCharacterSetSufficient()
// From CParserPropertyValue
/** Tests whether the property value can be represented using the ASCII character 
set.

Tests every item in the array and returns ETrue only if all items contain only 
7-bit characters.

@return ETrue if the property value can be represented using the ASCII character 
set. If not, EFalse. 
@see VersitUtils::DescriptorContainsOnlySevenBitCharacters() */
	{
	if	(!iValue)
		return ETrue;
	TInt count = iValue->Count();
	if	(!count)
		return ETrue;

	for(TInt i=0; i<count; i++)
		{
		const TDesC& item = (*iValue)[i];
		if	(!VersitUtils::DescriptorContainsOnlySevenBitCharacters(item))
			return EFalse;
		}

	return ETrue;
	}

EXPORT_C void CParserPropertyValueCDesCArray::ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt aLengthOutput)
// From CParserPropertyValue
/** Externalizes the descriptor array property value into aStream.

Uses the character set and encoding format specified in aEncodingCharset.

Called by CParserProperty::ExternalizeL().

@param aStream Stream into which the value is to be externalised.
@param aEncodingCharset Specifies the character set and encoding information.
@param aLengthOutput The amount of text that has been output so far on the 
line (for the property name), which needs to be taken into account when calculating 
if and where any line break should occur. */
	{
	FoldEncodeAndWriteValueToStreamL(aStream, iValue, aEncodingCharset, aLengthOutput);
	}

EXPORT_C TBool CParserPropertyValueCDesCArray::IsPresent(const TDesC& aValue) const
/** Tests whether a specified value is present in the array of descriptors owned 
by the property value object.

Not used internally.

@param aValue The value of interest. 
@return ETrue if the value specified is present in the descriptor array. EFalse 
if not. */
	{
	if (iValue)
		{
		TInt count=iValue->Count();
		for (TInt ii=0; ii<count; ii++)
			{
			if ((*iValue)[ii]==aValue)
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}

//
// CParserPropertyValueTimeZone
//

EXPORT_C CParserPropertyValueTimeZone::CParserPropertyValueTimeZone(TTimeIntervalSeconds aValue)
	: CParserPropertyValue(TUid::Uid(KVersitPropertyTimeZoneUid))
	, iValue(aValue)
/** Constructs a new time zone property value parser with a time interval in seconds, 
which represents the universal time offset of the time zone.

Sets the property value's UID to KVersitPropertyTimeZoneUid.

@param aValue A time interval (in seconds) which represents the offset of 
the time zone from universal time. The property value takes ownership of the 
pointer. */
	{}

void CParserPropertyValueTimeZone::EncodeTimeZone(TDes8& aBuf,TTimeIntervalSeconds aValue)
	{
	TInt hours=aValue.Int()/3600;
	TInt sign=1;
	_LIT8(KTwoDigits,"%02d");
	if (hours<0)	//This code could be simplified if & when the E32 bug is fixed.
		{
		_LIT8(KMinus,"-");
		aBuf=KMinus;
		sign=-1;
		}
	else
		{
		_LIT8(KPlus,"+");
		aBuf=KPlus;
		}
	aBuf.AppendFormat(KTwoDigits,sign*hours);
	TInt minutes=sign*(aValue.Int()/60-60*hours);
	if (minutes>0)
		aBuf.AppendFormat(KTwoDigits,minutes);
	}

EXPORT_C void CParserPropertyValueTimeZone::ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& /*aEncodingCharset*/
																												,TInt /*aLengthOutput*/)
/** Externalises the time zone property value into aStream.

@param aStream Stream into which the value is to be externalised.
@param aEncodingCharset Specifies the character set and encoding information. 
(Not used by this function).
@param aLengthOutput The amount of text that has been output so far 
on the line. (Not used by this function). */
	{
	TBuf8<KVersitDefaultBufferSize> buf;
	EncodeTimeZone(buf,iValue.Int());
	aStream.WriteL(buf);
	}

//
// CVersitDaylight
//

CVersitDaylight::CVersitDaylight(TBool aSavings, TTimeIntervalSeconds aOffset, TVersitDateTime* aStartTime, TVersitDateTime* aEndTime)
	: iSavings(aSavings),
	iOffset(aOffset),
	iStartTime(aStartTime),
	iEndTime(aEndTime)
	{
	if (aSavings)
		{
		iStartTimeSortKey = TTime(iStartTime->iDateTime).Int64();
		}
	else
		{
		iStartTimeSortKey = TTime(Time::MinTTime()).Int64();
		}
	}

void CVersitDaylight::ConstructL(const TDesC& aStandardDesignation, const TDesC& aDaylightDesignation)
	{
	iStandardDesignation = aStandardDesignation.AllocL();
	CleanupStack::PushL(iStandardDesignation);
	iDaylightDesignation = aDaylightDesignation.AllocL();
	CleanupStack::Pop(iStandardDesignation);
	}

EXPORT_C CVersitDaylight* CVersitDaylight::NewL(TBool aSavings, TTimeIntervalSeconds aOffset, TVersitDateTime* aStartTime, TVersitDateTime* aEndTime, const TDesC& aStandardDesignation, const TDesC& aDaylightDesignation)
/** Allocates and constructs a new universal time offset object.

Ownership of aStartTime and aEndTime is taken at end of this function.

@param aSavings The daylight savings flag, i.e. ETrue if daylight saving is 
currently observed in the locale; EFalse if not. 
@param aOffset The universal time offset (in seconds). 
@param aStartTime The date/time at which the period for daylight saving begins. 

@param aEndTime The date/time at which the period for daylight saving ends. 
@param aStandardDesignation The standard time designation, e.g. GMT, EST. 
@param aDaylightDesignation The daylight saving time designation, e.g. BST, 
EDT. 
@return The new universal time offset object. */
	{
	CVersitDaylight* self = new(ELeave) CVersitDaylight(aSavings, aOffset, aStartTime, aEndTime);
	CleanupStack::PushL(STATIC_CAST(TAny*,self));
	self->ConstructL(aStandardDesignation, aDaylightDesignation);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CVersitDaylight::~CVersitDaylight()
/** The destructor frees all resources owned by the object, prior to its destruction. */
	{
	delete iStartTime;
	delete iEndTime;
	delete iStandardDesignation;
	delete iDaylightDesignation;
	}

//
// CParserPropertyValueDaylight
//

EXPORT_C CParserPropertyValueDaylight::CParserPropertyValueDaylight(CVersitDaylight* aValue)
: CParserTimePropertyValue(TUid::Uid(KVersitPropertyDaylightUid)), iValue(aValue)
/** Constructs a new CParserPropertyValueDaylight.

Sets the property value's UID to KVersitPropertyDaylightUid.

@param aValue Pointer to the daylight saving specification. The property value 
takes ownership of the pointer. */
	{}

EXPORT_C CParserPropertyValueDaylight::~CParserPropertyValueDaylight()
/** Frees all resources owned by the property value, prior to its destruction. */
	{
	delete iValue;
	}

EXPORT_C void CParserPropertyValueDaylight::ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& /*aIncrement*/,const CVersitDaylight* /*aDaylight*/)
/** This function does nothing, as daylight saving times should always be specified 
as local times.

@param aIncrement Not used.
@param aDaylight Not used. 
@deprecated since 9.1
*/
	{}

EXPORT_C void CParserPropertyValueDaylight::ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& /*aIncrement*/)
/** Converts the start and end times for the daylight saving period to machine-local.

This process involves adjusting the date/time values by the offset in aIncrement.

It has no effect if daylight savings is not in effect or on values 
already stored in machine-local time.

This function is deprecated.

@param aIncrement A time interval in seconds to add to the start and end date/times 
for daylight saving. This should normally be the universal time offset for 
the machine's locale. 
@deprecated since 9.1
*/
	{
	}

EXPORT_C void CParserPropertyValueDaylight::ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& aEncodingCharset,TInt aLengthOutput)
// From CParserProperty
/** Externalizes the daylight saving property value into aStream.

This is invoked by the parser's ExternalizeL() function.

@param aStream Stream to which the value is to be externalised.
@param aEncodingCharset Specifies the character set and encoding information.
@param aLengthOutput The amount of text that has been output so far on the 
line, which needs to be taken into account when calculating if and where any 
line break should occur. */
	{
	if (!iValue)
		return;
	TBuf<64> buf;
	if (!iValue->iSavings)
		buf.Append(KVersitVarTokenFALSE);
	else
		{
		buf.Append(KVersitVarTokenTRUE);
		buf.Append(KVersitTokenSemiColonUnicode);
		TBuf8<KVersitDefaultBufferSize> timeBuf;
		CParserPropertyValueTimeZone::EncodeTimeZone(timeBuf,iValue->iOffset);
		Append(buf,timeBuf);
		buf.Append(KVersitTokenSemiColonUnicode);
		if (iValue->iStartTime)
			{
			EncodeVersitDateTimeL(timeBuf,*iValue->iStartTime);
			Append(buf,timeBuf);
			}
		buf.Append(KVersitTokenSemiColonUnicode);
		if (iValue->iEndTime)
			{
			EncodeVersitDateTimeL(timeBuf,*iValue->iEndTime);
			Append(buf,timeBuf);
			}
		buf.Append(KVersitTokenSemiColonUnicode);
		if (iValue->iStandardDesignation)
			{
			buf.Append(*(iValue->iStandardDesignation));
			}
		buf.Append(KVersitTokenSemiColonUnicode);
		if (iValue->iDaylightDesignation)
			{
			buf.Append(*(iValue->iDaylightDesignation));
			}
		}
	FoldAndWriteValueToStreamL(aStream,buf,aEncodingCharset,aLengthOutput);
	}

//
//  CParserPropertyValueDateTimeL
//
EXPORT_C CParserPropertyValueDateTime::CParserPropertyValueDateTime(TVersitDateTime* aValue)
	: CParserTimePropertyValue(TUid::Uid(KVersitPropertyDateTimeUid))
	, iValue(aValue)
/** Constructs a CParserPropertyValueDateTime with a TVersitDateTime value. 

Sets the property value's UID to KVersitPropertyDateTimeUid.

@param aValue Pointer to the date/time specification, which includes information 
about the date/time. The property value takes ownership of the pointer. */
	{}

EXPORT_C CParserPropertyValueDateTime::~CParserPropertyValueDateTime()
/** Frees all resources owned by the property value, prior to its destruction. */
	{
	delete iValue;
	}

EXPORT_C void CParserPropertyValueDateTime::ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight)
/** Converts the object's date/time value into universal time.

The date/time is checked against the daylight saving information provided 
in aDaylight. If it falls inside the daylight saving period then the daylight 
saving offset is subtracted from the time to convert it to universal time. 
Otherwise aIncrement is added to the date/time to convert it to universal 
time.

Note that the daylight savings offset will adjust the time both for the daylight 
saving and for the time zone.

The function has no effect if the value is already stored as universal time.

If aDaylight is a NULL pointer then aIncrement is used.

@param aIncrement A time interval in seconds which represents the negative 
of the time zone of the originating machine.For instance, if the time zone 
is +04:30 (that is 4hr 30mins ahead of UTC), aIncrement should be set to minus 
the number of seconds in 4hr 30mins.
@param aDaylight Pointer to the specification for daylight saving. If the date/time 
value is within the period for daylight saving, the value is modified by the 
daylight saving offset (which accounts for both the time zone and daylight 
saving rule). 
@deprecated since 9.1
*/
	{
	if ((iValue) && (iValue->iRelativeTime!=TVersitDateTime::EIsUTC) && !iValue->IsFlagSet(TVersitDateTime::EExportLeaveAsLocalTime))
		{
		ConvertDateTime(&iValue->iDateTime,aIncrement,aDaylight);
		iValue->iRelativeTime=TVersitDateTime::EIsUTC;
		}
	}

EXPORT_C void CParserPropertyValueDateTime::ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement)
/** Converts the date/time property value into machine-local time. 

This process involves adjusting the date/time value by the offset in aIncrement.

The function has no effect if the value is already stored as machine-local 
time.

@param aIncrement A time interval which represents the number of seconds which 
is to be added to the date/time value. This should normally be the universal 
time offset for the machine's locale. 
@deprecated since 9.1
*/
	{
	if ((iValue)&&(iValue->iRelativeTime==TVersitDateTime::EIsUTC))
		{
		ConvertDateTime(&iValue->iDateTime,aIncrement,NULL);
		iValue->iRelativeTime=TVersitDateTime::EIsMachineLocal;
		}
	}

EXPORT_C void CParserPropertyValueDateTime::ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& /*aEncodingCharset*/
																																,TInt /*aLengthOutput*/)
// From CParserPropertyValue
/** Externalises the date/time property value to aStream.

@param aStream Stream to which the value should be externalised
@param aEncodingCharset Specifies the character set and encoding information. This 
is not used by this function.
@param aLengthOutput The amount of text that has been output so far on the line (for the 
property name). This is not used by this function. */
	{
	if (iValue)
		{
		TBuf8<KVersitMaxDateTimeLength> buf;
		EncodeVersitDateTimeL(buf,*iValue);
		aStream.WriteL(buf);
		}
	}


//
//  CParserPropertyValueDate
//
EXPORT_C CParserPropertyValueDate::CParserPropertyValueDate(TVersitDateTime* aValue)
:CParserTimePropertyValue(TUid::Uid(KVersitPropertyDateUid))
	,iValue(aValue)
/** Constructs a date property value parser with a TVersitDateTime value.

Sets the property value's UID to KVersitPropertyDateUid.

@param aValue Specifies the date and information about the date. The object 
takes ownership of the pointer. */
	{}

EXPORT_C CParserPropertyValueDate::~CParserPropertyValueDate()
/** Frees all resources owned by the object, prior to its destruction. */
	{
	delete iValue;
	}

EXPORT_C void CParserPropertyValueDate::ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& /*aIncrement*/,const CVersitDaylight* /*aDaylight*/)
/** This function is inherited from the base class CParserTimePropertyValue.

It has an empty implementation because this class represents a date not a time value. 

@param aIncrement Not used.
@param aDaylight Not used. 
@deprecated since 9.1
*/
	{}

EXPORT_C void CParserPropertyValueDate::ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& /*aIncrement*/)
/** This function is inherited from the base class CParserTimePropertyValue.

It has an empty implementation because this class represents a date not a time value. 

@param aIncrement Not used.
@deprecated since 9.1
*/
	{}

EXPORT_C void CParserPropertyValueDate::ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& /*aEncodingCharset*/,TInt /*aLengthOutput*/)
// From CParserProperty
/** Externalises the property value into aStream.

@param aStream Stream to which the value should be externalised.
@param aEncodingCharset Specifies the character set and encoding information. This 
information is not used by this function
@param aLengthOutput The amount of text that has been output so far on the line 
(for the property name). This value is not used by this function. */
	{
	if (iValue)
		{
		TBuf8<KVersitMaxDateTimeLength> buf;
		EncodeVersitDateTimeL(buf,*iValue,EFalse);
		aStream.WriteL(buf);
		}
	}

//
//  CParserPropertyValueMultiDateTimeL
//
EXPORT_C CParserPropertyValueMultiDateTime::CParserPropertyValueMultiDateTime(CArrayPtr<TVersitDateTime>* aValue)
	: CParserTimePropertyValue(TUid::Uid(KVersitPropertyMultiDateTimeUid))
	,iValue(aValue)
/** Constructs a multi-date/time property value parser with an array of TVersitDateTimes. 

Sets the property value's UID to KVersitPropertyMultiDateTimeUid.

@param aValue Pointer to an array of TVersitDateTime objects, each of which 
specifies a date/time value, and information about the value. The property 
value takes ownership of the array. */
	{}

EXPORT_C CParserPropertyValueMultiDateTime::~CParserPropertyValueMultiDateTime()
/** Frees all resources owned by the property value, prior to its destruction. */
	{
	if (iValue)
		{
		iValue->ResetAndDestroy();
		delete iValue;
		}
	}

EXPORT_C void CParserPropertyValueMultiDateTime::ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight)
/** Converts each of the date/time values owned by the object into universal time.

Each date/time is checked against the daylight saving information provided 
in aDaylight. If it falls inside the daylight saving period then the daylight 
saving offset is subtracted from the time to convert it to universal time. 
Otherwise aIncrement is added to the date/time of the alarm to convert it 
to universal time.

Note that the daylight savings offset will adjust the time both for the daylight 
saving and for the time zone.

The function does not effect any date/time value already stored in universal 
time.

If aDaylight is a NULL pointer then aIncrement is used.

@param aIncrement A time interval in seconds which represents the negative 
of the time zone of the originating machine. For instance, if the time zone 
is +04:30 (that is 4hr 30mins ahead of UTC), aIncrement should be set to minus 
the number of seconds in 4hr 30mins.
@param aDaylight Pointer to the specification for daylight saving. If the time 
value is within the period for daylight saving, the value is modified by the 
daylight saving offset (which accounts for both the time zone and daylight 
saving rule). 
@deprecated since 9.1
*/
	{
	if (iValue)
		{
		TInt count=iValue->Count();
		for (TInt ii=0;ii<count; ii++)
			{
			TVersitDateTime& time=*(*iValue)[ii];
			if (!(time.iRelativeTime==TVersitDateTime::EIsUTC) && !time.IsFlagSet(TVersitDateTime::EExportLeaveAsLocalTime))
				{
				ConvertDateTime(&time.iDateTime,aIncrement,aDaylight);
				time.iRelativeTime=TVersitDateTime::EIsUTC;
				}
			}
		}
	}

EXPORT_C void CParserPropertyValueMultiDateTime::ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement)
/** Converts the date/time property values into machine-local time. 

This process involves adjusting the date/time values by the offset in aIncrement.

The function has no effect on any values already stored in machine-local time.

@param aIncrement A time interval which represents the number of seconds which 
is to be added to the date/time values. This should normally be the universal 
time offset for the machine's locale. 
@deprecated since 9.1
*/
	{
	if (iValue)
		{
		TInt count=iValue->Count();
		for (TInt ii=0;ii<count; ii++)
			{
			if ((*iValue)[ii]->iRelativeTime==TVersitDateTime::EIsUTC)
				{
				ConvertDateTime(&(*iValue)[ii]->iDateTime,aIncrement,NULL);
				(*iValue)[ii]->iRelativeTime=TVersitDateTime::EIsMachineLocal;
				}
			}
		}
	}

EXPORT_C void CParserPropertyValueMultiDateTime::ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& /*aEncodingCharset*/
																																	,TInt aLengthOutput)
// From CParserPropertyValue
/** Externalises the array of date/time property values into aStream.

@param Stream to which the value should be externalised.
@param Specifies the character set and encoding information. This information 
is not used by this function.
@param The amount of text that has been output on the line so far, which 
needs to be taken into account when calculating if and where any line break 
should occur. */
	{
	const TInt maxCharsTimeOutput=KVersitMaxDateTimeLength+1;
	if (iValue)
		{
		TBuf8<KVersitMaxDateTimeLength> buf;
		TInt count = iValue->Count();
		TInt ii=0;
		FOREVER
			{
			if (aLengthOutput+maxCharsTimeOutput>KMaxExternalizedTokenLength)
				{
				aStream.WriteL(KVersitLineBreak);
				aLengthOutput=2;
				}
			EncodeVersitDateTimeL(buf,*(*iValue)[ii]);
			aStream.WriteL(buf);
			if (++ii==count)
				break;
			aStream.WriteL(KVersitTokenSemiColon);
			aLengthOutput+=buf.Length()+1;
			}
		}
	}

//
//  CParserPropertyValueInt
//
EXPORT_C CParserPropertyValueInt::CParserPropertyValueInt(TInt aValue)
	: CParserPropertyValue(TUid::Uid(KVersitPropertyIntUid)), iValue(aValue)
/** Constructs the property value with a signed integer.

Sets the property value's UID to KVersitPropertyIntUid.

@param aValue A signed integer value. */
	{}

EXPORT_C void CParserPropertyValueInt::ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& /*aEncodingCharset*/,TInt /*aLengthOutput*/)
// From CParserPropertyValue
/** Externalizes the integer property value to aStream. 

@param aStream Stream to which the value is to be externalised
@param aEncodingCharset Specifies the character set and encoding information. 
Not used by this function.
@param aLengthOutput The amount of text that has been output so far on the line. 
Not used by this function.*/
	{
	TBuf8<KVersitDefaultBufferSize> buf;
	buf.Num(iValue);
	aStream.WriteL(buf);
	}

//
//	CParserProperty
//

EXPORT_C CParserProperty::CParserProperty(CParserPropertyValue& aPropertyValue,CArrayPtr<CParserParam>* aArrayOfParams)
	: iPropertyValue(&aPropertyValue), iArrayOfParams(aArrayOfParams)
// Constructor takes ownership of aArrayOfParams, but not aName
	{}

EXPORT_C CParserProperty::CParserProperty(CArrayPtr<CParserParam>* aArrayOfParams)
	: iArrayOfParams(aArrayOfParams)
/** C++ constructor which sets the array of property parameters only.

The property takes ownership of the array of parameters.

@param aArrayOfParams Pointer to the property parameters. */
	{}

EXPORT_C void CParserProperty::ConstructSelfL(CParserProperty& aSelf,const TDesC8& aName)
	{
	if (aName.Length()>0)
		{
		CleanupStack::PushL(STATIC_CAST(TAny*,&aSelf));
		aSelf.SetNameL(aName);
		CleanupStack::Pop(&aSelf);
		}
	}

EXPORT_C CParserProperty* CParserProperty::NewL(CParserPropertyValue& aPropertyValue, const TDesC8& aName, CArrayPtr<CParserParam>* aArrayOfParams)
/** Allocates and constructs a new vCalendar or vCard property with the property 
value, property name and array of property parameters specified.

Takes ownership of aPropertyValue and aArrayOfParams.

@param aPropertyValue The property value.
@param aName The property name.
@param aArrayOfParams Pointer to the property parameters. 
@return Pointer to the newly created property. */
	{
	// coverity [alloc_fn]
	CParserProperty* self = new(ELeave) CParserProperty(aPropertyValue,aArrayOfParams);
	ConstructSelfL(*self,aName);
	return self;
	}

EXPORT_C CParserProperty::~CParserProperty()
/** Frees all resources owned by the property, prior to its destruction. */
	{
	TRAPD(errIgnored, VersitUtils::FreeAdditionalPropertyStorageL(*this));
	UNUSED_VAR(errIgnored); // used to suppress build warnings
	//
	delete iPropertyValue;
	delete iPropertyName;
	if (iArrayOfParams)
		{
		iArrayOfParams->ResetAndDestroy();
		delete iArrayOfParams;
		}
	}

EXPORT_C void CParserProperty::ExternalizeL(RWriteStream& aStream, CVersitParser* aVersitParser)
/** Externalises a property. 

The property has the general format: 

\<NAME\>;\<PARAMNAME=PARAMVALUE\>:\<VALUE\>

Checks the property value, and Versit's default encoding and character 
set, to decide what encoding, if any, and character set should be applied 
to the property value.

If the encoding is not the default, (no encoding), then a parameter (with 
the name KVersitTokenENCODING) is added to the property to specify the encoding 
used. Similarly, if the character set used is not the default, (Ascii), 
then a parameter (with the name KVersitTokenCHARSET) is added 
to the property to specify the character set used.

A parser parameter needs to be supplied when calling this function: if NULL 
is passed then a panic is raised.

The name is externalised by this function directly. The parameters (if there 
are any) are externalised using CParserParam::ExternalizeL(). The value (if 
there is one) is externalised using the ExternalizeL() function of the appropriate 
property value class.

Using the encoding format and character set selected for the property value, 
the function forms a Versit specific UID (Versit::TEncodingAndCharset). When 
the property value is externalised, this UID is passed to the property value 
class' ExternalizeL() function, so that it knows which character set and encoding 
format to use.

Also passed to the ExternalizeL() function of the property value class are 
the output stream and the 'output length'. The output stream is the same as 
is passed to this function. The 'output length' refers to the amount of data 
that has been output to the current line in the stream. This is kept track 
of while the property name and parameters are externalised, and then passed 
to the ExternalizeL() function of the property value class to make sure the 
line length does not exceed the maximum length allowed.

As well as inserting the semi-colon and colon between the name and parameter(s) 
and parameter(s) and value, a CRLF is written at the end.

@param aStream Stream to which the value is to be externalised.
@param aVersitParser The Versit parser whose ExternalizeL() function calls 
this function, and in whose array of properties this instance of CParserProperty 
is held. */
	{
	if	(!iPropertyValue)
		return;
	__ASSERT_DEBUG(aVersitParser,Panic(EVersitPanicNeedToSpecifyParser));

	// Updated in this method
	const TUid propertyValueUid(iPropertyValue->Uid());
	TBool requiresEncoding = EFalse;
	TUint propCharsetId=KCharacterSetIdentifierAscii;
	
	TAny* extPlugIn=NULL;

	// Why Daylight?
	if	(propertyValueUid == TUid::Uid(KVersitPropertyHBufCUid) ||
		 propertyValueUid == TUid::Uid(KVersitPropertyCDesCArrayUid) ||
		 propertyValueUid == TUid::Uid(KVCalPropertyAlarmUid) ||
		 propertyValueUid == TUid::Uid(KVersitPropertyDaylightUid) ||
		 propertyValueUid == TUid::Uid(KVersitPropertyBinaryUid) ||
		 propertyValueUid == TUid::Uid(KVCalPropertyExtendedAlarmUid)
		)
		{
		TBool sevenBitsAreSufficient = ETrue;

		// This next section checks to see if the buffer passed to CheckEncodings requires a ENCODING=SOMETHING
		// line as a property parameter for this item. If it does, then encode is set to ETrue (in most instances...
		// Alarms, and Daylight's override any ETrue value to EFalse. Buffers that require this property parameter
		// usually contain carriage returns, linefeeds or contain characters that fall outside the 0-127 range
		switch(propertyValueUid.iUid)
			{
		case KVersitPropertyHBufCUid:
			{
			CParserPropertyValueHBufC* bufValue = STATIC_CAST(CParserPropertyValueHBufC*, iPropertyValue);
			//
			sevenBitsAreSufficient = VersitUtils::DescriptorContainsOnlySevenBitCharacters(bufValue->Value());
			requiresEncoding = VersitUtils::RequiresEncoding(bufValue->Value());
			break;
			}
		case KVersitPropertyCDesCArrayUid:
			{
			CParserPropertyValueCDesCArray* arrayValue = STATIC_CAST(CParserPropertyValueCDesCArray*, iPropertyValue);
			const CDesCArray* value = arrayValue->Value();
			if	(value)
				{
				TInt noOfProperties = value->Count();
				for (TInt jj = 0; jj < noOfProperties; jj++)
					{
					TPtrC pValue(value->MdcaPoint(jj));

					// Keep local flags
					TBool sevenBitsAreSufficientInternal = VersitUtils::DescriptorContainsOnlySevenBitCharacters(pValue);
					TBool requiresEncodingInternal = VersitUtils::RequiresEncoding(pValue);

					// Now check combined with global flags	& update as appropriate
					if	(!sevenBitsAreSufficientInternal && sevenBitsAreSufficient)
						sevenBitsAreSufficient = sevenBitsAreSufficientInternal;
					if	(requiresEncodingInternal && !requiresEncoding)
						requiresEncoding = requiresEncodingInternal;

					// Optimisation
					if	(!sevenBitsAreSufficient && requiresEncoding)
						break; // Need both of these, no point checking anymore
					}
				}
			break;
			}
		case KVCalPropertyAlarmUid:
			{
			CParserPropertyValueAlarm* bufValue = STATIC_CAST(CParserPropertyValueAlarm*, iPropertyValue);
			CVersitAlarm* alarm = bufValue->Value();
			if	(alarm)
				{
				if	(alarm->iAudioContent)
					{
					sevenBitsAreSufficient	= bufValue->IsAsciiCharacterSetSufficient();
					requiresEncoding		= VersitUtils::RequiresEncoding(*alarm->iAudioContent);		
					}
				// Only do this check if the audio content can still fit in 7 bits otherwise
				// we will need to encode both anyway...
				if	(alarm->iNote && sevenBitsAreSufficient)
					sevenBitsAreSufficient = VersitUtils::DescriptorContainsOnlySevenBitCharacters(*alarm->iNote);
				}
			break;
			}
		case KVCalPropertyExtendedAlarmUid:
			{
			CParserPropertyValueExtendedAlarm* bufValue = STATIC_CAST(CParserPropertyValueExtendedAlarm*, iPropertyValue);
			CVersitExtendedAlarm* extendedAlarm = bufValue->Value();
			if (extendedAlarm)
				{
				if	(extendedAlarm->iContent)
					{
					if (extendedAlarm->iDisposition == CVersitExtendedAlarm::EDispositionUrl)
						{
						sevenBitsAreSufficient = bufValue->IsAsciiCharacterSetSufficient();
						}
					else
						{
						sevenBitsAreSufficient = EFalse;
						requiresEncoding = ETrue;
						}
					}
				}
			break;
			}
		case KVersitPropertyDaylightUid:
			{
			// AW: requiresEncoding not used - I'm not sure why
			CParserPropertyValueDaylight* bufValue = STATIC_CAST(CParserPropertyValueDaylight*, iPropertyValue);
			CVersitDaylight* daylight = bufValue->Value();
			if	(daylight)
				{
				if	(daylight->iStandardDesignation)
					sevenBitsAreSufficient = VersitUtils::DescriptorContainsOnlySevenBitCharacters(*daylight->iStandardDesignation);

				// Only do this check if 7 bits was suffient for the standard designation
				if	(daylight->iDaylightDesignation && sevenBitsAreSufficient)
					sevenBitsAreSufficient = VersitUtils::DescriptorContainsOnlySevenBitCharacters(*daylight->iDaylightDesignation);
				}
			break;
			}
		case KVersitPropertyBinaryUid:
			{
			// Not handled here, see further down in the encodings "special case" section
			break;
			}
		default:
			__ASSERT_DEBUG(EFalse, User::Invariant());
			break;
			} // switch

		/*
		 * Work out what character set we need to use for this property.
		 *
		 * Always try and use 7 bit ASCII because this is assumed as the default
		 * and therefore requires no CHARSET parameter.
		 *
		 * If ASCII isn't sufficient (see 'sevenBitsAreSufficient'), because there
		 * are character that can't be represented in ASCII then use the default
		 * charset instead. However, in the instance where ASCII is also the
		 * default, then use UTF-8.
		 *
		 * UTF-7 overrides all charsets and if specified as the default it should always be
		 * used in preference to ASCII or any other CHARSET
		 */
		if	(!sevenBitsAreSufficient)
			{
			propCharsetId = aVersitParser->DefaultCharSetId();
			if	(propCharsetId == KCharacterSetIdentifierAscii && !iPropertyValue->IsAsciiCharacterSetSufficient())
				propCharsetId = KCharacterSetIdentifierUtf8;
			}

		/*
		 * If a default encoding has been specified then always use that, regardless of
		 * whether its required or not.
		 */
		if	(!requiresEncoding && aVersitParser->DefaultEncoding() != Versit::ENoEncoding)
			requiresEncoding = ETrue;
		} // if

	/*
	 * If the property value is 8 bit (because of the required character set) or the value
	 * cannot be represented as is (e.g. it might contain CR or LF inside the value) then
	 * the value must be encoded.
	 *
	 * If it's just the charset that's 8bit, then we only need write ENCODING=8-BIT, but
	 * if it's more complicated because of the property value, then we must use a suitable
	 * encoding method.
	 */
	Versit::TVersitEncoding encoding = Versit::ENoEncoding;
	if (!aVersitParser->PlugIn() || !aVersitParser->PlugIn()->EncodingType(encoding,requiresEncoding,aVersitParser->DefaultEncoding()
																										,propertyValueUid,propCharsetId))
		{
		if (propertyValueUid == TUid::Uid(KVersitPropertyBinaryUid))
			{
			// A special case - this is encoded using BASE64 *always*
			encoding = Versit::EBase64Encoding;
			}
		else if ((propertyValueUid == TUid::Uid(KVCalPropertyExtendedAlarmUid) && requiresEncoding))
			{
			encoding = Versit::EBase64Encoding;
			}
		else if	(requiresEncoding)
			{
			// This is superceeds an 8-bit charset - We will use the default, or
			// if none is specified then we'll go with QP.
			encoding = aVersitParser->DefaultEncoding();
			if	(encoding == Versit::ENoEncoding || (propertyValueUid==TUid::Uid(KVCalPropertyAlarmUid) && encoding==Versit::EBase64Encoding))
				encoding = Versit::EQuotedPrintableEncoding;
			}
		else if (VersitUtils::EightBitEncoding(propCharsetId))
			{
			// It's just an 8 bit charset, no QP or B64 required - Do nothing
			encoding = Versit::EEightBitEncoding;
			}
		else
			{
			// Don't need any ENCODING=<something> tag - Do nothing
			}
		/*
		 * Some properties can't output Base64 data
		 * this is now corrected so the encoded is specified correctly
		 */
		if (encoding==Versit::EBase64Encoding)
			{
			if (propertyValueUid == TUid::Uid(KVersitPropertyTimeZoneUid) ||
				propertyValueUid == TUid::Uid(KVersitPropertyDateTimeUid) ||
				propertyValueUid == TUid::Uid(KVersitPropertyDateUid) ||
				propertyValueUid == TUid::Uid(KVersitPropertyMultiDateTimeUid) ||
				propertyValueUid == TUid::Uid(KVersitPropertyIntUid) ||
				propertyValueUid == TUid::Uid(KVCalPropertyRecurrenceUid))
				{
				encoding=Versit::ENoEncoding;
				}
			}
		}

	/*
	 * If we have to add an encoding line parameter, then we do that here. If the encoding
	 * is BASE64 or QP, then we always write the encoding tag.
	 *
	 * In addition, if we're using an 8 bit character set, E.g. ISO-8859-1, then we should
	 * also add the 8-bit encoding
	 */
	if	(encoding != Versit::ENoEncoding)
		{
		CParserParam* param = CParserParam::NewL(KVersitTokenENCODING,KNullDesC8);
		CleanupStack::PushL(param);
		TBool pramAdded=EFalse;
		if (aVersitParser->PlugIn())
			{
			const TDesC8& des=aVersitParser->PlugIn()->EncodingName(encoding);
			if (des.Length()>0)
				{
				param->SetValueL(des);
				pramAdded=ETrue;
				}
			}
		if (!pramAdded)
			param->SetValueL(VersitUtils::IANAEncodingName(encoding));
		AddParamL(param); // takes ownership
		CleanupStack::Pop(param);
		if (encoding==Versit::EEightBitEncoding)
			encoding=Versit::ENoEncoding;
		}
	else
		{// delete the ENCODING parameter if it was there
		TPtrC8 bufEncoding(KVersitTokenENCODING);
		DeleteParam(bufEncoding);
		}

	/*
	 * We now combine the encoding (if any) with the character set to form
	 * a versit specific uid which represents both of these things in one
	 * value.
	 *
	 * This uid is passed to the property value when it's externalized, and
	 * used to ensure that any text is written in the correct character set
	 * and with the correct encoding.
	 *
	 */
	Versit::TEncodingAndCharset encodingAndCharset(encoding,propCharsetId);
	aVersitParser->SetCharacterConverter(encodingAndCharset);

	/*
	 * In the instance where ASCII representation is not sufficient, it is necessary to create
	 * an extra property parameter which indicates the CHARSET that was used during the
	 * export process.
	 */

	/*
	 * vCard 3.0 format allows automatic CHARSET parameter export to be disabled.
	 * And to generate a content line in the format <NAME>;<PARAMNAME=PARAMVALUE>:<VALUE> ,
	 * lines longer than 75 characters SHOULD be folded as defined by RFC2425 and RFC2426.
	 */
	TBool disableCharset=EFalse;
	if (aVersitParser->PlugIn())
		{
		aVersitParser->PlugIn()->GetInterface(KUidVersitPlugInExtension, extPlugIn);
		if (extPlugIn)
			{
			disableCharset = static_cast<MVersitPlugInExtension*>(extPlugIn)->DisableCharsetParam();
			}
		}

	if	(propCharsetId != KCharacterSetIdentifierAscii && !disableCharset)
		{
		CParserParam* param = CParserParam::NewL(KVersitTokenCHARSET,KNullDesC8);
		CleanupStack::PushL(param);
		HBufC8* standardName = aVersitParser->UnicodeUtils().StandardNameL(propCharsetId);
		if (standardName->CompareF(KVersitTokenUTF8()) == 0)
			{
				standardName->Des().Fold();
			}
		param->SetValueL(standardName);
		AddParamL(param);
		CleanupStack::Pop(param);
		}
	else
		{// delete the CHARSET parameter if it	was there
		TPtrC8 bufCharset(KVersitTokenCHARSET);
		DeleteParam(bufCharset);
		}

	/*
	 * General format is:
	 *
	 * <NAME>;<PARAMNAME=PARAMVALUE>:<VALUE>
	 *
	 */

	TBool foldParam=EFalse;
	if (extPlugIn)
		{
		foldParam = static_cast<MVersitPlugInExtension*>(extPlugIn)->FoldParam();
		}

	// Write out NAME part
	TInt outputLen = 0;
	if(foldParam)
		{
		HBufC8* name = Name().AllocLC();
		TPtr8 text(name->Des());
		aVersitParser->PlugIn()->WrapLine(aStream, outputLen, text);
		CleanupStack::PopAndDestroy(name);
		}
	else
		{
		aStream.WriteL(Name());
		outputLen = Name().Length();
		}

	// Write out PARAMNAME=PARAMVALUE part
	if	(iArrayOfParams)
		{
		TInt noOfParams = iArrayOfParams->Count();
		for (TInt ii = 0; ii < noOfParams; ii++)
			{
			aStream.WriteL(KVersitTokenSemiColon);

			if(foldParam)
				{
				outputLen++;			//1 for the ";"
				iArrayOfParams->At(ii)->ExternalizeL(aStream, outputLen, aVersitParser);
				}
			else
				{
				outputLen += iArrayOfParams->At(ii)->ExternalizeL(aStream)+1;
				}
			}
		}

	// Write out VALUE part
	aStream.WriteL(KVersitTokenColon);
	outputLen++;						//1 for the ":"
	
	iPropertyValue->ExternalizeL(aStream, encodingAndCharset, outputLen);
	aStream.WriteL(KVersitTokenCRLF);
	}

EXPORT_C CParserParam* CParserProperty::Param(const TDesC8& aParamName) const
// return first parameter found with specified name
/** Gets a pointer to the property parameter with the specified name. 

If the property has more than one parameter with the same name, the function 
returns the one nearest the start of the array, but there should never be 
more than one of each possible parameter.

@param aParamName The name of the parameter to search for. 
@return Pointer to a property parameter. NULL if the parameter name specified 
is not found in the array. */
	{
	if (iArrayOfParams)
		{
		TInt count = iArrayOfParams->Count();
		for (TInt ii = 0; ii < count; ii++)
			{
			if ((*iArrayOfParams)[ii]->Name().CompareF(aParamName) == 0)
				return ((*iArrayOfParams)[ii]);
			}
		}
	return NULL;
	}

EXPORT_C CArrayPtr<CParserParam>* CParserProperty::ParamArray()const
/** Returns a pointer to the array of property parameters.

@return  Pointer to array of property parameters. NULL if the property is not having any parameters.*/
	{
	//if the array doesn't exist, return NULL.
	if(!iArrayOfParams)
		{
		return NULL;
		}
	return iArrayOfParams;
	}

EXPORT_C void CParserProperty::AddParamL(CParserParam* aParam)
/** Adds a property parameter to the property.

Any existing parameter with the same name is replaced. The parameter is appended 
to the property's parameter array. If no property parameter array has been 
allocated, the function will first allocate one.

@param aParam Pointer to a generic property parameter, consisting of a name 
and optionally a value, both specified as descriptors. The property takes 
ownership of the new parameter. */
	{
	TPtrC8 name=aParam->Name();
	DeleteParam(name);

	// if the array doesn't exist, create one here
	if (!iArrayOfParams)
		iArrayOfParams = new(ELeave)CArrayPtrFlat<CParserParam>(4);
	iArrayOfParams->AppendL(aParam);
	}

EXPORT_C void CParserProperty::DeleteParam(TDesC8& aParamName)
/** Deletes the specified property parameter from the property's array of parameters, 
if it exists in the array.

If the property has more than one parameter with the same name, the function 
deletes the one nearest the start of the array, but there should never be 
more than one of each possible parameter.

@param aParamName The name of the parameter to delete. */
	{
	if (iArrayOfParams)
		{
		const TInt count = iArrayOfParams->Count();
		for (TInt ii = 0; ii < count; ii++)
			{
			if ((*iArrayOfParams)[ii]->Name() == aParamName)
				{
				delete (*iArrayOfParams)[ii];
				iArrayOfParams->Delete(ii);
				break;
				}
			}
		}
	}

EXPORT_C void CParserProperty::SetNameL(const TDesC8& aName)
/** Sets the property name. 

If a name has already been set, this function will replace it.

This function allocates and constructs a new heap descriptor 
and initialises it using the content of aName, so can leave if insufficient 
memory is available.

@param aName The new property name. Property names are defined in vtoken.h. */
	{
	delete iPropertyName;
	iPropertyName = NULL;
	iPropertyName = aName.AllocL();
	}

EXPORT_C TBool CParserProperty::SupportsInterface(const TUid& /*aInterfaceUid*/) const
/** Tests whether the property value supports the specified interface.

This implementation returns EFalse.

@param aInterfaceUid Not used.
@return EFalse. */
	{
	return EFalse;
	}

EXPORT_C TPtrC8 CParserProperty::Name() const
/** Gets the property name. 

If no name has been set, the function returns an empty descriptor.

@return The property name. */
	{
	if (iPropertyName)
		return iPropertyName->Des();
	return KVersitTokenEmptyNarrow();
	}


EXPORT_C TBool CParserProperty::SaveBinaryValuesToFilesL(TInt aSizeThreshold,const TDesC& aPath,RFs& aFileSession)
/** If the property value is binary and is larger than the specified threshold, 
this function saves the binary data to a file and sets the property value 
to be a URI representing the file rather than the binary data iself.

If the property value is not binary, or if it is binary, but is smaller than 
the threshold, the function just returns EFalse.

The file is created in the folder identified by aPath, and is assigned a unique 
filename that consists of the property name and some random numbers.

The new URI property value is prefixed with file:// and contains the path 
and filename of the file created.

The function uses the file server session supplied, which is needed to create 
the files. It leaves if there is a problem creating any of the files.

This function is used by CVersitParser::SaveBinaryValuesToFilesL().

@param aSizeThreshold The threshold number of bytes for the binary data, above 
which a file is created and the binary data is stored in it.
@param aPath The path identifying the location in which the file is created. 
Must not be greater than 240 characters long or the function leaves with KErrArgument. 
If it doesn't end in a slash, then one is appended.
@param aFileSession The file server session used to create the files.
@return ETrue if the property value is binary and its size is greater than 
the threshold. EFalse if not. */
	{

	// The length of the path can be maximum 240 characters.
	__ASSERT_ALWAYS((aPath.Length() <= (KMaxFileName - KMaxGeneratedfilenamelen)),User::Leave(KErrArgument));

	if (iPropertyValue->Uid().iUid==KVCardPropertyAgentUid)
		{
		return ((static_cast<CParserPropertyValueAgent*>(iPropertyValue)->Value())->SaveBinaryValuesToFilesL(aSizeThreshold,aPath,aFileSession));
		}
	else if (iPropertyValue->Uid().iUid!=KVersitPropertyBinaryUid)
		{
		return EFalse;
		}
	
	const CBufSeg* bufseg_ptr = static_cast<CParserPropertyValueBinary*>(iPropertyValue)->Value();
	if (!bufseg_ptr || bufseg_ptr->Size() <= aSizeThreshold)
		{
		return EFalse; 
		}
	
	// read the binary data into a buffer so as to create and write to a file later on
	HBufC8* buffer = HBufC8::NewL(0);
	CleanupStack::PushL(TCleanupItem(DestroyHBufC, &buffer));
	ReadBinaryDataL(bufseg_ptr, &buffer);

	// create a file . The file name is generated with the function call and then the file is created
	TBuf<KMaxFileName + KFileProtocolStringLength> filename(aPath);
	RFile file;
	GenerateNameAndCreateFileL(aFileSession, iPropertyName->Des(), file, filename);

	// Write the binary value to the file.
	CleanupClosePushL(file);
	User::LeaveIfError(file.Write(*buffer));
	// pop and destroy the file and the buffer objects 
	CleanupStack::PopAndDestroy(2);

	// Convert the filename to a URI in accordance with RFC 1738: prefix "file://" and change all \s to /s.
	filename.Insert(0,KFileProtocol);
	const TInt fileNameLength = filename.Length();
	for (TInt i = KFileProtocolStringLength; i < fileNameLength; i++)
		{
		if (filename[i] == '\\')
			{
			filename[i] = '/';
			}
		}
	
	CDesCArrayFlat* desarray = new(ELeave) CDesCArrayFlat(filename.Length());
	CleanupStack::PushL(desarray);
	TPtrC ptrc(filename);
	desarray->InsertL(0,ptrc);
	CParserPropertyValueCDesCArray* value = new(ELeave) CParserPropertyValueCDesCArray(desarray);
	CleanupStack::Pop(desarray);
	CleanupStack::PushL(value);
	
	// Add a parameter identifying the property value as a URI.
	CParserParam* uripropertyparam = CParserParam::NewL(KValue,KUri);
	CleanupStack::PushL(uripropertyparam);
	AddParamL(uripropertyparam);
	CleanupStack::Pop(uripropertyparam);

	// Remove any other value parameters. The new parameter is the last one.
	const TInt count = iArrayOfParams->Count();
	for (TInt ii = 0; ii < count; ii++)
		{
		if ((*iArrayOfParams)[ii]->Name() == KValue)
			{
			if (ii == (count-1))
				{
				break;
				}
			else
				{
				delete (*iArrayOfParams)[ii];
				iArrayOfParams->Delete(ii);
				}
			}
		}
	delete iPropertyValue;
	iPropertyValue = value;
	CleanupStack::Pop(value);
	return ETrue;
	}

EXPORT_C TBool CParserProperty::LoadBinaryValuesFromFilesL(RFs& aFileSession)
/** If the property value is a URI, this function loads the file represented by 
the URI and sets the binary data it contains to be the property value, instead 
of the URI.

If the property value is not a URI, the function just returns EFalse.

The function also operates on any agents in a vCard that contain URI property 
values.

The function uses the file server session supplied, which is needed to open 
the files. It leaves if there is a problem opening any of the files.

This function is used by CVersitParser::LoadBinaryValuesFromFilesL().

@param aFileSession The file server session used to open the files.
@return ETrue if the property value is a URI, EFalse if not. */
	{

	if (iPropertyValue->Uid().iUid==KVCardPropertyAgentUid)
		{
		return ((static_cast<CParserPropertyValueAgent*>(iPropertyValue)->Value())->LoadBinaryValuesFromFilesL(aFileSession));
		}
	// Do nothing if the value is not a text value .
	else if (iPropertyValue->Uid().iUid!=KVersitPropertyCDesCArrayUid) 
		{
		return EFalse;
		}
	CDesCArray* text_ptr = static_cast<CParserPropertyValueCDesCArray*>(iPropertyValue)->Value();
	if (text_ptr == NULL)
		{
		return EFalse;
		}
	const TDesC& text = text_ptr->MdcaPoint(0);
	//The URI should contain file:// and some filename with path.. The min file name would be eg. c:\<1digitnumber>
	if ((text.Length() < (KFileProtocolStringLength + KMinFileNameLen)) || (text.Left(KFileProtocolStringLength).CompareF(KFileProtocol) != 0))
		{
		return EFalse;
		}
	// Do nothing if this is not a URI value representing a file.
	const CParserParam* param = Param(KValue);
	if (param == NULL ||  ( param->Value().CompareF(KUri)  != 0))
		{
		return EFalse;
		}

	// Convert the URI to a filename: remove "file://" and change /s to \s.
	TPtrC fptrc = text.Mid(KFileProtocolStringLength);
	if (fptrc.Length() > KMaxFileName)
		{
		fptrc.Set(fptrc.Ptr(),KMaxFileName);
		}
	TFileName filename(fptrc);
	const TInt filenameLength = filename.Length();
	for (TInt i = 0; i < filenameLength; i++)
		{
		if (filename[i] == '/')
			{
			filename[i] = '\\';
			}
		}
	// Load the file and put it into a CVersitBinaryValue.
	RFile file;
	User::LeaveIfError(file.Open(aFileSession,filename,EFileRead));
	CleanupClosePushL(file);
	TInt bytes;
	User::LeaveIfError(file.Size(bytes));

	HBufC8* buffer = HBufC8::NewL(bytes);
	CleanupStack::PushL(TCleanupItem(DestroyHBufC, &buffer));
	TPtr8 des(buffer->Des());
	User::LeaveIfError(file.Read(des));
	__ASSERT_ALWAYS((buffer->Length() >= bytes),User::Leave(KErrGeneral));
	CParserPropertyValueBinary* value = CParserPropertyValueBinary::NewL(buffer->Left(buffer->Length()));
	// pop and destroy the file and the buffer objects 
	CleanupStack::PopAndDestroy(2);

	//There should only ever be one VALUE parameter, and, in order have gotten to
	// this point in the code, it has to be value=URI
	// so we just delete all VALUE parameters (there should only be one, but this handles
	// corrupt data better, even though it is a bit slower).

	const TInt count = iArrayOfParams->Count();
	for (TInt ii = 0; ii < count; ii++)
		{
		if ((*iArrayOfParams)[ii]->Name() == KValue)
			{
			delete (*iArrayOfParams)[ii];
			iArrayOfParams->Delete(ii);
			}
		}

	// Replace the existing value with the new binary value.
	delete iPropertyValue;
	iPropertyValue = value;

	return ETrue;
	}

/*
 * Reads the binary data from the segmented buffer onto a heap descriptor
 *
 *
 * @param     "const CBufSeg* aBufseg_ptr" ( source )
 *             The segemnted buffer that contains the binary data 
 * @param     "HBufC8** aBuffer" ( destination )
 *             The pointer to the heap descriptor data location which
 *             eventually holds the data
 *           A ptr to ptr is passed since the reloaction is happening inside
 */
void CParserProperty::ReadBinaryDataL(const CBufSeg* aBufseg_ptr,HBufC8** aBuffer)
	{
	TInt buf_size = aBufseg_ptr->Size();
	TInt pos = 0;
	TInt len = 0;

	while ( pos < buf_size)
		{
		len =const_cast<CBufSeg*>(aBufseg_ptr)->Ptr(pos).Length();
		TUint8* temp = new (ELeave)  TUint8 [len];
		CleanupArrayDeletePushL(temp);
		aBufseg_ptr->Read(pos,temp,len);
		// copy the data to the buffer
		*aBuffer = (*aBuffer)->ReAllocL(pos+len);
		TPtr8 localptr = (*aBuffer)->Des();
		localptr.Append(const_cast<TUint8*>(temp),len);
		//continue the iteration
		pos += len;
		CleanupStack::PopAndDestroy(temp);
		}
	return;
	}

/*
 *  Creates a file. The name of the file is generated using the following parameters
 *	  <propertyname limited in length if required depending on the length of the path>
 *    <a randomly generated number  which is max upto 5 characters in len>
 *   The path is specified as an input to teh fuction which is used in creating a file.
 *
 * @param      "RFs& aFileSession"
 *              The file server session to be used
 * @param      "TPtr8 aPropertyName" 
 *             The property name that holds the binary data 
 * @param      "RFile& aFile"
 *             The handle to the file created
 * @param      "TDes& aFileName"
 *              The name of the file created. Initially this contains the path when it is passed to the function.
 *              Inside teh function the filename is appended
 */
void CParserProperty::GenerateNameAndCreateFileL(RFs& aFileSession, TPtr8 aPropertyName, RFile& aFile, TDes& aFileName)
	{
	// the total space available to store the path+ filename = 256 ( KMaxFileName)
	// aFileName.Length() gives the length of the path
	// hence available length for filename ( propertyname + 5 digit randomnumber ) = 256 - aFileName.Length()
	TInt available = (KMaxFileName - aFileName.Length()); 

	if (aFileName[aFileName.Length() - 1] != '\\')
		{
		aFileName.Append('\\');
		available--;
		}
	// gets the property name into a TPtrC
	TInt proplen = aPropertyName.Length();
	HBufC* propertyname = HBufC::NewL(proplen);
	CleanupStack::PushL(TCleanupItem(DestroyHBufC, &propertyname));
	TPtr propnameptr = propertyname->Des();
	propnameptr.Copy(aPropertyName);
	TPtrC name(propertyname->Left(proplen));

	// limit the property name so that u limit the path+filename = 256 characters
	if (name.Length() > available - KRandomnumberlen)
		{
		name.Set(name.Ptr(),available - KRandomnumberlen);
		}
	aFileName.Append(name);
	CleanupStack::PopAndDestroy(&propertyname);
	// Fill in any missing parts of the filename.
	TParse parse;
	User::LeaveIfError(aFileSession.Parse(aFileName,parse));
	aFileName = parse.FullName();

	TTime time;
	time.UniversalTime();
	TInt64 seed = time.Int64();
	TInt length = aFileName.Length();
	TInt n;

	FOREVER
		{
		n = Math::Rand(seed) >> 16;
		aFileName.AppendNum(n);
		TInt error = aFile.Create(aFileSession,aFileName,EFileWrite);
		if(error == KErrNone)
			{
			break;
			}
		else
			{
			__ASSERT_ALWAYS((error == KErrAlreadyExists),User::Leave(error));
			}

		aFileName.SetLength(length);
		}

	return;
	}

EXPORT_C void CParserProperty::Reserved()
	{}
