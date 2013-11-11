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
// imapserver.h
// 
//

#ifndef __IMAPSPOOFSERVER_H__
#define __IMAPSPOOFSERVER_H__

#include <e32cons.h>
#include <e32const.h>
#include <in_sock.h>
#include <nifman.h>
#include "scriptfileprocessor.h"
#include "eventhandler.h"

// Definitions
const TInt KMaxBufferSize = 2048;
const TInt KListeningSocketQueueSize = 1;

//
// CImapSpoofServer
//

class CImapSpoofServer : public CActive 
	{

public:
	
	IMPORT_C static CImapSpoofServer* NewL(MImapTestEventHandler* aOwner,const TDesC& aScriptFile);
	IMPORT_C static CImapSpoofServer* NewLC(MImapTestEventHandler* aOwner,const TDesC& aScriptFile);
	void StartL();
	~CImapSpoofServer();
private:
	CImapSpoofServer();
	void ConstructL(MImapTestEventHandler* aOwner,const TDesC& aScriptFile);
	void IssueWrite();
	void IssueRead();
	//Implemented functions from CActive
	virtual void DoCancel();
	virtual void RunL();
	virtual IMPORT_C TInt RunError(TInt aError);

private:

	enum TIMAPServerState 
		{
		EIdle, EWriting, EReading
		};
		
	TIMAPServerState iServerStatus;
	RSocket iListeningSocket;
	RSocket iServiceSocket;
	RSocketServ iSocketServer;
	TBuf8<KMaxBufferSize> iBuffer; 
	TBuf8<KMaxBufferSize> iNextLine;
	TSockXfrLength iReadLength;
	CScriptFileProcessor* iScriptFileProcessor;
	MImapTestEventHandler* iOwner;
	};

#endif  //__IMAPSPOOFSERVER_H__

