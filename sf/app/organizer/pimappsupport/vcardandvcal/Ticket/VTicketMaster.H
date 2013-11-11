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
 
#if !defined(__VFOOT_H__)
#define __VCARD_H__

#if !defined(__VERSIT_H__)
#include "VERSIT.H"
#endif

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "versit_internal.h"
#endif


_LIT(KVersitTokenVTicketmasterVersionNo, "1.0");
_LIT(KVersitVarTokenTICKETMASTER,	"TICKETMASTER");
_LIT8(KVersitTokenEVENTTYPE,		"EVENT-TYPE");
_LIT8(KVersitTokenARTIST,			"ARTIST");
_LIT8(KVersitTICKETTERMINAL,		"TICKET-TERMINAL");
_LIT8(KVersitTokenVENUE,			"VENUE");
_LIT8(KVersitTokenNUMBEROFTICKETS,	"NUMBER-OF-TICKETS");
_LIT8(KVersitTokenSUPPORTINGACT,	"SUPPORTING-ACT");
_LIT8(KVersitTokenORGANISATION,		"ORGANISATION");
_LIT8(KVersitTokenCONCERTDATE,		"CONCERT-DATE");
_LIT8(KVersitTokenPRICELEVEL,		"PRICE-LEVEL");
_LIT8(KVersitTokenTICKETPRICE,		"TICKET-PRICE");
_LIT8(KVersitTokenBOOKINGFEE,		"BOOKING-FEE");
_LIT8(KVersitTokenEVENTCODE,		"EVENT-CODE");
_LIT8(KVersitTokenSEATNUMBERS,		"SEAT-NUMBERS");
_LIT8(KVersitTokenOTHERRELATEDSHOWS,"OTHER-RELATED-SHOWS");

_LIT(KVersitVarTokenVCONCERT,		"VCONCERT");
_LIT(KVersitVarTokenVMUSICAL,		"VMUSICAL");

//UID 270488168 - 270488183 have been reserved for Demo DLL to use
#define KVTicketmasterEntityUidVConcert		270488168
#define KVTicketmasterEntityUidVMusical		270488169
#define KVTicketmasterPriceUid				270488170
#define KVTicketmasterEventCodeUid			270488171
#define KVTicketmasterSeatNumUid			270488172
#define KVTicketmasterOtherRelatedShowsId	270488173

#define KSpace		0x0020
#define KComma		0x002C
#define KSemiColon	0x003B
#define KHyphen		0x002D

#define FirstNumComesBeforeSecondNum -1
#define FirstNumComesAfterSecondNum   1


enum TTicketMasterPanic
	{
	EUnexpectedBase64=100,
	};

inline void TMPanic(TTicketMasterPanic aPanic)
	{
	Panic(REINTERPRET_CAST(TVersitParserPanic&, aPanic));
	}

class TSeatNum
	{
public:
	static TInt CompareSeats(const TSeatNum& aFirst, const TSeatNum& aSecond);
public:
	TBuf<2> iRow;
	TInt	iSeatNum;
	TInt	iNextSeats; //number of seats next to the original seat
	};

class CParserVTicketmaster : public CVersitParser
	{
public:
	IMPORT_C static CParserVTicketmaster* NewL();
public:
	//InternalizeL - we will call the base class function CVersitParser::InternaliseL
	IMPORT_C void ExternalizeL(RWriteStream& aStream);
protected: 
	IMPORT_C TUid RecognizeToken(const TDesC8& aToken) const;
	IMPORT_C TInt RecognizeEntityName() const;
	IMPORT_C CVersitParser* MakeEntityL(TInt aEntityUid, HBufC* aEntityName);
private:
	CParserVTicketmaster();
	};

class CParserVTicketmasterEntity : public CVersitParser
	{
public:
	IMPORT_C static CParserVTicketmasterEntity* NewL();
	~CParserVTicketmasterEntity();
	void CreateSeatsL(TSeatNum& aSeat, TPtr16& aSpecificSeat);
protected:
	CParserPropertyValue* MakePropertyValueL(const TUid& aPropertyUid,HBufC16*& aValue);
	IMPORT_C TUid RecognizeToken(const TDesC8& aToken) const;
	IMPORT_C CParserPropertyValue* MakePropertyValuePriceL(TPtr16 aPriceValue);
	IMPORT_C CParserPropertyValue* MakePropertyValueEventCodeL(TPtr16 aEventCodeValue);
	CParserPropertyValue* MakePropertyValueSeatNumL(TPtr16 aSeatNum);
	CPtrCArray* MakePropertyValueOtherRelatedShowsL(TPtr16 aShow);
private:
	CParserVTicketmasterEntity();
	};

class CParserPropertyValuePrice : public CParserPropertyValue
	{
public:
	IMPORT_C CParserPropertyValuePrice(TChar aCurrencySymbol, TChar aDecimalPoint, TInt aPounds, TInt aPennys);
	IMPORT_C ~CParserPropertyValuePrice();
public: // from CParserPropertyValue
	IMPORT_C void ExternalizeL(RWriteStream& /*aStream*/,const Versit::TEncodingAndCharset& /*aEncodingCharset*/,TInt /*aLengthOutput*/);
private:
	TChar	iCurrencySymbol;
	TChar	iDecimalPoint;
	TInt	iPounds;
	TInt	iPennys;
	};

class CParserPropertyValueEventCode : public CParserPropertyValue
	{
public:
	CParserPropertyValueEventCode(TInt aFirstCode, TInt aFirstCodeLength,
								  TInt aSecondCode, TInt aSecondCodeLength,
								  TInt aThirdCode, TInt aThirdCodeLength);
public:
	void ExternalizeL(RWriteStream& /*aStream*/,const Versit::TEncodingAndCharset& aEncodingCharset,TInt /*aLengthOutput*/);
private:
	TInt	iFirstCode;
	TInt	iSecondCode;
	TInt	iThirdCode;
	TInt	iFirstCodeLength;
	TInt	iSecondCodeLength;
	TInt	iThirdCodeLength;
	};

class CParserPropertyValueSeatNum : public CParserPropertyValue
	{
public:
	CParserPropertyValueSeatNum(RArray<TSeatNum>& aSeatArray);
	~CParserPropertyValueSeatNum();
public:
	void ExternalizeL(RWriteStream& /*aStream*/,const Versit::TEncodingAndCharset& /*aEncodingCharset*/,TInt /*aLengthOutput*/);
private:
	RArray<TSeatNum> iSeatArray;
	TInt	iArrayLength;
	};

class CParserPropertyValueOtherRelatedShows : public CParserPropertyValue
	{
public:
	CParserPropertyValueOtherRelatedShows(CPtrCArray* aShowArray);
	~CParserPropertyValueOtherRelatedShows();
public:
	void ExternalizeL(RWriteStream& /*aStream*/,const Versit::TEncodingAndCharset& /*aEncodingCharset*/,TInt /*aLengthOutput*/);
private:
	CPtrCArray* iShowArray;
	};



#endif
