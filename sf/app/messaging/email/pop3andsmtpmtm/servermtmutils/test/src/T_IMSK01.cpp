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
//

#include "testenv.h"
#include <agentclient.h>
#include "nd_err.h"

#include "imsk.h"

#include <s32mem.h>
#include <s32file.h>
#include <iapprefs.h>
#include "EmailTestUtils.h"

const TInt KMaxSizeOfBinaryData = 200;
LOCAL_D RTest test(_L("IMSK CImTextServerSession Test"));
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D TBuf8<KMaxSizeOfBinaryData> someBinaryData=_L8("");

LOCAL_D RFs theFs;	
LOCAL_D CActiveScheduler* scheduler;

LOCAL_D CEmailTestUtils* testUtils; 


#define EActivePriorityLow		-1
const TUid KUidMsvMessageDriveStream={0x1000163E};

LOCAL_C void DisplayErrorReason(TInt& aReason)
	{
	if (aReason!=KErrNone)
		{
		testUtils->WriteComment(_L("\nERROR - "));
		}

	TBuf<64> logString;
	switch (aReason)
		{
	case KErrNone:
		testUtils->WriteComment(_L("Session completed successfully"));
		break;
	case KErrCancel:
		testUtils->WriteComment(_L("Session cancelled"));
		logString.Format(_L(" Session Cancelled %d"),KErrCancel);
		testUtils->WriteComment(logString);
//		log->AppendError(_L8("\t\tSession Cancelled"),KErrCancel);
		break;
	case KErrAlreadyExists:
		testUtils->WriteComment(_L(" something Already Exists"));
		logString.Format(_L(" something Already Exists %d"),KErrAlreadyExists);
		testUtils->WriteComment(logString);
//		log->AppendError(_L8("\t\tsomething Already Exists"),KErrAlreadyExists);
		break;
	case KErrEof:
		testUtils->WriteComment(_L("No messages to send"));
		logString.Format(_L(" No messages to send %d"),KErrEof);
		testUtils->WriteComment(logString);
//		log->AppendError(_L8("\t\tNo messages to send"),KErrEof);
		break;
	case KErrNoMemory:
		testUtils->WriteComment(_L("No memory"));
		logString.Format(_L(" No Memory %d"),KErrNoMemory);
		testUtils->WriteComment(logString);
//		log->AppendError(_L8("\t\tNo Memory"),KErrNoMemory);
		break;
	case KErrAccessDenied:
		testUtils->WriteComment(_L("Switch link off"));
		logString.Format(_L(" Switch link off %d"),KErrAccessDenied);
		testUtils->WriteComment(logString);
	//	log->AppendError(_L8("\t\tSwitch link off"),KErrAccessDenied);
		break;
	case KErrExitNoModem:
		testUtils->WriteComment(_L("No modem"));
		logString.Format(_L(" No Modem %d"),KErrExitNoModem);
		testUtils->WriteComment(logString);
//		log->AppendError(_L8("\t\tNo Modem"),KErrExitNoModem);
		break;
	case KErrExitLoginFail:
		testUtils->WriteComment(_L("Login failed"));
		logString.Format(_L(" Login Failed %d"),KErrExitLoginFail);
		testUtils->WriteComment(logString);
//		log->AppendError(_L8("\t\tLogin Failed"),KErrExitLoginFail);
		break;
	case KErrExitModemError:
		testUtils->WriteComment(_L("Modem error"));
		logString.Format(_L(" Modem Error %d"),KErrExitModemError);
		testUtils->WriteComment(logString);
//		log->AppendError(_L8("\t\tModem Error"),KErrExitModemError);
		break;
	case KErrExitScriptError:
		testUtils->WriteComment(_L("Script error"));
		logString.Format(_L(" Script Error %d"),KErrExitScriptError);
		testUtils->WriteComment(logString);
//		log->AppendError(_L8("\t\tScript Error"),KErrExitScriptError);
		break;
	case KImskErrorDNSNotFound:
		testUtils->WriteComment(_L("Failed to find SMTP server"));
		logString.Format(_L(" Failed to find SMTP server %d"),KImskErrorDNSNotFound);
		testUtils->WriteComment(logString);
//		log->AppendError(_L8("\t\tFailed to find SMTP server"),KImskErrorDNSNotFound);
		break;
	case KErrTimedOut:
		testUtils->WriteComment(_L("Timed out waiting to communicate to server"));
		logString.Format(_L(" Timed out waiting to communicate to server %d"),KErrTimedOut);
		testUtils->WriteComment(logString);
//		log->AppendError(_L8("\t\tTimed out waiting to communicate to server"),KErrTimedOut);
		break;
	case KErrDisconnected:
		testUtils->WriteComment(_L("Server disconnected from socket"));
		logString.Format(_L(" Server disconnected from socket %d"),KErrDisconnected);
		testUtils->WriteComment(logString);
//		log->AppendError(_L8("\t\tServer disconnected from socket"),KErrDisconnected);
		break;
	default:
		// unknown error 
//		testUtils->WriteComment(TRefByValue<const TDesC>_L("%d"),aReason);
		logString.Format(_L(" Error %d"),aReason);
		testUtils->WriteComment(logString);
	//	log->AppendError(_L8("\t\tError"),aReason);
		break;
		}
	}

// create our own active scheduler class

class CExampleScheduler : public CActiveScheduler
	{
public:
	void Error (TInt aError) const;
	};

void CExampleScheduler::Error(TInt aError) const
	{
	DisplayErrorReason(aError);
//	User::Panic(_L("CExampleScheduler-error"),aError);
	}


// create an active object to send the message

class CSendMessage : public CActive
	{
public: // functions
	// construct/destruct
	CSendMessage();
	static CSendMessage *NewL();
	static CSendMessage *NewLC();
	void ConstructL();
	~CSendMessage();
	// issue request
	void RequestSendL();
	void QueueNextReceive();
	
	void RunL();
	void DoCancel();

private:
	CImTextServerSession*	iImSocket;
	TInt					iCtr;
	TStreamId				iId;
	HBufC*					iBuf;
	TInt					iCurrentState;
	TInt					iLineStatus;
	TBool					iReceive;
	TSockXfrLength			iLen;
	CImIAPPreferences*		iIAPPreferences;
	};

CSendMessage::CSendMessage() // construct high-priority active object
	: CActive(EActivePriorityLow)
	{
	}

CSendMessage  *CSendMessage::NewLC()
	{
	CSendMessage* self=new (ELeave) CSendMessage();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSendMessage  *CSendMessage::NewL()
	{
	CSendMessage* self=NewLC();
	CleanupStack::Pop();
	return self;
	}

void CSendMessage::ConstructL()
	{
	iCtr=0;
	CActiveScheduler::Add(this); // add to active scheduler
	iImSocket= CImTextServerSession::NewL();
	iReceive=EFalse;
	}

CSendMessage::~CSendMessage()
	{ 
	Cancel(); // make sure we're cancelled
	delete iImSocket;
	delete iBuf;
	delete iIAPPreferences;
	}

void  CSendMessage::DoCancel()
	{
	iImSocket->Cancel();
	}

void  CSendMessage::RunL()
	{
	TBuf<64> logString;
	TInt error;
	TInt length=0;
	switch (iCtr)
		{
	case 0:	// iSocket.Connect()
		if(iStatus.Int()!=KErrNone)
			{
			error=iStatus.Int();
			DisplayErrorReason(error);
			CActiveScheduler::Stop();
			}
		else
			{	
			iReceive=ETrue;
			QueueNextReceive();
			iCtr++;
			}
		break;

	case 4:	// have set encryption
		iReceive=EFalse;
		RequestSendL();
		iCtr++;
		break;

	case 3:
		iCtr++;
	case 1:	// response from sending user name
	case 2:	// response from sending password
	//case 3:
	case 5:
		someBinaryData=_L8("");
		if(iStatus.Int()!=KErrNone)
			{
//			testUtils->WriteComment(TRefByValue<const TDesC>_L("Error during receipt %d"),iStatus.Int());
			logString.Format(_L(" Error during receipt%d"),iStatus.Int());
			testUtils->WriteComment(logString);
	//		log->AppendError(_L8("\t\tError during receipt"),iStatus.Int());
			CActiveScheduler::Stop();
			}
		else
			{
			TBuf8<40> buffer;
			TBuf<40> buffer16;
			iLineStatus=iImSocket->GetCurrentTextLine(buffer);
			buffer16.Copy(buffer);
			test.Printf(TRefByValue<const TDesC>_L("%S"),&buffer16);
			if(iReceive)
				{
				if(iLineStatus==EBufferTooSmall)
					{
					while(iLineStatus!=ECRLFTerminated)
						{
						iLineStatus=iImSocket->GetCurrentTextLine(buffer);
						test.Printf(TRefByValue<const TDesC>_L("%S"),&buffer);
						}
					}
				iReceive=EFalse;
				RequestSendL();
				iCtr++;
				}
			else
				{
				iReceive=ETrue;
 				QueueNextReceive();
				}
			}
		break;

	case 6:	// test BinaryDataReceive()
		if(iStatus.Int()!=KErrNone)
			{
			test.Printf(TRefByValue<const TDesC>_L("Error during receipt %d"),iStatus.Int());
			logString.Format(_L(" Error during receipt%d"),iStatus.Int());
			testUtils->WriteComment(logString);
//			log->AppendError(_L8("\t\tError during receipt"),iStatus.Int());
			CActiveScheduler::Stop();
			break;
			}

		length=someBinaryData.Length();
		if (length>2)
			{
			TBuf<KMaxSizeOfBinaryData> buffer16;
			buffer16.Copy(someBinaryData);
			test.Printf(TRefByValue<const TDesC>_L("%S"),&buffer16);
			if ((someBinaryData[length-3]=='.' && someBinaryData[length-2]==0x0D && someBinaryData[length-1]==0x0A)
				||(someBinaryData[length-3]=='-' && someBinaryData[length-2]=='E' && someBinaryData[length-1]=='R'))
				{
				// got ".CRLF" string (-ERR string) from LIST command, so go onto next state
				iReceive=EFalse;
				RequestSendL();
				iCtr++;
				break;
				}
			}
		// queue a read
		iReceive=ETrue;
		iImSocket->ReceiveBinaryData(iStatus,someBinaryData,someBinaryData.MaxLength());
		SetActive();
		break;

	default:
		iCtr=6;
		RequestSendL();
		break;
	case 7:
		iImSocket->LogError(_L8("\t\tEnd of POP3 test session; completion code="),iStatus.Int());
		TBuf8<KImMailMaxBufferSize> buffer;
		iLineStatus=iImSocket->GetCurrentTextLine(buffer);
		test.Printf(TRefByValue<const TDesC>_L("Final %S\n "),&buffer);
		CActiveScheduler::Stop();
		TInt error=iStatus.Int();
		DisplayErrorReason(error);
		if (error==KErrNone)
			testUtils->WriteComment(_L("PASSED test"));
		else
			testUtils->WriteComment(_L("FAILED test"));
		break;
		} // end of switch
	}


void CSendMessage::RequestSendL()
	{
	_LIT8(KImUser,"USER abf75\r\n");
	_LIT8(KImPass,"PASS oodifama\r\n");
	_LIT8(KImStat,"STAT\r\n");
	_LIT8(KImList,"LIST\r\n");
	_LIT8(KImQuit,"QUIT\r\n");

	TRequestStatus* p=&iStatus;
	switch(iCtr)
		{
	case 0:
		test.Next(_L("QueueConnect"));
		delete iIAPPreferences;
		iIAPPreferences = 0;
		iIAPPreferences = CImIAPPreferences::NewLC();
		CleanupStack::Pop(); // iIAPPrefernces
		// Add IAP's to iIAPPreferences
		TImIAPChoice iap;
		iap.iIAP = 3;
		iap.iDialogPref = ECommDbDialogPrefPrompt;
//		iIAPPreferences->AddIAPL(iap,0);
		iap.iIAP = 3;
		iap.iDialogPref = ECommDbDialogPrefPrompt;
//		iIAPPreferences->AddIAPL(iap,1);
		iImSocket->QueueConnectL(iStatus,_L("158.43.128.67"),110,*iIAPPreferences);
//		iImSocket->QueueConnectL(iStatus,_L("0:0:0:0:0:0:194.217.242.23"),110,*iIAPPreferences);
		test.Next(_L("Connect Queued"));
		testUtils->WriteComment(_L("Connect Queued"));
		break;
	case 1:
//        iImSocket->LogText(_L8("First IAP should fail (scripted)"));
 //       iImSocket->LogText(_L8("Second IAP should fail (iap 30 doesn't exist)"));
  //      iImSocket->LogText(_L8("Third should work iap 3 demon"));
        testUtils->WriteComment(_L("IP address of client is "));
        testUtils->WriteComment(iImSocket->LocalName());
//        test.Printf(_L("\n"));

		TUint32 convalue;
		TInt err;
		err=iImSocket->GetIAPValue(convalue);
		if(err==KErrNone)
			test.Printf(_L("GetIAPValue returned %u\n"),convalue);
		else 
			test.Printf(_L("GetIAPValue error %d\n"),err);
		err=iImSocket->GetIAPBearer(convalue);
		if(err==KErrNone)
			test.Printf(_L("GetIAPBearer returned %u\n"),convalue);
		else 
			test.Printf(_L("GetIAPBearer error %d\n"),err);
		iImSocket->Send(iStatus,KImUser());
		testUtils->WriteComment(_L("\tSent Message USER abf75"));
		break;
	case 2:
		iImSocket->Send(iStatus,KImPass());
		testUtils->WriteComment(_L("\tSent Message PASS oodifama"));
		break;
	case 3:
/*		iImSocket->SetSecurity(EImskEncryptionTLS1);
		iImSocket->LogText(_L8("**************************************"));
		iImSocket->LogText(_L8("Testing the logging code"));
		iImSocket->LogText(_L8("You can add your own logging text to this log file"));
		iImSocket->LogText(_L8("**************************************"));
*/		User::RequestComplete(p,KErrNone);
		break;
	case 4:
		iImSocket->Send(iStatus,KImStat());
		testUtils->WriteComment(_L("\tSent Message STAT"));
		break;
	case 5:
		iImSocket->Send(iStatus,KImList());
		testUtils->WriteComment(_L("\tSent Message LIST"));
		break;
	case 6:
		iImSocket->Send(iStatus,KImQuit());
		testUtils->WriteComment(_L("\tSent Message QUIT"));
		break;
		}
	
	SetActive();
	}

void CSendMessage::QueueNextReceive()
	{
	iImSocket->QueueReceiveNextTextLine(iStatus);
	SetActive();
	}

//

LOCAL_C void Init()
	{
	scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install( scheduler );

	User::LeaveIfError(theFs.Connect());
	theFs.SetSessionPath(_L("C:\\"));
	}
	
LOCAL_C void Closedown()
	{
	theFs.Close( );
	delete scheduler;
	}

//

LOCAL_C void doMainL()
	{
	Init();
	theFs.SetSessionPath(_L("c:\\"));
	theFs.MkDir(_L("c:\\logs\\"));
	theFs.MkDir(_L("c:\\logs\\email\\"));
	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->WriteComment(_L("T_IMSK01 Testing Imail Text Socket"));
	testUtils->WriteComment(_L("Please make sure that pipex is the default IAP."));

//	log=CImLog::NewL(_L("c:\\logs\\email\\T_IMSK.log"),EAppend);
//	CleanupStack::PushL(log);

//	log->AppendComment(_L8("********** T_IMSK01 Testing Imail Text Socket **********"));
//	log->AppendComment(_L8("\tInit() Completed"));

	// Set up the timeout in the comms database as it is needed by imsk.
	TUint32 timeoutInMts = 5; // ( five minutes )
	
	

//	TBuf<64> logString;
//	logString.Format(_L(" IAP is %u"),IAP);
//	testUtils->WriteComment(logString);

	CSendMessage *aSendMessage = CSendMessage::NewLC();
	aSendMessage->RequestSendL();

 	CActiveScheduler::Start();

	testUtils->WriteComment(_L("**********       T_IMSK Tests Complete       **********"));
	testUtils->TestHarnessCompleted();

	CleanupStack::PopAndDestroy(aSendMessage);
	CleanupStack::PopAndDestroy(testUtils);
	Closedown();
	}


GLDEF_C TInt E32Main()
	{
	test.Title();
	test.Start(_L("Testing Imail Text Socket"));
	__UHEAP_MARK;

	theCleanup=CTrapCleanup::New();
	test (theCleanup!=NULL);
	TRAPD(ret,doMainL());		
	test (ret==KErrNone);
	delete theCleanup;

	__UHEAP_MARKEND;
	test.End();
	return 0;
	}
