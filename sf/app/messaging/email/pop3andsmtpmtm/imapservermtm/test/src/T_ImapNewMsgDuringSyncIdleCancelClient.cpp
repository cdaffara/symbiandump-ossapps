// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// T_ImapNewMsgDuringSyncIdleCancel.cpp
// The server waits for the idle read to be cancelled by the sync command, and then sends a '* 2 EXISTS' command to indicate that
// a new message has arrived.
// 2. The client connects to the server, requests a sync and a populate.
// 3. Client / server interact to complete the sync and populate. One message is fetched.
// 4. The client requests a full sync (causing the idle read cancel).
// 5. The server indicates a new message has arrived
// 6. Client / server interact to complete the full sync. The new message is fetched
// 7. The client requests a full sync (causing the idle read cancel).
// 8. The server indicates a new message has arrived
// 9. Client / server interact to complete the full sync. The new message is fetched
// 10. The client disconnects.
// 
//

/**
 @SYMTestCaseID DEF067159
 @SYMTestType UT
 @SYMTestPriority High
 @SYMDEF DEF067159
 @SYMTestCaseDesc IMAP spoof server test that checks that a new message indication is correctly handled if idle read is being cancelled during a sync
 @SYMTestActions 1. The Server is started and listens on port 143.
 @SYMTestExpectedResults Client should have fetched three messages
*/

#include "T_ImapNewMsgDuringSyncIdleCancelClient.h"

#include <mtclbase.h>
#include <mtclreg.h>
#include <miutset.h>
#include <imapset.h>
#include <smtpset.h>
#include <iapprefs.h>
#include <imapcmds.h>

_LIT(KLoopbackAddress,"127.0.0.1");

void TDummySessionObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	}
	
CImapNewMsgDuringSyncIdleCancelClient::CImapNewMsgDuringSyncIdleCancelClient(MImapTestEventHandler* aOwner)
 : CActive(EPriorityStandard), iOwner(aOwner)
	{
	CActiveScheduler::Add(this);
	}

EXPORT_C CImapNewMsgDuringSyncIdleCancelClient* CImapNewMsgDuringSyncIdleCancelClient::NewL(MImapTestEventHandler* aOwner)
	{
	CImapNewMsgDuringSyncIdleCancelClient* self = NewLC(aOwner);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CImapNewMsgDuringSyncIdleCancelClient* CImapNewMsgDuringSyncIdleCancelClient::NewLC(MImapTestEventHandler* aOwner)
	{
	CImapNewMsgDuringSyncIdleCancelClient* self = new(ELeave) CImapNewMsgDuringSyncIdleCancelClient(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CImapNewMsgDuringSyncIdleCancelClient::~CImapNewMsgDuringSyncIdleCancelClient()
	{
	Cancel();
	delete iMsvOperation;	
	delete iSelection;
	delete iClientMtm;
	delete iClientRegistry;	
	delete iSession;
	delete iSessionObserver;
	delete iAccounts;
	}
	
	
void CImapNewMsgDuringSyncIdleCancelClient::ConstructL()
	{
	// create an account
	iAccounts = CEmailAccounts::NewL();	
	CreateImapAccountL();
	// Session observer. Needed to create a session
	iSessionObserver = new (ELeave) TDummySessionObserver;
	// Session. Needed to create a client registry.
	iSession=CMsvSession::OpenSyncL(*iSessionObserver);
	// Client registry. Needed to get the MTM component
	iClientRegistry=CClientMtmRegistry::NewL(*iSession,KMsvDefaultTimeoutMicroSeconds32);
	// get the client mtm
	iClientMtm=iClientRegistry->NewMtmL(KUidMsgTypeIMAP4);	
	
	// Load the serial comms device drivers.  If this is not done,
	// connecting via NT-RAS returns KErrNotFound (-1).
	TInt driverErr;
	driverErr=User::LoadPhysicalDevice(PDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	driverErr=User::LoadLogicalDevice(LDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	}
	
TImapAccount CImapNewMsgDuringSyncIdleCancelClient::GetImapAccount()
	{
	return iImapAccount;
	}
	
void CImapNewMsgDuringSyncIdleCancelClient::CreateImapAccountL()
	{
	// create objects and initialise with the defaults
	CImImap4Settings* imap4Settings=new(ELeave)CImImap4Settings;
	CleanupStack::PushL(imap4Settings);
	CImSmtpSettings* smtpSettings=new(ELeave)CImSmtpSettings;
	CleanupStack::PushL(smtpSettings);
	CImIAPPreferences* imapIap = CImIAPPreferences::NewLC();
	CImIAPPreferences* smtpIap = CImIAPPreferences::NewLC();
	
	iAccounts->PopulateDefaultImapSettingsL(*imap4Settings, *imapIap);
	iAccounts->PopulateDefaultSmtpSettingsL(*smtpSettings, *smtpIap);
	
	// override some of the defaults
	imap4Settings->SetPasswordL(_L8("roberth"));
	imap4Settings->SetLoginNameL(_L8("roberth"));
	imap4Settings->SetServerAddressL(KLoopbackAddress);
	imap4Settings->SetFolderPathL(_L8(""));
	imap4Settings->SetSynchronise(EUseLocal);
	
	// create the account
	iImapAccount = iAccounts->CreateImapAccountL(_L("TestAccount"), *imap4Settings, *imapIap, EFalse);
	TSmtpAccount smtpAccount;
	smtpAccount = iAccounts->CreateSmtpAccountL(iImapAccount, *smtpSettings, *smtpIap, EFalse);

	// clean up
	CleanupStack::PopAndDestroy(smtpIap);
	CleanupStack::PopAndDestroy(imapIap);	
	CleanupStack::PopAndDestroy(smtpSettings);
	CleanupStack::PopAndDestroy(imap4Settings); 
	}

void CImapNewMsgDuringSyncIdleCancelClient::StartL()
	{
	
	// we are to collect the whole email: the next operation will be to fetch it
	iNextStep = ESync1;
	iSelection = new (ELeave) CMsvEntrySelection;
	TPckg<MMsvImapConnectionObserver*> param(this);
	// select the imap service entry
	iSelection->AppendL(iImapAccount.iImapService);
	// make the service entry the current context
	iClientMtm->SwitchCurrentEntryL(iImapAccount.iImapService);
	// sync the account
	iMsvOperation = iClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndSyncCompleteAfterFullSync,*iSelection,param,iStatus);
	SetActive();
	}
	
void CImapNewMsgDuringSyncIdleCancelClient::DoCancel()
	{	
	__ASSERT_DEBUG(iMsvOperation!=NULL, User::Panic(_L("CImapNewMsgDuringSyncIdleCancelClient"), -3));
	iMsvOperation->Cancel();
	}

void CImapNewMsgDuringSyncIdleCancelClient::RunL()
	{
	TPckg<MMsvImapConnectionObserver*> param(this);
	
	switch(iNextStep)
		{
		case ESync1:
			{
			// First step is to do a full sync. This will cause the cancel idle to be sent, at
			// which point the spoof server will respond to indicate a new message has been
			// received. Without the defect fix, this step will panic.
			iNextStep=ESync2;
			delete iMsvOperation;
			iMsvOperation=NULL;
			iMsvOperation = iClientMtm->InvokeAsyncFunctionL(KIMAP4MTMFullSync, *iSelection, param, iStatus);
			SetActive();
			break;
			}

		case ESync2:
			{
			// Try to do another full sync. As before, this will cause the cancel idle to be
			// sent, at which point the spoof server will respond to indicate a new message
			// has been received.
			// This step is used to make sure that we have left the session state machine in
			// the correct state after the first step so that further operations can be
			// performed. It also makes it easy to spot when the test fails as for a failure,
			// the panic in the first step will mean only 2 messages are downloaded, whereas
			// for the successful case, 3 messages will be downloaded.
			iNextStep=EDisconnect;
			delete iMsvOperation;
			iMsvOperation=NULL;
			iMsvOperation = iClientMtm->InvokeAsyncFunctionL(KIMAP4MTMFullSync, *iSelection, param, iStatus);
			SetActive();
			break;
			}

		case EDisconnect:
			{
			// We have finished, so do a disconnect
			iNextStep=EComplete;
			delete iMsvOperation;
			iMsvOperation=NULL;
			iMsvOperation = iClientMtm->InvokeAsyncFunctionL(KIMAP4MTMDisconnect,*iSelection,param,iStatus);
			SetActive();
			break;
			}

		case EComplete:
			{
			// inform the owning object that the process is complete
			iOwner->TestComplete(KErrNone);
			break;
			}

		default:
			{
			__ASSERT_DEBUG(0, User::Panic(_L("CImapNewMsgDuringSyncIdleCancelClient unknown state"), KErrUnknown));
			break;			
			}
		}
	}
	
TInt CImapNewMsgDuringSyncIdleCancelClient::RunError(TInt aError)
	{
	iOwner->TestComplete(aError);
	return KErrNone;
	}
	
void CImapNewMsgDuringSyncIdleCancelClient::HandleImapConnectionEvent(TImapConnectionEvent /*aConnectionState*/)
	{
	// this method does nothing
	}
