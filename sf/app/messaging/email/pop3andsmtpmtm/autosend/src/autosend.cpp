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

#include <e32base.h>
#include <e32std.h>
#include <msventry.h>
#include <msvapi.h>
#include <msvids.h>
#include <mtclreg.h>
#include <autosend.h>
#include <smtcmtm.h>
#include <bacline.h>

const TInt KProgressPollingRate = 10000000; // 10 seconds

//uncomment the following line if you do not want logging.
//#define NO_AUTOSEND_LOGGING

#ifdef AUTOSEND_LOGGING
_LIT(KAutoSendLogDir, "email");
_LIT(KAutoSendLogFile, "autosend.txt");
#endif

TInt Execute(const TMsvId aDestinationId)
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(leaveValue, DoExecuteL(aDestinationId));

	delete cleanup;
	__UHEAP_MARKEND;
	return leaveValue;
	}

#include <u32std.h>

void TDummyMsvSessionObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny */*aArg1*/, TAny */*aArg2*/, TAny */*aArg3*/)
	{
	}

void CheckProcessCreatorL(void)
	{
	RProcess me;
	TSecurityInfo inf(me);
	inf.SetToCreatorInfo();
	TDummyMsvSessionObserver dummyObserver;
	
	CMsvSession *sess = CMsvSession::OpenSyncL(dummyObserver);
	CleanupStack::PushL(sess);
	
	// inf.iCaps should be the TCapabilitySet of the creator process.
	TCapabilitySet mtmCapSet;
	sess->GetMtmRequiredCapabilitiesL(KUidMsgTypeSMTP, mtmCapSet);

	if(!inf.iCaps.HasCapabilities(mtmCapSet))
		{
		// work out the missing capabilities for the diagnostics
		mtmCapSet.Remove(inf.iCaps);
		User::LeaveIfError(PlatSec::CreatorCapabilityCheckFail(mtmCapSet,__PLATSEC_DIAGNOSTIC_STRING("Autorun.exe: Creator Caps do not match those of the SMTP Server MTM")));
		}
		
	CleanupStack::PopAndDestroy(sess);
	}

void DoExecuteL(const TMsvId aDestinationId)
	{
	CActiveScheduler* activeSch = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(activeSch);

	CActiveScheduler::Install(activeSch);

	CheckProcessCreatorL();

	CImAutoSend::StartLC(aDestinationId);

	CleanupStack::PopAndDestroy(2); //activeSch, CAutoSend
	}

/*
//
	CImAutoSend Implementation
//
*/

/*
	CImAutoSend Constructor
*/
CImAutoSend::CImAutoSend(const TMsvId aDestinationId)
	: CActive(KDefaultPriority), iDestinationId(aDestinationId)
	{
	//Add this to the Active Scheduler.
	//This is a requirement if a class is derived from CActive
	CActiveScheduler::Add(this);
	}

/*
	CImAutoSend Destructor
*/
CImAutoSend::~CImAutoSend()
	{
	Cancel();
	delete iTimer;
	delete iOperation;
	delete iSession;
	}

/*
	StartLC

	1. Constructs CImAutoSend
	2. Attempts to send the messages
*/
CImAutoSend* CImAutoSend::StartLC(const TMsvId aDestinationId)
	{
	CImAutoSend* self = new (ELeave) CImAutoSend(aDestinationId);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CImAutoSend::ConstructL()
	{
#ifdef AUTOSEND_LOGGING
	FLog(_L("Autosend using service %d"), iDestinationId);
#endif
	iSession = CMsvSession::OpenSyncL(*this);

	CallMtmL();
	iActiveSchedulerWait.Start();
	}

/*
	CallMtmL

	Attempts to send the messages in the Outbox.  The selection of messages is determined
	on the Server side dependant on which IAP the user is connected to.
*/
void CImAutoSend::CallMtmL()
	{
#ifdef AUTOSEND_LOGGING
	FLog(_L("\tCallMtmL"));
#endif

	TBuf8<1> buf;
	CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection();
	CleanupStack::PushL(selection);
	selection->AppendL(iDestinationId);
	iOperation = iSession->TransferCommandL(*selection, KSMTPMTMSendOnNextConnection, buf, iStatus);

	CleanupStack::PopAndDestroy(selection);

	delete iTimer;
	iTimer = NULL;

	iTimer = CAutoSendProgressTimer::NewL(*iOperation, KProgressPollingRate);
	iTimer->Start();
	iStatus=KRequestPending;
	SetActive();
	}

void CImAutoSend::DoCancel()
	{
#ifdef AUTOSEND_LOGGING
	FLog(_L("\tDoCancel"));
#endif
	iActiveSchedulerWait.AsyncStop();
	}

/*
	RunL

	Called by the Active Scheduler with iStatus is complete,
	i.e when the selection of messages have been sent to the appropriate Mtm.
*/
void CImAutoSend::RunL()
	{
#ifdef AUTOSEND_LOGGING
	FLog(_L("\tRunL iStatus=%d"), iStatus.Int());
#endif
	iActiveSchedulerWait.AsyncStop();
	}

#ifdef AUTOSEND_LOGGING
void CImAutoSend::FLog(TRefByValue<const TDesC> aFormat, ...)
	{
	VA_LIST list;
	VA_START(list, aFormat);
	RFileLogger::WriteFormat(KAutoSendLogDir, KAutoSendLogFile, EFileLoggingModeAppend,
		aFormat, list);
	}

void CImAutoSend::FLog(const TDesC& buf)
	{
	RFileLogger::Write(KAutoSendLogDir, KAutoSendLogFile, EFileLoggingModeAppend,buf);
	}
#endif


/*
//
	CAutoSendProgressTimer Implementation
//
*/

CAutoSendProgressTimer* CAutoSendProgressTimer::NewL(CMsvOperation& aOperation, const TTimeIntervalMicroSeconds32& aPollInterval)
	{
	CAutoSendProgressTimer* self = new (ELeave) CAutoSendProgressTimer(aOperation, aPollInterval);
	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop();

	return self;
	}

CAutoSendProgressTimer::CAutoSendProgressTimer(CMsvOperation& aOperation, const TTimeIntervalMicroSeconds32& aPollInterval)
: CTimer(EPriorityHigh), iOperation(aOperation), iPollInterval(aPollInterval)
	{
	CActiveScheduler::Add(this);
	}

CAutoSendProgressTimer::~CAutoSendProgressTimer()
	{
	Cancel();
	}

void CAutoSendProgressTimer::Start()
	{
	if (iPollInterval.Int() > 0)
		{
		After(iPollInterval);
		}
	}

void CAutoSendProgressTimer::RunL()
	{
	if (iPollInterval.Int() > 0)
		{
		// Ignore progress errors (we don't care)
		TRAPD(error, iOperation.ProgressL());

#ifdef AUTOSEND_LOGGING
		RFileLogger::WriteFormat(KAutoSendLogDir, KAutoSendLogFile, EFileLoggingModeAppend,
			_L("\tPollProgress error=%d"), error);
#endif

		After(iPollInterval);
		}
	}


/*
//
	The function that actually does the stuff
//
*/

GLDEF_C TInt E32Main()
	{
	TMsvId serviceId = KMsvNullIndexEntryId;

	TBuf<0x100> cmd;
		User::CommandLine(cmd);
	
	TLex lex(cmd);
	lex.Val(serviceId);

	if (serviceId)
		return Execute(serviceId);

	return KErrArgument;
	}

