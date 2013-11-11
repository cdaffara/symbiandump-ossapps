// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <msvids.h>
#include <pop3set.h>

#include "cpopupsresponsewaiter.h"
#include "POPS.H"
// panics
#include "POPS.PAN"

/**
Construction method
@return a new instance of CPopUpsResponseWaiter
*/
CPopUpsResponseWaiter* CPopUpsResponseWaiter::NewL()
	{
	return new(ELeave)CPopUpsResponseWaiter();
	}

/**
Default constructor.
*/
CPopUpsResponseWaiter::CPopUpsResponseWaiter()
	: CMsgActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

/**
Destructor.
*/	
CPopUpsResponseWaiter::~CPopUpsResponseWaiter()
	{
	Cancel();
	iUpsSubsession.Close();
	iUpsSession.Close();
	}

/**
Connect to UPS server and check if the client thread has the capability to check to the 
pop server.
@param aPopSettings 		The CImPop3Settings object.
@param aClientThread		The Rthread of the client application.
@param aHasCapability		TBool indicating if the client has passes the server's check on capability.		
@param aStatus				TRequestStatus of the Active Object.
@return void.
*/
void CPopUpsResponseWaiter::AuthoriseAndConnectL(CImPop3Settings* aPopSettings, TThreadId aClientThreadId, TBool aHasCapability, TRequestStatus& aStatus)
	{ 
	iPopSettings = aPopSettings;
	
	iDecision = EUpsDecNo;	
	User::LeaveIfError(iUpsSession.Connect());
	
	RThread clientThread;
 	User::LeaveIfError(clientThread.Open(aClientThreadId));
	CleanupClosePushL(clientThread);
 	User::LeaveIfError(iUpsSubsession.Initialise(iUpsSession, clientThread));
	CleanupStack::PopAndDestroy(&clientThread);
	
	Queue(aStatus);	
	
	iUpsSubsession.Authorise(aHasCapability, KUidPOPService, iPopSettings->ServerAddress(), iDecision, iStatus);
	SetActive();
	}

void CPopUpsResponseWaiter::DoRunL()
	{
	User::LeaveIfError(iStatus.Int());
	if(iDecision != EUpsDecYes && iDecision != EUpsDecSessionYes)
		{
		User::Leave(KErrPermissionDenied);
		}
	Complete(iStatus.Int());
	}

void CPopUpsResponseWaiter::DoCancel()
	{
	// Cancel the Prompt request
	iUpsSubsession.CancelPrompt();

	CMsgActive::DoCancel();
	}
