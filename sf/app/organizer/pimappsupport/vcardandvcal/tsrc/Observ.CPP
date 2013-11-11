// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "Observ.H"

#include <e32test.h>
#include <vobserv.h>
#include <versit.h>
#include <vcal.h>
#include <vcard.h>

//class CAgnRptDef;
GLREF_D RTest TheTest;

class CTestVersitObserver : public CBase, public MVersitObserver, public MVersitPlugIn
	{
public:
	inline CTestVersitObserver(RFs& aFsSession) : iFs(aFsSession) {}
	void Test1L();
	void Test2L();
	void Test3L();
	void TestReccurrenceRulesL();
public:
//Pure virtual function from MVersitObserver
	void VersionSet(CVersitParser* aParser,const TDesC16& aVersion);
	void NewParser(CVersitParser* aParser);
//Pure virtual function from MVersitPlugIn
	TBool AddSpace();
	TBool DeleteAllSpaces();
	TBool NeedsBlankLine();
	void RemoveEscaping(TPtr16& aText);
	void AddEscaping(HBufC16*& aText);
	TBool WrapLine(RWriteStream& aStream,TInt& aCurrentLineLength,const TPtr8& aText);
	TBool EncodingType(Versit::TVersitEncoding& aEncoding,TBool aRequiresEncoding,Versit::TVersitEncoding aDefaultEncoding
																						,TUid aPropertyUid,TUint aPropertyCharset);
	const TDesC8& EncodingName(Versit::TVersitEncoding aEncoding);
private:
	enum TCountFlags
		{
		//Internalising
		ECountVersionSet=0x00000001,
		ECountNewParser=0x00000020,
		ECountAddSpace=0x00000400,
		ECountDeleteSpace=0x00008000,
		ECountNeedsBlank=0x00100000,
		ECountRemoveEscape=0x02000000,
		//Externalising
		ECountAddEscape=0x00000001,
		ECountWrapLine=0x00000100,
		ECountEncodeType=0x00010000,
		ECountEncodeName=0x01000000,
		};
private:
	RFs& iFs;
	CVersitParser* iExpectedParser;
	TBuf<16> iExpectedVersion;
	TBuf<16> iExpectedFirstValueRE;
	TBuf<16> iExpectedFirstValueAE;
	TBuf8<16> iExpectedFirstValueWL;
	TInt iExpectedFirstLineLength;
	TInt iExpectedDefaultEncoding;
	TUid iExpectedFirstUid;
	TInt iExpectedEncodingEN;
	TUint iCount;
	};

void CTestVersitObserver::VersionSet(CVersitParser* aParser,const TDesC16& aVersion)
	{
	if (iExpectedParser)
		TheTest(iExpectedParser==aParser);
	if (iExpectedVersion.Length()>0)
		TheTest(iExpectedVersion==aVersion);
	iCount+=ECountVersionSet;
	}

void CTestVersitObserver::NewParser(CVersitParser* aParser)
	{
	TheTest(iExpectedParser!=aParser);
	aParser->SetObserver(this);
	aParser->SetPlugIn(this);
	iCount+=ECountNewParser;
	}

TBool CTestVersitObserver::AddSpace()
	{
	iCount+=ECountAddSpace;
	return ETrue;
	}

TBool CTestVersitObserver::DeleteAllSpaces()
	{
	iCount+=ECountDeleteSpace;
	return ETrue;
	}

TBool CTestVersitObserver::NeedsBlankLine()
	{
	iCount+=ECountNeedsBlank;
	return ETrue;
	}

void CTestVersitObserver::RemoveEscaping(TPtr16& aText)
	{
	if (iExpectedFirstValueRE.Length()>0)
		{
		TheTest(iExpectedFirstValueRE==aText);
		iExpectedFirstValueRE.Zero();
		}
	iCount+=ECountRemoveEscape;
	}

void CTestVersitObserver::AddEscaping(HBufC16*& aText)
	{
	if (iExpectedFirstValueAE.Length()>0)
		{
		TheTest(iExpectedFirstValueAE==*aText);
		iExpectedFirstValueAE.Zero();
		}
	iCount+=ECountAddEscape;
	}

TBool CTestVersitObserver::WrapLine(RWriteStream& /*aStream*/,TInt& aCurrentLineLength,const TPtr8& aText)
	{
	if (iExpectedFirstValueWL.Length()>0)
		{
		TheTest(iExpectedFirstValueWL==aText);
		iExpectedFirstValueWL.Zero();
		}
	if (iExpectedFirstLineLength>0)
		{
		TheTest(iExpectedFirstLineLength==aCurrentLineLength);
		iExpectedFirstLineLength=0;
		}
	iCount+=ECountWrapLine;
	return EFalse;
	}

TBool CTestVersitObserver::EncodingType(Versit::TVersitEncoding& /*aEncoding*/,TBool /*aRequiresEncoding*/
												,Versit::TVersitEncoding aDefaultEncoding,TUid aPropertyUid,TUint /*aPropertyCharset*/)
	{
	TheTest(iExpectedDefaultEncoding==aDefaultEncoding);
	if (iExpectedFirstUid.iUid!=0)
		{
		TheTest(iExpectedFirstUid==aPropertyUid);
		iExpectedFirstUid.iUid=0;
		}
	iCount+=ECountEncodeType;
	return EFalse;
	}

const TDesC8& CTestVersitObserver::EncodingName(Versit::TVersitEncoding aEncoding)
	{
	TheTest(iExpectedEncodingEN==aEncoding);
	iCount+=ECountEncodeName;
	return KNullDesC8;
	}

void CTestVersitObserver::Test1L()
	{
	RFile file;
	_LIT(KFileI,"Z:\\TestFiles\\IVCal.VCS");
	_LIT(KFileP,"C:\\TestFiles\\PVCal.VCS");
	_LIT(KVersion,"1.0");
	_LIT8(KVersion8,"1.0");
	_LIT(KFirstValue,"VCALENDAR");
	CParserVCal* parser=CParserVCal::NewL();
	CleanupStack::PushL(parser);
	parser->SetObserver(this);
	parser->SetPlugIn(this);
	TInt start=0;
	iCount=0;
	iExpectedParser=parser;
	iExpectedVersion=KVersion;
	iExpectedFirstValueRE=KFirstValue;
	User::LeaveIfError(file.Open(iFs, KFileI, EFileRead));
	CleanupClosePushL(file);
	parser->CVersitParser::InternalizeL(file,start);
	TheTest(iCount==ECountVersionSet+2*ECountNewParser+19*ECountRemoveEscape);
	CleanupStack::Pop(&file);
	file.Close();
	iCount=0;
	iExpectedDefaultEncoding=Versit::ENoEncoding;
	iExpectedFirstValueAE=KVersion;
	iExpectedFirstValueWL=KVersion8;
	iExpectedFirstLineLength=8;
	iExpectedFirstUid.iUid=KVersitPropertyHBufCUid;
	User::LeaveIfError(file.Replace(iFs, KFileP, EFileWrite));
	CleanupClosePushL(file);
	parser->CVersitParser::ExternalizeL(file);
	TheTest(iCount==2*ECountAddEscape+5*ECountWrapLine+13*ECountEncodeType);
	CleanupStack::PopAndDestroy(2,parser);
	}

void CTestVersitObserver::Test2L()
	{
	RFile file;
	_LIT(KFileI,"Z:\\TestFiles\\vCard.VCF");
	_LIT(KFileP,"C:\\TestFiles\\PvCard.VCF");
	_LIT(KVersion,"2.1");
	_LIT8(KVersion8,"2.1");
	_LIT(KFirstValue,"VCARD");
	CParserVCard* parser=CParserVCard::NewL();
	CleanupStack::PushL(parser);
	parser->SetObserver(this);
	parser->SetPlugIn(this);
	TInt start=0;
	iCount=0;
	iExpectedParser=parser;
	iExpectedVersion=KVersion;
	iExpectedFirstValueRE=KFirstValue;
	User::LeaveIfError(file.Open(iFs, KFileI, EFileRead));
	CleanupClosePushL(file);
	parser->CVersitParser::InternalizeL(file,start);
	TheTest(iCount==ECountVersionSet+ECountAddSpace+5*ECountDeleteSpace+8*ECountRemoveEscape);
	CleanupStack::Pop(&file);
	file.Close();
	iCount=0;
	iExpectedDefaultEncoding=Versit::ENoEncoding;
	iExpectedFirstValueAE=KVersion;
	iExpectedFirstValueWL=KVersion8;
	iExpectedFirstLineLength=8;
	iExpectedFirstUid.iUid=KVersitPropertyHBufCUid;
	User::LeaveIfError(file.Replace(iFs, KFileP, EFileWrite));
	CleanupClosePushL(file);
	parser->CVersitParser::ExternalizeL(file);
	TheTest(iCount==5*ECountAddEscape+5*ECountWrapLine+6*ECountEncodeType);
	CleanupStack::PopAndDestroy(2,parser);
	}

void CTestVersitObserver::Test3L()
	{
	RFile file;
	_LIT(KFileI,"Z:\\TestFiles\\Base64.VCF");
	_LIT(KFileP,"C:\\TestFiles\\PBase64.VCF");
	_LIT(KFileP1,"C:\\TestFiles\\PBase64a.VCF");
	_LIT(KVersion,"2.1");
	_LIT8(KVersion8,"2.1");
	_LIT(KFirstValue,"VCARD");
	CParserVCard* parser=CParserVCard::NewL();
	CleanupStack::PushL(parser);
	parser->SetObserver(this);
	parser->SetPlugIn(this);
	TInt start=0;
	iCount=0;
	iExpectedParser=parser;
	iExpectedVersion=KVersion;
	iExpectedFirstValueRE=KFirstValue;
	User::LeaveIfError(file.Open(iFs, KFileI, EFileRead));
	CleanupClosePushL(file);
	parser->CVersitParser::InternalizeL(file,start);
	TheTest(iCount==ECountVersionSet+ECountNeedsBlank+5*ECountRemoveEscape);
	CleanupStack::Pop(&file);
	file.Close();
	iCount=0;
	iExpectedDefaultEncoding=Versit::ENoEncoding;
	iExpectedFirstValueAE=KVersion;
	iExpectedFirstValueWL=KVersion8;
	iExpectedFirstLineLength=8;
	iExpectedFirstUid.iUid=KVersitPropertyHBufCUid;
	iExpectedEncodingEN=Versit::EQuotedPrintableEncoding;
	User::LeaveIfError(file.Replace(iFs, KFileP, EFileWrite));
	CleanupClosePushL(file);
	parser->CVersitParser::ExternalizeL(file);
	TheTest(iCount==3*ECountAddEscape+2*ECountWrapLine+3*ECountEncodeType+ECountEncodeName);
	CleanupStack::Pop(&file);
	file.Close();
	iCount=0;
	iExpectedDefaultEncoding=Versit::EBase64Encoding;
	iExpectedFirstValueAE=KVersion;
	iExpectedEncodingEN=Versit::EBase64Encoding;
	User::LeaveIfError(file.Replace(iFs, KFileP1, EFileWrite));
	CleanupClosePushL(file);
	parser->SetDefaultEncoding(Versit::EBase64Encoding);
	parser->CVersitParser::ExternalizeL(file);
	TheTest(iCount==3*ECountAddEscape+3*ECountEncodeType+3*ECountEncodeName);
	CleanupStack::PopAndDestroy(2,parser);
	}

//test for Reccurrence rule in VCal (VCS)
void CTestVersitObserver::TestReccurrenceRulesL()
	{
	RFile file;
	_LIT(KFileI,"Z:\\TestFiles\\IRDailyVCal.VCS");
	CParserVCal* parser=CParserVCal::NewL();
	CleanupStack::PushL(parser);
	User::LeaveIfError(file.Open(iFs, KFileI, EFileRead));
	CleanupClosePushL(file);
	TInt start=0;
	parser->CVersitParser::InternalizeL(file,start);
	CleanupStack::Pop(&file);
	file.Close();
	CArrayPtr<CVersitParser>* entities= parser->ArrayOfEntities(EFalse);
	CArrayPtr<CParserProperty>* properties = (*entities)[0]->PropertyL(KVersitTokenRRULE, TUid::Uid(KVCalPropertyRecurrenceUid), EFalse); // don't take ownership
	CleanupStack::PushL(properties);
	CParserPropertyValueRecurrence* propertyrecurval = (CParserPropertyValueRecurrence*)((*properties)[0]->Value());	
	TheTest(2 == (propertyrecurval->Value())->iDuration);
	CleanupStack::PopAndDestroy(properties);
	CleanupStack::PopAndDestroy(parser);
	}

GLDEF_C void TestPlugInL(RFs& aFsSession)
	{
	_LIT(KTest,"PlugIn Test");
	
	TheTest.Start(KTest);
	CTestVersitObserver* test=new(ELeave) CTestVersitObserver(aFsSession);
	CleanupStack::PushL(test);
	_LIT(KTest1,"Simple VCal Test");
	
	TheTest.Next(KTest1);
	test->Test1L();
	_LIT(KTest2,"VCard Test");
	
	TheTest.Next(KTest2);
	test->Test2L();
	_LIT(KTest3,"Base64 Test");
	
	TheTest.Next(KTest3);
	test->Test3L();
	//Testing reccurrence rules of VCal (.vcs)
	_LIT(KTestRRule,"Testing Reccurrence Rules...");
	
	TheTest.Next(KTestRRule);
	test->TestReccurrenceRulesL();
	CleanupStack::PopAndDestroy(test);
	TheTest.End();
	}
