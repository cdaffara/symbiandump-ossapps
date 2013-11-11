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
// Name of test harness: T_MIUT04
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Tests storing/retrieving data from TMsvEmailEntry - most of the 
// functionality is tested - some new functions may not be tested!
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// None
// Intermediate files produced while running test harness:
// <DRIVE>:\msgtest\T_MIUT04\T_IMCM.log
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_MIUT04.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_MIUT04.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_MIUT04.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_MIUT04.exe on the other platform
// 
//

#include "emailtestutils.h"
#include "MIUTHDR.H"    // CImHeader
#include <miutlog.h>
#include <imapset.h>
#include <msvids.h>

// local variables etc //

RTest test(_L("MIUT04 - Test TMsvEmailEntry class"));
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CImLog* log;
LOCAL_D CEmailTestUtils* testUtils;

_LIT8(KNoDisconnectedOperations, "NoDisconnectedOperations");
_LIT8(KDisconnectedCreateOperation, "DisconnectedCreateOperation");
_LIT8(KDisconnectedDeleteOperation, "DisconnectedDeleteOperation");
_LIT8(KDisconnectedChangeOperation, "DisconnectedChangeOperation");
_LIT8(KDisconnectedCopyToOperation, "DisconnectedCopyToOperation");
_LIT8(KDisconnectedCopyFromOperation, "DisconnectedCopyFromOperation");
_LIT8(KDisconnectedCopyWithinServiceOperation, "DisconnectedCopyWithinServiceOp");
_LIT8(KDisconnectedMoveToOperation, "DisconnectedMoveToOperation");
_LIT8(KDisconnectedMoveFromOperation, "DisconnectedMoveFromOperation");
_LIT8(KDisconnectedMoveWithinServiceOperation, "DisconnectedMoveWithinServiceOp");
_LIT8(KDisconnectedSpecialOperation, "DisconnectedSpecialOperation");
_LIT8(KDisconnectedUnknownOperation, "DisconnectedUnknownOperation");
_LIT8(KDisconnectedMultipleOperation, "DisconnectedMultipleOperation");

//

LOCAL_C void ResultOfTest(TBool aResult,TInt aTestNo)
	{
	if (aResult)
		{
		test.Printf(_L("Passed test %d\n"),aTestNo);
		}
	else
		{
		test.Printf(_L("Failed test %d\n"),aTestNo);
		}
	}

/*LOCAL_C TBool CompareBooleans(TBool aBoolean1, TBool aBoolean2, TDesC8 aTestDescription)
	{
	_LIT8(KErrorReason, "\t\tSettings not set correctly for %S");
	TBuf8<80> buf;
	TBool result = (aBoolean1 == aBoolean2);
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &aTestDescription);
		log->AppendError(buf ,-1);
		}
	return result;
	}

LOCAL_C TBool CompareDescriptors(TDesC16& aDesC1, TDesC16& aDesC2, TDesC8 aTestDescription)
	{
	_LIT8(KErrorReason, "\t\tSettings not set correctly for %S");
	TBuf8<80> buf;
	TBool result = (aDesC1 == aDesC2);
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &aTestDescription);
		log->AppendError(buf ,-1);
		}
	return result;
	}

LOCAL_C TBool CompareDescriptors(TDesC8& aDesC1, TDesC8& aDesC2, TDesC8 aTestDescription)
	{
	_LIT8(KErrorReason, "\t\tSettings not set correctly for %S");
	TBuf8<80> buf;
	TBool result = (aDesC1 == aDesC2);
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &aTestDescription);
		log->AppendError(buf ,-1);
		}
	return result;
	}
*/

LOCAL_C TBool CompareDisconnectedOptions(TImDisconnectedOperationType type1, TImDisconnectedOperationType type2, const TDesC8* aTestDescription)
	{
	_LIT8(KErrorReason, "\t\tDisconnected Options not set correctly for %S");
	TBuf8<80> buf;
	TBool result = (type1 == type2);
	if (!result)
		{
		buf.AppendFormat(KErrorReason, &aTestDescription);
		log->AppendError(buf ,-1);
		}
	return result;
	}

//

LOCAL_C TBool TMsvEmailEntrySelectionL(TInt aTestNo)
	{
	log->AppendComment(_L8("\tEmail Entry Selection Test"));
	TMsvEntry* genericEntry = new (ELeave) TMsvEntry;
	genericEntry->iType = KUidMsvMessageEntry;
	genericEntry->iMtm = KUidMsgTypeIMAP4;
	genericEntry->iServiceId = 0x1087643;
	genericEntry->SetPriority(EMsvHighPriority);
	genericEntry->SetComplete(ETrue);
	genericEntry->SetVisible(ETrue);

	TBool result=ETrue;

	// test that iMtmDatax member variables have been reset
	TMsvEmailEntry* newEmailEntry = new (ELeave) TMsvEmailEntry();
	result &= !(newEmailEntry->MtmData1() && newEmailEntry->MtmData2() && newEmailEntry->MtmData3());
	if (!result)
		log->AppendError(_L8("\t\tTMsvEmailEntry Constructor hasn't reset iMtmData members"),-1);

	delete newEmailEntry;

	// test copy constructor...
	TMsvEmailEntry* EmailEntry = new (ELeave) TMsvEmailEntry(*genericEntry);


	EmailEntry->SetEncrypted(ETrue);
	TBool result1=(EmailEntry->Encrypted()!=EFalse);
	EmailEntry->SetEncrypted(EFalse);
	result1&=(EmailEntry->Encrypted()==EFalse);
	if (!result1)
		log->AppendError(_L8("\t\tSetEncrypted() not setting correct values"),-1);
	result&=result1;

	EmailEntry->SetSigned(ETrue);
	TBool result2=(EmailEntry->Signed()!=EFalse);
	EmailEntry->SetSigned(EFalse);
	result2&=(EmailEntry->Signed()==EFalse);
	if (!result2)
		log->AppendError(_L8("\t\tSetSigned() not setting correct values"),-1);
	result&=result2;

	EmailEntry->SetReceipt(ETrue);
	TBool result3=(EmailEntry->Receipt()!=EFalse);
	EmailEntry->SetReceipt(EFalse);
	result3&=(EmailEntry->Receipt()==EFalse);
	if (!result3)
		log->AppendError(_L8("\t\tSetReceipt() not setting correct values"),-1);
	result&=result3;

	
	// check MHTML flag
	EmailEntry->SetMHTMLEmail(ETrue);
	TBool resultp=(EmailEntry->MHTMLEmail()!=EFalse);
	EmailEntry->SetMHTMLEmail(EFalse);
	resultp&=(EmailEntry->MHTMLEmail()==EFalse);
	if (!resultp)
		log->AppendError(_L8("\t\tSetMHTMLEmail() not setting correct values"),-1);
	result&=resultp;

	EmailEntry->SetOrphan(ETrue);
	TBool result4=(EmailEntry->Orphan()!=EFalse);
	EmailEntry->SetOrphan(EFalse);
	result4&=(EmailEntry->Orphan()==EFalse);
	if (!result4)
		log->AppendError(_L8("\t\tSetOrphan() not setting correct values"),-1);
	result&=result4;

	EmailEntry->SetVCard(ETrue);
	TBool result5=(EmailEntry->VCard()!=EFalse);
	EmailEntry->SetVCard(EFalse);
	result5&=(EmailEntry->VCard()==EFalse);
	if (!result5)
		log->AppendError(_L8("\t\tSetVCard() not setting correct values"),-1);
	result&=result5;

	EmailEntry->SetVCalendar(ETrue);
	TBool result6=(EmailEntry->VCalendar()!=EFalse);
	EmailEntry->SetVCalendar(EFalse);
	result6&=(EmailEntry->VCalendar()==EFalse);
	if (!result6)
		log->AppendError(_L8("\t\tSetVCalendar() not setting correct values"),-1);
	result&=result6;

	EmailEntry->SetSubscribed(ETrue);
	TBool result7=(EmailEntry->Subscribed()!=EFalse);
	EmailEntry->SetSubscribed(EFalse);
	result7&=(EmailEntry->Subscribed()==EFalse);
	if (!result7)
		log->AppendError(_L8("\t\tSetSubscribed() not setting correct values"),-1);
	result&=result7;

	EmailEntry->SetUnreadIMAP4Flag(ETrue);
	TBool result8=(EmailEntry->UnreadIMAP4Flag()!=EFalse);
	EmailEntry->SetUnreadIMAP4Flag(EFalse);
	result8&=(EmailEntry->UnreadIMAP4Flag()==EFalse);
	if (!result8)
		log->AppendError(_L8("\t\tSetUnreadIMAP4Flag() not setting correct values"),-1);
	result&=result8;

	EmailEntry->SetSeenIMAP4Flag(ETrue);
	TBool result9=(EmailEntry->SeenIMAP4Flag()!=EFalse);
	EmailEntry->SetSeenIMAP4Flag(EFalse);
	result9&=(EmailEntry->SeenIMAP4Flag()==EFalse);
	if (!result9)
		log->AppendError(_L8("\t\tSetSeenIMAP4Flag() not setting correct values"),-1);
	result&=result9;

	EmailEntry->SetAnsweredIMAP4Flag(ETrue);
	TBool result10=(EmailEntry->AnsweredIMAP4Flag()!=EFalse);
	EmailEntry->SetAnsweredIMAP4Flag(EFalse);
	result10&=(EmailEntry->AnsweredIMAP4Flag()==EFalse);
	if (!result10)
		log->AppendError(_L8("\t\tSetAnsweredIMAP4Flag() not setting correct values"),-1);
	result&=result10;

	EmailEntry->SetFlaggedIMAP4Flag(ETrue);
	TBool result11=(EmailEntry->FlaggedIMAP4Flag()!=EFalse);
	EmailEntry->SetFlaggedIMAP4Flag(EFalse);
	result11&=(EmailEntry->FlaggedIMAP4Flag()==EFalse);
	if (!result11)
		log->AppendError(_L8("\t\tSetFlaggedIMAP4Flag() not setting correct values"),-1);
	result&=result11;

	EmailEntry->SetDeletedIMAP4Flag(ETrue);
	TBool result12=(EmailEntry->DeletedIMAP4Flag()!=EFalse);
	EmailEntry->SetDeletedIMAP4Flag(EFalse);
	result12&=(EmailEntry->DeletedIMAP4Flag()==EFalse);
	if (!result12)
		log->AppendError(_L8("\t\tSetDeletedIMAP4Flag() not setting correct values"),-1);
	result&=result12;

	EmailEntry->SetDraftIMAP4Flag(ETrue);
	TBool result13=(EmailEntry->DraftIMAP4Flag()!=EFalse);
	EmailEntry->SetDraftIMAP4Flag(EFalse);
	result13&=(EmailEntry->DraftIMAP4Flag()==EFalse);
	if (!result13)
		log->AppendError(_L8("\t\tSetDraftIMAP4Flag() not setting correct values"),-1);
	result&=result13;

	EmailEntry->SetRecentIMAP4Flag(ETrue);
	TBool result14=(EmailEntry->RecentIMAP4Flag()!=EFalse);
	EmailEntry->SetRecentIMAP4Flag(EFalse);
	result14&=(EmailEntry->RecentIMAP4Flag()==EFalse);
	if (!result14)
		log->AppendError(_L8("\t\tSetRecentIMAP4Flag() not setting correct values"),-1);
	result&=result14;

	EmailEntry->SetValidUID(ETrue);
	TBool result15=(EmailEntry->ValidUID()!=EFalse);
	EmailEntry->SetValidUID(EFalse);
	result15&=(EmailEntry->ValidUID()==EFalse);
	if (!result15)
		log->AppendError(_L8("\t\tSetValidUID() not setting correct values"),-1);
	result&=result15;

	EmailEntry->SetMailbox(ETrue);
	TBool result16=(EmailEntry->Mailbox()!=EFalse);
	EmailEntry->SetMailbox(EFalse);
	result16&=(EmailEntry->Mailbox()==EFalse);
	if (!result16)
		log->AppendError(_L8("\t\tSetMailbox() not setting correct values"),-1);
	result&=result16;

	EmailEntry->SetLocalSubscription(ETrue);
	TBool result17=(EmailEntry->LocalSubscription()!=EFalse);
	EmailEntry->SetLocalSubscription(EFalse);
	result17&=(EmailEntry->LocalSubscription()==EFalse);
	if (!result17)
		log->AppendError(_L8("\t\tSetLocalSubscription() not setting correct values"),-1);
	result&=result17;

	EmailEntry->SetIMAP4Flags(ETrue,EFalse,EFalse,EFalse,EFalse,EFalse,EFalse);
	TBool result18=(EmailEntry->UnreadIMAP4Flag()!=EFalse);
	EmailEntry->SetIMAP4Flags(EFalse,ETrue,EFalse,EFalse,EFalse,EFalse,EFalse);
	result18&=(EmailEntry->SeenIMAP4Flag()!=EFalse);
	EmailEntry->SetIMAP4Flags(EFalse,EFalse,ETrue,EFalse,EFalse,EFalse,EFalse);
	result18&=(EmailEntry->AnsweredIMAP4Flag()!=EFalse);
	EmailEntry->SetIMAP4Flags(EFalse,EFalse,EFalse,ETrue,EFalse,EFalse,EFalse);
	result18&=(EmailEntry->FlaggedIMAP4Flag()!=EFalse);
	EmailEntry->SetIMAP4Flags(EFalse,EFalse,EFalse,EFalse,ETrue,EFalse,EFalse);
	result18&=(EmailEntry->DeletedIMAP4Flag()!=EFalse);
	EmailEntry->SetIMAP4Flags(EFalse,EFalse,EFalse,EFalse,EFalse,ETrue,EFalse);
	result18&=(EmailEntry->DraftIMAP4Flag()!=EFalse);
	EmailEntry->SetIMAP4Flags(EFalse,EFalse,EFalse,EFalse,EFalse,EFalse,ETrue);
	result18&=(EmailEntry->RecentIMAP4Flag()!=EFalse);
	if (!result18)
		log->AppendError(_L8("\t\tSetIMAP4Flags() not setting correct values"),-1);
	result&=result18;

	// check folder Set/Get functions
	EmailEntry->SetMessageFolderType(EFolderTypeRelated);
	TBool result19=(EmailEntry->MessageFolderType()==EFolderTypeRelated);
	if (!result19)
		log->AppendError(_L8("\t\tSetMessageFolderType(EFolderTypeRelated) not setting correct values"),-1);
	result&=result19;

	EmailEntry->SetMessageFolderType(EFolderTypeAlternative);
	TBool result20=(EmailEntry->MessageFolderType()==EFolderTypeAlternative);
	if (!result20)
		log->AppendError(_L8("\t\tSetMessageFolderType(EFolderTypeAlternative) not setting correct values"),-1);
	result&=result20;

	EmailEntry->SetMessageFolderType(EFolderTypeMixed);
	TBool result21=(EmailEntry->MessageFolderType()==EFolderTypeMixed);
	if (!result21)
		log->AppendError(_L8("\t\tSetMessageFolderType(EFolderTypeMixed) not setting correct values"),-1);
	result&=result21;
	
	EmailEntry->SetMessageFolderType(EFolderTypeParallel);
	TBool result22=(EmailEntry->MessageFolderType()==EFolderTypeParallel);
	if (!result22)
		log->AppendError(_L8("\t\tSetMessageFolderType(EFolderTypeParallel) not setting correct values"),-1);
	result&=result22;
	
	EmailEntry->SetMessageFolderType(EFolderTypeDigest);
	TBool result23=(EmailEntry->MessageFolderType()==EFolderTypeDigest);
	if (!result23)
		log->AppendError(_L8("\t\tSetMessageFolderType(EFolderTypeDigest) not setting correct values"),-1);
	result&=result23;

	EmailEntry->SetMessageFolderType(EFolderTypePartial);
	TBool result24=(EmailEntry->MessageFolderType()==EFolderTypePartial);
	if (!result24)
		log->AppendError(_L8("\t\tSetMessageFolderType(EFolderTypePartial) not setting correct values"),-1);
	result&=result24;

	EmailEntry->SetMessageFolderType(EFolderTypeExternal);
	TBool result25=(EmailEntry->MessageFolderType()==EFolderTypeExternal);
	if (!result25)
		log->AppendError(_L8("\t\tSetMessageFolderType(EFolderTypeExternal) not setting correct values"),-1);
	result&=result25;

	EmailEntry->SetMessageFolderType(EFolderTypeUnknown);
	TBool result26=(EmailEntry->MessageFolderType()==EFolderTypeUnknown);
	if (!result26)
		log->AppendError(_L8("\t\tSetMessageFolderType(EFolderTypeUnknown) not setting correct values"),-1);
	result&=result26;

	EmailEntry->SetMessageFolderType(EFolderTypeDirectory);
	TBool result27=(EmailEntry->MessageFolderType()==EFolderTypeDirectory);
	if (!result27)
		log->AppendError(_L8("\t\tSetMessageFolderType(EFolderTypeDirectory) not setting correct values"),-1);
	result&=result27;

	TBool result28 = ETrue;
	// Huge test for all combinations of flag settings using the Get/Set inlines...
	for (TInt a=0;a<2;++a)
		{
		for (TInt b=0;b<2;++b)
			{
			for(TInt c=0;c<2;++c)
				{
				for (TInt d=0 ; d<2 ;++d)
					{
					for (TInt e=0;e<2 ;++e)
						{
						for (TInt f=0; f<2 ; ++f)
							{
							for (TInt g=0; g<2 ; ++g)
								{
								TBool ia=a==1;
								TBool ib=a==1;
								TBool ic=c==1;
								TBool id=d==1;
								TBool ie=e==1;
								TBool iff=f==1;
								TBool ig=g==1;
								EmailEntry->SetIMAP4Flags (ia,ib,ic,id,ie,iff,ig);
								TBool ra,rb,rc,rd,re,rf,rg;
								EmailEntry->GetIMAP4Flags (ra,rb,rc,rd,re,rf,rg);
								result28 &= (ia==(ra>0)) && ((rb>0)==ib) && (ic==(rc>0)) && (id==(rd>0)) && (ie==(re>0)) && (iff==(rf>0)) && ((rg>0)==ig);
								}	// g
							}	//f 
						}	// e
					}	// d
				}	// c
			}	// b
		}	// a
	if (!result28)
		log->AppendError(_L8("\t\tError in test for all combinations of flag settings using Get/Set inlines"),-1);
	result&=result28;

	const TUint32 KMyUID=8723640;

	TUint32 myUID=KMyUID;
	EmailEntry->SetUID(myUID);
	myUID=99;
	TBool result29=(EmailEntry->UID()==KMyUID);

	EmailEntry->SetIMAP4Flags(ETrue,EFalse,EFalse,ETrue,EFalse,ETrue,EFalse);
	TInt32 myInt = EmailEntry->MtmData1();
	delete genericEntry;
	TMsvEntry msvEntry=*EmailEntry;
	genericEntry=&msvEntry;
	result29&=(genericEntry->MtmData1()==myInt);

	msvEntry.SetMtmData2(KMyUID);
	TMsvEmailEntry msvEmailEntry1(msvEntry);
	TMsvEmailEntry msvEmailEntry2(msvEmailEntry1);

	result29 &= (msvEmailEntry2.UID()==KMyUID);	// test whether the copy constructors work ok...
	if (!result29)
		log->AppendError(_L8("\t\tCopy constructors do not work"),-1);
	result&=result29;

	TMsvEmailEntry msvEmailEntry3=msvEmailEntry1;	// test assignment operator
	TBool result30=(msvEmailEntry3.UID()==KMyUID);
	if (!result30)
		log->AppendError(_L8("\t\tTest Assignment Operator does not work"),-1);
	result&=result30;

	//test equality operator
	TBool result31=msvEmailEntry2==msvEmailEntry3;
	if (!result31)
		log->AppendError(_L8("\t\tTest Equality Operator does not work"),-1);
	result&=result31;

	TMsvEntry a2=msvEntry;

	// assignment TMsvEmailEntry->TMsvEntry
	a2 = msvEmailEntry1;

	TBool result32=(TUint32(a2.MtmData2())==KMyUID);
	result&=result32;
	
	// check ICalendar flag
	EmailEntry->SetICalendar(ETrue);
	TBool result33=(EmailEntry->ICalendar()!=EFalse);
	EmailEntry->SetICalendar(EFalse);
	result33&=(EmailEntry->ICalendar()==EFalse);
	if (!result33)
		log->AppendError(_L8("\t\tSetICalendar() not setting correct values"),-1);
	result&=result33;

	delete EmailEntry;

	ResultOfTest(result, aTestNo);
	return result;
	}

LOCAL_C TBool DisconnectedOperationTests(TInt aTestNo)
	{
	log->AppendComment(_L8("\tDisconnected Operation Tests"));

	TMsvEntry* genericEntry = new (ELeave) TMsvEntry;
	genericEntry->iType = KUidMsvMessageEntry;
	genericEntry->iMtm = KUidMsgTypePOP3;
	genericEntry->iServiceId = 0x1087643;
	genericEntry->SetPriority(EMsvHighPriority);
	genericEntry->SetComplete(ETrue);
	genericEntry->SetVisible(ETrue);

	TBool result=ETrue;

	TMsvEmailEntry* EmailEntry = new (ELeave) TMsvEmailEntry(*genericEntry);

	// check folder Set/Get functions
	EmailEntry->SetDisconnectedOperation(ENoDisconnectedOperations);
	result &= CompareDisconnectedOptions(EmailEntry->DisconnectedOperation(), ENoDisconnectedOperations, &KNoDisconnectedOperations);
	
	EmailEntry->SetDisconnectedOperation(EDisconnectedCreateOperation);
	result &= CompareDisconnectedOptions(EmailEntry->DisconnectedOperation(), EDisconnectedCreateOperation, &KDisconnectedCreateOperation);

	EmailEntry->SetDisconnectedOperation(EDisconnectedDeleteOperation);
	result &= CompareDisconnectedOptions(EmailEntry->DisconnectedOperation(), EDisconnectedDeleteOperation, &KDisconnectedDeleteOperation);

	EmailEntry->SetDisconnectedOperation(EDisconnectedChangeOperation);
	result &= CompareDisconnectedOptions(EmailEntry->DisconnectedOperation(), EDisconnectedChangeOperation, &KDisconnectedChangeOperation);

	EmailEntry->SetDisconnectedOperation(EDisconnectedCopyToOperation);
	result &= CompareDisconnectedOptions(EmailEntry->DisconnectedOperation(), EDisconnectedCopyToOperation, &KDisconnectedCopyToOperation);

	EmailEntry->SetDisconnectedOperation(EDisconnectedCopyFromOperation);
	result &= CompareDisconnectedOptions(EmailEntry->DisconnectedOperation(), EDisconnectedCopyFromOperation, &KDisconnectedCopyFromOperation);

	EmailEntry->SetDisconnectedOperation(EDisconnectedCopyWithinServiceOperation);
	result &= CompareDisconnectedOptions(EmailEntry->DisconnectedOperation(), EDisconnectedCopyWithinServiceOperation, &KDisconnectedCopyWithinServiceOperation);

	EmailEntry->SetDisconnectedOperation(EDisconnectedMoveToOperation);
	result &= CompareDisconnectedOptions(EmailEntry->DisconnectedOperation(), EDisconnectedMoveToOperation, &KDisconnectedMoveToOperation);

	EmailEntry->SetDisconnectedOperation(EDisconnectedMoveFromOperation);
	result &= CompareDisconnectedOptions(EmailEntry->DisconnectedOperation(), EDisconnectedMoveFromOperation, &KDisconnectedMoveFromOperation);

	EmailEntry->SetDisconnectedOperation(EDisconnectedMoveWithinServiceOperation);
	result &= CompareDisconnectedOptions(EmailEntry->DisconnectedOperation(), EDisconnectedMoveWithinServiceOperation, &KDisconnectedMoveWithinServiceOperation);

	EmailEntry->SetDisconnectedOperation(EDisconnectedSpecialOperation);
	result &= CompareDisconnectedOptions(EmailEntry->DisconnectedOperation(), EDisconnectedSpecialOperation, &KDisconnectedSpecialOperation);

	EmailEntry->SetDisconnectedOperation(EDisconnectedUnknownOperation);
	result &= CompareDisconnectedOptions(EmailEntry->DisconnectedOperation(), EDisconnectedUnknownOperation, &KDisconnectedUnknownOperation);

	EmailEntry->SetDisconnectedOperation(EDisconnectedMultipleOperation);
	result &= CompareDisconnectedOptions(EmailEntry->DisconnectedOperation(), EDisconnectedMultipleOperation, &KDisconnectedMultipleOperation);

	delete genericEntry;
	delete EmailEntry;

	ResultOfTest(result, aTestNo);
	return result;
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
	log->AppendComment(_L8("********** T_MIUT04 Test TMsvEmailEntry class **********"));
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
	log->AppendComment(_L8("************    T_MIUT04 Tests Complete    *************"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(3);  //testUtils, log, scheduler
	}

//

LOCAL_C void doMainL()
	{
	InitL();
	testUtils->GoClientSideL();
	test.Printf(_L("Performing Header Tests"));

	TBool result = ETrue;
	testUtils->TestStart(1);
	TBool result1 = TMsvEmailEntrySelectionL(1);
	result &= result1;
	if (result1)
		log->AppendComment(_L8("PASSED Test 1"));
	else
		log->AppendError(_L8("FAILED Test 1"),-1);
	testUtils->TestFinish(1);

	testUtils->TestStart(2);
	TBool result2 = DisconnectedOperationTests(2);
	result &= result2;
	if (result2)
		log->AppendComment(_L8("PASSED Test 2"));
	else
		log->AppendError(_L8("FAILED Test 2"),-1);
	testUtils->TestFinish(2);

	if (result)
		{
		log->AppendComment(_L8("PASSED All tests"));
		test.Printf(_L("PASSED all Tests"));
		testUtils->TestHarnessCompleted();
		}
	else
		{
		test.Printf(_L("FAILED Tests"));
		log->AppendError(_L8("FAILED At Least one test"),-1);
		testUtils->TestHarnessFailed(KErrGeneral);
		}

	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("MIUT04 - Test TMsvEmailEntry class"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
