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

#define __PROFILING__

// System includes
#include <e32std.h>
#include <e32base.h>
#include <e32test.h>
#include <s32mem.h>

// User includes
#include <versit.h>
#include <vcal.h>
#include <vcard.h>
#include <vtoken.h>

// Globals
_LIT(KTestName,"Performance Test");
_LIT(KTestDirInput,"Z:\\TPerFormance\\inputFile\\");
_LIT(KTestDirOutput,"c:\\TPerFormance\\outputFile\\");
_LIT(KTestDirResult,"c:\\TPerFormance\\resultFile\\");
_LIT(KTestFileNameExt,".TXT");

_LIT8(KVersitTokenAGENDAENTRYTYPE,		"AGENDAENTRYTYPE");
_LIT8(KVersitTokenNICKNAME,				"NICKNAME");
_LIT8(KVersitTokenXWABGENDER,				"X-WAB-GENDER");

LOCAL_D RTest TheTest(KTestName);

class SRHeapCopy 
	{// dummy heap which is a copy RHeapCopy butmake iAllocCount public
public:
	//enum TAllocFail {ERandom,ETrueRandom,EDeterministic,ENone,EFailNext};
public:
	TBool iTestCodeRunning;	
	TInt iTestNestingLevel;
	TInt iTestNumAllocCells;
	TUint iTestAddress;
	TInt iTestSize;
	TInt iTestAllocCount;
public:
	struct SDebugCell {TInt len;TInt nestingLevel;TInt allocCount;};
private:
	TInt iNestingLevel;
public:
	TInt iAllocCount;
private:
	TInt iLevelNumAllocCells;
	SDebugCell* iPtrDebugCell;
	RHeap::TAllocFail iFailType;
	TInt iFailRate;
	TBool iFailed;
	TInt iFailAllocCount;
	TInt iRand;
	};

class CTests : public CBase
	{
public:
	static void DoTestsL();
private:
	enum TDataType
		{
		ETvCardFile,
		ETvCalFile,
		ETvCardLine,
		ETvCalLine,
		};

	struct SPrintResult
		{
		TInt TimeInt;				//internalising time
		TInt TimeExt;				//externalising time
		TInt IncreasedCells;		//net increase in heap cells
		TInt IncreasedSizeInBytes;	//net increase in heap size
		TInt NumAlloInt;			//overall number of allocations when internalising
		TInt NumAlloExt;			//overall number of allocations when externalising
		};

	~CTests();
	void ConstructL();
	void AllTestsL();
	void TestIntExtFileL(TDataType aDataType);
	void TestVCalMemoryUsageByAddingEntitiesL(TInt aNumOfEntry,SPrintResult& aResult);
	void TestVCardMemoryUsageWithDiffCharsetsL(SPrintResult& aResult,const TDesC& aFileName, Versit::TVersitCharSet aCharset=Versit::EUSAsciiCharSet);
	void TestVCardMemoryUsageL();
	void TestVCalMemoryUsageL();
	void CreateEntityEventEntryL(CParserVCalEntity& aEntity);
	void CreateEntityTodoEntryL(CParserVCalEntity& aEntity);
	void PrintOutResult(const TDesC& aFilename, RFile& aFile, SPrintResult aResult);
	void PrintOutResult(RFile& aFile, TInt aNumOfEntries, SPrintResult aResult);
	void CreateAparserLC(TDataType aDataType,CVersitParser*& aParser);
	void ExternaliseFileL(CVersitParser& aParser,const TDesC& aFileName);
	void InternaliseFileL(CVersitParser& aParser,const TDesC& aFileName);
private:
	RFs iFsSession;
	TBool iSessionConnected;
	CVersitParser* iParser;		//This is to keep the Static Utilities around
	CFileMan* iFm;
	};

void CTests::DoTestsL()
	{
	CTests* self=new(ELeave) CTests;
	CleanupStack::PushL(self);
	self->ConstructL();
	self->AllTestsL();
	CleanupStack::PopAndDestroy(self);
	}

void CTests::ConstructL()
	{
	TheTest.Printf(_L("About to connect to file session\r\n"));
	User::LeaveIfError(iFsSession.Connect());
	iSessionConnected=ETrue;
	iFm = CFileMan::NewL(iFsSession);
	iParser=new(ELeave) CVersitParser(CVersitParser::ENoVersionProperty);
	iParser->ConstructL();
	}

CTests::~CTests()
	{
	delete iParser;
	if(iFm)
		{
		iFm->RmDir(KTestDirOutput);
		iFm->RmDir(KTestDirResult);
		iFm->RmDir(_L("c:\\TPerFormance\\"));
		delete iFm;
		}
	if (iSessionConnected)
		iFsSession.Close();
	}

void CTests::ExternaliseFileL(CVersitParser& aParser,const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError(file.Replace(iFsSession, aFileName, EFileWrite));
	CleanupClosePushL(file);
	aParser.ExternalizeL(file);
	CleanupStack::PopAndDestroy(&file);
	}

void CTests::InternaliseFileL(CVersitParser& aParser,const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError(file.Open(iFsSession, aFileName, EFileRead));
	CleanupClosePushL(file);
	TInt start=0;
	aParser.InternalizeL(file, start);
	CleanupStack::PopAndDestroy(&file);
	}

void CTests::CreateAparserLC(TDataType aDataType,CVersitParser*& aParser)
	{
	if ((aDataType==ETvCardFile)||(aDataType==ETvCardLine))
		aParser = CParserVCard::NewL();
	else
		aParser = CParserVCal::NewL();
	CleanupStack::PushL(aParser);
	}

void CTests::PrintOutResult(RFile& aFile,TInt aNumOfEntries, SPrintResult aResult)
	{
	TBuf<200> resultBuf;
	resultBuf.Format(_L("\t%d\t\t%d\t\t%d\t\t%d\x0d\x0a"),aNumOfEntries,aResult.IncreasedSizeInBytes,aResult.IncreasedCells,aResult.NumAlloExt);
	TheTest.Printf(resultBuf);
	TBuf8<100> resultBuf8;
	resultBuf8.Copy(resultBuf);
	aFile.Write(resultBuf8);
	}

void CTests::PrintOutResult(const TDesC& aDescription, RFile& file, SPrintResult aResult)
	{
	//output result
	TBuf<200> resultBuf;
	if (aResult.TimeInt==0)
		resultBuf.Format(_L("%-23S\t%d\t\t%d\t\t%d\x0d\x0a"),&aDescription, aResult.IncreasedSizeInBytes,aResult.IncreasedCells,aResult.NumAlloExt);
	else
		resultBuf.Format(_L("%-23S\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\x0d\x0a"),&aDescription, aResult.TimeInt, aResult.TimeExt, aResult.TimeInt+aResult.TimeExt,aResult.IncreasedCells,aResult.IncreasedSizeInBytes,aResult.NumAlloInt,aResult.NumAlloExt);
	TheTest.Printf(resultBuf);
	TBuf8<100> resultBuf8;
	resultBuf8.Copy(resultBuf);
	file.Write(resultBuf8);
	}

void CTests::AllTestsL()
	{
	TInt err;
	err=iFsSession.MkDirAll(KTestDirOutput());
	if((err!=KErrAlreadyExists) && (err!=KErrNone))
		{
		TheTest.Printf(_L("Directory failed to created err=%d\r\n"),err);
		TheTest.Getch();
		User::Leave(err);
		}
	err=iFsSession.MkDirAll(KTestDirResult());
	if((err!=KErrAlreadyExists) && (err!=KErrNone))
		User::Leave(err);

	_LIT(KTest1,"Int & Ext vCards\n");
	TheTest.Printf(KTest1);
	TestIntExtFileL(ETvCardFile);

	_LIT(KTest2,"\n\nInt & Ext vCals\n");
  	TheTest.Printf(KTest2);
  	TestIntExtFileL(ETvCalFile);
  
 	_LIT(KTest3,"\n\nInt & Ext vCards with single line\n");
  	TheTest.Printf(KTest3);
  	TestIntExtFileL(ETvCardLine);
  
  	_LIT(KTest4,"\n\nInt & Ext vCal with single line\n");
  	TheTest.Printf(KTest4);
  	TestIntExtFileL(ETvCalLine);

	_LIT(KTest5,"\n\nVCal Test with added entities\n");
	TheTest.Printf(KTest5);
	TestVCalMemoryUsageL();
	
	_LIT(KTest6, "\n\nCreate a vCard by Adding Properties\n\n");
	TheTest.Printf(KTest6);
	TestVCardMemoryUsageL(); 
	}

void CTests::TestVCalMemoryUsageL()
	{
	_LIT(KResultWithAddingEntities,"vCalAddingEntriesResult");
	TFileName resultFile;
	resultFile=KTestDirResult;
	resultFile.Append(KResultWithAddingEntities);
	resultFile.Append(KTestFileNameExt);
	RFile file;
	User::LeaveIfError(file.Replace(iFsSession, resultFile,EFileWrite+EFileShareAny+EFileStreamText));
	CleanupClosePushL(file);//file

	TBuf<200> title;
	title.Format(_L("num entries\theap size\tnum obj \tnum. Alloc\x0d\x0a"));
	TheTest.Printf(title);
	TBuf8<200> title8;
	title8.Copy(title);
	file.Write(title8);

	for(TInt i=5;i<41;i=i+5)
		{
		SPrintResult result;
		TestVCalMemoryUsageByAddingEntitiesL(i,result);
		PrintOutResult(file,i,result);
		}

	CleanupStack::PopAndDestroy(&file);
	}

void CTests::TestVCardMemoryUsageL()
	{
	_LIT(KvCardResult,"vCardAddingPropertyResult");
	TFileName resultFile;
	resultFile=KTestDirResult;
	resultFile.Append(KvCardResult);
	resultFile.Append(KTestFileNameExt);
	RFile file;
	User::LeaveIfError(file.Replace(iFsSession, resultFile,EFileWrite+EFileShareAny+EFileStreamText));
	CleanupClosePushL(file);//file

	TBuf<200> title;
	title.Format(_L("file Name\t\t\theap size\tnum obj \tnum. Alloc\x0d\x0a"));
	TheTest.Printf(title);
	TBuf8<200> title8;
	title8.Copy(title);
	file.Write(title8);
	SPrintResult result;
	result.TimeInt=0;
	Versit::TVersitCharSet charset;

	_LIT(KVCardTestFile1, "vCardAddedProAscii");
	TestVCardMemoryUsageWithDiffCharsetsL(result,KVCardTestFile1);
	PrintOutResult(KVCardTestFile1,file,result);

	_LIT(KVCardTestFile2, "vCardAddedProGBK");
	charset=Versit::EGBKCharSet;
	TestVCardMemoryUsageWithDiffCharsetsL(result,KVCardTestFile2,charset);
	PrintOutResult(KVCardTestFile2,file,result);

	_LIT(KVCardTestFile3, "vCardAddedProShifJis");
	charset=Versit::EShiftJISCharSet;
	TestVCardMemoryUsageWithDiffCharsetsL(result,KVCardTestFile3,charset);
	PrintOutResult(KVCardTestFile3,file,result);

	_LIT(KVCardTestFile4,"vCardAddedProUtf7");
	charset=Versit::EUTF7CharSet;
	TestVCardMemoryUsageWithDiffCharsetsL(result,KVCardTestFile4,charset);
	PrintOutResult(KVCardTestFile4,file,result);

	_LIT(KVCardTestFile5,"vCardAddedProJis");
	charset=Versit::EJISCharSet;
	TestVCardMemoryUsageWithDiffCharsetsL(result,KVCardTestFile5,charset);
	PrintOutResult(KVCardTestFile5,file,result);

	CleanupStack::PopAndDestroy(&file);
	}

void CTests::TestVCardMemoryUsageWithDiffCharsetsL(SPrintResult& aResult, const TDesC& aFileName,Versit::TVersitCharSet aCharset)
	{
	TInt numCellsBefore, numCellsAfter, sizeInBytesBefore, sizeInBytesAfter, numAllocBefore, numAllocAfter;
	RHeap& heap = User::Heap();
	SRHeapCopy& myHeap = REINTERPRET_CAST(SRHeapCopy&, heap);
	numCellsBefore = heap.AllocSize(sizeInBytesBefore);
	numAllocBefore = myHeap.iAllocCount; 
	
	CParserVCard* vCardParser = CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);

	_LIT8(KHOME, "HOME");
	_LIT8(KVOICE, "VOICE");
	_LIT8(KCELL, "CELL");
	_LIT8(KFAX, "FAX");
	_LIT8(KPOSTAL, "POSTAL");
	_LIT8(KPREF, "PREF");
	_LIT8(KWORK, "WORK");

	_LIT(KSurname, "Example");
	_LIT(KFirstname, "A");
	_LIT(KMiddlename, "N");
	_LIT(KFullname, "A N Example");
	_LIT(KTIME, "20010419T103325Z");
	_LIT(KEMAIL, "symbian.foundation@exp.example.test");
	_LIT(KTELNUM, "01-6329-60000");
	_LIT(KORG, "Symbian Foundation Center");
	_LIT(KDR, "DR");
	_LIT(KLONGNOTE, "This is a long note to test This is a long note to testThis is a long note to test This is a long note to test This is a long note to test This is a long note to test");
	_LIT(KADSFIELD1, "Example 1");
	_LIT(KADSFIELD2, "Example City Ø");
	_LIT(KADSFIELD3, "ZZ99 EXP");
	_LIT(KLONGLAB, "long label to test ong label to test ong label to test ong label to test ong label to test ong label to test ong label to test");
	_LIT(KNUM, "2");
	_LIT(KADSINTER, "http://www.symbianfoundation.example");
	_LIT(KDATE, "19520607");

	//property 1
	CDesCArrayFlat* desArray1 = new (ELeave) CDesCArrayFlat(3);
	CleanupStack::PushL(desArray1);
	desArray1->AppendL(KSurname);  
	desArray1->AppendL(KFirstname);
	desArray1->AppendL(KMiddlename); 
	CParserPropertyValue* valueN=new (ELeave) CParserPropertyValueCDesCArray(desArray1);
  	CleanupStack::Pop(desArray1);
	CleanupStack::PushL(valueN);
	CParserGroupedProperty* propertyN = CParserGroupedProperty::NewL(*valueN, KVersitTokenN, NULL, NULL);
	CleanupStack::Pop(valueN);
	CleanupStack::PushL(propertyN);
	vCardParser->AddPropertyL(propertyN);
	CleanupStack::Pop(propertyN);

	//property 2
	CParserPropertyValue* valueFN = CParserPropertyValueHBufC::NewL(KFullname);
	CleanupStack::PushL(valueFN);
	CParserGroupedProperty* propertyFN = CParserGroupedProperty::NewL(*valueFN, KVersitTokenFN, NULL, NULL);
	CleanupStack::Pop(valueFN);
	CleanupStack::PushL(propertyFN);
	vCardParser->AddPropertyL(propertyFN);
	CleanupStack::Pop(propertyFN);

	//property 3
	CParserPropertyValue* valueNN = CParserPropertyValueHBufC::NewL(KFullname);
	CleanupStack::PushL(valueNN);
	CParserGroupedProperty* propertyNN = CParserGroupedProperty::NewL(*valueNN, KVersitTokenNICKNAME, NULL, NULL);
	CleanupStack::Pop(valueNN);
	CleanupStack::PushL(propertyNN);
	vCardParser->AddPropertyL(propertyNN);
	CleanupStack::Pop(propertyNN);

	//property 4
	CParserPropertyValue* value4 = CParserPropertyValueHBufC::NewL(KORG);
	CleanupStack::PushL(value4);
	CParserGroupedProperty* property4 = CParserGroupedProperty::NewL(*value4, KVersitTokenORG, NULL, NULL);
	CleanupStack::Pop(value4);
	CleanupStack::PushL(property4);
	vCardParser->AddPropertyL(property4);
	CleanupStack::Pop(property4);

	//property 5
	CParserPropertyValue* value5 = CParserPropertyValueHBufC::NewL(KDR);
	CleanupStack::PushL(value5);
	CParserGroupedProperty* property5 = CParserGroupedProperty::NewL(*value5, KVersitTokenTITLE, NULL, NULL);
	CleanupStack::Pop(value5);
	CleanupStack::PushL(property5);
	vCardParser->AddPropertyL(property5);
	CleanupStack::Pop(property5);

	//property 6
	CParserPropertyValue* value6 = CParserPropertyValueHBufC::NewL(KLONGNOTE);
	CleanupStack::PushL(value6);
	CParserGroupedProperty* property6 = CParserGroupedProperty::NewL(*value6, KVersitTokenNOTE, NULL, NULL);
	CleanupStack::Pop(value6);
	CleanupStack::PushL(property6);
	vCardParser->AddPropertyL(property6);
	CleanupStack::Pop(property6);

	//property 7
	CArrayPtr<CParserParam>* arrayOfParams = new(ELeave) CArrayPtrFlat<CParserParam>(2);
	CParserParam* parserParam1 = CParserParam::NewL(KWORK, KNullDesC8);
	CleanupStack::PushL(parserParam1);
	arrayOfParams->AppendL(parserParam1);
	CleanupStack::Pop(parserParam1);

	CParserParam* parserParam2= CParserParam::NewL(KVOICE, KNullDesC8);
	CleanupStack::PushL(parserParam2);
	arrayOfParams->AppendL(parserParam2);
	CleanupStack::Pop(parserParam2);
	CParserPropertyValue* value7 = CParserPropertyValueHBufC::NewL(KTELNUM);
	CleanupStack::PushL(value7);
	CParserGroupedProperty* property7 = CParserGroupedProperty::NewL(*value7, KVersitTokenTEL, NULL, arrayOfParams);
	CleanupStack::Pop(value7);
	CleanupStack::PushL(property7);
	vCardParser->AddPropertyL(property7);
	CleanupStack::Pop(property7);

	//property 8
	CArrayPtr<CParserParam>* arrayOfParams8 = new(ELeave) CArrayPtrFlat<CParserParam>(2);
	CParserParam* parserParam8A = CParserParam::NewL(KHOME, KNullDesC8);
	CleanupStack::PushL(parserParam8A);
	arrayOfParams8->AppendL(parserParam8A);
	CleanupStack::Pop(parserParam8A);
	CParserParam* parserParam8B = CParserParam::NewL(KHOME, KNullDesC8);
	CleanupStack::PushL(parserParam8B);
	arrayOfParams8->AppendL(parserParam8B);
	CleanupStack::Pop(parserParam8B);
	CParserPropertyValue* value8 = CParserPropertyValueHBufC::NewL(KTELNUM);
	CleanupStack::PushL(value8);
	CParserGroupedProperty* property8 = CParserGroupedProperty::NewL(*value8, KVersitTokenTEL, NULL, arrayOfParams8);
	CleanupStack::Pop(value8);
	CleanupStack::PushL(property8);
	vCardParser->AddPropertyL(property8);
	CleanupStack::Pop(property8);

	//property 9
	CArrayPtr<CParserParam>* arrayOfParams9 = new(ELeave) CArrayPtrFlat<CParserParam>(2);
	CParserParam* parserParam9A = CParserParam::NewL(KCELL, KNullDesC8);
	CleanupStack::PushL(parserParam9A);
	arrayOfParams9->AppendL(parserParam9A);
	CleanupStack::Pop(parserParam9A);
	CParserParam* parserParam9B = CParserParam::NewL(KVOICE, KNullDesC8);
	CleanupStack::PushL(parserParam9B);
	arrayOfParams9->AppendL(parserParam9B);
	CleanupStack::Pop(parserParam9B);
	CParserPropertyValue* value9 = CParserPropertyValueHBufC::NewL(KTELNUM);
	CleanupStack::PushL(value9);
	CParserGroupedProperty* property9 = CParserGroupedProperty::NewL(*value9, KVersitTokenTEL, NULL, arrayOfParams9);
	CleanupStack::Pop(value9);
	CleanupStack::PushL(property9);
	vCardParser->AddPropertyL(property9);
	CleanupStack::Pop(property9);

	//property 10
	CArrayPtr<CParserParam>* arrayOfParams10 = new(ELeave) CArrayPtrFlat<CParserParam>(2);
	CParserParam* parserParam10A = CParserParam::NewL(KWORK, KNullDesC8);
	CleanupStack::PushL(parserParam10A);
	arrayOfParams10->AppendL(parserParam10A);
	CleanupStack::Pop(parserParam10A);
	CParserParam* parserParam10B = CParserParam::NewL(KFAX, KNullDesC8);
	CleanupStack::PushL(parserParam10B);
	arrayOfParams10->AppendL(parserParam10B);
	CleanupStack::Pop(parserParam10B);
	CParserPropertyValue* value10 = CParserPropertyValueHBufC::NewL(KTELNUM);
	CleanupStack::PushL(value10);
	CParserGroupedProperty* property10 = CParserGroupedProperty::NewL(*value10, KVersitTokenTEL, NULL, arrayOfParams10);
	CleanupStack::Pop(value10);
	CleanupStack::PushL(property10);
	vCardParser->AddPropertyL(property10);
	CleanupStack::Pop(property10);

	//property 11

	CArrayPtr<CParserParam>* arrayOfParams11 = new(ELeave) CArrayPtrFlat<CParserParam>(2);
	CParserParam* parserParam11A = CParserParam::NewL(KPOSTAL, KNullDesC8);
	CleanupStack::PushL(parserParam11A);
	arrayOfParams11->AppendL(parserParam11A);
	CleanupStack::Pop(parserParam11A);
	CParserParam* parserParam11B = CParserParam::NewL(KWORK, KNullDesC8);
	CleanupStack::PushL(parserParam11B);
	arrayOfParams11->AppendL(parserParam11B);
	CleanupStack::Pop(parserParam11B);
	CDesCArrayFlat* desArrayAdr = new (ELeave) CDesCArrayFlat(3);
	CleanupStack::PushL(desArrayAdr);
	desArrayAdr->AppendL(KNullDesC);  
	desArrayAdr->AppendL(KADSFIELD1);  
	desArrayAdr->AppendL(KADSFIELD2);
	desArrayAdr->AppendL(KNullDesC);  
	desArrayAdr->AppendL(KADSFIELD3); 
	CParserPropertyValue* valueAdr=new (ELeave) CParserPropertyValueCDesCArray(desArrayAdr);
  	CleanupStack::Pop(desArrayAdr);
	CleanupStack::PushL(valueAdr);
	CParserGroupedProperty* property11 = CParserGroupedProperty::NewL(*valueAdr, KVersitTokenADR, NULL, arrayOfParams11);
	CleanupStack::Pop(valueAdr);
	CleanupStack::PushL(property11);
	vCardParser->AddPropertyL(property11);
	CleanupStack::Pop(property11);

	//property 12
	CArrayPtr<CParserParam>* arrayOfParams12 = new(ELeave) CArrayPtrFlat<CParserParam>(2);
	CParserParam* parserParam12 = CParserParam::NewL(KWORK, KNullDesC8);
	CleanupStack::PushL(parserParam12);
	arrayOfParams12->AppendL(parserParam12);
	CleanupStack::Pop(parserParam12);
	CParserPropertyValue* value12 = CParserPropertyValueHBufC::NewL(KLONGLAB);
	CleanupStack::PushL(value12);
	CParserGroupedProperty* property12 = CParserGroupedProperty::NewL(*value12, KVersitTokenLABEL, NULL, arrayOfParams12);
	CleanupStack::Pop(value12);
	CleanupStack::PushL(property12);
	vCardParser->AddPropertyL(property12);
	CleanupStack::Pop(property12);

	//property 13

	CArrayPtr<CParserParam>* arrayOfParams13 = new(ELeave) CArrayPtrFlat<CParserParam>(3);
	CParserParam* parserParam13A = CParserParam::NewL(KPOSTAL, KNullDesC8);
	CleanupStack::PushL(parserParam13A);
	arrayOfParams13->AppendL(parserParam13A);
	CleanupStack::Pop(parserParam13A);
	CParserParam* parserParam13B = CParserParam::NewL(KHOME, KNullDesC8);
	CleanupStack::PushL(parserParam13B);
	arrayOfParams13->AppendL(parserParam13B);
	CleanupStack::Pop(parserParam13B);
	CDesCArrayFlat* desArrayAdr1 = new (ELeave) CDesCArrayFlat(3);
	CleanupStack::PushL(desArrayAdr1);
	desArrayAdr1->AppendL(KNullDesC);  
	desArrayAdr1->AppendL(KADSFIELD1);  
	desArrayAdr1->AppendL(KADSFIELD2);
	desArrayAdr1->AppendL(KNullDesC);  
	desArrayAdr1->AppendL(KADSFIELD3); 
	CParserPropertyValue* valueAdr1=new (ELeave) CParserPropertyValueCDesCArray(desArrayAdr1);
  	CleanupStack::Pop(desArrayAdr1);
	CleanupStack::PushL(valueAdr1);
	CParserGroupedProperty* property13 = CParserGroupedProperty::NewL(*valueAdr1, KVersitTokenADR, NULL, arrayOfParams13);
	CleanupStack::Pop(valueAdr1);
	CleanupStack::PushL(property13);
	vCardParser->AddPropertyL(property13);
	CleanupStack::Pop(property13);

	//property 14
	CArrayPtr<CParserParam>* arrayOfParams14 = new(ELeave) CArrayPtrFlat<CParserParam>(2);
	CParserParam* parserParam14 = CParserParam::NewL(KHOME, KNullDesC8);
	CleanupStack::PushL(parserParam14);
	arrayOfParams14->AppendL(parserParam14);
	CleanupStack::Pop(parserParam14);
	CParserPropertyValue* value14 = CParserPropertyValueHBufC::NewL(KLONGLAB);
	CleanupStack::PushL(value14);
	CParserGroupedProperty* property14 = CParserGroupedProperty::NewL(*value14, KVersitTokenLABEL, NULL, arrayOfParams14);
	CleanupStack::Pop(value14);
	CleanupStack::PushL(property14);
	vCardParser->AddPropertyL(property14);
	CleanupStack::Pop(property14);

	//property 15
	CParserPropertyValue* value15 = CParserPropertyValueHBufC::NewL(KNUM);
	CleanupStack::PushL(value15);
	CParserGroupedProperty* property15 = CParserGroupedProperty::NewL(*value15, KVersitTokenXWABGENDER, NULL, NULL);
	CleanupStack::Pop(value15);
	CleanupStack::PushL(property15);
	vCardParser->AddPropertyL(property15);
	CleanupStack::Pop(property15);

	//property 16
	CParserPropertyValue* value16 = CParserPropertyValueHBufC::NewL(KVersitTokenEmpty);
	CleanupStack::PushL(value16);
	CParserGroupedProperty* property16 = CParserGroupedProperty::NewL(*value16, KVersitTokenURL, NULL, NULL);
	CleanupStack::Pop(value16);
	CleanupStack::PushL(property16);
	vCardParser->AddPropertyL(property16);
	CleanupStack::Pop(property16);

	//property 17
	CParserPropertyValue* value17 = CParserPropertyValueHBufC::NewL(KADSINTER);
	CleanupStack::PushL(value17);
	CParserGroupedProperty* property17 = CParserGroupedProperty::NewL(*value17, KVersitTokenURL, NULL, NULL);
	CleanupStack::Pop(value17);
	CleanupStack::PushL(property17);
	vCardParser->AddPropertyL(property17);
	CleanupStack::Pop(property17);

	//property 18
	CParserPropertyValue* value18 = CParserPropertyValueHBufC::NewL(KDATE);
	CleanupStack::PushL(value18);
	CParserGroupedProperty* property18 = CParserGroupedProperty::NewL(*value18, KVersitTokenBDAY, NULL, NULL);
	CleanupStack::Pop(value18);
	CleanupStack::PushL(property18);
	vCardParser->AddPropertyL(property18);
	CleanupStack::Pop(property18);

	//property 19
	CArrayPtr<CParserParam>* arrayOfParams19 = new(ELeave) CArrayPtrFlat<CParserParam>(2);
	CParserParam* parserParam19A = CParserParam::NewL(KPREF, KNullDesC8);
	CleanupStack::PushL(parserParam19A);
	arrayOfParams19->AppendL(parserParam19A);
	CleanupStack::Pop(parserParam19A);
	CParserParam* parserParam19B= CParserParam::NewL(KVersitTokenINTERNET, KNullDesC8);
	CleanupStack::PushL(parserParam19B);
	arrayOfParams19->AppendL(parserParam19B);
	CleanupStack::Pop(parserParam19B);
	CParserPropertyValue* value19 = CParserPropertyValueHBufC::NewL(KEMAIL);
	CleanupStack::PushL(value19);
	CParserGroupedProperty* property19 = CParserGroupedProperty::NewL(*value19, KVersitTokenEMAIL, NULL, arrayOfParams19);
	CleanupStack::Pop(value19);
	CleanupStack::PushL(property19);
	vCardParser->AddPropertyL(property19);
	CleanupStack::Pop(property19);

	//property 20
	CArrayPtr<CParserParam>* arrayOfParams20 = new(ELeave) CArrayPtrFlat<CParserParam>(1);
	CParserParam* parserParam20= CParserParam::NewL(KVersitTokenINTERNET, KNullDesC8);
	CleanupStack::PushL(parserParam20);
	arrayOfParams20->AppendL(parserParam20);
	CleanupStack::Pop(parserParam20);
	CParserPropertyValue* value20 = CParserPropertyValueHBufC::NewL(KEMAIL);
	CleanupStack::PushL(value20);
	CParserGroupedProperty* property20 = CParserGroupedProperty::NewL(*value20, KVersitTokenEMAIL, NULL, arrayOfParams20);
	CleanupStack::Pop(value20);
	CleanupStack::PushL(property20);
	vCardParser->AddPropertyL(property20);
	CleanupStack::Pop(property20);

	//property 21
	CParserPropertyValue* value21 = CParserPropertyValueHBufC::NewL(KTIME);
	CleanupStack::PushL(value21);
	CParserGroupedProperty* property21 = CParserGroupedProperty::NewL(*value21, KVersitTokenREV, NULL, NULL);
	CleanupStack::Pop(value21);
	CleanupStack::PushL(property21);
	vCardParser->AddPropertyL(property21);
	CleanupStack::Pop(property21);

	_LIT(KvCard,".VCF");

	TFileName fullNameFile;
	fullNameFile=KTestDirOutput;
	fullNameFile.Append(aFileName);
	fullNameFile.Append(KvCard);
	vCardParser->SetDefaultCharSet(aCharset);
	ExternaliseFileL(*vCardParser, fullNameFile);

	numAllocAfter = myHeap.iAllocCount;
	numCellsAfter = heap.AllocSize(sizeInBytesAfter);
	aResult.NumAlloExt = numAllocAfter - numAllocBefore;
	aResult.IncreasedCells = numCellsAfter - numCellsBefore;
	aResult.IncreasedSizeInBytes = sizeInBytesAfter - sizeInBytesBefore;
	CleanupStack::PopAndDestroy(vCardParser);
	}

void CTests::TestVCalMemoryUsageByAddingEntitiesL(TInt aNumOfEntry,SPrintResult& aResult)
	{
	_LIT(KVCalTestFile, "VCalWithAddedEntries");
	_LIT(KvCal,".VCS");

	TInt numCellsBefore, numCellsAfter, sizeInBytesBefore, sizeInBytesAfter, numAllocBefore, numAllocAfter;
	RHeap& heap = User::Heap();
	SRHeapCopy& myHeap = REINTERPRET_CAST(SRHeapCopy&, heap);
	numCellsBefore = heap.AllocSize(sizeInBytesBefore);
	numAllocBefore = myHeap.iAllocCount; 

	CParserVCal* vCalParser = CParserVCal::NewL();
	CleanupStack::PushL(vCalParser);
	for(TInt i=0;i<aNumOfEntry/2;i++)
		{
		CParserVCalEntity* entityEvent = CParserVCalEntity::NewL();
		CleanupStack::PushL(entityEvent);
		CreateEntityEventEntryL(*entityEvent);
		CleanupStack::Pop(entityEvent);
		vCalParser->AddEntityL(entityEvent);
		}
	TInt mumTodo=aNumOfEntry/2+aNumOfEntry%2;
	for(TInt j=0;j<mumTodo;j++)
		{
		CParserVCalEntity* entityTodo = CParserVCalEntity::NewL();
		CleanupStack::PushL(entityTodo);
		CreateEntityTodoEntryL(*entityTodo);
		CleanupStack::Pop(entityTodo);
		vCalParser->AddEntityL(entityTodo);
		}

	//creating an output file and externalise all the data (properties)
	//that have just been created
	TBuf<2> numString;
	numString.Num(aNumOfEntry);
	TFileName fullNameFile;
	fullNameFile=KTestDirOutput;
	fullNameFile.Append(KVCalTestFile);
	fullNameFile.Append(numString);
	fullNameFile.Append(KvCal);
	RFile file;
	CleanupClosePushL(file);//file
	ExternaliseFileL(*vCalParser, fullNameFile);

	numAllocAfter = myHeap.iAllocCount;
	numCellsAfter = heap.AllocSize(sizeInBytesAfter);
	aResult.NumAlloExt = numAllocAfter - numAllocBefore;
	aResult.IncreasedCells = numCellsAfter - numCellsBefore;
	aResult.IncreasedSizeInBytes = sizeInBytesAfter - sizeInBytesBefore;
 	CleanupStack::PopAndDestroy(2,vCalParser);
	}

void CTests::CreateEntityEventEntryL(CParserVCalEntity& aEntity)
	{
	_LIT(KTIME, "19991101T000000Z");

	aEntity.SetEntityNameL(KVersitVarTokenVEVENT);

	//property 1 within same entity
	CParserPropertyValue* value1 = CParserPropertyValueHBufC::NewL(_L("APPOINTMENT"));
	CleanupStack::PushL(value1);
	CParserProperty* property1 = CParserProperty::NewL(*value1, KVersitTokenAGENDAENTRYTYPE, NULL);
	CleanupStack::Pop(value1);
	CleanupStack::PushL(property1);
	aEntity.AddPropertyL(property1);
	CleanupStack::Pop(property1);
	
	//property 2 within same entity
	CParserPropertyValue* value2 = CParserPropertyValueHBufC::NewL(_L("app engines weekly meeting APPOINTMENT"));
	CleanupStack::PushL(value2);
	CParserProperty* property2 = CParserProperty::NewL(*value2, KVersitTokenDESCRIPTION, NULL);
	CleanupStack::Pop(value2);
	CleanupStack::PushL(property2);
	aEntity.AddPropertyL(property2);
	CleanupStack::Pop(property2);

	//property 3 within same entity
	CParserPropertyValue* value3 = CParserPropertyValueHBufC::NewL(_L("2"));
	CleanupStack::PushL(value3);
	CParserProperty* property3 = CParserProperty::NewL(*value3, KVersitTokenUID, NULL);
	CleanupStack::Pop(value3);
	CleanupStack::PushL(property3);
	aEntity.AddPropertyL(property3);
	CleanupStack::Pop(property3);

	CArrayPtr<CParserParam>* arrayOfParams4 = new(ELeave) CArrayPtrFlat<CParserParam>(1);
	CParserParam* parserParam4 = CParserParam::NewL(KVersitTokenSOUND, KNullDesC8);
	CleanupStack::PushL(parserParam4);
	arrayOfParams4->AppendL(parserParam4);
	CleanupStack::Pop(parserParam4);

	TDateTime date(2001,ENovember,1,0,15,0,0);
	TVersitDateTime* runtime = new(ELeave) TVersitDateTime(date, TVersitDateTime::EIsMachineLocal);
	CleanupStack::PushL(runtime);
	CVersitAlarm* value4 = CVersitAlarm::NewL(runtime,0,0,_L("Fanfare"),KNullDesC);
	CleanupStack::Pop(runtime);
	CleanupStack::PushL(value4);

	CParserPropertyValueAlarm* alarmValue4 = new(ELeave) CParserPropertyValueAlarm(value4);
	CleanupStack::Pop(value4);
	CleanupStack::PushL(alarmValue4);
	CParserProperty* property4 = CParserProperty::NewL(*alarmValue4, KVersitTokenAALARM, arrayOfParams4);
	CleanupStack::Pop(alarmValue4);
	CleanupStack::PushL(property4);
	aEntity.AddPropertyL(property4);
	CleanupStack::Pop(property4);

	//property 5 within same entity
	CParserPropertyValue* value5 = CParserPropertyValueHBufC::NewL(_L("PUBLIC"));
	CleanupStack::PushL(value5);
	CParserProperty* property5 = CParserProperty::NewL(*value5, KVersitTokenCLASS, NULL);
	CleanupStack::Pop(value5);
	CleanupStack::PushL(property5);
	aEntity.AddPropertyL(property5);
	CleanupStack::Pop(property5);

	//property 6 within same entity
	CParserPropertyValue* value6 = CParserPropertyValueHBufC::NewL(_L("19991101T000000Z"));
	CleanupStack::PushL(value6);
	CParserProperty* property6 = CParserProperty::NewL(*value6, KVersitTokenDCREATED, NULL);
	CleanupStack::Pop(value6);
	CleanupStack::PushL(property6);
	aEntity.AddPropertyL(property6);
	CleanupStack::Pop(property6);

	//property 7 within same entity
	CParserPropertyValue* value7 = CParserPropertyValueHBufC::NewL(KTIME);
	CleanupStack::PushL(value7);
	CParserProperty* property7 = CParserProperty::NewL(*value7, KVersitTokenLASTMODIFIED, NULL);
	CleanupStack::Pop(value7);
	CleanupStack::PushL(property7);
	aEntity.AddPropertyL(property7);
	CleanupStack::Pop(property7);

	//property 8 within same entity
	CParserPropertyValue* value8 = CParserPropertyValueHBufC::NewL(KTIME);
	CleanupStack::PushL(value8);
	CParserProperty* property8 = CParserProperty::NewL(*value8, KVersitTokenDTSTART, NULL);
	CleanupStack::Pop(value8);
	CleanupStack::PushL(property8);
	aEntity.AddPropertyL(property8);
	CleanupStack::Pop(property8);

	//property 9 within same entity
	CParserPropertyValue* value9 = CParserPropertyValueHBufC::NewL(KTIME);
	CleanupStack::PushL(value9);
	CParserProperty* property9 = CParserProperty::NewL(*value9, KVersitTokenDTEND, NULL);
	CleanupStack::Pop(value9);
	CleanupStack::PushL(property9);
	aEntity.AddPropertyL(property9);
	CleanupStack::Pop(property9);
	}


void CTests::CreateEntityTodoEntryL(CParserVCalEntity& aEntity)
	{
	aEntity.SetEntityNameL(KVersitVarTokenVTODO);
	
	//property 1 within same entity
	CParserPropertyValue* value1 = CParserPropertyValueHBufC::NewL(_L("APPOINTMENT"));
	CleanupStack::PushL(value1);
	CParserProperty* property1 = CParserProperty::NewL(*value1, KVersitTokenAGENDAENTRYTYPE, NULL);
	CleanupStack::Pop(value1);
	CleanupStack::PushL(property1);
	aEntity.AddPropertyL(property1);
	CleanupStack::Pop(property1);

	//property 2 within same entity
	CParserPropertyValue* value2= CParserPropertyValueHBufC::NewL(_L("meeting with Nokia APPOINTMENT"));
	CleanupStack::PushL(value2);
	CParserProperty* property2 = CParserProperty::NewL(*value2, KVersitTokenDESCRIPTION, NULL);
	CleanupStack::Pop(value2);
	CleanupStack::PushL(property2);
	aEntity.AddPropertyL(property2);
	CleanupStack::Pop(property2);

	//property 3 within same entity
	CParserPropertyValue* value3 = CParserPropertyValueHBufC::NewL(_L("3"));
	CleanupStack::PushL(value3);
	CParserProperty* property3 = CParserProperty::NewL(*value3, KVersitTokenUID, NULL);
	CleanupStack::Pop(value3);
	CleanupStack::PushL(property3);
	aEntity.AddPropertyL(property3);
	CleanupStack::Pop(property3);

	//property 4 within same entity
	CParserPropertyValue* value4 = CParserPropertyValueHBufC::NewL(_L("PUBLIC"));
	CleanupStack::PushL(value4);
	CParserProperty* property4 = CParserProperty::NewL(*value4, KVersitTokenCLASS, NULL);
	CleanupStack::Pop(value4);
	CleanupStack::PushL(property4);
	aEntity.AddPropertyL(property4);
	CleanupStack::Pop(property4);

	//property 5 within same entity
	CParserPropertyValue* value5 = CParserPropertyValueHBufC::NewL(_L("19991101T000000Z"));
	CleanupStack::PushL(value5);
	CParserProperty* property5 = CParserProperty::NewL(*value5, KVersitTokenDCREATED, NULL);
	CleanupStack::Pop(value5);
	CleanupStack::PushL(property5);
	aEntity.AddPropertyL(property5);
	CleanupStack::Pop(property5);

	//property 6 within same entity
	CParserPropertyValue* value6 = CParserPropertyValueHBufC::NewL(_L("19991101T130400Z"));
	CleanupStack::PushL(value6);
	CParserProperty* property6 = CParserProperty::NewL(*value6, KVersitTokenLASTMODIFIED, NULL);
	CleanupStack::Pop(value6);
	CleanupStack::PushL(property6);
	aEntity.AddPropertyL(property6);
	CleanupStack::Pop(property6);


	//property 7 within same entity
	CParserPropertyValue* value7 = CParserPropertyValueHBufC::NewL(_L("19991101T100000Z"));
	CleanupStack::PushL(value7);
	CParserProperty* property7 = CParserProperty::NewL(*value7, KVersitTokenDTSTART, NULL);
	CleanupStack::Pop(value7);
	CleanupStack::PushL(property7);
	aEntity.AddPropertyL(property7);
	CleanupStack::Pop(property7);

	//property 8 within same entity
	CParserPropertyValue* value8 = CParserPropertyValueHBufC::NewL(_L("19991001T113000Z"));
	CleanupStack::PushL(value8);
	CParserProperty* property8 = CParserProperty::NewL(*value8, KVersitTokenDTEND, NULL);
	CleanupStack::Pop(value8);
	CleanupStack::PushL(property8);
	aEntity.AddPropertyL(property8);
	CleanupStack::Pop(property8);
	}

void CTests::TestIntExtFileL(TDataType aDataType)
	{
	//wild cards for searching testing files and store the profiling result in different files
	_LIT(KvCardFile,"File*.vcf");
	_LIT(KvCalFile,"File*.vcs");
	_LIT(KvCardLine,"Line*.vcf");
	_LIT(KvCalLine,"Line*.vcs");
	_LIT(KvCardResultFile,"Int&Ext_vCardFile_com");
	_LIT(KvCalResultFile,"Int&Ext_vCalFile_com");
	_LIT(KvCardResultLine,"Int&Ext_vCardLine_com");
	_LIT(KvCalResultLine,"Int&Ext_vCalLine_com");

	TFileName dataFile;
	TFileName resultFile;
	resultFile=KTestDirResult;
	switch (aDataType)
		{
		case ETvCardFile:
			dataFile=KvCardFile;
			resultFile.Append(KvCardResultFile);
			break;
		case ETvCalFile:
			dataFile=KvCalFile;
			resultFile.Append(KvCalResultFile);
			break;
		case ETvCardLine:
			dataFile=KvCardLine;
			resultFile.Append(KvCardResultLine);
			break;
		case ETvCalLine:
			dataFile=KvCalLine;
			resultFile.Append(KvCalResultLine);
			break;
		}
	resultFile.Append(KTestFileNameExt);

	TFindFile* findFile = new(ELeave) TFindFile(iFsSession);
	CleanupStack::PushL(findFile);//findFile
	CDir* directory=NULL;

	User::LeaveIfError(findFile->FindWildByDir(dataFile,KTestDirInput,directory));
	CleanupStack::PopAndDestroy(findFile);//findFile
	CleanupStack::PushL(directory);//directory
	RFile file;
	User::LeaveIfError(file.Replace(iFsSession, resultFile,EFileWrite+EFileShareAny+EFileStreamText));
	CleanupClosePushL(file);//file

	TBuf<200> title;
	title.Format(_L("file name\tinternalize\texternalize\ttotal time\tnum of obj\theap size\tnum. Alloc int\tnum. Alloc ext\x0d\x0a"));
	TheTest.Printf(title);
	TBuf8<200> title8;
	title8.Copy(title);
	file.Write(title8);

	TInt numberOfFiles=directory->Count();
	CleanupStack::PopAndDestroy(2,directory);
	}

//@SYMTestCaseID PIM-TPERFOMANCE-0001
	
GLDEF_C TInt E32Main()
	{
	
	TTime startTime;
	startTime.UniversalTime();
	
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if	(!cleanup)
		return KErrNoMemory;
	TheTest.Start(_L("@SYMTestCaseID PIM-TPERFOMANCE-0001 Performance Test"));
	
	TRAPD(err,CTests::DoTestsL());
#if !defined(__WINS__)
	if (err!=KErrNone)
		{
		_LIT(KError,"Test left with error %d\r\n");
		TheTest.Printf(KError,err);
		TheTest.Getch();
		}
#endif
	TheTest(err == KErrNone);
	TheTest.Printf(_L("Test complete\r\n"));
	User::After(3000000);
	TheTest.End();
	TheTest.Close();

	delete cleanup;
	__UHEAP_MARKEND;
	
	TTime finishTime;
	finishTime.UniversalTime();
	TReal elapsed = (TReal)finishTime.MicroSecondsFrom(startTime).Int64();
	TheTest.Printf(_L("Elapsed time: %.4f\n"), elapsed/1000000);
	
	return (KErrNone);
	}
