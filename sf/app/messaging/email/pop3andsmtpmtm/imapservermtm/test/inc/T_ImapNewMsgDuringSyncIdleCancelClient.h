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

#ifndef __T_IMAPNEWMSGDURINGSYNCIDLECANCELCLIENT_H__
#define __T_IMAPNEWMSGDURINGSYNCIDLECANCELCLIENT_H__

#include <e32base.h>
#include <msvapi.h>
#include <imapconnectionobserver.h>
#include "cspoofserver.h"
#include <cemailaccounts.h>

class CClientMtmRegistry;

// For loading the serial comms device drivers 
#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif

class TDummySessionObserver : public CBase, public MMsvSessionObserver
	{
public:
	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	};

class CImapNewMsgDuringSyncIdleCancelClient : public CActive, public MMsvImapConnectionObserver
	{
public:
	IMPORT_C static CImapNewMsgDuringSyncIdleCancelClient* NewL(MImapTestEventHandler* aOwner);
	IMPORT_C static CImapNewMsgDuringSyncIdleCancelClient* NewLC(MImapTestEventHandler* aOwner);
	~CImapNewMsgDuringSyncIdleCancelClient();
	void StartL();
	void CreateImapAccountL();
	TImapAccount GetImapAccount();

private:
	CImapNewMsgDuringSyncIdleCancelClient(MImapTestEventHandler* aOwner);
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
		ESync1 = 0,
		ESync2,
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
	TIMAPClientState iNextStep;
	CMsvEntrySelection* iSelection;
	MImapTestEventHandler* iOwner;
	};

#endif  //__T_IMAPNEWMSGDURINGSYNCIDLECANCELCLIENT_H__
