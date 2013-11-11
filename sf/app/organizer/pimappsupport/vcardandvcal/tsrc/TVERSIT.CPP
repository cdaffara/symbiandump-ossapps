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
 
#include "TVERSIT.H"
#include <vutil.h>
#include <charconv.h>
#include <e32test.h>
#include <coreappstest/testserver.h>

// Type definitions
#define UNUSED_VAR(a) a = a

// Currently no tests for multiple VCards in sequence in a stream

CConsoleBase* console;
CVersitTest* test=NULL;
TBool automatic=EFalse;
LOCAL_D CTestRegister* FileRegister;
#ifdef MANUAL
#define TheTest  (*console)
#define Next 	Printf
#else
static RTest TheTest(_L("TVersit"));
#endif

//
// CVersitTest
//

CVersitTest::CVersitTest()
	{}

CVersitTest::~CVersitTest()
	{
	delete iParser;
	delete iStore;
	}

void CVersitTest::StreamInL()
//Convert the iVersit into stream format, save it as "VersitIn"
//and internalize it as a CVersitParser
	{
	_LIT(KVersitInFileName, "VersitIn");
	RFs fsSession; 
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
	TFileName inFile;
	FileRegister->GetTempPath(inFile);
	inFile.Append(KVersitInFileName);
	fsSession.MkDirAll(inFile);
	FileRegister->RegisterL(inFile);
	CFileStore* store = CDirectFileStore::ReplaceLC(fsSession, inFile, EFileWrite);
    	store->SetTypeL(KDirectFileStoreLayoutUid);
	RStoreWriteStream outstream;
	TStreamId id = outstream.CreateLC(*store);
	TInt length=iVersit.Length();
	for (TInt ii=0; ii<length; ii++)
		outstream.WriteInt8L(iVersit[ii]);
	outstream.CommitL();
	CleanupStack::PopAndDestroy(); // outstream
	store->SetRootL(id);
	store->CommitL();
	CleanupStack::PopAndDestroy(store);
	store = CDirectFileStore::OpenLC(fsSession, inFile, EFileRead); //retrieve stream
	RStoreReadStream instream;
	instream.OpenLC(*store,store->Root());
	iParser->InternalizeL(instream);
	CleanupStack::PopAndDestroy(3); //store + stream + fsSession
	}

void CVersitTest::StreamOutL()
// externalize the CVersitParser into stream format, save the stream as "VersitOut"
// and output it to the console
	{
	_LIT(KVersitOutFileName, "VersitOut");
	RFs fsSession; // Save into stream
	User::LeaveIfError(fsSession.Connect());
	TFileName outFile;
	FileRegister->GetTempPath(outFile);
	outFile.Append(KVersitOutFileName);
	FileRegister->RegisterL(outFile);
	fsSession.MkDirAll(outFile);
	CFileStore* store = CDirectFileStore::ReplaceLC(fsSession, outFile, EFileWrite);
    store->SetTypeL(KDirectFileStoreLayoutUid);
	RStoreWriteStream outstream;
	TStreamId id = outstream.CreateLC(*store);
	iParser->ExternalizeL(outstream);
	outstream.CommitL();
	CleanupStack::PopAndDestroy();
	store->SetRootL(id);
	store->CommitL();
	CleanupStack::PopAndDestroy();
	
	store = CDirectFileStore::OpenLC(fsSession, outFile, EFileRead); //retrieve stream
	RStoreReadStream instream;
	instream.OpenLC(*store,store->Root());
	TInt err=0;
	TChar nextChar=0;
	iVersit.Zero();
#ifdef UNICODE
	CCnvCharacterSetConverter* unicodeConverter=CCnvCharacterSetConverter::NewLC();
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* arrayOfCharacterSetsAvailable=unicodeConverter->CreateArrayOfCharacterSetsAvailableL(fsSession);
	CleanupStack::PushL(arrayOfCharacterSetsAvailable);
	unicodeConverter->PrepareToConvertToOrFromL(0x100012b6, *arrayOfCharacterSetsAvailable, fsSession); // Code Page 1252
	unicodeConverter->SetReplacementForUnconvertibleUnicodeCharactersL(_L8("?"));
	CleanupStack::PopAndDestroy(); //arrayOfCharacterSetsAvailable
#endif	
	while (err==0)
		{
		if (nextChar!=CVersitParser::ELineFeed)
			{
			TRAP(err,nextChar=instream.ReadUint8L());
			if (err!=KErrEof)
				User::LeaveIfError(err);
			iVersit.Append(nextChar);
			}
		else
			{
#ifdef UNICODE
			HBufC16* output=HBufC16::NewLC(iVersit.Length());
			TInt state;
			TPtr16 temp = output->Des();
			TInt error = unicodeConverter->ConvertToUnicode(temp, iVersit, state);
			if (error!=KErrNone)
				User::Leave(error);
			TheTest.Printf(*output);
			CleanupStack::PopAndDestroy(); //output
#else
			TheTest.Printf(iVersit);
#endif
			if (!automatic)
				{
				TInt character=TheTest.Getch();
				if (character=='q') // character q quits
					break;
				}
			nextChar=0;
			iVersit.Zero();
			}
		}
#ifdef UNICODE
	CleanupStack::PopAndDestroy(); //unicodeConverter
#endif
	if (!automatic)
		{
		TheTest.Printf(_L("\n [press any key]\n"));
		TheTest.Getch(); // get and ignore character
		}
	CleanupStack::PopAndDestroy(2); //store +stream
	fsSession.Close();
	}

		
void CVersitTest::SaveFileVersitL(/*CVersitParser*/Versit::TVersitCharSet aDefaultCharSet)
// Save iParser into VersitFile	
	{
	if	(!iParser)
		return;

	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	fsSession.MkDirAll(_L("C:\\logs\\t_versit\\"));
	RFile file;
	// glenn hard code to test round trip
	iParser->SetDefaultCharSet(aDefaultCharSet);
	if (file.Replace(fsSession,_L("C:\\logs\\t_versit\\VersitFile"),EFileWrite)==KErrNone)
		iParser->ExternalizeL(file);
	file.Close();
	fsSession.Close();
	}

void CVersitTest::CheckIsValidLabelL()
	{
	TInt pos;
	if(CVersitParser::IsValidLabel(_L("abcdefghijklmnopqrstuvwxyz"), pos))
		TheTest.Printf(_L("\n test passed\n"));
	else
		TheTest.Printf(_L("\n test failed, pos = %d\n"), pos);

	if(CVersitParser::IsValidLabel(_L("abcd:"), pos))
		TheTest.Printf(_L("\n test failed, pos = %d\n"), pos);
	else
		TheTest.Printf(_L("\n test passed\n"));

	if(CVersitParser::IsValidLabel(_L("ab[c]d"), pos))
		TheTest.Printf(_L("\n test failed, pos = %d\n"), pos);
	else
		TheTest.Printf(_L("\n test passed\n"));

	if(CVersitParser::IsValidLabel(_L("abc]d"), pos))
		TheTest.Printf(_L("\n test failed, pos = %d\n"), pos);
	else
		TheTest.Printf(_L("\n test passed\n"));

	if(CVersitParser::IsValidLabel(_L(",abcd"), pos))
		TheTest.Printf(_L("\n test failed, pos = %d\n"), pos);
	else
		TheTest.Printf(_L("\n test passed\n"));
	}

//
// CVCalTest
//

void CVCalTest::LoadFileVersitL()
// Read VCalendar.vcs into iVersit	
	{
	if (iParser == NULL)
		iParser = CParserVCal::NewL();
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	RFile file;
	if (file.Open(fsSession,_L("C:\\VCalendar.vcs"),EFileRead)==KErrNone)
		{		
		TInt start=0;
		iParser->InternalizeL(file,start);
		}
	file.Close();
	fsSession.Close();
	}

void CVCalTest::BuildVersitL()
	{
	iParser = CParserVCal::NewL();
	iParser->SetDefaultCharSet(Versit::EISO88597CharSet);
	CParserPropertyValue* value = CParserPropertyValueHBufC::NewL(_L("TEST!!!"));
	CParserProperty* property = CParserProperty::NewL(*value, KVersitTokenDESCRIPTION, NULL);
	iParser->AddPropertyL(property); //takes ownership
	CParserVCalEntity* entity = CParserVCalEntity::NewL();
	entity->SetEntityNameL(KVersitVarTokenVTODO);
	CParserPropertyValue* value1 = CParserPropertyValueHBufC::NewL(_L("TEST!!!"));
	CParserProperty* property1 = CParserProperty::NewL(*value1, KVersitTokenDESCRIPTION, NULL);
	entity->AddPropertyL(property1);
	iParser->AddEntityL(entity);
	CArrayPtr<CVersitParser>* entities = iParser->EntityL(KVersitVarTokenVTODO,EFalse);	
	if (entities == NULL)
		User::Leave(KErrNotFound);
	CArrayPtr<CParserProperty>* properties = (*entities)[0]->PropertyL(/*KVersitVarTokenDESCRIPTION*/KVersitTokenDESCRIPTION, TUid::Uid(KVersitPropertyHBufCUid), EFalse);
	if (properties == NULL)
		User::Leave(KErrNotFound);
	delete entities;
	delete properties;
	}

//
// CVCardTest
//

void CVCardTest::LoadFileVersitL()
// Read VCard.vcf into iVersit	
	{
	if (iParser == NULL)
		iParser = CParserVCard::NewL();
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	RFile file;
	if (file.Open(fsSession,_L("C:\\VCard.vcf"),EFileRead)==KErrNone)
		{		
		TInt start=0;
		iParser->InternalizeL(file,start);
		}
	file.Close();
	fsSession.Close();
	}

void CVCardTest::BuildVersitL()
	{
	iParser = CParserVCard::NewL();
	iParser->SetDefaultCharSet(Versit::EISO88597CharSet);
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	CParserPropertyValue* value = CParserPropertyValueHBufC::NewL(_L("TEST!!!"));	
	CParserProperty* property = CParserGroupedProperty::NewL(*value, KVersitTokenFN, NULL, NULL);
	iParser->AddPropertyL(property); //takes ownership
	CArrayPtr<CParserProperty>* properties = iParser->PropertyL(KVersitTokenFN, TUid::Uid(KVersitPropertyHBufCUid), EFalse);
	if (property == NULL)
		User::Leave(KErrNotFound);
	delete properties;
	fsSession.Close();
	}

//
// Globals
//

void Help()
	{
	TheTest.Printf(_L("\n\n\nFiles must be placed in:"));
	TheTest.Printf(_L("\nC: for marm"));
	TheTest.Printf(_L("\n\\epoc32\\wins\\c for wins"));
	TheTest.Printf(_L("\n\nc:\\VersitIn file contains stream-internalized sample parse"));
	TheTest.Printf(_L("\nc:\\VersitOut file contains most recently stream-externalized parse"));
	TheTest.Printf(_L("\nc:\\VersitFile contains most recently file-externalized parse"));
	TheTest.Printf(_L("\nRename any sample file to be tested as suggested and choose options 1 and 2"));
	TheTest.Printf(_L("\nPress any key to scroll line by line through an externalized model"));
	TheTest.Printf(_L("\n'q' returns to menu"));
	TheTest.Printf(_L("\n(Press any key)\n\n"));
	TheTest.Getch();
	}

TInt Menu()
	{
	TheTest.Printf(_L("\n\n\nTest:"));
	TheTest.Printf(_L("\n\n1. Internalize c:\\vcalendar.vcs"));
	TheTest.Printf(_L("\n2. Internalize c:\\vcard.vcf"));
	TheTest.Printf(_L("\n3. Internalize full vcal test"));
	TheTest.Printf(_L("\n4. Internalize full vcard test"));
	TheTest.Printf(_L("\n5. Build up vcal dynamically"));
	TheTest.Printf(_L("\n6. Build up vcard dynamically"));
	TheTest.Printf(_L("\n7. Externalize current object"));
	TheTest.Printf(_L("\n8. Externalize object to c:\\VersitFile"));
	TheTest.Printf(_L("\n9. Externalize object to c:\\VersitFile using ISO-X charset"));
	TheTest.Printf(_L("\n0. Test IsValidlabel() method"));
	TheTest.Printf(_L("\na. Timed full vCard internalize x 100"));
	TheTest.Printf(_L("\nb. Internalize c:\\vcard.vcf (QP encoding, UTF8 charset)"));
	TheTest.Printf(_L("\nc. Internalize c:\\vcard.vcf (QP encoding, UTF7 charset)"));
	TheTest.Printf(_L("\nd. Internalize c:\\vcard.vcf (QP encoding, ISO x charsets)"));
	//
	//Change 26/01/2001
	//#001
	//Insert Start : Character sets for Chinese are added.
	TheTest.Printf(_L("\ne. Internalize c:\\vcard.vcf     (QP encoding, GBK charsets)"));
	TheTest.Printf(_L("\nf. Internalize c:\\vcard.vcf     (QP encoding, GB2312 charsets)"));
	TheTest.Printf(_L("\ng. Internalize c:\\vcard.vcf     (QP encoding, Big5 charsets)"));
	TheTest.Printf(_L("\ni. Internalize c:\\vcalendar.vcs (QP encoding, GBK charsets)"));
	TheTest.Printf(_L("\nj. Internalize c:\\vcalendar.vcs (QP encoding, GB2312 charsets)"));
	TheTest.Printf(_L("\nk. Internalize c:\\vcalendar.vcs (QP encoding, Big5 charsets)"));
	TheTest.Printf(_L("\nl. Internalize c:\\vcalendar.vcs (QP encoding, UTF8 charset)"));
	TheTest.Printf(_L("\nm. Internalize c:\\vcalendar.vcs (QP encoding, UTF7 charset)"));
	TheTest.Printf(_L("\nn. Externalize object to c:\\VersitFile (QP encoding Base64, GBK charset)"));
	TheTest.Printf(_L("\no. Externalize object to c:\\VersitFile (QP encoding Base64, GB2312 charset)"));
	TheTest.Printf(_L("\np. Externalize object to c:\\VersitFile (QP encoding Base64, Big5 charset)"));
	TheTest.Printf(_L("\nr. Externalize object to c:\\VersitFile (QP UTF-8 charset)"));
	TheTest.Printf(_L("\ns. Externalize object to c:\\VersitFile (QP UTF-7 charset)"));
	TheTest.Printf(_L("\nt. Externalize object to c:\\VersitFile (SHIFT_JIS charset)"));
	TheTest.Printf(_L("\nu. Internalize object from c:\\vCard.vcf (SHIFT_JIS charset)"));
	//Insert End
	//
	TheTest.Printf(_L("\nh. Help"));
	TheTest.Printf(_L("\nq. Quit\n\n"));
	return (TheTest.Getch());
	}

TInt DoTestL(TInt aChoice)
	{
	TInt error=KErrNone;
	switch(aChoice)
		{
		case 'h':
		case 'H':
			Help();
			break;
		case '1':
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\nVCalendar.vcs must be in c: in order to internalize it\n"));
			test=new(ELeave)CVCalTest;
			CleanupStack::PushL(test);
			TRAP(error,test->LoadFileVersitL());
			break;
		case '2':
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\nVCard.vcf must be in c: in order to internalize it\n"));
			test=new(ELeave)CVCardTest;
			CleanupStack::PushL(test);
			TRAP(error,test->LoadFileVersitL());
			break;
		case '3':
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			TheTest.Next(_L("\nCreate a sample vCal in file VersitIn and internalize it\n"));
			test=new(ELeave)CVCalTest;
			CleanupStack::PushL(test);
			test->GetSampleVersitL();
			TRAP(error,test->StreamInL()); // run tests
			break;
		case '4':
			{
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\nCreate a sample vCard in file VersitIn and internalize it\n"));
			test = new(ELeave) CVCardTest;
			CleanupStack::PushL(test);
			test->GetSampleVersitL();
			TRAP(error,test->StreamInL()); // run tests
			break;
			}
		case '5':
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\nBuild up vcal dynamically\n"));
			test=new(ELeave)CVCalTest;
			CleanupStack::PushL(test);
			test->BuildVersitL();
			break;
		case '6':
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\nBuild up vcard dynamically\n"));
			test=new(ELeave)CVCardTest;
			CleanupStack::PushL(test);
			test->BuildVersitL();
			
			
			TheTest.Next(_L("\nExternalize object\n"));
			if (test)
				TRAP(error,test->StreamOutL()); // run tests
			break;
		case '7':
			
			
			TheTest.Next(_L("\nExternalize object\n"));
			if (test)
				TRAP(error,test->StreamOutL()); // run tests
			break;
		case '8':
			
			
			TheTest.Next(_L("\n Externalize object to VersitFile\n"));
			if (test)
				TRAP(error,test->SaveFileVersitL()); // run tests
			break;
		case '9':
			
			
			TheTest.Next(_L("\n Externalize object to VersitFile using ISO-X charset\n"));
			if (test)
				TRAP(error,test->SaveFileVersitL(Versit::EISO88592CharSet)); // run tests
			break;
		case '0':
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\n Test IsValidLabel()\n"));
			test=new(ELeave)CVCalTest;
			CleanupStack::PushL(test);
			test->CheckIsValidLabelL(); // run tests
			break;
		case 'a':
		case 'A':
			{
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\n VCard test internalize x 100\n"));
			test = new(ELeave) CVCardTest;
			CleanupStack::PushL(test);
			TTime start;
			start.UniversalTime();
			TTime finish;
			TInt i;
			for(i = 0; i < 100 && error ==KErrNone; i++)
				{
				test->GetSampleVersitL();
				TRAP(error, test->StreamInL()); // run tests
				delete test->iParser;	// delete parser after each test or it gets HUGE and we run out of memory
				test->iParser=NULL;
				}
			finish.UniversalTime();
			TheTest.Printf(_L("\nTook %d microseconds for %i tests\n"), finish.MicroSecondsFrom(start).Int64(), i);
			break;
			}

		case 'b':
		case 'B':
			{
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\n VCard.vcf internalize (QP encoding, UTF8 charset)\n"));
			test = new(ELeave) CVCardTest;
			CleanupStack::PushL(test);
			test->iParser = CParserVCard::NewL();
 			test->iParser->SetDefaultCharSet(Versit::EUTF8CharSet);
			TRAP(error,test->LoadFileVersitL());
			break;
			}
		case 'c':
		case 'C':
			{
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\n VCard.vcf internalize (QP encoding, UTF7 charset)\n"));
			test = new(ELeave) CVCardTest;
			CleanupStack::PushL(test);
			test->iParser = CParserVCard::NewL();
 			test->iParser->SetDefaultCharSet(Versit::EUTF7CharSet);
			TRAP(error,test->LoadFileVersitL());
			break;
			}
		case 'd':
		case 'D':
			{
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\n VCard.vcf internalize (QP encoding, ISO X charset)\n"));
			test = new(ELeave) CVCardTest;
			CleanupStack::PushL(test);
			test->iParser = CParserVCard::NewL();
			test->iParser->SetDefaultEncoding(Versit::EQuotedPrintableEncoding);
 			test->iParser->SetDefaultCharSet(Versit::EISO88592CharSet);
			TRAP(error,test->LoadFileVersitL());
			if (error)
				break;

			test->iParser->SetDefaultCharSet(Versit::EISO88592CharSet);
			TRAP(error,test->LoadFileVersitL());
			if (error)
				break;

			test->iParser->SetDefaultCharSet(Versit::EISO88594CharSet);
			TRAP(error,test->LoadFileVersitL());
			if (error)
				break;

			test->iParser->SetDefaultCharSet(Versit::EISO88595CharSet);
			TRAP(error,test->LoadFileVersitL());
			if (error)
				break;

			test->iParser->SetDefaultCharSet(Versit::EISO88597CharSet);
			TRAP(error,test->LoadFileVersitL());
			if (error)
				break;

			test->iParser->SetDefaultCharSet(Versit::EISO88599CharSet);
			TRAP(error,test->LoadFileVersitL());
			break;
			}
		//
		//Change 31/01/2001
		//#002
		//Insert Start : Character sets for Chinese are added.
		// This is to test the default character sets and encodings for decoding non-standard Versit object in vCard.
		case 'e': //GBK
		case 'E':
			{
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			TheTest.Next(_L("\n VCard.vcf internalize (QP encoding, GBK charset)\n"));
			test = new(ELeave) CVCardTest;
			CleanupStack::PushL(test);
			test->iParser = CParserVCard::NewL();
			test->iParser->SetDefaultEncoding(Versit::ENoEncoding);
			test->iParser->SetDefaultCharSet(Versit::EGBKCharSet);
			TRAP(error,test->LoadFileVersitL());
			break;
			}
		case 'f': //GB2312
		case 'F':
			{
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\n VCard.vcf internalize (QP encoding, GB2312 charset)\n"));
			test = new(ELeave) CVCardTest;
			CleanupStack::PushL(test);
			test->iParser = CParserVCard::NewL();
			test->iParser->SetDefaultEncoding(Versit::ENoEncoding);
 			test->iParser->SetDefaultCharSet(Versit::EGB231280CharSet);
			TRAP(error,test->LoadFileVersitL());
			break;
			}
		case 'g': //Big5
		case 'G':
			{
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\n VCard.vcf internalize (QP encoding, Big5 charset)\n"));
			test = new(ELeave) CVCardTest;
			CleanupStack::PushL(test);
			test->iParser = CParserVCard::NewL();
			test->iParser->SetDefaultEncoding(Versit::ENoEncoding);
 			test->iParser->SetDefaultCharSet(Versit::EBIG5CharSet);
			TRAP(error,test->LoadFileVersitL());
			break;
			}
		// This is to test the default character sets and encodings for decoding non-standard Versit object in vCal.
		case 'i': //GBK
		case 'I':
			{
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\nVCalendar.vcs internalize (QP encoding, GBK charset)\n"));
			test = new(ELeave) CVCalTest;
			CleanupStack::PushL(test);
			test->iParser = CParserVCal::NewL();
			test->iParser->SetDefaultEncoding(Versit::ENoEncoding);
			test->iParser->SetDefaultCharSet(Versit::EGBKCharSet);
			TRAP(error,test->LoadFileVersitL());
			break;
			}
		case 'j': //GB2312
		case 'J':
			{
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\n VCalendar.vcs internalize (QP encoding, GB2312 charset)\n"));
			test = new(ELeave) CVCalTest;
			CleanupStack::PushL(test);
			test->iParser = CParserVCal::NewL();
			test->iParser->SetDefaultEncoding(Versit::ENoEncoding);
 			test->iParser->SetDefaultCharSet(Versit::EGB231280CharSet);
			TRAP(error,test->LoadFileVersitL());
			break;
			}
		case 'k': //Big5
		case 'K':
			{
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			TheTest.Next(_L("\n@VCalendar.vcs internalize (QP encoding, Big5 charset)\n"));
			test = new(ELeave) CVCalTest;
			CleanupStack::PushL(test);
			test->iParser = CParserVCal::NewL();
			test->iParser->SetDefaultEncoding(Versit::ENoEncoding);
 			test->iParser->SetDefaultCharSet(Versit::EBIG5CharSet);
			TRAP(error,test->LoadFileVersitL());
			break;
			}
		case 'l':
		case 'L':
			{
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\nVCalendar.vcs internalize (QP encoding, UTF8 charset)\n"));
			test = new(ELeave) CVCalTest;
			CleanupStack::PushL(test);
			test->iParser = CParserVCal::NewL();
			test->iParser->SetDefaultEncoding(Versit::ENoEncoding);
 			test->iParser->SetDefaultCharSet(Versit::EUTF8CharSet);
			TRAP(error,test->LoadFileVersitL());
			break;
			}
		case 'm':
		case 'M':
			{
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\nVCalendar.vcs internalize (QP encoding, UTF7 charset)\n"));
			test = new(ELeave) CVCalTest;
			CleanupStack::PushL(test);
			test->iParser = CParserVCal::NewL();
			test->iParser->SetDefaultEncoding(Versit::ENoEncoding);
 			test->iParser->SetDefaultCharSet(Versit::EUTF7CharSet);
			TRAP(error,test->LoadFileVersitL());
			break;
			}
		// This is to test the default character sets and encodings of GBK/GB2312/Big5 for the encoding versit object.
		case 'n': //GBK
		case 'N':
			{
			
			TheTest.Next(_L("\nExternalize object to VersitFile (QP encoding Base64, GBK charset)\n"));
			if (test)
				if (test->iParser)
					{
					test->iParser->SetDefaultEncoding(Versit::EBase64Encoding);
					TRAP(error,test->SaveFileVersitL(Versit::EGBKCharSet)); // run tests
				}
				break;
			}
		case 'o': //GB2312
		case 'O':
			{
				
			TheTest.Next(_L("\nExternalize object to VersitFile (QP encoding Base64, GB2312 charset)\n"));
			if (test)
				if (test->iParser)
					{
					test->iParser->SetDefaultEncoding(Versit::EBase64Encoding);
					TRAP(error,test->SaveFileVersitL(Versit::EGB231280CharSet)); // run tests
				}
				break;
			}
		case 'p': //Big5
		case 'P':
			{
				
			TheTest.Next(_L("\nExternalize object to VersitFile (QP encoding Base64, Big5 charset)\n"));
			if (test)
				if (test->iParser)
					{
					test->iParser->SetDefaultEncoding(Versit::EBase64Encoding);
					TRAP(error,test->SaveFileVersitL(Versit::EBIG5CharSet)); // run tests
				}
				break;
			}
		case 'r': //UTF-8
		case 'R':
			{
				
			TheTest.Next(_L("\nExternalize object to VersitFile (QP UTF-8 charset)\n"));
			if (test)
				if (test->iParser)
					{
					TRAP(error,test->SaveFileVersitL(Versit::EUTF8CharSet)); // run tests
				}
				break;
			}
		case 's': //UTF-7
		case 'S':
			{
			
			TheTest.Next(_L("\nExternalize object to VersitFile (QP UTF-7 charset)\n"));
			if (test)
				if (test->iParser)
					{
					TRAP(error,test->SaveFileVersitL(Versit::EUTF7CharSet)); // run tests
				}
				break;
			}
		case 't': //SHIFT_JIS externalize
		case 'T':
			{
			
			TheTest.Next(_L("\nExternalize object to VersitFile (SHIFT_JIS charset)\n"));
			if (test)
				if (test->iParser)
					{
					TRAP(error,test->SaveFileVersitL(Versit::EShiftJISCharSet)); // run tests
				}
				break;
			}
		case 'u': //SHIFT_JIS internalize
		case 'U':
			{
			if (test)
				CleanupStack::PopAndDestroy(test);
			
			
			TheTest.Next(_L("\nvCard.vcf internalize (SHIFT_JIS charset)\n"));
			test = new(ELeave) CVCardTest;
			CleanupStack::PushL(test);
			test->iParser = CParserVCard::NewL();
			test->iParser->SetDefaultEncoding(Versit::ENoEncoding);
			test->iParser->SetDefaultCharSet(Versit::EShiftJISCharSet);
			TRAP(error,test->LoadFileVersitL());
			break;
			}
		case 'z':
		case 'Z':
			{
			TInt testnumber='0';
			automatic=ETrue;
			FOREVER
				{
				if (DoTestL(testnumber)!=KErrNone)
					{
					_LIT(KContinue,"\n\nFAILED!!!  Test: '%c',   Press a key to continue\n");
					TheTest.Printf(KContinue, testnumber);
					TheTest.Getch();
					}
				switch (testnumber)
					{
				case '9':
					testnumber='a';
					break;
				case 'u':
					automatic=EFalse;
					return KErrNone;
				case 'g':
				case 'p':
					++testnumber;
				default:
					++testnumber;
					}
				}
			}
		default:
			{
			TheTest.Printf(_L("\nPressing wrong key"));
			error=KErrGeneral;
			break;
			}
		};
#ifdef MANUAL
	if (error) 
		{
		TheTest.Printf(_L("\n Failed: leave code=%d\n"), error);
		User::LeaveIfError(error);
		}
	else
		TheTest.Printf(_L("\nTest passed!"));
#else		
	TheTest(error == KErrNone);
#endif		
	return error;
	}

void SetupConsoleL() // initialize and call example code under cleanup stack
    {
	console=Console::NewL(_L("VersitTest"),
		TSize(76,18));
	CleanupStack::PushL(console);
	//TInt choice=Menu();
	DoTestL('z');		//Perform all tests.
	TInt choice='q';	//And quit

	while (!(choice=='q'||choice=='Q'))
		{
		DoTestL(choice);
		console->Printf(_L("\n\nPress any key to see available options, \ne.g. either quit or performing other testings"));
		console->Getch();
		choice=Menu();
		}
	if (test)
		CleanupStack::PopAndDestroy(test);
	CleanupStack::PopAndDestroy(console);
    }


void DoAllTestsL() // initialize and call all tests
	{
	FileRegister = CTestRegister::NewLC();

	DoTestL('z');		//Perform all tests.
	if (test)
		{
		CleanupStack::PopAndDestroy(test);
		}

	CleanupStack::PopAndDestroy(FileRegister);
	}

/**
@SYMTestCaseID PIM-TVERSIT-0001
*/	
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
#ifdef MANUAL
	TRAPD(error,SetupConsoleL());
#else
	TheTest.Start(_L("@SYMTestCaseID PIM-TVERSIT-0001 TVersit"));
	TRAPD(error,DoAllTestsL());
	UNUSED_VAR(error); //Used to supress build warnings
	
	TheTest.End();
	TheTest.Close();
#endif
	delete cleanup;
	__UHEAP_MARKEND;
	return (KErrNone);
	}
