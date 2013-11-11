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
// Name of test harness: T_IMCM06
// Component: IMCM
// Brief description of test harness:
// Tests the CImEmailMessage - GetBodyTextL(), GetMessageDigestEntriesL(), 
// GetAttachmentsListL() for EThisMessageAndEmbeddedMessages and 
// EThisMessageOnly types.  Also, it tests VCard,VCalenders, ICalendars and 
// VEntries for GetAttachmentsL().  Also performs heap failure tests.
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82201.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82201.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82202.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82202.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82203.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82203.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82204.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82204.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82205.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82205.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82206.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82206.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82207.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82207.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82208.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82208.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82209.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82209.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82210.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82210.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82211.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82211.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82212.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82212.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82213.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82213.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82214.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82214.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82215.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82215.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82216.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82216.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82217.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82217.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82218.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82218.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82219.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82219.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82220.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82220.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82221.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82221.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82222.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82222.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82223.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82223.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82224.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82224.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82225.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82225.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82226.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82226.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82227.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82227.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82228.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82228.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82229.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82229.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82230.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82230.txt
// \MSG\IMCMTSRC\RFCT_IMCM06\rfc82231.txt		<DRIVE>:\MSGTEST\IMCM\RFCT_IMCM06\rfc82231.txt
// Intermediate files produced while running test harness:
// None
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_IMCM06.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_IMCM06.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_IMCM06.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_IMCM06.exe on the other platform
// 
//

#include "emailtestutils.h"
#include <miutmsg.h>
#include <txtrich.h>

#include <mmsvattachmentmanager.h>
  
// local variables etc //

_LIT(KImcmTest, "T_IMCM06 - Testing URI resolving of real messages");
RTest test(KImcmTest);


LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D CRichText* rText;
LOCAL_D CParaFormatLayer* pLayer;
LOCAL_D CCharFormatLayer* cLayer;
LOCAL_D CActiveScheduler* scheduler;
LOCAL_D CMsvEntrySelection* msgList;
LOCAL_D TInt globalError;

//


LOCAL_C void Closedown()
	{
	if (globalError)
		testUtils->TestHarnessFailed(globalError);
	else
		testUtils->TestHarnessCompleted();
	CleanupStack::PopAndDestroy(2, scheduler);  //testUtils, scheduler
	}

//

LOCAL_C TInt AttachResult1L(const RPointerArray<CMsvAttachment>& array)
	{
	TInt error=0;
	error=(array.Count()==1)?KErrNone : KErrNotFound;
	if (error)
		{
		testUtils->WriteComment(_L("No attachments"));
		return error;
		}
	CMsvAttachment* attachmentInfo=array[0];
	if(attachmentInfo->Type()==(CMsvAttachment::EMsvFile) || attachmentInfo->Type()==(CMsvAttachment::EMsvLinkedFile))
		{
		error = (attachmentInfo->FilePath() == _L("C:\\Private\\1000484b\\Mail2\\00001001_S\\3\\00100003_F\\print.bmp")) ? KErrNone : KErrNotFound;
		if (error)
			{
			testUtils->WriteComment(_L("FilePath didn't match"));
			return error;
			}
		}
	else
		testUtils->WriteComment(_L("This is a message entry. Hence filename does not exist"));
		
	error=(attachmentInfo->Id()==0x00100003)?KErrNone : KErrNotFound;
	if (error)
		{
		testUtils->WriteComment(_L("Id didn't match"));
		return error;
		}
	error=(attachmentInfo->Complete())?KErrNone : KErrNotFound;
	if (error)
		{
		testUtils->WriteComment(_L("Complete didn't match"));
		return error;
		}
	return error;
	}
	

LOCAL_C TInt AttachResult3L(const RPointerArray<CMsvAttachment>& array)
	{
	TInt error=0;
	CMsvAttachment* attachmentInfo;
	error=(array.Count()==1)?KErrNone : KErrNotFound;
	if (error)
		{
		testUtils->WriteComment(_L("No attachments"));
		return error;
		}
	
	attachmentInfo=array[0];
	if(attachmentInfo->Type()==CMsvAttachment::EMsvFile || attachmentInfo->Type()==CMsvAttachment::EMsvLinkedFile)
		{
		error = (!attachmentInfo->FilePath().CompareF(_L("C:\\Private\\1000484b\\Mail2\\00001001_S\\7\\00100007_F\\Some Base64 encoded text"))) ? KErrNone : KErrNotFound;
		if (error)
			{
			testUtils->WriteComment(_L("FileName didn't match"));
			return error;
			}
		}
	else
		testUtils->WriteComment(_L("This is a message entry. Hence filename does not exist"));
	
	error=(attachmentInfo->Id()==0x00100007)?KErrNone : KErrNotFound;
	if (error)
		{
		testUtils->WriteComment(_L("Id didn't match"));
		return error;
		}
	error=(attachmentInfo->Complete())?KErrNone : KErrNotFound;
	if (error)
		{
		testUtils->WriteComment(_L("Complete didn't match"));
		return error;
		}
	return error;
	}
	

LOCAL_C void TestAttachmentResultsL(TInt aTest,const RPointerArray<CMsvAttachment>& array)
	{
	testUtils->TestStart(aTest);
	TInt error=0;
	switch (aTest)
		{
	case 0:  //has one attachment
		error=AttachResult1L(array);
		break;
	case 2:
		error=AttachResult3L(array);
		break;
	case 4:
		error=(array.Count()==3)? KErrNone:KErrNotFound;
		break;
	case 6:
		error=(array.Count()==6)?KErrNone : KErrNotFound;  
		break;
	case 8:
		error=(array.Count()==4)?KErrNone : KErrNotFound;  
		break;
	case 18:
	case 15:
		error=(array.Count()==1)?KErrNone : KErrNotFound;  
		break;
	case 5:
	case 9:
	case 19:
	case 20:
		error=(array.Count()==1)?KErrNone : KErrNotFound;  
		break;
	case 23:
		error=(array.Count()==3)? KErrNone:KErrNotFound;
		break;
	case 25:
		error=(array.Count()==3)?KErrNone : KErrNotFound;  
		break;
	case 27:
		error=(array.Count()==1)?KErrNone : KErrNotFound;  
		break;
	case 28:
		error=(array.Count()==3)?KErrNone : KErrNotFound;  
		break;
	case 29:
	case 30:
		error=(array.Count()==1)?KErrNone : KErrNotFound;  
		break;
	case 31:
		error=(array.Count()==1)?KErrNone : KErrNotFound;  
		break;				
	default:
		error=(array.Count()==0)?KErrNone : KErrNotFound;  
		break;
		}
	testUtils->TestFinish(aTest,error);
	if (!globalError)
		globalError=error;
	}
	

LOCAL_C	void TestAttachmentsResultsInEmbeddedMsgsL(TInt aTest,const RPointerArray<CMsvAttachment>& array)
	{
	testUtils->TestStart(aTest);
	TInt error=0;
	switch (aTest)
		{
	case 0:  //has one attachment
		error=AttachResult1L(array);
		break;
	case 2:
		error=AttachResult3L(array);
		break;
	case 4:
		error=(array.Count()==3)? KErrNone:KErrNotFound;
		break;
	case 6:
		error=(array.Count()==6)?KErrNone : KErrNotFound;  
		break;
	case 8:
		error=(array.Count()==4)?KErrNone : KErrNotFound;  
		break;
	case 5:
	case 9:
	case 19:
	case 20:
		error=(array.Count()==1)?KErrNone : KErrNotFound;  
		break;
	case 14:  //has one attachment
		{
		error=(array.Count()==2)?KErrNone : KErrNotFound;  //no attachments is 2
		if (error)
			{
			testUtils->WriteComment(_L("Number of attachments is wrong"));
			break;
			}
		CMsvAttachment* attachmentInfo1=array[0];
		CMsvAttachment* attachmentInfo2=array[1];

		if((attachmentInfo1->Type()==CMsvAttachment::EMsvFile || attachmentInfo1->Type()==CMsvAttachment::EMsvLinkedFile ) 
		&&(attachmentInfo2->Type()==CMsvAttachment::EMsvFile || attachmentInfo2->Type()==CMsvAttachment::EMsvLinkedFile ))
			{
			error = (!attachmentInfo1->FilePath().CompareF(_L("C:\\Private\\1000484b\\Mail2\\00001001_S\\3\\00100053_F\\WAP Security Group Meeting -- Minutes, final.doc")))
				&& (!attachmentInfo2->FilePath().CompareF(_L("C:\\Private\\1000484b\\Mail2\\00001001_S\\7\\00100057_F\\state_table_change.doc"))) ? KErrNone : KErrNotFound;
			if (error)
				{
				testUtils->WriteComment(_L("FileName didn't match"));
				break;
				}
			}
		else
			testUtils->WriteComment(_L("These are message entry. Hence filenames does not exist"));
		break;
		}
	case 15:
		{
		error=(array.Count()==2)?KErrNone : KErrNotFound;  //no attachments is 2
		if (error)
			{
			testUtils->WriteComment(_L("Number of attachments is wrong"));
			break;
			}
		CMsvAttachment* attachmentInfo1=array[0];
		CMsvAttachment* attachmentInfo2=array[1];

		if((attachmentInfo1->Type()==CMsvAttachment::EMsvFile || attachmentInfo1->Type()==CMsvAttachment::EMsvLinkedFile)
		&& (attachmentInfo2->Type()==CMsvAttachment::EMsvFile || attachmentInfo2->Type()==CMsvAttachment::EMsvLinkedFile))
			{
		error= (!attachmentInfo1->FilePath().CompareF(_L("C:\\Private\\1000484b\\Mail2\\00001001_S\\e\\0010005e_F\\Attachment.ll")))
				 && (!attachmentInfo2->FilePath().CompareF(_L("C:\\Private\\1000484b\\Mail2\\00001001_S\\f\\0010005f_F\\§§§.txt"))) ? KErrNone : KErrNotFound;
			if (error)
				{
				testUtils->WriteComment(_L("FileName didn't match"));
				break;
				}
			}
		else
			testUtils->WriteComment(_L("These are message entry. Hence filenames does not exist"));	
			
		break;
		}
	case 18:
		{
		error=(array.Count()==2)?KErrNone : KErrNotFound;  //no attachments is 2
		if (error)
			{
			testUtils->WriteComment(_L("Number of attachments is wrong"));
			break;
			}
		
		CMsvAttachment* attachmentInfo1=array[0];
		CMsvAttachment* attachmentInfo2=array[1];

		if((attachmentInfo1->Type()==CMsvAttachment::EMsvFile || attachmentInfo1->Type()==CMsvAttachment::EMsvLinkedFile) 
		&& (attachmentInfo2->Type()==CMsvAttachment::EMsvFile || attachmentInfo2->Type()==CMsvAttachment::EMsvLinkedFile))
			{
		error= (!attachmentInfo1->FilePath().CompareF(_L("C:\\Private\\1000484b\\Mail2\\00001001_S\\0\\00100070_F\\Attachment.ll")))
				 && (!attachmentInfo2->FilePath().CompareF(_L("C:\\Private\\1000484b\\Mail2\\00001001_S\\1\\00100071_F\\TELECA1.TXT.pgp"))) ? KErrNone : KErrNotFound;
			if (error)
				{
				testUtils->WriteComment(_L("FileName didn't match"));
				break;
				}
			}
		else
			testUtils->WriteComment(_L("These are message entry. Hence filenames does not exist"));	
		break;
		}
	case 21:
		{
		error=(array.Count()==1)?KErrNone : KErrNotFound;  //no attachments is 1
		if (error)
			{
			testUtils->WriteComment(_L("Number of attachments is wrong"));
			break;
			}
		
		CMsvAttachment* attachmentInfo1=array[0];
		if(attachmentInfo1->Type()==CMsvAttachment::EMsvFile || attachmentInfo1->Type()==CMsvAttachment::EMsvLinkedFile)
			{
		error=(!attachmentInfo1->FilePath().CompareF(_L("C:\\Private\\1000484b\\Mail2\\00001001_S\\4\\00100084_F\\Word file.doc"))) ? KErrNone : KErrNotFound;
			if (error)
				{
				testUtils->WriteComment(_L("FileName didn't match"));
				break;
				}
			}
			else
				testUtils->WriteComment(_L("This is a message entry. Hence filename does not exist"));	
			break;
		}
	case 23:
		error=(array.Count()==3)? KErrNone:KErrNotFound;
		break;
	case 25:
		error=(array.Count()==3)?KErrNone : KErrNotFound;  
		break;
	case 27:
		error=(array.Count()==1)? KErrNone : KErrNotFound;  
		break;
	case 28:
		error=(array.Count()==3)? KErrNone : KErrNotFound;  
		break;
	case 29:
	case 30:
		error=(array.Count()==1)? KErrNone : KErrNotFound;  
		break;
	case 31:
		error=(array.Count()==1)? KErrNone : KErrNotFound;  
		break;
    default:
		error=(array.Count()==0)?KErrNone : KErrNotFound;  
		if (error)
			{
			testUtils->WriteComment(_L("Number of attachments is wrong"));
			break;
			}
		break;
		}
	testUtils->TestFinish(aTest,error);
	if (!globalError)
		globalError=error;
	}


LOCAL_C void TestGetMsgDigestEntriesL(TInt aTest,const CMsvEntrySelection& aSel)
	{
	testUtils->TestStart(aTest);
	TInt error=0;
	switch (aTest)
		{
	case 14:  
		error=(aSel.Count()==3)?KErrNone:KErrNotFound;  //has 3 digest entries at the same level
		if (error)
			{
			testUtils->WriteComment(_L("Number of msgdigests are wrong"));
			break;
			}
		error=(aSel[0]==0x0010004e && aSel[1]==0x00100050 && aSel[2]==0x00100054)?KErrNone:KErrNotFound;
		if (error)
			{
			testUtils->WriteComment(_L("Ids don't match"));
			break;
			}
		break;
	case 15:
		error=(aSel.Count()==1)?KErrNone:KErrNotFound;  //has 3 digest entries at the same level
		if (error)
			{
			testUtils->WriteComment(_L("Number of msgdigests are wrong"));
			break;
			}
		error=(aSel[0]==0x0010005b)?KErrNone:KErrNotFound;
		if (error)
			{
			testUtils->WriteComment(_L("Id didn't match"));
			break;
			}
		break;
	case 16:
		error=(aSel.Count()==1)?KErrNone:KErrNotFound;  //has 3 digest entries at the same level
		if (error)
			{
			testUtils->WriteComment(_L("Number of msgdigests are wrong"));
			break;
			}
		error=(aSel[0]==0x00100063)?KErrNone:KErrNotFound;
		if (error)
			{
			testUtils->WriteComment(_L("Id didn't match"));
			break;
			}
		break;
	case 17:
		error=(aSel.Count()==1)?KErrNone:KErrNotFound;  //has 3 digest entries at the same level
		if (error)
			{
			testUtils->WriteComment(_L("Number of msgdigests are wrong"));
			break;
			}
		error=(aSel[0]==0x00100068)?KErrNone:KErrNotFound;
		if (error)
			{
			testUtils->WriteComment(_L("Id didn't match"));
			break;
			}
		break;
	case 18:
		error=(aSel.Count()==1)?KErrNone:KErrNotFound;  //has 3 digest entries at the same level
		if (error)
			{
			testUtils->WriteComment(_L("Number of msgdigests are wrong"));
			break;
			}
		error=(aSel[0]==0x0010006d)?KErrNone:KErrNotFound;
		if (error)
			{
			testUtils->WriteComment(_L("Id didn't match"));
			break;
			}
		break;
	case 21:
		error=(aSel.Count()==1)?KErrNone:KErrNotFound;  //has 3 digest entries at the same level
		if (error)
			{
			testUtils->WriteComment(_L("Number of msgdigests are wrong"));
			break;
			}
		error=(aSel[0]==0x0010007f)?KErrNone:KErrNotFound;
		if (error)
			{
			testUtils->WriteComment(_L("Id didn't match"));
			break;
			}

		break;
	case 24:
		error=(aSel.Count()==2)?KErrNone:KErrNotFound;  //has 3 digest entries at the same level
		if (error)
			{
			testUtils->WriteComment(_L("Number of msgdigests are wrong"));
			break;
			}
		error=(aSel[0]==0x0010008e && aSel[1]==0x00100090)?KErrNone:KErrNotFound;
		if (error)
			{
			testUtils->WriteComment(_L("Id didn't match"));
			break;
			}
		break;
	default:
		error=(aSel.Count()==0)?KErrNone : KErrNotFound; 
		break;
		}
	testUtils->TestFinish(aTest,error);
	if (!globalError)
		globalError=error;
	}


LOCAL_C void TestBodyTextL(TInt aTest,const CMsvEntrySelection& aSel,CImEmailMessage::TImEmailEntryType aEntryType)
	{
	testUtils->TestStart(aTest);
	TInt error=0;
	switch (aTest)
		{
	case 0:
		error=(aSel.Count()==1)?KErrNone:KErrNotFound;
		break;
	case 1:
		error=(aSel.Count()==1 )?KErrNone:KErrNotFound;
		break;
	case 7:
		error=(aSel.Count()==1 )?KErrNone:KErrNotFound;
		break;
	case 9:
		error=(aSel.Count()==2 )?KErrNone:KErrNotFound; 
		break;
	case 14:
		if (aEntryType==CImEmailMessage::EThisMessageOnly)
			error=(aSel.Count()==1 )?KErrNone:KErrNotFound;
		else
			error=(aSel.Count()==4 )?KErrNone:KErrNotFound;
		break;
	case 15:
		if (aEntryType==CImEmailMessage::EThisMessageOnly)
			error=(aSel.Count()==1 )?KErrNone:KErrNotFound;
		else
			error=(aSel.Count()==2 )?KErrNone:KErrNotFound;
		break;
	case 16:
		error=(aSel.Count()==1 )?KErrNone:KErrNotFound;
		break;
	case 17:
		if (aEntryType==CImEmailMessage::EThisMessageOnly)
			error=(aSel.Count()==1 )?KErrNone:KErrNotFound;
		else
			error=(aSel.Count()==2 )?KErrNone:KErrNotFound;
		break;
	case 18:
		if (aEntryType==CImEmailMessage::EThisMessageOnly)
			error=(aSel.Count()==1 )?KErrNone:KErrNotFound;
		else
			error=(aSel.Count()==2 )?KErrNone:KErrNotFound;
		break;
	case 20:
		error=(aSel.Count()==1 )?KErrNone:KErrNotFound;
		break;
	case 21:
		if (aEntryType==CImEmailMessage::EThisMessageOnly)
			error=(aSel.Count()==1 )?KErrNone:KErrNotFound;
		else
			error=(aSel.Count()==2 )?KErrNone:KErrNotFound;
		break;
	case 22:
		error=(aSel.Count()==1 )?KErrNone:KErrNotFound;
		break;
	case 24:
		if (aEntryType==CImEmailMessage::EThisMessageAndEmbeddedMessages)
			error=(aSel.Count()==2 )?KErrNone:KErrNotFound;
		break;
	case 26:
		error=(aSel.Count()==1 )?KErrNone:KErrNotFound;
		break;
	default:
		break;
		}
	testUtils->TestFinish(aTest,error);
	if (!globalError)
		globalError=error;
	}

LOCAL_C void GetAttachmentInfoSelectionL(CImEmailMessage& aEmailMessage,RPointerArray<CMsvAttachment>& aAttachmentInfoArray)
	{
	MMsvAttachmentManager& attachmentMgr = aEmailMessage.AttachmentManager();
	aAttachmentInfoArray.Reset();
	CMsvAttachment* attachmentInfo;
	for (TInt i= 0; i< attachmentMgr.AttachmentCount();i++)
		{
		attachmentInfo = attachmentMgr.GetAttachmentInfoL(i);
		aAttachmentInfoArray.Append(attachmentInfo);
		}
	}

LOCAL_C void GetAttachmentsL(TInt aTest,CImEmailMessage::TImAttachmentType aAttachmentType,CImEmailMessage::TImEmailEntryType aEntryType)
	{	
	RPointerArray<CMsvAttachment> attachmentArray;
	CTestActive* testActive = new (ELeave) CTestActive;
	CleanupStack::PushL(testActive);
	testActive->StartL();
	CImEmailMessage* emailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));
	emailMessage->GetAttachmentsListL(testActive->iStatus,(*msgList)[aTest],aAttachmentType,aEntryType);
	CActiveScheduler::Start();
	
	if (aTest==25 && (aAttachmentType==CImEmailMessage::EVCards || aAttachmentType==CImEmailMessage::EVEntries))  
		{//vcards test . there is only one msg with vcard
		testUtils->TestStart(aTest);
		if (aAttachmentType==CImEmailMessage::EVCards && emailMessage->AttachmentManager().AttachmentCount()==1)
	
			testUtils->TestFinish(aTest,0);
		else if (aAttachmentType==CImEmailMessage::EVEntries && emailMessage->AttachmentManager().AttachmentCount()==3)
			testUtils->TestFinish(aTest,0);
		else
			{
			testUtils->TestFinish(aTest,KErrNotFound);
			globalError=KErrNotFound;
			}
		}
		
	if ((aTest==27 || aTest==29 || aTest==30) && aAttachmentType==CImEmailMessage::EICalendars)  
		{
		testUtils->TestStart(aTest);
		TInt iCalCount = emailMessage->AttachmentManager().AttachmentCount();
		if (aAttachmentType==CImEmailMessage::EICalendars && emailMessage->AttachmentManager().AttachmentCount()==1)
			{
			testUtils->TestFinish(aTest,0);
			}
		else
			{
			testUtils->TestFinish(aTest,KErrNotFound);
			globalError=KErrNotFound;
			}
		}
		
	if (aTest==28 && aAttachmentType==CImEmailMessage::EVCalendars)  
		{
		testUtils->TestStart(aTest);
		TInt vCalCount = emailMessage->AttachmentManager().AttachmentCount();
		if (aAttachmentType==CImEmailMessage::EVCalendars && emailMessage->AttachmentManager().AttachmentCount()==2)
			{
			testUtils->TestFinish(aTest,0);
			}
		else
			{
			testUtils->TestFinish(aTest,KErrNotFound);
			globalError=KErrNotFound;
			}
		}
	
	if (aTest==31  && (aAttachmentType==CImEmailMessage::EVCards || aAttachmentType==CImEmailMessage::EVEntries))  
		{
		testUtils->TestStart(aTest);
		if (aAttachmentType==CImEmailMessage::EVCards && emailMessage->AttachmentManager().AttachmentCount()==1)
			testUtils->TestFinish(aTest,0);
		else if (aAttachmentType==CImEmailMessage::EVEntries && emailMessage->AttachmentManager().AttachmentCount()==1)
			testUtils->TestFinish(aTest,0);
		else
			{
			testUtils->TestFinish(aTest,KErrNotFound);
			globalError=KErrNotFound;
			}
		}
			
	if (aAttachmentType==CImEmailMessage::EAllAttachments && aEntryType==CImEmailMessage::EThisMessageOnly)
		{
		RPointerArray<CMsvAttachment> attachmentArray;
		GetAttachmentInfoSelectionL(*emailMessage,attachmentArray);
		TestAttachmentResultsL(aTest,attachmentArray);
		attachmentArray.ResetAndDestroy();
		}
	
		
	if (aAttachmentType==CImEmailMessage::EAllAttachments && aEntryType==CImEmailMessage::EThisMessageAndEmbeddedMessages)
		{
		GetAttachmentInfoSelectionL(*emailMessage,attachmentArray);
		TestAttachmentsResultsInEmbeddedMsgsL(aTest,attachmentArray);
		attachmentArray.ResetAndDestroy();
		}
	
		
	CleanupStack::PopAndDestroy(2,testActive); // emailMessage,testActive
	}

LOCAL_C void GetMsgDigestEntriesL(TInt aTest)
	{
	CTestActive* testActive = new (ELeave) CTestActive;
	CleanupStack::PushL(testActive);
	testActive->StartL();	
	CImEmailMessage* emailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));	
	emailMessage->GetMessageDigestEntriesL(testActive->iStatus,(*msgList)[aTest]);
	CActiveScheduler::Start();	
	TestGetMsgDigestEntriesL(aTest,emailMessage->Selection());
	CleanupStack::PopAndDestroy(2,testActive); // emailMessage,testActive
	}

LOCAL_C void SetCharacterSetAndGetMsgDigestL(TInt aTest)
	{
	CTestActive* testActive = new (ELeave) CTestActive;
	CleanupStack::PushL(testActive);
	testActive->StartL();
	CImEmailMessage* emailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));	
	emailMessage->SetCharacterSetL((*msgList)[aTest], 1);
	emailMessage->GetMessageDigestEntriesL(testActive->iStatus,(*msgList)[aTest]);
	CActiveScheduler::Start();	
	CleanupStack::PopAndDestroy(2, testActive); // emailMessage,testActive
	}

LOCAL_C void GetBodyTextL(TInt aTest,CImEmailMessage::TImEmailEntryType aEntryType)
	{
	CTestActive* testActive = new (ELeave) CTestActive;
	CleanupStack::PushL(testActive);
	testActive->StartL();
	CImEmailMessage* emailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));
	rText->Reset();	
	emailMessage->GetBodyTextL(testActive->iStatus,(*msgList)[aTest],aEntryType,*rText,*pLayer,*cLayer);
	CActiveScheduler::Start();	
	TestBodyTextL(aTest,emailMessage->Selection(),aEntryType);
	CleanupStack::PopAndDestroy(2, testActive); // emailMessage,testActive
	}

LOCAL_C void GetBodyTextWithHeapFailL(TInt aTest,CImEmailMessage::TImEmailEntryType aEntryType)
	{
	testUtils->TestStart(1);
	CTestActive* testActive = new (ELeave) CTestActive;
	CleanupStack::PushL(testActive);
	CImEmailMessage* emailMessage = NULL;
	TBool finished = EFalse;
#ifdef _DEBUG	
	TInt failCount=0; 
#endif	
	TInt err = KErrNone;
	while (!finished)
		{
		__UHEAP_FAILNEXT(failCount++);
		TRAP(err, emailMessage=CImEmailMessage::NewL(*(testUtils->iMsvEntry)));
		__UHEAP_RESET;
		if (err==KErrNone)
			{
			finished=ETrue;
			CleanupStack::PushL(emailMessage);
			}
		else
			{
			test(err==KErrNoMemory);
			test(emailMessage==NULL);
			}

		}
	testUtils->WriteComment(_L("CImEmailMessage::NewL Heapfailure test succesful"));
	testUtils->TestFinish(1,err);
#ifdef _DEBUG
	failCount=0;
#endif
	finished=EFalse;	
	testUtils->TestStart(2);
	testUtils->WriteComment(_L("Testing GetBodyText with HeapFailure"));
	while(!finished)
		{	
		__UHEAP_FAILNEXT(failCount++);
		rText->Reset();
		TRAP(err,emailMessage->GetBodyTextL(testActive->iStatus,(*msgList)[aTest],aEntryType,*rText,*pLayer,*cLayer));
		__UHEAP_RESET;
		if (err == KErrNone)
			{
			testActive->StartL();
			CActiveScheduler::Start();	
			}
		else
			test(err == KErrNoMemory);

		if (err== KErrNone && testActive->iStatus.Int()== KErrNone)
			{
			finished=ETrue;
			if (aEntryType==CImEmailMessage::EThisMessageOnly)
				{
				test(emailMessage->Selection().Count()==1);
				test(rText->DocumentLength()==128);
				}
			else
				{
				test(emailMessage->Selection().Count()==4);
				test(rText->DocumentLength()==128+375+327+142+3);
				}
			}
		}
	testUtils->TestFinish(2,err);
	CleanupStack::PopAndDestroy(2,testActive); // emailMessage,testActive
	}

LOCAL_C void GetAttachmentsWithHeapFailL(TInt aTest,CImEmailMessage::TImAttachmentType aAttachmentType,CImEmailMessage::TImEmailEntryType aEntryType)
	{
	testUtils->TestStart(1);
	CTestActive* testActive = new (ELeave) CTestActive;
	CleanupStack::PushL(testActive);
	TBool finished = EFalse;
#ifdef _DEBUG	
	TInt failCount=0;
#endif	
	TInt err = KErrNone;
	CImEmailMessage* emailMessage=NULL;
	while (!finished)
		{
		__UHEAP_FAILNEXT(failCount++);
		TRAP(err,emailMessage= CImEmailMessage::NewL(*(testUtils->iMsvEntry)));
		__UHEAP_RESET;
		if (err==KErrNone)
			{
			finished=ETrue;
			CleanupStack::PushL(emailMessage);
			}
		else
			{
			test(err==KErrNoMemory);
			test(emailMessage==NULL);
			}
		}
	testUtils->WriteComment(_L("CImEmailMessage::NewL tested with heapfailure"));
	testUtils->TestFinish(1,err);
#ifdef _DEBUG
	failCount=0;
#endif	
	finished=EFalse;
	err=KErrNone;
	testUtils->WriteComment(_L("Testing GetAttachmentsListL with heapfailure"));
	testUtils->TestStart(2);
	while(!finished)
		{
		__UHEAP_FAILNEXT(failCount++);
		TRAP(err,emailMessage->GetAttachmentsListL(testActive->iStatus,(*msgList)[aTest],aAttachmentType,aEntryType));
		__UHEAP_RESET;
		if (err == KErrNone)
			{
			testActive->StartL();
			CActiveScheduler::Start();	
			}
		else
			test(err == KErrNoMemory);
		
		if (err== KErrNone && testActive->iStatus.Int()== KErrNone)
			{
			finished=ETrue;
			test(emailMessage->AttachmentManager().AttachmentCount()==6);
		
			_LIT(KFilePath, "C:\\Private\\1000484b\\Mail2\\00001001_S\\3\\00100013_F\\text.txt");
			RPointerArray<CMsvAttachment> attachmentArray;
			GetAttachmentInfoSelectionL(*emailMessage,attachmentArray);		
			const TDesC& ref=(attachmentArray[0]->FilePath());
			test(ref==KFilePath);
			
			attachmentArray.ResetAndDestroy();			
			}
		}
	testUtils->TestFinish(2,err);
	CleanupStack::PopAndDestroy(2,testActive); // emailMessage,testActive
	}

LOCAL_C void InitL()
	{
	scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );
	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();
	}

LOCAL_C void doMainL()
	{
	InitL();
// Read the test message into the mail directory.
	testUtils->CreateMessageFilesL(KMsvGlobalInBoxIndexEntryId, KMsvDraftEntryId,_L("c:\\mailtest\\rfcT_IMCM06\\"));
	testUtils->GoClientSideL();
	testUtils->WriteComment(_L("T_IMCM06 Test CImEmailMessage Get- BodyText,Attachments,MsgDigest Methods"));
	testUtils->WriteComment(_L("this test code creates 31 messages of various structures and uses CImEmailMessage"));
	testUtils->WriteComment(_L("to get - attachments, text and digest entries.  It is important to maintain the order"));
	testUtils->WriteComment(_L("of the rfc files in t_imcm06 dir because the test code results are hardwired to the "));
	testUtils->WriteComment(_L("corresponding files i.e message ID,  attachment name."));

	CMsvEntry* entry = testUtils->iMsvEntry;
	entry->SetEntryL(KMsvDraftEntryId);
	msgList = entry->ChildrenL();
	CleanupStack::PushL(msgList);
	TInt count=msgList->Count();
	if (count)
		testUtils->WriteComment(_L("Messages created sucessfully"));
	else
		{
		testUtils->WriteComment(_L("Failed to create messages"));
		CleanupStack::PopAndDestroy();  //list
		Closedown();
		return;
		}
//	Testing attachments
	testUtils->WriteComment(_L("Testing Get AllAttachments in EThisMessageOnly"));
	for (TInt i=0;i<count;i++)
		GetAttachmentsL(i,CImEmailMessage::EAllAttachments,CImEmailMessage::EThisMessageOnly);
	testUtils->WriteComment(_L("Testing Get AllAttachments in EThisMessageOnly finished\n"));

	testUtils->WriteComment(_L("Testing Get EVCards in EThisMessageOnly"));
	for (TInt j=0;j<count;j++)
		GetAttachmentsL(j,CImEmailMessage::EVCards,CImEmailMessage::EThisMessageOnly);
	testUtils->WriteComment(_L("Testing Get EVCards in EThisMessageOnly finished\n"));

	testUtils->WriteComment(_L("Testing Get EVEntries in EThisMessageOnly"));
	for (TInt k=0;k<count;k++)
		GetAttachmentsL(k,CImEmailMessage::EVEntries,CImEmailMessage::EThisMessageOnly);
	testUtils->WriteComment(_L("Testing Get EVEntries in EThisMessageOnly finished\n"));
	
	testUtils->WriteComment(_L("Testing Get EICalendars in EThisMessageOnly"));
	for (TInt l=0;l<count;l++)
		GetAttachmentsL(l,CImEmailMessage::EICalendars,CImEmailMessage::EThisMessageOnly);
	testUtils->WriteComment(_L("Testing Get EICalendars in EThisMessageOnly finished\n"));
	
	testUtils->WriteComment(_L("Testing Get EVCalendars in EThisMessageOnly"));
	for (TInt m=0;m<count;m++)
		GetAttachmentsL(m,CImEmailMessage::EVCalendars,CImEmailMessage::EThisMessageOnly);
	testUtils->WriteComment(_L("Testing Get EVCalendars in EThisMessageOnly finished\n"));

	//	testing attachments in embeded messages
	testUtils->WriteComment(_L("Testing Get AllAttachments in EThisMessageAndEmbeddedMessages"));
	for (TInt n=0;n<count;n++)
		GetAttachmentsL(n,CImEmailMessage::EAllAttachments,CImEmailMessage::EThisMessageAndEmbeddedMessages);
	testUtils->WriteComment(_L("Testing Get AllAttachments in EThisMessageAndEmbeddedMessages finished\n"));

	//	testing getmsgdigest entries
	testUtils->WriteComment(_L("Testing GetMessageDigestEntriesL"));
	for (TInt o=0;o<count;o++)
		GetMsgDigestEntriesL(o);
	testUtils->WriteComment(_L("Testing GetMessageDigestEntriesL finished\n"));

	// This email number 14 has embedded messages and attachments
	// Test added for the defect DEF064292: Crash in CImEmailMessage
	testUtils->WriteComment(_L("Testing SetCharacterSetAndGetMsgDigestL"));
	SetCharacterSetAndGetMsgDigestL(14);
	testUtils->WriteComment(_L("Testing SetCharacterSetAndGetMsgDigestL finished\n"));
	
	//	testing getbody text
	pLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(pLayer);
	cLayer = CCharFormatLayer::NewL();
	CleanupStack::PushL(cLayer);
	rText = CRichText::NewL(pLayer, cLayer);
	CleanupStack::PushL(rText);

	testUtils->WriteComment(_L("Testing GetBodyTextL for EThisMessageOnly"));
	for (TInt p=0;p<count;p++)
		GetBodyTextL(p,CImEmailMessage::EThisMessageOnly);
	testUtils->WriteComment(_L("Testing GetBodyTextL for EThisMessageOnly finished"));
	
	testUtils->WriteComment(_L("Testing GetBodyTextL for EThisMessageAndEmbeddedMessages"));
	for (TInt q=0;q<count;q++)
		GetBodyTextL(q,CImEmailMessage::EThisMessageAndEmbeddedMessages);
	testUtils->WriteComment(_L("Testing GetBodyTextL for EThisMessageAndEmbeddedMessages finished"));
	
	testUtils->WriteComment(_L("Testing with HeapFailure"));	
	
	GetBodyTextWithHeapFailL(14,CImEmailMessage::EThisMessageOnly);

	testUtils->WriteComment(_L("Testing GetBodyTextWithHeapFailL for EThisMessageAndEmbeddedMessages"));
	GetBodyTextWithHeapFailL(14,CImEmailMessage::EThisMessageAndEmbeddedMessages);

	GetAttachmentsWithHeapFailL(6,CImEmailMessage::EAllAttachments,CImEmailMessage::EThisMessageOnly);

	CleanupStack::PopAndDestroy(4,msgList); //newMessageList,text,player,clayer
	Closedown();
	}


GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_IMCM06 Test CImEmailMessage Get- BodyText,Attachments,MsgDigest Methods"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	return(KErrNone);
	}
