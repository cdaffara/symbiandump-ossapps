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
 
#include <e32test.h>
#include <apgcli.h>
#include <ecom/ecom.h>
#include <apmrec.h>

// Type definitions
#define UNUSED_VAR(a) a = a

CConsoleBase* console;
LOCAL_D RTest	test(_L("TEST_RECOGNIZER"));

_LIT(KVCalFilePath, "z:\\testfiles\\cTestVcal.vcs");
_LIT8(KDataTypevCalendarPlain,"text/X-vCalendar");
_LIT8(KvCalMagicString,"BEGIN:VCALENDAR");

_LIT(KVCardFilePath, "z:\\testfiles\\cTestVcard.vcf");
_LIT8(KDataTypevCardPlain,"text/X-vCard");
_LIT8(KvCardMagicString,"BEGIN:VCARD");

const TUid KUidVersitRecognizer={0x100047EB};

void doTestL()
	{
//	TInt ret;
	
	CApaDataRecognizerType* dataRecType = NULL;
	TUid tempDtorKey = KNullUid;
	dataRecType = static_cast <CApaDataRecognizerType*>(REComSession::CreateImplementationL(KUidVersitRecognizer, tempDtorKey));
	CleanupStack::PushL(dataRecType);
	
	_LIT(KSupportedTypes, "Test Supported Types");
	
	test.Next(KSupportedTypes);
	test(dataRecType->SupportedDataTypeL(0) == TDataType(KDataTypevCardPlain));
	test(dataRecType->SupportedDataTypeL(1) == TDataType(KDataTypevCalendarPlain));

	_LIT(KRecognisevCal, "Test Recognise vCalendar");
	
	test.Next(KRecognisevCal);
	TDataRecognitionResult result;
	result = dataRecType->RecognizeL(KVCalFilePath, KvCalMagicString);
	test(result.iDataType==TDataType(KDataTypevCalendarPlain));
	test(result.iConfidence==CApaDataRecognizerType::ECertain);

	_LIT(KRecognisevCard, "Test Recognise vCard");
	
	test.Next(KRecognisevCard);
	result = dataRecType->RecognizeL(KVCardFilePath, KvCardMagicString);
	test(result.iDataType==TDataType(KDataTypevCardPlain));
	test(result.iConfidence==CApaDataRecognizerType::ECertain);
	
    CleanupStack::PopAndDestroy(dataRecType);
    REComSession::DestroyedImplementation(tempDtorKey);
    REComSession::FinalClose();
	}

/**
@SYMTestCaseID PIM-TESTRECOGNIZER-0001
*/	
TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTestCaseID PIM-TESTRECOGNIZER-0001 TEST_RECOGNIZER"));
	test.Title();
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error,doTestL());
	UNUSED_VAR(error); //Used to supress build warnings
	
	ASSERT(error == KErrNone);
	delete cleanup;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return (KErrNone);
	}
