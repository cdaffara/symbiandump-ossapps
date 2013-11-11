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

// System includes
#include <e32std.h>
#include <e32base.h>
#include <e32test.h>
#include <s32mem.h>

// User includes
#include <versit.h>
#include <vcal.h>
#include <vcard.h>
#include "Observ.H"
#include "base64extension.h"
#include <coreappstest/testserver.h>

// Globals
RTest TheTest(_L("Versit Escaping Test"));

_LIT(KSimpleFileName,"C:\\SimpleO.VCF");
_LIT(KOutEscFileName,"C:\\Output-Escaped.vcf");
_LIT(KOutputFileName,"C:\\Output-QP.vcf");
_LIT(KVCardFileShiftJIS,"c:\\charsetSJIS.vcf");
_LIT(KVCardFileJIS,"c:\\charsetJIS.vcf ");
_LIT(KBase64FileName,"C:\\Output-Base64.vcf");

_LIT(KOutputQPFileName,"C:\\Export-QP-Vcard.vcf");
_LIT(KQPFileName,"C:\\QP-Vcard.vcf");
_LIT(KOutputQPHashFileName,"C:\\Export-QP-Hash.vcf");
_LIT(KQPHashFileName, "C:\\QP-Hash.vcf");

_LIT(KHugeVCardFileName,"z:\\testvcards\\various\\HugeVCard.vcf");
_LIT(KExportHugeVCardFileName,"C:\\OutputHugeVCard.vcf");


class CTests;

// Typedefs
typedef void (*TVersitStreamFunction)(CVersitParser& aParser);
typedef void (*TOOMCallback)(CTests& aSelf);


//#define TBUGFIX_INCLUDE_OOM //Comment this OOM test out as it's time spending so not good for overnight build tests

/**
@file 
@SYMTestCaseID PIM-TBUGFIX-Versit_Escaping_Test-0001
@SYMTestCaseDesc Importing and exporting of VCards and VCals of different formats.
@SYMTestStatus Implemented
@SYMTestPriority 3
@SYMTestActions Imports pre-existing VCard and VCal files, exports them and than compares their output with a pre-existing VCard or VCal file containing the expected output.
@SYMTestExpectedResults KErrNone, all outputed VCards and VCals are identical with expected output.
@SYMTestType CT
@SYMPREQ 234
*/
class TParserSetUp
	{
public:
	enum TSyncMLTest {eSyncMLTest};
public:
	inline TParserSetUp(TUint aType): iType(aType), iCharset(Versit::EUSAsciiCharSet), iEncoding(Versit::ENoEncoding),
		iAutoDetect(EFalse), iInternaliseFlags(0), iTimeFlags(0) {}
	inline TParserSetUp(TUint aType,Versit::TVersitCharSet aCharSet,Versit::TVersitEncoding aEncoding)
		: iType(aType), iCharset(aCharSet), iEncoding(aEncoding), iAutoDetect(EFalse), iInternaliseFlags(0), iTimeFlags(0) {}
	inline TParserSetUp(TUint aType,TBool aAutoDetect): iType(aType), iCharset(Versit::EUSAsciiCharSet), iEncoding(Versit::ENoEncoding),
		iAutoDetect(aAutoDetect), iInternaliseFlags(0), iTimeFlags(0) {}
	inline TParserSetUp(TSyncMLTest);
public:
	TUint iType;
	Versit::TVersitCharSet iCharset;
	Versit::TVersitEncoding iEncoding;
	TBool iAutoDetect;
	TUint iInternaliseFlags;
	TUint iTimeFlags;
	};

class CTests : public CBase
	{
public:
	enum TParserType
		{
		ETvCard,
		ETvCal,
		ETuseBaseParser=0x08,
		//Masks
		ETparserType=ETuseBaseParser-1,
		};
public:
	static CTests* NewL();
	static TBool CheckRun();
	~CTests();
	void ConstructL();
	void AllTestsL();
	
	static void TestL(CTests& aSelf);
	static void LeaveTestsL(CTests& aSelf);
	static void TestJapanL(CTests& aSelf);
	static void SyncMLTestsL(CTests& aSelf);
	static void Test0L(CTests& aSelf);
	static void Test1L(CTests& aSelf);
	static void Test2L(CTests& aSelf);
	static void Test3L(CTests& aSelf);
	static void Test4L(CTests& aSelf);
	static void Test5L(CTests& aSelf);
	static void Test6L(CTests& aSelf);
	static void Test7L(CTests& aSelf);
	static void Test8L(CTests& aSelf);
	static void Test9L(CTests& aSelf);
	static void Test10L(CTests& aSelf);
	static void Test11L(CTests& aSelf);
	static void Test12L(CTests& aSelf);

	static void TestHugeVCardL(CTests& aSelf);
	static void TestNullDateL(CTests& aSelf);
	static void TestAlarmRepeatCountL(CTests& aSelf);
	static void TestHashSignEncodingL(CTests& aSelf);
	static void TestBase64ExtensionL(CTests& aSelf);
private:
	void ReportErrorL(TInt aErr,const TDesC&aFileName);
	void CreateFileName(TParserType aType,TFileName& aName,TChar aPrefix,const TDesC& aFile,TInt aPath=1);
	void ExistingFileName(TParserType aType,TFileName& aName,TChar aPrefix,const TDesC& aFile,TInt aPath=1);
	void SetTimeFlags(CVersitParser& aParser,TUint aTimeFlags);
	void IntAndExtL(const TFileName& aInputFile,const TFileName& aOutputFile,const TParserSetUp& aSetUp);
	void TestFileL(TUint aType,const TDesC& aTestName,const TDesC& aFile,Versit::TVersitCharSet aCharset=Versit::EUSAsciiCharSet
																				,Versit::TVersitEncoding aEncoding=Versit::ENoEncoding, TBool aCompareFile = ETrue);
	void TestFileL(const TDesC& aTestName,const TDesC& aFile,const TParserSetUp& aSetUp,TBool aCompareFile = ETrue);
	CVersitParser* CreateParserLC(TParserType aType,const TDesC& aTestName);
	void DoLeaveTestL(TParserType aType,TInt aLeaveCode,const TDesC& aTestName,const TDesC& aFile);
	void DoOomTestL(TParserType aType,TInt aFailAfter,const TDesC& aTestName,const TDesC& aFile);
	void DoJapanTestL(const TDesC& aTestName,TInt aTest,TParserType aType=ETvCard);
	void DoSyncMLTestL(TTimeIntervalSeconds aUTCOffset,const TDesC& aTestName,const TDesC& aFile);
	void ExternaliseFileL(CVersitParser& aParser,const TDesC& aFileName);
	void CreateAndExternaliseVCardL(const TDesC8& aVCardText,const TDesC& aFileName,Versit::TVersitEncoding aEncoding=Versit::ENoEncoding);
	void InternaliseFileL(CVersitParser& aParser,const TDesC& aFileName,TBool aUseBaseParser=EFalse);
	void InternalizeFileL(CVersitParser& aParser,RFile& aInputFile);
	void CompareFilesL(const TDesC& aOutputFile,const TDesC& aCompareFile,const TDesC& aInputFile=KNullDesC);
	void LineWrappingTestL(const TDesC& aFullFileName);
private:
	enum {eSecondsPerHour=3600};
private:
	RFs iFsSession;
	TBool iSessionConnected;
	CFileMan* iFm;
	CTestRegister* iTempFiles;
	CTestVersitExtension* iBase64Extension;
	};
void doOOMTest(CTests& aTest, TOOMCallback aCallback, TInt aFailStart);

GLREF_C void ResetAndDestroyArrayOfParams(TAny* aObject)
	{
	CArrayPtr<CParserParam>* array=REINTERPRET_CAST(CArrayPtr<CParserParam>*,aObject);
	if (array)
		array->ResetAndDestroy();
	delete array;
	}

GLREF_C void ResetAndDestroyArrayOfProperties(TAny* aObject)
	{
	CArrayPtr<CParserProperty>* array=REINTERPRET_CAST(CArrayPtr<CParserProperty>*,aObject);
	if (array)
		array->ResetAndDestroy();
	delete array;
	}


inline TParserSetUp::TParserSetUp(TSyncMLTest): iType(CTests::ETvCal), iCharset(Versit::EUSAsciiCharSet), iEncoding(Versit::ENoEncoding),
	iAutoDetect(EFalse), iInternaliseFlags(CVersitParser::EImportSyncML), iTimeFlags(TVersitDateTime::EExportLeaveAsLocalTime) {}


CTests* CTests::NewL()
	{
	CTests* self=new(ELeave) CTests;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

TBool CTests::CheckRun()
	{
	TInt key=TheTest.Getch();
	if (key==EKeySpace)
		return ETrue;
	_LIT(KNot,"Not running this test\r\n");
	TheTest.Printf(KNot);
	return EFalse;
	}

void CTests::ConstructL()
	{
	User::LeaveIfError(iFsSession.Connect());
	iSessionConnected=ETrue;
	iFm = CFileMan::NewL(iFsSession);
	iTempFiles = CTestRegister::NewLC();
	CleanupStack::Pop(iTempFiles);
	iFsSession.MkDir(_L("C:\\TestvCards\\"));
	iFsSession.MkDir(_L("C:\\TestFiles\\"));
	iFsSession.MkDir(_L("C:\\K3Data\\"));
	}

CTests::~CTests()
	{
	delete(iTempFiles);

	if(iFm)
		{
		iFm->RmDir(_L("C:\\TestvCards\\"));
		iFm->RmDir(_L("C:\\TestFiles\\"));
		iFm->RmDir(_L("C:\\K3Data\\"));
		delete iFm;
		}

	if (iSessionConnected)
		{
		iFsSession.Close();
		}
	delete iBase64Extension;
	}

void CTests::ReportErrorL(TInt aErr,const TDesC& aFileName)
	{
	if (aErr==KErrNotFound)
		{
		_LIT(KMissingFile,"\r\n** FILE NOT FOUND: %S\r\n\r\n");
		TheTest.Printf(KMissingFile,&aFileName);
		}
	else if (aErr==KErrPathNotFound)
		{	//You will get this error if you have not exported the test data files.
		_LIT(KMissingPath,"\r\n** PATH NOT FOUND: %S\r\n\r\n");
		TheTest.Printf(KMissingPath,&aFileName);
		}
	else
		{
		_LIT(KError,"Error %d opening file %S\r\n");
		TheTest.Printf(KError,aErr,&aFileName);
		}
	__DEBUGGER();
	User::Leave(aErr);
	}

void CTests::ExternaliseFileL(CVersitParser& aParser,const TDesC& aFileName)
	{
	RFile file;
	User::LeaveIfError(file.Replace(iFsSession, aFileName, EFileWrite));
	CleanupClosePushL(file);
	aParser.ExternalizeL(file);
	CleanupStack::PopAndDestroy(&file);
	}

void CTests::CreateAndExternaliseVCardL(const TDesC8& aVCardText,const TDesC& aFileName,Versit::TVersitEncoding aEncoding/*=ENoEncoding*/)
	{
	CParserVCard* vCardParser=CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);

	RDesReadStream stream(aVCardText);
	vCardParser->InternalizeL(stream);

	vCardParser->SetDefaultEncoding(aEncoding);
	ExternaliseFileL(*vCardParser,aFileName);

	CleanupStack::PopAndDestroy(vCardParser);
	}

void CTests::InternaliseFileL(CVersitParser& aParser,const TDesC& aFileName,TBool aUseBaseParser/*=EFalse*/)
	{
	RFile file;
	TInt err=file.Open(iFsSession, aFileName, EFileRead);
	if (err!=KErrNone)
		ReportErrorL(err,aFileName);
	CleanupClosePushL(file);
	TInt start = 0;
	
	if (iBase64Extension)
		{
		aParser.SetPlugIn(iBase64Extension);
		}

	if (aUseBaseParser)
		InternalizeFileL(aParser,file);
	else
		aParser.InternalizeL(file, start);
	CleanupStack::PopAndDestroy(&file);
	}

void CTests::InternalizeFileL(CVersitParser& aParser,RFile& aInputFile)
	{
	TInt size;
	if (aInputFile.Size(size)==KErrNone)
		{
		RFileReadStream stream(aInputFile);
		TRAPD(err,aParser.CVersitParser::InternalizeL(stream));
		STATIC_CAST(RFileBuf*,stream.Source())->Reset();
		stream.Close();
		User::LeaveIfError(err);
		}
	}

LOCAL_C TBool CompareLines(const TDesC8& aLine1, const TDesC8& aLine2)
	{
	_LIT8(KDayl,"DAYL");
	_LIT8(KTzPlus,"TZ:+");
	_LIT8(KTzMinus,"TZ:-");

	TBool ret=!aLine1.Compare(aLine2);
	if (!ret)
		{
		TBufC8<4> first4char(aLine1.Left(4));
		if(!first4char.Compare(KDayl())||!first4char.Compare(KTzPlus())||!first4char.Compare(KTzMinus()))
			{
			return ETrue;
			}
		}
	return ret;
	}


void CTests::CompareFilesL(const TDesC& aOutputFile,const TDesC& aCompareFile,const TDesC& aInputFile)
	{
	RFile file1,file2;
	TInt err=file1.Open(iFsSession, aOutputFile, EFileRead);
	if (err!=KErrNone)
		ReportErrorL(err,aOutputFile);
	CleanupClosePushL(file1);
	err=file2.Open(iFsSession, aCompareFile, EFileRead);
	if (err==KErrNotFound)//in this case, the input file and output file should be identical
		{
		err=file2.Open(iFsSession, aInputFile, EFileRead);
		if (err!=KErrNone)
			ReportErrorL(err,aInputFile);
		}
	if (err!=KErrNone)
		ReportErrorL(err,aCompareFile);
	CleanupClosePushL(file2);
	RFileReadStream stream1(file1);
	RFileReadStream stream2(file2);
	CleanupClosePushL(stream1);
	CleanupClosePushL(stream2);
	TBuf8<0x80> bufO,bufC;
	//__ASSERT(err==KErrNone);
	TInt line=1;
	do {
		TRAP(err,stream1.ReadL(bufO,TChar(0xa)));
		if (err==KErrNone || err==KErrEof)
			TRAP(err,stream2.ReadL(bufC,TChar(0xa)));
		if (err!=KErrNone && err!=KErrEof)
			User::Leave(err);
		TheTest(CompareLines(bufO, bufC));
		++line;
		} while (err!=KErrEof);
	CleanupStack::PopAndDestroy(4,&file1);
	}

void CTests::CreateFileName(TParserType aType,TFileName& aName,TChar aPrefix,const TDesC& aFile,TInt aPath/*=1*/)
	{
	_LIT(KPrefix1,"C:\\TestFiles\\");
	_LIT(KPrefix2,"C:\\K3Data\\");
	_LIT(KvCard,".VCF");
	_LIT(KvCal,".VCS");
	if (aPath==1)
		aName=KPrefix1;
	else
		aName=KPrefix2;
	if (aPrefix!=' ')
		aName.Append(aPrefix);
	aName.Append(aFile);
	if (aType==ETvCal)
		aName.Append(KvCal);
	else
		aName.Append(KvCard);
	}

void CTests::ExistingFileName(TParserType aType,TFileName& aName,TChar aPrefix,const TDesC& aFile,TInt aPath/*=1*/)
	{
	_LIT(KePrefix1,"Z:\\TestFiles\\");
	_LIT(KePrefix2,"Z:\\K3Data\\");
	_LIT(KvCard,".VCF");
	_LIT(KvCal,".VCS");
	if (aPath==1)
		aName=KePrefix1;
	else
		aName=KePrefix2;
	if (aPrefix!=' ')
		aName.Append(aPrefix);
	aName.Append(aFile);
	if (aType==ETvCal)
		aName.Append(KvCal);
	else
		aName.Append(KvCard);
	}

void CTests::SetTimeFlags(CVersitParser& aParser,TUint aTimeFlags)
	{
	CArrayPtr<CParserProperty>* propertiesArray=aParser.ArrayOfProperties(EFalse);
	TInt count;
	TInt ii;
	if (propertiesArray)
		{
		CParserTimePropertyValue* timeValue;
		CParserPropertyValue* value;
		count=propertiesArray->Count();
		TVersitDateTime::TVersitDateTimeFlags timeFlags=REINTERPRET_CAST(TVersitDateTime::TVersitDateTimeFlags&,aTimeFlags);
		TUid uid;
		uid.iUid = KVersitTimePropertyUid;
		for (ii=0; ii<count; ii++)
			{
			value=(*propertiesArray)[ii]->Value();
			if (value && value->SupportsInterface(uid))
				{
				timeValue=STATIC_CAST(CParserTimePropertyValue*,value);
				switch (timeValue->Uid().iUid)
					{
				case KVCalPropertyAlarmUid:
					{
					CVersitAlarm* alarm=STATIC_CAST(CParserPropertyValueAlarm*,timeValue)->Value();
					if (alarm && alarm->iRunTime)
						{
						alarm->iRunTime->ClearFlag(TVersitDateTime::EExportTimeZoneDesignator);
						alarm->iRunTime->SetFlag(timeFlags);
						}
					}
					break;
				case KVersitPropertyDaylightUid:
					{
					CVersitDaylight* daylight=STATIC_CAST(CParserPropertyValueDaylight*,timeValue)->Value();
					if (daylight)
						{
						if (daylight->iStartTime)
							{
							daylight->iStartTime->ClearFlag(TVersitDateTime::EExportTimeZoneDesignator);
							daylight->iStartTime->SetFlag(timeFlags);
							}
						if (daylight->iEndTime)
							{
							daylight->iEndTime->ClearFlag(TVersitDateTime::EExportTimeZoneDesignator);
							daylight->iEndTime->SetFlag(timeFlags);
							}
						}
					}
					break;
				case KVersitPropertyDateTimeUid:
					{
					TVersitDateTime* dateTime=STATIC_CAST(CParserPropertyValueDateTime*,timeValue)->Value();
					if (dateTime)
						{
						dateTime->ClearFlag(TVersitDateTime::EExportTimeZoneDesignator);
						dateTime->SetFlag(timeFlags);
						}
					}
					break;
				case KVersitPropertyDateUid:
					{
					TVersitDateTime* dateTime=STATIC_CAST(CParserPropertyValueDate*,timeValue)->Value();
					if (dateTime)
						{
						dateTime->ClearFlag(TVersitDateTime::EExportTimeZoneDesignator);
						dateTime->SetFlag(timeFlags);
						}
					}
					break;
				case KVersitPropertyMultiDateTimeUid:
					{
					CArrayPtr<TVersitDateTime>* values=STATIC_CAST(CParserPropertyValueMultiDateTime*,timeValue)->Value();
					if (values)
						{
						TInt count=values->Count();
						for (TInt ii=0;ii<count; ii++)
							{
							(*values)[ii]->ClearFlag(TVersitDateTime::EExportTimeZoneDesignator);
							(*values)[ii]->SetFlag(timeFlags);
							}
						}
					}
					break;
				case KVCalPropertyRecurrenceUid:
					{
					CVersitRecurrence* recurrence=STATIC_CAST(CParserPropertyValueRecurrence*,timeValue)->Value();
					if (recurrence && recurrence->iEndDate)
						{
						recurrence->iEndDate->ClearFlag(TVersitDateTime::EExportTimeZoneDesignator);
						recurrence->iEndDate->SetFlag(timeFlags);
						}
					}
					break;
					}
				}
			}
		}
	CArrayPtr<CVersitParser>* entityArray=aParser.ArrayOfEntities(EFalse);
	if (entityArray)
		{
		count=entityArray->Count();
		for (ii=0; ii<count; ii++)
			SetTimeFlags(*(*entityArray)[ii],aTimeFlags);
		}
	}

void CTests::IntAndExtL(const TFileName& aInputFile,const TFileName& aOutputFile,const TParserSetUp& aSetUp)
	{
	CVersitParser* vParser;
	if ((aSetUp.iType&ETparserType)==ETvCal)
		{
		vParser=CParserVCal::NewL();
		if (aSetUp.iInternaliseFlags)
			vParser->SetFlags(aSetUp.iInternaliseFlags);
		}
	else
		vParser=CParserVCard::NewL();
	vParser->SetAutoDetect(aSetUp.iAutoDetect);
	CleanupStack::PushL(vParser);
	InternaliseFileL(*vParser,aInputFile,aSetUp.iType&ETuseBaseParser);
	vParser->SetDefaultCharSet(aSetUp.iCharset);
	vParser->SetDefaultEncoding(aSetUp.iEncoding);
	if (aSetUp.iTimeFlags)
		SetTimeFlags(*vParser,aSetUp.iTimeFlags);
	ExternaliseFileL(*vParser,aOutputFile);
	CleanupStack::PopAndDestroy(vParser);
	}

void CTests::TestFileL(TUint aType,const TDesC& aTestName,const TDesC& aFile
				,Versit::TVersitCharSet aCharset/*=Versit::EUSAsciiCharSet*/,Versit::TVersitEncoding aEncoding/*=Versit::ENoEncoding*/, TBool aCompareFile)
	{
	User::SetUTCOffset(TTimeIntervalSeconds(0));//make sure it is GMT
	TParserSetUp setUp(aType,aCharset,aEncoding);
	TestFileL(aTestName,aFile,setUp,aCompareFile);
	}

void CTests::TestFileL(const TDesC& aTestName,const TDesC& aFile,const TParserSetUp& aSetUp, TBool aCompareFile )
	{
	TheTest.Next(aTestName);
	TParserType type=STATIC_CAST(TParserType,aSetUp.iType&ETparserType);
	TFileName inputFile,outputFile,compareFile;
	ExistingFileName(type,inputFile,'I',aFile);
	CreateFileName(type,outputFile,'O',aFile);
	ExistingFileName(type,compareFile,'C',aFile);
	IntAndExtL(inputFile,outputFile,aSetUp);
	if(aCompareFile)
		{
		CompareFilesL(outputFile,compareFile,inputFile);
		}
	}

CVersitParser* CTests::CreateParserLC(TParserType aType,const TDesC& aTestName)
	{
	TheTest.Next(aTestName);
	CVersitParser* vParser;
	if (aType==ETvCal)
		vParser=CParserVCal::NewL();
	else
		vParser=CParserVCard::NewL();
	CleanupStack::PushL(vParser);
	return vParser;
	}

void CTests::DoLeaveTestL(TParserType aType,TInt aLeaveCode,const TDesC& aTestName,const TDesC& aFile)
	{
	CVersitParser* vParser=CreateParserLC(aType,aTestName);
	TFileName inputFile;
	ExistingFileName(aType,inputFile,' ',aFile);
	TRAPD(err,InternaliseFileL(*vParser,inputFile));
	TheTest(err==aLeaveCode);
	CleanupStack::PopAndDestroy(vParser);
	}

#ifdef _DEBUG
void CTests::DoOomTestL(TParserType aType,TInt aFailAfter,const TDesC& aTestName,const TDesC& aFile)
#else
void CTests::DoOomTestL(TParserType aType,TInt /*aFailAfter*/,const TDesC& aTestName,const TDesC& aFile)
#endif
	{
	CVersitParser* vParser=CreateParserLC(aType,aTestName);
	TFileName inputFile;
	ExistingFileName(aType,inputFile,' ',aFile);
	__UHEAP_SETFAIL(RHeap::EDeterministic,aFailAfter);
	TRAPD(err,InternaliseFileL(*vParser,inputFile));
	__UHEAP_RESET;
	//++aFailAfter;
	TheTest(err==KErrNoMemory);
	CleanupStack::PopAndDestroy(vParser);
	}

void CTests::DoJapanTestL(const TDesC& aTestName,TInt aTest,TParserType aType/*=ETvCard*/)
	{
	TheTest.Next(aTestName);
	TBuf<16> file;
	_LIT(KFile,"Test%02d");
	file.Format(KFile,aTest);
	TFileName inputFile,outputFile1,storkFile,outputFile2;
	ExistingFileName(aType,inputFile,'I',file,2);
	CreateFileName(aType,outputFile1,'O',file,2);
	ExistingFileName(aType,storkFile,'S',file,2);
	CreateFileName(aType,outputFile2,'C',file,2);
	TParserSetUp setUp(aType);
	IntAndExtL(inputFile,outputFile1,setUp);
	IntAndExtL(storkFile,outputFile2,setUp);
	CompareFilesL(outputFile1,outputFile2);
	}

void CTests::DoSyncMLTestL(TTimeIntervalSeconds aUTCOffset,const TDesC& aTestName,const TDesC& aFile)
	{
	User::SetUTCOffset(aUTCOffset);
	
	TParserSetUp setUp(TParserSetUp::eSyncMLTest);
	TestFileL(aTestName,aFile,setUp);
	}

void CTests::Test0L(CTests& aSelf)
	{
	_LIT8(KEscapedText, "BEGIN:VCARD\r\nVERSION:2.1\r\nEMAIL:def\r\nEND:VCARD\r\n");
	CParserVCard* vCardParser=CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);

	RDesReadStream stream(KEscapedText);
	vCardParser->InternalizeL(stream);

#ifndef TBUGFIX_INCLUDE_OOM
	aSelf.iTempFiles->RegisterL(KSimpleFileName());
#endif	//TBUGFIX_INCLUDE_OOM

	aSelf.ExternaliseFileL(*vCardParser,KSimpleFileName);

	CleanupStack::PopAndDestroy(vCardParser);
	}

void CTests::Test1L(CTests& aSelf)
	{
	_LIT8(KEscapedText, "BEGIN:VCARD\r\nVERSION:2.1\r\nEMAIL:a\\b\r\nEND:VCARD\r\n");
	
#ifndef TBUGFIX_INCLUDE_OOM
	aSelf.iTempFiles->RegisterL(KOutEscFileName);
#endif	//TBUGFIX_INCLUDE_OOM

	aSelf.CreateAndExternaliseVCardL(KEscapedText,KOutEscFileName);

	CParserVCard* vCardParser=CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);

	aSelf.InternaliseFileL(*vCardParser,KOutEscFileName);

	TUid valueTypeUid = { KVersitPropertyHBufCUid };
	CArrayPtr<CParserProperty>* array = vCardParser->PropertyL(_L8("EMAIL"), valueTypeUid, EFalse);
	CleanupStack::PushL(array);
	TheTest(array->Count() == 1);
	CParserPropertyValueHBufC* value = static_cast<CParserPropertyValueHBufC*>(array->At(0)->Value());
	TheTest(value->Value() == _L("a\\b"));
	CleanupStack::PopAndDestroy(array);

	CleanupStack::PopAndDestroy(vCardParser);
	}

/**
@SYMTestCaseID PIM-TBUGFIX-PDEF109115-0001
@SYMTestCaseDesc Codebase-Versit: Add test to Quoted Printable encoding of properties with CRLF
@SYMTestStatus Implemented
@SYMTestPriority High
@SYMTestType UT
@SYMTestActions
 1)Save VCard test file
 2)Create a VCard and export it
 3) Compare the VCard test file with the exported VCard  file 

@SYMTestExpectedResults The address property uses Quoted Printable correctly.
@SYMDEF PDEF109115
@see DEF093368
*/
void CTests::Test12L(CTests& aSelf)
	{

#ifndef TBUGFIX_INCLUDE_OOM
	aSelf.iTempFiles->RegisterL(KQPFileName);
	aSelf.iTempFiles->RegisterL(KOutputQPFileName);
#endif	//TBUGFIX_INCLUDE_OOM
 
	//Test VCard file	  
	_LIT8(KVCardData,	"BEGIN:VCARD\r\n"
						"VERSION:2.1\r\n"
						"ADR;HOME;ENCODING=QUOTED-PRINTABLE:;;Address=20line1=0D=0Aaddress=20line2\r\n"
						"END:VCARD\r\n");
	
	//Save the test VCard file
	TheTest.Printf(_L("@SYMTestCaseID PIM-TBUGFIX-PDEF109115-0001  Save test Vcard file to be compared with exported vcard file later\n"));					
	RFile testFile;
	User::LeaveIfError(testFile.Replace(aSelf.iFsSession,KQPFileName,EFileWrite ));
	testFile.Write(KVCardData);
	testFile.Close();
	
	
	//Create a VCard						
	CParserVCard* vCardParser=CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);

	//Create an address value
	_LIT(KAddrValue, "Address line1\r\naddress line2");
	CDesCArrayFlat* addrArray = new (ELeave) CDesCArrayFlat(3);
	CleanupStack::PushL(addrArray);
	addrArray->AppendL(KNullDesC);
	addrArray->AppendL(KNullDesC);
	addrArray->AppendL(KAddrValue);  
	
	CParserPropertyValue* addrValue=new (ELeave) CParserPropertyValueCDesCArray(addrArray);
  	CleanupStack::Pop(addrArray);
  
	CleanupStack::PushL(addrValue);
	
	// Create a property parameter "HOME"
	CArrayPtr<CParserParam>* arrayOfParams = new(ELeave)CArrayPtrFlat<CParserParam>(1);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfParams,arrayOfParams));

	CParserParam* parserParam=CParserParam::NewL(_L8("HOME"),KNullDesC);
	CleanupStack::PushL(parserParam);
    arrayOfParams->AppendL(parserParam);
	CleanupStack::Pop(parserParam);
	
	//Create an ADR property 
	CParserProperty* property=CParserProperty::NewL(*addrValue,KVersitTokenADR,arrayOfParams);

	CleanupStack::Pop(2, addrValue);

	//Add the ADR property to the VCard
	CleanupStack::PushL(property);
	vCardParser->AddPropertyL(property);
	CleanupStack::Pop(property);

	//Export the VCard
	aSelf.ExternaliseFileL(*vCardParser,KOutputQPFileName);

	CleanupStack::PopAndDestroy(vCardParser);

	//Compare the VCard exported with the earlier saved VCard test file
	aSelf.CompareFilesL(KQPFileName, KOutputQPFileName);	
}

void CTests::Test2L(CTests& aSelf)
	{
	_LIT8(KVCardData, "BEGIN:VCARD\r\nVERSION:2.1\r\nN;ENCODING=QUOTED-PRINTABLE:=3D=3D=3D\r\nEND:VCARD\r\n");

#ifndef TBUGFIX_INCLUDE_OOM
	aSelf.iTempFiles->RegisterL(KOutputFileName);
#endif	//TBUGFIX_INCLUDE_OOM

	aSelf.CreateAndExternaliseVCardL(KVCardData,KOutputFileName);

	CParserVCard* vCardParser=CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);

	aSelf.InternaliseFileL(*vCardParser,KOutputFileName);

	const TUid valueTypeUid = { KVersitPropertyCDesCArrayUid };
	CArrayPtr<CParserProperty>* array = vCardParser->PropertyL(_L8("N"), valueTypeUid, EFalse);
	CleanupStack::PushL(array);
	TheTest(array->Count() == 1);
	CParserPropertyValueCDesCArray* value = static_cast<CParserPropertyValueCDesCArray*>(array->At(0)->Value());
	TheTest(value->Value()->Count() == 1);
	const TPtrC pValue(value->Value()->MdcaPoint(0));
	TheTest(pValue == _L("==="));
	CleanupStack::PopAndDestroy(array);

	CleanupStack::PopAndDestroy(vCardParser);
	}

void CTests::Test3L(CTests& aSelf)
	{
	_LIT(KFirstName,"\x3046\x3053\x3087"); //hiragana 'ukyo'
	_LIT(KSurname,"\x3084\x307e\x3060"); //hiragana 'yamada'
	CParserVCard* vCardParser=CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);

#ifndef TBUGFIX_INCLUDE_OOM
	aSelf.iTempFiles->RegisterL(KVCardFileShiftJIS);
	aSelf.iTempFiles->RegisterL(KVCardFileJIS);
#endif	//TBUGFIX_INCLUDE_OOM

	CDesCArrayFlat* desArray = new (ELeave) CDesCArrayFlat(5);
	CleanupStack::PushL(desArray);
	desArray->AppendL(KSurname);  //family name
	desArray->AppendL(KFirstName);//given name
	desArray->AppendL(KNullDesC); //additional names
	desArray->AppendL(KNullDesC); //prefix
	desArray->AppendL(KNullDesC); //suffix
	CParserPropertyValue* value=new (ELeave) CParserPropertyValueCDesCArray(desArray);
  	CleanupStack::Pop(desArray);
	CleanupStack::PushL(value);
	// add a character set property parameter
	CArrayPtr<CParserParam>* arrayOfParams = new(ELeave)CArrayPtrFlat<CParserParam>(5);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfParams,arrayOfParams));
	CParserParam* parserParam=CParserParam::NewL(KVersitTokenCHARSET,KVersitTokenShiftJIS);
	CleanupStack::PushL(parserParam);
    arrayOfParams->AppendL(parserParam);
	CleanupStack::Pop(parserParam);
	CParserGroupedProperty* property=CParserGroupedProperty::NewL(*value,KVersitTokenN,NULL,arrayOfParams);
	CleanupStack::Pop(2,value); //value, arrayOfParams
	vCardParser->AddPropertyL(property);

	vCardParser->SetDefaultCharSet(Versit::EShiftJISCharSet);
	aSelf.ExternaliseFileL(*vCardParser,KVCardFileShiftJIS);
	vCardParser->SetDefaultCharSet(Versit::EJISCharSet);
	aSelf.ExternaliseFileL(*vCardParser,KVCardFileJIS);
	CleanupStack::PopAndDestroy(vCardParser);

	TInt ii;
	for (ii=2;ii>0;--ii)
		{
		vCardParser=CParserVCard::NewL();
		CleanupStack::PushL(vCardParser);
		aSelf.InternaliseFileL(*vCardParser,(ii==1? KVCardFileShiftJIS:KVCardFileJIS));
		CArrayPtr<CParserProperty>* properties=vCardParser->PropertyL(KVersitTokenN,TUid::Uid(KVersitPropertyCDesCArrayUid));
  		if (properties)
			{
			CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfProperties,properties));
      		CParserProperty* property = (*properties)[0];
			CParserPropertyValueCDesCArray* value = STATIC_CAST(CParserPropertyValueCDesCArray*,property->Value());
			CDesCArray* array = value->Value();
			const TInt count = array->MdcaCount();
			TheTest(count>2);
			//TPtrC ptr = array->MdcaPoint(0);
			TheTest(array->MdcaPoint(0).Compare(KSurname)==0);
			TheTest(array->MdcaPoint(1).Compare(KFirstName)==0);
			CleanupStack::PopAndDestroy(properties);
			}
		CleanupStack::PopAndDestroy(vCardParser);
		}
	}

void CTests::Test4L(CTests& aSelf)
	{
	_LIT8(KVCardData, "BEGIN:VCARD\r\nVERSION:2.1\r\nN:Some\\Escaped\\Text\r\nEND:VCARD\r\n\r\n");

#ifndef TBUGFIX_INCLUDE_OOM
	aSelf.iTempFiles->RegisterL(KBase64FileName);
#endif	//TBUGFIX_INCLUDE_OOM

	aSelf.CreateAndExternaliseVCardL(KVCardData,KBase64FileName,Versit::EBase64Encoding);

	CParserVCard* vCardParser=CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);

	aSelf.InternaliseFileL(*vCardParser,KBase64FileName);

	const TUid valueTypeUid = { KVersitPropertyCDesCArrayUid };
	CArrayPtr<CParserProperty>* array = vCardParser->PropertyL(_L8("N"), valueTypeUid, EFalse);
	CleanupStack::PushL(array);
	TheTest(array->Count() == 1);
	CParserPropertyValueCDesCArray* value = static_cast<CParserPropertyValueCDesCArray*>(array->At(0)->Value());
	TheTest(value->Value()->Count() == 1);
	const TPtrC pValue(value->Value()->MdcaPoint(0));
	TheTest(pValue == _L("Some\\Escaped\\Text"));
	//
	CleanupStack::PopAndDestroy(array);

	CleanupStack::PopAndDestroy(vCardParser);
	}


void CTests::Test5L(CTests& aSelf)
	{
	_LIT(KInputFileName,"Z:\\testfiles\\Test5_ASCII_Input.vcf");
	_LIT(KOutputFileName,"C:\\testfiles\\Test5_Base64_Output.vcf");
	_LIT(KCompareFileName,"C:\\testfiles\\Test5_ASCII_Output.vcf");

	CParserVCard* vParser=CParserVCard::NewL();
	CleanupStack::PushL(vParser);
	aSelf.InternaliseFileL(*vParser,KInputFileName);
	vParser->SetDefaultEncoding(Versit::EBase64Encoding);
	aSelf.ExternaliseFileL(*vParser,KOutputFileName);
	vParser->SetDefaultEncoding(Versit::ENoEncoding);
	aSelf.ExternaliseFileL(*vParser,KCompareFileName);
	CleanupStack::PopAndDestroy(vParser);
	aSelf.CompareFilesL(KCompareFileName,KInputFileName,KNullDesC);
	}


// This test also verifies that PREQ1131 requirements are met:
// (1) Comprise one or more lines where one or more of the lines is longer than 76 characters (eg a URL) 
// (2) Comprise multiple lines which have been folded without any LWSP characters following the CRLF line break 
void CTests::Test6L(CTests& aSelf)
	{
	//Line wrapping in Ascii
	_LIT(KTestAscii,"Line Wrapping (WAG-4Z8D8C)");// bug fix
	_LIT(KFileAscii,"AsciiLineFolding");
	//compare files to make sure the break at right point
	aSelf.TestFileL(ETvCard,KTestAscii,KFileAscii);
	_LIT(KFileInputAscii,"C:\\TestFiles\\OAsciiLineFolding.VCF");
	//internalise the output and check the property value to make sure the value has been read back right
	aSelf.LineWrappingTestL(KFileInputAscii);

	//Line wrapping in Base64 encoding
	_LIT(KTestBase64,"Line Wrapping in base64");
	_LIT(KFileBase64,"Base64LineFolding");
	aSelf.TestFileL(ETvCard,KTestBase64,KFileBase64,Versit::EUSAsciiCharSet,Versit::EBase64Encoding);
	_LIT(KFileInputBase64,"C:\\TestFiles\\oBase64LineFolding.VCF");
	aSelf.LineWrappingTestL(KFileInputBase64);
	
	//Test line wrapping in Base64 encoding where there are no whitespace at the beginning of each new line in the vcard
	//The TestFileL() test is omitted as the generated file will create the whitespace by default
	_LIT(KFileInputBase64NoSpace,"Z:\\TestFiles\\cBase64NoSpaceLineFolding.VCF");
	aSelf.LineWrappingTestL(KFileInputBase64NoSpace);
	
	//Line wrapping in QP encoding
	_LIT(KTestQP,"Line Wrapping in base64");
	_LIT(KFileQP,"QPLineFolding");
	aSelf.TestFileL(ETvCard,KTestQP,KFileQP,Versit::EUSAsciiCharSet,Versit::EQuotedPrintableEncoding);
	_LIT(KFileInputQP,"C:\\TestFiles\\oQPLineFolding.VCF");
	aSelf.LineWrappingTestL(KFileInputQP);
	}

void CTests::LineWrappingTestL(const TDesC& aFullFileName)
	{
	//The following line is extracted from the original file "iAsciiLineFolding.vcf"
	_LIT(KNoteText,"This is a very long description that exists on along line. Thisisaverylongdescriptionthatexistsonalongline.aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaThisisaverylong... It breaks here.");
	CParserVCard* vCardParser=CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);
	InternaliseFileL(*vCardParser,aFullFileName);
	CArrayPtr<CParserProperty>* properties=vCardParser->PropertyL(KVersitTokenNOTE,TUid::Uid(KVersitPropertyHBufCUid));
  	if (properties)
		{
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfProperties,properties));
       	CParserProperty* property = (*properties)[0];
		CParserPropertyValueHBufC* value = STATIC_CAST(CParserPropertyValueHBufC*,property->Value());
		TPtrC externalisedNote(value->Value());
		TheTest(externalisedNote.Compare(KNoteText)==0);
		CleanupStack::PopAndDestroy(properties);
		}

	CleanupStack::PopAndDestroy(vCardParser);
	}

void CTests::Test7L(CTests& aSelf)
	{
	_LIT(KInputFileName,"Z:\\testfiles\\iTest7.vcf");
	_LIT(KOutputFileName,"C:\\testfiles\\oTest7.vcf");
	CParserVCard* vParser = CParserVCard::NewL();
	CleanupStack::PushL(vParser);
	aSelf.InternaliseFileL(*vParser,KInputFileName);
	aSelf.ExternaliseFileL(*vParser,KOutputFileName);
	CleanupStack::PopAndDestroy(vParser);
	}

void CTests::Test8L(CTests& aSelf)
	{
	_LIT(KInputFileName,"GroupedProperty");
	_LIT8(KPgroupNameA,"A");
	_LIT8(KPgroupNameB,"BBBB");

	_LIT(KTestGroupProperty,"Test grouped property");
	_LIT(KFileName,"GroupedProperty");
	aSelf.TestFileL(ETvCard,KTestGroupProperty,KFileName);

	CParserVCard* vParser = CParserVCard::NewL();
	CleanupStack::PushL(vParser);
	TFileName fullFileName;
	aSelf.ExistingFileName(ETvCard,fullFileName,'I',KInputFileName);
	aSelf.InternaliseFileL(*vParser,fullFileName);
    CArrayPtr<CParserProperty>* arrayOfGroupedPropertiesA=NULL;
	CArrayPtr<CParserProperty>* arrayOfGroupedPropertiesB=NULL;
	CParserGroupedProperty* property=NULL;

	arrayOfGroupedPropertiesA=vParser->GroupOfPropertiesL(KPgroupNameA);
	TInt count=arrayOfGroupedPropertiesA->Count();
	TheTest(count==2);

	for(TInt i=0;i<count;++i)
		{
		property=(CParserGroupedProperty*)(*arrayOfGroupedPropertiesA)[i];
		TheTest(property->Group(KPgroupNameA));
		TheTest(!property->Group(KPgroupNameB));
		}
	delete arrayOfGroupedPropertiesA;

	arrayOfGroupedPropertiesB=vParser->GroupOfPropertiesL(KPgroupNameB);
	count=arrayOfGroupedPropertiesB->Count();
	TheTest(count==3);
	for(TInt j=0;j<count;++j)
		{
		property=(CParserGroupedProperty*)(*arrayOfGroupedPropertiesB)[j];
		TheTest(property->Group(KPgroupNameB));
		TheTest(!property->Group(KPgroupNameA));
		}
	delete arrayOfGroupedPropertiesB;
	CleanupStack::PopAndDestroy(vParser);
	}

void CTests::Test9L(CTests& aSelf)
	{
	//create a vCard and externalise it
	_LIT(KNote,"\x4e79\x4f19\x5032"); //Jis code x3030 x305c x315c
	CParserVCard* vCardParser=CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);
	CParserPropertyValue* value=CParserPropertyValueHBufC::NewL(KNote);
	CleanupStack::PushL(value);
	// add a character set property parameter
	CArrayPtr<CParserParam>* arrayOfParams = new(ELeave)CArrayPtrFlat<CParserParam>(5);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfParams,arrayOfParams));
	CParserParam* parserParam=CParserParam::NewL(KVersitTokenCHARSET,KVersitTokenJIS);
	CleanupStack::PushL(parserParam);
    arrayOfParams->AppendL(parserParam);
	CleanupStack::Pop(parserParam);
	CParserGroupedProperty* property=CParserGroupedProperty::NewL(*value,KVersitTokenNOTE,NULL,arrayOfParams);
	CleanupStack::Pop(2,value); //value, arrayOfParams
	vCardParser->AddPropertyL(property);

#ifndef TBUGFIX_INCLUDE_OOM
	aSelf.iTempFiles->RegisterL(KVCardFileJIS);
#endif	//TBUGFIX_INCLUDE_OOM

	vCardParser->SetDefaultCharSet(Versit::EJISCharSet);
	aSelf.ExternaliseFileL(*vCardParser,KVCardFileJIS);
	CleanupStack::PopAndDestroy(vCardParser);
//internalise the vCard externalised and check the value to see if it is identical
	vCardParser=CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);
	aSelf.InternaliseFileL(*vCardParser,KVCardFileJIS);
	CArrayPtr<CParserProperty>* properties=vCardParser->PropertyL(KVersitTokenNOTE,TUid::Uid(KVersitPropertyHBufCUid));
  	if (properties)
		{
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfProperties,properties));
      	CParserProperty* property = (*properties)[0];
		CParserPropertyValueHBufC* value = STATIC_CAST(CParserPropertyValueHBufC*,property->Value());
		TPtrC note(value->Value());
		TheTest(note.Compare(KNote)==0);
		CleanupStack::PopAndDestroy(properties);
		}
	CleanupStack::PopAndDestroy(vCardParser);
	}

void CTests::Test10L(CTests& aSelf)
	{
	_LIT(KTest1,"vCard UTF-8");
	_LIT(KFile1,"AutoD");
	TParserSetUp setUp(ETvCard,ETrue);
	aSelf.TestFileL(KTest1,KFile1,setUp);
	_LIT(KTest2,"vCal Shift-JIS");
	_LIT(KFile2,"AutoD2");
	setUp.iType=ETvCal;
	setUp.iCharset=Versit::EShiftJISCharSet;
	aSelf.TestFileL(KTest2,KFile2,setUp);
	}


void CTests::Test11L(CTests& aSelf)
	{
	_LIT(KInputFileName,"Z:\\testvcards\\various\\6.vcs");
	CParserVCal* vParser = CParserVCal::NewL();
	CleanupStack::PushL(vParser);
	aSelf.InternaliseFileL(*vParser,KInputFileName);
	CleanupStack::PopAndDestroy(vParser);
	}


void CTests::TestHugeVCardL(CTests& aSelf)
	{
	aSelf.iTempFiles->RegisterL(KExportHugeVCardFileName());
	CParserVCard* vCardParser=CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);

	//Import the vCard
	aSelf.InternaliseFileL(*vCardParser,KHugeVCardFileName);
	
	aSelf.ExternaliseFileL(*vCardParser,KExportHugeVCardFileName);
	CleanupStack::PopAndDestroy(vCardParser);
}

void CTests::TestNullDateL(CTests& aSelf)
	{
	_LIT(KVcsNullDataFileName,"Z:\\testvcards\\various\\nulldate.vcf");
	RFileReadStream fileReadStream;
	User::LeaveIfError(fileReadStream.Open(aSelf.iFsSession,KVcsNullDataFileName,EFileRead));
	CleanupClosePushL(fileReadStream); //

	CParserVCard* parser = CParserVCard::NewL();
	CleanupStack::PushL(parser);
	
	//internalize vcf file
	fileReadStream>>(*parser);
	
	//now make sure the parser object contains a birthday property (KVersitTokenBDAY)
	TBool containsBirthday=EFalse;
	CArrayPtr<CParserProperty>* properties=parser->ArrayOfProperties(EFalse);
	for (TInt i=0;i<properties->Count();i++)
		{
		CParserProperty* property=properties->At(i);
		if (property->Name()==KVersitTokenBDAY)
			{
			containsBirthday=ETrue;
			break;
			}
		}

	TheTest(containsBirthday);

	CleanupStack::PopAndDestroy(2,&fileReadStream); // vParser
	}

void CTests::TestJapanL(CTests& aSelf)
	{
	TInt test=0;
	_LIT(KTest1,"QP Output");
	aSelf.DoJapanTestL(KTest1,++test);	//Had to remove a Yen symbol and ";" to stop it hitting another bug.
	_LIT(KTest2,"Folding Long Data");
	aSelf.DoJapanTestL(KTest2,++test);
	_LIT(KTest3,"Alarm Note Field");
	aSelf.DoJapanTestL(KTest3,++test,ETvCal);
	_LIT(KTest4,"Folding Fields");
	aSelf.DoJapanTestL(KTest4,++test);
	_LIT(KTest5,"ShiftJIS Escapes");
	aSelf.DoJapanTestL(KTest5,++test);		//Yen symbol not treated as an escape character
	_LIT(KTest6,"ShiftJIS QP Escape 1");
	aSelf.DoJapanTestL(KTest6,++test);
	_LIT(KTest7,"ShiftJIS QP Escape 2");
	aSelf.DoJapanTestL(KTest7,++test);
	_LIT(KTest8,"QP Line Length");
	aSelf.DoJapanTestL(KTest8,++test);
	_LIT(KTest9,"Encoding ;");
	aSelf.DoJapanTestL(KTest9,++test);
	//_LIT(KTest10,"Truncated Data");
	++test;//DoJapanTestL(KTest10,++test);		//Leaves because of truncated data and it has been tested in expected leave
	//_LIT(KTest11,"Data 11a");
	++test;//DoJapanTestL(KTest11,++test);		//comparing is failed because to externalise the code point 0x5c(escape), it is recongnised as "\" when it is in shiftjis charset, and yen sign when it is in all other charset
	}

void CTests::LeaveTestsL(CTests& aSelf)
	{
	_LIT(KTest1,"No End");
	_LIT(KFile1,"NoEnd");
	aSelf.DoLeaveTestL(ETvCard,KErrEof,KTest1,KFile1);
	_LIT(KTest2,"Half Line");
	_LIT(KFile2,"HalfLine");
	aSelf.DoLeaveTestL(ETvCard,KErrEof,KTest2,KFile2);
//	_LIT(KTest3,"Non-LeapDay"); This has been changed so that it is an internal leave
//	_LIT(KFile3,"Feb29");
//	DoLeaveTestL(ETvCal,KErrGeneral,KTest3,KFile3);
	_LIT(KTest4,"Entity Name 1");
	_LIT(KFile4,"EnName1");
	aSelf.DoLeaveTestL(ETvCal,KErrNotFound,KTest4,KFile4);
	_LIT(KTest5,"Entity Name 2");
	_LIT(KFile5,"EnName2");
	aSelf.DoLeaveTestL(ETvCal,KErrNotFound,KTest5,KFile5);
	_LIT(KTest6,"Entity Name 3");
	_LIT(KFile6,"EnName3");
	aSelf.DoLeaveTestL(ETvCard,KErrNotFound,KTest6,KFile6);
	_LIT(KTest7,"Entity Name 4");
	_LIT(KFile7,"EnName4");
	aSelf.DoLeaveTestL(ETvCard,KErrNotFound,KTest7,KFile7);
	_LIT(KTest8,"2 Begin's");
	_LIT(KFile8,"Begin2");
	aSelf.DoLeaveTestL(ETvCard,KErrNotSupported,KTest8,KFile8);
	_LIT(KTest9,"2 End's");
	_LIT(KFile9,"End2");
	aSelf.DoLeaveTestL(ETvCard,KErrEof,KTest9,KFile9);
	_LIT(KTest10,"3 Begin's");
	_LIT(KFile10,"Begin3");
	aSelf.DoLeaveTestL(ETvCal,KErrNotFound,KTest10,KFile10);
	_LIT(KTestNoLF,"No Line Feed");
	_LIT(KFileNoLF,"NoLF");
	aSelf.DoLeaveTestL(ETvCal,KErrEof,KTestNoLF,KFileNoLF);
	_LIT(KTestNestedAgent,"Nested Agent");
	_LIT(KFileNestedAgent,"NestedAgent");
	aSelf.DoLeaveTestL(ETvCard,KErrNotSupported,KTestNestedAgent,KFileNestedAgent);

	#ifdef _DEBUG
	_LIT(KTest11,"OOM Leave1");
	_LIT(KFile3,"Feb29");
	aSelf.DoOomTestL(ETvCard,8,KTest11,KFile1);
	_LIT(KTest12,"OOM Leave2");
	aSelf.DoOomTestL(ETvCard,10,KTest12,KFile2);
	_LIT(KTest13,"OOM Leave3");
	aSelf.DoOomTestL(ETvCal,12,KTest13,KFile3);
	
	#endif
	}

void CTests::SyncMLTestsL(CTests& aSelf)
	{
	_LIT(KTest1,"Japan");		//9 hours ahead of UTC
	aSelf.DoSyncMLTestL(9*eSecondsPerHour,KTest1,KTest1);
	_LIT(KTest2,"UK");			//Same as UTC
	aSelf.DoSyncMLTestL(0,KTest2,KTest2);
	_LIT(KTest3,"NewYork");		//5 hours behind UTC
	aSelf.DoSyncMLTestL(-5*eSecondsPerHour,KTest3,KTest3);
	}

void CTests::TestAlarmRepeatCountL(CTests& aSelf)
	{
	_LIT(KAlarmRepeatCount,"Test alarm repeat count");
	_LIT(KFileName,"alarmRepeatCount");
	
	//Compare output file with the input file. They should be identical.
	aSelf.TestFileL(ETvCal,KAlarmRepeatCount,KFileName);
	
	//Internalise the input file again
	CParserVCal* vParser = CParserVCal::NewL();
	CleanupStack::PushL(vParser);
	TFileName fullFileName;
	aSelf.ExistingFileName(ETvCal,fullFileName,'I',KFileName);
	aSelf.InternaliseFileL(*vParser,fullFileName);
 
    //Check the repeat count of the alarm
   	CArrayPtr<CVersitParser>* entities = vParser->ArrayOfEntities(EFalse);
   	
   	CArrayPtr<CParserProperty>* properties =(*entities)[0]->PropertyL(KVersitTokenAALARM, TUid::Uid(KVCalPropertyAlarmUid), EFalse);

	CleanupStack::PushL(properties);
	CParserPropertyValueAlarm* alarmProperty = STATIC_CAST(CParserPropertyValueAlarm*, (*properties)[0]->Value()); 
	TheTest(alarmProperty->Value()->iRepeatCount==0);
	CleanupStack::PopAndDestroy(properties);
	
    CleanupStack::PopAndDestroy(vParser);
	}

// This test case had been implemented for defect DEF112543.
// Some car kits don't understand the telephone number if '#' is encoded in QP
void CTests::TestHashSignEncodingL(CTests& aSelf)
	{
#ifndef TBUGFIX_INCLUDE_OOM
	aSelf.iTempFiles->RegisterL(KQPHashFileName);
	aSelf.iTempFiles->RegisterL(KOutputQPHashFileName);
#endif	//TBUGFIX_INCLUDE_OOM

	//Test VCard file	  
	_LIT8(KVCardData,	"BEGIN:VCARD\r\n"
						"VERSION:2.1\r\n"
						"TEL;HOME:#01#12546\r\n"
						"ADR;HOME;ENCODING=QUOTED-PRINTABLE:;;Address=20line1=0D=0Aaddress=20line2\r\n"
						"END:VCARD\r\n");
	
	//Save the test VCard file
	TheTest.Printf(_L("Save test Vcard file to be compared with exported vcard file later\n"));					
	RFile testFile;
	User::LeaveIfError(testFile.Replace(aSelf.iFsSession,KQPHashFileName,EFileWrite ));
	testFile.Write(KVCardData);
	testFile.Close();
	
	
	//Create a VCard						
	CParserVCard* vCardParser=CParserVCard::NewL();
	CleanupStack::PushL(vCardParser);
	
	//Create a tel value
	_LIT(KTelValue, "#01#12546");
	CDesCArrayFlat* telArray = new (ELeave) CDesCArrayFlat(3);
	CleanupStack::PushL(telArray);
	telArray->AppendL(KTelValue);  
	CParserPropertyValue* telValue=new (ELeave) CParserPropertyValueCDesCArray(telArray);
  	CleanupStack::Pop(telArray);  
	CleanupStack::PushL(telValue);
	// Create a property parameter "HOME"
	CArrayPtr<CParserParam>* arrayOfTelParams = new(ELeave)CArrayPtrFlat<CParserParam>(1);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfParams,arrayOfTelParams));
	CParserParam* homeParam=CParserParam::NewL(_L8("HOME"),KNullDesC);
	CleanupStack::PushL(homeParam);
    arrayOfTelParams->AppendL(homeParam);
	CleanupStack::Pop(homeParam);
	//Create a TEL property 
	CParserProperty* telProperty=CParserProperty::NewL(*telValue,KVersitTokenTEL,arrayOfTelParams);
	CleanupStack::Pop(arrayOfTelParams);
	CleanupStack::Pop(telValue);
	//Add the TEL property to the VCard
	CleanupStack::PushL(telProperty);
	vCardParser->AddPropertyL(telProperty);
	CleanupStack::Pop(telProperty);
	
	//Create an address value
	_LIT(KAddrValue, "Address line1\r\naddress line2");
	CDesCArrayFlat* addrArray = new (ELeave) CDesCArrayFlat(3);
	CleanupStack::PushL(addrArray);
	addrArray->AppendL(KNullDesC);
	addrArray->AppendL(KNullDesC);
	addrArray->AppendL(KAddrValue);  
	CParserPropertyValue* addrValue=new (ELeave) CParserPropertyValueCDesCArray(addrArray);
  	CleanupStack::Pop(addrArray);
  	CleanupStack::PushL(addrValue);
	// Create a property parameter "HOME"
	CArrayPtr<CParserParam>* arrayOfParams = new(ELeave)CArrayPtrFlat<CParserParam>(1);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfParams,arrayOfParams));
	CParserParam* parserParam=CParserParam::NewL(_L8("HOME"),KNullDesC);
	CleanupStack::PushL(parserParam);
    arrayOfParams->AppendL(parserParam);
	CleanupStack::Pop(parserParam);
	//Create an ADR property 
	CParserProperty* property=CParserProperty::NewL(*addrValue,KVersitTokenADR,arrayOfParams);
	CleanupStack::Pop(2, addrValue);
	//Add the ADR property to the VCard
	CleanupStack::PushL(property);
	vCardParser->AddPropertyL(property);
	CleanupStack::Pop(property);

	//Export the VCard
	aSelf.ExternaliseFileL(*vCardParser,KOutputQPHashFileName);

	CleanupStack::PopAndDestroy(vCardParser);

	//Compare the VCard exported with the earlier saved VCard test file
	aSelf.CompareFilesL(KQPFileName, KOutputQPFileName);	
	}

void CTests::TestBase64ExtensionL(CTests& aSelf)
	{
	TBool compareFile = aSelf.iBase64Extension?ETrue:EFalse;//If MVersitPlugInExtensionBase64Ending is not impletmented and set, non-standard base64 data is not internalised correctly. 
	_LIT(KTestVCALBase64vCard,"Base64 vCard Test");
	_LIT(KFileVCALBase64,"newbase64");
	aSelf.TestFileL(ETvCard,KTestVCALBase64vCard,KFileVCALBase64, Versit::EUSAsciiCharSet, Versit::ENoEncoding, compareFile );
	_LIT(KTestVCALBase64vCal,"Base64 vCal Test");
	aSelf.TestFileL(ETvCal,KTestVCALBase64vCal,KFileVCALBase64, Versit::EUSAsciiCharSet, Versit::ENoEncoding, compareFile );// a vCal with some invalid data
	_LIT(KTestVCALBase64vTrigger,"Base64 vTrigger Test");
	_LIT(KFileVCALBase64vTrigger,"vtrigger");
	aSelf.TestFileL(ETvCard,KTestVCALBase64vTrigger,KFileVCALBase64vTrigger, Versit::EUSAsciiCharSet, Versit::ENoEncoding, compareFile );
	}

void CTests::TestL(CTests& aSelf)
	{
	TRAPD(err, TestBase64ExtensionL(aSelf))
	
	if (aSelf.iBase64Extension)
		{//If MVersitPlugInExtensionBase64Ending is not impletmented and set, non-standard base64 data is not internalised correctly. 
		TheTest(err == KErrNone);
		}

	_LIT(KTestVCARD,"General vCard Test");
	_LIT(KFileVCARD,"TestVCard");
	aSelf.TestFileL(ETvCard,KTestVCARD,KFileVCARD);//a complete vCard with some invalid data
	_LIT(KTestVCAL,"General vCal Test");
	_LIT(KFileVCAL,"TestVCal");
	aSelf.TestFileL(ETvCal,KTestVCAL,KFileVCAL);// a vCal with some invalid data
	_LIT(KTest1,"Quoted Printable Tabs");
	_LIT(KFile1,"Tabs");
	aSelf.TestFileL(ETvCal,KTest1,KFile1);// Make sure the QP is auto-applied when a tab is included in the text
	_LIT(KTest2,"Shift-JIS with \\ (VAN-4YQGX3)");// bug fix
	_LIT(KFile2,"ShJisEs");
	aSelf.TestFileL(ETvCard,KTest2,KFile2,Versit::EShiftJISCharSet);
	_LIT(KTest3,"Alarm Durations (EVS-522G7D)");// bug fix
	_LIT(KFile3,"TimePer");
	aSelf.TestFileL(ETvCal,KTest3,KFile3);
	_LIT(KTest4,"Dalarm's display string (EVS-4ZUJBC)"); // bug fix
	_LIT(KFile4,"Dalarm");
	aSelf.TestFileL(ETvCal,KTest4,KFile4,Versit::EShiftJISCharSet);
	_LIT(KTest5,"Bad Base64 Data");
	_LIT(KFile5,"Base64");
	aSelf.TestFileL(ETvCard,KTest5,KFile5);
	_LIT(KTest6,"Alarm's and Base64");
	_LIT(KFile6,"Dalarm2");
	aSelf.TestFileL(ETvCal,KTest6,KFile6,Versit::EUSAsciiCharSet,Versit::EBase64Encoding);
	_LIT(KTest7,"Space & Wrapping");
	_LIT(KFile7,"Wrap");
	aSelf.TestFileL(ETvCal,KTest7,KFile7);
	//aSelf.TestFileL(ETvCard,KTest8,KFile8);
	//_LIT(KTest9,"GBK & QuotedPrintable");		//Doesn't work with CharConvn name matching
	//_LIT(KFile9,"GBK");
	//TestFileL(ETvCard,KTest9,KFile9,Versit::EGBKCharSet,Versit::EBase64Encoding);
	_LIT(KTest10,"Shift Jis");
	_LIT(KFile10,"ShiftJis");
	aSelf.TestFileL(ETvCal,KTest10,KFile10,Versit::EShiftJISCharSet);
	_LIT(KTest11,"vCard File");
	_LIT(KFile11,"vCard");
	aSelf.TestFileL(ETvCard,KTest11,KFile11);
	_LIT(KTest12,"vCal File");
	_LIT(KFile12,"vCal");
	aSelf.TestFileL(ETvCal,KTest12,KFile12);
	_LIT(KTest13,"Base64 encoded End");
	_LIT(KFile13,"EndBa64");
	aSelf.TestFileL(ETvCal,KTest13,KFile13);
	_LIT(KTestTimeZone,"Time-zone check");// bug fix
	_LIT(KFile14a,"TimeZoneP");
	_LIT(KFile14b,"TimeZoneN");
	aSelf.TestFileL(ETvCal,KTestTimeZone,KFile14a);
	aSelf.TestFileL(ETvCal,KTestTimeZone,KFile14b);
	_LIT(KTest15,"Wrapping on linelength");
	_LIT(KFile15,"Wrap2");
	aSelf.TestFileL(ETvCal,KTest15,KFile15);
	_LIT(KTest16,"TimeZone and DayLight Adjustment");
	_LIT(KFile16,"TZone");
	aSelf.TestFileL(ETvCal,KTest16,KFile16);
	_LIT(KTest17,"TimeZone and DayLight Syntax");
	_LIT(KFile17,"TZone2");
	aSelf.TestFileL(ETvCal|ETuseBaseParser,KTest17,KFile17);
	_LIT(KTest18,"String ends in '\\'");
	_LIT(KFile18,"ESlash");
	aSelf.TestFileL(ETvCard,KTest18,KFile18);
	_LIT(KTest19,"Null Parameters");
	_LIT(KFile19,"NullPar");
	aSelf.TestFileL(ETvCal,KTest19,KFile19);
	_LIT(KTest20,"ShiftJIS Escape (Yen) 1");
	_LIT(KFile20,"YenEsc1");
	aSelf.TestFileL(ETvCard,KTest20,KFile20);
	_LIT(KTest21,"ShiftJIS Escape (Yen) 2");
	_LIT(KFile21,"YenEsc2");
	aSelf.TestFileL(ETvCard,KTest21,KFile21,Versit::EShiftJISCharSet);
	_LIT(KTest22,"Escaping Parameters");
	_LIT(KFile22,"ParamE");
	aSelf.TestFileL(ETvCard,KTest22,KFile22);
	_LIT(KTest23,"Line Wrap Bug: SVG-58ZECH");
	_LIT(KFile23,"Fred");
	aSelf.TestFileL(ETvCard,KTest23,KFile23);
	_LIT(KTest24,"non-standard agent vCard Bug: HUN_5ELH9C");
	_LIT(KFile24,"NonStandardAgent");
	aSelf.TestFileL(ETvCard,KTest24,KFile24);
	_LIT(KTest25,"RRule defect INC017322");// bug fix
	_LIT(KFile25,"RRule");
	aSelf.TestFileL(ETvCal,KTest25,KFile25);
	_LIT(KTest26,"Blank line defect DEF022279");// bug fix
	_LIT(KFile26,"BlankLines");
	aSelf.TestFileL(ETvCard,KTest26,KFile26);
	_LIT(KTest27,"Defect fix INC021811");
	_LIT(KFile27,"LineFeed");
	aSelf.TestFileL(ETvCal,KTest27,KFile27);//file end with<LF>
	_LIT(KTest28,"Base64 Data with whitespace defect DEF036630");// bug fix
	_LIT(KFile28,"Base64ws");
	aSelf.TestFileL(ETvCard,KTest28,KFile28);
	_LIT(KTest29,"Defect DEF078802 - Ignore junk lines b/w vCal entities");// bug fix
	_LIT(KFile29,"JunkLine");
	aSelf.TestFileL(ETvCal,KTest29,KFile29);
	_LIT(KTest30,"Attachment Test");
	_LIT(KFile30,"attachment");
	aSelf.TestFileL(ETvCal,KTest30,KFile30);// a vCal with some invalid data

	}
	
void CTests::AllTestsL()
	{
	TheTest.Start(_L("TBugfix general tests"));
	
	TheTest.Next(_L("Simple Write File"));
	Test0L(*this);		//Does not require any data files to be present
	
	TheTest.Next(_L("versit files test"));
	TestL(*this);		//File Tests - externalise a file and compare the resultant with the expecting data

	iBase64Extension = CTestVersitExtension::NewL();

	TestL(*this);
	doOOMTest(*this, CTests::TestBase64ExtensionL, 0);
	LeaveTestsL(*this);

	delete iBase64Extension;
	iBase64Extension = NULL;
	
	TheTest.Start(_L("PlugIn Test"));
	TestPlugInL(iFsSession);		//Test the PlugIn and Observer functionality
	TheTest.End();
	
	TheTest.Next(_L("SyncML Tests"));
	SyncMLTestsL(*this); //Test data got from syncing with a SyncML server - particular to check a timezone bug.
	
	TheTest.Next(_L("ShiftJIS & Othert Tests"));
	TestJapanL(*this);	//Test data produced by K3 to illustrate bugs found in GT6.1 code
	
	TheTest.Next(_L("Leave Tests"));
	LeaveTestsL(*this);	//Internalise a file with bad data and check that you get the right leave code
	
	TheTest.Next(_L("Escaping Test"));//test the escape key "\"
	Test1L(*this);
	
	TheTest.Next(_L("Quoted-Printable Test"));
	Test2L(*this);
	
	//create a vCard, externalise it, then internalise it and compare the property values with the original ones
	TheTest.Next(_L("Encodings Test"));
	Test3L(*this);
	
	// test decoding the text which is base64 encoded
	TheTest.Next(_L("Base64 Encoding"));
	Test4L(*this);
	
	// bug fix
	TheTest.Next(_L("Base64 inconsistent (WAG-4YGCRX)"));
	Test5L(*this);
	
	// bug fix
	TheTest.Next(_L("Line wrapping test"));
	Test6L(*this);//Line wrapping test
	
	TheTest.Next(_L("EXT-54BGZJ"));
	Test7L(*this);

	TheTest.Next(_L("Monthly-By-Position RRULE with impartial date"));
	Test11L(*this);

	TheTest.Next(_L("Grouped proterty test"));
	Test8L(*this);		//grouped proterty
	
	TheTest.Next(_L("Jis charset"));//bug fix CHM-53KE5L
	Test9L(*this);
	
	/*PDEF088039 changes to autodetect means that the following test will not work
	there are no comments what this really tests, or why it is trying to test autodetect.
	the use case is contrived as it uses a vcard and then has an alarm property in it!
	the display string represents some garbage outside the ascii range.
	*/
	/*
	_LIT(KTest10,"Auto Detect");
	TheTest.Next(KTest10);
	Test10L();*/  
	TheTest.Next(_L("Quoted-Printable Test Encoding Test"));
	Test12L(*this);

	TheTest.Next(_L("Testing huge VCard with Random data..."));
	TestHugeVCardL(*this);

	TheTest.Next(_L("Testing null date..."));
	TestNullDateL(*this);
	
	TheTest.Next(_L("Alarm repeat count"));
	TestAlarmRepeatCountL(*this);
	
	TheTest.Next(_L("Encoding Test for '#'"));
	TestHashSignEncodingL(*this);

	TheTest.Next(_L("Finished Tests"));
	TheTest.End();
	}
	
void doOOMTest(CTests& aTest, TOOMCallback aCallback, TInt aFailStart)
	{
	TInt ret=KErrNoMemory;
	TInt failAt=aFailStart;
	while (ret!=KErrNone)
		{
		failAt++;
		TBuf<200>indicator;
		indicator.Format(_L("Memory fail at \t%d\x0d\x0a"),failAt);
		if(failAt%100==0)
			TheTest.Printf(indicator);
		__UHEAP_SETFAIL(RHeap::EDeterministic,failAt);
		__UHEAP_MARK;			
		TRAP(ret, aCallback(aTest));
		__UHEAP_MARKEND;
		__UHEAP_RESET;
		if (ret!=KErrNoMemory&&ret!=KErrNone)
			{
			TheTest.Printf(_L("Non standard error: %d\n"),ret);
			}
		}
	}

void OOMTestsL(CTests& aTest)
	{
	TheTest.Start(_L("TBugfix OOM tests"));
	
	TheTest.Next(_L("OOM on Simple write file"));
	doOOMTest(aTest, CTests::Test0L, 0);
	
	TheTest.Next(_L("OOM on versit files test"));
	doOOMTest(aTest, CTests::TestL, 786);
	
	TheTest.Next(_L("OOM on SyncML Tests"));
	doOOMTest(aTest, CTests::SyncMLTestsL, 0);
	
	TheTest.Next(_L("OOM on ShiftJIS & Othert Tests"));
	doOOMTest(aTest, CTests::TestJapanL, 0);
	
	TheTest.Next(_L("OOM on Escaping Test"));
	doOOMTest(aTest, CTests::Test1L, 0);
	
	TheTest.Next(_L("OOM on Quoted-Printable Test"));
	doOOMTest(aTest, CTests::Test2L, 0);
	
	//create a vCard, externalise it, then internalise it and compare the property values with the original ones
	TheTest.Next(_L("OOM on Encodings Test"));
	doOOMTest(aTest, CTests::Test3L, 0);
	
	// test decoding the text which is base64 encoded
	TheTest.Next(_L("OOM on Base64 Encodings Test"));
	doOOMTest(aTest, CTests::Test4L, 0);
	
	TheTest.Next(_L("OOM on Base64 inconsistent (WAG-4YGCRX)"));
	doOOMTest(aTest, CTests::Test5L, 0);
	
	TheTest.Next(_L("OOM on Line wrapping test"));
	doOOMTest(aTest, CTests::Test6L, 0);
	
	TheTest.Next(_L("OOM on EXT-54BGZJ"));
	doOOMTest(aTest, CTests::Test7L, 0);

	TheTest.Next(_L("OOM on Monthly-By-Position RRULE with impartial date"));
	doOOMTest(aTest, CTests::Test11L, 0);

	TheTest.Next(_L("OOM on Grouped proterty test"));
	doOOMTest(aTest, CTests::Test8L, 0);
	
	TheTest.Next(_L("OOM on Jis charset"));
	doOOMTest(aTest, CTests::Test9L, 145);
	
	/*PDEF088039 changes to autodetect means that the following test will not work
	there are no comments what this really tests, or why it is trying to test autodetect.
	the use case is contrived as it uses a vcard and then has an alarm property in it!
	the display string represents some garbage outside the ascii range.
	*/
	/*
	_LIT(KTest10,"Auto Detect");
	TheTest.Next(KTest10);
	Test10L();*/  
	
	TheTest.Next(_L("OOM on testing null date"));
	doOOMTest(aTest, CTests::TestNullDateL, 0);
	
	TheTest.Next(_L("OOM on Alarm repeat count"));
	doOOMTest(aTest, CTests::TestAlarmRepeatCountL, 0);

	TheTest.Next(_L("Finished OOM Tests"));
	TheTest.End();
	}

void DoTestsL()
	{
	CTests* testObj = CTests::NewL();
	CleanupStack::PushL(testObj);
	testObj->AllTestsL();
	
#ifdef TBUGFIX_INCLUDE_OOM
	OOMTestsL(*testObj);
#endif	//TBUGFIX_INCLUDE_OOM

	CleanupStack::PopAndDestroy(testObj);
	}

GLDEF_C TInt E32Main()
	{
	_LIT(KTest,"Entered Test Code");
	RDebug::Print(KTest);
	
	TTime startTime;
	startTime.UniversalTime();
	
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if	(!cleanup)
		return KErrNoMemory;

	_LIT(KTestName,"Regression Tests");	
	TheTest.Start(KTestName);
	TheTest.Printf(_L("@SYMTestCaseID PIM-TBUGFIX-Versit_Escaping_Test-0001"));
	TRAPD(err,DoTestsL());

	TheTest.End();
	TheTest(err == KErrNone);
	TheTest.Close();
	delete cleanup;

	__UHEAP_MARKEND;
	
	TTime finishTime;
	finishTime.UniversalTime();
	TReal elapsed = (TReal)finishTime.MicroSecondsFrom(startTime).Int64();
	TheTest.Printf(_L("Elapsed time: %.4f\n"), elapsed/1000000);
	
	return (KErrNone);
	}
