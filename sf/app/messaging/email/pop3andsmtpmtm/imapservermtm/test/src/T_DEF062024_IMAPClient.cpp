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
// IMAPClient.cpp
// 
//

#include "t_def062024_imapclient.h"

_LIT(KLoopbackAddress,"127.0.0.1");

void TDummySessionObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	}
 
TImapAccount CActiveImapClient::GetAccount()
	{
	return iImapAccount;
	}

CActiveImapClient::CActiveImapClient(MImapTestEventHandler& aObserver)
 : CActive(EPriorityStandard), iObserver(aObserver)
	{
	CActiveScheduler::Add(this); 	
	}

EXPORT_C CActiveImapClient* CActiveImapClient::NewL(MImapTestEventHandler& aObserver)
	{
	CActiveImapClient* self = NewLC(aObserver);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CActiveImapClient* CActiveImapClient::NewLC(MImapTestEventHandler& aObserver)
	{
	CActiveImapClient* self = new(ELeave) CActiveImapClient(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CActiveImapClient::~CActiveImapClient()
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
	
	
void CActiveImapClient::ConstructL()
	{	
	//create an account
	iAccounts = CEmailAccounts::NewL();	
	CreateImapAccountL();
	// Session observer. Needed to create a session
	iSessionObserver = new (ELeave) TDummySessionObserver;
	// Session. Needed to create a client registry.
	iSession=CMsvSession::OpenSyncL(*iSessionObserver);
	// Client registry. Needed to get the MTM component
	iClientRegistry=CClientMtmRegistry::NewL(*iSession,KMsvDefaultTimeoutMicroSeconds32);
	//get the client mtm
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
	
void CActiveImapClient::CreateImapAccountL()
	{
	//create objects and initialise with the defaults
	CImImap4Settings* imap4Settings=new(ELeave)CImImap4Settings;
	CleanupStack::PushL(imap4Settings);
	CImSmtpSettings* smtpSettings=new(ELeave)CImSmtpSettings;
	CleanupStack::PushL(smtpSettings);
	CImIAPPreferences* imapIap = CImIAPPreferences::NewLC();
	CImIAPPreferences* smtpIap = CImIAPPreferences::NewLC();
	
	iAccounts->PopulateDefaultImapSettingsL(*imap4Settings, *imapIap);
	iAccounts->PopulateDefaultSmtpSettingsL(*smtpSettings, *smtpIap);
	
	//override some of the defaults
	imap4Settings->SetPasswordL(_L8("davids"));
	imap4Settings->SetLoginNameL(_L8("davids"));
	imap4Settings->SetServerAddressL(KLoopbackAddress);
	imap4Settings->SetFolderPathL(_L8(""));
	imap4Settings->SetSynchronise(EUseLocal);
	
	//create the account
	iImapAccount = iAccounts->CreateImapAccountL(_L("TestAccount"), *imap4Settings, *imapIap, EFalse);
	TSmtpAccount smtpAccount;
	smtpAccount = iAccounts->CreateSmtpAccountL(iImapAccount, *smtpSettings, *smtpIap, EFalse);

	//clean up
	CleanupStack::PopAndDestroy(4,imap4Settings);  
	}

// Return iMessageId, the message ID of the email being fetched with its attachment
TMsvId CActiveImapClient::GetMessageId()
	{
	return iMessageId;
	}

void CActiveImapClient::StartL()
	{	
	//we are to collect the whole email then the next operation will be to fetch it
	iState = EFetch;
	iSelection = new (ELeave) CMsvEntrySelection;
	TPckg<MMsvImapConnectionObserver*> param(this);
	//select the imap service entry
	iSelection->AppendL(iImapAccount.iImapService);
	//make the service entry the current context
	iClientMtm->SwitchCurrentEntryL(iImapAccount.iImapService);
	//sync the account
	iMsvOperation = iClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndSyncCompleteAfterFullSync,*iSelection,param,iStatus);			
	SetActive();
	}
	
void CActiveImapClient::DoCancel()
	{	
	__ASSERT_DEBUG(iMsvOperation!=NULL, User::Panic(_L("CActiveImapClient"), -3));
	iMsvOperation->Cancel();
	}

void CActiveImapClient::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	switch(iState)
		{
		//connect and sync	
		case EDisconnect:
			{
			iState = EComplete;
			TPckg<MMsvImapConnectionObserver*> param(this);
			delete iMsvOperation;
			iMsvOperation = NULL;
			iMsvOperation = iClientMtm->InvokeAsyncFunctionL(KIMAP4MTMDisconnect,*iSelection,param,iStatus);	
			SetActive();
			break;		
			}
		case EFetch:
			{
			iState = EDisconnect;
			TImImap4GetMailInfo imap4GetMailInfo;
			imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
			imap4GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
			imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;					
			TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);			
			CMsvEntry* inboxEntry = NULL;
			CMsvEntry* imapService = NULL;
			TMsvSelectionOrdering ordering;
			// message in remote inbox					
			//create a CMsvEntry for the service entry
			imapService = CMsvEntry::NewL(*iSession, iImapAccount.iImapService,ordering);
			CleanupStack::PushL(imapService);
			//retrieve the inbox from the imap service entry
			CMsvEntrySelection* msvEntrySelection;
			msvEntrySelection=imapService->ChildrenL();
			CleanupStack::PushL(msvEntrySelection);
			//create a CMsvEntry for the inbox entry
			inboxEntry = CMsvEntry::NewL(*iSession, (*msvEntrySelection)[0],ordering);
			CleanupStack::PopAndDestroy(msvEntrySelection);
			CleanupStack::PushL(inboxEntry);
			//retrieve the message from the inboxEntry service entry
			msvEntrySelection=inboxEntry->ChildrenL();
			CleanupStack::PushL(msvEntrySelection);
			//append the message entry to our selection
			iSelection->Reset();
			//We store the message Id of the email we want to check attachment
			iMessageId = (*msvEntrySelection)[0];
			iSelection->AppendL(iMessageId);
			//cleanup
			CleanupStack::PopAndDestroy(msvEntrySelection);
			CleanupStack::PopAndDestroy(inboxEntry);
			CleanupStack::PopAndDestroy(imapService);

			package = imap4GetMailInfo;
			delete iMsvOperation;
			iMsvOperation = NULL;
			iMsvOperation = iClientMtm->InvokeAsyncFunctionL(KIMAP4MTMPopulate, *iSelection, package, iStatus);	
			SetActive();
			break;
			}
		case EComplete:
			{//inform the owning object that the process is complete
			iObserver.TestComplete(KErrNone);
			break;
			}
		default:
			{
			__ASSERT_DEBUG(0, User::Panic(_L("CActiveImapClient unknown state"), KErrUnknown));
			break;			
			}
		}
	}
	
TInt CActiveImapClient::RunError(TInt aError)
	{
	iObserver.TestComplete(aError);
	return KErrNone;
	}
	
void CActiveImapClient::HandleImapConnectionEvent(TImapConnectionEvent /*aConnectionState*/)
	{
	//this method does nothing
	}



