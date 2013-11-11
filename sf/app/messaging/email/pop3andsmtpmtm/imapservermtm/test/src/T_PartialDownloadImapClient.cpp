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
//

#include "t_partialdownloadimapclient.h"

_LIT(KLoopbackAddress,"127.0.0.1");

void TDummySessionObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	}

CPartialDownLoadImapClient::CPartialDownLoadImapClient(MImapTestEventHandler& aObserver,TBool aFetchWholeMessage) 
: CActive(EPriorityStandard), iObserver(aObserver), iFetchWholeMessage(aFetchWholeMessage) 
	{
	CActiveScheduler::Add(this);
	}

CPartialDownLoadImapClient* CPartialDownLoadImapClient::NewL(MImapTestEventHandler& aObserver,TBool aFetchWholeMessage)
	{
	CPartialDownLoadImapClient* self = NewLC(aObserver,aFetchWholeMessage);
	CleanupStack::Pop(self);
	return self;
	}

CPartialDownLoadImapClient* CPartialDownLoadImapClient::NewLC(MImapTestEventHandler& aObserver,TBool aFetchWholeMessage)
	{
	CPartialDownLoadImapClient* self = new(ELeave) CPartialDownLoadImapClient(aObserver,aFetchWholeMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CPartialDownLoadImapClient::~CPartialDownLoadImapClient()
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
	
TImapAccount CPartialDownLoadImapClient::GetImapAccount()
	{
	return iImapAccount;	
	}
	
void CPartialDownLoadImapClient::ConstructL()
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
		{
		User::Leave(driverErr);	
		}
	driverErr=User::LoadLogicalDevice(LDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		{
		User::Leave(driverErr);	
		}
	}
	
void CPartialDownLoadImapClient::CreateImapAccountL()
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
	
	imap4Settings->SetPartialMailOptionsL(ECumulative);
	imap4Settings->SetAttachmentSizeLimitL(2);
	
	//create the account
	iImapAccount = iAccounts->CreateImapAccountL(_L("TestAccount"), *imap4Settings, *imapIap, EFalse);
	TSmtpAccount smtpAccount;
	smtpAccount = iAccounts->CreateSmtpAccountL(iImapAccount, *smtpSettings, *smtpIap, EFalse);

	//clean up
	CleanupStack::PopAndDestroy(4,imap4Settings); 
	}

void CPartialDownLoadImapClient::StartL()
	{
	//if we are to collect the whole email then the next operation will be to fetch it
	iNextStep=(!iFetchWholeMessage? EDisconnect:EFetch);
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
	
void CPartialDownLoadImapClient::DoCancel()
	{	
	__ASSERT_DEBUG(iMsvOperation!=NULL, User::Invariant());
	iMsvOperation->Cancel();
	}

void CPartialDownLoadImapClient::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	TPckg<MMsvImapConnectionObserver*> param(this);
	TImImap4GetPartialMailInfo imapGetPartialMailInfo;
	imapGetPartialMailInfo.iPartialMailOptions=ECumulative;
	imapGetPartialMailInfo.iBodyTextSizeLimit = 2;	
	imapGetPartialMailInfo.iAttachmentSizeLimit = 2;
	TPckgBuf<TImImap4GetPartialMailInfo> package(imapGetPartialMailInfo);
	
	CMsvEntry* inboxEntry=NULL;
	CMsvEntry* imapService=NULL;
	TMsvSelectionOrdering ordering;
	
	switch(iNextStep)
		{
		//connect and sync	
		case EDisconnect:
			delete iMsvOperation;
			iMsvOperation=NULL;
			iMsvOperation = iClientMtm->InvokeAsyncFunctionL(KIMAP4MTMDisconnect,*iSelection,param,iStatus);	
			SetActive();
			iNextStep=EComplete;
			break;	
			
		case EFetch:
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
			iSelection=inboxEntry->ChildrenL();

			CleanupStack::PopAndDestroy(inboxEntry);
			CleanupStack::PopAndDestroy(imapService);
			delete iMsvOperation;
			iMsvOperation=NULL;
			iMsvOperation = iClientMtm->InvokeAsyncFunctionL(KIMAP4MTMPopulate, *iSelection, package, iStatus);	
			SetActive();
			iNextStep=EDisconnect;
			break;
		
		case EComplete:
			//inform the owning object that the process is complete
			iObserver.TestComplete(KErrNone);
			break;
			
		default:
			__ASSERT_DEBUG(0, User::Panic(_L("CPartialDownLoadImapClient unknown state"), KErrUnknown));
			break;			
		}
	}
	
TInt CPartialDownLoadImapClient::RunError(TInt aError)
	{
	iObserver.TestComplete(aError);
	return KErrNone;
	}
	
void CPartialDownLoadImapClient::HandleImapConnectionEvent(TImapConnectionEvent /*aConnectionState*/)
	{
	//this method does nothing
	}



