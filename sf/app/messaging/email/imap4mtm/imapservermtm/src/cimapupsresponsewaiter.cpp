// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <msventry.h>
#include <miut_err.h>
#include <imapcmds.h>

#include "cimapprotocolcontroller.h"
#include "cimapupsresponsewaiter.h"
#include "cimapsettings.h"

/**
Construction method
@return a new instance of CImapUpsResponseWaiter
*/
CImapUpsResponseWaiter* CImapUpsResponseWaiter::NewL(CMsvServerEntry& aServerEntry, CImapProtocolController& aImapProtocolController)
	{
	return new(ELeave)CImapUpsResponseWaiter(aServerEntry, aImapProtocolController);
	}

/**
Default constructor.
*/
CImapUpsResponseWaiter::CImapUpsResponseWaiter(CMsvServerEntry& aServerEntry, CImapProtocolController& aImapProtocolController)
	:CMsgActive(EPriorityStandard), iServerEntry(aServerEntry), iImapProtocolController(aImapProtocolController)
	{
    iImapSettings = CImapSettings::NewL(iServerEntry);
    CActiveScheduler::Add(this);
	}

/**
Destructor.
*/	
CImapUpsResponseWaiter::~CImapUpsResponseWaiter()
	{
	Cancel();
	delete iImapSettings;
	iUpsSubsession.Close();
	iUpsSession.Close(); 
	}

/**
Connect to UPS server and check if the client thread has the capability to check to the 
imap server.
@param aEntrySelection 	The CMsvEntrySelection containg the imap serviceID.
@param aCommand 		The Imap command(KIMAP4MTMConnect OR KIMAP4MTMConnectAndSynchronise).
		
@param aClientThread	The Rthread of the client application.
@param aHasCapability	TBool indicating if the client has passes the server's check on capability.
@param aStatus			TRequestStatus of the Active Object.
@return void.
*/
void CImapUpsResponseWaiter::AuthoriseAndConnectL(CMsvEntrySelection& aEntrySelection, TInt aCommand, TThreadId aClientThreadId, TBool aHasCapability, TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iState==EIMAP4MTMDisConnected, User::Invariant());
	
	if(aCommand == KIMAP4MTMConnect)
		{
		iState = EIMAP4MTMConnect;
		}
	else if(aCommand == KIMAP4MTMConnectAndSynchronise)	
		{
		iState = EIMAP4MTMConnectAndSynchronise;
		}
	
    iEntrySelection = &aEntrySelection;
	
	iDecision = EUpsDecNo;
	
	if( iImapSettings->SettingsLoaded() == EFalse)
	    {
	    iImapSettings->LoadSettingsL(iServerEntry.Entry().Id());
	    }

	// Connect to UPS service.....
	User::LeaveIfError(iUpsSession.Connect());

	RThread clientThread;
 	User::LeaveIfError(clientThread.Open(aClientThreadId));
	CleanupClosePushL(clientThread);

    User::LeaveIfError(iUpsSubsession.Initialise(iUpsSession, clientThread));
	CleanupStack::PopAndDestroy(&clientThread);
	
	iStatus=KRequestPending;
	iUpsSubsession.Authorise(aHasCapability, KUidIMAPService, iImapSettings->ServerAddress(), iDecision, iStatus);

	Queue(aStatus);
	SetActive();
    	
	}


void CImapUpsResponseWaiter::DoRunL()
	{
	User::LeaveIfError(iStatus.Int());
	if(iDecision != EUpsDecYes && iDecision != EUpsDecSessionYes)
		{
		Complete(KErrPermissionDenied);
		return;
		}
	switch (iState)
		{
		case EIMAP4MTMConnect:
			{
			iState = EIMAP4MTMConnected;
			iImapProtocolController.ConnectL(iStatus, *iEntrySelection);
			SetActive();
			}break;
		case EIMAP4MTMConnectAndSynchronise:
			{
			iState = EIMAP4MTMConnected;
			iImapProtocolController.ConnectAndSynchroniseL(iStatus, *iEntrySelection);
			SetActive();	
			}break;
		case EIMAP4MTMConnected:
			{
			Complete(iStatus.Int());
			iState=EIMAP4MTMDisConnected;
			}break;
		default:
			{
			__ASSERT_DEBUG(iState==EIMAP4MTMDisConnected, User::Invariant());
			break;
			}	
		}
	}


void CImapUpsResponseWaiter::DoCancel()
	{
	iUpsSubsession.CancelPrompt();
	iImapProtocolController.CancelAndCleanup();
	CMsgActive::DoCancel();
	}
