// cimapupsresponsewaiter.h

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

#ifndef __CIMAPUPSRESPONSEWAITER_H__
#define __CIMAPUPSRESPONSEWAITER_H__

#include <e32base.h>
#include <mentact.h>
#include <ups/upsclient.h>

using namespace UserPromptService;

class CMsvServerEntry;
class CImapProtocolController;
class CMsgActive;
class CMsvEntrySelection;

/**
An class that is used for connect to the UPS server and check if the client is authorised
to connect to the remote server.
@internalComponent
@released
*/

NONSHARABLE_CLASS (CImapUpsResponseWaiter) : public CMsgActive
	{
public:
	static CImapUpsResponseWaiter* NewL(CMsvServerEntry& aServerEntry, CImapProtocolController& aImapProtocolController);
	~CImapUpsResponseWaiter();
	void AuthoriseAndConnectL(CMsvEntrySelection& aEntrySelection, TInt aCommand, TThreadId aClientThreadId, TBool aHasCapability, TRequestStatus& aStatus);
	
protected:
	CImapUpsResponseWaiter(CMsvServerEntry& aServerEntry, CImapProtocolController& aImapProtocolController);

	// from CMsgActive;
	void DoRunL();
	void DoCancel();
private:
	CImapSettings*              iImapSettings;
	CMsvServerEntry& 			iServerEntry;
	CImapProtocolController& 	iImapProtocolController;
	CMsvEntrySelection* 		iEntrySelection;
	/** For interaction with UPS server */
	RUpsSession 				iUpsSession;
	RUpsSubsession 				iUpsSubsession;
	TUpsDecision 				iDecision;
	enum TImapConnectState
		{
		EIMAP4MTMDisConnected,
		EIMAP4MTMConnect, 
		EIMAP4MTMConnectAndSynchronise, 
		EIMAP4MTMConnected
		}iState;
	};

#endif// __CIMAPUPSRESPONSEWAITER_H__
