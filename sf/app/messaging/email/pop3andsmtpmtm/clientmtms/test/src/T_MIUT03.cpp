// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name of test harness: T_MIUT03
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Tests TImMessageField - tests for valid internet email addresses.
// Detailed description of test harness:
// As above.
// Functions not tested:
// ValidInternetEmailAddressChar(const TChar& aChar);	
// ValidSubjectLineChar(const TChar& aChar);
// GetValidInternetEmailAddressFromString(TDesC&, TInt&)
// GetValidInternetEmailAddressFromString(TDesC&, TInt&, TInt&)
// GetValidInternetEmailAddressFromString(TDesC&, TInt&, TInt&, TInt&)
// ValidAliasName(const TDesC16& aAliasName);
// ValidAliasName(const TDesC16& aAliasName, TInt& rFirstBadCharPos);
// Input files required to run test harness:
// None
// Intermediate files produced while running test harness:
// <DRIVE>:\msgtest\T_MIUT03\T_IMCM.log
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_MIUT03.<PLATFORM>.<VARIANT>.LOG.txt
// Description of how to build test harness:
// cd \msg\imcm\
// bldmake bldfiles
// abld test build
// Description of how to run test harness:
// The following instructions are the same for all platforms:
// 1. Build T_DB test harness from COMMDB component:
// cd \commdb\group
// bldmake bldfiles
// abld test build t_db
// 2. Build the test utilities:
// cd \msg\testutils\group\
// bldmake bldfiles
// abld build
// WINS running instructions:
// 1. \epoc32\release\wins\<VARIANT>\T_MIUT03.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_MIUT03.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_MIUT03.exe on the other platform
// 
//

#include "emailtestutils.h"
#include "miutpars.h"   // TImMessageField
#include <miutlog.h>
#include "miuthdr.h"    // CImHeader

// local variables etc //

RTest test(_L("MIUT03 - Test TImMessageField class"));
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CImLog* log;
LOCAL_D CEmailTestUtils* testUtils;

 //added test steps for PDEF119208:Can't send email with a especial symbol ">" in Your name field. 
 
 _LIT(KMiutFrom, "ralph>< <ralph-greenwell@psion.com>");
 _LIT(KMiutFrom1, "ralph<> <ralph-greenwell@psion.com>");
 _LIT(KMiutFrom2, "ralph> <ralph-greenwell@psion.com>");
 _LIT(KMiutFrom3, "ralph>>> <ralph-greenwell@psion.com>");
 _LIT(KMiutFrom4, "ralph< <ralph-greenwell@psion.com>");
 _LIT(KMiutFrom5, "ralph<<< <ralph-greenwell@psion.com>");
_LIT(KMiutFrom6, "David Noble Worrrkman <workmad3@gmail.com>");
//

LOCAL_C void ResultOfTest(TBool aResult,TInt aTestNo)
	{
	TBuf8<80> buf;
	_LIT8(KIMCMTestPassed, "PASSED Test %d");
	_LIT8(KIMCMTestFailed, "FAILED Test %d");
	if (aResult)
		{
		buf.AppendFormat(KIMCMTestPassed, aTestNo);
		test.Printf(TRefByValue<const TDesC>_L("Passed test %d\n"),aTestNo);
		log->AppendComment(buf);
		}
	else
		{
		buf.AppendFormat(KIMCMTestFailed, aTestNo);
		test.Printf(TRefByValue<const TDesC>_L("Failed test %d\n"),aTestNo);
		log->AppendComment(buf);
		}
	}

LOCAL_C TBool PassedTest(TBool aPassedTest, TDesC& aString, TBool aPass)
	{
	TBuf8<256> buf;
	TBuf8<256> stringBuf;
	if (aPass)
		{
		if (!aPassedTest)
			{
			_LIT8(KString, "\t\tString not valid: %S");
			stringBuf.Copy(aString);
			buf.AppendFormat(KString, &stringBuf);
			log->AppendError(buf,-1);
			}
		}
	else
		{
		if (aPassedTest)
			{
			_LIT8(KString, "\t\tString valid: %S");
			stringBuf.Copy(aString);
			buf.AppendFormat(KString, &stringBuf);
			log->AppendError(buf,-1);
			}
		}
	return (aPassedTest==aPass);
	}

LOCAL_C TBool CompareBadCharPos(TDesC& aString, TInt aBadPos1, TInt aBadPos2)
	{
	if (aBadPos1!=aBadPos2)
		{
		_LIT8(KString, "\t\tIncorrect bad character position in: %S");
		TBuf8<256> buf;
		TBuf8<256> stringBuf;
		stringBuf.Copy(aString);
		buf.AppendFormat(KString, &stringBuf);
		log->AppendError(buf,aBadPos1);
		}
	return (aBadPos1==aBadPos2);
	}

LOCAL_C TBool CompareFirstLastPos(TDesC& aString, TInt aFirstCharPos1, TInt aLastCharPos1, TInt aFirstCharPos2, TInt aLastCharPos2)
	{
	TBuf8<256> buf;
	TBuf8<256> stringBuf;
	if (aFirstCharPos1!=aFirstCharPos2)
		{
		_LIT8(KString, "\t\tIncorrect first character position in: %S");
		stringBuf.Copy(aString);
		buf.AppendFormat(KString, &stringBuf);
		log->AppendError(buf,aFirstCharPos1);
		}
	if (aLastCharPos1!=aLastCharPos2)
		{
		_LIT8(KString, "\t\tIncorrect last character position in: %S");
		stringBuf.SetLength(0);
		buf.SetLength(0);
		stringBuf.Copy(aString);
		buf.AppendFormat(KString, &stringBuf);
		log->AppendError(buf,aLastCharPos1);
		}
	return ((aFirstCharPos1==aFirstCharPos2)&&(aLastCharPos1==aLastCharPos2));
	}

LOCAL_C TBool CompareExtractedEmailAddress(TPtrC aExtractedString, TDesC& aString, TBool aPass)
	{
	TBuf<256> extractedStringBuf;
	extractedStringBuf.Copy(aExtractedString);
	_LIT8(KString, "\t\tWrong email address extracted : %S");

	if (((extractedStringBuf!=aString) & aPass)|((extractedStringBuf==aString) & !aPass))
		{
		TBuf8<256> buf;
		TBuf8<256> stringBuf;
		stringBuf.Copy(extractedStringBuf);
		buf.AppendFormat(KString, &stringBuf);
		log->AppendError(buf,-1);
		return EFalse;
		}
	return (((extractedStringBuf==aString) & aPass)|((extractedStringBuf!=aString) & !aPass));
	}

//
// TImMessageField Test Functions //
//

LOCAL_C TInt TestValidInternetEmailAddressFunction1(TBool aTest)
	{
	// This test tests the ValidInternetEmailAddress(TDesC&) function
	TBuf8<80> buf;
	_LIT8(KTest, "\tValidInternetEmailAddress(TDesC&) Test (Test %d)");
	buf.AppendFormat(KTest, aTest);
	log->AppendComment(buf);

	TBool result = ETrue;
	TImMessageField testMessageField;
	CImHeader* header = CImHeader::NewLC();
 	header->SetFromL(KMiutFrom);
 	header->SetFromL(KMiutFrom1);
 	header->SetFromL(KMiutFrom2);
 	header->SetFromL(KMiutFrom3);
 	header->SetFromL(KMiutFrom4);	
 	header->SetFromL(KMiutFrom5);		
	header->SetFromL(KMiutFrom6);		
	TBuf<80> string=_L("an.email@address.com");	// legal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

 	//added test steps for PDEF119205:Can't send email with a especial symbol ">" in Your name field.
 	string=_L("ralph>< <ralph-greenwell@psion.com>");
 	result &= PassedTest(testMessageField.ValidInternetEmailAddress(header->From()),string,ETrue);
 	
 	string=_L("ralph<> <ralph-greenwell@psion.com>");
 	result &= PassedTest(testMessageField.ValidInternetEmailAddress(header->From()),string,ETrue);
 	
 	string=_L("ralph> <ralph-greenwell@psion.com>");
 	result &= PassedTest(testMessageField.ValidInternetEmailAddress(header->From()),string,ETrue);
 	
 	string=_L("ralph>>> <ralph-greenwell@psion.com>");
 	result &= PassedTest(testMessageField.ValidInternetEmailAddress(header->From()),string,ETrue);
 	
 	string=_L("ralph< <ralph-greenwell@psion.com>");
 	result &= PassedTest(testMessageField.ValidInternetEmailAddress(header->From()),string,ETrue);
 	
 	string=_L("ralph<<< <ralph-greenwell@psion.com>");
 	result &= PassedTest(testMessageField.ValidInternetEmailAddress(header->From()),string,ETrue);
	
	string=_L("David Noble Worrrkman <workmad3@gmail.com>");
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(header->From()),string,ETrue);
	
	string=_L("An alias <an.email@address.com> (a comment)");	// legal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);
	
	string=_L("stuff <an.email@address>");		// valid 
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

	string=_L("<an.email@address.com> (a comment)");	// valid
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

	string=_L("  <an.email@address> no brackets");	// valid
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

/*	string=_L("send to test@psion.com by Thursday");	// valid
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);
*/
	string=_L("test@Psion <test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

	string=_L("\"Genet, Jean\"<test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

/*	string=_L("test test.psion.comç (test@Psion)");	// valid - illegal char in alias, but all chars in email address are actually valid
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

	string=_L("@ @ab bc@ a@c &<a@b>@");			// valid
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);
*/	
	string=_L("@ <ab@cd>");						// valid
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

	string=_L(" <a@c> @");						// valid
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

	string=_L("(comment before) an.email@address.com no brackets");	// valid
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

	string=_L("(comment before) an.email@address.com (a comment)");		// no alias
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

	string=_L("a@bc");							// valid
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

	string=_L("ab@c");							// valid
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

	string=_L("an.email@address>");				// illegal
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("<an.email@address");				// illegal
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("<@address.com>");				// illegal - "@" character not surrounded by other char
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("<address.com@>");				// illegal - "@" character not surrounded by other char
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("<an.address.com>");				// illegal - no "@" character
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("<@>");							// illegal - no substring surrounding "@"
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("  <an.email@address a comment");	// invalid - missing close angled bracket
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("an.email@address> a comment");	// invalid - missing open angled bracket
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("test@Psion <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("\" Genet,Jean \" <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("\"Jean Genet\" <test@psion@com> (test@Psion)");	// invalid - 2 '@' characters
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("test@Psion <test.psion.com> (test@Psion)");	// invalid - since <> don't contain legal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("");							// invalid - return empty string
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("\"\" <>");					// invalid
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("<>");						// invalid
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("A random bit of text");		// invalid
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("test@Psion <çtest@psion.com> (test@Psion)");			// invalid illegal char
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("test@Psion <test.psion.comç> (test@Psion)");		// invalid - illegal char
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("some stuff çtest@psion.com");		// invalid illegal char
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("word @ @start end@ test@home <test>");			// invalid - <> don't contain a valid email address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("@ <abc>");					// invalid - <> don't contain a valid email address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L(" <abc> @");					// invalid - <> don't contain a valid email address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("an.email@address.com, another.email@address.com");		// illegal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("an.email@address.com.another.email@address.com");		// illegal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("an.email@address.com,another.email@address.com");		// illegal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

/*	// this test should be invalid but it isn't at the moment (26/04/99)
	string=_L("an.email@address.com another.email@address.com");		// illegal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);
  */

	// new cases following changes for more rigourous checking
	string=_L("testing.@symbian.com");		// illegal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("testing.one.two@symbian-uk.com");		// legal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

	string=_L("~test@uk~symbian.com");		// illegal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);

	string=_L("c?e^o@megabytes.com");		// legal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

	string=_L("f#%ko$$!!.???@smartphone.com");		// legal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, ETrue);

	string=_L("clever.mobile@sm@rtphone.com");		// illegal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string), string, EFalse);


	ResultOfTest(result, aTest);
	CleanupStack::PopAndDestroy(header);
	return result;
	}

	LOCAL_C TInt TestValidInternetEmailAddressFunction2(TBool aTest)
	{
	// This test tests the ValidInternetEmailAddress(TDesC&, TInt&) function
	TBuf8<80> buf;
	_LIT8(KTest, "\tValidInternetEmailAddress(TDesC&, TInt&) Test (Test %d)");
	buf.AppendFormat(KTest, aTest);
	log->AppendComment(buf);

	TBool result = ETrue;
	TInt badCharPos = 0;
	TImMessageField testMessageField;
	TBuf<80> string=_L("an.email@address.com");	// legal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 0);
	
	string=_L("An alias <an.email@address.com> (a comment)");	// legal address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 10);
	
	string=_L("stuff <an.email@address>");		// valid 
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 7);

	string=_L("<an.email@address.com> (a comment)");	// valid
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 1);

	string=_L("  <an.email@address> no brackets");	// valid
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 3);

/*	string=_L("send to test@psion.com by Thursday");	// valid
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 8);
*/
	string=_L("test@Psion <test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 12);

	string=_L("\"Genet, Jean\"<test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 14);

/*	string=_L("test test.psion.comç (test@Psion)");	// valid - illegal char in alias, but all chars in email address are actually valid
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 18);

	string=_L("@ @ab bc@ a@c &<a@b>@");			// valid
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 16);
*/	
	string=_L("@ <ab@cd>");						// valid
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 3);

	string=_L(" <a@c> @");						// valid
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 2);

	string=_L("(comment before) an.email@address.com no brackets");	// valid
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 17);

	string=_L("(comment before) an.email@address.com (a comment)");		// no alias
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 17);

	string=_L("a@bc");							// valid
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 0);

	string=_L("ab@c");							// valid
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 0);

	string=_L("an.email@address>");				// illegal
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);

	string=_L("<an.email@address");				// illegal
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);

	string=_L("<@address.com>");				// illegal - "@" character not surrounded by other char
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);

	string=_L("<address.com@>");				// illegal - "@" character not surrounded by other char
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);

	string=_L("<an.address.com>");				// illegal - no "@" character
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);

	string=_L("<@>");							// illegal - no substring surrounding "@"
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);

	string=_L("  <an.email@address a comment");	// invalid - missing close angled bracket
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 2);

	string=_L("an.email@address> a comment");	// invalid - missing open angled bracket
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);

	string=_L("test@Psion <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 12);

	string=_L("\" Genet,Jean \" <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 16);

	string=_L("\"Jean Genet\" <test@psion@com> (test@Psion)");	// invalid - 2 '@' characters
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 14);

	string=_L("test@Psion <test.psion.com> (test@Psion)");	// invalid - since <> don't contain legal address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);

	string=_L("");							// invalid - return empty string
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);

	string=_L("\"\" <>");					// invalid
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);

	string=_L("<>");						// invalid
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);

	string=_L("A random bit of text");		// invalid
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);

	string=_L("test@Psion <çtest@psion.com> (test@Psion)");			// invalid illegal char
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 12);

	string=_L("test@Psion <test.psion.comç> (test@Psion)");		// invalid - illegal char
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);

	string=_L("some stuff çtest@psion.com");		// invalid illegal char
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 11);

	string=_L("word @ @start end@ test@home <test>");			// invalid - <> don't contain a valid email address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);

	string=_L("@ <abc>");					// invalid - <> don't contain a valid email address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);

	string=_L(" <abc> @");					// invalid - <> don't contain a valid email address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 7);

	string=_L("an.email@address.com, another.email@address.com");		// illegal address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);

	string=_L("an.email@address.com.another.email@address.com");		// illegal address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);

	string=_L("an.email@address.com,another.email@address.com");		// illegal address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);

/*	// this test should be invalid but it isn't at the moment (26/04/99)
	string=_L("an.email@address.com another.email@address.com");		// illegal address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);
  */

	// new cases following changes for more rigourous checking
	string=_L("testing.@symbian.com");		// illegal address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);

	string=_L("testing.one.two@symbian-uk.com");		// legal address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 0);

	string=_L("~test@uk~symbian.com");		// illegal address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);

	string=_L("c?e^o@megabytes.com");		// legal address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 0);

	string=_L("f#%ko$$!!.???@smartphone.com");		// legal address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 0);

	string=_L("clever.mobile@sm@rtphone.com");		// illegal address
	badCharPos = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);

	ResultOfTest(result, aTest);
	return result;
	}

	LOCAL_C TInt TestValidInternetEmailAddressFunction3(TBool aTest)
	{
	// This test tests the ValidInternetEmailAddress(TDesC&, TInt&, TInt&) function
	TBuf8<80> buf;
	_LIT8(KTest, "\tValidInternetEmailAddress(TDesC&, TInt&, TInt&) Test (Test %d)");
	buf.AppendFormat(KTest, aTest);
	log->AppendComment(buf);

	TBool result = ETrue;
	TInt leftChar = 0; 
	TInt rightChar = 0;
	TImMessageField testMessageField;
	TBuf<80> string=_L("an.email@address.com");	// legal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 19);
	
	string=_L("An alias <an.email@address.com> (a comment)");	// legal address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 10, 29);
	
	string=_L("stuff <an.email@address>");		// valid 
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 7, 22);

	string=_L("<an.email@address.com> (a comment)");	// valid
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 1, 20);

	string=_L("  <an.email@address> no brackets");	// valid
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 3, 18);

/*	string=_L("send to test@psion.com by Thursday");	// valid
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 9, 21);
*/
	string=_L("test@Psion <test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 12, 25);

	string=_L("\"Genet, Jean\"<test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 14, 27);

/*	string=_L("test test.psion.comç (test@Psion)");	// valid - illegal char in alias, but all chars in email address are actually valid
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 22, 31);

	string=_L("@ @ab bc@ a@c &<a@b>@");			// valid
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 16, 18);
*/	
	string=_L("@ <ab@cd>");						// valid
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 3, 7);

	string=_L(" <a@c> @");						// valid
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 2, 4);

	string=_L("(comment before) an.email@address.com no brackets");	// valid
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 17, 36);

	string=_L("(comment before) an.email@address.com (a comment)");		// no alias
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 17, 36);

	string=_L("a@bc");							// valid
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 3);

	string=_L("ab@c");							// valid
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 3);

	string=_L("an.email@address>");				// illegal
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 16);

	string=_L("<an.email@address");				// illegal
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 16);

	string=_L("<@address.com>");				// illegal - "@" character not surrounded by other char
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 12);

	string=_L("<address.com@>");				// illegal - "@" character not surrounded by other char
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 12);

	string=_L("<an.address.com>");				// illegal - no "@" character
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 0);

	string=_L("<@>");							// illegal - no substring surrounding "@"
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 1);

	string=_L("  <an.email@address a comment");	// invalid - missing close angled bracket
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 2, 18);

	string=_L("an.email@address> a comment");	// invalid - missing open angled bracket
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 16);

	string=_L("test@Psion <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 12, 26);

	string=_L("\" Genet,Jean \" <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 16, 30);

	string=_L("\"Jean Genet\" <test@psion@com> (test@Psion)");	// invalid - 2 '@' characters
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 14, 27);

	string=_L("test@Psion <test.psion.com> (test@Psion)");	// invalid - since <> don't contain legal address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 20);

	string=_L("");							// invalid - return empty string
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 0);

	string=_L("\"\" <>");					// invalid
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 0);

	string=_L("<>");						// invalid
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 0);

	string=_L("A random bit of text");		// invalid
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 0);

	string=_L("test@Psion <çtest@psion.com> (test@Psion)");			// invalid illegal char
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 12, 26);

	string=_L("test@Psion <test.psion.comç> (test@Psion)");		// invalid - illegal char
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 21);

	string=_L("some stuff çtest@psion.com");		// invalid illegal char
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 11, 25);

	string=_L("word @ @start end@ test@home <test>");			// invalid - <> don't contain a valid email address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 15);

	string=_L("@ <abc>");					// invalid - <> don't contain a valid email address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 0);

	string=_L(" <abc> @");					// invalid - <> don't contain a valid email address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 7, 0);

	string=_L("an.email@address.com, another.email@address.com");		// illegal address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 20);

	string=_L("an.email@address.com.another.email@address.com");		// illegal address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 45);

	string=_L("an.email@address.com,another.email@address.com");		// illegal address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 45);

/*	// this test should be invalid but it isn't at the moment (26/04/99)
	string=_L("an.email@address.com another.email@address.com");		// illegal address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 10, 29);
  */

	// new cases following changes for more rigourous checking
	string=_L("testing.@symbian.com");		// illegal address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 19);

	string=_L("testing.one.two@symbian-uk.com");		// legal address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 29);

	string=_L("~test@uk~symbian.com");		// illegal address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 19);

	string=_L("c?e^o@megabytes.com");		// legal address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 18 );

	string=_L("f#%ko$$!!.???@smartphone.com");		// legal address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, ETrue);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 27);

	string=_L("clever.mobile@sm@rtphone.com");		// illegal address
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, leftChar, rightChar), string, EFalse);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 27);

	ResultOfTest(result, aTest);
	return result;
	}

LOCAL_C TInt TestValidInternetEmailAddressFunction4(TBool aTest)
	{
	// This test tests the ValidInternetEmailAddress(TDesC&, TInt&, TInt&, TInt&) function
	TBuf8<80> buf;
	_LIT8(KTest, "\tValidInternetEmailAddress(TDesC&, TInt&, TInt&, TInt&) Test (Test %d)");
	buf.AppendFormat(KTest, aTest);
	log->AppendComment(buf);

	TBool result = ETrue;
	TInt badCharPos = 0;
	TInt leftChar = 0;
	TInt rightChar = 0;
	TImMessageField testMessageField;
	TBuf<80> string=_L("an.email@address.com");	// legal address
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 19, -1);
	
	string=_L("An alias <an.email@address.com> (a comment)");	// legal address
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 10);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 29, -1);
	
	string=_L("stuff <an.email@address>");		// valid 
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 7);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 22, -1);

	string=_L("<an.email@address.com> (a comment)");	// valid
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 20, -1);

	string=_L("  <an.email@address> no brackets");	// valid
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 3);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 18, -1);

/*	string=_L("send to test@psion.com by Thursday");	// valid
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 8);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 21, -1);
*/
	string=_L("test@Psion <test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 12);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 25, -1);

	string=_L("\"Genet, Jean\"<test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 14);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 27, -1);

/*	string=_L("test test.psion.comç (test@Psion)");	// valid - illegal char in alias, but all chars in email address are actually valid
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 18);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 31, -1);

	string=_L("@ @ab bc@ a@c &<a@b>@");			// valid
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 16);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 18, -1);
*/	
	string=_L("@ <ab@cd>");						// valid
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 3);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 7, -1);

	string=_L(" <a@c> @");						// valid
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 2);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 4, -1);

	string=_L("(comment before) an.email@address.com no brackets");	// valid
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 17);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 36, -1);

	string=_L("(comment before) an.email@address.com (a comment)");		// no alias
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 17);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 36, -1);

	string=_L("a@bc");							// valid
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 3, -1);

	string=_L("ab@c");							// valid
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 3, -1);

	string=_L("an.email@address>");				// illegal
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 16, 0);

	string=_L("<an.email@address");				// illegal
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 16, 0);

	string=_L("<@address.com>");				// illegal - "@" character not surrounded by other char
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 12, -1);

	string=_L("<address.com@>");				// illegal - "@" character not surrounded by other char
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 12, -1);

	string=_L("<an.address.com>");				// illegal - no "@" character
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, -1);

	string=_L("<@>");							// illegal - no substring surrounding "@"
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 1, -1);

	string=_L("  <an.email@address a comment");	// invalid - missing close angled bracket
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 2);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 18, 2);

	string=_L("an.email@address> a comment");	// invalid - missing open angled bracket
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 16, 0);

	string=_L("test@Psion <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 12);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 26, 12);

	string=_L("\" Genet,Jean \" <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 16);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 30, 16);

	string=_L("\"Jean Genet\" <test@psion@com> (test@Psion)");	// invalid - 2 '@' characters
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 14);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 27, 14);

	string=_L("test@Psion <test.psion.com> (test@Psion)");	// invalid - since <> don't contain legal address
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 20, -1);

	string=_L("");							// invalid - return empty string
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, -1);

	string=_L("\"\" <>");					// invalid
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, -1);

	string=_L("<>");						// invalid
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, -1);

	string=_L("A random bit of text");		// invalid
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, -1);

	string=_L("test@Psion <çtest@psion.com> (test@Psion)");			// invalid illegal char
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 12);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 26, 12);

	string=_L("test@Psion <test.psion.comç> (test@Psion)");		// invalid - illegal char
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 21, -1);

	string=_L("some stuff çtest@psion.com");		// invalid illegal char
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 11);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 25, 11);

	string=_L("word @ @start end@ test@home <test>");			// invalid - <> don't contain a valid email address
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 15, -1);

	string=_L("@ <abc>");					// invalid - <> don't contain a valid email address
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, -1);

	string=_L(" <abc> @");					// invalid - <> don't contain a valid email address
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 7);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 7);

	string=_L("an.email@address.com, another.email@address.com");		// illegal address
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 20, 0);

	string=_L("an.email@address.com.another.email@address.com");		// illegal address
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 45, 0);

	string=_L("an.email@address.com,another.email@address.com");		// illegal address
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 45, 0);

/*	// this test should be invalid but it isn't at the moment (26/04/99)
	string=_L("an.email@address.com another.email@address.com");		// illegal address
	badCharPos = 0;
	leftChar = 0;
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 29, 0);
  */
	
	// new cases following changes for more rigourous checking. 
	// N.B. These values are being passed in wrongly due to an historical error affecting the whole test harness
	// and are to be updated at a later date
	
	string=_L("testing.@symbian.com");		// illegal address
	badCharPos = 0;
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 19, 0);

	string=_L("testing.one.two@symbian-uk.com");		// legal address
	badCharPos = 0;
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 29, -1);

	string=_L("~test@uk~symbian.com");		// illegal address
	badCharPos = 0;
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 19, 0);

	string=_L("c?e^o@megabytes.com");		// legal address
	badCharPos = 0;
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 18, -1);

	string=_L("f#%ko$$!!.???@smartphone.com");		// legal address
	badCharPos = 0;
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, ETrue);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 27, -1);

	string=_L("clever.mobile@sm@rtphone.com");		// illegal address
	badCharPos = 0;
	leftChar = 0; 
	rightChar = 0;
	result &= PassedTest(testMessageField.ValidInternetEmailAddress(string, badCharPos, leftChar, rightChar), string, EFalse);
	result &= CompareBadCharPos(string, badCharPos, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 27, 0);
	
	ResultOfTest(result, aTest);
	return result;
	}


LOCAL_C TBool TestValidSubjectLineFunction(TInt aTest)
	{
	// This test tests the ValidSubjectLine() function
	// Most characters are valid and therefore all tests should be passed.
	// These tests have been incorporated from the original test harness for completeness
	TBuf8<80> buf;
	_LIT8(KTest, "\tValidSubjectLine() Test (Test %d)");
	buf.AppendFormat(KTest, aTest);
	log->AppendComment(buf);

	TBool result = ETrue;
	TInt badChar;
	TImMessageField testField;
	TBuf<80>string=_L("!£$%^&*()_+{}~@:;'#?><,.testing the subject line parser 1234567890");
	TBool result1=(testField.ValidSubjectLine(string,badChar)!=EFalse);
	if (!result1)
		log->AppendError(_L8("\t\tBad Subject Line (!£$%^&*()_+{}~@:;'#?><,.testing the subject line parser...)"),-1);
	result&=result1;

	string=_L("");
	TBool result2=(testField.ValidSubjectLine(string)!=EFalse);
	if (!result2)
		log->AppendError(_L8("\t\tBad Subject Line (Blank String)"),-1);
	result&=result2;

	string=_L("Keith de Mendonça");
	TBool result3=(testField.ValidSubjectLine(string,badChar)!=EFalse);
	if (!result3)
		log->AppendError(_L8("\t\tBad Subject Line (Keith de Mendonça)"),-1);
	result&=result3;
	
	string=_L("ça bad char at start of string");
	TBool result4=(testField.ValidSubjectLine(string,badChar)!=EFalse);
	if (!result4)
		log->AppendError(_L8("\t\tBad Subject Line (ça bad char at start of string)"),-1);
	result&=result4;
	
	string=_L("a bad char at end of stringç");
	TBool result5=(testField.ValidSubjectLine(string,badChar)!=EFalse);
	if (!result5)
		log->AppendError(_L8("\t\tBad Subject Line (a bad char at end of stringç)"),-1);
	result&=result5;
	
	string=_L("ç");
	TBool result6=(testField.ValidSubjectLine(string,badChar)!=EFalse);
	if (!result6)
		log->AppendError(_L8("\t\tBad Subject Line (ç)"),-1);
	result&=result6;

	ResultOfTest(result,aTest);
	return result;
	}
	
	
LOCAL_C TInt TestGetValidInternetEmailAddressFromString1(TBool aTest)
	{
	// This test tests the GetValidInternetEmailAddressFromString(TDesC&) function
	TBuf8<80> buf;
	_LIT8(KTest, "\tGetValidInternetEmailAddressFromString(TDesC&) Test (Test %d)");
	buf.AppendFormat(KTest, aTest);
	log->AppendComment(buf);

	TBool result = ETrue;
	TImMessageField testMessageField;
	TBuf<80> string=_L("an.email@address.com");	// legal address
	TBuf<80> extractedString = string;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);
	
	string=_L("An alias <an.email@address.com> (a comment)");	// legal address
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);
	
	string=_L("stuff <an.email@address>");		// valid 
	extractedString = _L("an.email@address");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);

	string=_L("<an.email@address.com> (a comment)");	// valid
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);

	string=_L("  <an.email@address> no brackets");	// valid
	extractedString = _L("an.email@address");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);

/*	string=_L("send to test@psion.com by Thursday");	// valid
	extractedString = _L("test@psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);
*/
	string=_L("test@Psion <test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	extractedString = _L("test@psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);

	string=_L("\"Genet, Jean\"<test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	extractedString = _L("test@psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);

/*	string=_L("test test.psion.comç (test@Psion)");	// valid - illegal char in alias, but all chars in email address are actually valid
	extractedString = _L("test@Psion");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);

	string=_L("@ @ab bc@ a@c &<a@b>@");			// valid
	extractedString = _L("a@b");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);
*/	
	string=_L("@ <ab@cd>");						// valid
	extractedString = _L("ab@cd");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);

	string=_L(" <a@c> @");						// valid
	extractedString = _L("a@c");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);

	string=_L("(comment before) an.email@address.com no brackets");	// valid
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);

	string=_L("(comment before) an.email@address.com (a comment)");		// no alias
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);

	string=_L("a@bc");							// valid
	extractedString = string;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);

	string=_L("ab@c");							// valid
	extractedString = string;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, ETrue);

	string=_L("an.email@address>");				// illegal
	extractedString = _L("an.email@address");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("<an.email@address");				// illegal
	extractedString = _L("an.email@address");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("<@address.com>");				// illegal - "@" character not surrounded by other char
	extractedString = _L("@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("<address.com@>");				// illegal - "@" character not surrounded by other char
	extractedString = _L("address.com@");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("<an.address.com>");				// illegal - no "@" character
	extractedString = _L("an.address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("<@>");							// illegal - no substring surrounding "@"
	extractedString = _L("@");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("  <an.email@address a comment");	// invalid - missing close angled bracket
	extractedString = _L("an.email@address");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("an.email@address> a comment");	// invalid - missing open angled bracket
	extractedString = _L("an.email@address");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

/*	string=_L("test@Psion <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	extractedString = _L("test@@psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("\" Genet,Jean \" <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	extractedString = _L("test@@psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("\"Jean Genet\" <test@psion@com> (test@Psion)");	// invalid - 2 '@' characters
	extractedString = _L("test@psion@com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);
*/
	string=_L("test@Psion <test.psion.com> (test@Psion)");	// invalid - since <> don't contain legal address
	extractedString = _L("test.psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

/*	string=_L("");							// invalid - return empty string
	extractedString = _L("");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);
*/
	string=_L("\"\" <>");					// invalid
	extractedString = _L("");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("<>");						// invalid
	extractedString = _L("");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("A random bit of text");		// invalid
	extractedString = _L("");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("test@Psion <çtest@psion.com> (test@Psion)");			// invalid illegal char
	extractedString = _L("çtest@psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("test@Psion <test.psion.comç> (test@Psion)");		// invalid - illegal char
	extractedString = _L("test.psion.comç");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("some stuff çtest@psion.com");		// invalid illegal char
	extractedString = _L("çtest@psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("word @ @start end@ test@home <test>");			// invalid - <> don't contain a valid email address
	extractedString = _L("test");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("@ <abc>");					// invalid - <> don't contain a valid email address
	extractedString = _L("abc");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L(" <abc> @");					// invalid - <> don't contain a valid email address
	extractedString = _L("abc");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("an.email@address.com, another.email@address.com");		// illegal address
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("an.email@address.com.another.email@address.com");		// illegal address
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

	string=_L("an.email@address.com,another.email@address.com");		// illegal address
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);

/*	// this test should be invalid but it isn't at the moment (26/04/99)
	string=_L("an.email@address.com another.email@address.com");		// illegal address
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string), extractedString, EFalse);
  */
	ResultOfTest(result, aTest);
	return result;
	}

/*LOCAL_C TInt TestGetValidInternetEmailAddressFromString2(TBool aTest)
	{
	// This test tests the GetValidInternetEmailAddressFromString(TDesC&, TInt&) function
	TBuf8<80> buf;
	_LIT8(KTest, "\tGetValidInternetEmailAddressFromString(TDesC&, TInt&) Test (Test %d)");
	buf.AppendFormat(KTest, aTest);
	log->AppendComment(buf);

	TBool result = ETrue;
	TInt error = 0;
	TImMessageField testMessageField;
	TBuf<80> string=_L("an.email@address.com");	// legal address
	TBuf<80> extractedString = string;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 0);
	
	string=_L("An alias <an.email@address.com> (a comment)");	// legal address
	error = 0;
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 10);
	
	string=_L("stuff <an.email@address>");		// valid 
	error = 0;
	extractedString = _L("an.email@address");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 7);

	string=_L("<an.email@address.com> (a comment)");	// valid
	error = 0;
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 1);

	string=_L("  <an.email@address> no brackets");	// valid
	error = 0;
	extractedString = _L("an.email@address");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 3);
*/
/*	string=_L("send to test@psion.com by Thursday");	// valid
	error = 0;
	extractedString = _L("test@psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 8);
*/
/*	string=_L("test@Psion <test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	error = 0;
	extractedString = _L("test@psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 12);

	string=_L("\"Genet, Jean\"<test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	error = 0;
	extractedString = _L("test@psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 14);
*/
/*	string=_L("test test.psion.comç (test@Psion)");	// valid - illegal char in alias, but all chars in email address are actually valid
	error = 0;
	extractedString = _L("test@Psion");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 18);

	string=_L("@ @ab bc@ a@c &<a@b>@");			// valid
	error = 0;
	extractedString = _L("a@b");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 16);
*/	
/*	string=_L("@ <ab@cd>");						// valid
	error = 0;
	extractedString = _L("ab@cd");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 3);

	string=_L(" <a@c> @");						// valid
	error = 0;
	extractedString = _L("a@c");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 2);

	string=_L("(comment before) an.email@address.com no brackets");	// valid
	error = 0;
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 17);

	string=_L("(comment before) an.email@address.com (a comment)");		// no alias
	error = 0;
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 17);

	string=_L("a@bc");							// valid
	error = 0;
	extractedString = string;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 0);

	string=_L("ab@c");							// valid
	error = 0;
	extractedString = string;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, ETrue);
	result &= CompareBadCharPos(string, error, 0);

	string=_L("an.email@address>");				// illegal
	error = 0;
	extractedString = _L("an.email@address");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, 0);

	string=_L("<an.email@address");				// illegal
	error = 0;
	extractedString = _L("an.email@address");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, 0);

	string=_L("<@address.com>");				// illegal - "@" character not surrounded by other char
	error = 0;
	extractedString = _L("@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, -1);

	string=_L("<address.com@>");				// illegal - "@" character not surrounded by other char
	error = 0;
	extractedString = _L("address.com@");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, -1);

	string=_L("<an.address.com>");				// illegal - no "@" character
	error = 0;
	extractedString = _L("an.address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, -1);

	string=_L("<@>");							// illegal - no substring surrounding "@"
	error = 0;
	extractedString = _L("@");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, -1);

	string=_L("  <an.email@address a comment");	// invalid - missing close angled bracket
	error = 0;
	extractedString = _L("an.email@address");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, 2);

	string=_L("an.email@address> a comment");	// invalid - missing open angled bracket
	error = 0;
	extractedString = _L("an.email@address");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, 0);
*/
/*	string=_L("test@Psion <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	error = 0;
	extractedString = _L("test@@psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, 12);

	string=_L("\" Genet,Jean \" <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	error = 0;
	extractedString = _L("test@@psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, 16);

	string=_L("\"Jean Genet\" <test@psion@com> (test@Psion)");	// invalid - 2 '@' characters
	error = 0;
	extractedString = _L("test@psion@com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, 14);
*/
/*	string=_L("test@Psion <test.psion.com> (test@Psion)");	// invalid - since <> don't contain legal address
	error = 0;
	extractedString = _L("test.psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, -1);
*/
/*	string=_L("");							// invalid - return empty string
	error = 0;
	extractedString = _L("");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, -1);
*/
/*	string=_L("\"\" <>");					// invalid
	error = 0;
	extractedString = _L("");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, -1);

	string=_L("<>");						// invalid
	error = 0;
	extractedString = _L("");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, -1);

	string=_L("A random bit of text");		// invalid
	error = 0;
	extractedString = _L("");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, -1);

	string=_L("test@Psion <çtest@psion.com> (test@Psion)");			// invalid illegal char
	error = 0;
	extractedString = _L("çtest@psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, 12);

	string=_L("test@Psion <test.psion.comç> (test@Psion)");		// invalid - illegal char
	error = 0;
	extractedString = _L("test.psion.comç");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, -1);

	string=_L("some stuff çtest@psion.com");		// invalid illegal char
	error = 0;
	extractedString = _L("çtest@psion.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, 11);

	string=_L("word @ @start end@ test@home <test>");			// invalid - <> don't contain a valid email address
	error = 0;
	extractedString = _L("test");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, -1);

	string=_L("@ <abc>");					// invalid - <> don't contain a valid email address
	error = 0;
	extractedString = _L("abc");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, -1);

	string=_L(" <abc> @");					// invalid - <> don't contain a valid email address
	error = 0;
	extractedString = _L("abc");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, 7);

	string=_L("an.email@address.com, another.email@address.com");		// illegal address
	error = 0;
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, 0);

	string=_L("an.email@address.com.another.email@address.com");		// illegal address
	error = 0;
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, 0);

	string=_L("an.email@address.com,another.email@address.com");		// illegal address
	error = 0;
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, 0);
*/
/*	// this test should be invalid but it isn't at the moment (26/04/99)
	string=_L("an.email@address.com another.email@address.com");		// illegal address
	error = 0;
	extractedString = _L("an.email@address.com");
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error), extractedString, EFalse);
	result &= CompareBadCharPos(string, error, 0);
  */
/*	ResultOfTest(result, aTest);
	return result;
	}
*/
/*
LOCAL_C TInt TestGetValidInternetEmailAddressFromString3(TBool aTest)
	{
	// This test tests the GetValidInternetEmailAddressFromString(TDesC&, TInt&, TInt&) function
	TBuf8<80> buf;
	_LIT8(KTest, "\tGetValidInternetEmailAddressFromString(TDesC&, TInt&, TInt&) Test (Test %d)");
	buf.AppendFormat(KTest, aTest);
	log->AppendComment(buf);

	TBool result = ETrue;
*/
/*	TInt leftChar = 0; 
	TInt rightChar = 0;
	TImMessageField testMessageField;
	TBuf<80> string=_L("an.email@address.com");	// legal address
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 19);
	
	string=_L("An alias <an.email@address.com> (a comment)");	// legal address
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 10, 29);
	
	string=_L("stuff <an.email@address>");		// valid 
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 7, 22);

	string=_L("<an.email@address.com> (a comment)");	// valid
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 1, 20);

	string=_L("  <an.email@address> no brackets");	// valid
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 3, 18);
*/
/*	string=_L("send to test@psion.com by Thursday");	// valid
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 9, 21);
*/
/*	string=_L("test@Psion <test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 12, 25);

	string=_L("\"Genet, Jean\"<test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 14, 27);
*/
/*	string=_L("test test.psion.comç (test@Psion)");	// valid - illegal char in alias, but all chars in email address are actually valid
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 22, 31);

	string=_L("@ @ab bc@ a@c &<a@b>@");			// valid
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 16, 18);
*/	
/*	string=_L("@ <ab@cd>");						// valid
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 3, 7);

	string=_L(" <a@c> @");						// valid
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 2, 4);

	string=_L("(comment before) an.email@address.com no brackets");	// valid
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 17, 36);

	string=_L("(comment before) an.email@address.com (a comment)");		// no alias
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 17, 36);

	string=_L("a@bc");							// valid
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 3);

	string=_L("ab@c");							// valid
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 3);

	string=_L("an.email@address>");				// illegal
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 16);

	string=_L("<an.email@address");				// illegal
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 16);

	string=_L("<@address.com>");				// illegal - "@" character not surrounded by other char
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 12);

	string=_L("<address.com@>");				// illegal - "@" character not surrounded by other char
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 12);

	string=_L("<an.address.com>");				// illegal - no "@" character
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 0);

	string=_L("<@>");							// illegal - no substring surrounding "@"
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 1);

	string=_L("  <an.email@address a comment");	// invalid - missing close angled bracket
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 2, 18);

	string=_L("an.email@address> a comment");	// invalid - missing open angled bracket
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 16);

	string=_L("test@Psion <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 12, 26);

	string=_L("\" Genet,Jean \" <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 16, 30);

	string=_L("\"Jean Genet\" <test@psion@com> (test@Psion)");	// invalid - 2 '@' characters
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 14, 27);

	string=_L("test@Psion <test.psion.com> (test@Psion)");	// invalid - since <> don't contain legal address
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 20);

	string=_L("");							// invalid - return empty string
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 0);

	string=_L("\"\" <>");					// invalid
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 0);

	string=_L("<>");						// invalid
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 0);

	string=_L("A random bit of text");		// invalid
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 0);

	string=_L("test@Psion <çtest@psion.com> (test@Psion)");			// invalid illegal char
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 12, 26);

	string=_L("test@Psion <test.psion.comç> (test@Psion)");		// invalid - illegal char
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 21);

	string=_L("some stuff çtest@psion.com");		// invalid illegal char
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 11, 25);

	string=_L("word @ @start end@ test@home <test>");			// invalid - <> don't contain a valid email address
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 15);

	string=_L("@ <abc>");					// invalid - <> don't contain a valid email address
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, -1, 0);

	string=_L(" <abc> @");					// invalid - <> don't contain a valid email address
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 7, 0);

	string=_L("an.email@address.com, another.email@address.com");		// illegal address
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 20);

	string=_L("an.email@address.com.another.email@address.com");		// illegal address
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 45);

	string=_L("an.email@address.com,another.email@address.com");		// illegal address
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 45);
*/
/*	// this test should be invalid but it isn't at the moment (26/04/99)
	string=_L("an.email@address.com another.email@address.com");		// illegal address
	leftChar = 0; 
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, leftChar, rightChar), string);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 10, 29);
  */
/*	ResultOfTest(result, aTest);
	return result;
	}
*/
/*
LOCAL_C TInt TestGetValidInternetEmailAddressFromString4(TBool aTest)
	{
	// This test tests the GetValidInternetEmailAddressFromString(TDesC&, TInt&, TInt&, TInt&) function
	TBuf8<90> buf;
	_LIT8(KTest, "\tGetValidInternetEmailAddressFromString(TDesC&, TInt&, TInt&, TInt&) Test (Test %d)");
	buf.AppendFormat(KTest, aTest);
	log->AppendComment(buf);

	TBool result = ETrue;
*/
/*	TInt error = 0;
	TInt leftChar = 0;
	TInt rightChar = 0;
	TImMessageField testMessageField;
	TBuf<80> string=_L("an.email@address.com");	// legal address
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 19, -1);
	
	string=_L("An alias <an.email@address.com> (a comment)");	// legal address
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 10);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 29, -1);
	
	string=_L("stuff <an.email@address>");		// valid 
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 7);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 22, -1);

	string=_L("<an.email@address.com> (a comment)");	// valid
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 20, -1);

	string=_L("  <an.email@address> no brackets");	// valid
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 3);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 18, -1);
*/
/*	string=_L("send to test@psion.com by Thursday");	// valid
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 8);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 21, -1);
*/
/*	string=_L("test@Psion <test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 12);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 25, -1);

	string=_L("\"Genet, Jean\"<test@psion.com> (test@Psion)");	// valid - should extract "<test@psion.com>"
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 14);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 27, -1);
*/
/*	string=_L("test test.psion.comç (test@Psion)");	// valid - illegal char in alias, but all chars in email address are actually valid
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 18);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 31, -1);

	string=_L("@ @ab bc@ a@c &<a@b>@");			// valid
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 16);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 18, -1);
*/	
/*	string=_L("@ <ab@cd>");						// valid
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 3);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 7, -1);

	string=_L(" <a@c> @");						// valid
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 2);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 4, -1);

	string=_L("(comment before) an.email@address.com no brackets");	// valid
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 17);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 36, -1);

	string=_L("(comment before) an.email@address.com (a comment)");		// no alias
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 17);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 36, -1);

	string=_L("a@bc");							// valid
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 3, -1);

	string=_L("ab@c");							// valid
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 3, -1);

	string=_L("an.email@address>");				// illegal
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 16, -1);

	string=_L("<an.email@address");				// illegal
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 16, -1);

	string=_L("<@address.com>");				// illegal - "@" character not surrounded by other char
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 12, -1);

	string=_L("<address.com@>");				// illegal - "@" character not surrounded by other char
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 12, -1);

	string=_L("<an.address.com>");				// illegal - no "@" character
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, -1);

	string=_L("<@>");							// illegal - no substring surrounding "@"
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 1, -1);

	string=_L("  <an.email@address a comment");	// invalid - missing close angled bracket
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 2);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 18, -1);

	string=_L("an.email@address> a comment");	// invalid - missing open angled bracket
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 16, -1);

	string=_L("test@Psion <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 12);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 26, 12);

	string=_L("\" Genet,Jean \" <test@@psion.com> (test@Psion)");	// invalid - 2 '@' characters
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 16);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 30, 16);

	string=_L("\"Jean Genet\" <test@psion@com> (test@Psion)");	// invalid - 2 '@' characters
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 14);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 27, 14);

	string=_L("test@Psion <test.psion.com> (test@Psion)");	// invalid - since <> don't contain legal address
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 20, -1);

	string=_L("");							// invalid - return empty string
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, -1);

	string=_L("\"\" <>");					// invalid
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, -1);

	string=_L("<>");						// invalid
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, -1);

	string=_L("A random bit of text");		// invalid
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, -1);

	string=_L("test@Psion <çtest@psion.com> (test@Psion)");			// invalid illegal char
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 12);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 26, 12);

	string=_L("test@Psion <test.psion.comç> (test@Psion)");		// invalid - illegal char
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 21, -1);

	string=_L("some stuff çtest@psion.com");		// invalid illegal char
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 11);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 25, 11);

	string=_L("word @ @start end@ test@home <test>");			// invalid - <> don't contain a valid email address
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 15, -1);

	string=_L("@ <abc>");					// invalid - <> don't contain a valid email address
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, -1);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, -1);

	string=_L(" <abc> @");					// invalid - <> don't contain a valid email address
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 7);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 0, 7);

	string=_L("an.email@address.com, another.email@address.com");		// illegal address
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 20, 0);

	string=_L("an.email@address.com.another.email@address.com");		// illegal address
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 45, 0);

	string=_L("an.email@address.com,another.email@address.com");		// illegal address
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 45, 0);
*/
/*	// this test should be invalid but it isn't at the moment (26/04/99)
	string=_L("an.email@address.com another.email@address.com");		// illegal address
	error = 0;
	leftChar = 0;
	rightChar = 0;
	result &= CompareExtractedEmailAddress(testMessageField.GetValidInternetEmailAddressFromString(string, error, leftChar, rightChar), string);
	result &= CompareBadCharPos(string, error, 0);
	result &= CompareFirstLastPos(string, leftChar, rightChar, 29, 0);
  */
/*	ResultOfTest(result, aTest);
	return result;
	}
*/
	LOCAL_C TBool TestGetValidAliasFunction(TInt aTest)
	{
	// This test tests the GetValidAlias(TDesC&, TInt&) function
	// The tests are also valid for GetValidAlias(TDesC16);
	// These tests have been incorporated from the original test harness for completeness
	TBuf8<80> buf;
	_LIT8(KTest, "\tGetValidAlias(TDesC&, TInt&) Test (Test %d)");
	buf.AppendFormat(KTest, aTest);
	log->AppendComment(buf);

	TBool result = ETrue;
	TImMessageField testField;
	TBuf<80>string=_L("An alias <an.email@address.com> (a comment)");	// valid address
	TBuf<80>output;
	TInt error;
	
	output=testField.GetValidAlias(string,error);
	_LIT(KFirstResult,"An alias");
	TBool result1=(error==KErrNone);
	if(output !=KFirstResult)
		result1=EFalse;

	if (!result1)
		log->AppendError(_L8("\t\tAn alias <an.email@address.com> (a comment) not valid"),-1);
	result&=result1;

	TImMessageField msgField;
	_LIT(KATest, "z <a@a.a>");
	_LIT(KATestResult, "z");
	TPtrC foo(msgField.GetValidAlias(KATest));
	if(foo!=KATestResult)
		{
		log->AppendError(_L8("\t\tGetValidAlias on \"z <a@a.a>\" didn't work"),-1);
		result=EFalse;
		}
	
	if(foo!=KATestResult)
		{
		log->AppendError(_L8("\t\tGetValidAlias on \"aa <a@a.a>\" didn't work"),-1);
		result=EFalse;
		}



	string=_L("<an.email@address.com> (a comment)");		// no alias
	output=testField.GetValidAlias(string,error);
	TBool result2=(error==KErrNotFound);
	if (!result2)
		log->AppendError(_L8("\t\t<an.email@address.com> (a comment) cannot be an alias"),-1);
	result&=result2;

	string=_L("<an.email@address.com> no brackets");		// no alias
	output=testField.GetValidAlias(string,error);
	TBool result3=(error==KErrNotFound);
	if (!result3)
		log->AppendError(_L8("\t\t<an.email@address.com> no brackets cannot be an alias"),-1);
	result&=result3;

	string=_L("<an.email@address.com>");					// no alias
	output=testField.GetValidAlias(string,error);
	TBool result4=(error==KErrNotFound);
	if (!result4)
		log->AppendError(_L8("\t\t<an.email@address.com> cannot be an alias"),-1);
	result&=result4;

	string=_L("    <an.email@address.com>");				// no alias (empty chars)
	output=testField.GetValidAlias(string,error);
	TBool result5=(error==KErrNotFound);
	if (!result5)
		log->AppendError(_L8("\t\t    <an.email@address.com> cannot be an alias"),-1);
	result&=result5;

	string=_L("just a normal sentence");					// no alias
	output=testField.GetValidAlias(string,error);
	TBool result6=(error==KErrNotFound);
	if (!result6)
		log->AppendError(_L8("\t\tjust a normal sentence cannot be an alias"),-1);
	result&=result6;

	string=_L("");											// no alias
	output=testField.GetValidAlias(string,error);
	TBool result7=(error==KErrNotFound);
	if (!result7)
		log->AppendError(_L8("\t\tEmpty String cannot be an alias"),-1);
	result&=result7;

	string=_L("Alias -> A <an.email@address.com> (a comment)");	// valid address
	output=testField.GetValidAlias(string,error);
	_LIT(KFirstResult1,"Alias -> A");
	TBool result8=(error==KErrNone);
	if(output !=KFirstResult1)
		result8=EFalse;

	if (!result8)
		log->AppendError(_L8("\t\tAlias -> A <an.email@address.com> (a comment) not valid"),-1);
	result&=result8;

	string=_L("Alias -< A <an.email@address.com> (a comment)");	// valid address
	output=testField.GetValidAlias(string,error);
	_LIT(KFirstResult2,"Alias -< A");
	TBool result9=(error==KErrNone);
	if(output !=KFirstResult2)
		result9=EFalse;

	if (!result9)
		log->AppendError(_L8("\t\tAlias -< A <an.email@address.com> (a comment) not valid"),-1);
	result&=result9;

	ResultOfTest(result,aTest);
	return result;
	}

LOCAL_C TBool TestGetValidCommentFunction(TInt aTest)
	{
	// This test tests the GetValidComment(TDesC&, TInt&) function
	// The tests are also valid for GetValidComment(TDesC16);
	// These tests have been incorporated from the original test harness for completeness
	TBuf8<80> buf;
	_LIT8(KTest, "\tGetValidComment(TDesC&, TInt&) Test (Test %d)");
	buf.AppendFormat(KTest, aTest);
	log->AppendComment(buf);

	TBool result = ETrue;
	TImMessageField testField;
	TBuf<80>string=_L("An alias <an.email@address.com> (a comment)");	// valid address
	TBuf<80>output;
	TInt error;
	//
	output=testField.GetValidComment(string,error);
	TBool result1=(error==KErrNone);
	if (!result1)
		log->AppendError(_L8("\t\tAn alias <an.email@address.com> (a comment) has no comment"),-1);
	result&=result1;

	string=_L("<an.email@address.com> (a comment)");		// no alias
	output=testField.GetValidComment(string,error);
	TBool result2=(error==KErrNone);
	if (!result2)
		log->AppendError(_L8("\t\t<an.email@address.com> (a comment) has a comment"),-1);
	result&=result2;

	string=_L("(comment before) an.email@address.com no brackets");			// invalid
	output=testField.GetValidComment(string,error);
	TBool result3=(error==KErrNotFound);
	if (!result3)
		log->AppendError(_L8("\t\t(comment before) an.email@address.com no brackets has a comment"),-1);
	result&=result3;

	string=_L("(comment before) an.email@address.com (a comment)");		// no alias
	output=testField.GetValidComment(string,error);
	TBool result4=(error==KErrNotFound);
	if (!result4)
		log->AppendError(_L8("\t\t(comment before) an.email@address.com (a comment) has a comment"),-1);
	result&=result4;

	string=_L("<an.email@address.com> no brackets");		// no alias
	output=testField.GetValidComment(string,error);
	TBool result5=(error==KErrNotFound);
	if (!result5)
		log->AppendError(_L8("\t\t<an.email@address.com> no brackets has a comment"),-1);
	result&=result5;

	string=_L("<an.email@address.com>");					// no alias
	output=testField.GetValidComment(string,error);
	TBool result6=(error==KErrNotFound);
	if (!result6)
		log->AppendError(_L8("\t\t<an.email@address.com> has a comment"),-1);
	result&=result6;

	string=_L("    <an.email@address.com>");				// no alias (empty chars)
	output=testField.GetValidComment(string,error);
	TBool result7=(error==KErrNotFound);
	if (!result7)
		log->AppendError(_L8("\t\t    <an.email@address.com> has a comment"),-1);
	result&=result7;

	string=_L("just a normal sentence");					// no alias
	output=testField.GetValidComment(string,error);
	TBool result8=(error==KErrNotFound);
	if (!result8)
		log->AppendError(_L8("\t\tjust a normal sentence has a comment"),-1);
	result&=result8;

	string=_L("");											// no alias
	output=testField.GetValidComment(string,error);
	TBool result9=(error==KErrNotFound);
	if (!result9)
		log->AppendError(_L8("\t\tEmpty String has a comment"),-1);
	result&=result9;

	ResultOfTest(result,aTest);

	return result;
	}

LOCAL_C void ParserFunctionSelectionL()
	{
	TBool result = ETrue;
	TInt testNo = 1;

	testUtils->TestStart(testNo);
	result &= TestValidInternetEmailAddressFunction1(testNo++);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	result &= TestValidInternetEmailAddressFunction2(testNo++);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	result &= TestValidInternetEmailAddressFunction3(testNo++);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	result &= TestValidInternetEmailAddressFunction4(testNo++);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	result &= TestValidSubjectLineFunction(testNo++);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	result &= TestGetValidInternetEmailAddressFromString1(testNo++);
	testUtils->TestFinish(testNo-1);

/*
	NOT YET SUPPORTED
	testUtils->TestStart(testNo);
	result &= TestGetValidInternetEmailAddressFromString2(testNo++);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	result &= TestGetValidInternetEmailAddressFromString3(testNo++);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	result &= TestGetValidInternetEmailAddressFromString4(testNo++);
	testUtils->TestFinish(testNo-1);
*/
	testUtils->TestStart(testNo);
	result &= TestGetValidAliasFunction(testNo++);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	result &= TestGetValidCommentFunction(testNo++);
	testUtils->TestFinish(testNo-1);

	if (result)
		{
		log->AppendComment(_L8("PASSED All tests Correctly"));
		test.Printf(_L("PASSED all Tests"));
		testUtils->TestHarnessCompleted();
		}
	else
		{
		test.Printf(_L("FAILED Tests"));
		log->AppendError(_L8("FAILED At Least one test"),-1);
		testUtils->TestHarnessFailed(KErrGeneral);
		}
	}

//

LOCAL_C void InitL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();

	log = CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"), EAppend);
	CleanupStack::PushL(log);
	log->AppendComment(_L8("********** T_MIUT03 Test TImMessageField class **********"));
	TBuf8<80> buf;

#if defined(__WINS__)	
	buf.Append(_L8("WINS "));
#else
	buf.Append(_L8("MARM "));
#endif
#if defined(_UNICODE)
	buf.Append(_L8("U"));
#endif
#if defined(_DEBUG)
	buf.Append(_L8("DEB"));
#else
	buf.Append(_L8("REL"));
#endif
	log->AppendComment(buf);
	}

LOCAL_C void Closedown()
	{
	log->AppendComment(_L8("************    T_MIUT03 Tests Complete    ************"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(3);  //testUtils, log, scheduler
	}


//

LOCAL_C void doMainL()
	{
	InitL();
	testUtils->GoClientSideL();
	test.Printf(_L("Performing Header Tests\n"));

	ParserFunctionSelectionL();

	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("MIUT_03 - Test TImMessageField class"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
