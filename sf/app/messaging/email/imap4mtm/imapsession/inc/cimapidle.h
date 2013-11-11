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
//


#ifndef __CIMAPIDLE_H__
#define __CIMAPIDLE_H__

#include <e32std.h>

#include "cimapcommand.h"

/**
Provides implementation of the IDLE command, as specified in RFC 2177.
All untagged responses are handled by the base class.
@internalTechnology
@prototype
*/
class CImapIdle : public CImapCommandEx
	{
public:
	enum TIdleState
		{
		/**
		The IDLE command has been issued, and we are waiting for the server to 
		send the continiation response.
		Any untagged responses that are received before the continuation response
		will be parsed but a notification will not be sent until the continuation response
		has been received.
		*/
		EIdleWaitContinue,
		/**
		The continuation response has now been received, so every untagged response
		will cause a notification.
		*/
		EIdleWaitResponse,
		/**
		The DONE command has been issued, and we are waiting for the tagged OK respose.
		Any untagged responses that are received before the tagged OK response
		will be parsed, but no notifications will be sent, other than the one for the tagged OK response.
		*/
		EIdleWaitDone
		};

	// Construction and Destruction
	static CImapIdle* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	~CImapIdle();
	
	void SendDone(MOutputStream& aStream);
	TIdleState IdleState(); 
	
private:
	CImapIdle(CImapFolderInfo* aSelectedFolderData, TInt aLogId);
	void ConstructL();
	
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	void Cancel();
	
	void ParseContinuationResponseL();	
	
private:
	// No data for send
	// No data for receive (the only expected responses are unsolicited which are all handle by the base class)
	
	TIdleState iIdleState;	
	};
			 
#endif // __CIMAPIDLE_H__
