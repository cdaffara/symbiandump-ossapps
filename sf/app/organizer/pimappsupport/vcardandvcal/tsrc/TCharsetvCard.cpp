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

#include "TCharsetvCard.h"

// System includes
#include <versit.h>
#include <f32file.h>
#include <e32test.h>
#include <charconv.h>

// Literal constants
_LIT(KTestName, "TCharSetvCard");

// Globals
static RFs					TheFsSession;
//static CTrapCleanup*		TheCleanup;
static RTest				TheTest(KTestName);

static void FileCleanup(TAny * aFileMan)
	{
	CFileMan* FileMan = (CFileMan*)aFileMan;
	FileMan->RmDir(_L("C:\\OutputTestvCards\\"));
	}

static void DoTestsL()
	{
	CFileMan* FileMan = CFileMan::NewL(TheFsSession);
	CleanupStack::PushL(FileMan);
	CleanupStack::PushL(TCleanupItem(FileCleanup, FileMan));
	CVersitCardTest* test = CVersitCardTest::NewLC();
	test->StartTestsL();
	CleanupStack::PopAndDestroy(3, FileMan); // filecleanup, test
	}


/**
@SYMTestCaseID PIM-TCHARSETVCARD-0001
*/	
GLDEF_C TInt E32Main()
	{
	
	TTime startTime;
	startTime.UniversalTime();
	
	__UHEAP_MARK;
	CTrapCleanup* TheCleanup = CTrapCleanup::New();
	if	(!TheCleanup)
		return KErrNoMemory;

	if	(TheFsSession.Connect() < KErrNone)
		return KErrGeneral;

	TheTest.Start(_L("@SYMTestCaseID PIM-TCHARSETVCARD-0001 TCharSetvCard"));
	TRAPD(error, DoTestsL());
	TheTest(error == KErrNone);
	TheTest.End();
	TheTest.Close();
	TheFsSession.Close();

	delete TheCleanup;
	__UHEAP_MARKEND;
	
	TTime finishTime;
	finishTime.UniversalTime();
	TReal elapsed = (TReal)finishTime.MicroSecondsFrom(startTime).Int64();
	TheTest.Printf(_L("Elapsed time: %.4f\n"), elapsed/1000000);
	
	return KErrNone;
	}




//
// ------> CVersitCardTest (source)
//

//********************************************************************************************
CVersitCardTest::~CVersitCardTest()
	{
	delete iDirectoryScanner;
	delete iArrayOfCharacterSetsAvailable;
	}


//********************************************************************************************
void CVersitCardTest::ConstructL()
	{
	iDirectoryScanner = CDirScan::NewL(TheFsSession);
	iArrayOfCharacterSetsAvailable = CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL(TheFsSession);
	}


//********************************************************************************************
CVersitCardTest* CVersitCardTest::NewLC()
	{
	CVersitCardTest* self = new(ELeave) CVersitCardTest;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


//
//
//


//********************************************************************************************
void CVersitCardTest::StartTestsL()
	{
/*
	CParserVCard* parser;
	
	parser = new(ELeave) CParserVCard();
	CleanupStack::PushL(parser);
	InternalizeCardL(_L("C:\\Testvcards\\Encoded\\2.vcf"), *parser, CVersitParser::EUSAsciiCharSet, CVersitParser::ENoEncoding);
	ExternalizeCardL(_L("C:\\2.vcf"), *parser, CVersitParser::EUSAsciiCharSet, CVersitParser::ENoEncoding);
	CleanupStack::PopAndDestroy(); // parser

	return;
	parser = new(ELeave) CParserVCard();
	CleanupStack::PushL(parser);
	InternalizeCardL(_L("C:\\Testvcards\\Various\\9.vcf"), *parser, CVersitParser::EISO88591CharSet, CVersitParser::ENoEncoding);
	//ExternalizeCardL(_L("C:\\vCard-jis.vcf"), *parser, CVersitParser::EJISCharSet, CVersitParser::ENoEncoding);
	CleanupStack::PopAndDestroy(); // parser

	
	return;
	CParserVCard* parser;
	
	parser = new(ELeave) CParserVCard();
	CleanupStack::PushL(parser);
	InternalizeCardL(_L("C:\\Testvcards\\Encoded\\1.vcf"), *parser, CVersitParser::EISO88591CharSet, CVersitParser::ENoEncoding);
	ExternalizeCardL(_L("C:\\vCard-shiftjis.vcf"), *parser, CVersitParser::EShiftJISCharSet, CVersitParser::ENoEncoding);
	CleanupStack::PopAndDestroy(); // parser

	// Read ISO-1 -> Write JIS
	parser = new(ELeave) CParserVCard();
	CleanupStack::PushL(parser);
	InternalizeCardL(_L("C:\\Testvcards\\Encoded\\1.vcf"), *parser, CVersitParser::EISO88591CharSet, CVersitParser::ENoEncoding);
	ExternalizeCardL(_L("C:\\vCard-jis.vcf"), *parser, CVersitParser::EJISCharSet, CVersitParser::ENoEncoding);
	CleanupStack::PopAndDestroy(); // parser

	// Read ShiftJIS -> Write ISO-1
	parser = new(ELeave) CParserVCard();
	CleanupStack::PushL(parser);
	InternalizeCardL(_L("C:\\vCard-shiftjis.vcf"), *parser, CVersitParser::EShiftJISCharSet, CVersitParser::ENoEncoding);
	ExternalizeCardL(_L("C:\\vCard-iso1-from-shiftjis.vcf"), *parser, CVersitParser::EISO88591CharSet, CVersitParser::ENoEncoding);
	CleanupStack::PopAndDestroy(); // parser

	// Read JIS -> Write ISO-1
	parser = new(ELeave) CParserVCard();
	CleanupStack::PushL(parser);
	InternalizeCardL(_L("C:\\vCard-jis.vcf"), *parser, CVersitParser::EJISCharSet, CVersitParser::ENoEncoding);
	ExternalizeCardL(_L("C:\\vCard-iso1-from-jis.vcf"), *parser, CVersitParser::EISO88591CharSet, CVersitParser::ENoEncoding);
	CleanupStack::PopAndDestroy(); // parser
	*/

	_LIT(KTestDataDirectory, "Z:\\TestvCards\\");
	_LIT(KvCardExtension, ".vCF");

	iDirectoryScanner->SetScanDataL(KTestDataDirectory, KEntryAttDir, ESortByExt|EAscending, CDirScan::EScanUpTree);

	CDir* dirList = NULL;

	// Fetch next dir
	FOREVER
		{
		iDirectoryScanner->NextL(dirList);
		if	(!dirList)
			break;
		CleanupStack::PushL(dirList);

		TPtrC path(iDirectoryScanner->FullPath());

		const TInt count = dirList->Count();
		for(TInt i=0; i<count; i++)
			{
			const TEntry& entry = (*dirList)[i];

			TFileName file(path);
			file += entry.iName;

			// Check for vCards
			TParsePtrC parser(file);
			if	(parser.Ext().CompareF(KvCardExtension) != 0)
				continue;

			// We've got a vCard so internalize it
			CParserVCard* cardParser = CParserVCard::NewL();
			CleanupStack::PushL(cardParser);
			
			// Internalize & externalize the card	
			TheTest.Next(file);
			TheTest.Start(KNullDesC);
			
			TRAPD(err, InternalizeAndExternalizeCardL(file, *cardParser));
			TheTest.End();
			CleanupStack::PopAndDestroy(); // cardParser
			

			// Handling for invalid cards
			if	(err < KErrNone)
				{
				if	(file.Find(_L("\\invalid\\")) >= KErrNone)
					{
					// This was an invalid cards, but it's ok for versit
					// to leave if this is the case...
					continue;
					}
				}
			}

		CleanupStack::PopAndDestroy(); // dirList

		}
	}


//
//
//


//********************************************************************************************
void CVersitCardTest::InternalizeAndExternalizeCardL(const TDesC& aFile, CParserVCard& aParser) const
	{
	_LIT(KSpacer, " - ");
	//_LIT(KCharset, "CharSet=");
	//_LIT(KEncoding, "Encoding=");
	_LIT(KTestOutputDirPrefix, "Output");

	TParsePtrC outputDirParser(aFile);
	TFileName outputDir;

	// Make output directory
	outputDir += outputDirParser.Drive();
	outputDir.Append('\\');
	outputDir += KTestOutputDirPrefix;
	{
	TInt length = outputDir.Length();
	outputDir += outputDirParser.Path();
	outputDir.Delete(length, 1);
	outputDir += outputDirParser.Name();
	outputDir.Append('\\');
	TheFsSession.MkDirAll(outputDir);
	}

	{
	// Guess the charset of the vCard
	TUint charConvCharSet = AutoDetectCharacterSetL(TheFsSession, aFile);

	// Map to versit charset
	Versit::TVersitCharSet charSet = MapCharConvCharSetToVersitCharSet(charConvCharSet);

	TFileName testName(aFile);
	testName += _L(" [");
	testName += CharsetAsDescriptor(charSet);
	testName += _L("]");
	
	// Dump test title
	TheTest.Next(testName);

	// Internalize it
	InternalizeCardL(aFile, aParser, charSet, Versit::ENoEncoding);
	}

	TInt countCharSets = Versit::EJISCharSet;
	for(TInt i=Versit::EUSAsciiCharSet; i<=countCharSets; i++)
		{
		// Get charset
		Versit::TVersitCharSet charSet = static_cast<Versit::TVersitCharSet>(i);
	
		TInt countEncodings = Versit::EBase64Encoding;
		for(TInt j=Versit::ENoEncoding; j<=countEncodings; j++)
			{
			Versit::TVersitEncoding encoding = static_cast<Versit::TVersitEncoding>(j);

			// Generate filename
			TFileName file(outputDir);
			file += CharsetAsDescriptor(charSet);
			file += KSpacer;
			file += EncodingAsDescriptor(encoding);
			file += outputDirParser.Ext();

			// Write test
			{
			TParsePtrC testNameParser(file);
			TName name(testNameParser.NameAndExt());
			name.Insert(0, _L("\t"));	
			TheTest.Next(name);
			}

			// Externalize 
			ExternalizeCardL(file, aParser, charSet, encoding);
			}
		}
	}


//********************************************************************************************
//void CVersitCardTest::InternalizeCardL(const TDesC& aFile, CParserVCard& aParser, CVersitParser::TVersitCharSet aCharSet, CVersitParser::TVersitEncoding aEncoding) const
void CVersitCardTest::InternalizeCardL(const TDesC& aFile, CParserVCard& aParser, Versit::TVersitCharSet aCharSet, Versit::TVersitEncoding aEncoding) const
//
//	Internalize the specified file
//
	{
	TInt pos = 0;
	RFile file;
	User::LeaveIfError(file.Open(TheFsSession, aFile, EFileRead));
	CleanupClosePushL(file);

	// Set defaults
	aParser.SetDefaultEncoding(aEncoding);
	aParser.SetDefaultCharSet(aCharSet);

	// Read from the file
	static_cast<CVersitParser&>(aParser).InternalizeL(file, pos);

	CleanupStack::PopAndDestroy(); // file
	}


//********************************************************************************************
//void CVersitCardTest::ExternalizeCardL(const TDesC& aFile, CParserVCard& aParser, CVersitParser::TVersitCharSet aCharSet, CVersitParser::TVersitEncoding aEncoding) const
void CVersitCardTest::ExternalizeCardL(const TDesC& aFile, CParserVCard& aParser, Versit::TVersitCharSet aCharSet, Versit::TVersitEncoding aEncoding) const
//
//	Externalize the specified file
//
	{
	RFile file;
	User::LeaveIfError(file.Replace(TheFsSession, aFile, EFileWrite));
	CleanupClosePushL(file);

	// Set defaults
	aParser.SetDefaultEncoding(aEncoding);
	aParser.SetDefaultCharSet(aCharSet);

	// Write from the file
	static_cast<CVersitParser&>(aParser).ExternalizeL(file);

	CleanupStack::PopAndDestroy(); // file
	}


//********************************************************************************************
//const TDesC& CVersitCardTest::CharsetAsDescriptor(CVersitParser::TVersitCharSet aCharSet)
const TDesC& CVersitCardTest::CharsetAsDescriptor(Versit::TVersitCharSet aCharSet)
	{
	_LIT(KVersitCharSetASCII,		"ASCII");
	_LIT(KVersitCharSetUTF8,		"UTF8");
	_LIT(KVersitCharSetUTF7,		"UTF7");
	_LIT(KVersitCharSetISO_8859_1,	"ISO-8859-1");
	_LIT(KVersitCharSetISO_8859_2,	"ISO-8859-2");
	_LIT(KVersitCharSetISO_8859_4,	"ISO-8859-4");
	_LIT(KVersitCharSetISO_8859_5,	"ISO-8859-5");
	_LIT(KVersitCharSetISO_8859_7,	"ISO-8859-7");
	_LIT(KVersitCharSetISO_8859_9,	"ISO-8859-9");
	_LIT(KVersitCharSetISO_8859_3,	"ISO-8859-3");
	_LIT(KVersitCharSetISO_8859_0,	"ISO-8859-0");
	_LIT(KVersitCharSetGB_231280,	"GB-231280");
	_LIT(KVersitCharSetGBK,			"GBK");
	_LIT(KVersitCharSetBIG5,		"BIG5");
	_LIT(KVersitCharSetISO_2022JP,	"ISO-2022JP");
	_LIT(KVersitCharSetEUCJP,		"EUCJP");
	_LIT(KVersitCharSetShiftJIS,	"ShiftJIS");
	_LIT(KVersitCharSetJIS,			"JIS");
	//
	switch(aCharSet)
		{
	default:
	case Versit::EUSAsciiCharSet:
		return KVersitCharSetASCII;
	case Versit::EUTF8CharSet:
		return KVersitCharSetUTF8;
	case Versit::EUTF7CharSet:
		return KVersitCharSetUTF7;
	case Versit::EISO88591CharSet:
		return KVersitCharSetISO_8859_1;
	case Versit::EISO88592CharSet:
		return KVersitCharSetISO_8859_2;
	case Versit::EISO88594CharSet:
		return KVersitCharSetISO_8859_4;
	case Versit::EISO88595CharSet:
		return KVersitCharSetISO_8859_5;
	case Versit::EISO88597CharSet:
		return KVersitCharSetISO_8859_7;
	case Versit::EISO88599CharSet:
		return KVersitCharSetISO_8859_9;
	case Versit::EISO88593CharSet:
		return KVersitCharSetISO_8859_3;
	case Versit::EISO885910CharSet:
		return KVersitCharSetISO_8859_0;
	case Versit::EGB231280CharSet:
		return KVersitCharSetGB_231280;
	case Versit::EGBKCharSet:
		return KVersitCharSetGBK;
	case Versit::EBIG5CharSet:
		return KVersitCharSetBIG5;
	case Versit::EISO2022JPCharSet:
		return KVersitCharSetISO_2022JP;
	case Versit::EEUCJPCharSet:
		return KVersitCharSetEUCJP;
	case Versit::EShiftJISCharSet:
		return KVersitCharSetShiftJIS;
	case Versit::EJISCharSet:
		return KVersitCharSetJIS;
		}
	}


//********************************************************************************************
const TDesC& CVersitCardTest::EncodingAsDescriptor(Versit::TVersitEncoding aEncoding)
	{
	_LIT(KVersitEncodingNone,	"None");
	_LIT(KVersitEncodingQP,		"Quoted Printable");
	_LIT(KVersitEncodingBase64, "Base64");
	//
	switch(aEncoding)
		{
	default:
	case Versit::ENoEncoding:
		return KVersitEncodingNone;
	case Versit::EQuotedPrintableEncoding:
		return KVersitEncodingQP;
	case Versit::EBase64Encoding:
		return KVersitEncodingBase64;
		}
	}


//********************************************************************************************
TUint CVersitCardTest::AutoDetectCharacterSetL(RFs& aFsSession, const TDesC& aFile) const
	{
	_LIT8(KCharset, "CHARSET");
	_LIT8(KBase64, "BASE64");

	RFile file;
	User::LeaveIfError(file.Open(aFsSession, aFile, EFileRead));

	// Read a bit of the file
	TBuf8<400> sample;
	file.Read(sample); // ignore error
	file.Close();

	const TInt length = sample.Length();
	if	(!length)
		User::Leave(KErrUnderflow);

	// First of all, try and find a charset identifier in the vCard...
	TInt pos = sample.Find(KCharset);
	if	(pos >= KErrNone)
		{
		// There is a charset line, so read until we hit a = 
		while(pos < length && sample[pos] != '=')
			pos++;

		// Did we find the '=' ?
		if	(pos < length && sample[pos] == '=')
			{
			// Yes, so mark the start
			TInt start = ++pos;

			// There is a charset line, so read until we hit a : or a ;
			while(pos < length && !(sample[pos] == ';' || sample[pos] == ':'))
				pos++;

			// Did we find the ':' or ';' ?
			if	(pos < length && sample[pos] == ';' || sample[pos] == ':')
				{
				// We now have a range for the charset identifier
				TPtrC8 pCharset(sample.Mid(start, pos-start));

				// Attempt to ask CharConv to identify it...
				CCnvCharacterSetConverter* charConv = CCnvCharacterSetConverter::NewLC();
				TUint charset = charConv->ConvertStandardNameOfCharacterSetToIdentifierL(pCharset, TheFsSession);
				CleanupStack::PopAndDestroy(); // charConv
				
				// Charconv returns 0 if it doesn't recognize it
				if	(charset)
					return charset;
				}
			}
		}
	// See if we can find a BASE64 marker... if we can then ignore everything
	// before this point in the stream, since it confuses CharConv
	pos = sample.Find(KBase64);
	if	(pos >= KErrNone)
		{
		// Snip off anything after this point
		sample = sample.Left(pos);
		}

	// Attempt to detect charset
	TUint charset = 0;
	TInt confidence = 0;
	CCnvCharacterSetConverter::AutoDetectCharacterSetL(confidence, charset, *iArrayOfCharacterSetsAvailable, sample);
	if	(!confidence)
		return KCharacterSetIdentifierAscii;
	return charset;
	}


//********************************************************************************************
Versit::TVersitCharSet CVersitCardTest::MapCharConvCharSetToVersitCharSet(TUint aCharConvCharSet)
	{
	switch(aCharConvCharSet)
		{
	case KCharacterSetIdentifierAscii:
	case KCharacterSetIdentifierIso88596:
	case KCharacterSetIdentifierIso88598:
	case KCharacterSetIdentifierCodePage1252:
		return Versit::EUSAsciiCharSet;
	case KCharacterSetIdentifierUtf8:
	case KCharacterSetIdentifierJavaConformantUtf8:
		return Versit::EUTF8CharSet;
	case KCharacterSetIdentifierUtf7:
	case KCharacterSetIdentifierImapUtf7:
		return Versit::EUTF7CharSet;
	case KCharacterSetIdentifierIso88591:
		return Versit::EISO88591CharSet;
	case KCharacterSetIdentifierIso88592:
		return Versit::EISO88592CharSet;
	case KCharacterSetIdentifierIso88593:
		return Versit::EISO88593CharSet;
	case KCharacterSetIdentifierIso88594:
		return Versit::EISO88594CharSet;
	case KCharacterSetIdentifierIso88595:
		return Versit::EISO88595CharSet;
	case KCharacterSetIdentifierIso88597:
		return Versit::EISO88597CharSet;
	case KCharacterSetIdentifierIso88599:
		return Versit::EISO88599CharSet;
	case KCharacterSetIdentifierGb2312:
		return Versit::EGB231280CharSet;
	case KCharacterSetIdentifierGbk:
		return Versit::EGBKCharSet;
	case KCharacterSetIdentifierBig5:
		return Versit::EBIG5CharSet;
	case KCharacterSetIdentifierShiftJis:
		return Versit::EShiftJISCharSet;
	case KCharacterSetIdentifierJis:
		return Versit::EJISCharSet;
	case KCharacterSetIdentifierEucJpPacked:
		return Versit::EShiftJISCharSet;
	case KCharacterSetIdentifierIso2022Jp:
	case KCharacterSetIdentifierIso2022Jp1:
		return Versit::EShiftJISCharSet;

	default:
	case KCharacterSetIdentifierHz:
	case KCharacterSetIdentifierGb12345:
	case KCharacterSetIdentifierIso885910:
	case KCharacterSetIdentifierIso885913:
	case KCharacterSetIdentifierIso885914:
	case KCharacterSetIdentifierIso885915:
	case KCharacterSetIdentifierSms7Bit:
		return Versit::EUSAsciiCharSet;
		}
	}




