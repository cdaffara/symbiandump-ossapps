// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "thelpers.h"

// System includes
#include <e32math.h>
#include <charconv.h>
#include <concnf.h>
#include <confndr.h>
#include <conlist.h>
#include <utf.h>
#include <vprop.h>

// Constants
//_LIT8(KReplacementChars, " ?");


//
// VTestHelpers (source)
//


//****************************************************************************************
TInt VTestHelpers::MakeRandomNumber(const TInt aLow, const TInt aHigh, TInt64& aSeed)
	{
	TReal initialRand = (Math::FRand(aSeed) * (aHigh - aLow));
	TInt32 rand;

	// Round to 0 decimal places, ie. the nearest whole numer
	Math::Round(initialRand, initialRand, 0);
	Math::Int(rand, initialRand);

	return (aLow + rand);
	}


//****************************************************************************************
/*void VTestHelpers::ConvertUnicodeToNarrowL(const TDesC16& aSource, TDes8& aDest, TUint aCharConvCharSet)
	{
	TInt stateValueDummyParameter = 0;

	switch(aCharConvCharSet)
		{
		// UTF-X
		case KVersitTest2InternalUidForUTF8:
		{
			User::LeaveIfError(CnvUtfConverter::ConvertFromUnicodeToUtf8(aDest, aSource));
			break;
		}
		case KVersitTest2InternalUidForUTF7:
		{
			User::LeaveIfError(CnvUtfConverter::ConvertFromUnicodeToUtf7(aDest, aSource, stateValueDummyParameter));
			break;
		}
		
		// ISO-X
		case KCharacterSetIdentifierIso88591:
		case KCharacterSetIdentifierIso88592:
		case KCharacterSetIdentifierIso88594:
		case KCharacterSetIdentifierIso88595:
		case KCharacterSetIdentifierIso88597:
		case KCharacterSetIdentifierIso88599:
		{
			RFs fileServerSession;
			CleanupClosePushL(fileServerSession);
			User::LeaveIfError(fileServerSession.Connect());
			CCnvCharacterSetConverter* characterSetConverter=CCnvCharacterSetConverter::NewLC();

			CCnvCharacterSetConverter::TAvailability avail=CCnvCharacterSetConverter::ENotAvailable;
			avail = characterSetConverter->PrepareToConvertToOrFromL(aCharConvCharSet, fileServerSession);
			if (avail==CCnvCharacterSetConverter::ENotAvailable)
				{
				CleanupStack::PopAndDestroy(2);
				return;
				}

			characterSetConverter->SetReplacementForUnconvertibleUnicodeCharactersL(KReplacementChars);
			characterSetConverter->ConvertFromUnicode(aDest, aSource);	

			CleanupStack::PopAndDestroy(2); // fileserver, charactersetconverter

			TBuf8<1000> buffer1 = aDest;
			TInt pos = 0;
			const TInt length = aDest.Length();
			for(TInt i=1; i<length; i+=2)
				{
				buffer1[pos] = buffer1[i];
				pos++;
				}
			aDest = buffer1;
			aDest.SetLength(length/2);
			break;
		}

		// Not handled by CharConv
		default:
		{
			const TInt length = aSource.Length();
			aDest.SetLength(length);
			for(TInt i = 0; i < length; i++)
				aDest[i] = (TUint8) aSource[i];
			break;
		}
		}

	}*/


//****************************************************************************************
TBool VTestHelpers::DoesParameterAlreadyExist(CArrayPtr<CParserParam>* aArrayOfParams, const TDesC8& aName)
	{
	for(TInt i=0; i<aArrayOfParams->Count(); i++)
		{
		if	(aArrayOfParams->At(i)->Name() == aName)
			return ETrue;
		}

	return EFalse;
	}


//
//
//


//****************************************************************************************
CParserPropertyValueHBufC* VTestHelpers::CreateTextPropertyValueL(const TDesC& aValue)
	{
	CParserPropertyValueHBufC* value = CParserPropertyValueHBufC::NewL(aValue);
	return value;
	}


//****************************************************************************************
CParserPropertyValueDateTime* VTestHelpers::CreateDateTimePropertyValueL(const TDateTime& aDateTime)
	{
	TVersitDateTime* versitDateTime = new(ELeave) TVersitDateTime(aDateTime, TVersitDateTime::EIsMachineLocal);
	CleanupStack::PushL(versitDateTime);
	CParserPropertyValueDateTime* value = new(ELeave) CParserPropertyValueDateTime(versitDateTime); // takes ownership
	CleanupStack::Pop(versitDateTime);
	return value;
	}


//****************************************************************************************
CParserPropertyValueDate* VTestHelpers::CreateDatePropertyValueL(const TDateTime& aDateTime)
	{
	TVersitDateTime* versitDateTime = new(ELeave) TVersitDateTime(aDateTime, TVersitDateTime::EIsMachineLocal);
	CleanupStack::PushL(versitDateTime);
	CParserPropertyValueDate* value = new(ELeave) CParserPropertyValueDate(versitDateTime); // takes ownership
	CleanupStack::Pop(versitDateTime);
	return value;
	}


//
//
//


//****************************************************************************************
void VTestHelpers::CreateDescriptorArrayL(const TDesC& aItem, CDesCArray*& aArray, TBool aAddSpace)
	{
	CleanupStack::PushL(aArray);
	if (!aAddSpace)
		aArray->AppendL(aItem);
	else
		{
		TBuf<64> string;
		string.Append(' ');
		string.Append(aItem);
		aArray->AppendL(string);
		}
	CleanupStack::Pop();
	}


//****************************************************************************************
CParserPropertyValueCDesCArray* VTestHelpers::CreateArrayPropertyValueL(CDesCArray* aArray)
	{
	CleanupStack::PushL(aArray);
	CParserPropertyValueCDesCArray* value = new(ELeave) CParserPropertyValueCDesCArray(aArray);
	CleanupStack::Pop();
	return value;
	}








//
// CComparitorParserProperty (source)
//


//****************************************************************************************
TBool CComparitorParserProperty::IsEqualL(const CComparitorParserProperty* aProp) const
	{
	if	(aProp->Name() != Name())
		return EFalse;
	if	(aProp->Value()->Uid() != Value()->Uid())
		return EFalse;
	if	(aProp->ParameterCount() != ParameterCount())
		return EFalse;


	switch(aProp->Value()->Uid().iUid)
		{
	case KVersitPropertyCDesCArrayUid:
		{
		return PropCDesCArrayIsEqualL(STATIC_CAST(const CParserPropertyValueCDesCArray*, aProp->Value())->Value(), STATIC_CAST(CParserPropertyValueCDesCArray*, Value())->Value());
		}
	case KVersitPropertyTimeZoneUid:
		{
		return PropTimeZoneIsEqualL(STATIC_CAST(const CParserPropertyValueTimeZone*, aProp->Value())->Value(), STATIC_CAST(CParserPropertyValueTimeZone*, Value())->Value());
		}
	case KVersitPropertyDaylightUid:
		{
		return PropDaylightIsEqualL(STATIC_CAST(const CParserPropertyValueDaylight*, aProp->Value())->Value(), STATIC_CAST(CParserPropertyValueDaylight*, Value())->Value());
		}
	case KVersitPropertyDateTimeUid:
		{
		return PropDateTimeIsEqualL(STATIC_CAST(const CParserPropertyValueDateTime*, aProp->Value())->Value(), STATIC_CAST(CParserPropertyValueDateTime*, Value())->Value());
		}
	case KVersitPropertyDateUid:
		{
		return PropDateIsEqualL(STATIC_CAST(const CParserPropertyValueDate*, aProp->Value())->Value(), STATIC_CAST(CParserPropertyValueDate*, Value())->Value());
		}
	case KVersitPropertyMultiDateTimeUid:
		{
		return PropMultiDateTimeIsEqualL(STATIC_CAST(const CParserPropertyValueMultiDateTime*, aProp->Value())->Value(), STATIC_CAST(CParserPropertyValueMultiDateTime*, Value())->Value());
		}
	case KVersitPropertyIntUid:
		{
		return PropIntIsEqualL(STATIC_CAST(const CParserPropertyValueInt*, aProp->Value())->Value(), STATIC_CAST(CParserPropertyValueInt*, Value())->Value());
		}
	default:
		{
		return PropHBufCIsEqualL(STATIC_CAST(const CParserPropertyValueHBufC*, aProp->Value())->Value(), STATIC_CAST(CParserPropertyValueHBufC*, Value())->Value());
		}
		}
	}


//
//
//


//****************************************************************************************
TBool CComparitorParserProperty::PropCDesCArrayIsEqualL(CDesCArray* a1, CDesCArray* a2)
	{
	TInt count = a1->MdcaCount();
	if	(count != a2->MdcaCount())
		return EFalse;

	for(TInt i=0; i<count; i++)
		{
		const TDesC& des = a1->MdcaPoint(i);
		if	(des.CompareC(a2->MdcaPoint(i)))
			return EFalse;
		}
	return ETrue;
	}


//****************************************************************************************
TBool CComparitorParserProperty::PropTimeZoneIsEqualL(TTimeIntervalSeconds a1, TTimeIntervalSeconds a2)
	{
	return (a1.Int() == a2.Int());
	}


//****************************************************************************************
TBool CComparitorParserProperty::PropDaylightIsEqualL(CVersitDaylight* a1, CVersitDaylight* a2)
	{
	if	(a1->iSavings != a2->iSavings)
		return EFalse;
	if	(a1->iOffset.Int() != a2->iOffset.Int())
		return EFalse;

	// Just compare the dateTime not the relative time
	if	(!DateTimeIsEqual(a1->iStartTime->iDateTime, a2->iStartTime->iDateTime))
		return EFalse;
	if	(!DateTimeIsEqual(a1->iEndTime->iDateTime, a2->iEndTime->iDateTime))
		return EFalse;

	if	(a1->iStandardDesignation->CompareC(*a2->iStandardDesignation))
		return EFalse;
	if	(a1->iDaylightDesignation->CompareC(*a2->iDaylightDesignation))
		return EFalse;
	return ETrue;
	}


//****************************************************************************************
TBool CComparitorParserProperty::PropDateTimeIsEqualL(TVersitDateTime* a1, TVersitDateTime* a2)
	{
	return DateTimeIsEqual(a1->iDateTime, a2->iDateTime);
	}


//****************************************************************************************
TBool CComparitorParserProperty::PropDateIsEqualL(TVersitDateTime* a1, TVersitDateTime* a2)
	{
	return DateEqual(a1->iDateTime, a2->iDateTime);
	}


//****************************************************************************************
TBool CComparitorParserProperty::PropMultiDateTimeIsEqualL(CArrayPtr<TVersitDateTime>* a1, CArrayPtr<TVersitDateTime>* a2)
	{
	if	((!a1 && a2) || (a1 && !a2))
		return EFalse;

	TInt count = a1->Count();
	if	(count != a2->Count())
		return EFalse;

	for(TInt i=0; i<count; i++)
		{
		if	(!PropDateTimeIsEqualL(a1->At(i), a2->At(i)))
			return EFalse;
		}
	return ETrue;
	}


//****************************************************************************************
TBool CComparitorParserProperty::PropIntIsEqualL(TInt a1, TInt a2)
	{
	return (a1 == a2);
	}


//****************************************************************************************
TBool CComparitorParserProperty::PropHBufCIsEqualL(const TDesC& a1, const TDesC& a2)
	{
	return !(a1.CompareC(a2));
	}


//****************************************************************************************
TBool CComparitorParserProperty::DateTimeIsEqual(const TDateTime& a1, const TDateTime& a2)
	{
	TTime time1(a1);
	TTime time2(a2);
	return (time1 == time2);
	}


//****************************************************************************************
TBool CComparitorParserProperty::DateEqual(const TDateTime& a1, const TDateTime& a2)
	{
	if	(a1.Year() == a2.Year() && a1.Month() == a2.Month() && a1.Day() == a2.Day())
		return ETrue;
	return EFalse;
	}



//
// CParserPropertyWithoutCharsetTags (source)
//

//****************************************************************************************
void CParserPropertyWithoutCharsetTags::RemoveAllCharsetTagsFromParametersL()
	{
	if	(!iArrayOfParams)
		return;

	TInt count = iArrayOfParams->Count();
	for(TInt i=count-1; i>=0; i--)
		{
		CParserParam* param = iArrayOfParams->At(i);
		HBufC8* name = param->Name().AllocLC();
		TPtr8 pName = name->Des();
		pName.UpperCase();
		pName.Trim();

		if	(pName == KVersitTokenCHARSET)
			{
			iArrayOfParams->Delete(i);
			delete param;
			}
		CleanupStack::PopAndDestroy(); // name
		}
	}

