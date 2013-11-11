// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#include "VTicketMaster.H"

#include <s32mem.h>
#include <e32std.h>

#include <vutil.h>
#include <vstaticutils.h>


EXPORT_C CParserVTicketmaster* CParserVTicketmaster::NewL()
	{
	CParserVTicketmaster* self=new(ELeave) CParserVTicketmaster();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

CParserVTicketmaster::CParserVTicketmaster()
: CVersitParser(ESupportsVersion)
	{
	iDefaultVersion = KVersitTokenVTicketmasterVersionNo;
	}

EXPORT_C void CParserVTicketmaster::ExternalizeL(RWriteStream& aStream)
	{
	if	(!iEntityName)
		SetEntityNameL(KVersitVarTokenVCONCERT);
	CVersitParser::ExternalizeL(aStream);
	}

EXPORT_C TUid CParserVTicketmaster::RecognizeToken(const TDesC8& aToken) const
// From CVersitParser
	{
	TUid uid = KNullUid;
	TChar firstChar(aToken.Ptr()[0]);
	firstChar=firstChar.GetUpperCase();
	switch (firstChar)
		{
	case 'V':
		if (!aToken.CompareF(KVersitTokenVERSION))
			uid.iUid = KVersitPropertyHBufCUid;
		break;
	default:
		break;
		}
	if (uid == KNullUid)
		return CVersitParser::RecognizeToken(aToken);		
	return uid;
	}

EXPORT_C TUid CParserVTicketmasterEntity::RecognizeToken(const TDesC8& aToken) const
// From CVersitParser
	{
	TUid uid = KNullUid;
	TChar firstChar(aToken.Ptr()[0]);
	firstChar=firstChar.GetUpperCase();
	switch (firstChar)
		{
	case 'A':
		if (!aToken.CompareF(KVersitTokenARTIST))
			uid.iUid = KVersitPropertyHBufCUid;
		break;
	case 'B':
		if (!aToken.CompareF(KVersitTokenBOOKINGFEE))
			uid.iUid = KVTicketmasterPriceUid;
		break;
	case 'C':
		if (!aToken.CompareF(KVersitTokenCONCERTDATE))
			uid.iUid = KVersitPropertyDateTimeUid;
		break;
	case 'E':
		if (!aToken.CompareF(KVersitTokenEVENTCODE))
			uid.iUid = KVTicketmasterEventCodeUid;
		break;
	case 'N':
		if (!aToken.CompareF(KVersitTokenNUMBEROFTICKETS))
			uid.iUid = KVersitPropertyIntUid;
		break;
	case 'O':
		if (!aToken.CompareF(KVersitTokenORGANISATION))
			uid.iUid = KVersitPropertyHBufCUid;
		else if (!aToken.CompareF(KVersitTokenOTHERRELATEDSHOWS))
			uid.iUid = KVTicketmasterOtherRelatedShowsId;
		break;
	case 'P':
		if (!aToken.CompareF(KVersitTokenPRICELEVEL))
			uid.iUid = KVersitPropertyIntUid;
		break;
	case 'S':
		if (!aToken.CompareF(KVersitTokenSEATNUMBERS))
			uid.iUid = KVTicketmasterSeatNumUid;
		else if (!aToken.CompareF(KVersitTokenSUPPORTINGACT))
			uid.iUid = KVersitPropertyHBufCUid;
		break;
	case 'T':
		if (!aToken.CompareF(KVersitTICKETTERMINAL))
			uid.iUid = KVersitPropertyIntUid;
		else if (!aToken.CompareF(KVersitTokenTICKETPRICE))
			uid.iUid = KVTicketmasterPriceUid;
		break;
	case 'V':
		if (!aToken.CompareF(KVersitTokenVENUE))
			uid.iUid = KVersitPropertyHBufCUid;
		break;
	default:
		break;
		}
	if (uid == KNullUid)
		return CVersitParser::RecognizeToken(aToken);		
	return uid;
	}

EXPORT_C TInt CParserVTicketmaster::RecognizeEntityName() const
// From CVersitParser
	{
	if (iCurrentProperty && iCurrentProperty->Value())
		{
		TPtrC entityName=STATIC_CAST(CParserPropertyValueHBufC*,iCurrentProperty->Value())->Value();
		if (entityName==KVersitVarTokenVCONCERT)
			return KVTicketmasterEntityUidVConcert;
		else if (entityName==KVersitVarTokenVMUSICAL)
			return KVTicketmasterEntityUidVMusical;
		}
	return 0;
	}

EXPORT_C CVersitParser* CParserVTicketmaster::MakeEntityL(TInt aEntityUid,HBufC* aEntityName)
// From CVersitParser
	{
	CVersitParser* newEntity = NULL;
	CleanupStack::PushL(aEntityName);
	switch (aEntityUid)
		{
		case KVTicketmasterEntityUidVConcert:
		case KVTicketmasterEntityUidVMusical:
			newEntity=CParserVTicketmasterEntity::NewL();
			break;
		default: 
			//if entity is not concert nor musical,
			//then create base class entity with no version
			newEntity = new(ELeave) CVersitParser(ENoVersionProperty);
			CleanupStack::PushL(newEntity);
			newEntity->ConstructL();
			CleanupStack::Pop(newEntity);
		}
	CleanupStack::Pop(aEntityName);
	CleanupStack::PushL(newEntity);		//Cannot leave as we have just popped an item and the stack never shrinks
	newEntity->InternalizeL(aEntityName, iLineReader);
	CleanupStack::Pop(newEntity);
	return newEntity;
	}

CParserPropertyValue* CParserVTicketmasterEntity::MakePropertyValueL(const TUid& aPropertyUid,HBufC16*& aValue)
// From CVersitParser
	{
	switch (aPropertyUid.iUid)
		{
		case KVTicketmasterPriceUid: //won't be recognized except by subclass
			return MakePropertyValuePriceL(aValue->Des());
		case KVTicketmasterEventCodeUid:
			return MakePropertyValueEventCodeL(aValue->Des());
		case KVTicketmasterSeatNumUid:
			return MakePropertyValueSeatNumL(aValue->Des());
		case KVTicketmasterOtherRelatedShowsId:
			{
			CParserPropertyValue* value = NULL;
			CPtrCArray* array=MakePropertyValueOtherRelatedShowsL(aValue->Des());
			CleanupStack::PushL(array);
			value = new(ELeave) CParserPropertyValueOtherRelatedShows(array);
			aValue = NULL;
			CleanupStack::Pop(array);
			return value;
			}
			
		default:
			break;
		};
	return CVersitParser::MakePropertyValueL(aPropertyUid,aValue);
	}

EXPORT_C CParserPropertyValue* CParserVTicketmasterEntity::MakePropertyValuePriceL(TPtr16 aPriceValue)
	{
	const TInt stringLength = aPriceValue.Length();
	TInt valueInPennys = 0;
	TInt valueInPounds = 0;
	TChar currencySymbol = 0xa4; //£ symbol

	TInt i=0;
	//check if there is any white space before hitting £ symbol
	//we have assume the case that all price values are non-corrupted
	while (VersitUtils::IsWhiteSpace(aPriceValue[i]) && i<stringLength)
		++i;

	if (i==stringLength) //when there is nothing after £ symbol
		{
		CParserPropertyValue* value = new(ELeave) CParserPropertyValuePrice(currencySymbol, '.', valueInPounds, valueInPennys);
		return value;
		}

	const TInt currencySymbolPosition = i; //remembering where £ symbol is

	TInt actualValue = aPriceValue[++i];

	//work out the pounds part of the value
	while ((VersitUtils::IsWhiteSpace(actualValue) || (actualValue>='0' && actualValue<='9')) && i<stringLength) 
		{
		if (actualValue>='0' && actualValue<='9')
			valueInPounds = 10*valueInPounds + actualValue -'0';
		if ((++i)<stringLength) //this condition will kept in used when there is no penny or decimal point of the price value
			actualValue = aPriceValue[i];
		}
	CParserPropertyValue* value = NULL;

	if (i==stringLength) //when there are no pennys & no decimal point
		value = new(ELeave) CParserPropertyValuePrice(aPriceValue[currencySymbolPosition], '.', valueInPounds, valueInPennys);
	else if (i<stringLength) //when there is a decimal point
		{
		const TInt decimalPos = i;
		actualValue = aPriceValue[++i];

		//work out the pennys part of the value
		while (i<stringLength) 
			{
			if (actualValue>='0' && actualValue<='9')
				valueInPennys = 10*valueInPennys + actualValue -'0';
			if ((++i)<stringLength)
				actualValue = aPriceValue[i];
			}
		value = new(ELeave) CParserPropertyValuePrice(aPriceValue[currencySymbolPosition], aPriceValue[decimalPos], valueInPounds, valueInPennys);
		}
	return value;
	}

EXPORT_C CParserPropertyValue* CParserVTicketmasterEntity::MakePropertyValueEventCodeL(TPtr16 aEventCodeValue)
	{
	TInt i = 0;
	//assuming no white space before the number begins
	while (aEventCodeValue[i] != KHyphen)
		++i;

	TInt firstCode = 0;
	TInt firstCodeLength = i;
	TPtr16 num(&aEventCodeValue[0], firstCodeLength, firstCodeLength);
	Val(num, firstCode);

	TInt positionOfSecondCode = ++i;
	while (aEventCodeValue[i] != KHyphen)
		++i;
	
	TInt secondCodeLength = i - positionOfSecondCode;
	TInt secondCode = 0;
	num.Set(&aEventCodeValue[positionOfSecondCode], secondCodeLength, secondCodeLength);
	Val(num, secondCode);

	++i;
	TInt thirdCodeLength = aEventCodeValue.Length() - i;
	TInt thirdCode = 0;
	num.Set(&aEventCodeValue[i], thirdCodeLength, thirdCodeLength);
	Val(num, thirdCode);
	
	CParserPropertyValue* value = new(ELeave) CParserPropertyValueEventCode(firstCode, firstCodeLength,
																			secondCode, secondCodeLength,
																			thirdCode, thirdCodeLength);
	return value;
	}

//
// Data type that will pass into this function looks like
// A7,BA4,AG8,B4
//
CParserPropertyValue* CParserVTicketmasterEntity::MakePropertyValueSeatNumL(TPtr16 aSeatNum)
	{
	TInt i = 0;
	TInt stringLength = aSeatNum.Length();
	TInt seatLength = 0; //the length of the seat, e.g seat A1->seatLength=2, seat AB1->seatLength=3
	TSeatNum seat;
	RArray<TSeatNum> seatArray;
	TPtr16 specificSeat(&aSeatNum[0], seatLength, seatLength); //initialization of specificSeat

	while (i<stringLength) //scan through the string until reaching the last one
		{
		TInt firstCharacterOfSeat = i-seatLength; //position of the first character
		
		if (aSeatNum[i] == KComma)
			{
			specificSeat.Set(&aSeatNum[firstCharacterOfSeat], seatLength, seatLength);
			CreateSeatsL(seat, specificSeat);

			seatLength = 0;

			seatArray.InsertInOrder(seat,TLinearOrder<TSeatNum>(TSeatNum::CompareSeats));;
			}
		else if (aSeatNum[i] == KHyphen) //if it is A1-7,A9 OR just A1-7
			{
			specificSeat.Set(&aSeatNum[firstCharacterOfSeat], seatLength, seatLength);
			CreateSeatsL(seat, specificSeat);

			seatLength = 0;

			++i;
			while (i+1<=stringLength && aSeatNum[i]!=KComma)
				{
				++seatLength;
				++i;
				}

			TInt temp;
			TPtr16 num(&aSeatNum[i-seatLength], seatLength, seatLength);
			Val(num, temp);
			seat.iNextSeats = temp - seat.iSeatNum;

			seatArray.InsertInOrder(seat,TLinearOrder<TSeatNum>(TSeatNum::CompareSeats));

			seatLength = 0;

			}
		else if (i+1>=stringLength) //if it is the last one in the string
			{
			specificSeat.Set(&aSeatNum[firstCharacterOfSeat], seatLength+1, seatLength+1);
			CreateSeatsL(seat, specificSeat);

			seatArray.InsertInOrder(seat,TLinearOrder<TSeatNum>(TSeatNum::CompareSeats));;
			}
		else
			++seatLength;

		++i;
		}

	//buf is for testing purpose - to check what is stored in the array
	TBuf8<200> buf;
	TInt arrayLength = seatArray.Count();
	for (TInt j=0; j<arrayLength; j++)
		{
		buf.Append(seatArray[j].iRow);
		buf.Append(seatArray[j].iSeatNum);
		buf.Append(seatArray[j].iNextSeats);
		}

	TInt m=0;
	//detecting those that should be in the same range
	while (m+1<seatArray.Count())
		{
		if (seatArray[m].iRow == seatArray[m+1].iRow)
			{
			if (seatArray[m+1].iSeatNum - seatArray[m].iSeatNum - seatArray[m].iNextSeats== 1)
				{
				seatArray[m].iNextSeats = seatArray[m].iNextSeats + seatArray[m+1].iNextSeats + 1;
				seatArray.Remove(m+1);
				}
			else if(seatArray[m+1].iSeatNum - seatArray[m].iSeatNum - seatArray[m].iNextSeats < 1)
				{
				seatArray.Remove(m+1);
				}
			else
				m++;
			}
		else
			m++;
		}

	//The buf is for testing purpose, to see what is stored after sorting
	for (TInt k=0; k<seatArray.Count(); k++)
		{
		buf.Append(seatArray[k].iRow);
		buf.Append(seatArray[k].iSeatNum);
		buf.Append(seatArray[k].iNextSeats);
		}

	CParserPropertyValue* value = new(ELeave) CParserPropertyValueSeatNum(seatArray);
	return value;
	}


void CParserVTicketmasterEntity::CreateSeatsL(TSeatNum& aSeat, 
											 TPtr16& aSpecificSeat)
	{
	TInt i=0;
	TInt stringLength = aSpecificSeat.Length();

	while (!(aSpecificSeat[i]>='0' && aSpecificSeat[i]<='9'))
		++i;

	__ASSERT_ALWAYS(i<=2, User::Leave(KErrGeneral));

	TPtr16 num(&aSpecificSeat[0], i, i);
	aSeat.iRow = num;

	num.Set(&aSpecificSeat[i], stringLength-i, stringLength-i);
	Val(num, aSeat.iSeatNum);

	aSeat.iNextSeats = 0;

	}


TInt TSeatNum::CompareSeats(const TSeatNum& aFirst, const TSeatNum& aSecond)
	{
	if (aFirst.iRow.Length() == aSecond.iRow.Length())
		{
		if (aFirst.iRow.Length() == 1)
			{
			if (aFirst.iRow == aSecond.iRow)
				return (aFirst.iSeatNum - aSecond.iSeatNum);
			else 
				return (aFirst.iRow[0] - aSecond.iRow[0]);
			}
		else
			{
			if (aFirst.iRow[0] == aSecond.iRow[0])
				{
				if (aFirst.iRow[1]==aSecond.iRow[1])
					return (aFirst.iSeatNum - aSecond.iSeatNum);
				else
					return (aFirst.iRow[1] - aSecond.iRow[1]);
				}
			else
				return (aFirst.iRow[0] - aSecond.iRow[0]);
			}
		}
	else
		return (aFirst.iRow.Length() - aSecond.iRow.Length());
	}



//
// input data may look like STEPS;KYLIE;SAM
//
CPtrCArray* CParserVTicketmasterEntity::MakePropertyValueOtherRelatedShowsL(TPtr16 aShow)
	{
	TInt stringLength = aShow.Length();
	CPtrCArray* arrayOfValues = new(ELeave)CPtrC16Array(5);

	TInt i = 0;
	TInt showLength = 0; //length of the string of the show, say STEPS->showLength=5
	while(i<stringLength)
		{
		if (aShow[i] == KSemiColon)
			{
			TPtr16 show(&aShow[i-showLength], showLength, showLength);
			arrayOfValues->AppendL(show);
			showLength = 0;
			}
		else if (i+1>=stringLength)
			{
			TPtr16 show(&aShow[i-showLength], showLength+1, showLength+1);
			arrayOfValues->AppendL(show);
			}
		else
			++showLength;
		++i;
		}

	return arrayOfValues;
	}

//*************************************************************************************
//
// CParserPropertyValuePrice
//
//*************************************************************************************
EXPORT_C CParserPropertyValuePrice::CParserPropertyValuePrice(TChar aCurrencySymbol, TChar aDecimalPoint, TInt aPounds, TInt aPennys)
:CParserPropertyValue(TUid::Uid(KVTicketmasterPriceUid))
,iCurrencySymbol(aCurrencySymbol)
,iDecimalPoint(aDecimalPoint)
,iPounds(aPounds)
,iPennys(aPennys)
	{}

EXPORT_C CParserPropertyValuePrice::~CParserPropertyValuePrice()
	{
	}

EXPORT_C void CParserPropertyValuePrice::ExternalizeL(RWriteStream& aStream, const Versit::TEncodingAndCharset& /*aEncodingCharset*/
													  ,TInt /*aLengthOutput*/)
	{
	TBuf8<KVersitDefaultBufferSize> buf;
	buf.Append(iCurrencySymbol);
	buf.AppendNum(iPounds);
	buf.Append(iDecimalPoint);
	buf.AppendNumFixedWidth(iPennys, EDecimal, 2);
	aStream.WriteL(buf);
	}


//*************************************************************************************
//
// CParserPropertyValueEventCode
//
//*************************************************************************************

CParserPropertyValueEventCode::CParserPropertyValueEventCode(TInt aFirstCode, TInt aFirstCodeLength,
															 TInt aSecondCode, TInt aSecondCodeLength,
															 TInt aThirdCode, TInt aThirdCodeLength)
:CParserPropertyValue(TUid::Uid(KVTicketmasterEventCodeUid))
,iFirstCode(aFirstCode)
,iSecondCode(aSecondCode)
,iThirdCode(aThirdCode)
,iFirstCodeLength(aFirstCodeLength)
,iSecondCodeLength(aSecondCodeLength)
,iThirdCodeLength(aThirdCodeLength)
	{}


void CParserPropertyValueEventCode::ExternalizeL(RWriteStream& aStream, const Versit::TEncodingAndCharset& aEncodingCharset
													  ,TInt /*aLengthOutput*/)
	{

	__ASSERT_ALWAYS(aEncodingCharset.iEncoding!=Versit::EBase64Encoding, TMPanic(EUnexpectedBase64));

	TBuf8<KVersitDefaultBufferSize> buf;
	buf.AppendNumFixedWidth(iFirstCode, EDecimal, iFirstCodeLength);
	buf.Append('-');
	buf.AppendNumFixedWidth(iSecondCode, EDecimal, iSecondCodeLength);
	buf.Append('-');
	buf.AppendNumFixedWidth(iThirdCode, EDecimal, iThirdCodeLength);
	aStream.WriteL(buf);
	}



//*************************************************************************************
//
// CParserPropertyValueSeatNumL
//
//*************************************************************************************
CParserPropertyValueSeatNum::CParserPropertyValueSeatNum(RArray<TSeatNum>& aSeatArray)
:CParserPropertyValue(TUid::Uid(KVTicketmasterSeatNumUid))
,iSeatArray(aSeatArray)
	{}

CParserPropertyValueSeatNum::~CParserPropertyValueSeatNum()
	{
	}

void CParserPropertyValueSeatNum::ExternalizeL(RWriteStream& aStream, const Versit::TEncodingAndCharset& /*aEncodingCharset*/
													  ,TInt /*aLengthOutput*/)
	{
	TBuf8<KVersitDefaultBufferSize> buf;

	iArrayLength = iSeatArray.Count();
	for (TInt i=0; i<iArrayLength; i++)
		{
		if (iSeatArray[i].iNextSeats > 0)
			{
			buf.Append(iSeatArray[i].iRow);
			buf.AppendNum(iSeatArray[i].iSeatNum);
			buf.Append('-');
			buf.AppendNum(iSeatArray[i].iNextSeats + iSeatArray[i].iSeatNum);
			}
		else
			{
			buf.Append(iSeatArray[i].iRow);
			buf.AppendNum(iSeatArray[i].iSeatNum);
			}

		if (i+1<iArrayLength)
			buf.Append(KComma);

		aStream.WriteL(buf);
		buf.Zero();
			
		}
	}



//*************************************************************************************
//
// CParserPropertyValueOtherRelatedShows
//
//*************************************************************************************

CParserPropertyValueOtherRelatedShows::CParserPropertyValueOtherRelatedShows(CPtrCArray* aShowArray)
:CParserPropertyValue(TUid::Uid(KVTicketmasterOtherRelatedShowsId))
,iShowArray(aShowArray)
	{}

CParserPropertyValueOtherRelatedShows::~CParserPropertyValueOtherRelatedShows()
	{
	delete iShowArray;
	}

void CParserPropertyValueOtherRelatedShows::ExternalizeL(RWriteStream& aStream, const Versit::TEncodingAndCharset& /*aEncodingCharset*/
													  ,TInt /*aLengthOutput*/)
	{
	TBuf8<KVersitDefaultBufferSize> buf;
	TInt arraySize = iShowArray->Count();
	for (TInt j=0; j<arraySize; j++)
		{
		buf.Append(iShowArray->At(j));

		if (j+1<arraySize)
		buf.Append(KSemiColon);

		aStream.WriteL(buf);
		buf.Zero();
		}
	}


//*************************************************************************************
//
//CParserTicketmasterEntity
//
//*************************************************************************************

EXPORT_C CParserVTicketmasterEntity* CParserVTicketmasterEntity::NewL()
	{
	CParserVTicketmasterEntity* self=new(ELeave) CParserVTicketmasterEntity();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

// Constructs a vTicketmaster sub-entity parser.
CParserVTicketmasterEntity::CParserVTicketmasterEntity()
: CVersitParser(EFalse)
	{
	}

//The destructor is empty, and is present only to cause the virtual
//function table to be defined in a unique module.
CParserVTicketmasterEntity::~CParserVTicketmasterEntity()
	{
	}


