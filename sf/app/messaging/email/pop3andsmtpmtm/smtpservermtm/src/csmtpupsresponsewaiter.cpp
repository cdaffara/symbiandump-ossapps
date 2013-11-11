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

#include "IMSMSEND.H"
#include "csmtpupsresponsewaiter.h"

#include <miut_err.h>
#include <msvids.h>
#include <smtpset.h>

/**
Construction method
@return a new instance of CSmtpUpsResponseWaiter
*/
CSmtpUpsResponseWaiter* CSmtpUpsResponseWaiter::NewL()
	{
	return new(ELeave)CSmtpUpsResponseWaiter();
	}

/**
Default constructor.
*/
CSmtpUpsResponseWaiter::CSmtpUpsResponseWaiter()
	: CMsgActive(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

/**
Destructor.
*/	
CSmtpUpsResponseWaiter::~CSmtpUpsResponseWaiter()
	{ 
	Cancel();
	// Close UPS session
	iUpsSubsession.Close();
	iUpsSession.Close();
	}

/**
Connect to UPS server and check if the client thread has the capability to check to the 
smtp server.
@param iSettings 		The CImSmtpSettings object.
@param aHasCapability	TBool indicating if the client has passes the server's check on capability.		
@param aClientThread	The Rthread of the client application.
@param aStatus			TRequestStatus of the Active Object.
@return void.
*/
void CSmtpUpsResponseWaiter::AuthoriseAndConnectL(const CImSmtpSettings& aSettings, TBool aHasCapability, TThreadId aClientThreadId, TRequestStatus& aStatus)
	{
	iDecision = EUpsDecNo;
	// Connect to UPS service.....
	User::LeaveIfError(iUpsSession.Connect());
 	RThread clientThread;
 	User::LeaveIfError(clientThread.Open(aClientThreadId));
	CleanupClosePushL(clientThread);
 	User::LeaveIfError(iUpsSubsession.Initialise(iUpsSession, clientThread));
	CleanupStack::PopAndDestroy(&clientThread);
	
	Queue(aStatus);
	
	iUpsSubsession.Authorise(aHasCapability, KUidSMTPService, aSettings.ServerAddress(), iDecision, iStatus);
	SetActive();
	}

void CSmtpUpsResponseWaiter::DoRunL()
	{
	User::LeaveIfError(iStatus.Int());
	if(iDecision != EUpsDecYes && iDecision != EUpsDecSessionYes)
		{
		User::Leave(KErrPermissionDenied);
		}
	Complete(iStatus.Int());	
	}

void CSmtpUpsResponseWaiter::DoCancel()
	{
	iUpsSubsession.CancelPrompt();
	
	CMsgActive::DoCancel();
	}
