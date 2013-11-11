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

#ifndef __T_DEF062024_IMAPCLIENT_H__
#define __T_DEF062024_IMAPCLIENT_H__

#include <e32cons.h>
#include <e32const.h>
#include <in_sock.h>
#include <nifman.h>
#include "emailtestutils.h"
#include <iapprefs.h>
#include <cemailaccounts.h>
#include "impsmtm.h"
#include <imapconnectionobserver.h>
#include "cspoofserver.h"

// For loading the serial comms device drivers 
#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif

class TDummySessionObserver : public CBase , public MMsvSessionObserver
	{
public:
	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	};

class CActiveImapClient : public CActive , public MMsvImapConnectionObserver
	{

public:
	
	IMPORT_C static CActiveImapClient* NewL(MImapTestEventHandler& aObserver);
	IMPORT_C static CActiveImapClient* NewLC(MImapTestEventHandler& aObserver);
	void StartL();
	void CreateImapAccountL();
	~CActiveImapClient();
	TImapAccount GetAccount();
	// Return iMessageId, the message ID of the email being fetched with its attachment
	TMsvId GetMessageId();

private:
	CActiveImapClient(MImapTestEventHandler& aObserver);
	void ConstructL();
	
	// From CActive
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	
	//implementation of pure virtual inherited from MMsvImapConnectionObserver 
	virtual void HandleImapConnectionEvent(TImapConnectionEvent aConnectionState);
private:

	enum TIMAPClientState 
		{			
		EFetch=0,
		EDisconnect, 
		EComplete
		};
		
	CEmailAccounts* iAccounts;
	TImapAccount iImapAccount;
	CMsvSession* iSession;
	CBaseMtm* iClientMtm;
	CClientMtmRegistry*	iClientRegistry;
	TDummySessionObserver*	iSessionObserver;
	CMsvOperation* iMsvOperation;
	TIMAPClientState iState;
	CMsvEntrySelection* iSelection;
	MImapTestEventHandler& iObserver;
	TMsvId iMessageId;
	};

#endif  //__T_DEF062024_IMAPCLIENT_H__
