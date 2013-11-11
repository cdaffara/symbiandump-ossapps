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

#ifndef __CIMAPSERVERGREETING_H__
#define __CIMAPSERVERGREETING_H__

#include <e32std.h>

#include "cimapcommand.h"

// Forward Declaration
class CImapServerGreetingInfo;

/**
The CImapServerGreeting class encapsulates the receipt of the inital server greeting that
is sent from the remote server when a client connection is made.

Data received in the server greeting is stored in the CImapServerGreetingInfo object that
is passed into this class on construction.

This class does not send any messages.  It's SendMessageL() implementation is empty.

@internalTechnology
@prototype
*/
class CImapServerGreeting : public CImapCommandEx
	{
public:
	// Construction and Destruction
	static CImapServerGreeting* NewL(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImapServerGreetingInfo& aServerGreetingInfo);
	~CImapServerGreeting();
	
private:
	CImapServerGreeting(CImapFolderInfo* aSelectedFolderData, TInt aLogId, CImapServerGreetingInfo& aServerGreetingInfo);
	void ConstructL();
	
	// Implements CImapCommand
	void SendMessageL(TInt aTagId, MOutputStream& aStream);
	TParseBlockResult ParseUntaggedResponseL();
	
private:
	CImapServerGreetingInfo& iServerGreetingInfo;
	};
			 
#endif // __CIMAPSERVERGREETING_H__
