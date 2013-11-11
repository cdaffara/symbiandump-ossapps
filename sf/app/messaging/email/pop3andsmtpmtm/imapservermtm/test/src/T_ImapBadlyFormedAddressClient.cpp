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
// T_ImapBadlyFormedAddress.cpp
// 
//

#include "T_ImapBadlyFormedAddressClient.h"

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
	
CImapBadlyFormedAddressClient::CImapBadlyFormedAddressClient(MImapTestEventHandler* aOwner)
 : CActive(EPriorityStandard), iOwner(aOwner)
	{
	CActiveScheduler::Add(this);
	}

EXPORT_C CImapBadlyFormedAddressClient* CImapBadlyFormedAddressClient::NewL(MImapTestEventHandler* aOwner)
	{
	CImapBadlyFormedAddressClient* self = NewLC(aOwner);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CImapBadlyFormedAddressClient* CImapBadlyFormedAddressClient::NewLC(MImapTestEventHandler* aOwner)
	{
	CImapBadlyFormedAddressClient* self = new(ELeave) CImapBadlyFormedAddressClient(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CImapBadlyFormedAddressClient::~CImapBadlyFormedAddressClient()
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
	
	
void CImapBadlyFormedAddressClient::ConstructL()
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
	
TImapAccount CImapBadlyFormedAddressClient::GetImapAccount()
	{
	return iImapAccount;
	}
	
void CImapBadlyFormedAddressClient::CreateImapAccountL()
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

void CImapBadlyFormedAddressClient::StartL()
	{
	// we are to collect the whole email: the next operation will be to fetch it
	iNextStep = EDisconnect;
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

void CImapBadlyFormedAddressClient::DoCancel()
	{	
	__ASSERT_DEBUG(iMsvOperation!=NULL, User::Panic(_L("CImapBadlyFormedAddressClient"), -3));
	iMsvOperation->Cancel();
	}

void CImapBadlyFormedAddressClient::RunL()
	{
	TPckg<MMsvImapConnectionObserver*> param(this);
	
	switch(iNextStep)
		{
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
			__ASSERT_DEBUG(0, User::Panic(_L("CImapBadlyFormedAddressClient unknown state"), KErrUnknown));
			break;			
			}
		}
	}
	
TInt CImapBadlyFormedAddressClient::RunError(TInt aError)
	{
	iOwner->TestComplete(aError);
	return KErrNone;
	}
	
void CImapBadlyFormedAddressClient::HandleImapConnectionEvent(TImapConnectionEvent /*aConnectionState*/)
	{
	// this method does nothing
	}
