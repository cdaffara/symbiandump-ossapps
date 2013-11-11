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
// Name of test harness: T_MIUT02
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Test classes derived from CBaseEmailSettings - CImPop3Settings, 
// CImImap4Settings, CImSmtpSettings - most of the 
// functionality is tested - some new functions may not be tested!
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// None
// Intermediate files produced while running test harness:
// <DRIVE>:\msgtest\T_MIUT02\T_IMCM.log
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_MIUT02.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_MIUT02.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_MIUT02.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_MIUT02.exe on the other platform
// 
//

#include "emailtestutils.h"
#include "miuthdr.h"    // CImHeader
#include <miutlog.h>
#include <pop3set.h>
#include <imapset.h>
#include <smtpset.h>
#include <miutset.h>
#include <msvids.h>
#include <cemailaccounts.h>
#include <iapprefs.h>

// local variables etc //

RTest test(_L("MIUT02 - Test classes derived from CBaseEmailSettings"));
LOCAL_D CTrapCleanup* theCleanup;

LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D CImPop3Settings* ImPop3Settings;
LOCAL_D CImPop3Settings* ImPop3SettingsVerify;
LOCAL_D CImImap4Settings* ImImap4Settings;
LOCAL_D CImImap4Settings* ImImap4SettingsVerify;
LOCAL_D CImSmtpSettings* ImSmtpSettings;
LOCAL_D CImSmtpSettings* ImSmtpSettingsVerify;
LOCAL_D TMsvEntry* entry; 
LOCAL_D CEmailAccounts* account;
LOCAL_D TPopAccount popAccount;
LOCAL_D TImapAccount imapAccount;

#define KMiutSmtpBodyEncoding		EMsgOutboxNoAlgorithm
#define KMiutSmtpIAP				0x10000001
#define KMaxSettingStringLengthLong	KMaxSettingStringLength


_LIT (KMiutPop3ServerAddress, "10.0.0.6");
_LIT8(KMiutImap4FolderPath, "../");
_LIT (KMiutSmtpEmailAlias, "Joe Bloggs");
_LIT (KMiutSmtpEmailAddress, "joe@bloggs.com");

const TUint KMiutPop3Port = 110;
const TUid KMiutSmtpDefaultMsgCharSet={0x10003C80};

enum Apop {
		EApopTrue,
		EApopFalse,
		EApopOther
	};

_LIT(KPOPAccountName, "PopAccount");
_LIT(KIMAPAccountName, "ImapAccount");
_LIT8(KServerAddress, "ServerAddress");
_LIT8(KLoginName, "LoginName");
_LIT8(KPassword, "Password");
_LIT8(KAutoSendOnConnect, "AutoSendOnConnect");
_LIT8(KDisconnectedUserMode, "DisconnectedUserMode");
_LIT8(KDeleteEmailsWhenDisconnecting, "DeleteEmailsWhenDisconnecting");
_LIT8(KAcknowledgeReceipts, "AcknowledgeReceipts");
_LIT8(KMaxEmailSize, "MaxEmailSize");
_LIT8(KFolderPath, "FolderPath");
_LIT8(KPathSeparator, "PathSeperator");
_LIT8(KEmailAlias, "EmailAlias");
_LIT8(KEmailAddress, "EmailAddress");
_LIT8(KReplyToAddress, "ReplyToAddress");
_LIT8(KReceiptAddress, "ReceiptAddress");
_LIT8(KBodyEncoding, "BodyEncoding");
_LIT8(KAddVCardToEmail, "AddVCardToEmail");
_LIT8(KAddSignatureToEmail, "AddSignatureToEmail");
_LIT8(KRequestReceipts, "RequestReceipts");
_LIT8(KInboxSyncLimit, "InboxSynchonisationLimit");
_LIT8(KMailboxSyncLimit, "MailboxSynchonisationLimit");
_LIT8(KSyncRate, "SyncRate");
_LIT8(KImapIdle, "ImapIdle");
_LIT8(KImapIdleTimeout, "ImapIdleTimeout");
_LIT8(KBodyTextSizeLimit, "BodyTextSizeLimit");
_LIT8(KAttachmentSizeLimit, "AttachmentSizeLimit");

TBuf16<KMaxSettingStringLength> buffer1;
TBuf16<KMaxSettingStringLength> buffer2;
TBuf8<KMaxSettingStringLength> buf1;
TBuf8<KMaxSettingStringLength> buf2;
TText8 separator1;
TText8 separator2;


//

LOCAL_C void ResultOfTest(TBool aResult,TInt aTestNo)
	{
	if (aResult)
		{
		test.Printf(TRefByValue<const TDesC>_L("Passed test %d\n"),aTestNo);
		}
	else
		{
		test.Printf(TRefByValue<const TDesC>_L("Failed test %d\n"),aTestNo);
		}
	}

LOCAL_C TBool CompareBooleans(TBool aBoolean1, TBool aBoolean2, const TDesC8* aTestDescription)
	{
	_LIT(KErrorReason, "\t\tSettings not set correctly for %S");
	_LIT(KPassReason, "\t\tSettings set correctly for %S");
	TBuf<80> buf;
	TBuf<80> temp;
	temp.Copy(*aTestDescription);
	TBool result = ((aBoolean1 && aBoolean2) || (!aBoolean1 && !aBoolean2));
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &temp);
		testUtils->WriteComment(buf);
		}
	else
		{
		buf.AppendFormat(KPassReason, &temp);
		testUtils->WriteComment(buf);
		}
	return result;
	}

LOCAL_C TBool CompareDescriptors(TDesC16& aDesC1, TDesC16& aDesC2, const TDesC8* aTestDescription)
	{
	_LIT(KErrorReason, "\t\tSettings not set correctly for %S");
	_LIT(KPassReason, "\t\tSettings set correctly for %S");
	TBuf<80> buf;
	TBuf<80> temp;
	temp.Copy(*aTestDescription);
	TBool result = (aDesC1 == aDesC2);
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &temp);
		testUtils->WriteComment(buf);
		}
	else
		{
		buf.AppendFormat(KPassReason, &temp);
		testUtils->WriteComment(buf);
		}
	return result;
	}

LOCAL_C TBool CompareDescriptors(TDesC8& aDesC1, TDesC8& aDesC2, const TDesC8* aTestDescription)
	{
	_LIT(KErrorReason, "\t\tSettings not set correctly for %S");
	_LIT(KPassReason, "\t\tSettings set correctly for %S");
	TBuf<80> buf;
	TBuf<80> temp;
	temp.Copy(*aTestDescription);
	TBool result = (aDesC1 == aDesC2);
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &temp);
		testUtils->WriteComment(buf);
		}
	else
		{
		buf.AppendFormat(KPassReason, &temp);
		testUtils->WriteComment(buf);
		}

	return result;
	}

LOCAL_C TBool CompareIntegers(TInt aInt1, TInt aInt2, const TDesC8* aTestDescription)
	{
	_LIT(KErrorReason, "\t\tSettings not set correctly for %S");
	_LIT(KPassReason, "\t\tSettings set correctly for %S");
	TBuf<80> buf;
	TBuf<80> temp;
	temp.Copy(*aTestDescription);
	TBool result = (aInt1 == aInt2);
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &temp);
		testUtils->WriteComment(buf);
		}
	else
		{
		buf.AppendFormat(KPassReason, &temp);
		testUtils->WriteComment(buf);
		}

	return result;
	}

LOCAL_C TBool CompareTextChars(TText8 aChar1, TText8 aChar2, const TDesC8* aTestDescription)
	{
	_LIT(KErrorReason, "\t\tSettings not set correctly for %S");
	_LIT(KPassReason, "\t\tSettings set correctly for %S");
	TBuf<80> buf;
	TBuf<80> temp;
	temp.Copy(*aTestDescription);
	TBool result = (aChar1 == aChar2);
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &temp);
		testUtils->WriteComment(buf);
		}
	else
		{
		buf.AppendFormat(KPassReason, &temp);
		testUtils->WriteComment(buf);
		}

	return result;
	}


//
// CImPop3Settings Test Functions //
//

LOCAL_C void CreateFilledPOP3Settings(Apop apop)
	{
	ImPop3Settings->SetServerAddressL(KMiutPop3ServerAddress);
	ImPop3Settings->SetPort(KMiutPop3Port);	// port number can only be positive (TUint)
	ImPop3Settings->SetLoginNameL(testUtils->MachineName());
	ImPop3Settings->SetPasswordL(testUtils->MachineName());

	if (apop==EApopTrue)
		ImPop3Settings->SetApop(ETrue);
	else if (apop==EApopFalse)
		ImPop3Settings->SetApop(EFalse);
	else	// Set an incorrect APOP value
		ImPop3Settings->SetApop(0);

	ImPop3Settings->SetDisconnectedUserMode(ETrue);
	ImPop3Settings->SetAutoSendOnConnect(ETrue);
	ImPop3Settings->SetDeleteEmailsWhenDisconnecting(ETrue);
	ImPop3Settings->SetAcknowledgeReceipts(ETrue);
	ImPop3Settings->SetMaxEmailSize(123456);
	ImPop3Settings->SetGetMailOptions(EGetPop3EmailHeaders);
	ImPop3Settings->SetInboxSynchronisationLimit(123);			// New in V3
	_LIT(KComment, "\t\tCreated Filled POP3 Settings");
	testUtils->WriteComment(KComment);
	}

LOCAL_C void CreateLongPOP3Settings()
	{
	// a bunch of data for the header object....
	// all the fields are set to a length greater than 1000 i.e 1001

	TBuf8<KMaxSettingStringLengthLong> longValue;
	longValue.Fill('x',KMaxSettingStringLengthLong);
	TBuf<KMaxSettingStringLengthLong> longValue1;
	longValue1.Fill('x',KMaxSettingStringLengthLong);
	ImPop3Settings->SetServerAddressL(longValue1);
	ImPop3Settings->SetPort(KMiutPop3Port);	// port number can only be positive (TUint)
	ImPop3Settings->SetLoginNameL(testUtils->MachineName());
	ImPop3Settings->SetPasswordL(testUtils->MachineName());
	ImPop3Settings->SetApop(ETrue);
	ImPop3Settings->SetDisconnectedUserMode(ETrue);
	ImPop3Settings->SetAutoSendOnConnect(ETrue);
	ImPop3Settings->SetDeleteEmailsWhenDisconnecting(ETrue);
	ImPop3Settings->SetAcknowledgeReceipts(ETrue);
	ImPop3Settings->SetMaxEmailSize(123456);
	ImPop3Settings->SetGetMailOptions(EGetPop3EmailHeaders);
	ImPop3Settings->SetInboxSynchronisationLimit(123);			// New in V3
	_LIT(KComment, "\t\tCreated Long POP3 Settings");
	testUtils->WriteComment(KComment);
	}

LOCAL_C TBool CompareFilledPOP3Settings(TInt aTest)
	{
	_LIT(KComment, "\t\tComparing Filled POP3 Settings");
	testUtils->WriteComment(KComment);
	TBool result = ETrue;
	buffer1 = ImPop3Settings->ServerAddress();
	buffer2 = ImPop3SettingsVerify->ServerAddress();
	result &= CompareDescriptors(buffer1, buffer2, &KServerAddress); 


	buf1 = ImPop3Settings->LoginName();
	buf2 = ImPop3SettingsVerify->LoginName();
	result &= CompareDescriptors(buf1, buf2, &KLoginName); 

	buf1 = ImPop3Settings->Password();
	buf2 = ImPop3SettingsVerify->Password();
	result &= CompareDescriptors(buf1, buf2, &KPassword); 

	result &= CompareBooleans(ImPop3Settings->AutoSendOnConnect(), ImPop3SettingsVerify->AutoSendOnConnect(), &KAutoSendOnConnect);
	result &= CompareBooleans(ImPop3Settings->DisconnectedUserMode(), ImPop3SettingsVerify->DisconnectedUserMode(), &KDisconnectedUserMode);
	result &= CompareBooleans(ImPop3Settings->DeleteEmailsWhenDisconnecting(), ImPop3SettingsVerify->DeleteEmailsWhenDisconnecting(), &KDeleteEmailsWhenDisconnecting);
	result &= CompareBooleans(ImPop3Settings->AcknowledgeReceipts(), ImPop3SettingsVerify->AcknowledgeReceipts(), &KAcknowledgeReceipts);

	result &= CompareIntegers((ImPop3Settings->MaxEmailSize()), (ImPop3SettingsVerify->MaxEmailSize()), &KMaxEmailSize);
	
	result &= CompareIntegers((ImPop3Settings->InboxSynchronisationLimit()), (ImPop3SettingsVerify->InboxSynchronisationLimit()), &KInboxSyncLimit);

	TBool result1 = ETrue;;
	if (ImPop3Settings->Port()<=65535)
		result1 &= ((ImPop3Settings->Port())==(ImPop3SettingsVerify->Port()));
	else	// port numbers can only be in the range 0-65535
		result1 &= EFalse;
	if (!result1)
		{
		_LIT(KComment, "\t\tPort Numbers out of Range");
		testUtils->WriteComment(KComment);
		}
	result &= result1;

	TBool result2 = ETrue;
	if (ImPop3Settings->Apop()==1 || ImPop3Settings->Apop()==0)
		result2 &= ((ImPop3Settings->Apop())==(ImPop3SettingsVerify->Apop()));
	if (!result2)
		{
		_LIT(KComment, "\t\tApop values incorrectly set");
		testUtils->WriteComment(KComment);
		}
	result &= result2;

	TBool result3=((ImPop3Settings->GetMailOptions())==(ImPop3SettingsVerify->GetMailOptions()));
	if (!result3)
		{
		_LIT(KComment, "\t\tGetMailOptions not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result3;
	
	ResultOfTest(result,aTest);
	return result;
	}

LOCAL_C TBool CompareBlankPOP3Settings(TInt aTest)
	{
	// need to check each of the fields of the stored header (imHeaderVerify) are 
	// the same as that originally entered (imHeader).  Here the length should be 
	// 0 so we don't actually need to compare!

	TBool result = ETrue;
	_LIT(KComment, "\t\tComparing Blank POP3 Settings");
	testUtils->WriteComment(KComment);

	result &= CompareBooleans((ImPop3Settings->ServerAddress().Length()==0), (ImPop3SettingsVerify->ServerAddress().Length()==0), &KServerAddress);
	result &= CompareBooleans((ImPop3Settings->LoginName().Length()==0), (ImPop3SettingsVerify->LoginName().Length()==0), &KLoginName);
	result &= CompareBooleans((ImPop3Settings->Password().Length()==0), (ImPop3SettingsVerify->Password().Length()==0), &KPassword);
	
	result &= CompareBooleans(ImPop3Settings->AutoSendOnConnect(), EFalse, &KAutoSendOnConnect);
	result &= CompareBooleans(ImPop3Settings->DisconnectedUserMode(), EFalse, &KDisconnectedUserMode);
	result &= CompareBooleans(ImPop3Settings->DeleteEmailsWhenDisconnecting(), EFalse, &KDeleteEmailsWhenDisconnecting);
	result &= CompareBooleans(ImPop3Settings->AcknowledgeReceipts(), EFalse, &KAcknowledgeReceipts);

	result &= CompareIntegers((ImPop3Settings->InboxSynchronisationLimit()), (ImPop3SettingsVerify->InboxSynchronisationLimit()), &KInboxSyncLimit);

	ResultOfTest(result,aTest);
	return result;
	}

LOCAL_C TBool CompareLongPOP3Settings(TInt aTest)
	{
	TBool result = ETrue;
	buffer1 = ImPop3Settings->ServerAddress().Left(KMaxSettingStringLength);
	buffer2 = ImPop3SettingsVerify->ServerAddress().Left(KMaxSettingStringLength);
	result &= CompareDescriptors(buffer1, buffer2, &KServerAddress); 


	buf1 = ImPop3Settings->LoginName().Left(KMaxSettingStringLength);
	buf2 = ImPop3SettingsVerify->LoginName().Left(KMaxSettingStringLength);
	result &= CompareDescriptors(buf1, buf2, &KLoginName); 

	buf1 = ImPop3Settings->Password().Left(KMaxSettingStringLength);
	buf2 = ImPop3SettingsVerify->Password().Left(KMaxSettingStringLength);
	result &= CompareDescriptors(buf1, buf2, &KPassword); 

	result &= CompareBooleans(ImPop3Settings->AutoSendOnConnect(), ImPop3SettingsVerify->AutoSendOnConnect(), &KAutoSendOnConnect);
	result &= CompareBooleans(ImPop3Settings->DisconnectedUserMode(), ImPop3SettingsVerify->DisconnectedUserMode(), &KDisconnectedUserMode);
	result &= CompareBooleans(ImPop3Settings->DeleteEmailsWhenDisconnecting(), ImPop3SettingsVerify->DeleteEmailsWhenDisconnecting(), &KDeleteEmailsWhenDisconnecting);
	result &= CompareBooleans(ImPop3Settings->AcknowledgeReceipts(), ImPop3SettingsVerify->AcknowledgeReceipts(), &KAcknowledgeReceipts);

	result &= CompareIntegers((ImPop3Settings->MaxEmailSize()), (ImPop3SettingsVerify->MaxEmailSize()), &KMaxEmailSize);
	
	result &= CompareIntegers((ImPop3Settings->InboxSynchronisationLimit()), (ImPop3SettingsVerify->InboxSynchronisationLimit()), &KInboxSyncLimit);

	TBool result1 = ETrue;;
	if (ImPop3Settings->Port()<=65535)
		result1 &= ((ImPop3Settings->Port())==(ImPop3SettingsVerify->Port()));
	else	// port numbers can only be in the range 0-65535
		result1 &= EFalse;
	if (!result1)
		{
		_LIT(KComment, "\t\tPort Numbers out of Range");
		testUtils->WriteComment(KComment);
		}
	result &= result1;

	TBool result2 = ETrue;
	if (ImPop3Settings->Apop()==1 || ImPop3Settings->Apop()==0)
		result2 &= ((ImPop3Settings->Apop())==(ImPop3SettingsVerify->Apop()));
	if (!result2)
		{
		_LIT(KComment, "\t\tApop values incorrectly set");
		testUtils->WriteComment(KComment);
		}
	result &= result2;

	TBool result3=((ImPop3Settings->GetMailOptions())==(ImPop3SettingsVerify->GetMailOptions()));
	if (!result3)
		{
		_LIT(KComment, "\t\tGetMailOptions not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result3;
	
	ResultOfTest(result,aTest);
	return result;
	}

LOCAL_C TBool ComparePOP3Copy()
	{
	_LIT(KComment, "\t\tComparing POP3 Copy with Original");
	testUtils->WriteComment(KComment);
	TBool result=ETrue;
	buffer1 = ImPop3Settings->ServerAddress();
	buffer2 = ImPop3SettingsVerify->ServerAddress();
	result &= CompareDescriptors(buffer1, buffer2, &KServerAddress); 


	buf1 = ImPop3Settings->LoginName();
	buf2 = ImPop3SettingsVerify->LoginName();
	result &= CompareDescriptors(buf1, buf2, &KLoginName); 

	buf1 = ImPop3Settings->Password();
	buf2 = ImPop3SettingsVerify->Password();
	result &= CompareDescriptors(buf1, buf2, &KPassword); 

	result &= CompareBooleans(ImPop3Settings->AutoSendOnConnect(), ImPop3SettingsVerify->AutoSendOnConnect(), &KAutoSendOnConnect);
	result &= CompareBooleans(ImPop3Settings->DisconnectedUserMode(), ImPop3SettingsVerify->DisconnectedUserMode(), &KDisconnectedUserMode);
	result &= CompareBooleans(ImPop3Settings->DeleteEmailsWhenDisconnecting(), ImPop3SettingsVerify->DeleteEmailsWhenDisconnecting(), &KDeleteEmailsWhenDisconnecting);
	result &= CompareBooleans(ImPop3Settings->AcknowledgeReceipts(), ImPop3SettingsVerify->AcknowledgeReceipts(), &KAcknowledgeReceipts);

	result &= CompareIntegers((ImPop3Settings->MaxEmailSize()), (ImPop3SettingsVerify->MaxEmailSize()), &KMaxEmailSize);

	result &= CompareIntegers((ImPop3Settings->InboxSynchronisationLimit()), (ImPop3SettingsVerify->InboxSynchronisationLimit()), &KInboxSyncLimit);

	TBool result1 = ETrue;;
	if (ImPop3Settings->Port()<=65535)
		result1 &= ((ImPop3Settings->Port())==(ImPop3SettingsVerify->Port()));
	else	// port numbers can only be in the range 0-65535
		result1 &= EFalse;
	if (!result1)
		{
		_LIT(KComment, "\t\tPort Numbers out of Range");
		testUtils->WriteComment(KComment);
		}
	result &= result1;

	TBool result2 = ETrue;
	if (ImPop3Settings->Apop()==1 || ImPop3Settings->Apop()==0)
		result2 &= ((ImPop3Settings->Apop())==(ImPop3SettingsVerify->Apop()));
	if (!result2)
		{
		_LIT(KComment, "\t\tApop values incorrectly set");
		testUtils->WriteComment(KComment);
		}
	result &= result2;

	TBool result3=((ImPop3Settings->GetMailOptions())==(ImPop3SettingsVerify->GetMailOptions()));
	if (!result3)
		{
		_LIT(KComment, "\t\tGetMailOptions not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result3;

	return result;
	}

//

	
LOCAL_C TBool TestPOP3EqualityAndCopyConstructorsL(TInt aTest)
	{
	_LIT(KComment, "\tPOP3 Equality and Copy Constructors Test");
	testUtils->WriteComment(KComment);
	entry = new(ELeave) TMsvEntry;
	entry->iDescription.Set(_L("Description"));
	entry->iDetails.Set(_L("Details"));
	entry->iType = KUidMsvServiceEntry;
	ImPop3Settings=new(ELeave)CImPop3Settings;
	ImPop3SettingsVerify=new(ELeave)CImPop3Settings;
	CreateFilledPOP3Settings(EApopTrue);
//	ImPop3SettingsVerify->SetVersion(2);
	ImPop3SettingsVerify->CopyL(*ImPop3Settings);
	TBool result=ComparePOP3Copy();
	TBool result1=(*ImPop3SettingsVerify==*ImPop3Settings);
	if (!result1)
		{
		_LIT(KComment, "\t\tError in Equality Operator");
		testUtils->WriteComment(KComment);
		}
	else
		{
		_LIT(KComment, "\t\tEquality Operator test passed");
		testUtils->WriteComment(KComment);
		}

	result&=result1;
	delete entry;
	delete ImPop3Settings;
	delete ImPop3SettingsVerify;

	ResultOfTest(result,aTest);
	return result;
	}

//
// CImImap4Settings Test Functions //
//

LOCAL_C void CreateFilledIMAP4Settings()
	{
	ImImap4Settings->SetLoginNameL(testUtils->MachineName());
	ImImap4Settings->SetPasswordL(testUtils->MachineName());
	ImImap4Settings->SetFolderPathL(KMiutImap4FolderPath);
	ImImap4Settings->SetAutoSendOnConnect(ETrue);
	ImImap4Settings->SetPathSeparator('\0');
	ImImap4Settings->SetDisconnectedUserMode(ETrue);
	ImImap4Settings->SetSynchronise(EUseCombination);
	ImImap4Settings->SetSubscribe(EUpdateBoth);
	ImImap4Settings->SetAutoSendOnConnect(ETrue);
	ImImap4Settings->SetDeleteEmailsWhenDisconnecting(ETrue);
	ImImap4Settings->SetAcknowledgeReceipts(ETrue);
	ImImap4Settings->SetMaxEmailSize(123456);
	ImImap4Settings->SetGetMailOptions(EGetImap4EmailHeaders);
	ImImap4Settings->SetInboxSynchronisationLimit(123);			// New in V3
	ImImap4Settings->SetMailboxSynchronisationLimit(321);		// New in V3
	ImImap4Settings->SetSyncRateL(60);							// New in V4
	ImImap4Settings->SetImapIdle(ETrue);						
	ImImap4Settings->SetImapIdleTimeoutL(29);					// New in V5
	ImImap4Settings->SetPartialMailOptionsL(ENoSizeLimits);		// New in V7
	ImImap4Settings->SetBodyTextSizeLimitL(123456);				// New in V7	
	ImImap4Settings->SetAttachmentSizeLimitL(123456);			// New in V7
	_LIT(KComment, "\tCreated Filled IMAP4 Settings");
	testUtils->WriteComment(KComment);
	}

//Testing for -ve value of sync rate
LOCAL_C TInt CreateIMAP4SettingsNegativeTest(TInt aTest)
	{
	TBool result;	
	_LIT(KComment, "CreateIMAP4SettingsNegativeTest");
	testUtils->WriteComment(KComment);

	entry = new(ELeave) TMsvEntry;
	entry->iDescription.Set(_L("Description"));
	entry->iDetails.Set(_L("Details"));
	entry->iType = KUidMsvServiceEntry;

	ImImap4Settings=new(ELeave)CImImap4Settings;
	ImImap4Settings->SetLoginNameL(testUtils->MachineName());
	ImImap4Settings->SetPasswordL(testUtils->MachineName());
	ImImap4Settings->SetFolderPathL(KMiutImap4FolderPath);
	ImImap4Settings->SetAutoSendOnConnect(ETrue);
	ImImap4Settings->SetPathSeparator('\0');
	ImImap4Settings->SetDisconnectedUserMode(ETrue);
	ImImap4Settings->SetSynchronise(EUseCombination);
	ImImap4Settings->SetSubscribe(EUpdateBoth);
	ImImap4Settings->SetAutoSendOnConnect(ETrue);
	ImImap4Settings->SetDeleteEmailsWhenDisconnecting(ETrue);
	ImImap4Settings->SetAcknowledgeReceipts(ETrue);
	ImImap4Settings->SetMaxEmailSize(123456);
	ImImap4Settings->SetGetMailOptions(EGetImap4EmailHeaders);
	ImImap4Settings->SetInboxSynchronisationLimit(123);
	ImImap4Settings->SetMailboxSynchronisationLimit(321);
	ImImap4Settings->SetImapIdle(ETrue);
	ImImap4Settings->SetImapIdleTimeoutL(29);
	ImImap4Settings->SetPartialMailOptionsL(ENoSizeLimits);
	ImImap4Settings->SetBodyTextSizeLimitL(123456);
	ImImap4Settings->SetAttachmentSizeLimitL(123456);

	//When sync rate is negative, it should leave with KErrArgument
	TRAPD(err, ImImap4Settings->SetSyncRateL(-360));

	if(err == KErrArgument)
		{
		result=ETrue;
		_LIT(KComment1, "\t CreateIMAP4SettingsNegativeTest Completed");
		testUtils->WriteComment(KComment1);
		}
	else
		{
		result=EFalse;
		}
		ResultOfTest(result,aTest);

	delete entry;
	delete ImImap4Settings;
	return result;
	}

LOCAL_C void CreateLongIMAP4Settings()
	{
	// a bunch of data for the header object....
	// all the fields are set to a length greater than 1000 i.e 1001

	TBuf8<KMaxSettingStringLengthLong> longValue;
	longValue.Fill('x',KMaxSettingStringLengthLong);

	ImImap4Settings->SetLoginNameL(testUtils->MachineName());
	ImImap4Settings->SetPasswordL(testUtils->MachineName());
	ImImap4Settings->SetFolderPathL(longValue);
	ImImap4Settings->SetAutoSendOnConnect(ETrue);
	ImImap4Settings->SetPathSeparator('\0');
	ImImap4Settings->SetDisconnectedUserMode(ETrue);
	ImImap4Settings->SetSynchronise(EUseCombination);
	ImImap4Settings->SetSubscribe(EUpdateBoth);
	ImImap4Settings->SetAutoSendOnConnect(ETrue);
	ImImap4Settings->SetDeleteEmailsWhenDisconnecting(ETrue);
	ImImap4Settings->SetAcknowledgeReceipts(ETrue);
	ImImap4Settings->SetMaxEmailSize(123456);
	ImImap4Settings->SetGetMailOptions(EGetImap4EmailHeaders);
	ImImap4Settings->SetInboxSynchronisationLimit(123);			// New in V3
	ImImap4Settings->SetMailboxSynchronisationLimit(321);		// New in V3
	ImImap4Settings->SetSyncRateL(60);							// New in V4
	ImImap4Settings->SetImapIdle(ETrue);						
	ImImap4Settings->SetImapIdleTimeoutL(29);					// New in V5
	ImImap4Settings->SetPartialMailOptionsL(ENoSizeLimits);		// New in V7
	ImImap4Settings->SetBodyTextSizeLimitL(123456);				// New in V7	
	ImImap4Settings->SetAttachmentSizeLimitL(123456);			// New in V7
	_LIT(KComment, "\tCreated Long IMAP4 Settings");
	testUtils->WriteComment(KComment);
	}

LOCAL_C TBool CompareFilledIMAP4Settings(TInt aTest)
	{
	TBool result = ETrue;
	_LIT(KComment, "\tComparing Filled IMAP4 Settings");
	testUtils->WriteComment(KComment);

	buf1 = ImImap4Settings->LoginName();
	buf2 = ImImap4SettingsVerify->LoginName();
	result &= CompareDescriptors(buf1, buf2, &KLoginName); 

	buf1 = ImImap4Settings->Password();
	buf2 = ImImap4SettingsVerify->Password();
	result &= CompareDescriptors(buf1, buf2, &KPassword); 

	buf1 = ImImap4Settings->FolderPath();
	buf2 = ImImap4SettingsVerify->FolderPath();
	result &= CompareDescriptors(buf1, buf2, &KFolderPath);

	separator1 = ImImap4Settings->PathSeparator();
	separator2 = ImImap4SettingsVerify->PathSeparator();
	result &= CompareTextChars(separator1, separator2, &KPathSeparator);

	result &= CompareBooleans(ImImap4Settings->AutoSendOnConnect(), ImImap4SettingsVerify->AutoSendOnConnect(), &KAutoSendOnConnect);
	result &= CompareBooleans(ImImap4Settings->DisconnectedUserMode(), ImImap4SettingsVerify->DisconnectedUserMode(), &KDisconnectedUserMode);
	result &= CompareBooleans(ImImap4Settings->DeleteEmailsWhenDisconnecting(), ImImap4SettingsVerify->DeleteEmailsWhenDisconnecting(), &KDeleteEmailsWhenDisconnecting);
	result &= CompareBooleans(ImImap4Settings->AcknowledgeReceipts(), ImImap4SettingsVerify->AcknowledgeReceipts(), &KAcknowledgeReceipts);
	result &= CompareBooleans(ImImap4Settings->ImapIdle(), ImImap4SettingsVerify->ImapIdle(), &KImapIdle);

	result &= CompareIntegers((ImImap4Settings->MaxEmailSize()), (ImImap4SettingsVerify->MaxEmailSize()), &KMaxEmailSize);
	
	result &= CompareIntegers((ImImap4Settings->InboxSynchronisationLimit()), (ImImap4SettingsVerify->InboxSynchronisationLimit()), &KInboxSyncLimit);
	result &= CompareIntegers((ImImap4Settings->MailboxSynchronisationLimit()), (ImImap4SettingsVerify->MailboxSynchronisationLimit()), &KMailboxSyncLimit);

	result &= CompareIntegers((ImImap4Settings->SyncRate()), (ImImap4SettingsVerify->SyncRate()), &KSyncRate);
	result &= CompareIntegers((ImImap4Settings->ImapIdleTimeout()), (ImImap4SettingsVerify->ImapIdleTimeout()), &KImapIdleTimeout);
	result &= CompareIntegers((ImImap4Settings->BodyTextSizeLimit()), (ImImap4SettingsVerify->BodyTextSizeLimit()), &KBodyTextSizeLimit);
	result &= CompareIntegers((ImImap4Settings->AttachmentSizeLimit()), (ImImap4SettingsVerify->AttachmentSizeLimit()), &KAttachmentSizeLimit);

	TBool result1 = ETrue;;
	if (ImImap4Settings->Port()<=65535)
		result1 &= ((ImImap4Settings->Port())==(ImImap4SettingsVerify->Port()));
	else	// port numbers can only be in the range 0-65535
		result1 &= EFalse;
	if (!result1)
		{
		_LIT(KComment, "\t\tPort Numbers out of Range");
		testUtils->WriteComment(KComment);
		}
	result &= result1;

	TBool result2=((ImImap4Settings->GetMailOptions())==(ImImap4SettingsVerify->GetMailOptions()));
	if (!result2)
		{
		_LIT(KComment, "\t\tGetMailOptions settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result2;

	TBool result3=((ImImap4Settings->Synchronise())==(ImImap4SettingsVerify->Synchronise()));
	if (!result3)
		{
		_LIT(KComment, "\t\tSynchronise settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result3;

	TBool result4=((ImImap4Settings->Subscribe())==(ImImap4SettingsVerify->Subscribe()));
	if (!result4)
		{
		_LIT(KComment, "\t\tSubscribe settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result4;

	TBool result5=((ImImap4Settings->PartialMailOptions())==(ImImap4SettingsVerify->PartialMailOptions()));
	if (!result5)
		{
		_LIT(KComment, "\t\tPartialMailOptions settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result5;

	ResultOfTest(result,aTest);
	return result;
	}

LOCAL_C TBool CompareBlankIMAP4Settings(TInt aTest)
	{
	// need to check each of the fields of the stored header (ImImap4SettingsVerify) are 
	// the same as that originally entered (ImImap4Settings).  Here the length should be 
	// 0 so we don't actually need to compare!

	TBool result = ETrue;

	result &= CompareBooleans((ImImap4Settings->LoginName().Length()==0), (ImImap4SettingsVerify->LoginName().Length()==0), &KLoginName);
	result &= CompareBooleans((ImImap4Settings->Password().Length()==0), (ImImap4SettingsVerify->Password().Length()==0), &KPassword);
	result &= CompareBooleans((ImImap4Settings->FolderPath().Length()==0), (ImImap4SettingsVerify->FolderPath().Length()==0), &KFolderPath);

	result &= CompareBooleans(ImImap4Settings->AutoSendOnConnect(), EFalse, &KAutoSendOnConnect);
	result &= CompareBooleans(ImImap4Settings->DisconnectedUserMode(), EFalse, &KDisconnectedUserMode);
	result &= CompareBooleans(ImImap4Settings->DeleteEmailsWhenDisconnecting(), EFalse, &KDeleteEmailsWhenDisconnecting);
	result &= CompareBooleans(ImImap4Settings->AcknowledgeReceipts(), EFalse, &KAcknowledgeReceipts);
	result &= CompareBooleans(ImImap4Settings->ImapIdle(), ETrue, &KImapIdle); // default is set to ETrue

	result &= CompareIntegers((ImImap4Settings->InboxSynchronisationLimit()), (ImImap4SettingsVerify->InboxSynchronisationLimit()), &KInboxSyncLimit);
	result &= CompareIntegers((ImImap4Settings->MailboxSynchronisationLimit()), (ImImap4SettingsVerify->MailboxSynchronisationLimit()), &KMailboxSyncLimit);

	result &= CompareIntegers((ImImap4Settings->SyncRate()), (ImImap4SettingsVerify->SyncRate()), &KSyncRate);
	result &= CompareIntegers((ImImap4Settings->ImapIdleTimeout()), (ImImap4SettingsVerify->ImapIdleTimeout()), &KImapIdleTimeout);
	result &= CompareIntegers((ImImap4Settings->BodyTextSizeLimit()), (ImImap4SettingsVerify->BodyTextSizeLimit()), &KBodyTextSizeLimit);
	result &= CompareIntegers((ImImap4Settings->AttachmentSizeLimit()), (ImImap4SettingsVerify->AttachmentSizeLimit()), &KAttachmentSizeLimit);

	TBool result1=((ImImap4Settings->PartialMailOptions())==(ImImap4SettingsVerify->PartialMailOptions()));
	if (!result1)
		{
		_LIT(KComment, "\t\tPartialMailOptions settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result1;

	ResultOfTest(result,aTest);
	return result;
	}

LOCAL_C TBool CompareLongIMAP4Settings(TInt aTest)
	{
	TBool result = ETrue;
	_LIT(KComment, "\tComparing Long IMAP4 Settings");
	testUtils->WriteComment(KComment);

	buf1 = ImImap4Settings->LoginName().Left(KMaxSettingStringLength);
	buf2 = ImImap4SettingsVerify->LoginName().Left(KMaxSettingStringLength);
	result &= CompareDescriptors(buf1, buf2, &KLoginName); 

	buf1 = ImImap4Settings->Password().Left(KMaxSettingStringLength);
	buf2 = ImImap4SettingsVerify->Password().Left(KMaxSettingStringLength);
	result &= CompareDescriptors(buf1, buf2, &KPassword); 

	buf1 = ImImap4Settings->FolderPath().Left(KMaxSettingStringLength);
	buf2 = ImImap4SettingsVerify->FolderPath().Left(KMaxSettingStringLength);
	result &= CompareDescriptors(buf1, buf2, &KFolderPath);

	separator1 = ImImap4Settings->PathSeparator();
	separator2 = ImImap4SettingsVerify->PathSeparator();
	result &= CompareTextChars(separator1, separator2, &KPathSeparator);

	result &= CompareBooleans(ImImap4Settings->AutoSendOnConnect(), ImImap4SettingsVerify->AutoSendOnConnect(), &KAutoSendOnConnect);
	result &= CompareBooleans(ImImap4Settings->DisconnectedUserMode(), ImImap4SettingsVerify->DisconnectedUserMode(), &KDisconnectedUserMode);
	result &= CompareBooleans(ImImap4Settings->DeleteEmailsWhenDisconnecting(), ImImap4SettingsVerify->DeleteEmailsWhenDisconnecting(), &KDeleteEmailsWhenDisconnecting);
	result &= CompareBooleans(ImImap4Settings->AcknowledgeReceipts(), ImImap4SettingsVerify->AcknowledgeReceipts(), &KAcknowledgeReceipts);
	result &= CompareBooleans(ImImap4Settings->ImapIdle(), ImImap4SettingsVerify->ImapIdle(), &KImapIdle);

	result &= CompareIntegers((ImImap4Settings->MaxEmailSize()), (ImImap4SettingsVerify->MaxEmailSize()), &KMaxEmailSize);
	
	result &= CompareIntegers((ImImap4Settings->InboxSynchronisationLimit()), (ImImap4SettingsVerify->InboxSynchronisationLimit()), &KInboxSyncLimit);
	result &= CompareIntegers((ImImap4Settings->MailboxSynchronisationLimit()), (ImImap4SettingsVerify->MailboxSynchronisationLimit()), &KMailboxSyncLimit);

	result &= CompareIntegers((ImImap4Settings->SyncRate()), (ImImap4SettingsVerify->SyncRate()), &KSyncRate);
	result &= CompareIntegers((ImImap4Settings->ImapIdleTimeout()), (ImImap4SettingsVerify->ImapIdleTimeout()), &KImapIdleTimeout);
	result &= CompareIntegers((ImImap4Settings->BodyTextSizeLimit()), (ImImap4SettingsVerify->BodyTextSizeLimit()), &KBodyTextSizeLimit);
	result &= CompareIntegers((ImImap4Settings->AttachmentSizeLimit()), (ImImap4SettingsVerify->AttachmentSizeLimit()), &KAttachmentSizeLimit);

	TBool result1 = ETrue;;
	if (ImImap4Settings->Port()<=65535)
		result1 &= ((ImImap4Settings->Port())==(ImImap4SettingsVerify->Port()));
	else	// port numbers can only be in the range 0-65535
		result1 &= EFalse;
	if (!result1)
		{
		_LIT(KComment, "\t\tPort Numbers out of Range");
		testUtils->WriteComment(KComment);
		}
	result &= result1;

	TBool result2=((ImImap4Settings->GetMailOptions())==(ImImap4SettingsVerify->GetMailOptions()));
	if (!result2)
		{
		_LIT(KComment, "\t\tGetMailOptions settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result2;

	TBool result3=((ImImap4Settings->Synchronise())==(ImImap4SettingsVerify->Synchronise()));
	if (!result3)
		{
		_LIT(KComment, "\t\tSynchronise settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result3;

	TBool result4=((ImImap4Settings->Subscribe())==(ImImap4SettingsVerify->Subscribe()));
	if (!result4)
		{
		_LIT(KComment, "\t\tSubscribe settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result4;

	TBool result5=((ImImap4Settings->PartialMailOptions())==(ImImap4SettingsVerify->PartialMailOptions()));
	if (!result5)
		{
		_LIT(KComment, "\t\tPartialMailOptions settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result5;

	ResultOfTest(result,aTest);
	return result;
	}

LOCAL_C TBool CompareIMAP4Copy()
	{
	TBool result = ETrue;
	_LIT(KComment, "\tComparing Filled IMAP4 Settings");
	testUtils->WriteComment(KComment);
	buf1 = ImImap4Settings->LoginName();
	buf2 = ImImap4SettingsVerify->LoginName();
	result &= CompareDescriptors(buf1, buf2, &KLoginName); 

	buf1 = ImImap4Settings->Password();
	buf2 = ImImap4SettingsVerify->Password();
	result &= CompareDescriptors(buf1, buf2, &KPassword); 

	buf1 = ImImap4Settings->FolderPath();
	buf2 = ImImap4SettingsVerify->FolderPath();
	result &= CompareDescriptors(buf1, buf2, &KFolderPath);

	separator1 = ImImap4Settings->PathSeparator();
	separator2 = ImImap4SettingsVerify->PathSeparator();
	result &= CompareTextChars(separator1, separator2, &KPathSeparator);

	result &= CompareBooleans(ImImap4Settings->AutoSendOnConnect(), ImImap4SettingsVerify->AutoSendOnConnect(), &KAutoSendOnConnect);
	result &= CompareBooleans(ImImap4Settings->DisconnectedUserMode(), ImImap4SettingsVerify->DisconnectedUserMode(), &KDisconnectedUserMode);
	result &= CompareBooleans(ImImap4Settings->DeleteEmailsWhenDisconnecting(), ImImap4SettingsVerify->DeleteEmailsWhenDisconnecting(), &KDeleteEmailsWhenDisconnecting);
	result &= CompareBooleans(ImImap4Settings->AcknowledgeReceipts(), ImImap4SettingsVerify->AcknowledgeReceipts(), &KAcknowledgeReceipts);
	result &= CompareBooleans(ImImap4Settings->ImapIdle(), ImImap4SettingsVerify->ImapIdle(), &KImapIdle);

	result &= CompareIntegers((ImImap4Settings->MaxEmailSize()), (ImImap4SettingsVerify->MaxEmailSize()), &KMaxEmailSize);
	
	result &= CompareIntegers((ImImap4Settings->InboxSynchronisationLimit()), (ImImap4SettingsVerify->InboxSynchronisationLimit()), &KInboxSyncLimit);
	result &= CompareIntegers((ImImap4Settings->MailboxSynchronisationLimit()), (ImImap4SettingsVerify->MailboxSynchronisationLimit()), &KMailboxSyncLimit);

	result &= CompareIntegers((ImImap4Settings->SyncRate()), (ImImap4SettingsVerify->SyncRate()), &KSyncRate);
	result &= CompareIntegers((ImImap4Settings->ImapIdleTimeout()), (ImImap4SettingsVerify->ImapIdleTimeout()), &KImapIdleTimeout);
	result &= CompareIntegers((ImImap4Settings->BodyTextSizeLimit()), (ImImap4SettingsVerify->BodyTextSizeLimit()), &KBodyTextSizeLimit);
	result &= CompareIntegers((ImImap4Settings->AttachmentSizeLimit()), (ImImap4SettingsVerify->AttachmentSizeLimit()), &KAttachmentSizeLimit);

	TBool result1 = ETrue;;
	if (ImImap4Settings->Port()<=65535)
		result1 &= ((ImImap4Settings->Port())==(ImImap4SettingsVerify->Port()));
	else	// port numbers can only be in the range 0-65535
		result1 &= EFalse;
	if (!result1)
		{
		_LIT(KComment, "\t\tPort Numbers out of Range");
		testUtils->WriteComment(KComment);
		}
	result &= result1;

	TBool result2=((ImImap4Settings->GetMailOptions())==(ImImap4SettingsVerify->GetMailOptions()));
	if (!result2)
		{
		_LIT(KComment, "\t\tGetMailOptions settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result2;

	TBool result3=((ImImap4Settings->Synchronise())==(ImImap4SettingsVerify->Synchronise()));
	if (!result3)
		{
		_LIT(KComment, "\t\tSynchronise settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result3;

	TBool result4=((ImImap4Settings->Subscribe())==(ImImap4SettingsVerify->Subscribe()));
	if (!result4)
		{
		_LIT(KComment, "\t\tSubscribe settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result4;

	TBool result5=((ImImap4Settings->PartialMailOptions())==(ImImap4SettingsVerify->PartialMailOptions()));
	if (!result5)
		{
		_LIT(KComment, "\t\tPartialMailOptions settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result5;

	return result;
	}

//


LOCAL_C TBool TestIMAP4EqualityAndCopyConstructorsL(TInt aTest)
	{
	_LIT(KComment, "\tIMAP4 Equality and Copy Constructors Test");
	testUtils->WriteComment(KComment);
	entry = new(ELeave) TMsvEntry;
	entry->iDescription.Set(_L("Description"));
	entry->iDetails.Set(_L("Details"));
	entry->iType = KUidMsvServiceEntry;
	ImImap4Settings=new(ELeave)CImImap4Settings;
	ImImap4SettingsVerify=new(ELeave)CImImap4Settings;
	CreateFilledIMAP4Settings();
	ImImap4SettingsVerify->CopyL(*ImImap4Settings);
	TBool result=CompareIMAP4Copy();
	TBool result1=(*ImImap4SettingsVerify==*ImImap4Settings);
	if (!result1)
		{
		_LIT(KComment, "\t\tError in Equality Operator");
		testUtils->WriteComment(KComment);
		}
	else
		{
		_LIT(KComment, "\t\tEquality Operator test passed");
		testUtils->WriteComment(KComment);
		}
	result&=result1;

	delete entry;
	delete ImImap4Settings;
	delete ImImap4SettingsVerify;

	ResultOfTest(result,aTest);
	return result;
	}


//
// CImSmtpSettings Test Functions //
//

LOCAL_C void CreateFilledSMTPSettings()
	{
	ImSmtpSettings->SetEmailAliasL(KMiutSmtpEmailAlias);
	ImSmtpSettings->SetEmailAddressL(KMiutSmtpEmailAddress);
	ImSmtpSettings->SetBodyEncoding(KMiutSmtpBodyEncoding);
	ImSmtpSettings->SetReplyToAddressL(KMiutSmtpEmailAddress);
	ImSmtpSettings->SetReceiptAddressL(KMiutSmtpEmailAddress);
	ImSmtpSettings->SetBodyEncoding(EMsgOutboxMIME);
	ImSmtpSettings->SetDefaultMsgCharSet(KMiutSmtpDefaultMsgCharSet);
	ImSmtpSettings->SetAddVCardToEmail(ETrue);
	ImSmtpSettings->SetAddSignatureToEmail(ETrue);
	ImSmtpSettings->SetRequestReceipts(ETrue);
	ImSmtpSettings->SetSendCopyToSelf(ESendNoCopy);
	ImSmtpSettings->SetSendMessageOption(ESendMessageImmediately);
	ImSmtpSettings->SetLoginNameL(testUtils->MachineName());
	ImSmtpSettings->SetPasswordL(testUtils->MachineName());

	_LIT(KComment, "\tCreated Filled SMTP Settings");
	testUtils->WriteComment(KComment);
	}

LOCAL_C void CreateLongSMTPSettings()
	{
	// a bunch of data for the header object....
	// all the fields are set to a length greater than 1000 i.e 1001

	TBuf<KMaxSettingStringLengthLong> longValue;
	longValue.Fill('x',KMaxSettingStringLengthLong);

	ImSmtpSettings->SetEmailAliasL(longValue);
	ImSmtpSettings->SetEmailAddressL(longValue);
	ImSmtpSettings->SetBodyEncoding(EMsgOutboxMIME);
	ImSmtpSettings->SetReplyToAddressL(longValue);
	ImSmtpSettings->SetReceiptAddressL(longValue);
	ImSmtpSettings->SetBodyEncoding(EMsgOutboxMIME);
	ImSmtpSettings->SetDefaultMsgCharSet(KMiutSmtpDefaultMsgCharSet);
	ImSmtpSettings->SetAddVCardToEmail(ETrue);
	ImSmtpSettings->SetAddSignatureToEmail(ETrue);
	ImSmtpSettings->SetRequestReceipts(ETrue);
	ImSmtpSettings->SetSendCopyToSelf(ESendNoCopy);
	ImSmtpSettings->SetSendMessageOption(ESendMessageImmediately);
	ImSmtpSettings->SetLoginNameL(testUtils->MachineName());
	ImSmtpSettings->SetPasswordL(testUtils->MachineName());

	_LIT(KComment, "\tCreated Long SMTP Settings");
	testUtils->WriteComment(KComment);
	}

LOCAL_C TBool CompareFilledSMTPSettings(TInt aTest)
	{
	TBool result = ETrue;
	_LIT(KComment, "\tComparing Filled SMTP Settings");
	testUtils->WriteComment(KComment);

	buffer1 = ImSmtpSettings->EmailAlias();
	buffer2 = ImSmtpSettingsVerify->EmailAlias();
	result &= CompareDescriptors(buffer1, buffer2, &KEmailAlias); 

	buffer1 = ImSmtpSettings->EmailAddress();
	buffer2 = ImSmtpSettingsVerify->EmailAddress();
	result &= CompareDescriptors(buffer1, buffer2, &KEmailAddress); 

	buffer1 = ImSmtpSettings->ReplyToAddress();
	buffer2 = ImSmtpSettingsVerify->ReplyToAddress();
	result &= CompareDescriptors(buffer1, buffer2, &KReplyToAddress);

	buffer1 = ImSmtpSettings->ReceiptAddress();
	buffer2 = ImSmtpSettingsVerify->ReceiptAddress();
	result &= CompareDescriptors(buffer1, buffer2, &KReceiptAddress);

	buf1 = ImSmtpSettings->LoginName();
	buf2 = ImSmtpSettingsVerify->LoginName();
	result &= CompareDescriptors(buf1, buf2, &KLoginName); 

	buf1 = ImSmtpSettings->Password();
	buf2 = ImSmtpSettingsVerify->Password();
	result &= CompareDescriptors(buf1, buf2, &KPassword); 

	result &= CompareBooleans(ImSmtpSettings->BodyEncoding(), ImSmtpSettingsVerify->BodyEncoding(), &KBodyEncoding);
	result &= CompareBooleans(ImSmtpSettings->AddVCardToEmail(), ImSmtpSettingsVerify->AddVCardToEmail(), &KAddVCardToEmail);
	result &= CompareBooleans(ImSmtpSettings->AddSignatureToEmail(), ImSmtpSettingsVerify->AddSignatureToEmail(), &KAddSignatureToEmail);
	result &= CompareBooleans(ImSmtpSettings->RequestReceipts(), ImSmtpSettingsVerify->RequestReceipts(), &KRequestReceipts);
	
	TBool result1=((ImSmtpSettings->BodyEncoding())==(ImSmtpSettingsVerify->BodyEncoding()));
	if (!result1)
		{
		_LIT(KComment, "\t\tBodyEncoding settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result1;

	TBool result2=((ImSmtpSettings->DefaultMsgCharSet())==(ImSmtpSettingsVerify->DefaultMsgCharSet()));
	if (!result2)
		{
		_LIT(KComment, "\t\tDefaultMsgCharSet settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result2;

	TBool result3=((ImSmtpSettings->SendCopyToSelf())==(ImSmtpSettingsVerify->SendCopyToSelf()));
	if (!result3)
		{
		_LIT(KComment, "\t\tSendCopyToSelf settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result3;

	TBool result4 = ((ImSmtpSettings->SendMessageOption())==(ImSmtpSettingsVerify->SendMessageOption()));
	if (!result4)
		{
		_LIT(KComment, "\t\tSendMessageOption settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result4;
	
	ResultOfTest(result,aTest);
	return result;
	}

LOCAL_C TBool CompareBlankSMTPSettings(TInt aTest)
	{
	// need to check each of the fields of the stored header (ImSmtpSettingsVerify) are 
	// the same as that originally entered (ImSmtpSettings).  Here the length should be 
	// 0 so we don't actually need to compare!

	TBool result = ETrue;
  	_LIT(KComment, "\tComparing Blank SMTP Settings");
	testUtils->WriteComment(KComment);

	result &= CompareBooleans((ImSmtpSettings->EmailAlias().Length()==0), (ImSmtpSettingsVerify->EmailAlias().Length()==0), &KEmailAlias);
	result &= CompareBooleans((ImSmtpSettings->EmailAddress().Length()==0), (ImSmtpSettingsVerify->EmailAddress().Length()==0), &KEmailAddress);
	result &= CompareBooleans((ImSmtpSettings->ReplyToAddress().Length()==0), (ImSmtpSettingsVerify->ReplyToAddress().Length()==0), &KReplyToAddress);
	result &= CompareBooleans((ImSmtpSettings->ReceiptAddress().Length()==0), (ImSmtpSettingsVerify->ReceiptAddress().Length()==0), &KReceiptAddress);
	result &= CompareBooleans((ImSmtpSettings->LoginName().Length()==0), (ImSmtpSettingsVerify->LoginName().Length()==0), &KLoginName);
	result &= CompareBooleans((ImSmtpSettings->Password().Length()==0), (ImSmtpSettingsVerify->Password().Length()==0), &KPassword);
	
	result &= CompareBooleans(ImSmtpSettings->AddVCardToEmail(), ImSmtpSettingsVerify->AddVCardToEmail(), &KAddVCardToEmail);
	result &= CompareBooleans(ImSmtpSettings->AddSignatureToEmail(), ImSmtpSettingsVerify->AddSignatureToEmail(), &KAddSignatureToEmail);
	result &= CompareBooleans(ImSmtpSettings->RequestReceipts(), ImSmtpSettingsVerify->RequestReceipts(), &KRequestReceipts);

	ResultOfTest(result,aTest);
	return result;
	}

LOCAL_C TBool CompareLongSMTPSettings(TInt aTest)
	{
	_LIT(KComment, "\tComparing Long SMTP Settings");
	testUtils->WriteComment(KComment);
	TBool result = ETrue;
	buffer1 = ImSmtpSettings->EmailAlias().Left(KMaxSettingStringLength);
	buffer2 = ImSmtpSettingsVerify->EmailAlias().Left(KMaxSettingStringLength);
	result &= CompareDescriptors(buffer1, buffer2, &KEmailAlias); 

	buffer1 = ImSmtpSettings->EmailAddress().Left(KMaxSettingStringLength);
	buffer2 = ImSmtpSettingsVerify->EmailAddress().Left(KMaxSettingStringLength);
	result &= CompareDescriptors(buffer1, buffer2, &KEmailAddress); 

	buffer1 = ImSmtpSettings->ReplyToAddress().Left(KMaxSettingStringLength);
	buffer2 = ImSmtpSettingsVerify->ReplyToAddress().Left(KMaxSettingStringLength);
	result &= CompareDescriptors(buffer1, buffer2, &KReplyToAddress);

	buffer1 = ImSmtpSettings->ReceiptAddress().Left(KMaxSettingStringLength);
	buffer2 = ImSmtpSettingsVerify->ReceiptAddress().Left(KMaxSettingStringLength);
	result &= CompareDescriptors(buffer1, buffer2, &KReceiptAddress);

	buf1 = ImSmtpSettings->LoginName().Left(KMaxSettingStringLength);
	buf2 = ImSmtpSettingsVerify->LoginName().Left(KMaxSettingStringLength);
	result &= CompareDescriptors(buf1, buf2, &KLoginName);

	buf1 = ImSmtpSettings->Password().Left(KMaxSettingStringLength);
	buf2 = ImSmtpSettingsVerify->Password().Left(KMaxSettingStringLength);
	result &= CompareDescriptors(buf1, buf2, &KPassword);

	result &= CompareBooleans(ImSmtpSettings->BodyEncoding(), ImSmtpSettingsVerify->BodyEncoding(), &KBodyEncoding);
	result &= CompareBooleans(ImSmtpSettings->AddVCardToEmail(), ImSmtpSettingsVerify->AddVCardToEmail(), &KAddVCardToEmail);
	result &= CompareBooleans(ImSmtpSettings->AddSignatureToEmail(), ImSmtpSettingsVerify->AddSignatureToEmail(), &KAddSignatureToEmail);
	result &= CompareBooleans(ImSmtpSettings->RequestReceipts(), ImSmtpSettingsVerify->RequestReceipts(), &KRequestReceipts);
	
	TBool result1=((ImSmtpSettings->BodyEncoding())==(ImSmtpSettingsVerify->BodyEncoding()));
	if (!result1)
		{
		_LIT(KComment, "\t\tBodyEncoding settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result1;

	TBool result2=((ImSmtpSettings->DefaultMsgCharSet())==(ImSmtpSettingsVerify->DefaultMsgCharSet()));
	if (!result2)
		{
		_LIT(KComment, "\t\tDefaultMsgCharSet settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result2;

	TBool result3=((ImSmtpSettings->SendCopyToSelf())==(ImSmtpSettingsVerify->SendCopyToSelf()));
	if (!result3)
		{
		_LIT(KComment, "\t\tSendCopyToSelf settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result3;

	TBool result4 = ((ImSmtpSettings->SendMessageOption())==(ImSmtpSettingsVerify->SendMessageOption()));
	if (!result4)
		{
		_LIT(KComment, "\t\tSendMessageOption settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result4;

	ResultOfTest(result,aTest);
	return result;
	}

LOCAL_C TBool CompareSMTPCopy()
	{
	TBool result = ETrue;
	_LIT(KComment, "Comparing Filled SMTP Settings");
	testUtils->WriteComment(KComment);
	buffer1 = ImSmtpSettings->EmailAlias();
	buffer2 = ImSmtpSettingsVerify->EmailAlias();
	result &= CompareDescriptors(buffer1, buffer2, &KEmailAlias); 

	buffer1 = ImSmtpSettings->EmailAddress();
	buffer2 = ImSmtpSettingsVerify->EmailAddress();
	result &= CompareDescriptors(buffer1, buffer2, &KEmailAddress); 

	buffer1 = ImSmtpSettings->ReplyToAddress();
	buffer2 = ImSmtpSettingsVerify->ReplyToAddress();
	result &= CompareDescriptors(buffer1, buffer2, &KReplyToAddress);

	buffer1 = ImSmtpSettings->ReceiptAddress();
	buffer2 = ImSmtpSettingsVerify->ReceiptAddress();
	result &= CompareDescriptors(buffer1, buffer2, &KReceiptAddress);

	buf1 = ImSmtpSettings->LoginName();
	buf2 = ImSmtpSettingsVerify->LoginName();
	result &= CompareDescriptors(buf1, buf2, &KLoginName);

	buf1 = ImSmtpSettings->Password();
	buf2 = ImSmtpSettingsVerify->Password();
	result &= CompareDescriptors(buf1, buf2, &KPassword);

	result &= CompareBooleans(ImSmtpSettings->BodyEncoding(), ImSmtpSettingsVerify->BodyEncoding(), &KBodyEncoding);
	result &= CompareBooleans(ImSmtpSettings->AddVCardToEmail(), ImSmtpSettingsVerify->AddVCardToEmail(), &KAddVCardToEmail);
	result &= CompareBooleans(ImSmtpSettings->AddSignatureToEmail(), ImSmtpSettingsVerify->AddSignatureToEmail(), &KAddSignatureToEmail);
	result &= CompareBooleans(ImSmtpSettings->RequestReceipts(), ImSmtpSettingsVerify->RequestReceipts(), &KRequestReceipts);
	
	TBool result1=((ImSmtpSettings->BodyEncoding())==(ImSmtpSettingsVerify->BodyEncoding()));
	if (!result1)
		{
		_LIT(KComment, "\t\tBodyEncoding settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result1;

	TBool result2=((ImSmtpSettings->DefaultMsgCharSet())==(ImSmtpSettingsVerify->DefaultMsgCharSet()));
	if (!result2)
		{
		_LIT(KComment, "\t\tDefaultMsgCharSet settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result2;

	TBool result3=((ImSmtpSettings->SendCopyToSelf())==(ImSmtpSettingsVerify->SendCopyToSelf()));
	if (!result3)
		{
		_LIT(KComment, "\t\tSendCopyToSelf settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result3;

	TBool result4 = ((ImSmtpSettings->SendMessageOption())==(ImSmtpSettingsVerify->SendMessageOption()));
	if (!result4)
		{
		_LIT(KComment, "\t\tSendMessageOption settings not Identical");
		testUtils->WriteComment(KComment);
		}
	result &= result4;
	
	return result;
	}

//


LOCAL_C TBool TestSMTPEqualityAndCopyConstructorsL(TInt aTest)
	{
	_LIT(KComment, "\tSMTP Equality and Copy Constructors Test");
	testUtils->WriteComment(KComment);
	entry = new(ELeave) TMsvEntry;
	entry->iDescription.Set(_L("Description"));
	entry->iDetails.Set(_L("Details"));
	entry->iType = KUidMsvServiceEntry;
	ImSmtpSettings=new(ELeave)CImSmtpSettings;
	ImSmtpSettingsVerify=new(ELeave)CImSmtpSettings;
	CreateFilledSMTPSettings();
	ImSmtpSettingsVerify->CopyL(*ImSmtpSettings);
	TBool result=CompareSMTPCopy();
	TBool result1=(*ImSmtpSettingsVerify==*ImSmtpSettings);
	if (!result1)
		{
		_LIT(KComment, "\t\tError in Equality Operator");
		testUtils->WriteComment(KComment);
		}
	else
		{
		_LIT(KComment, "\t\tEquality Operator test passed");
		testUtils->WriteComment(KComment);
		}
	result&=result1;

	delete entry;
	delete ImSmtpSettings;
	delete ImSmtpSettingsVerify;

	ResultOfTest(result,aTest);
	return result;
	}

	
//
//

LOCAL_C TBool FilledPOPAccountSettingsL(Apop apop, TInt aTest)
	{
	_LIT(KComment, "\tFilled POP Account Settings Test");
	testUtils->WriteComment(KComment);

	ImPop3Settings=new(ELeave)CImPop3Settings;
	ImSmtpSettings=new(ELeave)CImSmtpSettings;
	ImPop3SettingsVerify=new(ELeave)CImPop3Settings;
	ImSmtpSettingsVerify=new(ELeave)CImSmtpSettings;
	CImIAPPreferences* iap = CImIAPPreferences::NewLC();
	CImIAPPreferences* smtpIAP = CImIAPPreferences::NewLC();
		
	account->LoadPopSettingsL(popAccount, *ImPop3Settings);
	account->LoadPopIapSettingsL(popAccount, *iap);

	TSmtpAccount smtpAccount;
	account->GetSmtpAccountL(popAccount.iSmtpService, smtpAccount);
	account->LoadSmtpSettingsL(smtpAccount, *ImSmtpSettings);
	account->LoadSmtpIapSettingsL(smtpAccount, *smtpIAP);

	// check that Port has been set to 110
	TBool result = ImPop3Settings->Port()==110;

	CreateFilledPOP3Settings(apop);
	CreateFilledSMTPSettings();

	account->SavePopSettingsL(popAccount, *ImPop3Settings);
	account->SavePopIapSettingsL(popAccount, *iap);

	account->SaveSmtpSettingsL(smtpAccount, *ImSmtpSettings);
	account->SaveSmtpIapSettingsL(smtpAccount, *smtpIAP);

	account->LoadPopSettingsL(popAccount, *ImPop3SettingsVerify);
	account->LoadPopIapSettingsL(popAccount, *iap);

	account->LoadSmtpSettingsL(smtpAccount, *ImSmtpSettingsVerify);
	account->LoadSmtpIapSettingsL(smtpAccount, *smtpIAP);

	result&=CompareFilledPOP3Settings(aTest);
	result&=CompareFilledSMTPSettings(aTest);
	
	CleanupStack::PopAndDestroy(2, iap); // smtpIAP, iap
	delete ImSmtpSettingsVerify;
	delete ImPop3SettingsVerify;
	delete ImSmtpSettings;
	delete ImPop3Settings;

	return result;	
	}

LOCAL_C TBool FilledIMAPAccountSettingsL(TInt aTest)
	{
	_LIT(KComment, "\tFilled IMAP Account Settings Test");
	testUtils->WriteComment(KComment);

	ImImap4Settings=new(ELeave)CImImap4Settings;
	ImSmtpSettings=new(ELeave)CImSmtpSettings;
	ImImap4SettingsVerify=new(ELeave)CImImap4Settings;
	ImSmtpSettingsVerify=new(ELeave)CImSmtpSettings;
	CImIAPPreferences* iap = CImIAPPreferences::NewLC();
	CImIAPPreferences* smtpIAP = CImIAPPreferences::NewLC();
		
	account->LoadImapSettingsL(imapAccount, *ImImap4Settings);
	account->LoadImapIapSettingsL(imapAccount, *iap);

	TSmtpAccount smtpAccount;
	account->GetSmtpAccountL(imapAccount.iSmtpService, smtpAccount);
	account->LoadSmtpSettingsL(smtpAccount, *ImSmtpSettings);
	account->LoadSmtpIapSettingsL(smtpAccount, *smtpIAP);

	CreateFilledIMAP4Settings();
	CreateFilledSMTPSettings();

	account->SaveImapSettingsL(imapAccount, *ImImap4Settings);
	account->SaveImapIapSettingsL(imapAccount, *iap);

	account->SaveSmtpSettingsL(smtpAccount, *ImSmtpSettings);
	account->SaveSmtpIapSettingsL(smtpAccount, *smtpIAP);

	account->LoadImapSettingsL(imapAccount, *ImImap4SettingsVerify);
	account->LoadImapIapSettingsL(imapAccount, *iap);

	account->LoadSmtpSettingsL(smtpAccount, *ImSmtpSettingsVerify);
	account->LoadSmtpIapSettingsL(smtpAccount, *smtpIAP);
	
	TBool result=CompareFilledIMAP4Settings(aTest);
	result&=CompareFilledSMTPSettings(aTest);
	
	CleanupStack::PopAndDestroy(2, iap); // smtpIAP, iap
	delete ImSmtpSettingsVerify;
	delete ImImap4SettingsVerify;
	delete ImSmtpSettings;
	delete ImImap4Settings;

	return result;	
	}
	
LOCAL_C TBool BlankPOPAccountSettingsL(TInt aTest)
	{
	_LIT(KComment, "\tBlank POP Account Settings Test");
	testUtils->WriteComment(KComment);

	ImPop3Settings=new(ELeave)CImPop3Settings;
	ImSmtpSettings=new(ELeave)CImSmtpSettings;
	ImPop3SettingsVerify=new(ELeave)CImPop3Settings;
	ImSmtpSettingsVerify=new(ELeave)CImSmtpSettings;
	CImIAPPreferences* iap = CImIAPPreferences::NewLC();
	CImIAPPreferences* smtpIAP = CImIAPPreferences::NewLC();
		
	account->PopulateDefaultPopSettingsL(*ImPop3Settings, *iap);
	account->PopulateDefaultSmtpSettingsL(*ImSmtpSettings, *smtpIAP);

	account->SavePopSettingsL(popAccount, *ImPop3Settings);
	account->SavePopIapSettingsL(popAccount, *iap);

	TSmtpAccount smtpAccount;
	account->GetSmtpAccountL(popAccount.iSmtpService, smtpAccount);
	account->SaveSmtpSettingsL(smtpAccount, *ImSmtpSettings);
	account->SaveSmtpIapSettingsL(smtpAccount, *smtpIAP);

	account->LoadPopSettingsL(popAccount, *ImPop3SettingsVerify);
	account->LoadPopIapSettingsL(popAccount, *iap);

	account->LoadSmtpSettingsL(smtpAccount, *ImSmtpSettingsVerify);
	account->LoadSmtpIapSettingsL(smtpAccount, *smtpIAP);
	
	TBool result=CompareBlankPOP3Settings(aTest);
	result&=CompareBlankSMTPSettings(aTest);
	
	CleanupStack::PopAndDestroy(2, iap); // smtpIAP, iap
	delete ImSmtpSettingsVerify;
	delete ImPop3SettingsVerify;
	delete ImSmtpSettings;
	delete ImPop3Settings;

	return result;		
	}

LOCAL_C TBool BlankIMAPAccountSettingsL(TInt aTest)
	{
	_LIT(KComment, "\tBlank IMAP Account Settings Test");
	testUtils->WriteComment(KComment);

	ImImap4Settings=new(ELeave)CImImap4Settings;
	ImSmtpSettings=new(ELeave)CImSmtpSettings;
	ImImap4SettingsVerify=new(ELeave)CImImap4Settings;
	ImSmtpSettingsVerify=new(ELeave)CImSmtpSettings;
	CImIAPPreferences* iap = CImIAPPreferences::NewLC();
	CImIAPPreferences* smtpIAP = CImIAPPreferences::NewLC();
		
	account->PopulateDefaultImapSettingsL(*ImImap4Settings, *iap);
	account->PopulateDefaultSmtpSettingsL(*ImSmtpSettings, *smtpIAP);

	account->SaveImapSettingsL(imapAccount, *ImImap4Settings);
	account->SaveImapIapSettingsL(imapAccount, *iap);

	TSmtpAccount smtpAccount;
	account->GetSmtpAccountL(imapAccount.iSmtpService, smtpAccount);
	account->SaveSmtpSettingsL(smtpAccount, *ImSmtpSettings);
	account->SaveSmtpIapSettingsL(smtpAccount, *smtpIAP);

	account->LoadImapSettingsL(imapAccount, *ImImap4SettingsVerify);
	account->LoadImapIapSettingsL(imapAccount, *iap);

	account->LoadSmtpSettingsL(smtpAccount, *ImSmtpSettingsVerify);
	account->LoadSmtpIapSettingsL(smtpAccount, *smtpIAP);
	
	TBool result = CompareBlankIMAP4Settings(aTest);
	result&=CompareBlankSMTPSettings(aTest);
	
	CleanupStack::PopAndDestroy(2, iap); // smtpIAP, iap
	delete ImSmtpSettingsVerify;
	delete ImImap4SettingsVerify;
	delete ImSmtpSettings;
	delete ImImap4Settings;

	return result;		
	}

LOCAL_C TBool LongPOPAccountSettingsL(TInt aTest)
	{
	_LIT(KComment, "\tLong POP Account Settings Test");
	testUtils->WriteComment(KComment);

	ImPop3Settings=new(ELeave)CImPop3Settings;
	ImSmtpSettings=new(ELeave)CImSmtpSettings;
	ImPop3SettingsVerify=new(ELeave)CImPop3Settings;
	ImSmtpSettingsVerify=new(ELeave)CImSmtpSettings;
	CImIAPPreferences* iap = CImIAPPreferences::NewLC();
	CImIAPPreferences* smtpIAP = CImIAPPreferences::NewLC();

	account->LoadPopSettingsL(popAccount, *ImPop3Settings);
	account->LoadPopIapSettingsL(popAccount, *iap);

	TSmtpAccount smtpAccount;
	account->GetSmtpAccountL(popAccount.iSmtpService, smtpAccount);
	account->LoadSmtpSettingsL(smtpAccount, *ImSmtpSettings);
	account->LoadSmtpIapSettingsL(smtpAccount, *smtpIAP);

	CreateLongPOP3Settings();
	CreateLongSMTPSettings();

	account->SavePopSettingsL(popAccount, *ImPop3Settings);
	account->SavePopIapSettingsL(popAccount, *iap);

	account->SaveSmtpSettingsL(smtpAccount, *ImSmtpSettings);
	account->SaveSmtpIapSettingsL(smtpAccount, *smtpIAP);
	
	account->LoadPopSettingsL(popAccount, *ImPop3SettingsVerify);
	account->LoadPopIapSettingsL(popAccount, *iap);

	account->LoadSmtpSettingsL(smtpAccount, *ImSmtpSettingsVerify);
	account->LoadSmtpIapSettingsL(smtpAccount, *smtpIAP);

	TBool result=CompareLongPOP3Settings(aTest);
	result&=CompareLongSMTPSettings(aTest);

	CleanupStack::PopAndDestroy(2, iap); // smtpIAP, iap
	delete ImSmtpSettingsVerify;
	delete ImPop3SettingsVerify;
	delete ImSmtpSettings;
	delete ImPop3Settings;

	return result;		
	}	

LOCAL_C TBool LongIMAPAccountSettingsL(TInt aTest)
	{
	_LIT(KComment, "\tLong IMAP Settings Test");
	testUtils->WriteComment(KComment);

	ImImap4Settings=new(ELeave)CImImap4Settings;
	ImSmtpSettings=new(ELeave)CImSmtpSettings;
	ImImap4SettingsVerify=new(ELeave)CImImap4Settings;
	ImSmtpSettingsVerify=new(ELeave)CImSmtpSettings;
	CImIAPPreferences* iap = CImIAPPreferences::NewLC();
	CImIAPPreferences* smtpIAP = CImIAPPreferences::NewLC();

	account->LoadImapSettingsL(imapAccount, *ImImap4Settings);
	account->LoadImapIapSettingsL(imapAccount, *iap);

	TSmtpAccount smtpAccount;
	account->GetSmtpAccountL(imapAccount.iSmtpService, smtpAccount);
	account->LoadSmtpSettingsL(smtpAccount, *ImSmtpSettings);
	account->LoadSmtpIapSettingsL(smtpAccount, *smtpIAP);

	CreateLongIMAP4Settings();
	CreateLongSMTPSettings();

	account->SaveImapSettingsL(imapAccount, *ImImap4Settings);
	account->SaveImapIapSettingsL(imapAccount, *iap);

	account->SaveSmtpSettingsL(smtpAccount, *ImSmtpSettings);
	account->SaveSmtpIapSettingsL(smtpAccount, *smtpIAP);
	
	account->LoadImapSettingsL(imapAccount, *ImImap4SettingsVerify);
	account->LoadImapIapSettingsL(imapAccount, *iap);

	account->LoadSmtpSettingsL(smtpAccount, *ImSmtpSettingsVerify);
	account->LoadSmtpIapSettingsL(smtpAccount, *smtpIAP);

	TBool result=CompareLongIMAP4Settings(aTest);
	result&=CompareLongSMTPSettings(aTest);

	CleanupStack::PopAndDestroy(2, iap); // smtpIAP, iap
	delete ImSmtpSettingsVerify;
	delete ImImap4SettingsVerify;
	delete ImSmtpSettings;
	delete ImImap4Settings;

	return result;		
	}	

LOCAL_C TBool TestDeletingDefaultSMTPAccountL(TInt aTest)
	{
	_LIT(KComment, "\tDeleting Default SMTP Account Test");
	testUtils->WriteComment(KComment);
	
	// Delete default smtp account
	TSmtpAccount smtpAccount;
	account->GetSmtpAccountL(imapAccount.iSmtpService, smtpAccount);
	account->SetDefaultSmtpAccountL(smtpAccount);	
	account->DeleteSmtpAccountL(smtpAccount);
	
	// The following should return KErrNotFound
	TSmtpAccount tempAccount;
	
	TInt error = account->DefaultSmtpAccountL(tempAccount);
	
	TBool result = ETrue;
	if (error != KErrNotFound)
		{
		result = EFalse;
		}
	
	ResultOfTest(result, aTest);
	return result;			
	}
	
/**
Create a smtp account, set that as default account
Delete the default account and check whether the account still exists
Returns True if the default account is deleted
*/    	
LOCAL_C TBool TestDeleteDefaultSMTPAccountL(TInt aTest)
	{
	CImSmtpSettings* smtpSetting = new (ELeave) CImSmtpSettings;
	CleanupStack::PushL(smtpSetting);
	
	TSmtpAccount smtpAccount;
	CImIAPPreferences* smtpIAP = CImIAPPreferences::NewLC();
   	smtpAccount = account->CreateSmtpAccountL(imapAccount, *smtpSetting, *smtpIAP, EFalse);
	account->GetSmtpAccountL(imapAccount.iSmtpService, smtpAccount);
	account->SetDefaultSmtpAccountL(smtpAccount);	// Set the account to default account
   	account->DeleteDefaultSmtpAccountL(); 
	// check whether default account exists	
	TInt error = account->DefaultSmtpAccountL(smtpAccount);

	TBool result = ETrue;
	if (error != KErrNotFound)
		{
		result = EFalse;
		}
	CleanupStack::PopAndDestroy(2, smtpSetting);//smtpIAP,smtpSetting
	ResultOfTest(result, aTest);
	return result;			
	}

LOCAL_C TBool TestReadOnlyAccountSettingL(TInt aTest)
	{
	TBool result = ETrue;
	CImPop3Settings* popSetting = new (ELeave) CImPop3Settings;
	CleanupStack::PushL(popSetting);
	CImImap4Settings* imapSetting = new (ELeave) CImImap4Settings;
	CleanupStack::PushL(imapSetting);
	CImSmtpSettings* smtpSetting = new (ELeave) CImSmtpSettings;
	CleanupStack::PushL(smtpSetting);
	CImIAPPreferences* iAP = CImIAPPreferences::NewLC();
	
	// Test pop account
  	popAccount = account->CreatePopAccountL(KPOPAccountName, *popSetting, *iAP, EFalse);	
	TBool readOnly = account->IsAccountReadOnlyL(popAccount);	
	test(readOnly == EFalse);

  	popAccount = account->CreatePopAccountL(KPOPAccountName, *popSetting, *iAP, ETrue);	
	readOnly = account->IsAccountReadOnlyL(popAccount);	
	test(readOnly);

	// Test imap account
  	imapAccount = account->CreateImapAccountL(KIMAPAccountName, *imapSetting, *iAP, EFalse);	
	readOnly = account->IsAccountReadOnlyL(imapAccount);	
	test(readOnly == EFalse);

  	imapAccount = account->CreateImapAccountL(KIMAPAccountName, *imapSetting, *iAP, ETrue);	
	readOnly = account->IsAccountReadOnlyL(imapAccount);	
	test(readOnly);

	// Test smtp account
  	TSmtpAccount smtpAccount = account->CreateSmtpAccountL(KIMAPAccountName, *smtpSetting, *iAP, EFalse);	
	readOnly = account->IsAccountReadOnlyL(smtpAccount);	
	test(readOnly == EFalse);

  	smtpAccount = account->CreateSmtpAccountL(KIMAPAccountName, *smtpSetting, *iAP, ETrue);	
	readOnly = account->IsAccountReadOnlyL(smtpAccount);	
	test(readOnly);

  	smtpAccount = account->CreateSmtpAccountL(popAccount, *smtpSetting, *iAP, EFalse);	
	readOnly = account->IsAccountReadOnlyL(smtpAccount);	
	test(readOnly == EFalse);

  	smtpAccount = account->CreateSmtpAccountL(popAccount, *smtpSetting, *iAP, ETrue);	
	readOnly = account->IsAccountReadOnlyL(smtpAccount);	
	test(readOnly);

  	smtpAccount = account->CreateSmtpAccountL(imapAccount, *smtpSetting, *iAP, EFalse);	
	readOnly = account->IsAccountReadOnlyL(smtpAccount);	
	test(readOnly == EFalse);

  	smtpAccount = account->CreateSmtpAccountL(imapAccount, *smtpSetting, *iAP, ETrue);	
	readOnly = account->IsAccountReadOnlyL(smtpAccount);	
	test(readOnly);

	CleanupStack::PopAndDestroy(4, popSetting);
	ResultOfTest(result, aTest);
	return result;			
	}

//
_LIT(KPass,   "\tPASSED Test %d");
_LIT(KFailed, "\tFAILED Test %d");

LOCAL_C TBool RunTestL (TInt aTestNo)
{
	TBuf16<80> buf;
	TBool result = ETrue;
	testUtils->TestStart(aTestNo);

	switch(aTestNo)
		{
		case  1:  
			{
			result = FilledPOPAccountSettingsL(EApopTrue, aTestNo); 
			}
			break;
		case  2:  
			{
			result = FilledPOPAccountSettingsL(EApopFalse, aTestNo);
			}
			break; 
		case  3:  
			{
			result = FilledPOPAccountSettingsL(EApopOther, aTestNo);
			}
			break;
		case  4:  
			{
			result = BlankPOPAccountSettingsL(aTestNo); 
			}
			break;
		case  5:  
			{
			result = LongPOPAccountSettingsL(aTestNo);
			}
			break;
		case  6:  
			{
			result = TestPOP3EqualityAndCopyConstructorsL(aTestNo);
			}
			break;
		case  7:  
			{
			result = FilledIMAPAccountSettingsL(aTestNo); 
			}
			break;
		case  8:  
			{
			result = BlankIMAPAccountSettingsL(aTestNo);
			}
			break;
		case  9: 
			{
			result = LongIMAPAccountSettingsL(aTestNo);
			}
			break;
		case 10: 
			{
		    result = TestIMAP4EqualityAndCopyConstructorsL(aTestNo);
		    }
			break;
		case 11: 
			{
			 result = TestSMTPEqualityAndCopyConstructorsL(aTestNo);
			}
			break;
		case 12: 
			{
			 result = TestDeletingDefaultSMTPAccountL(aTestNo);
			}
			break;
		case 13: 
			{
			result = TestDeleteDefaultSMTPAccountL(aTestNo);		
			}
		  	break;		
		case 14: 
			{
			result = TestReadOnlyAccountSettingL(aTestNo);		
			}
		  	break;
		case 15: 
			{
			result = CreateIMAP4SettingsNegativeTest(aTestNo);		
			}
		  	break;		  			
		  
		}

	if (result)
		{
		buf.AppendFormat(KPass, aTestNo);
		testUtils->WriteComment(buf);
		}
	else
		{
		buf.AppendFormat(KFailed, aTestNo);
		testUtils->WriteComment(buf);
		}

	testUtils->TestFinish(aTestNo);
	return (result);
}



LOCAL_C void ServiceEntrySelectionL()
	{
	TBool result = ETrue;
	TInt TotalNumberOfTests = 15;// Amend this number when adding (+) or removing (-) tests
	
	for (TInt testNo = 1; testNo <= TotalNumberOfTests; testNo++)
	{	
	TRAPD(err,result &= RunTestL(testNo));
	if( err != KErrNone )
            {
            _LIT(KComment, "!!LEAVE - test case left.");
            test.Printf(KComment);

            result = EFalse;
            }
	}


	if (result)
		{
		_LIT(KComment, "PASSED All tests");
		testUtils->WriteComment(KComment);
		test.Printf(KComment);
		testUtils->TestHarnessCompleted();
		}
	else
		{
		_LIT(KComment, "FAILED At Least one test");
		test.Printf(KComment);
		testUtils->WriteComment(KComment);
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
	testUtils->ClearEmailAccountsL();
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();
	
	test.Next(_L("Create Data Component FileStores"));
	testUtils->CreateSmtpServerMtmRegL();
	testUtils->CreatePopServerMtmRegL();

	//	Loading the DLLs
	testUtils->CreateRegistryObjectAndControlL();
	testUtils->InstallSmtpMtmGroupL();
	testUtils->InstallPopMtmGroupL();
	testUtils->GoClientSideL();
	
	// Create email accounts
	account = CEmailAccounts::NewLC();	

	CImPop3Settings* popSetting = new (ELeave) CImPop3Settings;
	CleanupStack::PushL(popSetting);

	CImSmtpSettings* smtpSetting = new (ELeave) CImSmtpSettings;
	CleanupStack::PushL(smtpSetting);

	CImIAPPreferences* iap = CImIAPPreferences::NewLC();
	CImIAPPreferences* smtpIAP = CImIAPPreferences::NewLC();

	account->PopulateDefaultPopSettingsL(*popSetting, *iap);
	account->PopulateDefaultSmtpSettingsL(*smtpSetting, *smtpIAP);

	popAccount = account->CreatePopAccountL(KPOPAccountName, *popSetting, *iap, EFalse);

	TSmtpAccount smtpAccount;
	smtpAccount = account->CreateSmtpAccountL(popAccount, *smtpSetting, *smtpIAP, EFalse);

	CImImap4Settings* imapSetting = new (ELeave) CImImap4Settings;
	CleanupStack::PushL(imapSetting);

	account->PopulateDefaultImapSettingsL(*imapSetting, *iap);
	account->PopulateDefaultSmtpSettingsL(*smtpSetting, *smtpIAP);

	imapAccount = account->CreateImapAccountL(KIMAPAccountName, *imapSetting, *iap, EFalse);
	smtpAccount = account->CreateSmtpAccountL(imapAccount, *smtpSetting, *smtpIAP, EFalse);

	CleanupStack::PopAndDestroy(5, popSetting); // imapSetting, smtpIAP, iap, smtpSetting, popSetting

	TBuf<80> buf;

#if defined(__WINS__)	
	buf.Append(_L("WINS "));
#else
	buf.Append(_L("MARM "));
#endif
#if defined(_UNICODE)
	buf.Append(_L("U"));
#endif
#if defined(_DEBUG)
	buf.Append(_L("DEB"));
#else
	buf.Append(_L("REL"));
#endif
	testUtils->WriteComment(buf);
	}
	
LOCAL_C void Closedown()
	{
	CleanupStack::PopAndDestroy(3);  // scheduler, testUtils, account
	}


//

LOCAL_C void doMainL()
	{
	InitL();
	test.Printf(_L("Performing Tests"));

	ServiceEntrySelectionL();
	
	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_MIUT02 - Test classes derived from CBaseEmailSettings"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.Console()->SetPos(0, 13);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
