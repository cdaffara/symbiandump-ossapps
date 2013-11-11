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

#if !defined(__THELPERS_H__)
	#define __THELPERS_H__

// System includes
#include <e32std.h>
#include <badesca.h>
#include <vprop.h>

// Classes referenced
class CParserPropertyValueHBufC;
class CParserPropertyValueDateTime;
class CParserPropertyValueDate;
class CParserPropertyValueCDesCArray;
class CParserParam;

// Constants
const TUint KVersitTest2InternalUidForUTF8 = 0x100001;
const TUint KVersitTest2InternalUidForUTF7 = 0x100002;


//
// VTestHelpers (header)
//
class VTestHelpers
	{
//
public:											// STATIC HELPER FUNCTIONS
//
	static TInt									MakeRandomNumber(const TInt aLow, const TInt aHigh, TInt64& aSeed);
	static void									ConvertUnicodeToNarrowL(const TDesC16& aSource, TDes8& aDest, TUint aCharConvCharSet);
	static TBool								DoesParameterAlreadyExist(CArrayPtr<CParserParam>* aArrayOfParams, const TDesC8& aName);
	static CParserPropertyValueHBufC*			CreateTextPropertyValueL(const TDesC& aValue);
	static CParserPropertyValueDateTime*		CreateDateTimePropertyValueL(const TDateTime& aDateTime);
	static CParserPropertyValueDate*			CreateDatePropertyValueL(const TDateTime& aDateTime);
	
	// Use together...
	static void									CreateDescriptorArrayL(const TDesC& aItem, CDesCArray*& aArray, TBool aAddSpace);
	static CParserPropertyValueCDesCArray*		CreateArrayPropertyValueL(CDesCArray* aArray);
	};



//
// CComparitorParserProperty (header)
//
class CComparitorParserProperty : public CParserProperty
	{
//
public:								// ACCESS
//
	inline CComparitorParserProperty(CParserPropertyValue& aPropertyValue) :CParserProperty(aPropertyValue,NULL) {}
	inline CParserPropertyValue*	PropertyValue() const		{ return iPropertyValue;	}
	inline HBufC8*					PropertyName() const		{ return iPropertyName;		}
	inline CArrayPtr<CParserParam>*	ArrayOfParameters() const	{ return iArrayOfParams;	}
	inline TInt						ParameterCount() const		{ return (iArrayOfParams)? iArrayOfParams->Count() : 0;	}

//
public:								// EQUALITY CHECKS
//
	TBool							IsEqualL(const CComparitorParserProperty* aProp) const;

//
private:							// INTERNAL COMPARISON METHODS
//
	static TBool					PropCDesCArrayIsEqualL(CDesCArray* a1, CDesCArray* a2);
	static TBool					PropTimeZoneIsEqualL(TTimeIntervalSeconds a1, TTimeIntervalSeconds a2);
	static TBool					PropDaylightIsEqualL(CVersitDaylight* a1, CVersitDaylight* a2);
	static TBool					PropDateTimeIsEqualL(TVersitDateTime* a1, TVersitDateTime* a2);
	static TBool					PropDateIsEqualL(TVersitDateTime* a1, TVersitDateTime* a2);
	static TBool					PropMultiDateTimeIsEqualL(CArrayPtr<TVersitDateTime>* a1, CArrayPtr<TVersitDateTime>* a2);
	static TBool					PropIntIsEqualL(TInt a1, TInt a2);
	static TBool					PropHBufCIsEqualL(const TDesC& a1, const TDesC& a2);
	static TBool					DateEqual(const TDateTime& a1, const TDateTime& a2);
	static TBool					DateTimeIsEqual(const TDateTime& a1, const TDateTime& a2);
	};



//
// CParserPropertyWithoutCharsetTags (header)
//
class CParserPropertyWithoutCharsetTags : public CParserProperty
	{
	inline CParserPropertyWithoutCharsetTags(CParserPropertyValue& aPropertyValue) :CParserProperty(aPropertyValue,NULL) {}
//
public:								// ACCESS
//
	void							RemoveAllCharsetTagsFromParametersL();
	};

#endif
