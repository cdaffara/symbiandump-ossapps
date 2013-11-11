// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PBAPAPPHEADER_H
#define PBAPAPPHEADER_H

#include <e32base.h>
#include <cntdb.h>
#include "pbaprequest.h"
#include "pbapoperation.h"


// PBAP obex type header values
_LIT8(KObexTypeBTvCard,				"x-bt/vcard");
_LIT8(KObexTypeBTvCardPhoneBook,	"x-bt/phonebook");
_LIT8(KObexTypeBTvCardListing,		"x-bt/vcard-listing");


class CPbapAppHeader : public CBase
	{
public:
	enum TApplicationHeaderTagId
		{
		EOrder			=0x1,
		ESearchValue	=0x2,
		ESearchAttribute=0x3,
		EMaxListCount	=0x4,
		EListStartOffset=0x5,
		EFilter			=0x6,
		EFormat			=0x7,
		EPhonebookSize	=0x8,	// PSE sends
		ENewMissedCalls	=0x9,	// PSE sends
		EBeginningOfUnspecifiedTags = 0xA,
		};

public:
	static CPbapAppHeader* NewL();
	~CPbapAppHeader();

	void ParseL(const TDesC8& aObexApplicationHeader);
	TPbapOperation DeterminePBAPOperationL(const TDesC8& aTypeHeader);

	inline TPbapOperation Operation() const;
	inline TBool TagPresent(TApplicationHeaderTagId aTag) const;
	inline TUint64 Filter() const;
	inline SymbianPBAP::TOrder Order() const;
	inline TInt MaxListCount() const;
	inline TVCardVersion VCardVersion() const;
	inline TInt ListStartOffset() const;
	inline const RBuf& SearchValue() const;
	inline SymbianPBAP::TSearchAttribute SearchAttribute() const;
	inline TBool IsAbsolutePathOp() const;
	
	void Reset();

private:
	CPbapAppHeader();

	SymbianPBAP::TOrder ConvertToOrder(TUint8 aValue) const;
	SymbianPBAP::TSearchAttribute ConvertToSearchAttribute(TUint8 aValue) const;
	TVCardVersion ConvertToVCardVersionL(TUint8 aValue) const;
	TUint64 ConvertToFilter(const TUint8* aPtr) const;
	
private:
	TFixedArray<TBool, EBeginningOfUnspecifiedTags>	iTagsPresent;
	SymbianPBAP::TOrder				iOrder;
	TUint16							iMaxListCount;
	TUint16							iListStartOffset;
	TUint64							iAttributeMask;
	TVCardVersion					iVCardVersion;
	RBuf							iSearchValue;
	SymbianPBAP::TSearchAttribute	iSearchAttribute;
	TPbapOperation					iOperation;
	TBool							iIsAbsolutePathOp;
	};


//
// inline functions	
//	
inline TPbapOperation CPbapAppHeader::Operation() const
	{
	return iOperation;
	}
	
inline TUint64 CPbapAppHeader::Filter() const
	{
	return iAttributeMask;
	}
	
inline SymbianPBAP::TOrder CPbapAppHeader::Order() const
	{
	return iOrder;
	}
	
inline TInt CPbapAppHeader::MaxListCount() const
	{
	return iMaxListCount;
	}

inline TVCardVersion CPbapAppHeader::VCardVersion() const
	{
	return iVCardVersion;
	}

inline TInt CPbapAppHeader::ListStartOffset() const
	{
	return iListStartOffset;
	}

inline const RBuf& CPbapAppHeader::SearchValue() const
	{
	return iSearchValue;
	}
	
inline SymbianPBAP::TSearchAttribute CPbapAppHeader::SearchAttribute() const
	{
	return iSearchAttribute;
	}
	
inline TBool CPbapAppHeader::IsAbsolutePathOp() const
	{
	return iIsAbsolutePathOp;
	}
		
#endif //PBAPAPPHEADER_H
