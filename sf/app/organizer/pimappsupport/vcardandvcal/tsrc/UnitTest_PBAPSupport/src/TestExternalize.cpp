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

// User includes
#include "TestExternalize.h"

// Constants
_LIT(KTestName, "TestExternalize");

void CTests::DoTestsL()
	{
	CTests* self = new(ELeave) CTests();
	CleanupStack::PushL(self);
	self->ConstructL();
	self->UnitTestsL();
	CleanupStack::PopAndDestroy(self);
	}
	
CTests::CTests()
	: iTest(KTestName)
 	{
 	}
 	
CTests::~CTests()
	{
	delete iUtil;
	iTest.Close();
	}

void CTests::ConstructL()
	{	
	iUtil = CUtils::NewL(iTest);
	}
	
/**
@SYMTestCaseID PIM-APPSERV-TESTEXTERNALIZE-0001	
*/
void CTests::UnitTestsL()
	{
	iTest.Title(); 
	iTest.Start(_L("@SYMTestCaseID PIM-APPSERV-TESTEXTERNALIZE-0001 Unit Tests"));
		
	TestsL();
	
	iTest.End();	
	}
	
/** 
 * File Tests - externalise a vCard entity to a file and compare the 
 * result with the expecting data.
 */
void CTests::TestsL()
	{	
	_LIT(KTest1,"Line wrapping");
	
	iTest.Next(KTest1);
	LineWrappingTestL();
	
	_LIT(KTest2,"Charset");
	
	iTest.Next(KTest2);
	CharsetTestL();
	
	_LIT(KTest3,"Encoding");
	
	iTest.Next(KTest3);
	EncodingTestL();
	
	_LIT(KTest4,"Escape special characters");
	
	iTest.Next(KTest4);
	EscapingTestL();
	
	_LIT(KTest5,"Replace CRLF with backslash escaped (n Or N)");
	
	iTest.Next(KTest5);
	ReplaceCRLFTestL();
	
	_LIT(KTestEnd,"Finished Unit Tests");
	
	iTest.Next(KTestEnd);
	}

//Unit Tests

/**
 * @SYMPREQ 1375
 * @SYMTestType UT
 * @SYMTestPriority Critical
 * @SYMTestCaseDesc Folding of exported lines after 75 characters
 * 
 * @SYMTestActions
 * 1.Externalize a vCard entity and all of its properties to a file.
 * 2.Compare externalized file with expected output.
 *
 * @SYMTestExpectedResults for the above tests
 * 1.Externalized line exceeding 75 characters is folded into lines of maximum length of 75 characters.
 * 2.Comparison of expected vCard should pass.
 * 3.No leaves or panics should occur.
 */
void CTests::LineWrappingTestL()
	{
	/** Line wrapping in Ascii.
		Long lines should be folded. */
	
	//	
	// Short lines
	//	
			
	_LIT(KFile1, "ShortAsciiLine");
	_LIT8(KVCardData1, "BEGIN:VCARD\r\nDESCRIPTION;paramname=paramvalue:short line\r\nEND:VCARD\r\n");
	TFileName outputFile1;
    
    iUtil->CreateAndExternaliseVCardL(KVCardData1, KFile1, outputFile1);
    
    //compare files to make sure that no lines wrapped
	iUtil->CheckVCardL(KFile1, outputFile1);
	
	       
	//	
	// Long property name
	//	
	
	_LIT(KFile2, "LongPropertyName");
    _LIT8(KVCardData2, "BEGIN:VCARD\r\nDESCRIPTIONaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa;paramname=paramvalue:some text\r\nEND:VCARD\r\n");
    TFileName outputFile2;
    
    iUtil->CreateAndExternaliseVCardL(KVCardData2, KFile2, outputFile2);
    
	//compare files to make sure that long lines wrapped
    iUtil->CheckVCardL(KFile2, outputFile2);
    
    
	//		
	// Long property parameter
	//	
	
	_LIT(KFile3, "LongPropertyParam");
	_LIT8(KVCardData3, "BEGIN:VCARD\r\nDESCRIPTION;paramname=paramvalueeeeeeeeeeeeee is a long line and needs to be folded:some text\r\nEND:VCARD\r\n");
	TFileName outputFile3;
    
    iUtil->CreateAndExternaliseVCardL(KVCardData3, KFile3, outputFile3);
    
	//compare files to make sure that long lines wrapped
    iUtil->CheckVCardL(KFile3, outputFile3);
    
    
	//	
	// Long property value
	//
	
	_LIT(KFile4, "LongPropertyValue");
	_LIT8(KVCardData4, "BEGIN:VCARD\r\nDESCRIPTION;paramname=paramvalue:this isa long line and this needs folding.Fold here\r\nEND:VCARD\r\n");
	TFileName outputFile4;
    
    iUtil->CreateAndExternaliseVCardL(KVCardData4, KFile4, outputFile4);
    
	//compare files to make sure that long lines wrapped
    iUtil->CheckVCardL(KFile4, outputFile4);	
    
	
	/** Line wrapping in Base64 encoding.
		Long lines should be folded. */
			 
	//		 
	// Long binary property value
	//	
	  
	_LIT(KFileBase64, "Base64LineFolding");
	
	//compare files to make sure that long lines wrapped
	iUtil->TestFileL(KFileBase64);
	}
 
/**
 * @SYMPREQ 1375
 * @SYMTestType UT
 * @SYMTestPriority Critical
 * @SYMTestCaseDesc CHARSET type parameter has been eliminated during externalize.
 *
 * @SYMTestActions
 * 1.Externalize a vCard entity and all of its properties to a file.
 * 2.Compare externalized file with expected output.
 *
 * @SYMTestExpectedResults for the above tests
 * 1.CHARSET parameter is deleted in externalised file.
 * 2.UTF-8 charset is used for encoding of non ascii property value.
 * 3.Comparison of expected vCard should pass.
 * 4.No leaves or panics should occur.
 */
void CTests::CharsetTestL()
	{
	//	   
	// FN property with charset parameter
	//
			
	_LIT(KFile1, "DisableCharset");
	_LIT8(KVCardData1, "BEGIN:VCARD\r\nFN;CHARSET=ISO-8859-1:A N Example\r\nEND:VCARD\r\n");
	TFileName outputFile1;
    
    iUtil->CreateAndExternaliseVCardL(KVCardData1, KFile1, outputFile1);
    
	//compare files to make sure that charset parameter is disabled
    iUtil->CheckVCardL(KFile1, outputFile1);
    

	//	   
	// NOTE property with non ascii characters 
	//
	
    _LIT(KFile2, "UseUTF8");
 	_LIT(KNote, "about jäkë"); //note has accented letters.
	TFileName outputFile2;
	
	iUtil->CreateAndExternaliseVCardL(KNote, KFile2, outputFile2);
    
	//compare files to make sure that UTF-8 charset is used for encoding.
    iUtil->CheckVCardL(KFile2, outputFile2);
	}

/**
 * @SYMPREQ 1375
 * @SYMTestType UT
 * @SYMTestPriority Critical
 * @SYMTestCaseDesc
 * 1.Inline encoding for binary property values.
 * 2.No Inline quoted-printable encoding for non binary properties.
 *
 * @SYMTestActions
 * 1.Externalize a vCard entity and all of its properties to a file.
 * 2.Compare externalized file with expected output.
 *
 * @SYMTestExpectedResults For the above tests
 * 1.Encoding name 'b' is used for binary properties.
 * 2.No inline quoted-printable encoding for non binary properties.
 * 3.Comparison of expected vCard should pass.
 * 4.No leaves or panics should occur.
 */
void CTests::EncodingTestL()
	{	
	/** Encoding name as 'b' for binary properties.
		Base64 encoding parameter NAME=VALUE should be ENCODING=b */
		   
	//	   
	// Photo property with binary value
	//
	
	_LIT(KFile1, "BinaryEncoding");
	_LIT8(KVCardData1, "BEGIN:VCARD\r\nPHOTO:file://jqpublic.gif\r\nEND:VCARD\r\n");
	TFileName outputFile1;
    
    iUtil->CreateAndExternaliseVCardL(KVCardData1, KFile1, outputFile1);
    
	//compare files to make sure that externalized Photo property contains encoding name as 'b'
    iUtil->CheckVCardL(KFile1, outputFile1);	
    
		
	/** No Inline quoted-printable encoding for non binary properties. */
	
	//	
	// EMAIL property with non printable characters.
	//
	
	_LIT(KFile2, "QPEncodingTest1");
	_LIT8(KVCardData2, "BEGIN:VCARD\r\nEMAIL:a\\b\r\nEND:VCARD\r\n");
	TFileName outputFile2;
    
    iUtil->CreateAndExternaliseVCardL(KVCardData2, KFile2, outputFile2);
    
	//compare files to make sure that externalized EMAIL property contains no QP encoding.
    iUtil->CheckVCardL(KFile2, outputFile2);
    
		
	//		
	// FN property with non printable characters.
	//
				
	_LIT(KFile3, "QPEncodingTest2");
	_LIT8(KVCardData3, "BEGIN:VCARD\r\nFN:A N Ex\\ample\r\nEND:VCARD\r\n");
	TFileName outputFile3;
    
    iUtil->CreateAndExternaliseVCardL(KVCardData3, KFile3, outputFile3);
    
	//compare files to make sure that externalized FN property contains no QP encoding.
    iUtil->CheckVCardL(KFile3, outputFile3);
	}

/**
 * @SYMPREQ 1375
 * @SYMTestType UT
 * @SYMTestPriority Critical
 * @SYMTestCaseDesc Backslash escaping for comma, backslash and semicolon characters.
 * 
 * @SYMTestActions
 * 1.Externalize a vCard entity and all of its properties to a file.
 * 2.Compare externalized file with expected output.
 *
 * @SYMTestExpectedResults For the above tests
 * 1.Any comma, semicolon and backslash characters in property/parameter values are escaped.
 * 2.Comparison of expected vCard should pass.
 * 3.No leaves or panics should occur.
 */
void CTests::EscapingTestL()
	{	
	//
	// Description property/parameter value with comma characters
	//
	
	_LIT(KFile1, "EscapeComma");
	_LIT8(KVCardData1, "BEGIN:VCARD\r\nDESCRIPTION;pname=pv,a,lue:a,ei,ou\r\nEND:VCARD\r\n");
	TFileName outputFile1;
    
    iUtil->CreateAndExternaliseVCardL(KVCardData1, KFile1, outputFile1);
    
	//compare files to make sure that externalized file contains escaped text.
    iUtil->CheckVCardL(KFile1, outputFile1);
    
    
	//	
	// Description property/parameter value with semicolon characters
	//
	
	_LIT(KFile2, "EscapeSemicolon");
	_LIT8(KVCardData2, "BEGIN:VCARD\r\nDESCRIPTION;pname=pv\\;a\\;lue:a\\;ei\\;ou\r\nEND:VCARD\r\n");
	TFileName outputFile2;
    
    iUtil->CreateAndExternaliseVCardL(KVCardData2, KFile2, outputFile2);
    
	//compare files to make sure that externalized file contains escaped text.
    iUtil->CheckVCardL(KFile2, outputFile2);
    
		
	//	
	// Description property/parameter value with backslash characters
	//
	
	_LIT(KFile3, "EscapeBackslash");
	_LIT8(KVCardData3, "BEGIN:VCARD\r\nDESCRIPTION;pname=pv\\a\\lue:ae\\io\\u\r\nEND:VCARD\r\n");
	TFileName outputFile3;
    
    iUtil->CreateAndExternaliseVCardL(KVCardData3, KFile3, outputFile3);
    
	//compare files to make sure that externalized file contains escaped text.
    iUtil->CheckVCardL(KFile3, outputFile3);
	}

/**
 * @SYMPREQ 1375
 * @SYMTestType UT
 * @SYMTestPriority Critical
 * @SYMTestCaseDesc Replace CRLF with '\n' or '\N'.
 * 
 * @SYMTestActions
 * 1.Externalize a vCard entity and all of its properties to a file.
 * 2.Compare externalized file with expected output.
 *
 * @SYMTestExpectedResults For the above tests
 * 1.CRLF sequence present in property value is correctly replaced with '\n'.
 * 2.Comparison of expected vCard should pass.
 * 3.No leaves or panics should occur.
 */
void CTests::ReplaceCRLFTestL()
	{		
	_LIT(KFile, "ReplaceCRLF");
	_LIT(KText, "aa\r\nbb");
	
	TFileName outputFile;
	iUtil->CreateAndExternaliseVCardL(KText, KFile, outputFile);
	
	CVersitParser* vCardParser = iUtil->CreateParserLC();
	iUtil->InternaliseFileL(*vCardParser, outputFile);
	
	TUid valueTypeUid = { KVersitPropertyHBufCUid };
	CArrayPtr<CParserProperty>* array = vCardParser->PropertyL(_L8("NOTE"), valueTypeUid, EFalse);
	CleanupStack::PushL(array);
	iTest(array->Count() == 1);
	CParserPropertyValueHBufC* value = static_cast<CParserPropertyValueHBufC*>(array->At(0)->Value());
	
	iTest(value->Value().Length() == 6);
	iTest(value->Value() == _L("aa\\nbb"));
	
	CleanupStack::PopAndDestroy(array);	
	CleanupStack::PopAndDestroy(vCardParser);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if	(!cleanup)
		{
		return KErrNoMemory;
		}
	
	TRAPD(err, CTests::DoTestsL());
	if (err != KErrNone)
		{
		_LIT(KError, "TestExternalize left with error %d\r\n");
		RDebug::Print(KError, err);
		}
	
	delete cleanup;
	__UHEAP_MARKEND;
	return (KErrNone);
	}
